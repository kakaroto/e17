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
