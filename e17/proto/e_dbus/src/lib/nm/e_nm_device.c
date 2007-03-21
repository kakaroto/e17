#include "E_Nm.h"
#include "e_nm_private.h"
#include <Ecore_Data.h>

#define e_nm_device_call_new(path, member) dbus_message_new_method_call(E_NM_SERVICE, path, E_NM_INTERFACE_DEVICE, member)
#define e_nm_device_wired_call_new(path, member) dbus_message_new_method_call(E_NM_SERVICE, path, E_NM_INTERFACE_DEVICE_WIRED, member)
#define e_nm_device_wireless_call_new(path, member) dbus_message_new_method_call(E_NM_SERVICE, path, E_NM_INTERFACE_DEVICE_WIRELESS, member)


int
e_nm_device_deactivate(E_NM_Context *ctx, const char *device, E_NM_Callback_Func cb_func, void *data)
{
  E_NM_Callback *cb;
  DBusMessage *msg;

  cb = e_nm_callback_new(cb_func, data);
  msg = e_nm_device_call_new(device, "Deactivate");
  return e_dbus_message_send(ctx->conn, msg, cb_nm_generic, -1, cb) ? 1 : 0;
}

int
e_nm_device_wired_activate(E_NM_Context *ctx, const char *device, E_NM_Callback_Func cb_func, void *data, char user_requested)
{
  E_NM_Callback *cb;
  DBusMessage *msg;
  dbus_bool_t val;

  val = user_requested;

  cb = e_nm_callback_new(cb_func, data);
  msg = e_nm_device_wired_call_new(device, "Activate");
  dbus_message_append_args(msg, DBUS_TYPE_BOOLEAN, &val, DBUS_TYPE_INVALID);
  return e_dbus_message_send(ctx->conn, msg, cb_nm_generic, -1, cb) ? 1 : 0;
}

int
e_nm_device_wireless_get_active_networks(E_NM_Context *ctx, const char *device, E_NM_Callback_Func cb_func, void *data)
{
  E_NM_Callback *cb;
  DBusMessage *msg;

  cb = e_nm_callback_new(cb_func, data);
  msg = e_nm_device_wireless_call_new(device, "GetActiveNetworks");
  return e_dbus_message_send(ctx->conn, msg, cb_nm_string_list, -1, cb) ? 1 : 0;
}

int
e_nm_device_wireless_activate(E_NM_Context *ctx, const char *device, E_NM_Callback_Func cb_func, void *data, const char *access_point, char user_requested)
{
  E_NM_Callback *cb;
  DBusMessage *msg;
  dbus_bool_t val;

  val = user_requested;

  cb = e_nm_callback_new(cb_func, data);
  msg = e_nm_device_wireless_call_new(device, "Activate");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &access_point, DBUS_TYPE_BOOLEAN, &val, DBUS_TYPE_INVALID);
  return e_dbus_message_send(ctx->conn, msg, cb_nm_generic, -1, cb) ? 1 : 0;
}
