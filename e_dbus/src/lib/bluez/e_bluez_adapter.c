#include "e_bluez_private.h"

static void
_device_found_callback(void *data, DBusMessage *msg)
{
   E_Bluez_Element *element = (E_Bluez_Element *)data;
   E_Bluez_Device_Found *device;
   DBusMessageIter itr;
   int t;
   char *name = NULL;
   void *value = NULL;

   DBG("Device found %s", element->path);

   if (!_dbus_callback_check_and_init(msg, &itr, NULL))
     return;

   device = calloc(sizeof(E_Bluez_Device_Found), 1);
   if (!device) {
       ERR("No memory to alocate E_Bluez_Device_Found");
       return;
   }

   t = dbus_message_iter_get_arg_type(&itr);
   if (!_dbus_iter_type_check(t, DBUS_TYPE_STRING))
     {
	ERR("missing device name in DeviceFound");
	return;
     }
   dbus_message_iter_get_basic(&itr, &name);

   dbus_message_iter_next(&itr);
   t = dbus_message_iter_get_arg_type(&itr);
   if (!_dbus_iter_type_check(t, DBUS_TYPE_ARRAY))
     {
	ERR("missing array in DeviceFound");
	return;
     }

   value = e_bluez_element_iter_get_array(&itr, name);

   if (!value)
      return;

   device->name = eina_stringshare_add(name);
   device->adapter = element;
   device->array = value;

   ecore_event_add(E_BLUEZ_EVENT_DEVICE_FOUND, device, NULL, NULL);
}

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
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(address, 0);

   return e_bluez_element_property_get_stringshared
     (element, e_bluez_prop_address, NULL, address);
}

/**
 * Get property "Powered" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * @param offline where to store the property value, must be a pointer
 *        to booleans (bool *).
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_manager_offline_mode_set()
 */
bool
e_bluez_adapter_powered_get(E_Bluez_Element *element, bool *powered)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(powered, 0);

   return e_bluez_element_property_get_stringshared
     (element, e_bluez_prop_powered, NULL, powered);
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
e_bluez_adapter_powered_set(E_Bluez_Element *element, bool powered, E_DBus_Method_Return_Cb cb, const void *data)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   return e_bluez_element_property_set_full
     (element, e_bluez_prop_powered, DBUS_TYPE_BOOLEAN,
      &powered, cb, data);
}

/**
 * Get property "Discovering" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * @param discovering where to store the property value, must be a pointer
 *        to booleans (bool *).
 *
 * @return 1 on success, 0 otherwise.
 * @see e_bluez_manager_offline_mode_set()
 */
bool
e_bluez_adapter_discovering_get(E_Bluez_Element *element, bool *discovering)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(discovering, 0);

   return e_bluez_element_property_get_stringshared
     (element, e_bluez_prop_discovering, NULL, discovering);
}

/**
 * Start Discovery of Bluetooth Devices
 *
 * call StartDiscovery()
 *
 * @param element the adapter's element.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_bluez_adapter_start_discovery(E_Bluez_Element *element, E_DBus_Method_Return_Cb cb, const void *data)
{
   const char name[] = "StartDiscovery";

   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);

   element->device_found_handler =
     e_dbus_signal_handler_add
     (e_bluez_conn, e_bluez_system_bus_name_get(),
      element->path, element->interface, "DeviceFound",
      _device_found_callback, element);

   return e_bluez_element_call_full(element, name, NULL,
		   &element->_pending.start_discovery, cb, data);
}

/**
 * Stop Discovery of Bluetooth Devices
 *
 * call StopDiscovery()
 *
 * @param element the adapter's element.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_bluez_adapter_stop_discovery(E_Bluez_Element *element, E_DBus_Method_Return_Cb cb, const void *data)
{
   const char name[] = "StopDiscovery";

   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);

   return e_bluez_element_call_full(element, name, NULL,
		   &element->_pending.stop_discovery, cb, data);
}

/**
 * Create a new Paired Device.
 *
 * Call method CreatePairedDevice()
 *
 * @param element adapter's element
 * @param object_path object to be registered.
 * @param capability input/output agent capabilities
 * @param device device to pair with
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_bluez_adapter_create_paired_device(E_Bluez_Element *element, const char *object_path, const char *capability, const char *device, E_DBus_Method_Return_Cb cb, const void *data)
{
   DBusMessageIter itr;
   DBusMessage *msg;

   const char name[] = "CreatePairedDevice";

   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(object_path, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(device, 0);

   msg = dbus_message_new_method_call
     (e_bluez_system_bus_name_get(), element->path, element->interface,
      name);

   if (!msg)
     return 0;

   dbus_message_iter_init_append(msg, &itr);
   dbus_message_iter_append_basic(&itr, DBUS_TYPE_STRING, &device);
   dbus_message_iter_append_basic(&itr, DBUS_TYPE_OBJECT_PATH, &object_path);
   dbus_message_iter_append_basic(&itr, DBUS_TYPE_STRING, &capability);

   return e_bluez_element_message_send(element, name, NULL, msg,
			 &element->_pending.create_paired_device, cb, data);
}
