#include <E_Hal.h>
#include <string.h>

/**
 * @internal
 * @brief free a property structure
 * @param prop the property to free
 */
EAPI void
e_hal_property_free(E_Hal_Property *prop)
{
  if (prop->type == E_HAL_PROPERTY_TYPE_STRLIST)
    ecore_list_destroy(prop->val.strlist);
  free(prop);
}

/**
 * @brief Retrive a string from an element of a property hash
 * @param properties the E_Hal_Properties structure
 * @param key the key of the property to retrieve
 * @param err a pointer to an int, which if supplied, will be set to 0 on success and 1 on an error
 */
EAPI char *
e_hal_property_string_get(E_Hal_Properties *properties, const char *key, int *err)
{
  E_Hal_Property *prop;
  if (err) *err = 0;
  if (!properties->properties) return NULL;
  prop = ecore_hash_get(properties->properties, key);
  if (prop) return strdup(prop->val.s);

  if (err) *err = 1;
  return NULL;
}

EAPI char
e_hal_property_bool_get(E_Hal_Properties *properties, const char *key, int *err)
{
  E_Hal_Property *prop;
  if (err) *err = 0;
  if (!properties->properties) return 0;
  prop = ecore_hash_get(properties->properties, key);
  if (prop) return prop->val.b;
  
  if (err) *err = 1;
  return 0;
}

EAPI int
e_hal_property_int_get(E_Hal_Properties *properties, const char *key, int *err)
{
  E_Hal_Property *prop;
  if (err) *err = 0;
  if (!properties->properties) return 0;
  prop = ecore_hash_get(properties->properties, key);
  if (prop) return prop->val.i;

  if (err) *err = 1;
  return 0;
}

EAPI dbus_uint64_t
e_hal_property_uint64_get(E_Hal_Properties *properties, const char *key, int *err)
{
  E_Hal_Property *prop;
  if (err) *err = 0;
  if (!properties->properties) return 0;
  prop = ecore_hash_get(properties->properties, key);
  if (prop) return prop->val.u64;

  if (err) *err = 1;
  return 0;
}

EAPI double
e_hal_property_double_get(E_Hal_Properties *properties, const char *key, int *err)
{
  E_Hal_Property *prop;
  if (err) *err = 0;
  if (!properties->properties) return 0;
  prop = ecore_hash_get(properties->properties, key);
  if (prop) return prop->val.d;

  if (err) *err = 1;
  return 0;
}

EAPI Ecore_List *
e_hal_property_strlist_get(E_Hal_Properties *properties, const char *key, int *err)
{
  E_Hal_Property *prop;
  if (err) *err = 0;
  if (!properties->properties) return NULL;
  prop = ecore_hash_get(properties->properties, key);
  if (prop) return prop->val.strlist;

  if (err) *err = 1;
  return NULL;
}
