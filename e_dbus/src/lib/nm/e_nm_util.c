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

static E_NM_Variant *property_string(DBusMessageIter *iter, const char *sig, void *value);
static E_NM_Variant *property_basic(DBusMessageIter *iter, const char *sig, void *value);
static E_NM_Variant *property_variant(DBusMessageIter *iter, const char *sig, void *value);
static E_NM_Variant *property_array(DBusMessageIter *iter, const char *sig, void *value);
static void          property_free(E_NM_Variant *var);

typedef E_NM_Variant *(*Property_Cb)(DBusMessageIter *iter, const char *sig, void *value);

typedef struct Sig_Property Sig_Property;
struct Sig_Property
{
  const char *sig;
  Property_Cb func;
};

static Sig_Property sigs[] = {
  { .sig = "s", property_string },
  { .sig = "o", property_string },
  { .sig = "u", property_basic },
  { .sig = "b", property_basic },
  { .sig = "y", property_basic },
  { .sig = "t", property_basic },
  { .sig = "v", property_variant },
  { .sig = "a", property_array },
  { .sig = "as", property_array },
  { .sig = "ao", property_array },
  { .sig = "ay", property_array },
  { .sig = "au", property_array },
  { .sig = "aau", property_array },
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

static E_NM_Variant *
property_string(DBusMessageIter *iter, const char *sig, void *value)
{
  const char   *str;
  E_NM_Variant *var = NULL;

  if ((value) && (!sig))
  {
    printf("Error: Can't have value and no sig\n");
    return NULL;
  }
  dbus_message_iter_get_basic(iter, &str);
  if (sig)
  {
    if (!check_arg_type(iter, sig[0])) return NULL;
    if (!value) value = &var;
  }
  else
  {
    var = malloc(sizeof(E_NM_Variant));
    var->type = dbus_message_iter_get_arg_type(iter);
    value = &var->s;
  }
  *((char **)value) = strdup(str);

  return var;
}

static E_NM_Variant *
property_basic(DBusMessageIter *iter, const char *sig, void *value)
{
  E_NM_Variant *var = NULL;

  if ((value) && (!sig))
  {
    printf("Error: Can't have value and no sig\n");
    return NULL;
  }
  if (sig)
  {
    if (!check_arg_type(iter, sig[0])) return NULL;
    if (!value)
    {
      // TODO: Only alloc right size
      var = malloc(sizeof(long long));
      value = var;
    }
  }
  else
  {
    var = malloc(sizeof(E_NM_Variant));
    var->type = dbus_message_iter_get_arg_type(iter);
    value = var;
  }
  dbus_message_iter_get_basic(iter, value);

  return var;
}

static E_NM_Variant *
property_variant(DBusMessageIter *iter, const char *sig, void *value)
{
  DBusMessageIter v_iter;
  Property_Cb     func;
  char            tmp[2];

  if (!check_arg_type(iter, 'v')) return NULL;
  dbus_message_iter_recurse(iter, &v_iter);
  tmp[0] = dbus_message_iter_get_arg_type(&v_iter);
  tmp[1] = 0;
  func = find_property_cb(tmp);
  if (!func) return NULL;
  return (*func)(&v_iter, NULL, NULL);
}

static E_NM_Variant *
property_array(DBusMessageIter *iter, const char *sig, void *value)
{
  DBusMessageIter   a_iter;
  Ecore_List      **list;
  Property_Cb       func;
  E_NM_Variant    *var = NULL;
  const char       *subsig = NULL;

  if ((value) && (!sig))
  {
    printf("Error: Can't have value and no sig\n");
    return NULL;
  }

  dbus_message_iter_recurse(iter, &a_iter);
  if (sig)
  {

    if (!check_arg_type(iter, sig[0])) return NULL;
    subsig = (sig + 1);
    func = find_property_cb(subsig);
    if (!func) return NULL;
    if (!value) value = &var;
    list = (Ecore_List **)value;
    *list = ecore_list_new();
    if (subsig[0] == 'a') 
      ecore_list_free_cb_set(*list, ECORE_FREE_CB(ecore_list_destroy));
    else
      ecore_list_free_cb_set(*list, free);
  }
  else
  {
    char tmp[] = { dbus_message_iter_get_arg_type(&a_iter), 0 };
    func = find_property_cb(tmp);
    if (!func) return NULL;
    var = malloc(sizeof(E_NM_Variant));
    var->type = dbus_message_iter_get_arg_type(iter);
    list = (Ecore_List **)&var->a;
    *list = ecore_list_new();
    ecore_list_free_cb_set(*list, ECORE_FREE_CB(property_free));
  }

  while (dbus_message_iter_get_arg_type(&a_iter) != DBUS_TYPE_INVALID)
  {
    void *subvar;

    subvar = (*func)(&a_iter, subsig, NULL);
    if (subvar) ecore_list_append(*list, subvar);
    dbus_message_iter_next(&a_iter);
  }

  return var;
}

static void
property_free(E_NM_Variant *var)
{
  if (!var) return;
  if ((var->type == 's') || (var->type == 'o'))
    free(var->s);
  else if (var->type == 'a')
    ecore_list_destroy(var->a);
  free(var);
}

void
property(void *data, DBusMessage *msg, DBusError *err)
{
  DBusMessageIter  iter, v_iter;
  Property_Data   *d;
  void            *value;
  Property_Cb      func = NULL;

  d = data;
  if (dbus_error_is_set(err))
  {
    printf("Error: %s - %s\n", err->name, err->message);
    goto error;
  }
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
  if (func) (*func)(&v_iter, d->property->sig, value);

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
    if (!check_arg_type(&d_iter, 's')) return;
    dbus_message_iter_get_basic(&d_iter, &name);

    dbus_message_iter_next(&d_iter);
    if (!check_arg_type(&d_iter, 'v')) return;
    dbus_message_iter_recurse(&d_iter, &v_iter);

    p = find_property(name, properties);
    if (!p) goto next;
    value = ((char *)data + p->offset);
    func = find_property_cb(p->sig);
    if (!func) goto next;
    func(&v_iter, p->sig, value);

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

    if (!check_arg_type(&sub, 'o')) goto error;
    dbus_message_iter_get_basic(&sub, &dev);
    if (dev) ecore_list_append(devices, dev);
    dbus_message_iter_next(&sub);
  }

  return devices;
error:
  ecore_list_destroy(devices);
  return NULL;
}

void
free_nm_object_path_list(void *data)
{
  Ecore_List *list = data;

  if (list) ecore_list_destroy(list);
}

void *
cb_nm_settings(DBusMessage *msg, DBusError *err)
{
  Ecore_Hash *settings;
  DBusMessageIter iter, a_iter;

  CHECK_SIGNATURE(msg, err, "a{sa{sv}}");

  dbus_message_iter_init(msg, &iter);

  settings = ecore_hash_new(ecore_str_hash, ecore_str_compare);
  ecore_hash_free_key_cb_set(settings, free);
  ecore_hash_free_value_cb_set(settings, ECORE_FREE_CB(ecore_hash_destroy));
  dbus_message_iter_recurse(&iter, &a_iter);
  while (dbus_message_iter_get_arg_type(&a_iter) != DBUS_TYPE_INVALID)
  {
    DBusMessageIter  d_iter, a2_iter;
    E_NM_Variant   *prop;
    const char      *name;
    Ecore_Hash      *value;

    dbus_message_iter_recurse(&a_iter, &d_iter);
    if (!check_arg_type(&d_iter, 's')) goto error;
    dbus_message_iter_get_basic(&d_iter, &name);

    dbus_message_iter_next(&d_iter);
    if (!check_arg_type(&d_iter, 'a')) goto error;
    dbus_message_iter_recurse(&d_iter, &a2_iter);

    value = ecore_hash_new(ecore_str_hash, ecore_str_compare);
    ecore_hash_free_key_cb_set(value, free);
    ecore_hash_free_value_cb_set(value, ECORE_FREE_CB(property_free));
    ecore_hash_set(settings, strdup(name), value);
    while (dbus_message_iter_get_arg_type(&a2_iter) != DBUS_TYPE_INVALID)
    {
      dbus_message_iter_recurse(&a2_iter, &d_iter);
      if (!check_arg_type(&d_iter, 's')) goto error;
      dbus_message_iter_get_basic(&d_iter, &name);
      dbus_message_iter_next(&d_iter);
      if (!check_arg_type(&d_iter, 'v')) goto error;
      prop = property_variant(&d_iter, NULL, NULL);
      if (prop) ecore_hash_set(value, strdup(name), prop);
      dbus_message_iter_next(&a2_iter);
    }

    dbus_message_iter_next(&a_iter);
  }

  return settings;
error:
  ecore_hash_destroy(settings);
  return NULL;
}

void
free_nm_settings(void *data)
{
  Ecore_Hash *hash = data;

  if (hash) ecore_hash_destroy(hash);
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

const char *
ip4_address2str(unsigned int address)
{
  static char buf[16];

  snprintf(buf, sizeof(buf), "%u.%u.%u.%u",
           ((address      ) & 0xff),
           ((address >> 8 ) & 0xff),
           ((address >> 16) & 0xff),
           ((address >> 24) & 0xff));
  return buf;
}
