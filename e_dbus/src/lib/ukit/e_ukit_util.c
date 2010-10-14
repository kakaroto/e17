#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <E_Ukit.h>
#include "e_ukit_private.h"

/**
 * @internal
 * @brief free a property structure
 * @param prop the property to free
 */
EAPI void
e_ukit_property_free(E_Ukit_Property *prop)
{
   if (prop->type == E_UKIT_PROPERTY_TYPE_STRLIST)
     eina_list_free(prop->val.strlist);
   free(prop);
}

/**
 * @brief Retrive a string from an element of a property hash
 * @param properties the E_Ukit_Properties structure
 * @param key the key of the property to retrieve
 * @param err a pointer to an int, which if supplied, will be set to 0 on success and 1 on an error
 */
EAPI const char *
e_ukit_property_string_get(E_Ukit_Properties *properties, const char *key, int *err)
{
   E_Ukit_Property *prop;
   if (err) *err = 0;
   if (!properties->properties) return NULL;
   prop = eina_hash_find(properties->properties, key);
   if (prop) return prop->val.s;

   if (err) *err = 1;
   return NULL;
}

EAPI Eina_Bool
e_ukit_property_bool_get(E_Ukit_Properties *properties, const char *key, int *err)
{
   E_Ukit_Property *prop;
   if (err) *err = 0;
   if (!properties->properties) return EINA_FALSE;
   prop = eina_hash_find(properties->properties, key);
   if (prop) return prop->val.b;
   
   if (err) *err = 1;
   return EINA_FALSE;
}

EAPI int
e_ukit_property_int_get(E_Ukit_Properties *properties, const char *key, int *err)
{
   E_Ukit_Property *prop;
   if (err) *err = 0;
   if (!properties->properties) return 0;
   prop = eina_hash_find(properties->properties, key);
   if (prop) return prop->val.i;

   if (err) *err = 1;
   return 0;
}

EAPI uint32_t
e_ukit_property_uint32_get(E_Ukit_Properties *properties, const char *key, int *err)
{
   E_Ukit_Property *prop;
   if (err) *err = 0;
   if (!properties->properties) return 0;
   prop = eina_hash_find(properties->properties, key);
   if (prop) return prop->val.u;

   if (err) *err = 1;
   return 0;
}

EAPI uint64_t
e_ukit_property_uint64_get(E_Ukit_Properties *properties, const char *key, int *err)
{
   E_Ukit_Property *prop;
   if (err) *err = 0;
   if (!properties->properties) return 0;
   prop = eina_hash_find(properties->properties, key);
   if (prop) return prop->val.t;

   if (err) *err = 1;
   return 0;
}

EAPI int64_t
e_ukit_property_int64_get(E_Ukit_Properties *properties, const char *key, int *err)
{
   E_Ukit_Property *prop;
   if (err) *err = 0;
   if (!properties->properties) return 0;
   prop = eina_hash_find(properties->properties, key);
   if (prop) return prop->val.x;

   if (err) *err = 1;
   return 0;
}

EAPI double
e_ukit_property_double_get(E_Ukit_Properties *properties, const char *key, int *err)
{
   E_Ukit_Property *prop;
   if (err) *err = 0;
   if (!properties->properties) return 0;
   prop = eina_hash_find(properties->properties, key);
   if (prop) return prop->val.d;

   if (err) *err = 1;
   return 0;
}

EAPI const Eina_List *
e_ukit_property_strlist_get(E_Ukit_Properties *properties, const char *key, int *err)
{
   E_Ukit_Property *prop;
   if (err) *err = 0;
   if (!properties->properties) return NULL;
   prop = eina_hash_find(properties->properties, key);
   if (prop) return prop->val.strlist;

   if (err) *err = 1;
   return NULL;
}
