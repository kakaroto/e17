#include "e_connman_private.h"

E_Connman_Element *
e_connman_network_get(const char *path)
{
   E_Connman_Element *network;

   EINA_SAFETY_ON_NULL_RETURN_VAL(path, NULL);

   network = e_connman_element_get(path);
   if (!network)
     return NULL;

   if (!e_connman_element_is_network(network))
     {
	WRN("path '%s' is not a network!\n", path);
	return NULL;
     }

   return network;
}

/**
 * Connect to network.
 *
 * Call method Connect() at the given network on server in order to
 * connect to it.
 *
 * @param network_path to call method on server.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_network_connect(E_Connman_Element *network, E_DBus_Method_Return_Cb cb, const void *data)
{
   const char name[] = "Connect";

   EINA_SAFETY_ON_NULL_RETURN_VAL(network, 0);
   return e_connman_element_call_full
     (network, name, NULL, &network->_pending.network_connect, cb, data);
}

/**
 * Disconnect from network.
 *
 * Call method Disconnect() at the given network on server in order to
 * disconnect from it.
 *
 * @param network_path to call method on server.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_network_disconnect(E_Connman_Element *network, E_DBus_Method_Return_Cb cb, const void *data)
{
   const char name[] = "Disconnect";

   EINA_SAFETY_ON_NULL_RETURN_VAL(network, 0);
   return e_connman_element_call_full
     (network, name, NULL, &network->_pending.network_disconnect, cb, data);
}

/**
 * Get property "Name" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * The pretty/long version of the network name. For
 * example in case of WiFi this should be the UTF-8
 * valid version of the SSID.
 *
 * @param network_path to get property.
 * @param name where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_network_name_get(const E_Connman_Element *network, const char **name)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(network, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, 0);
   return e_connman_element_property_get_stringshared
     (network, e_connman_prop_name, NULL, name);
}

/**
 * Get property "Available" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * Indicates that this network is in range and
 * ready to be used.
 *
 * The scanning process can change this property.
 *
 * @param network_path to get property.
 * @param available where to store the property value, must be a pointer
 *        to boolean (bool **).
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_network_available_get(const E_Connman_Element *network, bool *available)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(network, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(available, 0);
   return e_connman_element_property_get_stringshared
     (network, e_connman_prop_available, NULL, available);
}

/**
 * Get property "Connected" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * Indicates that this network is currently connected.
 *
 * @param network_path to get property.
 * @param connected where to store the property value, must be a pointer
 *        to boolean (bool **).
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_network_connected_get(const E_Connman_Element *network, bool *connected)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(network, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(connected, 0);
   return e_connman_element_property_get_stringshared
     (network, e_connman_prop_connected, NULL, connected);
}

/**
 * Get property "Remember" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * Indicates that this network will be remembered.
 *
 * For manually created networks this is set by
 * default.
 *
 * @param network_path to get property.
 * @param remember where to store the property value, must be a pointer
 *        to boolean (bool **).
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_network_remember_set()
 */
bool
e_connman_network_remember_get(const E_Connman_Element *network, bool *remember)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(network, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(remember, 0);
   return e_connman_element_property_get_stringshared
     (network, e_connman_prop_remember, NULL, remember);
}

/**
 * Call method SetProperty("Remember", remember) at the given element on server.
 *
 * This is a server call, not local, so it may fail and in that case
 * no property is updated locally. If the value was set the event
 * E_CONNMAN_EVENT_ELEMENT_UPDATED will be added to main loop.
 *
 * Indicates that this network will be remembered.
 *
 * For manually created networks this is set by
 * default.
 *
 * @param network_path to set property.
 * @param remember value to set.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_network_remember_get()
 */
bool
e_connman_network_remember_set(E_Connman_Element *network, bool remember, E_DBus_Method_Return_Cb cb, const void *data)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(network, 0);
   return e_connman_element_property_set_full
     (network, e_connman_prop_remember, DBUS_TYPE_BOOLEAN, &remember, cb, data);
}

/**
 * Get property "Strength" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * Indicates the signal strength of the network. This
 * is a normalized value between 0 and 100.
 *
 * @param network_path to get property.
 * @param strength where to store the property value, must be a pointer
 *        to byte (unsigned char*).
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_network_strength_get(const E_Connman_Element *network, unsigned char *strength)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(network, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(strength, 0);
   return e_connman_element_property_get_stringshared
     (network, e_connman_prop_strengh, NULL, strength);
}

/**
 * Get the device element this network
 * belongs to.
 *
 * @param network_path to get property.
 * @param element where to store element, just changed if return is 1
 *
 * @return 1 on success, 0 otherwise
 */
bool
e_connman_network_device_get(const E_Connman_Element *network, E_Connman_Element **element)
{
   char *device_path;

   EINA_SAFETY_ON_NULL_RETURN_VAL(network, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);

   if (!e_connman_element_property_get_stringshared
       (network, e_connman_prop_device, NULL, &device_path))
     return 0;
   *element = e_connman_element_get(device_path);
   return 1;
}

/**
 * Get property "WiFi.SSID" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * If the network type is WiFi, then this property is
 * present and contains the binary SSID value.
 *
 * @param network_path to get property.
 * @param count return the number of elements in array.
 * @param wifi_ssid where to store the property value, must be a pointer
 *        to array of bytes (unsigned char **).
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_network_wifi_ssid_get(const E_Connman_Element *network, unsigned int *count, unsigned char **wifi_ssid)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(network, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(wifi_ssid, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(count, 0);
   *wifi_ssid = e_connman_element_bytes_array_get_stringshared
     (network, e_connman_prop_wifi_ssid, count);
   if (!*wifi_ssid)
     return 0;
   return 1;
}

/**
 * Get property "WiFi.Mode" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * If the network type is WiFi, then this property is
 * present and contains the mode of the network. The
 * possible values are "managed" or "adhoc".
 *
 * For scanned networks this value is read only, but in
 * case the network was manually created it is also
 * changeable.
 *
 * @param network_path to get property.
 * @param wifi_mode where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_network_wifi_mode_set()
 */
bool
e_connman_network_wifi_mode_get(const E_Connman_Element *network, const char **wifi_mode)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(network, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(wifi_mode, 0);
   return e_connman_element_property_get_stringshared
     (network, e_connman_prop_wifi_mode, NULL, wifi_mode);
}

/**
 * Call method SetProperty("WiFi.Mode", wifi_mode) at the given element on server.
 *
 * This is a server call, not local, so it may fail and in that case
 * no property is updated locally. If the value was set the event
 * E_CONNMAN_EVENT_ELEMENT_UPDATED will be added to main loop.
 *
 * If the network type is WiFi, then this property is
 * present and contains the mode of the network. The
 * possible values are "managed" or "adhoc".
 *
 * For scanned networks this value is read only, but in
 * case the network was manually created it is also
 * changeable.
 *
 * @param network_path to set property.
 * @param wifi_mode value to set.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_network_wifi_mode_get()
 */
bool
e_connman_network_wifi_mode_set(E_Connman_Element *network, const char *wifi_mode, E_DBus_Method_Return_Cb cb, const void *data)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(network, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(wifi_mode, 0);
   return e_connman_element_property_set_full
     (network, e_connman_prop_wifi_mode, DBUS_TYPE_STRING, wifi_mode, cb, data);
}

/**
 * Get property "WiFi.Security" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * If the network type is WiFi, then this property is
 * present and contains the security method or key
 * management setting.
 *
 * For scanned networks this value is read only, but in
 * case the network was manually created it is also
 * changeable.
 *
 * Possible values are "none", "wep", "wpa" and "wpa2".
 *
 * @param network_path to get property.
 * @param wifi_security where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_network_wifi_security_set()
 */
bool
e_connman_network_wifi_security_get(const E_Connman_Element *network, const char **wifi_security)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(network, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(wifi_security, 0);
   return e_connman_element_property_get_stringshared
     (network, e_connman_prop_wifi_security, NULL, wifi_security);
}

/**
 * Call method SetProperty("WiFi.Security", wifi_security) at the given element on server.
 *
 * This is a server call, not local, so it may fail and in that case
 * no property is updated locally. If the value was set the event
 * E_CONNMAN_EVENT_ELEMENT_UPDATED will be added to main loop.
 *
 * If the network type is WiFi, then this property is
 * present and contains the security method or key
 * management setting.
 *
 * For scanned networks this value is read only, but in
 * case the network was manually created it is also
 * changeable.
 *
 * Possible values are "none", "wep", "wpa" and "wpa2".
 *
 * @param network_path to set property.
 * @param wifi_security value to set.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_network_wifi_security_get()
 */
bool
e_connman_network_wifi_security_set(E_Connman_Element *network, const char *wifi_security, E_DBus_Method_Return_Cb cb, const void *data)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(network, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(wifi_security, 0);
   return e_connman_element_property_set_full
     (network, e_connman_prop_wifi_security, DBUS_TYPE_STRING,
      wifi_security, cb, data);
}

/**
 * Get property "WiFi.Passphrase" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * If the network type is WiFi and a passhrase is
 * requires, then this property is present and contains
 * the passphrase in clear text.
 *
 * For systems using PolicyKit, the access to this value
 * will be protected by the security policy.
 *
 * @param network_path to get property.
 * @param wifi_passphrase where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_network_wifi_passphrase_set()
 */
bool
e_connman_network_wifi_passphrase_get(const E_Connman_Element *network, const char **wifi_passphrase)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(network, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(wifi_passphrase, 0);
   return e_connman_element_property_get_stringshared
     (network, e_connman_prop_wifi_passphrase, NULL, wifi_passphrase);
}

/**
 * Call method SetProperty("WiFi.Passphrase", wifi_passphrase) at the given element on server.
 *
 * This is a server call, not local, so it may fail and in that case
 * no property is updated locally. If the value was set the event
 * E_CONNMAN_EVENT_ELEMENT_UPDATED will be added to main loop.
 *
 * If the network type is WiFi and a passhrase is
 * requires, then this property is present and contains
 * the passphrase in clear text.
 *
 * For systems using PolicyKit, the access to this value
 * will be protected by the security policy.
 *
 * @param network_path to set property.
 * @param wifi_passphrase value to set.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return 1 on success, 0 otherwise.
 * @see e_connman_network_wifi_passphrase_get()
 */
bool
e_connman_network_wifi_passphrase_set(E_Connman_Element *network, const char *wifi_passphrase, E_DBus_Method_Return_Cb cb, const void *data)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(network, 0);
   return e_connman_element_property_set_full
     (network, e_connman_prop_wifi_passphrase, DBUS_TYPE_STRING,
      wifi_passphrase, cb, data);
}
