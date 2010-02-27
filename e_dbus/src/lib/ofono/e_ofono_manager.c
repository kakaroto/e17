#include "e_ofono_private.h"

/**
 * Get the element manager.
 *
 * @return element pointer if found, NULL otherwise.
 */
E_Ofono_Element *
e_ofono_manager_get(void)
{
   return e_ofono_element_get(manager_path);
}
