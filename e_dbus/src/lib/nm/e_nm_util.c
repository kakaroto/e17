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

static void property_string(DBusMessageIter *iter, void *value);
static void property_object_path(DBusMessageIter *iter, void *value);
static void property_uint32(DBusMessageIter *iter, void *value);
static void property_bool(DBusMessageIter *iter, void *value);
static void property_uchar(DBusMessageIter *iter, void *value);
static void property_string_list(DBusMessageIter *iter, void *value);
static void property_object_path_list(DBusMessageIter *iter, void *value);
static void property_uchar_list(DBusMessageIter *iter, void *value);
static void property_uint_list(DBusMessageIter *iter, void *value);
static void property_uint_list_list(DBusMessageIter *iter, void *value);

typedef void (*Property_Cb)(DBusMessageIter *iter, void *value);

typedef struct Sig_Property Sig_Property;
struct Sig_Property
{
  const char *sig;
  Property_Cb func;
};

static Sig_Property sigs[] = {
  { .sig = "s", property_string },
  { .sig = "o", property_object_path },
  { .sig = "u", property_uint32 },
  { .sig = "b", property_bool },
  { .sig = "y", property_uchar },
  { .sig = "as", property_string_list },
  { .sig = "ao", property_object_path_list },
  { .sig = "ay", property_uchar_list },
  { .sig = "au", property_uint_list },
  { .sig = "aau", property_uint_list_list },
  { .sig = NULL }
};

static Property_Cb
find_property_cb(const char *sig)
{
  Sig_Property *t;

  if (!sig) return NULL;

  for (t = sigs; t->sig; t++)
  {
    if (!strcmp(t->sig, sig))
      return t->func;
  }
  fprintf(stderr, "Missing property parser for sig: %s\n", sig);
  return NULL;
}

static Property *
find_property(const char *name, Property *properties)
{
  Property *p;

  if (!name) return NULL;

  for (p = properties; p->name; p++)
  {
    if (!strcmp(p->name, name))
      return p;
  }
  return NULL;
}

static void
property_string(DBusMessageIter *iter, void *value)
{
  const char *str;

  if (!check_arg_type(iter, 's')) return;

  dbus_message_iter_get_basic(iter, &str);
  *((char **)value) = strdup(str);
}

static void
property_object_path(DBusMessageIter *iter, void *value)
{
  const char *str;

  if (!check_arg_type(iter, 'o')) return;

  dbus_message_iter_get_basic(iter, &str);
  *((char **)value) = strdup(str);
}

static void
property_uint32(DBusMessageIter *iter, void *value)
{
  if (!check_arg_type(iter, 'u')) return;

  dbus_message_iter_get_basic(iter, value);
}

static void
property_bool(DBusMessageIter *iter, void *value)
{
  if (!check_arg_type(iter, 'b')) return;

  dbus_message_iter_get_basic(iter, value);
}

static void
property_uchar(DBusMessageIter *iter, void *value)
{
  if (!check_arg_type(iter, 'y')) return;

  dbus_message_iter_get_basic(iter, value);
}

static void
property_string_list(DBusMessageIter *iter, void *value)
{
  DBusMessageIter a_iter;
  Ecore_List **list;

  if (!check_arg_type(iter, 'a')) return;
  dbus_message_iter_recurse(iter, &a_iter);
  if (!check_arg_type(&a_iter, 's')) return;

  list = (Ecore_List **)value;
  *list = ecore_list_new();
  ecore_list_free_cb_set(*list, free);
  while (dbus_message_iter_get_arg_type(&a_iter) != DBUS_TYPE_INVALID)
  {
    const char *str;

    dbus_message_iter_get_basic(&a_iter, &str);
    if (str) ecore_list_append(*list, strdup(str));
    dbus_message_iter_next(&a_iter);
  }
}

static void
property_object_path_list(DBusMessageIter *iter, void *value)
{
  DBusMessageIter a_iter;
  Ecore_List **list;

  if (!check_arg_type(iter, 'a')) return;
  dbus_message_iter_recurse(iter, &a_iter);
  if (!check_arg_type(&a_iter, 'o')) return;

  list = (Ecore_List **)value;
  *list = ecore_list_new();
  ecore_list_free_cb_set(*list, free);
  while (dbus_message_iter_get_arg_type(&a_iter) != DBUS_TYPE_INVALID)
  {
    const char *str;

    dbus_message_iter_get_basic(&a_iter, &str);
    if (str) ecore_list_append(*list, strdup(str));
    dbus_message_iter_next(&a_iter);
  }
}

static void
property_uchar_list(DBusMessageIter *iter, void *value)
{
  DBusMessageIter a_iter;
  Ecore_List **list;

  if (!check_arg_type(iter, 'a')) return;
  dbus_message_iter_recurse(iter, &a_iter);
  if (!check_arg_type(&a_iter, 'y')) return;

  list = (Ecore_List **)value;
  *list = ecore_list_new();
  ecore_list_free_cb_set(*list, free);
  while (dbus_message_iter_get_arg_type(&a_iter) != DBUS_TYPE_INVALID)
  {
    unsigned char *c;

    c = malloc(sizeof(unsigned char));
    dbus_message_iter_get_basic(&a_iter, c);
    if (c) ecore_list_append(*list, c);
    dbus_message_iter_next(&a_iter);
  }
}

static void
property_uint_list(DBusMessageIter *iter, void *value)
{
  DBusMessageIter a_iter;
  Ecore_List **list;

  if (!check_arg_type(iter, 'a')) return;
  dbus_message_iter_recurse(iter, &a_iter);
  if (!check_arg_type(&a_iter, 'u')) return;

  list = (Ecore_List **)value;
  *list = ecore_list_new();
  ecore_list_free_cb_set(*list, free);
  while (dbus_message_iter_get_arg_type(&a_iter) != DBUS_TYPE_INVALID)
  {
    unsigned int *c;

    c = malloc(sizeof(unsigned int));
    dbus_message_iter_get_basic(&a_iter, c);
    if (c) ecore_list_append(*list, c);
    dbus_message_iter_next(&a_iter);
  }
}

static void
property_uint_list_list(DBusMessageIter *iter, void *value)
{
  DBusMessageIter a_iter, a2_iter;
  Ecore_List **list, *list2;

  if (!check_arg_type(iter, 'a')) return;
  dbus_message_iter_recurse(iter, &a_iter);
  if (!check_arg_type(&a_iter, 'a')) return;

  list = (Ecore_List **)value;
  *list = ecore_list_new();
  ecore_list_free_cb_set(*list, ECORE_FREE_CB(ecore_list_destroy));
  while (dbus_message_iter_get_arg_type(&a_iter) != DBUS_TYPE_INVALID)
  {
    dbus_message_iter_recurse(&a_iter, &a2_iter);
    if (!check_arg_type(&a2_iter, 'u')) return;
    list2 = ecore_list_new();
    ecore_list_free_cb_set(list2, free);
    ecore_list_append(*list, list);
    while (dbus_message_iter_get_arg_type(&a2_iter) != DBUS_TYPE_INVALID)
    {
      unsigned int *c;

      c = malloc(sizeof(unsigned int));
      dbus_message_iter_get_basic(&a_iter, c);
      if (c) ecore_list_append(list2, c);
      dbus_message_iter_next(&a2_iter);
    }
    dbus_message_iter_next(&a_iter);
  }
}

void
property(void *data, DBusMessage *msg, DBusError *err)
{
  DBusMessageIter iter, v_iter;
  Property_Data *d;
  void *value;
  void (*func)(DBusMessageIter *iter, void *value) = NULL;

  d = data;
  if (dbus_error_is_set(err)) goto error;
  if (!dbus_message_has_signature(msg, "v")) goto error;
  dbus_message_iter_init(msg, &iter);
  dbus_message_iter_recurse(&iter, &v_iter);
  if (d->property->func)
  {
    d->property->func(d, &v_iter);
    return;
  }

  value = ((char *)d->reply + d->property->offset);
  func = find_property_cb(d->property->sig);
  if (func) (*func)(&v_iter, value);

  d->property++;
  if (d->property->name)
    e_nm_device_properties_get(d->nmi->conn, d->object, d->property->name, property, d);
  else
  {
    if (d->cb_func) d->cb_func(d->data, d->reply);
    property_data_free(d);
  }
  return;

error:
  if (d->reply) free(d->reply); /* TODO: Correct free for object */
  if (d->cb_func) d->cb_func(d->data, NULL);
  property_data_free(d);
}

void
parse_properties(void *data, Property *properties, DBusMessage *msg)
{
  DBusMessageIter iter, a_iter;

  if (!dbus_message_has_signature(msg, "a{sv}")) return;

  dbus_message_iter_init(msg, &iter);

  dbus_message_iter_recurse(&iter, &a_iter);
  while (dbus_message_iter_get_arg_type(&a_iter) != DBUS_TYPE_INVALID)
  {
    DBusMessageIter d_iter, v_iter;
    Property *p;
    Property_Cb func;
    const char *name;
    void *value;

    dbus_message_iter_recurse(&a_iter, &d_iter);
    dbus_message_iter_get_basic(&d_iter, &name);

    dbus_message_iter_next(&d_iter);
    dbus_message_iter_recurse(&d_iter, &v_iter);

    p = find_property(name, properties);
    if (!p) goto next;
    value = ((char *)data + p->offset);
    func = find_property_cb(p->sig);
    if (!func) goto next;
    func(&v_iter, value);

next:
    dbus_message_iter_next(&a_iter);
  }
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
check_arg_type(DBusMessageIter *iter, char type)
{
  char sig;
 
  sig = dbus_message_iter_get_arg_type(iter);
  return sig == type;
}

void
property_data_free(Property_Data *data)
{
  if (data->object) free(data->object);
  free(data);
}

