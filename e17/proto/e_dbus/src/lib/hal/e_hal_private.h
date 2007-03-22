#ifndef E_HAL_PRIVATE_H
#define E_HAL_PRIVATE_H

#define E_HAL_HANDLE_ERROR(cb, err) \
  if (dbus_error_is_set(err)) \
  { \
    if (cb->func) \
      cb->func(cb->user_data, NULL, err); \
    dbus_error_free(err); \
    goto error; \
  } 
#endif
