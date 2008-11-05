#include "E_Nm.h"
#include "e_nm_private.h"
#include "E_DBus.h"
#include <string.h>
#include <Ecore_Data.h>

#define CHECK_SIGNATURE(msg, err, sig)                       \
  if (dbus_error_is_set((err)))                              \
  {                                                          \
    printf("Error: %s - %s\n", (err)->name, (err)->message); \
    return NULL;                                             \
  }                                                          \
                                                             \
  if (!dbus_message_has_signature((msg), (sig)))             \
  {                                                          \
    dbus_set_error((err), DBUS_ERROR_INVALID_SIGNATURE, ""); \
    return NULL;                                             \
  }

void
property_string(void *data, DBusMessage *msg, DBusError *err)
{
  DBusMessageIter iter, v_iter;
  E_NM_Data *d;
  const char *str;
  char **value;

  d = data;
  if (dbus_error_is_set(err)) goto error;
  if (!dbus_message_has_signature(msg, "v")) goto error;

  dbus_message_iter_init(msg, &iter);
  dbus_message_iter_recurse(&iter, &v_iter);
  if (!nm_check_arg_type(&v_iter, 's')) goto error;

  dbus_message_iter_get_basic(&v_iter, &str);

  value = (char **)((char *)d->reply + d->property->offset);
  *value = strdup(str);
  d->property++;
  if (d->property->name)
    e_nm_device_properties_get(d->nmi->conn, d->object, d->property->name, d->property->func, d);
  else
  {
    if (d->cb_func) d->cb_func(d->data, d->reply);
    e_nm_data_free(d);
  }
  return;

error:
  if (d->reply) free(d->reply); /* TODO: Correct free for object */
  if (d->cb_func) d->cb_func(d->data, NULL);
  e_nm_data_free(d);
}

void
property_object_path(void *data, DBusMessage *msg, DBusError *err)
{
  DBusMessageIter iter, v_iter;
  E_NM_Data *d;
  const char *str;
  char **value;

  d = data;
  if (dbus_error_is_set(err)) goto error;
  if (!dbus_message_has_signature(msg, "v")) goto error;

  dbus_message_iter_init(msg, &iter);
  dbus_message_iter_recurse(&iter, &v_iter);
  if (!nm_check_arg_type(&v_iter, 'o')) goto error;

  dbus_message_iter_get_basic(&v_iter, &str);

  value = (char **)((char *)d->reply + d->property->offset);
  *value = strdup(str);
  d->property++;
  if (d->property->name)
    e_nm_device_properties_get(d->nmi->conn, d->object, d->property->name, d->property->func, d);
  else
  {
    if (d->cb_func) d->cb_func(d->data, d->reply);
    e_nm_data_free(d);
  }
  return;

error:
  if (d->reply) free(d->reply); /* TODO: Correct free for object */
  if (d->cb_func) d->cb_func(d->data, NULL);
  e_nm_data_free(d);
}

void
property_uint32(void *data, DBusMessage *msg, DBusError *err)
{
  DBusMessageIter iter, v_iter;
  E_NM_Data *d;
  uint *value;

  d = data;
  if (dbus_error_is_set(err)) goto error;
  if (!dbus_message_has_signature(msg, "v")) goto error;

  dbus_message_iter_init(msg, &iter);
  dbus_message_iter_recurse(&iter, &v_iter);
  if (!nm_check_arg_type(&v_iter, 'u')) goto error;

  value = (uint *)((char *)d->reply + d->property->offset);
  dbus_message_iter_get_basic(&v_iter, value);
  d->property++;
  if (d->property->name)
    e_nm_device_properties_get(d->nmi->conn, d->object, d->property->name, d->property->func, d);
  else
  {
    if (d->cb_func) d->cb_func(d->data, d->reply);
    e_nm_data_free(d);
  }
  return;

error:
  if (d->reply) free(d->reply); /* TODO: Correct free for object */
  if (d->cb_func) d->cb_func(d->data, NULL);
  e_nm_data_free(d);
}

void
property_bool(void *data, DBusMessage *msg, DBusError *err)
{
  DBusMessageIter iter, v_iter;
  E_NM_Data *d;
  dbus_bool_t *value;

  d = data;
  if (dbus_error_is_set(err)) goto error;
  if (!dbus_message_has_signature(msg, "v")) goto error;

  dbus_message_iter_init(msg, &iter);
  dbus_message_iter_recurse(&iter, &v_iter);
  if (!nm_check_arg_type(&v_iter, 'b')) goto error;

  value = (uint *)((char *)d->reply + d->property->offset);
  dbus_message_iter_get_basic(&v_iter, value);
  d->property++;
  if (d->property->name)
    e_nm_device_properties_get(d->nmi->conn, d->object, d->property->name, d->property->func, d);
  else
  {
    if (d->cb_func) d->cb_func(d->data, d->reply);
    e_nm_data_free(d);
  }
  return;

error:
  if (d->reply) free(d->reply); /* TODO: Correct free for object */
  if (d->cb_func) d->cb_func(d->data, NULL);
  e_nm_data_free(d);
}

void
property_uint32_list_list(void *data, DBusMessage *msg, DBusError *err)
{
}

void
property_uint32_list(void *data, DBusMessage *msg, DBusError *err)
{
}

void
property_string_list(void *data, DBusMessage *msg, DBusError *err)
{
  DBusMessageIter iter, v_iter, a_iter;
  E_NM_Data *d;
  Ecore_List **value;

  d = data;
  if (dbus_error_is_set(err)) goto error;
  if (!dbus_message_has_signature(msg, "v")) goto error;

  dbus_message_iter_init(msg, &iter);
  dbus_message_iter_recurse(&iter, &v_iter);
  if (!nm_check_arg_type(&v_iter, 'a')) goto error;
  dbus_message_iter_recurse(&v_iter, &a_iter);
  if (!nm_check_arg_type(&a_iter, 's')) goto error;

  value = (Ecore_List **)((char *)d->reply + d->property->offset);
  *value = ecore_list_new();
  ecore_list_free_cb_set(*value, free);
  while (dbus_message_iter_get_arg_type(&a_iter) != DBUS_TYPE_INVALID)
  {
    const char *str;

    dbus_message_iter_get_basic(&a_iter, &str);
    if (str) ecore_list_append(*value, strdup(str));
    dbus_message_iter_next(&a_iter);
  }

  d->property++;
  if (d->property->name)
    e_nm_device_properties_get(d->nmi->conn, d->object, d->property->name, d->property->func, d);
  else
  {
    if (d->cb_func) d->cb_func(d->data, d->reply);
    e_nm_data_free(d);
  }
  return;

error:
  if (d->reply) free(d->reply); /* TODO: Correct free for object */
  if (d->cb_func) d->cb_func(d->data, NULL);
  e_nm_data_free(d);
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

  CHECK_SIGNATURE(msg, err, "i");

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

  CHECK_SIGNATURE(msg, err, "u");

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

  CHECK_SIGNATURE(msg, err, "b");

  i = malloc(sizeof(dbus_bool_t));
  /* Actually emit the unsigned integer */
  dbus_message_get_args(msg, err,
                        DBUS_TYPE_BOOLEAN, i,
                        DBUS_TYPE_INVALID);
  
  return i;
}

/**
 * @internal
 * @brief Callback for methods returning a single object path
 */
void *
cb_nm_object_path(DBusMessage *msg, DBusError *err)
{
  char *str;

  CHECK_SIGNATURE(msg, err, "o");

  /* Actually emit the object_path */
  dbus_message_get_args(msg, err,
                        DBUS_TYPE_OBJECT_PATH, &str,
                        DBUS_TYPE_INVALID);

  return str;
}


/**
 * @internal
 * @brief Callback for methods returning a list of object paths
 */
void *
cb_nm_object_path_list(DBusMessage *msg, DBusError *err)
{
  Ecore_List *devices;
  DBusMessageIter iter, sub;

  CHECK_SIGNATURE(msg, err, "ao");

  dbus_message_iter_init(msg, &iter);

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
free_nm_object_path_list(void *data)
{
  Ecore_List *list = data;

  if (list) ecore_list_destroy(list);
}

int
nm_check_arg_type(DBusMessageIter *iter, char type)
{
  char sig;
 
  sig = dbus_message_iter_get_arg_type(iter);
  return sig == type;
}

void
e_nm_data_free(E_NM_Data *data)
{
  if (data->object) free(data->object);
  free(data);
}

