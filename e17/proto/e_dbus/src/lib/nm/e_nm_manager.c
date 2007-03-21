#include "E_Nm.h"
#include "e_nm_private.h"
#define e_nm_manager_call_new(member) dbus_message_new_method_call(E_NM_SERVICE, E_NM_PATH_NETWORK_MANAGER, E_NM_INTERFACE_NETWORK_MANAGER, member)

/**
 * Get all network devices.
 *
 * Returns an Ecore_List of dbus object paths for network devices. This list is
 * of const char *, and is freed automatically after the callback returns.
 *
 * @param ctx an e_nm context
 * @param cb a callback to call when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 */
int
e_nm_get_devices(E_NM_Context *ctx, E_NM_Callback_Func cb_func, void *data)
{
  E_NM_Callback *cb;
  DBusMessage *msg;

  cb = e_nm_callback_new(cb_func, data);
  msg = e_nm_manager_call_new("GetDevices");

  return e_dbus_message_send(ctx->conn, msg, cb_nm_string_list, -1, cb) ? 1 : 0;
}

/**
 * Sleep or wake up network manager.
 *
 * The return_data of the callback will be NULL.
 *
 * @param ctx an e_nm context
 * @param cb a callback to call when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 */
int
e_nm_sleep(E_NM_Context *ctx, E_NM_Callback_Func cb_func, void *data, int do_sleep)
{
  E_NM_Callback *cb;
  DBusMessage *msg;
  dbus_bool_t var = do_sleep;

  cb = e_nm_callback_new(cb_func, data);
  msg = e_nm_manager_call_new("Sleep");
  dbus_message_append_args(msg, DBUS_TYPE_BOOLEAN, &var, DBUS_TYPE_INVALID);

  return e_dbus_message_send(ctx->conn, msg, cb_nm_generic, -1, cb) ? 1 : 0;
}
