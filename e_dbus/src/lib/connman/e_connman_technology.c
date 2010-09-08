#include "e_connman_private.h"

E_Connman_Element *
e_connman_technology_get(const char *path)
{
   E_Connman_Element *technology;

   EINA_SAFETY_ON_NULL_RETURN_VAL(path, NULL);

   technology = e_connman_element_get(path);
   if (!technology)
     return NULL;

   if (!e_connman_element_is_technology(technology))
     {
	WRN("path '%s' is not a technology!", path);
	return NULL;
     }

   return technology;
}

/**
 * Get property "Name" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * Name of this technology.
 *
 * @param technology path to get property
 * @param name where to store the property value, must be a pointer
 *        to string (const char *), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise
 */
bool
e_connman_technology_name_get(const E_Connman_Element *technology, const char **name)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(technology, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, 0);
   return e_connman_element_property_get_stringshared
     (technology, e_connman_prop_name, NULL, name);
}

/**
 * Get property "Type" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * The technology type (for example "ethernet" etc.)
 *
 * This information should only be used to determine
 * advanced properties or showing the correct icon
 * to the user.
 *
 * @param technology path to get property.
 * @param type where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_technology_type_get(const E_Connman_Element *technology, const char **type)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(technology, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(type, 0);
   return e_connman_element_property_get_stringshared
     (technology, e_connman_prop_type, NULL, type);
}

/**
 * Get property "State" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * The technology state information.
 *
 * Valid states are "offline", "available", "enabled",
 * and "connected".
 *
 * @param technology path to get property.
 * @param state where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_technology_state_get(const E_Connman_Element *technology, const char **state)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(technology, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(state, 0);
   return e_connman_element_property_get_stringshared
     (technology, e_connman_prop_state, NULL, state);
}

/**
 * Get array of device elements.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * @param technology path to get property.
 * @param count return the number of elements in array.
 * @param t_elements array with all elements, these are not referenced
 *        and in no particular order, just set if return is 1.  The
 *        array itself is allocated using malloc() and should be freed
 *        after usage is done.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_technology_devices_get(const E_Connman_Element *technology, unsigned int *count, E_Connman_Element ***t_elements)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(technology, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(count, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(t_elements, 0);
   return e_connman_element_objects_array_get_stringshared
     (technology, e_connman_prop_devices, count, t_elements);
}
