#include "e_bluez_private.h"

/**
 * Get property "Name" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * @param address where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_bluez_device_name_get(E_Bluez_Element *element, const char **name)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, 0);

   return e_bluez_element_property_get_stringshared
     (element, e_bluez_prop_name, NULL, name);
}
