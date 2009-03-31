#include "e_connman_private.h"

E_Connman_Element *
e_connman_connection_get(const char *path)
{
   E_Connman_Element *connection;

   EINA_SAFETY_ON_NULL_RETURN_VAL(path, NULL);

   connection = e_connman_element_get(path);
   if (!connection)
     return NULL;

   if (!e_connman_element_is_connection(connection))
     {
	WRN("path '%s' is not a connection!\n", path);
	return NULL;
     }

   return connection;
}

/**
 * Get property "Type" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * The connection type (for example "wifi", etc.)
 *
 * @param connection_path to get property.
 * @param type where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_connection_type_get(const E_Connman_Element *connection, const char **type)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(connection, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(type, 0);
   return e_connman_element_property_get_stringshared
     (connection, e_connman_prop_type, NULL, type);
}

/**
 * Get property "Interface" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * The connection interface (for example "eth0" etc.)
 *
 * This value is for pure informational purposes. It
 * is not guaranteed that it is always present.
 *
 * @param connection_path to get property.
 * @param interface where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_connection_interface_get(const E_Connman_Element *connection, const char **interface)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(connection, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(interface, 0);
   return e_connman_element_property_get_stringshared
     (connection, e_connman_prop_interface, NULL, interface);
}

/**
 * Get property "Strength" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * Indicates the signal strength of the connection.
 *
 * This property is optional and not always present.
 *
 * @param connection_path to get property.
 * @param strength where to store the property value, must be a pointer
 *        to byte (unsigned char*).
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_connection_strength_get(const E_Connman_Element *connection, unsigned char *strength)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(connection, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(strength, 0);
   return e_connman_element_property_get_stringshared
     (connection, e_connman_prop_strengh, NULL, strength);
}

/**
 * Get property "Default" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * Indicates if it is a default connection. It is
 * possible to have multiple default connections.
 *
 * @param connection_path to get property.
 * @param connection_default where to store the property value, must be a
 *        pointer to boolean (bool *).
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_connection_default_get(const E_Connman_Element *connection, bool *connection_default)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(connection, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(connection_default, 0);
   return e_connman_element_property_get_stringshared
     (connection, e_connman_prop_default, NULL, connection_default);
}

/**
 * Get the device element this connection has
 * been established with.
 *
 * @param connection_path to get property.
 * @param element where to store element, just changed if return is 1
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_connection_device_get(const E_Connman_Element *connection, E_Connman_Element **element)
{
   const char *device_path;
   EINA_SAFETY_ON_NULL_RETURN_VAL(connection, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   if (!e_connman_element_property_get_stringshared
       (connection, e_connman_prop_device, NULL, &device_path))
     return 0;
   *element = e_connman_element_get(device_path);
   return 1;
}

/**
 * Get the network element this connection
 * belongs to.
 *
 * @param connection_path to get property.
 * @param element where to store element, just changed if return is 1
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_connection_network_get(const E_Connman_Element *connection, E_Connman_Element **element)
{
   const char *network_path;
   EINA_SAFETY_ON_NULL_RETURN_VAL(connection, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, 0);
   if (!e_connman_element_property_get_stringshared
       (connection, e_connman_prop_network, NULL, &network_path))
     return 0;
   *element = e_connman_element_get(network_path);
   return 1;
}

/**
 * Get property "IPv4.Method" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * Indicates the way how the IPv4 settings were
 * configured. Possible values here are "dhcp"
 * and "static".
 *
 * @param connection_path to get property.
 * @param method where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_connection_ipv4_method_get(const E_Connman_Element *connection, const char **method)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(connection, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(method, 0);
   return e_connman_element_property_get_stringshared
     (connection, e_connman_prop_ipv4_method, NULL, method);
}

/**
 * Get property "IPv4.Address" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * Shows the current configured IPv4 address.
 *
 * @param connection_path to get property.
 * @param address where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return 1 on success, 0 otherwise.
 */
bool
e_connman_connection_ipv4_address_get(const E_Connman_Element *connection, const char **address)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(connection, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(address, 0);
   return e_connman_element_property_get_stringshared
     (connection, e_connman_prop_ipv4_address, NULL, address);
}
