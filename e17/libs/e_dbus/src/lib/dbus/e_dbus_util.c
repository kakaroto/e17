#include "E_DBus.h"
#include "e_dbus_private.h"

/**
 * @internal
 * @brief Create a callback structure
 * @param cb_func the callback function
 * @param user_data data to pass to the callback
 */
EAPI E_DBus_Callback *
e_dbus_callback_new(E_DBus_Callback_Func cb_func, E_DBus_Unmarshal_Func unmarshal_func, E_DBus_Free_Func free_func, void *user_data)
{
  E_DBus_Callback *cb;

  if (!cb_func) return NULL;

  cb = calloc(1, sizeof(E_DBus_Callback));
  if (!cb) return NULL;
  cb->cb_func = cb_func;
  cb->unmarshal_func = unmarshal_func;
  cb->free_func = free_func;
  cb->user_data = user_data;
  return cb;
}

/**
 * @internal
 * @brief Free a callback structure
 * @param callback the callback to free
 */
EAPI void
e_dbus_callback_free(E_DBus_Callback *callback)
{
  free(callback);
}

EAPI void
e_dbus_callback_call(E_DBus_Callback *cb, void *data, DBusError *error)
{
  if (cb && cb->cb_func)
    cb->cb_func(cb->user_data, data, error);
}

EAPI void *
e_dbus_callback_unmarshal(E_DBus_Callback *cb, DBusMessage *msg, DBusError *err)
{
  if (cb && cb->unmarshal_func)
    return cb->unmarshal_func(msg, err);
  else
    return NULL;
}

EAPI void
e_dbus_callback_return_free(E_DBus_Callback *cb, void *data)
{
  if (cb && cb->free_func)
    cb->free_func(data);
}

const char *
e_dbus_basic_type_as_string(int type)
{
  switch (type)
  {
    case DBUS_TYPE_BYTE:
      return DBUS_TYPE_BYTE_AS_STRING;
    case DBUS_TYPE_BOOLEAN:
      return DBUS_TYPE_BOOLEAN_AS_STRING;
    case DBUS_TYPE_INT16:
      return DBUS_TYPE_INT16_AS_STRING;
    case DBUS_TYPE_UINT16:
      return DBUS_TYPE_UINT16_AS_STRING;
    case DBUS_TYPE_INT32:
      return DBUS_TYPE_INT32_AS_STRING;
    case DBUS_TYPE_UINT32:
      return DBUS_TYPE_UINT32_AS_STRING;
    case DBUS_TYPE_INT64:
      return DBUS_TYPE_INT64_AS_STRING;
    case DBUS_TYPE_UINT64:
      return DBUS_TYPE_UINT64_AS_STRING;
    case DBUS_TYPE_DOUBLE:
      return DBUS_TYPE_DOUBLE_AS_STRING;
    case DBUS_TYPE_STRING:
      return DBUS_TYPE_STRING_AS_STRING;
    case DBUS_TYPE_OBJECT_PATH:
      return DBUS_TYPE_OBJECT_PATH_AS_STRING;
    case DBUS_TYPE_SIGNATURE:
      return DBUS_TYPE_SIGNATURE_AS_STRING;
    default:
      return NULL;
  }
}
