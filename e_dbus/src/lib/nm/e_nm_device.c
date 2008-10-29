/*
 * This file defines functions that query each of the functions provided by
 * the org.freedesktop.NetworkManager.Device DBus interface.
 */

#include "E_Nm.h"
#include "e_nm_private.h"
#include <Ecore_Data.h>

#include <string.h>

static void
cb_nm_device_properties(void *data, DBusMessage *msg, DBusError *err)
{
  DBusMessageIter iter, a_iter;
  E_NM_Data *d;
  E_NM_Device *device = NULL;
  DBusError new_err;

  d = data;
  if (dbus_error_is_set(err))
  {
    d->cb_func(d->data, NULL, err);
    free(d);
    return;
  }
  dbus_error_init(&new_err);
  if (!dbus_message_has_signature(msg, "a{sv}"))
  {
    dbus_set_error(&new_err, DBUS_ERROR_INVALID_SIGNATURE, "");
    goto error;
  }
  device = calloc(1, sizeof(E_NM_Device));
  if (!device)
  {
    dbus_set_error(&new_err, DBUS_ERROR_NO_MEMORY, "");
    goto error;
  }

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
    if (!strcmp(name, "Udi"))
    {
      if (!nm_check_arg_type(&v_iter, 's'))
      {
        dbus_set_error(&new_err, DBUS_ERROR_INVALID_ARGS, "Wrong arg for %s", name);
        goto error;
      }
      dbus_message_iter_get_basic(&v_iter, &value);
      device->udi = strdup(value);
    }
    else if (!strcmp(name, "Interface"))
    {
      if (!nm_check_arg_type(&v_iter, 's'))
      {
        dbus_set_error(&new_err, DBUS_ERROR_INVALID_ARGS, "Wrong arg for %s", name);
        goto error;
      }
      dbus_message_iter_get_basic(&v_iter, &value);
      device->interface = strdup(value);
    }
    else if (!strcmp(name, "Driver"))
    {
      if (!nm_check_arg_type(&v_iter, 's'))
      {
        dbus_set_error(&new_err, DBUS_ERROR_INVALID_ARGS, "Wrong arg for %s", name);
        goto error;
      }
      dbus_message_iter_get_basic(&v_iter, &value);
      device->driver = strdup(value);
    }
    else if (!strcmp(name, "Capabilities"))
    {
      if (!nm_check_arg_type(&v_iter, 'u'))
      {
        dbus_set_error(&new_err, DBUS_ERROR_INVALID_ARGS, "Wrong arg for %s", name);
        goto error;
      }
      dbus_message_iter_get_basic(&v_iter, &(device->capabilities));
    }
    else if (!strcmp(name, "Ip4Address"))
    {
      if (!nm_check_arg_type(&v_iter, 'u'))
      {
        dbus_set_error(&new_err, DBUS_ERROR_INVALID_ARGS, "Wrong arg for %s", name);
        goto error;
      }
      dbus_message_iter_get_basic(&v_iter, &(device->ip4address));
    }
    else if (!strcmp(name, "State"))
    {
      if (!nm_check_arg_type(&v_iter, 'u'))
      {
        dbus_set_error(&new_err, DBUS_ERROR_INVALID_ARGS, "Wrong arg for %s", name);
        goto error;
      }
      dbus_message_iter_get_basic(&v_iter, &(device->state));
    }
    else if (!strcmp(name, "Ip4Config"))
    {
      if (!nm_check_arg_type(&v_iter, 'o'))
      {
        dbus_set_error(&new_err, DBUS_ERROR_INVALID_ARGS, "Wrong arg for %s", name);
        goto error;
      }
      dbus_message_iter_get_basic(&v_iter, &value);
      device->ip4config = strdup(value);
    }
    else if (!strcmp(name, "Dhcp4Config"))
    {
      if (!nm_check_arg_type(&v_iter, 'o'))
      {
        dbus_set_error(&new_err, DBUS_ERROR_INVALID_ARGS, "Wrong arg for %s", name);
        goto error;
      }
      dbus_message_iter_get_basic(&v_iter, &value);
      device->dhcp4config = strdup(value);
    }
    else if (!strcmp(name, "Managed"))
    {
      if (!nm_check_arg_type(&v_iter, 'b'))
      {
        dbus_set_error(&new_err, DBUS_ERROR_INVALID_ARGS, "Wrong arg for %s", name);
        goto error;
      }
      dbus_message_iter_get_basic(&v_iter, &(device->managed));
    }
    else if (!strcmp(name, "DeviceType"))
    {
      if (!nm_check_arg_type(&v_iter, 'u'))
      {
        dbus_set_error(&new_err, DBUS_ERROR_INVALID_ARGS, "Wrong arg for %s", name);
        goto error;
      }
      dbus_message_iter_get_basic(&v_iter, &(device->device_type));
    }
    dbus_message_iter_next(&a_iter);
  }

  d->cb_func(d->data, device, &new_err);
  dbus_error_free(&new_err);
  free(d);
  return;

error:
  if (device) e_nm_device_free_device(device);
  d->cb_func(d->data, NULL, &new_err);
  dbus_error_free(&new_err);
  free(d);
}

EAPI int
e_nm_device_get_device(E_NM_Context *ctx, const char *device,
                       E_DBus_Callback_Func cb_func, void *data)
{
  E_NM_Data   *d;

  d = calloc(1, sizeof(E_NM_Data));
  d->ctx = ctx;
  d->cb_func = cb_func;
  d->data = data;

  return e_dbus_properties_get_all(ctx->conn, E_NM_SERVICE, device,
                                   E_NM_INTERFACE_DEVICE, cb_nm_device_properties,
                                   d) ? 1 : 0;
}

EAPI void
e_nm_device_free_device(E_NM_Device *device)
{
  if (!device) return;
  if (device->udi) free(device->udi);
  if (device->interface) free(device->interface);
  if (device->driver) free(device->driver);
  if (device->ip4config) free(device->ip4config);
  if (device->dhcp4config) free(device->dhcp4config);
  free(device);
}

EAPI void
e_nm_device_dump_device(E_NM_Device *device)
{
  if (!device) return;
  printf("udi         : %s\n", device->udi);
  printf("interface   : %s\n", device->interface);
  printf("driver      : %s\n", device->driver);
  printf("capabilities:");
  if (device->capabilities & E_NM_DEVICE_CAP_NM_SUPPORTED)
    printf(" E_NM_DEVICE_CAP_NM_SUPPORTED");
  if (device->capabilities & E_NM_DEVICE_CAP_CARRIER_DETECT)
    printf(" E_NM_DEVICE_CAP_CARRIER_DETECT");
  if (device->capabilities == E_NM_DEVICE_CAP_NONE)
    printf(" E_NM_DEVICE_CAP_NONE");
  printf("\n");
  printf("ip4address  : %i.%i.%i.%i\n",
         ((device->ip4address      ) & 0xff),
         ((device->ip4address >> 8 ) & 0xff),
         ((device->ip4address >> 16) & 0xff),
         ((device->ip4address >> 24) & 0xff)
         );
  printf("state       : ");
  switch (device->state)
  {
    case E_NM_DEVICE_STATE_UNKNOWN:
      printf("E_NM_DEVICE_STATE_UNKNOWN\n");
      break;
    case E_NM_DEVICE_STATE_DOWN:
      printf("E_NM_DEVICE_STATE_DOWN\n");
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
    case E_NM_DEVICE_STATE_CANCELLED:
      printf("E_NM_DEVICE_STATE_CANCELLED\n");
      break;
  }
  printf("ip4config   : %s\n", device->ip4config);
  printf("dhcp4config : %s\n", device->dhcp4config);
  printf("managed     : %d\n", device->managed);
  printf("device_type : %u\n", device->device_type);
}
