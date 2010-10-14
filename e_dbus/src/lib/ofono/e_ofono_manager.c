#include "e_ofono_private.h"

/**
 * Get the element manager.
 *
 * @return element pointer if found, NULL otherwise.
 */
E_Ofono_Element *
e_ofono_manager_get(void)
{
   return e_ofono_element_get(manager_path, e_ofono_iface_manager);
}

/**
 * Get array of modem object paths.
 *
 * If this property isn't found then 0 is returned.
 * If zero is returned, then this call failed and parameter-returned
 * values shall be considered invalid.
 *
 * @param array where to store the property value, must be a pointer
 *        to Eina_Array (Eina_Array **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_ofono_manager_modems_get(Eina_Array **array)
{
   E_Ofono_Element *element;
   E_Ofono_Array *a = NULL;
   Eina_Bool r;

   EINA_SAFETY_ON_NULL_RETURN_VAL(array, EINA_FALSE);

   element = e_ofono_element_get(manager_path, e_ofono_iface_manager);
   r = e_ofono_element_property_get_stringshared
         (element, e_ofono_prop_modems, NULL, &a);
   if (a)
      *array = a->array;

   return r;
}

