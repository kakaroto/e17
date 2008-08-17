#include "E_Nm.h"
#include "e_nm_private.h"


/**
 * Get all network devices.
 *
 * Returns an Ecore_List of dbus object paths for network devices. This list is
 * of const char *, and is freed automatically after the callback returns.
 *
 * @param ctx an e_nm context
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 **/
EAPI int
e_nm_get_devices(E_NM_Context *ctx, E_DBus_Callback_Func cb_func, void *data)
{
  DBusMessage *msg;
  int ret;

  msg = e_nm_manager_call_new("GetDevices");

  ret = e_dbus_method_call_send(ctx->conn, msg, cb_nm_string_list, cb_func, free_nm_string_list, -1, data) ? 1 : 0;
  dbus_message_unref(msg);
  return ret;
}


/**
 * Find the active device that NM has chosen
 * 
 * Returns a single string containing the dbus path to the active device
 *
 * @param ctx an e_nm context
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 **/
EAPI int
e_nm_get_active_device(E_NM_Context *ctx, E_DBus_Callback_Func cb_func,
                       void *data)
{
  return e_nm_get_from_nm(ctx, cb_func, data,
                          "getActiveDevice", DBUS_TYPE_STRING);
}


/**
 * Query the current state of the network
 * 
 * Returns a single string containing the status:
 *
 *  "connecting":  there is a pending network connection (waiting for a DHCP
 *                  request to complete, waiting for an encryption
 *                  key/passphrase, waiting for a wireless network, etc)
 *
 *  "connected":    there is an active network connection
 *
 *  "scanning":     there is no active network connection, but NetworkManager
 *                  is looking for an access point to associate with
 *
 *  "disconnected": there is no network connection
 * 
 *
 *
 * @param ctx an e_nm context
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 **/
EAPI int
e_nm_status(E_NM_Context *ctx, E_DBus_Callback_Func cb_func,
                       void *data)
{

  return e_nm_get_from_nm(ctx, cb_func, data,
                          "status", DBUS_TYPE_STRING);
}
