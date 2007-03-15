#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Ecore.h>
#include <Ecore_Data.h>
#include "E_DBus.h"
#include "dbus/dbus.h"

static Ecore_List *signal_handlers = NULL;

static Ecore_Event_Handler *event_handler = NULL;
static int init = 0;


struct E_DBus_Signal_Handler
{
  char *sender;
  char *path;
  char *interface;
  char *member;

  E_DBus_Signal_Cb cb_signal;
  void *data;
};

static int cb_signal_event(void *data, int type, void *event);

void e_dbus_signal_handler_free(E_DBus_Signal_Handler *sh);


/**
 * Initialize the signal subsystem
 * @internal
 */
int
e_dbus_signal_init(void)
{
  if (++init != 1) return init;
  signal_handlers = ecore_list_new();
  if (!signal_handlers) {--init; return 0;};
  ecore_list_set_free_cb(signal_handlers, ECORE_FREE_CB(e_dbus_signal_handler_free));

  event_handler = ecore_event_handler_add(E_DBUS_EVENT_SIGNAL, cb_signal_event, NULL);
  return init;
}

/**
 * Shutdown the signal subsystem
 * @internal
 */
void
e_dbus_signal_shutdown(void)
{
  printf("SHUTDOWN\n");
  if (--init) return;
  ecore_list_destroy(signal_handlers);

  if (event_handler) ecore_event_handler_del(event_handler);
  event_handler = NULL;
}

/**
 * Free a signal handler
 * @param sh the signal handler to free
 */
void
e_dbus_signal_handler_free(E_DBus_Signal_Handler *sh)
{
  printf("free: %p\n", sh);
  if (sh->sender) free(sh->sender);
  if (sh->path) free(sh->path);
  if (sh->interface) free(sh->interface);
  if (sh->member) free(sh->member);

  free(sh);
}

static void
cb_name_owner(void *data, DBusMessage *msg)
{
  DBusError err;
  const char *unique_name = NULL;
  E_DBus_Signal_Handler *sh;

  sh = data;

  dbus_error_init(&err);
  dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &unique_name, DBUS_TYPE_INVALID);

  if (dbus_error_is_set(&err))
  {
    DEBUG(1, "Invalid signature in reply to name owner call\n");
    dbus_error_free(&err);
    return;
  }

  if (unique_name)
  {
    if (sh->sender) free(sh->sender);
    sh->sender = strdup(unique_name);
  }
  else DEBUG(1, "Error, no unique name?\n");

}

static void
cb_name_owner_error(void *data, const char *error_name, const char *error_msg)
{
  E_DBus_Signal_Handler *sh;
  sh = ecore_list_goto(signal_handlers, data);
  if (sh)
  {
    ecore_list_remove(signal_handlers);
    e_dbus_signal_handler_free(sh);
  }
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
E_DBus_Signal_Handler *
e_dbus_signal_handler_add(DBusConnection *conn, const char *sender, const char *path, const char *interface, const char *member, E_DBus_Signal_Cb cb_signal, void *data)
{
  E_DBus_Signal_Handler *sh;
  char match[DBUS_MAXIMUM_MATCH_RULE_LENGTH];
  int started = 0;
  int len = 0;
  DBusError err;

  sh = calloc(1, sizeof(E_DBus_Signal_Handler));
  printf("calloc: %p\n", sh);

  strcpy(match, "type='signal'");
  len = 13;

#undef ADD_MATCH_PIECE
#define ADD_MATCH_PIECE(PIECE) \
  if (PIECE) \
  {\
    len += strlen("," #PIECE "=''") + strlen(PIECE);\
    if (len >= sizeof(match)) return NULL;\
    strcat(match, "," #PIECE "='");\
    strcat(match, PIECE);\
    strcat(match, "'");\
    sh->PIECE = strdup(PIECE);\
    started = 1;\
  }

  ADD_MATCH_PIECE(sender)
  ADD_MATCH_PIECE(path)
  ADD_MATCH_PIECE(interface)
  ADD_MATCH_PIECE(member)

  printf("MATCH ON: %s\n", match);
  sh->cb_signal = cb_signal;
  sh->data = data;

  dbus_error_init(&err);
  dbus_bus_add_match(conn, match, NULL);
  ecore_list_append(signal_handlers, sh);

  /* if we have a sender, and it is not a unique name, we need to know the unique name to match since signals will have the name owner as ther sender. */
  if (sender && sender[0] != ':')
    e_dbus_get_name_owner(conn, sender, cb_name_owner, cb_name_owner_error, sh);

  return sh;
}

static int
cb_signal_event(void *data, int type, void *event)
{
  DBusMessage *msg = event;
  E_DBus_Signal_Handler *sh;

  ecore_list_goto_first(signal_handlers);
  while ((sh = ecore_list_next(signal_handlers)))
  {
    if (!sh->cb_signal) continue;

    if (sh->sender && !dbus_message_has_sender(msg, sh->sender)) continue;
    if (sh->path && !dbus_message_has_path(msg, sh->path)) continue;
    if (sh->interface && !dbus_message_has_interface(msg, sh->interface)) continue;
    if (sh->member && !dbus_message_has_member(msg, sh->member)) continue;

    sh->cb_signal(sh->data, msg);
  }

  return 1;
}


