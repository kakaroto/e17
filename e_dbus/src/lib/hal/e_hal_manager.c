#include "E_Hal.h"

#define e_hal_manager_call_new(member) dbus_message_new_method_call(E_HAL_SENDER, E_HAL_MANAGER_PATH, E_HAL_MANAGER_INTERFACE, member)

/* GetAllDevices */

static void *
unmarshal_string_list(DBusMessage *msg, DBusError *err)
{
  E_Hal_String_List_Return *ret = NULL;
  DBusMessageIter iter, sub;

  if (!dbus_message_has_signature(msg, "as")) 
  {
    dbus_set_error(err, DBUS_ERROR_INVALID_SIGNATURE, "");
    return NULL;
  }

  ret = calloc(1, sizeof(E_Hal_String_List_Return));
  if (!ret) 
  {
    dbus_set_error(err, DBUS_ERROR_NO_MEMORY, "");
    return NULL;
  }

  ret->strings = NULL;

  dbus_message_iter_init(msg, &iter);
  dbus_message_iter_recurse(&iter, &sub);
  while (dbus_message_iter_get_arg_type(&sub) != DBUS_TYPE_INVALID)
  {
    char *dev = NULL;

    dbus_message_iter_get_basic(&sub, &dev);
    if (dev) ret->strings = eina_list_append(ret->strings, dev);
    dbus_message_iter_next(&sub);
  }

  return ret;
}

static void
free_string_list(void *data)
{
  E_Hal_String_List_Return *ret = data;

  if (!ret) return;
  while (ret->strings)
    ret->strings = eina_list_remove_list(ret->strings, ret->strings);

  free(ret);
}

EAPI DBusPendingCall *
e_hal_manager_get_all_devices(E_DBus_Connection *conn, E_DBus_Callback_Func cb_func, void *data)
{
  DBusMessage *msg;
  DBusPendingCall *ret;

  msg = e_hal_manager_call_new("GetAllDevices");
  ret = e_dbus_method_call_send(conn, msg, unmarshal_string_list, cb_func, free_string_list, -1, data);
  dbus_message_unref(msg);
  return ret;
}

/* Manager.DeviceExists(string udi) */

static void *
unmarshal_manager_device_exists(DBusMessage *msg, DBusError *err)
{
  E_Hal_Manager_Device_Exists_Return *ret = NULL;
  dbus_bool_t val;

  ret = calloc(1, sizeof(E_Hal_Manager_Device_Exists_Return));
  if (!ret) 
  {
    dbus_set_error(err, DBUS_ERROR_NO_MEMORY, "");
    return NULL;
  }

  dbus_message_get_args(msg, err, DBUS_TYPE_BOOLEAN, &val, DBUS_TYPE_INVALID);

  if (dbus_error_is_set(err))
  {
    free(ret);
    return NULL;
  }

  ret->boolean = val;
  return ret;
}

static void
free_manager_device_exists(void *data)
{
  E_Hal_Manager_Device_Exists_Return *ret = data;

  if (!ret) return;
  free(ret);
}

EAPI DBusPendingCall *
e_hal_manager_device_exists(E_DBus_Connection *conn, const char *udi, E_DBus_Callback_Func cb_func, void *data)
{
  DBusPendingCall *ret;
  DBusMessage *msg;

  msg = e_hal_manager_call_new("DeviceExists");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &udi, DBUS_TYPE_INVALID);
  ret = e_dbus_method_call_send(conn, msg, unmarshal_manager_device_exists, cb_func, free_manager_device_exists, -1, data);
  dbus_message_unref(msg);
  return ret;
}

/* Manager.FindDeviceStringMatch */
EAPI DBusPendingCall *
e_hal_manager_find_device_string_match(E_DBus_Connection *conn, const char *key, const char *value, E_DBus_Callback_Func cb_func, void *data)
{
  DBusMessage *msg;
  DBusPendingCall *ret;

  msg = e_hal_manager_call_new("FindDeviceStringMatch");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &key, DBUS_TYPE_STRING, &value, DBUS_TYPE_INVALID);
  ret = e_dbus_method_call_send(conn, msg, unmarshal_string_list, cb_func, free_string_list, -1, data);
  dbus_message_unref(msg);
  return ret;
}

/* Manager.FindDeviceByCapability */

EAPI DBusPendingCall *
e_hal_manager_find_device_by_capability(E_DBus_Connection *conn, const char *capability, E_DBus_Callback_Func cb_func, void *data)
{
  DBusMessage *msg;
  DBusPendingCall *ret;

  msg = e_hal_manager_call_new("FindDeviceByCapability");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &capability, DBUS_TYPE_INVALID);
  ret = e_dbus_method_call_send(conn, msg, unmarshal_string_list, cb_func, free_string_list, -1, data);
  dbus_message_unref(msg);
  return ret;
}
