#include "e_bluez_private.h"

/**
 * Register new agent for handling user requests.
 *
 * Call method RegisterAgent(object) on server in order to
 * register new agent for handling user requests.
 *
 * @param element adapter's element
 * @param object_path object to be registered.
 * @param capability input/output agent capabilities
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_bluez_adapter_agent_register(E_Bluez_Element *element, const char *object_path, const char *capability, E_DBus_Method_Return_Cb cb, const void *data)
{
   const char name[] = "RegisterAgent";

   EINA_SAFETY_ON_NULL_RETURN_VAL(object_path, 0);

   return e_bluez_element_call_with_path_and_string
     (element, name, object_path, capability, NULL,
      &element->_pending.agent_register, cb, data);
}

/**
 * Unregister an existing agent.
 *
 * Call method UnregisterAgent(object) on server in order to
 * unregister an existing agent.
 *
 * @param element adapter's element
 * @param object_path agent to be unregistered.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_bluez_adapter_agent_unregister(E_Bluez_Element *element, const char *object_path, E_DBus_Method_Return_Cb cb, const void *data)
{
   const char name[] = "UnregisterAgent";

   EINA_SAFETY_ON_NULL_RETURN_VAL(object_path, 0);

   return e_bluez_element_call_with_path
     (element, name, object_path, NULL,
      &element->_pending.agent_unregister, cb, data);
}

/**
 * Get property "Address" value.
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
e_bluez_adapter_address_get(E_Bluez_Element *element, const char **address)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(address, 0);

   return e_bluez_element_property_get_stringshared
     (element, e_bluez_prop_address, NULL, address);
}

/**
 * Call method SetProperty("Powered", powered) at the given element on server.
 *
 *
 * @param powered value to set.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_bluez_adapter_powered_set(E_Bluez_Element *profile, bool offline, E_DBus_Method_Return_Cb cb, const void *data)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(profile, 0);
   return e_bluez_element_property_set_full
     (profile, e_bluez_prop_powered, DBUS_TYPE_BOOLEAN,
      &offline, cb, data);
}
