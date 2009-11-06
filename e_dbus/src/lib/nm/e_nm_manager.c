#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>

#include "E_Nm.h"
#include "e_nm_private.h"
#include "e_dbus_private.h"

static void
check_done(Reply_Data *d, Eina_List *list)
{
  if (!list)
  {
    d->cb_func(d->data, NULL);
    free(d);
  }
  else if (eina_list_data_get(list) != (void *)-1)
  {
    d->cb_func(d->data, list);
    free(d);
  }
}

static int
cb_nm_device(void *data, E_NM_Device *dev)
{
  Reply_Data  *d;
  Eina_List   *list;

  d = data;
  list = d->reply;
  if (dev)
    list = eina_list_append(list, dev);
  list = eina_list_remove_list(list, list);

  check_done(d, list);
  d->reply = list;
  return 1;
}

static void
cb_nm_devices(void *data, void *reply, DBusError *err)
{
  Reply_Data *d;
  E_NM       *nm;
  Eina_List  *devices;
  Eina_List  *list = NULL;
  Eina_List  *l;
  const char *dev;

  d = data;
  nm = d->object;
  if (dbus_error_is_set(err))
  {
    ERR("%s - %s", err->name, err->message);
    d->cb_func(d->data, NULL);
    free(d);
    return;
  }
  devices = reply;
  //TODO: ecore_list_free_cb_set(list, ECORE_FREE_CB(e_nm_device_free));
  EINA_LIST_FOREACH(devices, l, dev)
  {
    list = eina_list_prepend(list, (void *)-1);
    d->reply = list;
    e_nm_device_get(nm, dev, cb_nm_device, d);
  }
}

/**
 * Get all network devices.
 *
 * Returns an Eina_List of dbus object paths for network devices. This list is
 * of const char *, and is freed automatically after the callback returns.
 *
 * @param nm an e_nm context
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 **/
EAPI int
e_nm_get_devices(E_NM *nm, int (*cb_func)(void *data, Eina_List *list), void *data)
{
  DBusMessage *msg;
  Reply_Data   *d;
  E_NM_Internal *nmi;
  int ret;

  nmi = (E_NM_Internal *)nm;
  d = calloc(1, sizeof(Reply_Data));
  d->object = nmi;
  d->cb_func = OBJECT_CB(cb_func);
  d->data = data;

  msg = e_nm_call_new("GetDevices");

  ret = e_dbus_method_call_send(nmi->conn, msg, cb_nm_object_path_list, cb_nm_devices, free_nm_object_path_list, -1, d) ? 1 : 0;
  dbus_message_unref(msg);
  return ret;
}

EAPI int
e_nm_activate_connection(E_NM *nm, const char *service_name, const char *connection, E_NM_Device *device, const char *specific_object)
{
  DBusMessage *msg;
  E_NM_Internal *nmi;
  int ret;

  nmi = (E_NM_Internal *)nm;

  msg = e_nm_call_new("ActivateConnection");
  dbus_message_append_args(msg,
                           DBUS_TYPE_STRING, &service_name,
                           DBUS_TYPE_OBJECT_PATH, &connection,
                           DBUS_TYPE_OBJECT_PATH, &device->udi,
                           DBUS_TYPE_OBJECT_PATH, &specific_object,
                           DBUS_TYPE_INVALID);

  ret = e_dbus_message_send(nmi->conn, msg, NULL, -1, NULL) ? 1 : 0;
  dbus_message_unref(msg);
  return ret;
}

EAPI int
e_nm_deactivate_connection(E_NM *nm, E_NM_Active_Connection *conn)
{
  DBusMessage *msg;
  E_NM_Internal *nmi;
  int ret;

  nmi = (E_NM_Internal *)nm;

  msg = e_nm_call_new("DeactivateConnection");
  dbus_message_append_args(msg,
                           DBUS_TYPE_OBJECT_PATH, &conn->path,
                           DBUS_TYPE_INVALID);

  ret = e_dbus_message_send(nmi->conn, msg, NULL, -1, NULL) ? 1 : 0;
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

