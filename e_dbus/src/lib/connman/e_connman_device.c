#include "e_connman_private.h"

E_Connman_Element *
e_connman_device_get(const char *path)
{
   E_Connman_Element *device;

   EINA_SAFETY_ON_NULL_RETURN_VAL(path, NULL);

   device = e_connman_element_get(path);
   if (!device)
     return NULL;

   if (!e_connman_element_is_device(device))
     {
	WRN("path '%s' is not a device!\n", path);
	return NULL;
     }

   return device;
}

static void
_e_connman_device_network_create_callback(void *user_data, DBusMessage *msg, DBusError *err)
{
   WRN("FIXME need to receive the network object path\n");
}

/**
 * Creates a network object from the specified properties. Valid
 * properties are WiFi.SSID, WiFi.Security and WiFi.Passphrase.
 *
 * Call method CreateNetwork(dict_network) at the given device
 * on server.
 *
 * @param device_path to call method on server.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_device_network_create(E_Connman_Element *device, E_DBus_Method_Return_Cb cb, const void *data)
{
   const char name[] = "CreateNetwork";
   DBusMessageIter itr, v;
   DBusMessage *msg;

   EINA_SAFETY_ON_NULL_RETURN_VAL(device, 0);

   msg = dbus_message_new_method_call
     (e_connman_system_bus_name_get(), device->path, device->interface,
      name);

   if (!msg)
     return 0;

   dbus_message_iter_init_append(msg, &itr);
   dbus_message_iter_open_container(&itr, DBUS_TYPE_ARRAY,
				    DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
				    DBUS_TYPE_STRING_AS_STRING
				    DBUS_TYPE_VARIANT_AS_STRING
				    DBUS_DICT_ENTRY_END_CHAR_AS_STRING, &v);

   /* FIXME need to create the message */

   dbus_message_iter_close_container(&itr, &v);
   return e_connman_element_message_send
     (device, name, _e_connman_device_network_create_callback,
      msg, &device->_pending.network_create, cb, data);
}

/**
 * Removes a previoulsy created network object.
 *
 * Call method RemoveNetwork(network) at the given device on server in order to remove
 * it.
 *
 * @param device_path to call method on server.
 * @param network_path network path to be removed.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_device_network_remove(E_Connman_Element *device, const char *network_path, E_DBus_Method_Return_Cb cb, const void *data)
{
   const char name[] = "RemoveNetwork";

   EINA_SAFETY_ON_NULL_RETURN_VAL(device, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(network_path, 0);
   return e_connman_element_call_with_path
     (device, name, network_path, NULL,
      &device->_pending.network_remove, cb, data);
}

/**
 * Propose device to scan.
 *
 * Call method ProposeScan() at the given device on server in order to propose it
 * to scan.
 *
 * @param device_path to call method on server.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_device_propose_scan(E_Connman_Element *device, E_DBus_Method_Return_Cb cb, const void *data)
{
   const char name[] = "ProposeScan";

   EINA_SAFETY_ON_NULL_RETURN_VAL(device, 0);
   return e_connman_element_call_full
     (device, name, NULL, &device->_pending.device_propose_scan, cb, data);
}

/**
 * Get property "Name" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * The device name (for example "Wireless" etc.)
 *
 * This name can be used for directly displaying it in
 * the application. It has pure informational purpose.
 *
 * @param device_path to get property.
 * @param name where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_device_name_get(const E_Connman_Element *device, const char **name)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(device, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, 0);
   return e_connman_element_property_get_stringshared
     (device, e_connman_prop_name, NULL, name);
}

/**
 * Get property "Type" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * The device type (for example "ethernet", "wifi" etc.)
 *
 * @param device_path to get property.
 * @param type where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_device_type_get(const E_Connman_Element *device, const char **type)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(device, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(type, 0);
   return e_connman_element_property_get_stringshared
     (device, e_connman_prop_type, NULL, type);
}

/**
 * Get property "Interface" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * The device interface (for example "eth0" etc.)
 *
 * This value is for pure informational purposes. It
 * is not guaranteed that it is always present.
 *
 * @param device_path to get property.
 * @param interface where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_device_interface_get(const E_Connman_Element *device, const char **interface)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(device, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(interface, 0);
   return e_connman_element_property_get_stringshared
     (device, e_connman_prop_interface, NULL, interface);
}

/**
 * Get property "Policy" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * Setting of the device power and connection policy.
 * Possible values are "ignore", "off", "auto"
 * and "manual".
 *
 * The policy defines on how the device is initialized
 * when brought up and how it connects. The actual
 * device power state can be changed independently to
 * this value.
 *
 * If a device is switched off and the policy is changed
 * to "auto" or "manual", the device will be switched
 * on. For a current active device changing the policy
 * to "off" results in powering down the device.
 *
 * The "ignore" policy can be set for devices that are
 * detected, but managed by a different entity on the
 * system. For example for complex network setups.
 *
 * Devices that can connect to various networks, the
 * difference between "auto" or "manual" defines if
 * known networks are connected automatically or not.
 * For simple devices like Ethernet cards, setting
 * the "manual" policy might fail.
 *
 * @param device_path to get property.
 * @param policy where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_device_policy_set()
 */
bool
e_connman_device_policy_get(const E_Connman_Element *device, const char **policy)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(device, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(policy, 0);
   return e_connman_element_property_get_stringshared
     (device, e_connman_prop_policy, NULL, policy);
}

/**
 * Call method SetProperty("Policy", policy) at the given element on server.
 *
 * This is a server call, not local, so it may fail and in that case
 * no property is updated locally. If the value was set the event
 * E_CONNMAN_EVENT_ELEMENT_UPDATED will be added to main loop.
 *
 * Setting of the device power and connection policy.
 * Possible values are "ignore", "off", "auto"
 * and "manual".
 *
 * The policy defines on how the device is initialized
 * when brought up and how it connects. The actual
 * device power state can be changed independently to
 * this value.
 *
 * If a device is switched off and the policy is changed
 * to "auto" or "manual", the device will be switched
 * on. For a current active device changing the policy
 * to "off" results in powering down the device.
 *
 * The "ignore" policy can be set for devices that are
 * detected, but managed by a different entity on the
 * system. For example for complex network setups.
 *
 * Devices that can connect to various networks, the
 * difference between "auto" or "manual" defines if
 * known networks are connected automatically or not.
 * For simple devices like Ethernet cards, setting
 * the "manual" policy might fail.
 *
 * @param device_path to set property.
 * @param policy value to set.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_device_policy_get()
 */
bool
e_connman_device_policy_set(E_Connman_Element *device, const char *policy, E_DBus_Method_Return_Cb cb, const void *data)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(device, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(policy, 0);
   return e_connman_element_property_set_full
     (device, e_connman_prop_policy, DBUS_TYPE_STRING, policy, cb, data);
}

/**
 * Get property "Priority" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * The device priority. Higher values indicate the
 * preference for this device.
 *
 * @param device_path to get property.
 * @param priority where to store the property value, must be a pointer
 *        to byte (unsigned char *).
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_device_priority_set()
 */
bool
e_connman_device_priority_get(const E_Connman_Element *device, unsigned char *priority)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(device, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(priority, 0);
   return e_connman_element_property_get_stringshared
     (device, e_connman_prop_priority, NULL, priority);
}

/**
 * Call method SetProperty("Priority", priority) at the given element on server.
 *
 * This is a server call, not local, so it may fail and in that case
 * no property is updated locally. If the value was set the event
 * E_CONNMAN_EVENT_ELEMENT_UPDATED will be added to main loop.
 *
 * The device priority. Higher values indicate the
 * preference for this device.
 *
 * @param device_path to set property.
 * @param priority value to set.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_device_priority_get()
 */
bool
e_connman_device_priority_set(E_Connman_Element *device, unsigned char priority, E_DBus_Method_Return_Cb cb, const void *data)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(device, 0);
   return e_connman_element_property_set_full
     (device, e_connman_prop_priority, DBUS_TYPE_BYTE, &priority, cb, data);
}

/**
 * Get property "Powered" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * Switch a device on or off. This will also modify
 * the list of networks in range. All known networks
 * will be still available via the Networks property.
 *
 * Changing this value doesn't change the value of the
 * Policy property.
 *
 * The value of this property can be changed by other
 * parts of the system (including the kernel). An
 * example would be modifications via the "ifconfig"
 * command line utility.
 *
 * @param device_path to get property.
 * @param powered where to store the property value, must be a pointer
 *        to boolean (bool *).
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_device_powered_set()
 */
bool
e_connman_device_powered_get(const E_Connman_Element *device, bool *powered)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(device, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(powered, 0);
   return e_connman_element_property_get_stringshared
     (device, e_connman_prop_powered, NULL, powered);
}

/**
 * Call method SetProperty("Powered", powered) at the given element on server.
 *
 * This is a server call, not local, so it may fail and in that case
 * no property is updated locally. If the value was set the event
 * E_CONNMAN_EVENT_ELEMENT_UPDATED will be added to main loop.
 *
 * Switch a device on or off. This will also modify
 * the list of networks in range. All known networks
 * will be still available via the Networks property.
 *
 * Changing this value doesn't change the value of the
 * Policy property.
 *
 * The value of this property can be changed by other
 * parts of the system (including the kernel). An
 * example would be modifications via the "ifconfig"
 * command line utility.
 *
 * @param device_path to set property.
 * @param powered value to set.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_device_powered_get()
 */
bool
e_connman_device_powered_set(E_Connman_Element *device, bool powered, E_DBus_Method_Return_Cb cb, const void *data)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(device, 0);
   return e_connman_element_property_set_full
     (device, e_connman_prop_powered, DBUS_TYPE_BOOLEAN, &powered, cb, data);
}

/**
 * Get property "ScanInterval" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * The scan interval describes the time in seconds
 * between automated scan attempts. Setting this
 * value to 0 will disable the background scanning.
 *
 * The default value is 300 and so every 5 minutes
 * a scan procedure will be triggered.
 *
 * This property is not available with all types
 * of devices. Some might not support background
 * scanning at all.
 *
 * @param device_path to get property.
 * @param scan_interval where to store the property value, must be a pointer
 *        to uint16 (unsigned short *).
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_device_scan_interval_set()
 */
bool
e_connman_device_scan_interval_get(const E_Connman_Element *device, unsigned short *scan_interval)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(device, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(scan_interval, 0);
   return e_connman_element_property_get_stringshared
     (device, e_connman_prop_scan_interval, NULL, scan_interval);
}

/**
 * Call method SetProperty("ScanInterval", scan_interval) at the given element on server.
 *
 * This is a server call, not local, so it may fail and in that case
 * no property is updated locally. If the value was set the event
 * E_CONNMAN_EVENT_ELEMENT_UPDATED will be added to main loop.
 *
 * The scan interval describes the time in seconds
 * between automated scan attempts. Setting this
 * value to 0 will disable the background scanning.
 *
 * The default value is 300 and so every 5 minutes
 * a scan procedure will be triggered.
 *
 * This property is not available with all types
 * of devices. Some might not support background
 * scanning at all.
 *
 * @param device_path to set property.
 * @param scan_interval value to set.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_device_scan_interval_get()
 */
bool
e_connman_device_scan_interval_set(E_Connman_Element *device, unsigned short scan_interval, E_DBus_Method_Return_Cb cb, const void *data)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(device, 0);
   return e_connman_element_property_set_full
     (device, e_connman_prop_scan_interval, DBUS_TYPE_UINT16,
      &scan_interval, cb, data);
}

/**
 * Get property "Scanning" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * Indicates if a device is scanning. Not all device
 * types might support this. Also some hardware might
 * execute background scanning without notifying the
 * driver about it. Use this property only for visual
 * indication.
 *
 * @param device_path to get property.
 * @param scanning where to store the property value, must be a pointer
 *        to boolean (bool *).
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_device_scanning_get(const E_Connman_Element *device, bool *scanning)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(device, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(scanning, 0);
   return e_connman_element_property_get_stringshared
     (device, e_connman_prop_scanning, NULL, scanning);
}

/**
 * Get array of network elements.
 *
 * @param device_path to get property.
 * @param count return the number of elements in array.
 * @param elements where to store elements array, just changed if return is 1.
 *        Elements are not referenced and in no particular order.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_device_networks_get(const E_Connman_Element *device, unsigned int *count, E_Connman_Element ***elements)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(device, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(count, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(elements, 0);
   return e_connman_element_objects_array_get_stringshared
     (device, e_connman_prop_networks, count, elements);
}
