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
	WRN("E-Dbus connman: path '%s' is not a profile!", path);
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

/**
 * Get property "OfflineMode" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * The offline mode indicates the global setting for
 * switching all radios on or off. Changing offline mode
 * to true results in powering down all devices.
 *
 * @param offline where to store the property value, must be a pointer
 *        to booleans (bool *).
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_profile_offline_mode_set()
 */
bool
e_connman_profile_offline_mode_get(const E_Connman_Element *profile, bool *offline)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(profile, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(offline, 0);
   return e_connman_element_property_get_stringshared
     (profile, e_connman_prop_offline_mode, NULL, offline);
}

/**
 * Call method SetProperty("OfflineMode", offline) at the given element on server.
 *
 * This is a server call, not local, so it may fail and in that case
 * no property is updated locally. If the value was set the event
 * E_CONNMAN_EVENT_ELEMENT_UPDATED will be added to main loop.
 *
 * The offline mode indicates the global setting for
 * switching all radios on or off. Changing offline mode
 * to true results in powering down all devices.
 *
 * @param offline value to set.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_profile_offline_mode_get()
 */
bool
e_connman_profile_offline_mode_set(E_Connman_Element *profile, bool offline, E_DBus_Method_Return_Cb cb, const void *data)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(profile, 0);
   return e_connman_element_property_set_full
     (profile, e_connman_prop_offline_mode, DBUS_TYPE_BOOLEAN,
      &offline, cb, data);
}

/**
 * Get array of service elements.
 *
 * @param count return the number of elements in array.
 * @param p_elements array with all elements, these are not referenced
 *        and in no particular order, just set if return is 1.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_profile_services_get(const E_Connman_Element *profile, unsigned int *count, E_Connman_Element ***p_elements)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(profile, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(count, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(p_elements, 0);
   return e_connman_element_objects_array_get_stringshared
     (profile, e_connman_prop_services, count, p_elements);
}
