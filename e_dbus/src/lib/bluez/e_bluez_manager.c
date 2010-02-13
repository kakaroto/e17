#include "e_bluez_private.h"

/**
 * Get the element manager.
 *
 * @return element pointer if found, NULL otherwise.
 */
E_Bluez_Element *
e_bluez_manager_get(void)
{
   return e_bluez_element_get(manager_path);
}
