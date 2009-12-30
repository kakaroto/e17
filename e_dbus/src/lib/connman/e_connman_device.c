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
	WRN("path '%s' is not a device!", path);
	return NULL;
     }

   return device;
}

/**
 * Propose device to scan.
 *
 * Call method ProposeScan() at the given device on server in order to propose it
 * to scan.
 *
 * @param device path to call method on server.
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
 * Get property "Address" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * The device address (mac-address for ethernet, wifi...).
 *
 * This address can be used for directly displaying it in
 * the application. It has pure informational purpose.
 *
 * @param device path to get property.
 * @param address where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_device_address_get(const E_Connman_Element *device, const char **address)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(device, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(address, 0);
   return e_connman_element_property_get_stringshared
     (device, e_connman_prop_address, NULL, address);
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
 * @param device path to get property.
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
 * @param device path to get property.
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
 * @param device path to get property.
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
 * @param device path to get property.
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
 * @param device path to set property.
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
 * @param device path to get property.
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
 * @param device path to set property.
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
 * @param device path to get property.
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
 * @param device path to get property.
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
