#include "e_ofono_private.h"

/**
 * Get property "Mode" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * @param mode where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_ofono_netreg_mode_get(const E_Ofono_Element *element, const char **mode)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(mode, EINA_FALSE);

   return e_ofono_element_property_get_stringshared
             (element, e_ofono_prop_mode, NULL, mode);
}

/**
 * Get property "Status" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * @param status where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_ofono_netreg_status_get(const E_Ofono_Element *element, const char **status)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(status, EINA_FALSE);

   return e_ofono_element_property_get_stringshared
             (element, e_ofono_prop_status, NULL, status);
}

/**
 * Get property "Operator" value.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * @param operator where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_ofono_netreg_operator_get(const E_Ofono_Element *element, const char **op)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(op, EINA_FALSE);

   return e_ofono_element_property_get_stringshared
             (element, e_ofono_prop_operator, NULL, op);
}

/**
 * Get property "Strength" value.
 *
 * @param strength where to store the property value, must be a pointer
 *        to char (char *), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_ofono_netreg_strength_get(const E_Ofono_Element *element, uint8_t *strength)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(strength, EINA_FALSE);

   return e_ofono_element_property_get_stringshared
             (element, e_ofono_prop_strength, NULL, strength);
}

