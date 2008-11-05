/*
 * This file defines functions that query each of the functions provided by
 * the org.freedesktop.NetworkManager.Device DBus interface.
 */

#include "E_Nm.h"
#include "e_nm_private.h"

#include <string.h>

static void property_device_type(void *data, DBusMessage *msg, DBusError *err);
#if 0
static void property_ip4_config(void *data, DBusMessage *msg, DBusError *err);
#endif

static Property device_wired_properties[] = {
  { .name = "HwAddress", .func = property_string, .offset = offsetof(E_NM_Device, wired.hw_address) },
  { .name = "Speed", .func = property_uint32, .offset = offsetof(E_NM_Device, wired.speed) },
  { .name = "Carrier", .func = property_bool, .offset = offsetof(E_NM_Device, wired.carrier) },
  { .name = NULL, .func = NULL, .offset = 0 }
};

static Property device_wireless_properties[] = {
  { .name = "HwAddress", .func = property_string, .offset = offsetof(E_NM_Device, wireless.hw_address) },
  { .name = "Mode", .func = property_uint32, .offset = offsetof(E_NM_Device, wireless.mode) },
  { .name = "Bitrate", .func = property_uint32, .offset = offsetof(E_NM_Device, wireless.bitrate) },
  { .name = "ActiveAccessPoint", .func = property_object_path, .offset = offsetof(E_NM_Device, wireless.active_access_point) },
  { .name = "WirelessCapabilities", .func = property_uint32, .offset = offsetof(E_NM_Device, wireless.wireless_capabilities) },
  { .name = NULL, .func = NULL, .offset = 0 }
};

static Property device_properties[] = {
  { .name = "Udi", .func = property_string, .offset = offsetof(E_NM_Device, udi) },
  { .name = "Interface", .func = property_string, .offset = offsetof(E_NM_Device, interface) },
  { .name = "Driver", .func = property_string, .offset = offsetof(E_NM_Device, driver) },
  { .name = "Capabilities", .func = property_uint32, .offset = offsetof(E_NM_Device, capabilities) },
  { .name = "Ip4Address", .func = property_uint32, .offset = offsetof(E_NM_Device, ip4_address) },
  { .name = "State", .func = property_uint32, .offset = offsetof(E_NM_Device, state) },
  { .name = "Ip4Config", .func = property_object_path, .offset = offsetof(E_NM_Device, ip4_config) },
  { .name = "Dhcp4Config", .func = property_object_path, .offset = offsetof(E_NM_Device, dhcp4_config) },
  { .name = "Managed", .func = property_bool, .offset = offsetof(E_NM_Device, managed) },
  { .name = "DeviceType", .func = property_device_type, .offset = offsetof(E_NM_Device, device_type) },
  { .name = NULL, .func = NULL, .offset = 0 }
};

#if 0
static void
property_ip4_config(void *data, DBusMessage *msg, DBusError *err)
{
  DBusMessageIter iter, v_iter;
  E_NM_Data *d;
  E_NM_Device *device;
  const char *str;

  d = data;
  if (dbus_error_is_set(err)) goto error;
  if (!dbus_message_has_signature(msg, "v")) goto error;

  dbus_message_iter_init(msg, &iter);
  dbus_message_iter_recurse(&iter, &v_iter);
  if (!nm_check_arg_type(&v_iter, 'o')) goto error;

  device = d->reply;
  dbus_message_iter_get_basic(&v_iter, &str);
  device->ip4_config = strdup(str);
  e_nm_ip4_config_get(&(d->nmi->nm), str, NULL, NULL);

  d->property++;
  if (d->property->name)
    e_nm_device_properties_get(d->nmi->conn, d->object, d->property->name, d->property->func, d);
  else
  {
    if (d->cb_func) d->cb_func(d->data, d->reply);
    e_nm_data_free(d);
  }
  return;

error:
  if (d->reply) free(d->reply); /* TODO: Correct free for object */
  if (d->cb_func) d->cb_func(d->data, NULL);
  e_nm_data_free(d);
}
#endif

static void
property_device_type(void *data, DBusMessage *msg, DBusError *err)
{
  DBusMessageIter iter, v_iter;
  E_NM_Data *d;
  E_NM_Device *device;

  d = data;
  if (dbus_error_is_set(err)) goto error;
  if (!dbus_message_has_signature(msg, "v")) goto error;

  dbus_message_iter_init(msg, &iter);
  dbus_message_iter_recurse(&iter, &v_iter);
  if (!nm_check_arg_type(&v_iter, 'u')) goto error;

  device = d->reply;
  dbus_message_iter_get_basic(&v_iter, &(device->device_type));
  switch (device->device_type)
  {
    case E_NM_DEVICE_TYPE_WIRED:
      d->property = device_wired_properties;
      e_nm_device_properties_get(d->nmi->conn, d->object, d->property->name, d->property->func, d);
      break;
    case E_NM_DEVICE_TYPE_WIRELESS:
      d->property = device_wireless_properties;
      e_nm_device_properties_get(d->nmi->conn, d->object, d->property->name, d->property->func, d);
      break;
    default:
      if (d->cb_func) d->cb_func(d->data, device);
      e_nm_data_free(d);
      break;
  }
  return;
 
error:
  if (d->reply) e_nm_device_free(d->reply);
  if (d->cb_func) d->cb_func(d->data, NULL);
  e_nm_data_free(d);
}

static int
parse_properties(E_NM_Device_Internal *dev, DBusMessage *msg, DBusError *err)
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
    /* TODO */
    dbus_message_iter_next(&a_iter);
  }
  return 1;
}

static void
cb_state_changed(void *data, DBusMessage *msg)
{
  E_NM_Device_Internal *dev;
  dbus_uint32_t state;
  DBusError err;
  if (!msg || !data) return;

  dev = data;
  dbus_error_init(&err);
  dbus_message_get_args(msg, &err, DBUS_TYPE_UINT32, &state, DBUS_TYPE_INVALID);
  if (dbus_error_is_set(&err))
  {
    printf("Error: %s - %s\n", err.name, err.message);
    return;
  }

  dev->dev.state = state;
  if (dev->state_changed)
    dev->state_changed(&(dev->dev), state);
}

static void
cb_properties_changed(void *data, DBusMessage *msg)
{
  E_NM_Device_Internal *dev;
  if (!msg || !data) return;

  dev = data;
  if (!parse_properties(dev, msg, NULL)) return;

  if (dev->properties_changed)
    dev->properties_changed(&(dev->dev));
}

EAPI int
e_nm_device_get(E_NM *nm, const char *device,
                int (*cb_func)(void *data, void *reply),
                void *data)
{
  E_NM_Internal *nmi;
  E_NM_Device_Internal *dev;
  E_NM_Data     *d;

  nmi = (E_NM_Internal *)nm;
  dev = calloc(1, sizeof(E_NM_Device_Internal));
  dev->nmi = nmi;
  d = calloc(1, sizeof(E_NM_Data));
  d->nmi = nmi;
  d->cb_func = cb_func;
  d->data = data;
  d->reply = dev;
  d->property = device_properties;
  d->object = strdup(device);

  dev->handlers = ecore_list_new();
  ecore_list_append(dev->handlers, e_nm_device_signal_handler_add(nmi->conn, device, "StateChanged", cb_state_changed, nmi));
  ecore_list_append(dev->handlers, e_nm_device_signal_handler_add(nmi->conn, device, "PropertiesChanged", cb_properties_changed, nmi));
 
  return e_nm_device_properties_get(nmi->conn, d->object, d->property->name, d->property->func, d) ? 1 : 0;
}

EAPI void
e_nm_device_free(E_NM_Device *device)
{
  E_NM_Device_Internal *dev;

  if (!device) return;
  dev = (E_NM_Device_Internal *)device;
  if (dev->dev.udi) free(dev->dev.udi);
  if (dev->dev.interface) free(dev->dev.interface);
  if (dev->dev.driver) free(dev->dev.driver);
  if (dev->dev.ip4_config) free(dev->dev.ip4_config);
  if (dev->dev.dhcp4_config) free(dev->dev.dhcp4_config);
  switch (dev->dev.device_type)
  {
    case E_NM_DEVICE_TYPE_WIRED:
      if (dev->dev.wired.hw_address) free(dev->dev.wired.hw_address);
      break;
    case E_NM_DEVICE_TYPE_WIRELESS:
      if (dev->dev.wireless.hw_address) free(dev->dev.wireless.hw_address);
      if (dev->dev.wireless.active_access_point) free(dev->dev.wireless.active_access_point);
      break;
  }
  if (dev->handlers)
  {
    E_DBus_Signal_Handler *sh;

    while ((sh = ecore_list_first_remove(dev->handlers)))
      e_dbus_signal_handler_del(dev->nmi->conn, sh);
    ecore_list_destroy(dev->nmi->handlers);
  }
  free(dev);
}

EAPI void
e_nm_device_dump(E_NM_Device *dev)
{
  if (!dev) return;
  printf("E_NM_Device:\n");
  printf("udi                  : %s\n", dev->udi);
  printf("interface            : %s\n", dev->interface);
  printf("driver               : %s\n", dev->driver);
  printf("capabilities         :");
  if (dev->capabilities & E_NM_DEVICE_CAP_NM_SUPPORTED)
    printf(" E_NM_DEVICE_CAP_NM_SUPPORTED");
  if (dev->capabilities & E_NM_DEVICE_CAP_CARRIER_DETECT)
    printf(" E_NM_DEVICE_CAP_CARRIER_DETECT");
  if (dev->capabilities == E_NM_DEVICE_CAP_NONE)
    printf(" E_NM_DEVICE_CAP_NONE");
  printf("\n");
  printf("ip4_address          : %i.%i.%i.%i\n",
         ((dev->ip4_address      ) & 0xff),
         ((dev->ip4_address >> 8 ) & 0xff),
         ((dev->ip4_address >> 16) & 0xff),
         ((dev->ip4_address >> 24) & 0xff)
         );
  printf("state                : ");
  switch (dev->state)
  {
    case E_NM_DEVICE_STATE_UNKNOWN:
      printf("E_NM_DEVICE_STATE_UNKNOWN\n");
      break;
    case E_NM_DEVICE_STATE_UNMANAGED:
      printf("E_NM_DEVICE_STATE_UNMANAGED\n");
      break;
    case E_NM_DEVICE_STATE_UNAVAILABLE:
      printf("E_NM_DEVICE_STATE_UNAVAILABLE\n");
      break;
    case E_NM_DEVICE_STATE_DISCONNECTED:
      printf("E_NM_DEVICE_STATE_DISCONNECTED\n");
      break;
    case E_NM_DEVICE_STATE_PREPARE:
      printf("E_NM_DEVICE_STATE_PREPARE\n");
      break;
    case E_NM_DEVICE_STATE_CONFIG:
      printf("E_NM_DEVICE_STATE_CONFIG\n");
      break;
    case E_NM_DEVICE_STATE_NEED_AUTH:
      printf("E_NM_DEVICE_STATE_NEED_AUTH\n");
      break;
    case E_NM_DEVICE_STATE_IP_CONFIG:
      printf("E_NM_DEVICE_STATE_IP_CONFIG\n");
      break;
    case E_NM_DEVICE_STATE_ACTIVATED:
      printf("E_NM_DEVICE_STATE_ACTIVATED\n");
      break;
    case E_NM_DEVICE_STATE_FAILED:
      printf("E_NM_DEVICE_STATE_FAILED\n");
      break;
  }
  printf("ip4_config           : %s\n", dev->ip4_config);
  printf("dhcp4_config         : %s\n", dev->dhcp4_config);
  printf("managed              : %d\n", dev->managed);
  printf("device_type          : %u\n", dev->device_type);
  switch (dev->device_type)
  {
    case E_NM_DEVICE_TYPE_WIRED:
      printf("hw_address           : %s\n", dev->wired.hw_address);
      printf("speed                : %u\n", dev->wired.speed);
      printf("carrier              : %d\n", dev->wired.carrier);
      break;
    case E_NM_DEVICE_TYPE_WIRELESS:
      printf("hw_address           : %s\n", dev->wireless.hw_address);
      printf("mode                 : ");
      switch (dev->wireless.mode)
      {
        case E_NM_802_11_MODE_UNKNOWN:
          printf("E_NM_802_11_MODE_UNKNOWN\n");
          break;
        case E_NM_802_11_MODE_ADHOC:
          printf("E_NM_802_11_MODE_ADHOC\n");
          break;
        case E_NM_802_11_MODE_INFRA:
          printf("E_NM_802_11_MODE_INFRA\n");
          break;
      }
      printf("bitrate              : %u\n", dev->wireless.bitrate);
      printf("active_access_point  : %s\n", dev->wireless.active_access_point);
      printf("wireless_capabilities:");
      if (dev->wireless.wireless_capabilities & E_NM_802_11_DEVICE_CAP_CIPHER_WEP40)
        printf(" E_NM_802_11_DEVICE_CAP_CIPHER_WEP40");
      if (dev->wireless.wireless_capabilities & E_NM_802_11_DEVICE_CAP_CIPHER_WEP104)
        printf(" E_NM_802_11_DEVICE_CAP_CIPHER_WEP104");
      if (dev->wireless.wireless_capabilities & E_NM_802_11_DEVICE_CAP_CIPHER_TKIP)
        printf(" E_NM_802_11_DEVICE_CAP_CIPHER_TKIP");
      if (dev->wireless.wireless_capabilities & E_NM_802_11_DEVICE_CAP_CIPHER_CCMP)
        printf(" E_NM_802_11_DEVICE_CAP_CIPHER_CCMP");
      if (dev->wireless.wireless_capabilities & E_NM_802_11_DEVICE_CAP_WPA)
        printf(" E_NM_802_11_DEVICE_CAP_WPA");
      if (dev->wireless.wireless_capabilities & E_NM_802_11_DEVICE_CAP_RSN)
        printf(" E_NM_802_11_DEVICE_CAP_RSN");
      if (dev->wireless.wireless_capabilities == E_NM_802_11_DEVICE_CAP_NONE)
        printf(" E_NM_802_11_DEVICE_CAP_NONE");
      printf("\n");
      break;
  }
  printf("\n");
}

EAPI void
e_nm_device_data_set(E_NM_Device *device, void *data)
{
  E_NM_Device_Internal *dev;

  dev = (E_NM_Device_Internal *)device;
  dev->data = data;
}

EAPI void *
e_nm_device_data_get(E_NM_Device *device)
{
  E_NM_Device_Internal *dev;

  dev = (E_NM_Device_Internal *)device;
  return dev->data;
}

EAPI void
e_nm_device_callback_state_changed_set(E_NM_Device *device, int (*cb_func)(E_NM_Device *device, E_NM_State state))
{
  E_NM_Device_Internal *dev;

  dev = (E_NM_Device_Internal *)device;
  dev->state_changed = cb_func;
}

EAPI void
e_nm_device_callback_properties_changed_set(E_NM_Device *device, int (*cb_func)(E_NM_Device *dev))
{
  E_NM_Device_Internal *dev;

  dev = (E_NM_Device_Internal *)device;
  dev->properties_changed = cb_func;
}
