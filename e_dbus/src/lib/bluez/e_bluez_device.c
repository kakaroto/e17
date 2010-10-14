#include "e_bluez_private.h"

E_Bluez_Element *
e_bluez_device_get(const char *path)
{
   E_Bluez_Element *device;

   EINA_SAFETY_ON_NULL_RETURN_VAL(path, NULL);

   device = e_bluez_element_get(path);
   if (!device)
      return NULL;

   if (!e_bluez_element_is_device(device))
     {
        WRN("path '%s' is not a device!", path);
        return NULL;
     }

   return device;
}

/**
 * Get property "Name" value.
 *
 * If this property isn't found then @c EINA_FALSE is returned.
 * If @c EINA_FALSE is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * @param address where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_bluez_device_name_get(const E_Bluez_Element *element, const char **name)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, EINA_FALSE);

   return e_bluez_element_property_get_stringshared
             (element, e_bluez_prop_name, NULL, name);
}

/**
 * Get property "Alias" value.
 *
 * If this property isn't found then @c EINA_FALSE is returned.
 * If @c EINA_FALSE is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * @param address where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_bluez_device_alias_get(const E_Bluez_Element *element, const char **alias)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(alias, EINA_FALSE);

   return e_bluez_element_property_get_stringshared
             (element, e_bluez_prop_alias, NULL, alias);
}

/**
 * Get property "Paired" value.
 *
 * If this property isn't found then @c EINA_FALSE is returned.
 * If @c EINA_FALSE is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * @param powered where to store the property value, must be a pointer
 *        to boolean (Eina_Bool *).
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_bluez_device_paired_get(const E_Bluez_Element *element, Eina_Bool *paired)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(paired, EINA_FALSE);

   return e_bluez_element_property_get_stringshared
             (element, e_bluez_prop_paired, NULL, paired);
}

