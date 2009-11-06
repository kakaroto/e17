#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>

#include "E_Nm.h"
#include "e_nm_private.h"
#include "e_dbus_private.h"

static const Property nms_properties[] = {
  { .name = "UnmanagedDevices", .sig = "ao", .offset = offsetof(E_NMS, unmanaged_devices) },
  { .name = "Hostname", .sig = "s", .offset = offsetof(E_NMS, hostname) },
  { .name = NULL }
};

static void
cb_nms_connections(void *data, void *reply, DBusError *err)
{
  Reply_Data  *d;
  E_NMS_Internal *nmsi;
  Eina_List *connections;
  Eina_List *list = NULL;
  Eina_List *l;
  const char *path;

  d = data;
  nmsi = d->object;
  if (dbus_error_is_set(err))
  {
    ERR("%s - %s", err->name, err->message);
    d->cb_func(d->data, NULL);
    free(d);
    return;
  }
  //TODO: ecore_list_free_cb_set(list, ECORE_FREE_CB(e_nms_connection_free));
  connections = reply;
  EINA_LIST_FOREACH(connections, l, path)
    list = eina_list_append(list, e_nms_connection_get(&(nmsi->nms), nmsi->nms.service_name, path));
  d->cb_func(d->data, list);
  free(d);
}

static void
cb_new_connection(void *data, DBusMessage *msg)
{
  E_NMS_Internal *nmsi;
  const char *conn;
  DBusError err;
  if (!msg || !data) return;

  nmsi = data;
  dbus_error_init(&err);
  dbus_message_get_args(msg, &err, DBUS_TYPE_OBJECT_PATH, &conn, DBUS_TYPE_INVALID);
  if (dbus_error_is_set(&err))
  {
    ERR("%s - %s", err.name, err.message);
    return;
  }

  if (nmsi->new_connection)
    nmsi->new_connection(&(nmsi->nms), nmsi->nms.service_name, conn);
}

static void
cb_properties_changed(void *data, DBusMessage *msg)
{
  E_NMS_Internal *nmsi;
  if (!msg || !data) return;

  nmsi = data;
  parse_properties(nmsi, nms_properties, msg);

  if (nmsi->properties_changed)
    nmsi->properties_changed(&(nmsi->nms));
}

static void
add_basic(DBusMessageIter *iter, E_NM_Variant *variant)
{
  switch (variant->type)
  {
    case 'a':
      ERR("No support for array of array");
      break;
    case 's':
    case 'o':
      dbus_message_iter_append_basic(iter, variant->type, &variant->s);
      break;
    case 'u':
      dbus_message_iter_append_basic(iter, variant->type, &variant->u);
      break;
    case 'b':
      dbus_message_iter_append_basic(iter, variant->type, &variant->b);
      break;
    case 'y':
      dbus_message_iter_append_basic(iter, variant->type, &variant->y);
      break;
    case 't':
      dbus_message_iter_append_basic(iter, variant->type, &variant->t);
      break;
  }
}

static void
add_variant(DBusMessageIter *iter, E_NM_Variant *variant)
{
  DBusMessageIter v_iter;
  char            sig[3];

  switch (variant->type)
  {
    case 'a': {
      E_NM_Variant    *subvar;
      DBusMessageIter  a_iter;
      Eina_List       *l;

      if (variant->a)
      {
        subvar = eina_list_data_get(variant->a);
	sig[0] = 'a';
        sig[1] = subvar->type;
        sig[2] = 0;
        dbus_message_iter_open_container(iter, DBUS_TYPE_VARIANT, sig, &v_iter);
        dbus_message_iter_open_container(&v_iter, DBUS_TYPE_ARRAY, sig, &a_iter);
        EINA_LIST_FOREACH(variant->a, l, subvar)
          add_basic(&a_iter, subvar);
        dbus_message_iter_close_container(&v_iter, &a_iter);
        dbus_message_iter_close_container(iter, &v_iter);
      }
      break;
    }
    default:
      sig[0] = variant->type;
      sig[1] = 0;

      dbus_message_iter_open_container(iter, DBUS_TYPE_VARIANT, sig, &v_iter);
      add_basic(&v_iter, variant);
      dbus_message_iter_close_container(iter, &v_iter);
      break;
  }
}

static Eina_Bool
add_value(const Eina_Hash *hash __UNUSED__, const void *key, void *data, void *fdata)
{
  DBusMessageIter *iter, d_iter;

  iter = fdata;

  dbus_message_iter_open_container(iter, DBUS_TYPE_DICT_ENTRY, NULL, &d_iter);
  dbus_message_iter_append_basic(&d_iter, DBUS_TYPE_STRING, &key);
  add_variant(&d_iter, data);
  dbus_message_iter_close_container(iter, &d_iter);

  return 1;
}

static Eina_Bool
add_array(const Eina_Hash *hash __UNUSED__, const void *key, void *data, void *fdata)
{
  DBusMessageIter *iter, d_iter, a_iter;

  iter = fdata;

  dbus_message_iter_open_container(iter, DBUS_TYPE_DICT_ENTRY, NULL, &d_iter);
  dbus_message_iter_append_basic(&d_iter, DBUS_TYPE_STRING, &key);
  dbus_message_iter_open_container(&d_iter, DBUS_TYPE_ARRAY, "{sv}", &a_iter);
  eina_hash_foreach(data, add_value, &a_iter);
  dbus_message_iter_close_container(&d_iter, &a_iter);
  dbus_message_iter_close_container(iter, &d_iter);

  return 1;
}

EAPI int
e_nms_get(E_NM *nm, E_NMS_Context context, int (*cb_func)(void *data, E_NMS *nms), void *data)
{
  E_NMS_Internal *nmsi;
  Property_Data     *d;

  nmsi = calloc(1, sizeof(E_NMS_Internal));
  nmsi->nmi = (E_NM_Internal *)nm;

  d = calloc(1, sizeof(Property_Data));
  d->nmi = nmsi->nmi;
  d->cb_func = OBJECT_CB(cb_func);
  d->data = data;
  d->reply = nmsi;
  d->property = nms_properties;
  d->object = strdup(E_NMS_PATH);
  d->interface = E_NMS_INTERFACE;
  switch (context)
  {
    case E_NMS_CONTEXT_SYSTEM:
      nmsi->nms.service_name = E_NMS_SERVICE_SYSTEM;
      nmsi->handlers = eina_list_append(nmsi->handlers, e_nms_signal_handler_add(nmsi->nmi->conn, nmsi->nms.service_name, "NewConnection", cb_new_connection, nmsi));
      nmsi->handlers = eina_list_append(nmsi->handlers, e_nms_system_signal_handler_add(nmsi->nmi->conn, nmsi->nms.service_name, "PropertiesChanged", cb_properties_changed, nmsi));
      d->service = nmsi->nms.service_name;
      //(*cb_func)(data, &(nmsi->nms));
      return property_get(nmsi->nmi->conn, d);
  }
  return 0;
}

EAPI void
e_nms_free(E_NMS *nms)
{
  E_NMS_Internal *nmsi;
  void *data;

  if (!nms) return;
  nmsi = (E_NMS_Internal *)nms;

  EINA_LIST_FREE(nms->unmanaged_devices, data)
    free(data);
  if (nms->hostname) free(nms->hostname);
  EINA_LIST_FREE(nmsi->handlers, data)
    e_dbus_signal_handler_del(nmsi->nmi->conn, data);
  free(nmsi);
}

EAPI void
e_nms_dump(E_NMS *nms)
{
  const char *dev;
  Eina_List *l;

  if (!nms) return;
  printf("E_NMS:\n");
  printf("unmanaged_devices:\n");
  EINA_LIST_FOREACH(nms->unmanaged_devices, l, dev)
    printf(" - %s\n", dev);
  printf("hostname         : %s\n", nms->hostname);
}

EAPI int
e_nms_list_connections(E_NMS *nms, int (*cb_func)(void *data, Eina_List *list), void *data)
{
  DBusMessage *msg;
  Reply_Data   *d;
  E_NMS_Internal *nmsi;
  int ret;

  nmsi = (E_NMS_Internal *)nms;
  d = calloc(1, sizeof(Reply_Data));
  d->cb_func = OBJECT_CB(cb_func);
  d->data = data;
  d->object = nmsi;

  msg = e_nms_call_new(nms->service_name, "ListConnections");

  ret = e_dbus_method_call_send(nmsi->nmi->conn, msg, cb_nm_object_path_list, cb_nms_connections, free_nm_object_path_list, -1, d) ? 1 : 0;
  dbus_message_unref(msg);
  return ret;
}

EAPI int
e_nms_system_save_hostname(E_NMS *nms, const char *hostname)
{
  DBusMessage *msg;
  E_NMS_Internal *nmsi;
  int ret;

  nmsi = (E_NMS_Internal *)nms;
  msg = e_nms_system_call_new(nms->service_name, "SaveHostname");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &hostname, DBUS_TYPE_INVALID);

  ret = e_dbus_message_send(nmsi->nmi->conn, msg, NULL, -1, NULL) ? 1 : 0;
  dbus_message_unref(msg);
  return ret;
}

static void
cb(void *data __UNUSED__, DBusMessage *msg __UNUSED__, DBusError *err)
{
  if (dbus_error_is_set(err))
  {
    ERR("%s - %s", err->name, err->message);
  }
  else
  {
    ERR("Yay!");
  }
}

EAPI int
e_nms_system_add_connection(E_NMS *nms, Eina_Hash *settings)
{
  DBusMessage     *msg;
  DBusMessageIter  iter, a_iter;
  E_NMS_Internal  *nmsi;
  int              ret;

  nmsi = (E_NMS_Internal *)nms;
  msg = e_nms_system_call_new(nms->service_name, "AddConnection");
  dbus_message_iter_init_append(msg, &iter);
  dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "{sa{sv}}", &a_iter);
  eina_hash_foreach(settings, add_array, &a_iter);
  dbus_message_iter_close_container(&iter, &a_iter);

  ret = e_dbus_message_send(nmsi->nmi->conn, msg, cb, -1, NULL) ? 1 : 0;
  dbus_message_unref(msg);
  return ret;
}

EAPI void
e_nms_data_set(E_NMS *nms, void *data)
{
  E_NMS_Internal *nmsi;

  nmsi = (E_NMS_Internal *)nms;
  nmsi->data = data;
}

EAPI void *
e_nms_data_get(E_NMS *nms)
{
  E_NMS_Internal *nmsi;

  nmsi = (E_NMS_Internal *)nms;
  return nmsi->data;
}

EAPI void
e_nms_callback_new_connection_set(E_NMS *nms, int (*cb_func)(E_NMS *nms, const char *service_name, const char *connection))
{
  E_NMS_Internal *nmsi;

  nmsi = (E_NMS_Internal *)nms;
  nmsi->new_connection = cb_func;
}

EAPI void
e_nms_system_callback_properties_changed_set(E_NMS *nms, int (*cb_func)(E_NMS *nms))
{
  E_NMS_Internal *nmsi;

  nmsi = (E_NMS_Internal *)nms;
  nmsi->properties_changed = cb_func;
}

