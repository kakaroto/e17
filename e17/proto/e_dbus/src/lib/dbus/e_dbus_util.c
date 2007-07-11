#include "E_DBus.h"

/**
 * @internal
 * @brief Create a callback structure
 * @param cb_func the callback function
 * @param user_data data to pass to the callback
 */
E_DBus_Callback *
e_dbus_callback_new(E_DBus_Callback_Func cb_func, void *user_data)
{
  E_DBus_Callback *cb;

  if (!cb_func) return NULL;

  cb = calloc(1, sizeof(E_DBus_Callback));
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
e_dbus_callback_free(E_DBus_Callback *callback)
{
  free(callback);
}

void
e_dbus_callback_call(E_DBus_Callback *cb, void *data)
{
  cb->func(cb->user_data, data);
}
