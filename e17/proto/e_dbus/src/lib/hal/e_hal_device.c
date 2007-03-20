#include <E_Hal.h>
#include "e_hal_private.h"

#define e_hal_device_call_new(udi, member) dbus_message_new_method_call(E_HAL_SENDER, udi, E_HAL_DEVICE_INTERFACE, member)
#define e_hal_device_volume_call_new(udi, member) dbus_message_new_method_call(E_HAL_SENDER, udi, E_HAL_DEVICE_VOLUME_INTERFACE, member)

static void cb_device_get_property(void *data, DBusMessage *msg, DBusError *err);
static void cb_device_get_all_properties(void *data, DBusMessage *msg, DBusError *err);
static void cb_device_query_capability(void *data, DBusMessage *msg, DBusError *err);

/* Device.GetProperty */
static void 
cb_device_get_property(void *data, DBusMessage *msg, DBusError *err)
{
  E_Hal_Callback *cb = data;
  E_Hal_Device_Get_Property_Return *ret;
  DBusMessageIter iter;
  int type;

  if (!cb) return;
  E_HAL_HANDLE_ERROR(cb, err);

  ret = calloc(1, sizeof(E_Hal_Device_Get_Property_Return));
  if (!ret) return;

  dbus_message_iter_init(msg, &iter);
  type = dbus_message_iter_get_arg_type(&iter);
  switch(type)
  {
    case 's':
      dbus_message_iter_get_basic(&iter, &(ret->val.s));
      break;
    case 'i':
      dbus_message_iter_get_basic(&iter, &(ret->val.i));
      break;
    case 'b':
      dbus_message_iter_get_basic(&iter, &(ret->val.b));
      break;
    case 'd':
      dbus_message_iter_get_basic(&iter, &(ret->val.d));
      break;
  }

  if (dbus_error_is_set(err))
  {
    /* XXX do something with an error */
    printf("ERROR: %s,  %s!\n", err->name, err->message);
    dbus_error_free(err);
    goto error;
  }

  if (cb->func)
    cb->func(cb->user_data, ret, err);

error:
  free(ret);
  e_hal_callback_free(cb);
}

int
e_hal_device_get_property(DBusConnection *conn, const char *udi, const char *property, E_Hal_Callback_Func cb_func, void *data)
{
  E_Hal_Callback *cb;
  DBusMessage *msg;

  cb = e_hal_callback_new(cb_func, data);
  msg = e_hal_device_call_new(udi, "GetProperty");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &property, DBUS_TYPE_INVALID);
  return e_dbus_message_send(conn, msg, cb_device_get_property, -1, cb) ? 1 : 0;
}

/* Device.GetAllProperties */

static void 
cb_device_get_all_properties(void *data, DBusMessage *msg, DBusError *err)
{
  E_Hal_Callback *cb = data;
  E_Hal_Device_Get_All_Properties_Return *ret;
  DBusMessageIter iter, a_iter, s_iter, v_iter;

  if (!cb) return;
  E_HAL_HANDLE_ERROR(cb, err);

  ret = calloc(1, sizeof(E_Hal_Device_Get_All_Properties_Return));
  if (!ret) return;

  ret->properties = ecore_hash_new(ecore_str_hash, ecore_str_compare);
  ecore_hash_set_free_key(ret->properties, ECORE_FREE_CB(ecore_string_release));
  ecore_hash_set_free_value(ret->properties, ECORE_FREE_CB(e_hal_property_free));


  dbus_message_iter_init(msg, &iter);
  if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_ARRAY &&
      dbus_message_iter_get_element_type(&iter) == DBUS_TYPE_DICT_ENTRY)
  {
    dbus_message_iter_recurse(&iter, &a_iter);
    while (dbus_message_iter_get_arg_type(&a_iter) != DBUS_TYPE_INVALID)
    {
      const char *name;
      E_Hal_Property *prop = calloc(1, sizeof(E_Hal_Property));
      dbus_message_iter_recurse(&a_iter, &s_iter);
      dbus_message_iter_get_basic(&s_iter, &name);
      dbus_message_iter_next(&s_iter);
      dbus_message_iter_recurse(&s_iter, &v_iter);
      
      switch(dbus_message_iter_get_arg_type(&v_iter))
      {
        case 's':
          prop->type = E_HAL_PROPERTY_TYPE_STRING;
          dbus_message_iter_get_basic(&v_iter, &(prop->val.s));
          break;
        case 'i':
          prop->type = E_HAL_PROPERTY_TYPE_INT;
          dbus_message_iter_get_basic(&v_iter, &(prop->val.i));
          break;
        case 't':
          prop->type = E_HAL_PROPERTY_TYPE_UINT64;
          dbus_message_iter_get_basic(&v_iter, &(prop->val.u64));
          break;
        case 'b':
          prop->type = E_HAL_PROPERTY_TYPE_BOOL;
          dbus_message_iter_get_basic(&v_iter, &(prop->val.b));
          break;
        case 'd':
          prop->type = E_HAL_PROPERTY_TYPE_DOUBLE;
          dbus_message_iter_get_basic(&v_iter, &(prop->val.d));
          break;
        case 'a':
          prop->type = E_HAL_PROPERTY_TYPE_STRLIST;
          {
            DBusMessageIter list_iter;
            prop->val.strlist = ecore_list_new();
            dbus_message_iter_recurse(&v_iter, &list_iter);
            while (dbus_message_iter_get_arg_type(&list_iter) != DBUS_TYPE_INVALID)
            {
              char *str;
              dbus_message_iter_get_basic(&list_iter, &str);
              ecore_list_append(prop->val.strlist, str);
              dbus_message_iter_next(&list_iter);
            }
          }
          break;
        default:
          printf("Error: unexpected property type (%s): %c\n", name, dbus_message_iter_get_arg_type(&v_iter));
          break;
      }
      ecore_hash_set(ret->properties, (void *)ecore_string_instance(name), prop);


      dbus_message_iter_next(&a_iter);
    }
  }
  else printf("error, wrong type\n");

  if (dbus_error_is_set(err))
  {
    /* XXX do something with an error */
    printf("ERROR: %s,  %s!\n", err->name, err->message);
    dbus_error_free(err);
    goto error;
  }

  if (cb->func)
    cb->func(cb->user_data, ret, err);

error:
  ecore_hash_destroy(ret->properties);
  free(ret);
  e_hal_callback_free(cb);
}

int
e_hal_device_get_all_properties(DBusConnection *conn, const char *udi, E_Hal_Callback_Func cb_func, void *data)
{
  E_Hal_Callback *cb;
  DBusMessage *msg;

  cb = e_hal_callback_new(cb_func, data);
  msg = e_hal_device_call_new(udi, "GetAllProperties");
  return e_dbus_message_send(conn, msg, cb_device_get_all_properties, -1, cb) ? 1 : 0;
}



/* bool Device.QueryCapability(string udi) */

static void 
cb_device_query_capability(void *data, DBusMessage *msg, DBusError *err)
{
  E_Hal_Callback *cb = data;
  E_Hal_Device_Query_Capability_Return *ret;
  dbus_bool_t val;

  if (!cb) return;
  E_HAL_HANDLE_ERROR(cb, err);

  ret = calloc(1, sizeof(E_Hal_Device_Query_Capability_Return));
  if (!ret) return;

  dbus_message_get_args(msg, err, DBUS_TYPE_BOOLEAN, &val, DBUS_TYPE_INVALID);

  if (dbus_error_is_set(err))
  {
    /* XXX do something with an error */
    printf("ERROR: %s,  %s!\n", err->name, err->message);
    dbus_error_free(err);
    goto error;
  }

  ret->boolean = val;
  if (cb->func)
    cb->func(cb->user_data, ret, err);

error:
  free(ret);
  e_hal_callback_free(cb);
}

int
e_hal_device_query_capability(DBusConnection *conn, const char *udi, const char *capability, E_Hal_Callback_Func cb_func, void *data)
{
  E_Hal_Callback *cb;
  DBusMessage *msg;

  cb = e_hal_callback_new(cb_func, data);
  msg = e_hal_device_call_new(udi, "QueryCapability");
  dbus_message_append_args(msg, DBUS_TYPE_STRING, &capability, DBUS_TYPE_INVALID);
  return e_dbus_message_send(conn, msg, cb_device_query_capability, -1, cb) ? 1 : 0;
}



/* void Device.Mount(string mount_point, string fstype, array{string}options) */

static void 
cb_device_volume_mount(void *data, DBusMessage *msg, DBusError *err)
{
  E_Hal_Callback *cb = data;

  if (!cb) return;
  E_HAL_HANDLE_ERROR(cb, err);
  if (cb->func)
    cb->func(cb->user_data, NULL, err);

error:
  e_hal_callback_free(cb);
}

/**
 * @brief Mount a Volume
 *
 * @param conn the DBusConnection
 * @param udi the udi of the device object
 * @param mount_point the path to mount to, or null for default
 * @param fstype the fstype of the device (e.g. volume.fstype property)
 * @param options a list of additional options (not sure... fstype dependant?)
 */
int
e_hal_device_volume_mount(DBusConnection *conn, const char *udi, const char *mount_point, const char *fstype, Ecore_List *options, E_Hal_Callback_Func cb_func, void *data)
{
  E_Hal_Callback *cb;
  DBusMessage *msg;
  DBusMessageIter iter, subiter;

  cb = e_hal_callback_new(cb_func, data);
  msg = e_hal_device_volume_call_new(udi, "Mount");

  dbus_message_iter_init_append(msg, &iter);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &mount_point);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &fstype);
  dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "s", &subiter);

  if (options)
  {
    const char *opt;
    ecore_list_goto_first(options);
    while ((opt = ecore_list_next(options)))
    {
      dbus_message_iter_append_basic(&subiter, DBUS_TYPE_STRING, &opt);
    }
  }
  dbus_message_iter_close_container(&iter, &subiter) ;

  return e_dbus_message_send(conn, msg, cb_device_volume_mount, -1, cb) ? 1 : 0;
}

/* void Unmount(array{string} options) */

static void 
cb_device_volume_unmount(void *data, DBusMessage *msg, DBusError *err)
{
  E_Hal_Callback *cb = data;

  if (!cb) return;
  E_HAL_HANDLE_ERROR(cb, err);

  if (cb->func)
    cb->func(cb->user_data, NULL, err);

error:
  e_hal_callback_free(cb);
}

/**
 * @brief Mount a Volume
 *
 * @param conn the DBusConnection
 * @param udi the udi of the device object
 * @param options a list of additional options (not sure... fstype dependant?)
 */
int
e_hal_device_volume_unmount(DBusConnection *conn, const char *udi, Ecore_List *options, E_Hal_Callback_Func cb_func, void *data)
{
  E_Hal_Callback *cb;
  DBusMessage *msg;
  DBusMessageIter iter, subiter;

  cb = e_hal_callback_new(cb_func, data);
  msg = e_hal_device_volume_call_new(udi, "Unmount");

  dbus_message_iter_init_append(msg, &iter);
  dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "s", &subiter);
  if (options)
  {
    const char *opt;
    ecore_list_goto_first(options);
    while ((opt = ecore_list_next(options)))
    {
      dbus_message_iter_append_basic(&subiter, DBUS_TYPE_STRING, &opt);
    }
  }
  dbus_message_iter_close_container(&iter, &subiter) ;

  return e_dbus_message_send(conn, msg, cb_device_volume_unmount, -1, cb) ? 1 : 0;
}
