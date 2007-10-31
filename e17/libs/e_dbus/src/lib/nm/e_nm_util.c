#include "E_Nm.h"
#include "e_nm_private.h"
#include "E_DBus.h"
#include <string.h>
#include <Ecore_Data.h>

/**
 * @internal
 * @brief returns an e_dbus callback for a given dbus type
 * @param rettype the return type we want to find a callback for
 **/
static E_DBus_Unmarshal_Func
e_nm_callback_by_type(int rettype)
{
  switch (rettype)
  {
    case DBUS_TYPE_STRING:
      return cb_nm_string;
      
    case DBUS_TYPE_INT32:
      return cb_nm_int32;
      
    case DBUS_TYPE_UINT32:
      return cb_nm_uint32;
      
    case DBUS_TYPE_BOOLEAN:
      return cb_nm_boolean;

    default:
      return cb_nm_generic;
  }
}

/**
 * @internal
 * @brief returns an e_dbus free for a given dbus type
 * @param rettype the return type we want to find a free for
 **/
static E_DBus_Free_Func
e_nm_free_by_type(int rettype)
{
  switch (rettype)
  {
    case DBUS_TYPE_STRING:
      return NULL;
    case DBUS_TYPE_INT32:
    case DBUS_TYPE_UINT32:
    case DBUS_TYPE_BOOLEAN:
    default:
      return free_nm_generic;
  }
}

/**
 * @internal
 * @brief Send "get" messages to NetworkManager via e_dbus
 * @param ctx an e_nm context
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 * @param method the name of the method that should be called
 * @param rettype the type of the data that will be returned to the callback
 **/
int
e_nm_get_from_nm(E_NM_Context *ctx, E_DBus_Callback_Func cb_func, void *data,
                 const char *method, int rettype)
{
  DBusMessage *msg;
  int ret;

  msg = e_nm_manager_call_new(method);
  ret = e_dbus_method_call_send(ctx->conn, msg, e_nm_callback_by_type(rettype),
                                cb_func, e_nm_free_by_type(rettype), -1, data) ? 1 : 0;
  dbus_message_unref(msg);
  return ret;
}


/**
 * @internal
 * @brief Send "get" messages to a Device via e_dbus
 * @param ctx an e_nm context
 * @param cb a callback, used when the method returns (or an error is received)
 * @param data user data to pass to the callback function
 * @param method the name of the method that should be called
 * @param rettype the type of the data that will be returned to the callback
 **/
int
e_nm_get_from_device(E_NM_Context *ctx, const char *device,
                     E_DBus_Callback_Func cb_func, void *data,
                     const char *method, int rettype)
{
  DBusMessage *msg;
  int ret;

  msg = e_nm_device_call_new(device, method);
  ret = e_dbus_method_call_send(ctx->conn, msg, e_nm_callback_by_type(rettype),
                                cb_func,
				e_nm_free_by_type(rettype),
				-1, data) ? 1 : 0;
  dbus_message_unref(msg);
  return ret;
}

/**
 * @internal
 * @brief Generic callback for methods that return nothing
 */
void *
cb_nm_generic(DBusMessage *msg, DBusError *err)
{
  return NULL;
}

/**
 * @internal
 * @brief Generic free for methods
 */
void
free_nm_generic(void *data)
{
  if (!data) return;
  free(data);
}

/**
 * @internal
 * @brief Callback for methods that return DBUS_TYPE_INT32
 */
void *
cb_nm_int32(DBusMessage *msg, DBusError *err)
{
  dbus_int32_t *i;

  i = malloc(sizeof(dbus_int32_t));
  /* Actually emit the integer */
  dbus_message_get_args(msg, err,
                        DBUS_TYPE_INT32, i,
                        DBUS_TYPE_INVALID);

  return i;
}

/**
 * @internal
 * @brief Callback for methods that return DBUS_TYPE_UINT32
 */
void *
cb_nm_uint32(DBusMessage *msg, DBusError *err)
{
  dbus_uint32_t *i;

  i = malloc(sizeof(dbus_uint32_t));
  /* Actually emit the unsigned integer */
  dbus_message_get_args(msg, err,
                        DBUS_TYPE_UINT32, i,
                        DBUS_TYPE_INVALID);

  return i;
}

/**
 * @internal
 * @brief Callback for methods that return DBUS_TYPE_BOOLEAN
 */
void *
cb_nm_boolean(DBusMessage *msg, DBusError *err)
{
  dbus_bool_t *i;

  i = malloc(sizeof(dbus_bool_t));
  /* Actually emit the unsigned integer */
  dbus_message_get_args(msg, err,
                        DBUS_TYPE_BOOLEAN, i,
                        DBUS_TYPE_INVALID);
  
  return i;
}

/**
 * @internal
 * @brief Callback for methods returning a single string
 */
void *
cb_nm_string(DBusMessage *msg, DBusError *err)
{
  char *str;

  /* Actually emit the string */
  dbus_message_get_args(msg, err,
                        DBUS_TYPE_STRING, &str,
                        DBUS_TYPE_INVALID);

  return str;
}


/**
 * @internal
 * @brief Callback for methods returning a list of strings or object paths
 */
void *
cb_nm_string_list(DBusMessage *msg, DBusError *err)
{
  Ecore_List *devices;
  DBusMessageIter iter, sub;

  dbus_message_iter_init(msg, &iter);
  if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_ARRAY ||
      dbus_message_iter_get_element_type(&iter) != DBUS_TYPE_OBJECT_PATH) return NULL;

  devices = ecore_list_new();
  dbus_message_iter_recurse(&iter, &sub);
  while (dbus_message_iter_get_arg_type(&sub) != DBUS_TYPE_INVALID)
  {
    char *dev = NULL;

    dbus_message_iter_get_basic(&sub, &dev);
    if (dev) ecore_list_append(devices, dev);
    dbus_message_iter_next(&sub);
  }

  return devices;
}

void
free_nm_string_list(void *data)
{
  Ecore_List *list = data;

  if (list) ecore_list_destroy(list);
}

