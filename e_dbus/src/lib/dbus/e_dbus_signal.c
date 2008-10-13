#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Ecore.h>
#include <Ecore_Data.h>
#include "E_DBus.h"
#include "e_dbus_private.h"
#include "dbus/dbus.h"

static int init = 0;

struct E_DBus_Signal_Handler
{
  char *sender;
  char *path;
  char *interface;
  char *member;

  E_DBus_Signal_Cb cb_signal;
  void *data;
  unsigned char delete_me : 1;
};

static void cb_signal_dispatcher(E_DBus_Connection *conn, DBusMessage *msg);

/**
 * Free a signal handler
 * @param sh the signal handler to free
 */
static void
e_dbus_signal_handler_free(E_DBus_Signal_Handler *sh)
{
  free(sh->sender);
  free(sh);
}

struct cb_name_owner_data
{
   E_DBus_Connection *conn;
   E_DBus_Signal_Handler *sh;
};

static void
cb_name_owner(void *data, DBusMessage *msg, DBusError *err)
{
  const char *unique_name = NULL;
  struct cb_name_owner_data *d = data;
  E_DBus_Connection *conn;
  E_DBus_Signal_Handler *sh;

  conn = d->conn;
  sh = d->sh;
  free(d);

  if (dbus_error_is_set(err))
    goto error;

  dbus_message_get_args(msg, err, DBUS_TYPE_STRING, &unique_name, DBUS_TYPE_INVALID);

  if (dbus_error_is_set(err))
    goto error;

  if (!unique_name)
    goto error;

  free(sh->sender);
  sh->sender = strdup(unique_name);

  return;

 error:
  if (err)
    DEBUG(1, "ERROR: %s %s\n", err->name, err->message);

  if (ecore_list_goto(conn->signal_handlers, sh))
    ecore_list_remove(conn->signal_handlers);
  e_dbus_signal_handler_free(sh);
  dbus_error_free(err);
}

static int
_match_append(char *buf, int size, int *used, const char *keyword, int keyword_size, const char *value, int value_size)
{
   if (*used + keyword_size + value_size + sizeof(",=''") >= size)
     {
	DEBUG(1, "ERROR: cannot add match %s='%s': too long!\n", keyword, value);
	return 0;
     }

   buf += *used;

   *buf = ',';
   buf++;

   memcpy(buf, keyword, keyword_size);
   buf += keyword_size;

   *buf = '=';
   buf++;

   *buf = '\'';
   buf++;

   memcpy(buf, value, value_size);
   buf += value_size;

   *buf = '\'';
   buf++;

   *buf = '\0';

   *used += keyword_size + value_size + sizeof(",=''") - 1;

   return 1;
}

/**
 * Add a signal handler
 *
 * @param conn the dbus connection
 * @param bus name of the signal's sender
 * @param path the object path of the signal's sender
 * @param interface the signal's interface
 * @param memeber the signal's name
 * @param cb_signal a callback to call when the signal is received
 * @param data custom data to pass in to the callback
 */
EAPI E_DBus_Signal_Handler *
e_dbus_signal_handler_add(E_DBus_Connection *conn, const char *sender, const char *path, const char *interface, const char *member, E_DBus_Signal_Cb cb_signal, void *data)
{
  E_DBus_Signal_Handler *sh;
  char match[DBUS_MAXIMUM_MATCH_RULE_LENGTH];
  int len, sender_len, path_len, interface_len, member_len;
  DBusError err;

  strcpy(match, "type='signal'");
  len = sizeof("type='signal'") - 1;

#define ADD_MATCH_PIECE(PIECE)						\
  do {									\
     PIECE ## _len = PIECE ? strlen(PIECE) : 0;				\
     if (!_match_append(match, sizeof(match), &len, #PIECE, sizeof(#PIECE) - 1, PIECE, PIECE ## _len)) \
       return NULL;							\
  } while (0)

  ADD_MATCH_PIECE(sender);
  ADD_MATCH_PIECE(path);
  ADD_MATCH_PIECE(interface);
  ADD_MATCH_PIECE(member);
#undef ADD_MATCH_PIECE

  len = path_len + interface_len + member_len + 4;
  sh = malloc(sizeof(*sh) + len);
  if (!sh)
    {
       DEBUG(1, "ERROR: could not allocate signal handler.\n");
       return NULL;
    }

  len = sizeof(*sh);

#define SET_STRING(PIECE)				\
  do {							\
     sh->PIECE = (char *)sh + len;			\
     if (PIECE)						\
       memcpy(sh->PIECE, PIECE, PIECE ## _len + 1);	\
     else						\
       sh->PIECE = NULL;				\
     len += PIECE ## _len + 1;				\
  } while (0)

  SET_STRING(path);
  SET_STRING(interface);
  SET_STRING(member);
#undef SET_STRING

  sh->sender = strdup(sender);

  sh->cb_signal = cb_signal;
  sh->data = data;
  sh->delete_me = 0;

  dbus_error_init(&err);
  dbus_bus_add_match(conn->conn, match, NULL);

  if (!conn->signal_handlers)
    {
       conn->signal_handlers = ecore_list_new();
       ecore_list_free_cb_set
	 (conn->signal_handlers, ECORE_FREE_CB(e_dbus_signal_handler_free));
       conn->signal_dispatcher = cb_signal_dispatcher;
    }

  /* if we have a sender, and it is not a unique name, we need to know the unique name to match since signals will have the name owner as ther sender. */
  if (sender && sender[0] != ':')
    {
       struct cb_name_owner_data *data;
       data = malloc(sizeof(*data));
       if (!data)
	 {
	    e_dbus_signal_handler_free(sh);
	    return NULL;
	 }
       data->conn = conn;
       data->sh = sh;
       e_dbus_get_name_owner(conn, sender, cb_name_owner, data);
    }

  ecore_list_append(conn->signal_handlers, sh);
  return sh;
}

static int e_dbus_handler_deletions = 0;

/**
 * Delete a signal handler
 *
 * @param conn the dbus connection
 * @param sh the handler to delete
 */
EAPI void
e_dbus_signal_handler_del(E_DBus_Connection *conn, E_DBus_Signal_Handler *sh)
{
  char match[DBUS_MAXIMUM_MATCH_RULE_LENGTH];
  int len, sender_len, path_len, interface_len, member_len;

  sh->delete_me = 1;
  if (e_dbus_idler_active)
  {
    e_dbus_handler_deletions = 1;
    return;
  }

  strcpy(match, "type='signal'");
  len = sizeof("type='signal'") - 1;

#define ADD_MATCH_PIECE(PIECE)						\
  do {									\
     PIECE ## _len = sh->PIECE ? strlen(sh->PIECE) : 0;			\
     if (!_match_append(match, sizeof(match), &len, #PIECE, sizeof(#PIECE) - 1, sh->PIECE, PIECE ## _len)) \
       return;								\
  } while (0)

  ADD_MATCH_PIECE(sender);
  ADD_MATCH_PIECE(path);
  ADD_MATCH_PIECE(interface);
  ADD_MATCH_PIECE(member);
#undef ADD_MATCH_PIECE

  dbus_bus_remove_match(conn->conn, match, NULL);

  if (!conn->signal_handlers) return;
  if (!ecore_list_goto(conn->signal_handlers, sh)) return;
  ecore_list_remove(conn->signal_handlers);
  e_dbus_signal_handler_free(sh);
}

static void
cb_signal_dispatcher(E_DBus_Connection *conn, DBusMessage *msg)
{
  E_DBus_Signal_Handler *sh;

  ecore_list_first_goto(conn->signal_handlers);
  while ((sh = ecore_list_next(conn->signal_handlers)))
  {
    if ((!sh->cb_signal) || (sh->delete_me)) continue;

    if (sh->sender && !dbus_message_has_sender(msg, sh->sender)) continue;
    if (sh->path && !dbus_message_has_path(msg, sh->path)) continue;
    if (sh->interface && !dbus_message_has_interface(msg, sh->interface)) continue;
    if (sh->member && !dbus_message_has_member(msg, sh->member)) continue;

    sh->cb_signal(sh->data, msg);
  }
}

void
e_dbus_signal_handlers_clean(E_DBus_Connection *conn)
{
  E_DBus_Signal_Handler *sh;

  if (!e_dbus_handler_deletions) return;
  if (!conn->signal_handlers) return;
  ecore_list_first_goto(conn->signal_handlers);
  while ((sh = ecore_list_next(conn->signal_handlers)))
  {
    if (sh->delete_me)
      e_dbus_signal_handler_del(conn, sh);
  }
  e_dbus_handler_deletions = 0;
}
