#include "e_connman_private.h"

/**
 * Get the element manager.
 *
 * @return element pointer if found, NULL otherwise.
 */
E_Connman_Element *
e_connman_manager_get(void)
{
   return e_connman_element_get(manager_path);
}

/**
 * Register new agent for handling user requests.
 *
 * Call method RegisterAgent(object) on server in order to
 * register new agent for handling user requests.
 *
 * @param object_path object to be registered.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_connman_manager_agent_register(const char *object_path, E_DBus_Method_Return_Cb cb, const void *data)
{
   const char name[] = "RegisterAgent";
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(object_path, EINA_FALSE);

   element = e_connman_manager_get();
   if (!element)
      return EINA_FALSE;

   return e_connman_element_call_with_path
             (element, name, object_path, NULL,
             &element->_pending.agent_register, cb, data);
}

/**
 * Unregister an existing agent.
 *
 * Call method UnregisterAgent(object) on server in order to
 * unregister an existing agent.
 *
 * @param object_path agent to be unregistered.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_connman_manager_agent_unregister(const char *object_path, E_DBus_Method_Return_Cb cb, const void *data)
{
   const char name[] = "UnregisterAgent";
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(object_path, EINA_FALSE);

   element = e_connman_manager_get();
   if (!element)
      return EINA_FALSE;

   return e_connman_element_call_with_path
             (element, name, object_path, NULL,
             &element->_pending.agent_unregister, cb, data);
}

/**
 * Get property "State" value.
 *
 * If this property isn't found then @c EINA_FALSE is returned.
 * If @c EINA_FALSE is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * The global connection state of a system. Possible
 * values are "online" if at least one connection exists
 * and "offline" if no service is connected.
 *
 * In certain situations the state might change to
 * the value "connected". This can only be seen if
 * previously no connection was present.
 *
 * @param state where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_connman_manager_state_get(const char **state)
{
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(state, EINA_FALSE);

   element = e_connman_manager_get();
   if (!element)
      return EINA_FALSE;

   return e_connman_element_property_get_stringshared
             (element, e_connman_prop_state, NULL, state);
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
 * use radio technology. When leaving offline mode the individual policy of each
 * technology decides to switch the radio back on or not.
 *
 * During offline mode, it is still possible to switch certain technologies
 * manually back on. For example the limited usage of WiFi or Bluetooth
 * technologies might be allowed in some situations.
 *
 * @param offline where to store the property value, must be a pointer
 *        to Eina_Bool (Eina_Bool *).
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 * @see e_connman_manager_offline_mode_set()
 */
Eina_Bool
e_connman_manager_offline_mode_get(Eina_Bool *offline)
{
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(offline, EINA_FALSE);

   element = e_connman_manager_get();
   if (!element)
      return EINA_FALSE;

   return e_connman_element_property_get_stringshared
             (element, e_connman_prop_offline_mode, NULL, offline);
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
 * use radio technology. When leaving offline mode the individual policy of each
 * technology decides to switch the radio back on or not.
 *
 * During offline mode, it is still possible to switch certain technologies
 * manually back on. For example the limited usage of WiFi or Bluetooth
 * technologies might be allowed in some situations.
 *
 * @param offline value to set.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 * @see e_connman_manager_offline_mode_get()
 */
Eina_Bool
e_connman_manager_offline_mode_set(Eina_Bool offline, E_DBus_Method_Return_Cb cb, const void *data)
{
   E_Connman_Element *element = e_connman_manager_get();
   if (!element)
      return EINA_FALSE;

   return e_connman_element_property_set_full
             (element, e_connman_prop_offline_mode, DBUS_TYPE_BOOLEAN,
             &offline, cb, data);
}

/**
 * Get array of profile elements.
 *
 * @param count return the number of elements in array.
 * @param p_elements array with all elements, these are not referenced
 *        and in no particular order, just set if return is @c EINA_TRUE.  The
 *        array itself is allocated using malloc() and should be freed
 *        after usage is done.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_connman_manager_profiles_get(unsigned int *count, E_Connman_Element ***p_elements)
{
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(count, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(p_elements, EINA_FALSE);

   element = e_connman_manager_get();
   if (!element)
      return EINA_FALSE;

   return e_connman_element_objects_array_get_stringshared
             (element, e_connman_prop_profiles, count, p_elements);
}

/**
 * Get array of services elements.
 *
 * List of service object paths. The list is sorted
 * internally to have the service with the default
 * route always first and then the favorite services
 * followed by scan results.
 *
 * This list represents the available services for the
 * current selected profile. If the profile gets changed
 * then this list will be updated.
 *
 * The same list is available via the profile object
 * itself. It is just provided here for convenience of
 * applications only dealing with the current active
 * profile.
 *
 * @param count return the number of elements in array.
 * @param p_elements array with all elements, these are not referenced
 *        and in no particular order, just set if return is @c EINA_TRUE.  The
 *        array itself is allocated using malloc() and should be freed
 *        after usage is done.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_connman_manager_services_get(unsigned int *count, E_Connman_Element ***p_elements)
{
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(count, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(p_elements, EINA_FALSE);

   element = e_connman_manager_get();
   if (!element)
      return EINA_FALSE;

   return e_connman_element_objects_array_get_stringshared
             (element, e_connman_prop_services, count, p_elements);
}

/**
 * Get array of technology elements.
 *
 * @param count return the number of elements in array.
 * @param p_elements array with all elements, these are not referenced
 *        and in no particular order, just set if return is @c EINA_TRUE.  The
 *        array itself is allocated using malloc() and should be freed
 *        after usage is done.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_connman_manager_technologies_get(unsigned int *count, E_Connman_Element ***p_elements)
{
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(count, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(p_elements, EINA_FALSE);

   element = e_connman_manager_get();
   if (!element)
      return EINA_FALSE;

   return e_connman_element_objects_array_get_stringshared
             (element, e_connman_prop_technologies, count, p_elements);
}

/**
 * Request to trigger a scan for given technology.
 *
 * Call method RequestScan(type) on server in order to find new services for
 * such technology type.
 *
 * The empty string for type means all technolgies.
 *
 * @param type technology type to scan. Empty or NULL for all technologies.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_connman_manager_request_scan(const char *type, E_DBus_Method_Return_Cb cb, const void *data)
{
   const char name[] = "RequestScan";
   E_Connman_Element *element;

   if (!type)
      type = "";

   element = e_connman_manager_get();
   if (!element)
      return EINA_FALSE;

   return e_connman_element_call_with_string
             (element, name, type, NULL,
             &element->_pending.request_scan, cb, data);
}

/**
 * Enable specified type of technology.
 *
 * Call method EnableTechnology(type) on server.
 *
 * @param type technology type to enable.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_connman_manager_technology_enable(const char *type, E_DBus_Method_Return_Cb cb, const void *data)
{
   const char name[] = "EnableTechnology";
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(type, EINA_FALSE);

   element = e_connman_manager_get();
   if (!element)
      return EINA_FALSE;

   return e_connman_element_call_with_string
             (element, name, type, NULL,
             &element->_pending.technology_enable, cb, data);
}

/**
 * Disable specified type of technology.
 *
 * Call method DisableTechnology(type) on server.
 *
 * @param type technology type to disable.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_connman_manager_technology_disable(const char *type, E_DBus_Method_Return_Cb cb, const void *data)
{
   const char name[] = "DisableTechnology";
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(type, EINA_FALSE);

   element = e_connman_manager_get();
   if (!element)
      return EINA_FALSE;

   return e_connman_element_call_with_string
             (element, name, type, NULL,
             &element->_pending.technology_disable, cb, data);
}

/**
 * Get property "DefaultTechnology" value.
 *
 * If this property isn't found then @c EINA_FALSE is returned.
 * If @c EINA_FALSE is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * The current connected technology which holds the default route.
 *
 * @param type where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_connman_manager_technology_default_get(const char **type)
{
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(type, EINA_FALSE);

   element = e_connman_manager_get();
   if (!element)
      return EINA_FALSE;

   return e_connman_element_property_get_stringshared
             (element, e_connman_prop_technology_default, NULL, type);
}

/**
 * Remove specified profile.
 *
 * Call method RemoveProfile(profile) on server.
 *
 * It is not possible to remove the current active profile. To remove
 * the active profile a different one must be selected via
 * ActiveProfile property first.
 *
 * At minimum one profile must be available all the time.
 *
 * @param profile element to remove, must be of type profile.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_connman_manager_profile_remove(const E_Connman_Element *profile, E_DBus_Method_Return_Cb cb, const void *data)
{
   E_Connman_Element *element;
   const char name[] = "RemoveProfile";

   EINA_SAFETY_ON_NULL_RETURN_VAL(profile, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(profile->path, EINA_FALSE);

   if (!e_connman_element_is_profile(profile))
      return EINA_FALSE;

   element = e_connman_manager_get();
   if (!element)
      return EINA_FALSE;

   return e_connman_element_call_with_path
             (element, name, profile->path, NULL,
             &element->_pending.profile_remove, cb, data);
}

/**
 * Get property "ActiveProfile" value.
 *
 * If this property isn't found then @c EINA_FALSE is returned.
 * If @c EINA_FALSE is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * @param element where to store the element, just changed if return is @c EINA_TRUE
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 *
 * @see e_connman_manager_profile_active_set()
 */
Eina_Bool
e_connman_manager_profile_active_get(E_Connman_Element **profile)
{
   E_Connman_Element *element;
   char *profile_path;

   EINA_SAFETY_ON_NULL_RETURN_VAL(profile, EINA_FALSE);

   element = e_connman_manager_get();
   if (!element)
      return EINA_FALSE;

   if (!e_connman_element_property_get_stringshared
          (element, e_connman_prop_profile_active, NULL, &profile_path))
      return EINA_FALSE;

   *profile = e_connman_element_get(profile_path);
   return EINA_TRUE;
}

/**
 * Call method SetProperty("ActiveProfile", profile) at the given
 * element on server.
 *
 * This is a server call, not local, so it may fail and in that case
 * no property is updated locally. If the value was set the event
 * E_CONNMAN_EVENT_ELEMENT_UPDATED will be added to main loop.
 *
 * @param profile object to set.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 * @see e_connman_manager_profile_active_get()
 */
Eina_Bool
e_connman_manager_profile_active_set(const E_Connman_Element *profile, E_DBus_Method_Return_Cb cb, const void *data)
{
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(profile, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(profile->path, EINA_FALSE);

   if (!e_connman_element_is_profile(profile))
      return EINA_FALSE;

   element = e_connman_manager_get();
   if (!element)
      return EINA_FALSE;

   return e_connman_element_property_set_full
             (element, e_connman_prop_profile_active, DBUS_TYPE_OBJECT_PATH,
             profile->path, cb, data);
}

/**
 * Get array of strings representing the available technologies.
 *
 * @param count return the number of elements in array.
 * @param p_strings array with pointers to internal strings. These
 *        strings are not copied in any way, and they are granted to
 *        be eina_stringshare instances, so one can use
 *        eina_stringshare_ref() if he wants to save memory and cpu to
 *        get an extra reference. The array itself is allocated using
 *        malloc() and should be freed after usage is done. This
 *        pointer is just set if return is @c EINA_TRUE.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_connman_manager_technologies_available_get(unsigned int *count, const char ***p_strings)
{
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(count, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(p_strings, EINA_FALSE);

   element = e_connman_manager_get();
   if (!element)
      return EINA_FALSE;

   return e_connman_element_strings_array_get_stringshared
             (element, e_connman_prop_technologies_available, count, p_strings);
}

/**
 * Get array of strings representing the enabled technologies.
 *
 * @param count return the number of elements in array.
 * @param p_strings array with pointers to internal strings. These
 *        strings are not copied in any way, and they are granted to
 *        be eina_stringshare instances, so one can use
 *        eina_stringshare_ref() if he wants to save memory and cpu to
 *        get an extra reference. The array itself is allocated using
 *        malloc() and should be freed after usage is done. This
 *        pointer is just set if return is @c EINA_TRUE.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_connman_manager_technologies_enabled_get(unsigned int *count, const char ***p_strings)
{
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(count, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(p_strings, EINA_FALSE);

   element = e_connman_manager_get();
   if (!element)
      return EINA_FALSE;

   return e_connman_element_strings_array_get_stringshared
             (element, e_connman_prop_technologies_enabled, count, p_strings);
}

/**
 * Get array of strings representing the connected technologies.
 *
 * @param count return the number of elements in array.
 * @param p_strings array with pointers to internal strings. These
 *        strings are not copied in any way, and they are granted to
 *        be eina_stringshare instances, so one can use
 *        eina_stringshare_ref() if he wants to save memory and cpu to
 *        get an extra reference. The array itself is allocated using
 *        malloc() and should be freed after usage is done. This
 *        pointer is just set if return is @c EINA_TRUE.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_connman_manager_technologies_connected_get(unsigned int *count, const char ***p_strings)
{
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(count, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(p_strings, EINA_FALSE);

   element = e_connman_manager_get();
   if (!element)
      return EINA_FALSE;

   return e_connman_element_strings_array_get_stringshared
             (element, e_connman_prop_technologies_connected, count, p_strings);
}

