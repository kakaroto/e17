#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>

#include "e_dbus_private.h"

#define SENDER_KEY "sender"
#define PATH_KEY "path"
#define INTERFACE_KEY "interface"
#define MEMBER_KEY "member"
#define NAME_OWNER_MATCH "type='signal',sender='org.freedesktop.DBus',\
         path='/org/freedesktop/DBus',interface='org.freedesktop.DBus',\
         member='NameOwnerChanged',arg0='%s'"
#define MEMBER_NAME_OWNER_CHANGED "NameOwnerChanged"

struct E_DBus_Signal_Handler
{
   char *sender;
   char *path;
   char *interface;
   char *member;
   char *owner;
   char *match;
   char *match_name_owner_change;
   
   E_DBus_Signal_Cb cb_signal;
   DBusPendingCall *get_name_owner_pending;
   void *data;
   unsigned char delete_me : 1;
};

static void cb_signal_dispatcher(E_DBus_Connection *conn, DBusMessage *msg);

/*
 * Free a signal handler
 * @param sh the signal handler to free
 */
static void
e_dbus_signal_handler_free(E_DBus_Signal_Handler *sh)
{
  free(sh->sender);
  free(sh->interface);
  free(sh->path);
  free(sh->member);
  free(sh->owner);
  free(sh->match);
  free(sh->match_name_owner_change);
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
  E_DBus_Signal_Handler *sh;
  DBusError new_err;

  sh = d->sh;
  sh->get_name_owner_pending = NULL;
  free(d);

  if (dbus_error_is_set(err)) return;

  dbus_error_init(&new_err);
  dbus_message_get_args(msg, &new_err, DBUS_TYPE_STRING,
                        &unique_name, DBUS_TYPE_INVALID);

  if (dbus_error_is_set(&new_err)) return;

  if (unique_name != NULL && unique_name[0]) sh->owner = strdup(unique_name);
}

static void
_match_append(Eina_Strbuf *match, const char *key, const char *value)
{
   if (value == NULL || !value[0]) return;

   if ((eina_strbuf_length_get(match) + 4 + strlen(key) + strlen(value))
       >= DBUS_MAXIMUM_MATCH_RULE_LENGTH)
     {
        ERR("cannot add match %s='%s' to %s: too long!", key, value,
            eina_strbuf_string_get(match));
        return;
     }

   eina_strbuf_append_printf(match, ",%s='%s'", key, value);
}

EAPI E_DBus_Signal_Handler *
e_dbus_signal_handler_add(E_DBus_Connection *conn, const char *sender, const char *path, const char *interface, const char *member, E_DBus_Signal_Cb cb_signal, void *data)
{
  E_DBus_Signal_Handler *sh;
  Eina_Strbuf *match;
  DBusError err;

  EINA_SAFETY_ON_NULL_RETURN_VAL(conn, NULL);
  sh = calloc(1, sizeof(E_DBus_Signal_Handler));
  if (!sh)
    {
       ERR("could not allocate signal handler.");
       return NULL;
    }

  match = eina_strbuf_new();
  eina_strbuf_append(match, "type='signal'");

  _match_append(match, SENDER_KEY, sender);
  _match_append(match, PATH_KEY, path);
  _match_append(match, INTERFACE_KEY, interface);
  _match_append(match, MEMBER_KEY, member);

  if (sender) sh->sender = strdup(sender);
  if (path) sh->path = strdup(path);
  if (interface) sh->interface = strdup(interface);
  if (member) sh->member = strdup(member);
  sh->match = strdup(eina_strbuf_string_get(match));

  sh->cb_signal = cb_signal;
  sh->get_name_owner_pending = NULL;
  sh->data = data;
  sh->delete_me = 0;

  dbus_error_init(&err);
  dbus_bus_add_match(conn->conn, sh->match, &err);

  if (!conn->signal_handlers) conn->signal_dispatcher = cb_signal_dispatcher;

  /* if we have a sender, and it is not a unique name, we need to know the
   * unique name to match since signals will have the name owner as their
   * sender.
   */
  if (sender && sender[0] != ':' && strcmp(sender, E_DBUS_FDO_BUS) != 0)
    {
       struct cb_name_owner_data *data_cb;

       // listen when the owner of the sender name change
       eina_strbuf_reset(match);
       eina_strbuf_append_printf(match, NAME_OWNER_MATCH, sh->sender);
       sh->match_name_owner_change = strdup(eina_strbuf_string_get(match));

       dbus_bus_add_match(conn->conn, sh->match_name_owner_change, NULL);
       DBG("add name owner match=%s", sh->match_name_owner_change);

       data_cb = malloc(sizeof(*data_cb));
       if (!data_cb)
	 {
	    e_dbus_signal_handler_free(sh);
	    eina_strbuf_free(match);
            ERR("could not allocate cb_name_owner_data.");
	    return NULL;
	 }
       data_cb->conn = conn;
       data_cb->sh = sh;
       sh->get_name_owner_pending =
         e_dbus_get_name_owner(conn, sender, cb_name_owner, data_cb);
    }
  else if (sender) sh->owner = strdup(sender);

  eina_strbuf_free(match);
  conn->signal_handlers = eina_list_append(conn->signal_handlers, sh);

  return sh;
}

static int e_dbus_handler_deletions = 0;

EAPI void
e_dbus_signal_handler_del(E_DBus_Connection *conn, E_DBus_Signal_Handler *sh)
{
   if ((!conn) || (!sh)) return;

   if (sh->get_name_owner_pending)
     {
        dbus_pending_call_cancel(sh->get_name_owner_pending);
        sh->get_name_owner_pending = NULL;
     }
   sh->delete_me = 1;
   if (e_dbus_idler_active)
     {
        e_dbus_handler_deletions = 1;
        return;
     }

   conn->signal_handlers = eina_list_remove(conn->signal_handlers, sh);

   dbus_bus_remove_match(conn->conn, sh->match, NULL);
   if (sh->match_name_owner_change)
     dbus_bus_remove_match(conn->conn, sh->match_name_owner_change, NULL);

   e_dbus_signal_handler_free(sh);
}

static void
cb_signal_dispatcher(E_DBus_Connection *conn, DBusMessage *msg)
{
  E_DBus_Signal_Handler *sh;
  Eina_List *l;

  if(dbus_message_has_sender(msg, E_DBUS_FDO_BUS) &&
     dbus_message_has_path(msg, E_DBUS_FDO_PATH) &&
     dbus_message_has_interface(msg, E_DBUS_FDO_INTERFACE) &&
     dbus_message_has_member(msg, MEMBER_NAME_OWNER_CHANGED))
    {
       DBusError new_err;
       char *bus, *old_owner, *new_owner;
       dbus_error_init(&new_err);
       dbus_message_get_args(msg, &new_err, DBUS_TYPE_STRING, &bus,
                             DBUS_TYPE_STRING, &old_owner,
                             DBUS_TYPE_STRING, &new_owner, DBUS_TYPE_INVALID);

       if (dbus_error_is_set(&new_err)) return;

       EINA_LIST_FOREACH(conn->signal_handlers, l, sh)
         {
            if (!sh->delete_me && sh->sender && strcmp(sh->sender, bus) == 0)
              {
                 free(sh->owner);
                 sh->owner = NULL;
                 if (new_owner != NULL && new_owner[0])
                    sh->owner = strdup(new_owner);
              }
         }
    }

  EINA_LIST_FOREACH(conn->signal_handlers, l, sh)
  {
    if ((!sh->cb_signal) || (sh->delete_me)) continue;

    if (sh->get_name_owner_pending ||
        (sh->owner && !dbus_message_has_sender(msg, sh->owner))) continue;
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
  Eina_List *l, *l_next;

  if (!e_dbus_handler_deletions) return;
  e_dbus_handler_deletions = 0;
  if (!conn->signal_handlers) return;
  EINA_LIST_FOREACH_SAFE(conn->signal_handlers, l, l_next, sh)
  {
    if (sh->delete_me)
      e_dbus_signal_handler_del(conn, sh);
  }
}

void
e_dbus_signal_handlers_free_all(E_DBus_Connection *conn)
{
   E_DBus_Signal_Handler *sh;
   EINA_LIST_FREE(conn->signal_handlers, sh)
     e_dbus_signal_handler_free(sh);
}
