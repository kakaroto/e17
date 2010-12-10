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
        WRN("path '%s' is not a profile!", path);
        return NULL;
     }

   return profile;
}

/**
 * Get property "Name" value.
 *
 * If this property isn't found then @c EINA_FALSE is returned.
 * If @c EINA_FALSE is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * The profile name, if set with e_connman_profile_name_set()
 *
 * @param profile path to get property.
 * @param name where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 * @see e_connman_profile_name_set()
 */
Eina_Bool
e_connman_profile_name_get(const E_Connman_Element *profile, const char **name)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(profile, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, EINA_FALSE);
   return e_connman_element_property_get_stringshared
             (profile, e_connman_prop_name, NULL, name);
}

/**
 * Call method SetProperty("Name", name) at the given element on server.
 *
 * This is a server call, not local, so it may fail and in that case
 * no property is updated locally. If the value was set the event
 * E_CONNMAN_EVENT_ELEMENT_UPDATED will be added to main loop.
 *
 * @param name value to set.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 * @see e_connman_profile_name_get()
 */
Eina_Bool
e_connman_profile_name_set(E_Connman_Element *profile, const char *name, E_DBus_Method_Return_Cb cb, const void *data)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(profile, EINA_FALSE);
   return e_connman_element_property_set_full
             (profile, e_connman_prop_name, DBUS_TYPE_STRING, name, cb, data);
}

/**
 * Get property "OfflineMode" value.
 *
 * If this property isn't found then @c EINA_FALSE is returned.
 * If @c EINA_FALSE is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * The offline mode indicates the global setting for switching all radios on or
 * off. Changing offline mode to true results in powering down all devices that
 * use radio technology.
 *
 * @param offline where to store the property value, must be a pointer
 *        to Eina_Bool (Eina_Bool *).
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 * @see e_connman_profile_offline_mode_set()
 */
Eina_Bool
e_connman_profile_offline_mode_get(const E_Connman_Element *profile, Eina_Bool *offline)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(profile, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(offline, EINA_FALSE);
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
 * The offline mode indicates the global setting for switching all radios on or
 * off. Changing offline mode to true results in powering down all devices that
 * use radio technology.
 *
 * @param offline value to set.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 * @see e_connman_profile_offline_mode_get()
 */
Eina_Bool
e_connman_profile_offline_mode_set(E_Connman_Element *profile, Eina_Bool offline, E_DBus_Method_Return_Cb cb, const void *data)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(profile, EINA_FALSE);
   return e_connman_element_property_set_full
             (profile, e_connman_prop_offline_mode, DBUS_TYPE_BOOLEAN,
             &offline, cb, data);
}

/**
 * Get array of service elements.
 *
 * @param count return the number of elements in array.
 * @param p_elements array with all elements, these are not referenced
 *        and in no particular order, just set if return is @c EINA_TRUE.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_connman_profile_services_get(const E_Connman_Element *profile, unsigned int *count, E_Connman_Element ***p_elements)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(profile, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(count, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(p_elements, EINA_FALSE);
   return e_connman_element_objects_array_get_stringshared
             (profile, e_connman_prop_services, count, p_elements);
}

