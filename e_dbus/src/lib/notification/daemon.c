#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "E_Notify.h"
#include "E_Notification_Daemon.h"
#include "e_notify_private.h"

#include <stdlib.h>
#include <string.h>

static int init_count = 0;
static E_DBus_Interface *daemon_iface = NULL;

static int e_notification_daemon_bus_init(E_Notification_Daemon *ndaemon);
static int e_notification_daemon_object_init(E_Notification_Daemon *ndaemon);

DBusMessage *
method_get_capabilities(E_DBus_Object *obj __UNUSED__, DBusMessage *message)
{
   const char *capabilities[] = {
      "body",
      "actions",
      NULL
   };
   return e_notify_marshal_get_capabilities_return(message, capabilities);
}

DBusMessage *
method_notify(E_DBus_Object *obj, DBusMessage *message)
{
   E_Notification *n;
   E_Notification_Daemon *ndaemon;
   int id = -1;

   ndaemon = e_dbus_object_data_get(obj);
   n = e_notify_unmarshal_notify(message, NULL);
   if (ndaemon->func.notify)
     id = ndaemon->func.notify(ndaemon, n);
   else
     return dbus_message_new_error(message, E_NOTIFICATION_INTERFACE ".Unimplemented", "This functionality has not yet been implemented");

   e_notification_unref(n);
   return e_notify_marshal_notify_return(message, id);
}

DBusMessage *
method_close_notification(E_DBus_Object *obj, DBusMessage *message)
{
   E_Notification_Daemon *ndaemon;
   dbus_uint32_t id;

   ndaemon = e_dbus_object_data_get(obj);
   id = e_notify_unmarshal_close_notification(message, NULL);
   if (ndaemon->func.close_notification)
     ndaemon->func.close_notification(ndaemon, id);
   return dbus_message_new_method_return(message);
}

DBusMessage *
method_get_server_information(E_DBus_Object *obj, DBusMessage *message)
{
   E_Notification_Daemon *ndaemon;

   ndaemon = e_dbus_object_data_get(obj);

   return e_notify_marshal_get_server_information_return(message, ndaemon->name, ndaemon->vendor, E_NOTIFICATION_DAEMON_VERSION, E_NOTIFICATION_DAEMON_SUPPORTS_SPEC_VERSION);
}

/**** daemon api ****/

EAPI int
e_notification_daemon_init(void)
{
   if (init_count) return ++init_count;
   if (!e_dbus_init()) return 0;

   daemon_iface = e_dbus_interface_new(E_NOTIFICATION_INTERFACE);

   return ++init_count;
}

EAPI int
e_notification_daemon_shutdown(void)
{
   if (--init_count) return init_count;
   e_dbus_shutdown();
   return 0;
}

EAPI E_Notification_Daemon *
e_notification_daemon_add(const char *name, const char *vendor)
{
   E_Notification_Daemon *ndaemon;

   loginit();
   ndaemon = calloc(1, sizeof(E_Notification_Daemon));
   if (ndaemon)
     e_notification_daemon_bus_init(ndaemon);

   if (!ndaemon || !ndaemon->conn)
     {
        if (ndaemon) free(ndaemon);
        e_dbus_shutdown();
        return NULL;
     }

   ndaemon->name = strdup(name);
   ndaemon->vendor = strdup(vendor);

   e_dbus_interface_ref(daemon_iface);
   ndaemon->iface = daemon_iface;
   e_dbus_interface_method_add(ndaemon->iface, "GetCapabilities", "", "as", method_get_capabilities);
   e_dbus_interface_method_add(ndaemon->iface, "Notify", "susssasa{sv}i", "u", method_notify);
   e_dbus_interface_method_add(ndaemon->iface, "CloseNotification", "u", "u", method_close_notification);
   e_dbus_interface_method_add(ndaemon->iface, "GetServerInformation", "", "ssss", method_get_server_information);

   return ndaemon;
}

EAPI void
e_notification_daemon_free(E_Notification_Daemon *ndaemon)
{
   e_dbus_release_name(ndaemon->conn, E_NOTIFICATION_BUS_NAME, NULL, NULL);
   if (ndaemon->obj)
     {
        e_dbus_object_interface_detach(ndaemon->obj, ndaemon->iface);
        e_dbus_object_free(ndaemon->obj);
     }
   if (ndaemon->conn) e_dbus_connection_close(ndaemon->conn);
   if (ndaemon->name) free(ndaemon->name);
   if (ndaemon->vendor) free(ndaemon->vendor);
   if (ndaemon->iface) e_dbus_interface_unref(ndaemon->iface);
   free(ndaemon);
}

EAPI void
e_notification_daemon_data_set(E_Notification_Daemon *ndaemon, void *data)
{
   ndaemon->data = data;
}

EAPI void *
e_notification_daemon_data_get(E_Notification_Daemon *ndaemon)
{
   return ndaemon->data;
}

EAPI void
e_notification_daemon_callback_notify_set(E_Notification_Daemon *ndaemon, E_Notification_Daemon_Callback_Notify func)
{
   ndaemon->func.notify = func;
}

EAPI void
e_notification_daemon_callback_close_notification_set(E_Notification_Daemon *ndaemon, E_Notification_Daemon_Callback_Close_Notification func)
{
   ndaemon->func.close_notification = func;
}

static void
cb_request_name(void *data, DBusMessage *msg, DBusError *err)
{
   E_Notification_Daemon *ndaemon = data;
   dbus_uint32_t ret;
   DBusError new_err;

   if (dbus_error_is_set(err))
     {
        ERR("request_name: %s", err->message);
        dbus_error_free(err);
        return;
     }

   INF("received response with signature: '%s'",
       dbus_message_get_signature(msg));
   dbus_error_init(&new_err);
   dbus_message_get_args
     (msg, &new_err, DBUS_TYPE_UINT32, &ret, DBUS_TYPE_INVALID);
   if (dbus_error_is_set(&new_err))
     {
        ERR("req name unmarshal: %s", new_err.message);
        dbus_error_free(&new_err);
        return;
     }

   switch (ret)
     {
      case DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER:
      case DBUS_REQUEST_NAME_REPLY_ALREADY_OWNER:
        e_notification_daemon_object_init(ndaemon);
        break;

      case DBUS_REQUEST_NAME_REPLY_IN_QUEUE:
        //XXX mark ndaemon as queued?
        break;

      case DBUS_REQUEST_NAME_REPLY_EXISTS:
        //XXX exit?
        break;
     }
}

static int
e_notification_daemon_bus_init(E_Notification_Daemon *ndaemon)
{
   ndaemon->conn = e_dbus_bus_get(DBUS_BUS_SESSION);
   if (!ndaemon->conn) return 0;

   // this blocks... make it async, and handle failure, etc
   e_dbus_request_name(ndaemon->conn, E_NOTIFICATION_BUS_NAME, DBUS_NAME_FLAG_REPLACE_EXISTING, cb_request_name, ndaemon);

   return 1;
}

static int
e_notification_daemon_object_init(E_Notification_Daemon *ndaemon)
{
   if (!ndaemon || !ndaemon->conn) return 0;
   ndaemon->obj = e_dbus_object_add(ndaemon->conn, E_NOTIFICATION_PATH, ndaemon);
   if (!ndaemon->obj) return 0;

   e_dbus_object_interface_attach(ndaemon->obj, ndaemon->iface);

   return 1;
}

EAPI void
e_notification_daemon_signal_notification_closed(E_Notification_Daemon *ndaemon, unsigned int id, E_Notification_Closed_Reason reason)
{
   DBusMessage *msg = e_notify_marshal_notification_closed_signal(id, reason);
   e_dbus_message_send(ndaemon->conn,
                       msg,
                       NULL, -1, NULL);
   dbus_message_unref(msg);
}

EAPI void
e_notification_daemon_signal_action_invoked(E_Notification_Daemon *ndaemon, unsigned int notification_id, const char *action_id)
{
   DBusMessage *msg = e_notify_marshal_action_invoked_signal(notification_id, action_id);
   e_dbus_message_send(ndaemon->conn,
                       msg,
                       NULL, -1, NULL);
   dbus_message_unref(msg);
}
