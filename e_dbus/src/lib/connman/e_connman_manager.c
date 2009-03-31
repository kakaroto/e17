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
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_manager_register_agent(const char *object_path, E_DBus_Method_Return_Cb cb, const void *data)
{
   const char name[] = "RegisterAgent";
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(object_path, 0);

   element = e_connman_manager_get();
   if (!element)
     return 0;

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
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_manager_unregister_agent(const char *object_path, E_DBus_Method_Return_Cb cb, const void *data)
{
   const char name[] = "UnregisterAgent";
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(object_path, 0);

   element = e_connman_manager_get();
   if (!element)
     return 0;

   return e_connman_element_call_with_path
     (element, name, object_path, NULL,
      &element->_pending.agent_unregister, cb, data);
}

/**
 * Get property "State" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * The global connection state of a system. Possible
 * values are "online" if at least one connection exists
 * and "offline" if no device is connected.
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
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_manager_state_get(const char **state)
{
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(state, 0);

   element = e_connman_manager_get();
   if (!element)
     return 0;
   return e_connman_element_property_get_stringshared
     (element, e_connman_prop_state, NULL, state);
}

/**
 * Get property "Policy" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * The global connection policy of a system. This
 * allows to configure how connections are established
 * and also when they are taken down again.
 *
 * Possible values are "single", "multiple" and "ask".
 *
 * For the single policy, the priority setting of the
 * device defines which becomes the default connection
 * when multiple are available.
 *
 * @param policy where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_manager_policy_set()
 */
bool
e_connman_manager_policy_get(const char **policy)
{
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(policy, 0);

   element = e_connman_manager_get();
   if (!element)
     return 0;
   return e_connman_element_property_get_stringshared
     (element, e_connman_prop_policy, NULL, policy);
}

/**
 * Call method SetProperty("Policy", policy) at the given element on server.
 *
 * This is a server call, not local, so it may fail and in that case
 * no property is updated locally. If the value was set the event
 * E_CONNMAN_EVENT_ELEMENT_UPDATED will be added to main loop.
 *
 * The global connection policy of a system. This
 * allows to configure how connections are established
 * and also when they are taken down again.
 *
 * Possible values are "single", "multiple" and "ask".
 *
 * For the single policy, the priority setting of the
 * device defines which becomes the default connection
 * when multiple are available.
 *
 * @param policy value to set.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_manager_policy_get()
 */
bool
e_connman_manager_policy_set(const char *policy, E_DBus_Method_Return_Cb cb, const void *data)
{
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(policy, 0);

   element = e_connman_manager_get();
   if (!element)
     return 0;

   return e_connman_element_property_set_full
     (element, e_connman_prop_policy, DBUS_TYPE_STRING, policy, cb, data);
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
 * to true results in powering down all devices. When
 * leaving offline mode the individual policy of each
 * device decides to switch the radio back on or not.
 *
 * During offline mode, it is still possible to switch
 * certain technologies manually back on. For example
 * the limited usage of WiFi or Bluetooth devices might
 * be allowed in some situations.
 *
 * @param offline where to store the property value, must be a pointer
 *        to booleans (bool *).
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_manager_offline_mode_set()
 */
bool
e_connman_manager_offline_mode_get(bool *offline)
{
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(offline, 0);

   element = e_connman_manager_get();
   if (!element)
     return 0;
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
 * The offline mode indicates the global setting for
 * switching all radios on or off. Changing offline mode
 * to true results in powering down all devices. When
 * leaving offline mode the individual policy of each
 * device decides to switch the radio back on or not.
 *
 * During offline mode, it is still possible to switch
 * certain technologies manually back on. For example
 * the limited usage of WiFi or Bluetooth devices might
 * be allowed in some situations.
 *
 * @param offline value to set.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_manager_offline_mode_get()
 */
bool
e_connman_manager_offline_mode_set(bool offline, E_DBus_Method_Return_Cb cb, const void *data)
{
   E_Connman_Element *element = e_connman_manager_get();
   if (!element)
     return 0;
   return e_connman_element_property_set_full
     (element, e_connman_prop_offline_mode, DBUS_TYPE_BOOLEAN,
      &offline, cb, data);
}

/**
 * Get array of profile elements.
 *
 * @param count return the number of elements in array.
 * @param p_elements array with all elements, these are not referenced
 *        and in no particular order, just set if return is 1.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_manager_profiles_get(unsigned int *count, E_Connman_Element ***p_elements)
{
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(count, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(p_elements, 0);

   element = e_connman_manager_get();
   if (!element)
     return 0;
   return e_connman_element_objects_array_get_stringshared
     (element, e_connman_prop_profiles, count, p_elements);
}

/**
 * Get array of device elements.
 *
 * @param count return the number of elements in array.
 * @param p_elements array with all elements, these are not referenced
 *        and in no particular order, just set if return is 1.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_manager_devices_get(unsigned int *count, E_Connman_Element ***p_elements)
{
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(count, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(p_elements, 0);

   element = e_connman_manager_get();
   if (!element)
     return 0;
   return e_connman_element_objects_array_get_stringshared
     (element, e_connman_prop_devices, count, p_elements);
}

/**
 * Get array of connection elements.
 *
 * @param count return the number of elements in array.
 * @param p_elements array with all elements, these are not referenced
 *        and in no particular order, just set if return is 1.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_manager_connections_get(unsigned int *count, E_Connman_Element ***p_elements)
{
   E_Connman_Element *element;

   EINA_SAFETY_ON_NULL_RETURN_VAL(count, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(p_elements, 0);

   element = e_connman_manager_get();
   if (!element)
     return 0;
   return e_connman_element_objects_array_get_stringshared
     (element, e_connman_prop_connections, count, p_elements);
}
