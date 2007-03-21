#include "E_Nm.h"
#include "e_nm_private.h"
#include <string.h>
#include <Ecore_Data.h>

/**
 * @internal
 * @brief Create a callback structure
 * @param cb_func the callback function
 * @param user_data data to pass to the callback
 */
E_NM_Callback *
e_nm_callback_new(E_NM_Callback_Func cb_func, void *user_data)
{
  E_NM_Callback *cb;

  if (!cb_func) return NULL;

  cb = calloc(1, sizeof(E_NM_Callback));
  if (!cb) return NULL;
  cb->func = cb_func;
  cb->user_data = user_data;
  return cb;
}

/**
 * @internal
 * @brief Free a callback structure
 * @param callback the callback to free
 */
void
e_nm_callback_free(E_NM_Callback *callback)
{
  free(callback);
}

/**
 * @internal
 * @brief Generic callback for methods that return nothing
 */
void
cb_nm_generic(void *data, DBusMessage *msg, DBusError *err)
{
  E_NM_Callback *cb;
  DBusMessageIter iter, sub;

  cb = data;
  if (cb->func)
    cb->func(cb->user_data, NULL, err);

  if (dbus_error_is_set(err)) dbus_error_free(err);
  e_nm_callback_free(cb);
}


/**
 * @internal
 * @brief Callback for methods returning a list of strings or object paths
 */
void
cb_nm_string_list(void *data, DBusMessage *msg, DBusError *err)
{
  E_NM_Callback *cb;
  Ecore_List *devices;
  DBusMessageIter iter, sub;

  cb = data;
  if (!cb->func)
    goto out;

  if (dbus_error_is_set(err))
  {
    cb->func(cb->user_data, NULL, err);
    dbus_error_free(err);
    goto out;
  }

  devices = ecore_list_new();

  dbus_message_iter_init(msg, &iter);
  if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_ARRAY ||
      dbus_message_iter_get_element_type(&iter) != DBUS_TYPE_OBJECT_PATH) goto out;

  dbus_message_iter_recurse(&iter, &sub);
  while (dbus_message_iter_get_arg_type(&sub) != DBUS_TYPE_INVALID)
  {
    char *dev = NULL;

    dbus_message_iter_get_basic(&sub, &dev);
    if (dev) ecore_list_append(devices, dev);
    dbus_message_iter_next(&sub);
  }

  cb->func(cb->user_data, devices, err);
  ecore_list_destroy(devices);
out:
  e_nm_callback_free(cb);
  return;
}

