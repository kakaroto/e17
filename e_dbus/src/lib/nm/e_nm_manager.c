#include "E_Nm.h"
#include "e_nm_private.h"

static int
cb_nm_device(void *data, void *reply)
{
  E_NM_Data   *d;
  Ecore_List  *list;

  d = data;
  list = d->reply;
  if (reply)
    ecore_list_append(list, reply);
  ecore_list_first_remove(list);

  ecore_list_first_goto(list);
  if (ecore_list_empty_is(list))
  {
    d->cb_func(d->data, NULL);
    ecore_list_destroy(list);
    free(d);
  }
  else if (ecore_list_current(list) != (void *)-1)
  {
    d->cb_func(d->data, list);
    free(d);
  }
  return 1;
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
    d->cb_func(d->data, NULL);
    free(d);
    return;
  }
  devices = reply;
  ecore_list_first_goto(devices);
  list = ecore_list_new();
  ecore_list_free_cb_set(list, ECORE_FREE_CB(e_nm_device_free));
  d->reply = list;
  while ((dev = ecore_list_next(devices)))
  {
    ecore_list_append(list, (void *)-1);
    e_nm_device_get(&(d->nmi->nm), dev, cb_nm_device, d);
  }
}

/**
 * Get all network devices.
 *
 * Returns an Ecore_List of dbus object paths for network devices. This list is
 * of const char *, and is freed automatically after the callback returns.
 *
 * @param nm an e_nm context
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 **/
EAPI int
e_nm_get_devices(E_NM *nm, int (*cb_func)(void *data, void *reply), void *data)
{
  DBusMessage *msg;
  E_NM_Data   *d;
  E_NM_Internal *nmi;
  int ret;

  nmi = (E_NM_Internal *)nm;
  d = calloc(1, sizeof(E_NM_Data));
  d->nmi = nmi;
  d->cb_func = cb_func;
  d->data = data;

  msg = e_nm_call_new("GetDevices");

  ret = e_dbus_method_call_send(nmi->conn, msg, cb_nm_object_path_list, cb_nm_devices, free_nm_object_path_list, -1, d) ? 1 : 0;
  dbus_message_unref(msg);
  return ret;
}

EAPI int
e_nm_sleep(E_NM *nm, int sleep)
{
  DBusMessage *msg;
  E_NM_Internal *nmi;
  int ret;

  nmi = (E_NM_Internal *)nm;

  msg = e_nm_call_new("Sleep");

  ret = e_dbus_message_send(nmi->conn, msg, NULL, -1, NULL) ? 1 : 0;
  dbus_message_unref(msg);
  return ret;
}

