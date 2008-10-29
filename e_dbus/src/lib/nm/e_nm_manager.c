#include "E_Nm.h"
#include "e_nm_private.h"

static void
cb_nm_device(void *data, void *reply, DBusError *err)
{
  E_NM_Data   *d;
  E_NM_Device *device;
  Ecore_List  *list;

  d = data;
  list = d->reply;
  if (!dbus_error_is_set(err))
    ecore_list_append(list, reply);
  ecore_list_first_remove(list);

  ecore_list_first_goto(list);
  if (ecore_list_current(list) != (void *)-1)
  {
    DBusError new_err;

    dbus_error_init(&new_err);
    d->cb_func(d->data, list, &new_err);
    dbus_error_free(&new_err);
    free(d);
  }
}

static void
cb_nm_devices(void *data, void *reply, DBusError *err)
{
  E_NM_Data  *d;
  Ecore_List *devices;
  Ecore_List *list;
  const char *dev;

  d = data;
  if (dbus_error_is_set(err))
  {
    d->cb_func(d->data, NULL, err);
    return;
  }
  devices = reply;
  ecore_list_first_goto(devices);
  list = ecore_list_new();
  ecore_list_free_cb_set(list, e_nm_device_free_device);
  d->reply = list;
  while ((dev = ecore_list_next(devices)))
  {
    ecore_list_append(list, (void *)-1);
    e_nm_device_get_device(d->ctx, dev, cb_nm_device, d);
  }
}

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
  E_NM_Data   *d;
  int ret;

  d = calloc(1, sizeof(E_NM_Data));
  d->ctx = ctx;
  d->cb_func = cb_func;
  d->data = data;

  msg = e_nm_manager_call_new("GetDevices");

  ret = e_dbus_method_call_send(ctx->conn, msg, cb_nm_object_path_list, cb_nm_devices, free_nm_object_path_list, -1, d) ? 1 : 0;
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
