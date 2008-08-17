#include "E_Nm.h"
#include "e_nm_private.h"

/**
 * Get the ESSID of a wireless network
 *
 * @param ctx an e_nm context
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 **/
int
e_nm_network_get_name(E_NM_Context *ctx, const char *device,
                      E_DBus_Callback_Func cb_func, void *data)
{
  return e_nm_get_from_device(ctx, device, cb_func, data, "getName",
                              DBUS_TYPE_STRING);
}

/**
 * Get the MAC address of a wireless network's AP
 *
 * @param ctx an e_nm context
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 **/
int
e_nm_network_get_address(E_NM_Context *ctx, const char *device,
                         E_DBus_Callback_Func cb_func, void *data)
{
  return e_nm_get_from_device(ctx, device, cb_func, data, "getAddress",
                              DBUS_TYPE_STRING);
}


/**
 * Get the strength of the network; given out of 100
 *
 * @param ctx an e_nm context
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 **/
int
e_nm_network_get_strength(E_NM_Context *ctx, const char *device,
                          E_DBus_Callback_Func cb_func, void *data)
{
  return e_nm_get_from_device(ctx, device, cb_func, data, "getStrength",
                              DBUS_TYPE_INT32);
}


/**
 * Get the frequency of the network; given in GHz
 *
 * @param ctx an e_nm context
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 **/
int
e_nm_network_get_frequency(E_NM_Context *ctx, const char *device,
                           E_DBus_Callback_Func cb_func, void *data)
{
  return e_nm_get_from_device(ctx, device, cb_func, data, "getFrequency",
                              DBUS_TYPE_DOUBLE);
}


/**
 * Get the data rate of the network; given in Mbps
 *
 * @param ctx an e_nm context
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 **/
int
e_nm_network_get_rate(E_NM_Context *ctx, const char *device,
                         E_DBus_Callback_Func cb_func, void *data)
{
  return e_nm_get_from_device(ctx, device, cb_func, data, "getRate",
                              DBUS_TYPE_INT32);
}


/**
 * Return true if the network requires encryption
 *
 * @param ctx an e_nm context
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 **/
int
e_nm_network_get_encryption(E_NM_Context *ctx, const char *device,
                            E_DBus_Callback_Func cb_func, void *data)
{
  return e_nm_get_from_device(ctx, device, cb_func, data, "getEncryption",
                              DBUS_TYPE_BOOLEAN);
}
