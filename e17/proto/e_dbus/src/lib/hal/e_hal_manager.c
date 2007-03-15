#include <E_Hal.h>

static void cb_manager_get_all_devices(void *data, DBusMessage *msg);
static void cb_manager_device_exists(void *data, DBusMessage *msg);
static void cb_manager_find_device_string_match(void *data, DBusMessage *msg);
static void cb_manager_find_device_by_capability(void *data, DBusMessage *msg);

#define e_hal_manager_call_new(member) dbus_message_new_method_call(E_HAL_SENDER, E_HAL_MANAGER_PATH, E_HAL_MANAGER_INTERFACE, member)

/* GetAllDevices */

static void 
cb_manager_get_all_devices(void *data, DBusMessage *msg)
{
  E_Hal_Callback *cb = data;
  E_Hal_Manager_Get_All_Devices_Return *ret;
  DBusMessageIter iter, sub;

  if (!cb) return;

  ret = calloc(1, sizeof(E_Hal_Manager_Get_All_Devices_Return));
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

  cb->func(cb->user_data, ret);

error:
  if (ret->strings) ecore_list_destroy(ret->strings);
  free(ret);
  e_hal_callback_free(cb);
}

int
e_hal_manager_get_all_devices(DBusConnection *conn, E_Hal_Callback_Func cb_func, void *data)
{
  E_Hal_Callback *cb;
  DBusMessage *msg;

  cb = e_hal_callback_new(cb_func, data);
  msg = e_hal_manager_call_new("GetAllDevices");
  /* add params here (for method calls that have them) */
  return e_dbus_message_send(conn, msg, cb_manager_get_all_devices, cb_error_generic, -1, cb) ? 1 : 0;
}

/* Manager.DeviceExists(string udi) */

static void 
cb_manager_device_exists(void *data, DBusMessage *msg)
{
  E_Hal_Callback *cb = data;
  E_Hal_Manager_Device_Exists_Return *ret;
  DBusError err;
  dbus_bool_t val;

  if (!cb) return;

  ret = calloc(1, sizeof(E_Hal_Manager_Device_Exists_Return));
  if (!ret) return;

  dbus_error_init(&err);
  dbus_message_get_args(msg, &err, DBUS_TYPE_BOOLEAN, &val, DBUS_TYPE_INVALID);

  if (dbus_error_is_set(&err))
  {
    /* XXX do something with an error */
    printf("ERROR: %s,  %s!\n", err.name, err.message);
    dbus_error_free(&err);
    goto error;
  }

  ret->boolean = val;
  cb->func(cb->user_data, ret);

error:
  free(ret);
  e_hal_callback_free(cb);
}

int
e_hal_manager_device_exists(DBusConnection *conn, const char *udi, E_Hal_Callback_Func cb_func, void *data)
{
  E_Hal_Callback *cb;
  DBusMessage *msg;

  cb = e_hal_callback_new(cb_func, data);
  msg = e_hal_manager_call_new("DeviceExists");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &udi, DBUS_TYPE_INVALID);
  return e_dbus_message_send(conn, msg, cb_manager_device_exists, cb_error_generic, -1, cb) ? 1 : 0;
}

/* Manager.FindDeviceStringMatch */

static void 
cb_manager_find_device_string_match(void *data, DBusMessage *msg)
{
  E_Hal_Callback *cb = data;
  E_Hal_Manager_Find_Device_String_Match_Return *ret;
  DBusMessageIter iter, sub;

  if (!cb) return;

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

  cb->func(cb->user_data, ret);

error:
  if (ret->strings) ecore_list_destroy(ret->strings);
  free(ret);
  e_hal_callback_free(cb);
}

int
e_hal_manager_find_device_string_match(DBusConnection *conn, const char *key, const char *value, E_Hal_Callback_Func cb_func, void *data)
{
  E_Hal_Callback *cb;
  DBusMessage *msg;

  cb = e_hal_callback_new(cb_func, data);
  msg = e_hal_manager_call_new("FindDeviceStringMatch");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &key, DBUS_TYPE_STRING, &value, DBUS_TYPE_INVALID);
  return e_dbus_message_send(conn, msg, cb_manager_find_device_string_match, cb_error_generic, -1, cb) ? 1 : 0;
}

/* Manager.FindDeviceByCapability */

static void 
cb_manager_find_device_by_capability(void *data, DBusMessage *msg)
{
  E_Hal_Callback *cb = data;
  E_Hal_Manager_Find_Device_By_Capability_Return *ret;
  DBusMessageIter iter, sub;

  if (!cb) return;

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

  cb->func(cb->user_data, ret);

error:
  if (ret->strings) ecore_list_destroy(ret->strings);
  free(ret);
  e_hal_callback_free(cb);
}

int
e_hal_manager_find_device_by_capability(DBusConnection *conn, const char *capability, E_Hal_Callback_Func cb_func, void *data)
{
  E_Hal_Callback *cb;
  DBusMessage *msg;

  cb = e_hal_callback_new(cb_func, data);
  msg = e_hal_manager_call_new("FindDeviceByCapability");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &capability, DBUS_TYPE_INVALID);
  return e_dbus_message_send(conn, msg, cb_manager_find_device_by_capability, cb_error_generic, -1, cb) ? 1 : 0;
}
