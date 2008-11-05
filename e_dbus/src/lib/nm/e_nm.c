#include "E_Nm.h"
#include "e_nm_private.h"

#include <string.h>

static Property properties[] = {
  { .name = "WirelessEnabled", .func = property_bool, .offset = offsetof(E_NM, wireless_enabled) },
  { .name = "WirelessHardwareEnabled", .func = property_bool, .offset = offsetof(E_NM, wireless_hardware_enabled) },
#if 0
  { .name = "ActiveConnections", .func = property_object_path_list, .offset = offsetof(E_NM, ...) },
#endif
  { .name = "State", .func = property_uint32, .offset = offsetof(E_NM, state) },
  { .name = NULL, .func = NULL, .offset = 0 }
};
 
static int
parse_properties(E_NM_Internal *nmi, DBusMessage *msg, DBusError *err)
{
  DBusMessageIter iter, a_iter;

  if (dbus_error_is_set(err))
    return 0;
  if (!dbus_message_has_signature(msg, "a{sv}"))
    return 0;

  dbus_message_iter_init(msg, &iter);

  dbus_message_iter_recurse(&iter, &a_iter);
  while (dbus_message_iter_get_arg_type(&a_iter) != DBUS_TYPE_INVALID)
  {
    DBusMessageIter d_iter, v_iter;
    const char *name, *value;

    dbus_message_iter_recurse(&a_iter, &d_iter);
    dbus_message_iter_get_basic(&d_iter, &name);

    dbus_message_iter_next(&d_iter);
    dbus_message_iter_recurse(&d_iter, &v_iter);
    if (!strcmp(name, "WirelessEnabled"))
    {
      if (!nm_check_arg_type(&v_iter, 'b')) return 0;
      dbus_message_iter_get_basic(&v_iter, &(nmi->nm.wireless_enabled));
    }
    else if (!strcmp(name, "WirelessHardwareEnabled"))
    {
      if (!nm_check_arg_type(&v_iter, 'b')) return 0;
      dbus_message_iter_get_basic(&v_iter, &(nmi->nm.wireless_hardware_enabled));
    }
    else if (!strcmp(name, "ActiveConnections"))
    {
      if (!nm_check_arg_type(&v_iter, 'a')) return 0;
      /* TODO */
    }
    else if (!strcmp(name, "State"))
    {
      if (!nm_check_arg_type(&v_iter, 'u')) return 0;
      dbus_message_iter_get_basic(&v_iter, &(nmi->nm.state));
    }
    dbus_message_iter_next(&a_iter);
  }
  return 1;
}

static void
cb_state_changed(void *data, DBusMessage *msg)
{
  E_NM_Internal *nmi;
  dbus_uint32_t state;
  DBusError err;
  if (!msg || !data) return;

  nmi = data;
  dbus_error_init(&err);
  dbus_message_get_args(msg, &err, DBUS_TYPE_UINT32, &state, DBUS_TYPE_INVALID);
  if (dbus_error_is_set(&err))
  {
    printf("Error: %s - %s\n", err.name, err.message);
    return;
  }

  nmi->nm.state = state;
  if (nmi->state_changed)
    nmi->state_changed(&(nmi->nm), state);
}

static void
cb_properties_changed(void *data, DBusMessage *msg)
{
  E_NM_Internal *nmi;
  if (!msg || !data) return;

  nmi = data;
  if (!parse_properties(nmi, msg, NULL)) return;

  if (nmi->properties_changed)
    nmi->properties_changed(&(nmi->nm));
}

static void
cb_device_added(void *data, DBusMessage *msg)
{
  E_NM_Internal *nmi;
  const char *device;
  DBusError err;
  if (!msg || !data) return;

  /* TODO: Return E_NM_Device */
  nmi = data;
  dbus_error_init(&err);
  dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &device, DBUS_TYPE_INVALID);
  if (dbus_error_is_set(&err))
  {
    printf("Error: %s - %s\n", err.name, err.message);
    return;
  }

  if (nmi->device_added)
    nmi->device_added(&(nmi->nm), device);
}

static void
cb_device_removed(void *data, DBusMessage *msg)
{
  E_NM_Internal *nmi;
  const char *device;
  DBusError err;
  if (!msg || !data) return;

  /* TODO: Return E_NM_Device */
  nmi = data;
  dbus_error_init(&err);
  dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &device, DBUS_TYPE_INVALID);
  if (dbus_error_is_set(&err))
  {
    printf("Error: %s - %s\n", err.name, err.message);
    return;
  }

  if (nmi->device_removed)
    nmi->device_removed(&(nmi->nm), device);
}

#if 0
static int
cb_devices(void *data, void *reply)
{
  E_NM_Internal *nmi;

  nmi = data;
  nmi->devices = reply;
  return 1;
}
#endif

EAPI int
e_nm_get(int (*cb_func)(void *data, void *reply), void *data)
{
  E_NM_Internal *nmi = NULL;
  E_NM_Data     *d = NULL;

  nmi = calloc(1, sizeof(E_NM_Internal));
  if (!nmi) goto error;
  d = calloc(1, sizeof(E_NM_Data));
  if (!d) goto error;
  d->nmi = (E_NM_Internal *)nmi;
  d->cb_func = cb_func;
  d->data = data;
  d->reply = nmi;

  nmi->conn = e_dbus_bus_get(DBUS_BUS_SYSTEM);
  if (!nmi->conn) goto error;
  nmi->handlers = ecore_list_new();
  ecore_list_append(nmi->handlers, e_nm_signal_handler_add(nmi->conn, "StateChanged", cb_state_changed, nmi));
  ecore_list_append(nmi->handlers, e_nm_signal_handler_add(nmi->conn, "PropertiesChanged", cb_properties_changed, nmi));
  ecore_list_append(nmi->handlers, e_nm_signal_handler_add(nmi->conn, "DeviceAdded", cb_device_added, nmi));
  ecore_list_append(nmi->handlers, e_nm_signal_handler_add(nmi->conn, "DeviceRemoved", cb_device_removed, nmi));

  d->property = properties;
  d->object = strdup(_E_NM_PATH);

  return e_nm_device_properties_get(nmi->conn, d->object, d->property->name, d->property->func, d) ? 1 : 0;

error:
  if (d) free(d);
  if (nmi) free(nmi);
  return 0;
}

EAPI void
e_nm_free(E_NM *nm)
{
  E_NM_Internal *nmi;

  if (!nm) return;
  nmi = (E_NM_Internal *)nm;
  if (nmi->nm.active_connections) ecore_list_destroy(nmi->nm.active_connections);
#if 0
  if (nmi->devices) ecore_list_destroy(nmi->devices);
#endif
  if (nmi->handlers)
  {
    E_DBus_Signal_Handler *sh;

    while ((sh = ecore_list_first_remove(nmi->handlers)))
      e_dbus_signal_handler_del(nmi->conn, sh);
    ecore_list_destroy(nmi->handlers);
  }
  e_dbus_connection_close(nmi->conn);
  free(nmi);
}

EAPI void
e_nm_dump(E_NM *nm)
{
  if (!nm) return;
  printf("E_NM:\n");
  printf("wireless_enabled         : %d\n", nm->wireless_enabled);
  printf("wireless_hardware_enabled: %d\n", nm->wireless_hardware_enabled);
  /* TODO: active_connections */
  printf("state                    : ");
  switch (nm->state)
  {
    case E_NM_STATE_UNKNOWN:
      printf("E_NM_STATE_UNKNOWN\n");
      break;
    case E_NM_STATE_ASLEEP:
      printf("E_NM_STATE_ASLEEP\n");
      break;
    case E_NM_STATE_CONNECTING:
      printf("E_NM_STATE_CONNECTING\n");
      break;
    case E_NM_STATE_CONNECTED:
      printf("E_NM_STATE_CONNECTED\n");
      break;
    case E_NM_STATE_DISCONNECTED:
      printf("E_NM_STATE_DISCONNECTED\n");
      break;
  }
  printf("\n");
}

EAPI void
e_nm_data_set(E_NM *nm, void *data)
{
  E_NM_Internal *nmi;

  nmi = (E_NM_Internal *)nm;
  nmi->data = data;
}

EAPI void *
e_nm_data_get(E_NM *nm)
{
  E_NM_Internal *nmi;

  nmi = (E_NM_Internal *)nm;
  return nmi->data;
}

EAPI void
e_nm_callback_state_changed_set(E_NM *nm, int (*cb_func)(E_NM *nm, E_NM_State state))
{
  E_NM_Internal *nmi;

  nmi = (E_NM_Internal *)nm;
  nmi->state_changed = cb_func;
}

EAPI void
e_nm_callback_properties_changed_set(E_NM *nm, int (*cb_func)(E_NM *nm))
{
  E_NM_Internal *nmi;

  nmi = (E_NM_Internal *)nm;
  nmi->properties_changed = cb_func;
}

EAPI void
e_nm_callback_device_added_set(E_NM *nm, int (*cb_func)(E_NM *nm, const char *device))
{
  E_NM_Internal *nmi;

  nmi = (E_NM_Internal *)nm;
  nmi->device_added = cb_func;
}

EAPI void
e_nm_callback_device_removed_set(E_NM *nm, int (*cb_func)(E_NM *nm, const char *device))
{
  E_NM_Internal *nmi;

  nmi = (E_NM_Internal *)nm;
  nmi->device_removed = cb_func;
}
