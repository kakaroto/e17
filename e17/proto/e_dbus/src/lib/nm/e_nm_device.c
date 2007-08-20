/*
 * This file defines functions that query each of the functions provided by
 * the NetworkManager Device interface.
 */

#include "E_Nm.h"
#include "e_nm_private.h"
#include <Ecore_Data.h>

/**
 * Get the system name of a NetworkManager device
 *
 * Returns an Ecore_List of dbus object paths for network devices. This list is
 * of const char *, and is freed automatically after the callback returns.
 *
 * @param ctx an e_nm context
 * @param device a NetworkManager device to communicate with
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 */
int
e_nm_device_get_name(E_NM_Context *ctx, const char *device,
                     E_DBus_Callback_Func cb_func, void *data)
{
  return e_nm_get_from_device(ctx, device, cb_func, data, "getName",
                              DBUS_TYPE_STRING);
}


/**
 * Return the type of a an NM device:
 *
 * 0: unknown
 * 1: wired
 * 2: wireless
 *
 * @param ctx an e_nm context
 * @param device a NetworkManager device to communicate with
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 */
int
e_nm_device_get_type(E_NM_Context *ctx, const char *device, 
                     E_DBus_Callback_Func cb_func, void *data)
{
  return e_nm_get_from_device(ctx, device, cb_func, data, "getType",
                              DBUS_TYPE_INT32);
}


/**
 * Get the HAL UDI of a NetworkManager device
 *
 * @param ctx an e_nm context
 * @param device a NetworkManager device to communicate with
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 */
int
e_nm_device_get_hal_udi(E_NM_Context *ctx, const char *device,
                        E_DBus_Callback_Func cb_func, void *data)
{
  return e_nm_get_from_device(ctx, device, cb_func, data, "getHalUdi",
                              DBUS_TYPE_STRING);
}


/**
 * Get the IPv4 address of a NetworkManager device
 *
 * @param ctx an e_nm context
 * @param device a NetworkManager device to communicate with
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 */
int
e_nm_device_get_ip4_address(E_NM_Context *ctx, const char *device,
                            E_DBus_Callback_Func cb_func, void *data)
{
  return e_nm_get_from_device(ctx, device, cb_func, data, "getIP4Address",
                              DBUS_TYPE_UINT32);
}


/**
 * Get the link status of a NetworkManager device
 *
 * @param ctx an e_nm context
 * @param device a NetworkManager device to communicate with
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 */
int
e_nm_device_get_link_active(E_NM_Context *ctx, const char *device,
                            E_DBus_Callback_Func cb_func, void *data)
{
  return e_nm_get_from_device(ctx, device, cb_func, data, "getLinkActive",
                              DBUS_TYPE_BOOLEAN);
}


/**
 * Get the signal strength of a the wireless network that a NetworkManager 
 * device is connected to.
 *
 * @param ctx an e_nm context
 * @param device a NetworkManager device to communicate with
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 */
int
e_nm_device_wireless_get_strength(E_NM_Context *ctx, const char *device,
                            E_DBus_Callback_Func cb_func, void *data)
{
  return e_nm_get_from_device(ctx, device, cb_func, data, "getStrength",
                              DBUS_TYPE_INT32);
}


/**
 * Find the NetworkManager device's currently associated wireless network
 *
 * @param ctx an e_nm context
 * @param device a NetworkManager device to communicate with
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 */
int
e_nm_device_wireless_get_active_network(E_NM_Context *ctx, const char *device,
                                        E_DBus_Callback_Func cb_func, void *data)
{
  return e_nm_get_from_device(ctx, device, cb_func, data, "getActiveNetwork",
                              DBUS_TYPE_STRING);
}


/**
 * Get the list of available wireless networks
 *
 * Returns an Ecore_List of wireless network names
 *
 * @param ctx an e_nm context
 * @param device a NetworkManager device to communicate with
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 */
int
e_nm_device_wireless_get_networks(E_NM_Context *ctx, const char *device, 
                                  E_DBus_Callback_Func cb_func, void *data)
{
  DBusMessage *msg;
  int ret;

  msg = e_nm_device_call_new(device, "getNetworks");
  ret = e_dbus_method_call_send(ctx->conn, msg, cb_nm_string_list, cb_func, free_nm_string_list, -1, data) ? 1 : 0;
  dbus_message_unref(msg);
  return ret;
}

