/*
 * This file defines functions that query each of the functions provided by
 * the org.freedesktop.NetworkManager.Device DBus interface.
 */

#include "E_Nm.h"
#include "e_nm_private.h"
#include <Ecore_Data.h>


/**
 * Get the UDI of a NetworkManager device
 *
 * @param ctx an e_nm context
 * @param device a NetworkManager device to communicate with
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 */
EAPI int
e_nm_device_get_udi(E_NM_Context *ctx, const char *device,
                    E_DBus_Callback_Func cb_func, void *data)
{
  /* FIXME: Decide how to handle the return value for this functions */
  e_nm_device_properties_get(ctx->conn, device, "Udi", cb_func, data);
}


/**
 * Get the interface name of a NetworkManager device
 *
 * @param ctx an e_nm context
 * @param device a NetworkManager device to communicate with
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 */
EAPI int
e_nm_device_get_interface(E_NM_Context *ctx, const char *device,
                          E_DBus_Callback_Func cb_func, void *data)
{
  /* FIXME: Decide how to handle the return value for this functions */
  e_nm_device_properties_get(ctx->conn, device, "Interface", cb_func, data);
}


/**
 * Get the driver name of a NetworkManager device
 *
 * @param ctx an e_nm context
 * @param device a NetworkManager device to communicate with
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 */
EAPI int
e_nm_device_get_driver(E_NM_Context *ctx, const char *device,
                       E_DBus_Callback_Func cb_func, void *data)
{
  /* FIXME: Decide how to handle the return value for this functions */
  e_nm_device_properties_get(ctx->conn, device, "Driver", cb_func, data);
}


/**
 * Get the capabilities of a NetworkManager device
 *
 * @param ctx an e_nm context
 * @param device a NetworkManager device to communicate with
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 */
EAPI int
e_nm_device_get_capabilities(E_NM_Context *ctx, const char *device,
                             E_DBus_Callback_Func cb_func, void *data)
{
  /* FIXME: Decide how to handle the return value for this functions */
  e_nm_device_properties_get(ctx->conn, device, "Capabilities", cb_func, data);
}


/**
 * Get the IPv4 address of a NetworkManager device
 *
 * @param ctx an e_nm context
 * @param device a NetworkManager device to communicate with
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 */
EAPI int
e_nm_device_get_ip4address(E_NM_Context *ctx, const char *device,
                           E_DBus_Callback_Func cb_func, void *data)
{
  /* FIXME: Decide how to handle the return value for this functions */
  e_nm_device_properties_get(ctx->conn, device, "Ip4Address", cb_func, data);
}


/**
 * Get the state of a NetworkManager device
 *
 * @param ctx an e_nm context
 * @param device a NetworkManager device to communicate with
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 */
EAPI int
e_nm_device_get_state(E_NM_Context *ctx, const char *device,
                      E_DBus_Callback_Func cb_func, void *data)
{
  /* FIXME: Decide how to handle the return value for this functions */
  e_nm_device_properties_get(ctx->conn, device, "State", cb_func, data);
}


/**
 * Get the IPv4 config object path of a NetworkManager device
 *
 * @param ctx an e_nm context
 * @param device a NetworkManager device to communicate with
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 */
EAPI int
e_nm_device_get_ip4config(E_NM_Context *ctx, const char *device,
                          E_DBus_Callback_Func cb_func, void *data)
{
  /* FIXME: Decide how to handle the return value for this functions */
  e_nm_device_properties_get(ctx->conn, device, "Ip4Config", cb_func, data);
}


/**
 * Get the carrier status of a NetworkManager device
 *
 * @param ctx an e_nm context
 * @param device a NetworkManager device to communicate with
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 */
EAPI int
e_nm_device_get_carrier(E_NM_Context *ctx, const char *device,
                        E_DBus_Callback_Func cb_func, void *data)
{
  /* FIXME: Decide how to handle the return value for this functions */
  e_nm_device_properties_get(ctx->conn, device, "Carrier", cb_func, data);
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
EAPI int
e_nm_device_get_type(E_NM_Context *ctx, const char *device, 
                     E_DBus_Callback_Func cb_func, void *data)
{
  /* FIXME: Decide how to handle the return value for this functions */
  e_nm_device_properties_get(ctx->conn, device, "DeviceType", cb_func, data);
}
