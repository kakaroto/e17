#include "E_Nm.h"
#include "e_nm_private.h"

#include <string.h>

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
  Ecore_List *connections;
  Ecore_List *list;
  const char *path;

  d = data;
  nmsi = d->object;
  if (dbus_error_is_set(err))
  {
    printf("Error: %s - %s\n", err->name, err->message);
    d->cb_func(d->data, NULL);
    free(d);
    return;
  }
  list = ecore_list_new();
  ecore_list_free_cb_set(list, ECORE_FREE_CB(e_nms_connection_free));
  connections = reply;
  ecore_list_first_goto(connections);
  while ((path = ecore_list_next(connections)))
    ecore_list_append(list, e_nms_connection_get(&(nmsi->nms), nmsi->nms.service_name, path));
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
    printf("Error: %s - %s\n", err.name, err.message);
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
      printf("Error: No support for array of array\n");
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

      if (!ecore_list_empty_is(variant->a))
      {
        subvar = ecore_list_first(variant->a);
	sig[0] = 'a';
        sig[1] = subvar->type;
        sig[2] = 0;
        dbus_message_iter_open_container(iter, DBUS_TYPE_VARIANT, sig, &v_iter);
        dbus_message_iter_open_container(&v_iter, DBUS_TYPE_ARRAY, sig, &a_iter);
        ecore_list_first_goto(variant->a);
        while ((subvar = ecore_list_next(variant->a)))
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

static void
add_value(void *value, void *data)
{
  Ecore_Hash_Node *node;
  E_NM_Variant    *variant;
  DBusMessageIter *iter, d_iter;

  node = value;
  iter = data;

  dbus_message_iter_open_container(iter, DBUS_TYPE_DICT_ENTRY, NULL, &d_iter);
  dbus_message_iter_append_basic(&d_iter, DBUS_TYPE_STRING, &node->key);
  add_variant(&d_iter, node->value);
  dbus_message_iter_close_container(iter, &d_iter);
}

static void
add_array(void *value, void *data)
{
  Ecore_Hash_Node *node;
  DBusMessageIter *iter, d_iter, a_iter;

  node = value;
  iter = data;

  dbus_message_iter_open_container(iter, DBUS_TYPE_DICT_ENTRY, NULL, &d_iter);
  dbus_message_iter_append_basic(&d_iter, DBUS_TYPE_STRING, &node->key);
  dbus_message_iter_open_container(&d_iter, DBUS_TYPE_ARRAY, "{sv}", &a_iter);
  ecore_hash_for_each_node(node->value, add_value, &a_iter);
  dbus_message_iter_close_container(&d_iter, &a_iter);
  dbus_message_iter_close_container(iter, &d_iter);
}

EAPI int
e_nms_get(E_NM *nm, E_NMS_Context context, int (*cb_func)(void *data, E_NMS *nms), void *data)
{
  E_NMS_Internal *nmsi;
  Property_Data     *d;

  nmsi = calloc(1, sizeof(E_NMS_Internal));
  nmsi->nmi = (E_NM_Internal *)nm;
  nmsi->handlers = ecore_list_new();

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
      ecore_list_append(nmsi->handlers, e_nms_signal_handler_add(nmsi->nmi->conn, nmsi->nms.service_name, "NewConnection", cb_new_connection, nmsi));
      ecore_list_append(nmsi->handlers, e_nms_system_signal_handler_add(nmsi->nmi->conn, nmsi->nms.service_name, "PropertiesChanged", cb_properties_changed, nmsi));
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
  if (!nms) return;
  nmsi = (E_NMS_Internal *)nms;

  if (nms->unmanaged_devices) ecore_list_destroy(nms->unmanaged_devices);
  if (nms->hostname) free(nms->hostname);
  if (nmsi->handlers)
  {
    E_DBus_Signal_Handler *sh;

    while ((sh = ecore_list_first_remove(nmsi->handlers)))
      e_dbus_signal_handler_del(nmsi->nmi->conn, sh);
    ecore_list_destroy(nmsi->handlers);
  }
  free(nmsi);
}

EAPI void
e_nms_dump(E_NMS *nms)
{
  if (!nms) return;
  printf("E_NMS:\n");
  printf("unmanaged_devices:\n");
  if (nms->unmanaged_devices)
  {
    const char *dev;

    ecore_list_first_goto(nms->unmanaged_devices);
    while ((dev = ecore_list_next(nms->unmanaged_devices)))
      printf(" - %s\n", dev);
  }
  printf("hostname         : %s\n", nms->hostname);
  printf("\n");
}

EAPI int
e_nms_list_connections(E_NMS *nms, int (*cb_func)(void *data, Ecore_List *list), void *data)
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
cb(void *data, DBusMessage *msg, DBusError *err)
{
  if (dbus_error_is_set(err))
  {
    printf("Error: %s - %s\n", err->name, err->message);
  }
  else
  {
    printf("Yay!\n");
  }
}

EAPI int
e_nms_system_add_connection(E_NMS *nms, Ecore_Hash *settings)
{
  DBusMessage     *msg;
  DBusMessageIter  iter, a_iter;
  E_NMS_Internal  *nmsi;
  int              ret;

  nmsi = (E_NMS_Internal *)nms;
  msg = e_nms_system_call_new(nms->service_name, "AddConnection");
  dbus_message_iter_init_append(msg, &iter);
  dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "{sa{sv}}", &a_iter);
  ecore_hash_for_each_node(settings, add_array, &a_iter);
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

