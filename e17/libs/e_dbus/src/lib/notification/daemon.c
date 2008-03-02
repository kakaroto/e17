#include "E_Notify.h"
#include "E_Notification_Daemon.h"
#include "e_notify_private.h"
#include <string.h>

static int e_notification_daemon_bus_init(E_Notification_Daemon *daemon);
static int e_notification_daemon_object_init(E_Notification_Daemon *daemon);

DBusMessage *
method_get_capabilities(E_DBus_Object *obj, DBusMessage *message)
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
  E_Notification_Daemon *daemon;
  int id = -1;
  
  daemon = e_dbus_object_data_get(obj);
  n = e_notify_unmarshal_notify(message, NULL);
  if (daemon->func.notify)
    id = daemon->func.notify(daemon, n);
  else
    return dbus_message_new_error(message, E_NOTIFICATION_INTERFACE".Unimplemented", "This functionality has not yet been implemented");

  e_notification_unref(n);
  return e_notify_marshal_notify_return(message, id);
}

DBusMessage *
method_close_notification(E_DBus_Object *obj, DBusMessage *message)
{
  E_Notification_Daemon *daemon;
  dbus_uint32_t id;

  daemon = e_dbus_object_data_get(obj);
  id = e_notify_unmarshal_close_notification(message, NULL);
  if (daemon->func.close_notification)
    daemon->func.close_notification(daemon, id);
  return dbus_message_new_method_return(message);
}

DBusMessage *
method_get_server_information(E_DBus_Object *obj, DBusMessage *message)
{
  E_Notification_Daemon *daemon;

  daemon = e_dbus_object_data_get(obj);

  return e_notify_marshal_get_server_information_return(message, daemon->name, daemon->vendor, E_NOTIFICATION_DAEMON_VERSION);
}


/**** daemon api ****/


EAPI E_Notification_Daemon *
e_notification_daemon_add(const char *name, const char *vendor)
{
  E_Notification_Daemon *daemon;

  if (!e_dbus_init()) return NULL;
  daemon = calloc(1, sizeof(E_Notification_Daemon));
  if (daemon)
    e_notification_daemon_bus_init(daemon);

  if (!daemon || !daemon->conn)
  {
    if (daemon) free(daemon);
    e_dbus_shutdown();
    return NULL;
  }

  daemon->name = strdup(name);
  daemon->vendor = strdup(vendor);

  daemon->iface = e_dbus_interface_new(E_NOTIFICATION_INTERFACE);
  e_dbus_interface_method_add(daemon->iface, "GetCapabilities", "", "as", method_get_capabilities);
  e_dbus_interface_method_add(daemon->iface, "Notify", "susssasa{sv}i", "u", method_notify);
  e_dbus_interface_method_add(daemon->iface, "CloseNotification", "u", "u", method_close_notification);
  e_dbus_interface_method_add(daemon->iface, "GetServerInformation", "", "sss", method_get_server_information);

  return daemon;
}


EAPI void
e_notification_daemon_free(E_Notification_Daemon *daemon)
{
  e_dbus_release_name(daemon->conn, E_NOTIFICATION_BUS_NAME, NULL, NULL);
  if (daemon->obj) 
    {
      e_dbus_object_interface_detach(daemon->obj, daemon->iface);
      e_dbus_object_free(daemon->obj);
    }
  if (daemon->conn) e_dbus_connection_close(daemon->conn);
  if (daemon->name) free(daemon->name);
  if (daemon->vendor) free(daemon->vendor);
  if (daemon->iface) e_dbus_interface_unref(daemon->iface);
  free(daemon);
  e_dbus_shutdown();
}

EAPI void
e_notification_daemon_data_set(E_Notification_Daemon *daemon, void *data)
{
  daemon->data = data;
}

EAPI void *
e_notification_daemon_data_get(E_Notification_Daemon *daemon)
{
  return daemon->data;
}

EAPI void
e_notification_daemon_callback_notify_set(E_Notification_Daemon *daemon, E_Notification_Daemon_Callback_Notify func)
{
  daemon->func.notify = func;
}

EAPI void
e_notification_daemon_callback_close_notification_set(E_Notification_Daemon *daemon, E_Notification_Daemon_Callback_Close_Notification func)
{
  daemon->func.close_notification = func;
}


static void
cb_request_name(void *data, DBusMessage *msg, DBusError *err)
{
  E_Notification_Daemon *daemon = data;
  dbus_uint32_t ret;
  DBusError new_err;

  if (dbus_error_is_set(err))
  {
    printf("Error (request_name): %s\n", err->message);
    dbus_error_free(err);
    return;
  }

  printf("received response with signature: '%s'\n", dbus_message_get_signature(msg));
  dbus_error_init(&new_err);
  dbus_message_get_args(msg, &new_err, DBUS_TYPE_UINT32, &ret, DBUS_TYPE_INVALID);
  if (dbus_error_is_set(&new_err))
  {
    printf("Error (req name unmarshal): %s\n", new_err.message);
    dbus_error_free(&new_err);
    return;
  }

  switch(ret)
  {
    case DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER:
    case DBUS_REQUEST_NAME_REPLY_ALREADY_OWNER:
      e_notification_daemon_object_init(daemon);
      break;
    case DBUS_REQUEST_NAME_REPLY_IN_QUEUE:
      //XXX mark daemon as queued?
      break;
    case DBUS_REQUEST_NAME_REPLY_EXISTS:
      //XXX exit?
      break;
  }
}

static int
e_notification_daemon_bus_init(E_Notification_Daemon *daemon)
{
  daemon->conn = e_dbus_bus_get(DBUS_BUS_SESSION);
  if (!daemon->conn) return 0;

  // this blocks... make it async, and handle failure, etc
  e_dbus_request_name(daemon->conn, E_NOTIFICATION_BUS_NAME, DBUS_NAME_FLAG_REPLACE_EXISTING, cb_request_name, daemon);

  return 1;
}

static int
e_notification_daemon_object_init(E_Notification_Daemon *daemon)
{
  if (!daemon || !daemon->conn) return 0;
  daemon->obj = e_dbus_object_add(daemon->conn, E_NOTIFICATION_PATH, daemon);
  if (!daemon->obj) return 0;

  e_dbus_object_interface_attach(daemon->obj, daemon->iface);

  return 1;
}


EAPI void
e_notification_daemon_signal_notification_closed(E_Notification_Daemon *daemon, unsigned int id, E_Notification_Closed_Reason reason)
{
  DBusMessage *msg = e_notify_marshal_notification_closed_signal(id, reason);
  e_dbus_message_send(daemon->conn, 
                      msg,
                      NULL, -1, NULL);
  dbus_message_unref(msg);
}

EAPI void
e_notification_daemon_signal_action_invoked(E_Notification_Daemon *daemon, unsigned int notification_id, const char *action_id)
{
  DBusMessage *msg = e_notify_marshal_action_invoked_signal(notification_id, action_id);
  e_dbus_message_send(daemon->conn, 
                      msg,
                      NULL, -1, NULL);
  dbus_message_unref(msg);
}
