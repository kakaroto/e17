#include "E_Hal.h"
#include "e_hal_private.h"

static void cb_manager_get_all_devices(void *data, DBusMessage *msg, DBusError *err);
static void cb_manager_device_exists(void *data, DBusMessage *msg, DBusError *err);
static void cb_manager_find_device_string_match(void *data, DBusMessage *msg, DBusError *err);
static void cb_manager_find_device_by_capability(void *data, DBusMessage *msg, DBusError *err);

#define e_hal_manager_call_new(member) dbus_message_new_method_call(E_HAL_SENDER, E_HAL_MANAGER_PATH, E_HAL_MANAGER_INTERFACE, member)



/* GetAllDevices */

static void 
cb_manager_get_all_devices(void *data, DBusMessage *msg, DBusError *err)
{
  E_Hal_Callback *cb = data;
  E_Hal_Manager_Get_All_Devices_Return *ret;
  DBusMessageIter iter, sub;

  if (!cb) return;

  E_HAL_HANDLE_ERROR(cb, err);

  ret = calloc(1, sizeof(E_Hal_Manager_Get_All_Devices_Return));
  if (!ret) goto error;
  ret->strings = ecore_list_new();
  if (!ret->strings) goto error;

  dbus_message_iter_init(msg, &iter);

  if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_ARRAY ||
      dbus_message_iter_get_element_type(&iter) != DBUS_TYPE_STRING) goto error;

  dbus_message_iter_recurse(&iter, &sub);
  while (dbus_message_iter_get_arg_type(&sub) != DBUS_TYPE_INVALID)
  {
    char *dev = NULL;

    dbus_message_iter_get_basic(&sub, &dev);
    if (dev) ecore_list_append(ret->strings, dev);
    dbus_message_iter_next(&sub);
  }

  cb->func(cb->user_data, ret, err);

error:
  if (ret->strings) ecore_list_destroy(ret->strings);
  free(ret);
  e_hal_callback_free(cb);
}

int
e_hal_manager_get_all_devices(E_DBus_Connection *conn, E_Hal_Callback_Func cb_func, void *data)
{
  E_Hal_Callback *cb;
  DBusMessage *msg;
  int ret;

  if (!cb_func) return 0;
  cb = e_hal_callback_new(cb_func, data);
  msg = e_hal_manager_call_new("GetAllDevices");
  /* add params here (for method calls that have them) */
  ret = e_dbus_message_send(conn, msg, cb_manager_get_all_devices, -1, cb) ? 1 : 0;
  dbus_message_unref(msg);
  return ret;
}

/* Manager.DeviceExists(string udi) */

static void 
cb_manager_device_exists(void *data, DBusMessage *msg, DBusError *err)
{
  E_Hal_Callback *cb = data;
  E_Hal_Manager_Device_Exists_Return *ret;
  dbus_bool_t val;

  if (!cb) return;
  E_HAL_HANDLE_ERROR(cb, err);

  ret = calloc(1, sizeof(E_Hal_Manager_Device_Exists_Return));
  if (!ret) return;

  dbus_message_get_args(msg, err, DBUS_TYPE_BOOLEAN, &val, DBUS_TYPE_INVALID);

  if (dbus_error_is_set(err))
  {
    if (cb->func)
      cb->func(cb->user_data, NULL, err);
    dbus_error_free(err);
    goto error;
  }

  ret->boolean = val;
  cb->func(cb->user_data, ret, err);

error:
  free(ret);
  e_hal_callback_free(cb);
}

int
e_hal_manager_device_exists(E_DBus_Connection *conn, const char *udi, E_Hal_Callback_Func cb_func, void *data)
{
  E_Hal_Callback *cb;
  int ret;
  DBusMessage *msg;

  cb = e_hal_callback_new(cb_func, data);
  msg = e_hal_manager_call_new("DeviceExists");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &udi, DBUS_TYPE_INVALID);
  ret = e_dbus_message_send(conn, msg, cb_manager_device_exists, -1, cb) ? 1 : 0;
  dbus_message_unref(msg);
  return ret;
}

/* Manager.FindDeviceStringMatch */

static void 
cb_manager_find_device_string_match(void *data, DBusMessage *msg, DBusError *err)
{
  E_Hal_Callback *cb = data;
  E_Hal_Manager_Find_Device_String_Match_Return *ret;
  DBusMessageIter iter, sub;

  if (!cb) return;
  E_HAL_HANDLE_ERROR(cb, err);

  ret = calloc(1, sizeof(E_Hal_Manager_Find_Device_String_Match_Return));
  if (!ret) return;
  ret->strings = ecore_list_new();
  if (!ret->strings) goto error;

  dbus_message_iter_init(msg, &iter);

  if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_ARRAY ||
      dbus_message_iter_get_element_type(&iter) != DBUS_TYPE_STRING) goto error;

  dbus_message_iter_recurse(&iter, &sub);
  while (dbus_message_iter_get_arg_type(&sub) != DBUS_TYPE_INVALID)
  {
    char *dev = NULL;

    dbus_message_iter_get_basic(&sub, &dev);
    if (dev) ecore_list_append(ret->strings, dev);
    dbus_message_iter_next(&sub);
  }

  cb->func(cb->user_data, ret, err);

error:
  if (ret->strings) ecore_list_destroy(ret->strings);
  free(ret);
  e_hal_callback_free(cb);
}

int
e_hal_manager_find_device_string_match(E_DBus_Connection *conn, const char *key, const char *value, E_Hal_Callback_Func cb_func, void *data)
{
  E_Hal_Callback *cb;
  DBusMessage *msg;
  int ret;

  cb = e_hal_callback_new(cb_func, data);
  msg = e_hal_manager_call_new("FindDeviceStringMatch");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &key, DBUS_TYPE_STRING, &value, DBUS_TYPE_INVALID);
  ret = e_dbus_message_send(conn, msg, cb_manager_find_device_string_match, -1, cb) ? 1 : 0;
  dbus_message_unref(msg);
  return ret;
}

/* Manager.FindDeviceByCapability */

static void 
cb_manager_find_device_by_capability(void *data, DBusMessage *msg, DBusError *err)
{
  E_Hal_Callback *cb = data;
  E_Hal_Manager_Find_Device_By_Capability_Return *ret;
  DBusMessageIter iter, sub;

  if (!cb) return;
  E_HAL_HANDLE_ERROR(cb, err);

  ret = calloc(1, sizeof(E_Hal_Manager_Find_Device_By_Capability_Return));
  if (!ret) return;
  ret->strings = ecore_list_new();
  if (!ret->strings) goto error;

  dbus_message_iter_init(msg, &iter);

  if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_ARRAY ||
      dbus_message_iter_get_element_type(&iter) != DBUS_TYPE_STRING) goto error;

  dbus_message_iter_recurse(&iter, &sub);
  while (dbus_message_iter_get_arg_type(&sub) != DBUS_TYPE_INVALID)
  {
    char *dev = NULL;

    dbus_message_iter_get_basic(&sub, &dev);
    if (dev) ecore_list_append(ret->strings, dev);
    dbus_message_iter_next(&sub);
  }

  cb->func(cb->user_data, ret, err);

error:
  if (ret->strings) ecore_list_destroy(ret->strings);
  free(ret);
  e_hal_callback_free(cb);
}

int
e_hal_manager_find_device_by_capability(E_DBus_Connection *conn, const char *capability, E_Hal_Callback_Func cb_func, void *data)
{
  E_Hal_Callback *cb;
  DBusMessage *msg;
  int ret;

  cb = e_hal_callback_new(cb_func, data);
  msg = e_hal_manager_call_new("FindDeviceByCapability");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &capability, DBUS_TYPE_INVALID);
  ret = e_dbus_message_send(conn, msg, cb_manager_find_device_by_capability, -1, cb) ? 1 : 0;
  dbus_message_unref(msg);
  return ret;
}
