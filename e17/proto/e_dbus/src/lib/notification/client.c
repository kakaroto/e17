#include "E_Notify.h"
#include "e_notify_private.h"
static E_DBus_Connection *client_conn;
static int init_count = 0;
  
int
e_notification_init(void)
{
  if (init_count) return ++init_count;

  if (!e_dbus_init()) return 0;
  client_conn = e_dbus_bus_get(DBUS_BUS_SESSION);
  if (!client_conn)
  {
    e_dbus_shutdown();
    return 0;
  }

  return ++init_count;
}

int
e_notification_shutdown(void)
{
  if (--init_count) return init_count;
  e_dbus_connection_unref(client_conn);
  client_conn = NULL;
  e_dbus_shutdown();
  return 0;
}

/**** client api ****/

static void
cb_notify(void *data, DBusMessage *msg, DBusError *err)
{
  E_DBus_Callback *cb;
  E_Notification_Return_Notify *ret = NULL;
  cb = data;
  if (!cb) return;

  if (!dbus_error_is_set(err))
    ret = e_notify_unmarshal_notify_return(msg);

  e_dbus_callback_call(cb, ret, err);
}

void
e_notification_send(E_Notification *n, E_DBus_Callback_Func func, void *data)
{
  DBusMessage *msg;
  E_DBus_Callback *cb;

  cb = e_dbus_callback_new(func, data);
  msg = e_notify_marshal_notify(n);
  printf("msg: %p\n", msg);
  e_dbus_message_send(client_conn, msg, cb_notify, -1, cb);
}


static void
cb_get_capabilities(void *data, DBusMessage *msg, DBusError *err)
{
  E_DBus_Callback *cb;
  E_Notification_Return_Get_Capabilities *ret = NULL;
  cb = data;
  if (!cb) return;

  if (!dbus_error_is_set(err))
    ret = e_notify_unmarshal_get_capabilities_return(msg);

  e_dbus_callback_call(cb, ret, err);
}

void
e_notification_get_capabilities(E_DBus_Callback_Func func, void *data)
{
  DBusMessage *msg;
  E_DBus_Callback *cb;

  cb = e_dbus_callback_new(func, data);
  msg = e_notify_marshal_get_capabilities();
  e_dbus_message_send(client_conn, msg, cb_notify, -1, cb);
}

static void
cb_get_server_information(void *data, DBusMessage *msg, DBusError *err)
{
  E_DBus_Callback *cb;
  E_Notification_Return_Get_Server_Information *ret = NULL;
  cb = data;
  if (!cb) return;

  if (!dbus_error_is_set(err))
    ret = e_notify_unmarshal_get_server_information_return(msg);

  e_dbus_callback_call(cb, ret, err);
}

void
e_notification_get_server_information(E_DBus_Callback_Func func, void *data)
{
  DBusMessage *msg;
  E_DBus_Callback *cb;

  cb = e_dbus_callback_new(func, data);
  msg = e_notify_marshal_get_server_information();
  e_dbus_message_send(client_conn, msg, cb_notify, -1, cb);
}
