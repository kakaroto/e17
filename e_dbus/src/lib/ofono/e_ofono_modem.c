#include "e_ofono_private.h"

/* TODO: add a getter for Interfaces property */

/**
 * Get property "Powered" value.
 *
 * @param powered where to store the property value, must be a pointer
 *        to Eina_Bool (Eina_Bool *), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_ofono_modem_powered_get(const E_Ofono_Element *element, Eina_Bool *powered)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(powered, EINA_FALSE);

   return e_ofono_element_property_get_stringshared
             (element, e_ofono_prop_powered, NULL, powered);
}

/**
 * Call method SetProperty("Powered", powered) at the given element on server.
 *
 *
 * @param powered value to set.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_ofono_modem_powered_set(E_Ofono_Element *element, Eina_Bool powered, E_DBus_Method_Return_Cb cb, const void *data)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, EINA_FALSE);

   return e_ofono_element_property_set_full
             (element, e_ofono_prop_powered, DBUS_TYPE_BOOLEAN,
             &powered, cb, data);
}

/**
 * Get property "Name" value.
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
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_ofono_modem_name_get(const E_Ofono_Element *element, const char **name)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, EINA_FALSE);

   return e_ofono_element_property_get_stringshared
             (element, e_ofono_prop_name, NULL, name);
}

