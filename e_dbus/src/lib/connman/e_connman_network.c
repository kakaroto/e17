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
        WRN("path '%s' is not a network!", path);
        return NULL;
     }

   return network;
}

/**
 * Get property "Address" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * The network hardware address (mac-address for ethernet, wifi...).
 *
 * This address can be used for directly displaying it in
 * the application. It has pure informational purpose.
 *
 * @param network path to get property.
 * @param address where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_network_address_get(const E_Connman_Element *network, const char **address)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(network, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(address, 0);
   return e_connman_element_property_get_stringshared
             (network, e_connman_prop_address, NULL, address);
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
 * @param network path to get property.
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
 * Get property "Connected" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * Indicates that this network is currently connected.
 *
 * @param network path to get property.
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
 * Get property "Strength" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * Indicates the signal strength of the network. This
 * is a normalized value between 0 and 100.
 *
 * @param network path to get property.
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
             (network, e_connman_prop_strength, NULL, strength);
}

/**
 * Get property "Frequency" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * Indicates the frequency of the network.
 *
 * @param network path to get property.
 * @param frequency where to store the property value, must be a pointer
 *        to uint16 (unsigned short*).
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_network_frequency_get(const E_Connman_Element *network, unsigned short *frequency)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(network, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(frequency, 0);
   return e_connman_element_property_get_stringshared
             (network, e_connman_prop_frequency, NULL, frequency);
}

/**
 * Get the device element this network
 * belongs to.
 *
 * @param network path to get property.
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
 * @param network path to get property.
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
 * @param network path to get property.
 * @param wifi_mode where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
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
 * @param network path to get property.
 * @param wifi_security where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
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
 * @param network path to get property.
 * @param wifi_passphrase where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
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
 * Get property "WiFi.Channel" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * Indicates the channel this network is.
 *
 * @param network path to get property.
 * @param channel where to store the property value, must be a pointer
 *        to uint16 (unsigned short*).
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_network_wifi_channel_get(const E_Connman_Element *network, unsigned short *wifi_channel)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(network, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(wifi_channel, 0);
   return e_connman_element_property_get_stringshared
             (network, e_connman_prop_wifi_channel, NULL, wifi_channel);
}

/**
 * Get property "WiFi.EAP" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * This property is only available if WiFi.Security is "ieee8021x".
 *
 * For systems using PolicyKit, the access to this value
 * will be protected by the security policy.
 *
 * @param network path to get property.
 * @param wifi_eap where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_network_wifi_eap_get(const E_Connman_Element *network, const char **wifi_eap)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(network, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(wifi_eap, 0);
   return e_connman_element_property_get_stringshared
             (network, e_connman_prop_wifi_eap, NULL, wifi_eap);
}

