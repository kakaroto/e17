#include "e_connman_private.h"

E_Connman_Element *
e_connman_profile_get(const char *path)
{
   E_Connman_Element *profile;

   EINA_SAFETY_ON_NULL_RETURN_VAL(path, NULL);

   profile = e_connman_element_get(path);
   if (!profile)
     return NULL;

   if (!e_connman_element_is_profile(profile))
     {
	WRN("path '%s' is not a profile!\n", path);
	return NULL;
     }

   return profile;
}

/**
 * Get property "Name" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * The profile name.
 *
 * @param profile_path to get property.
 * @param name where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_profile_name_get(const E_Connman_Element *profile, const char **name)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(profile, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, 0);
   return e_connman_element_property_get_stringshared
     (profile, e_connman_prop_name, NULL, name);
}
