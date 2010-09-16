#include "e_bluez_private.h"

/**
 * Free a E_Bluez_Device_Found struct
 *
 * @param device the struct to be freed
 */
void
e_bluez_devicefound_free(E_Bluez_Device_Found *device)
{
   EINA_SAFETY_ON_NULL_RETURN(device);

   eina_stringshare_del(device->name);
   e_bluez_element_array_free(device->array, NULL);
}

/**
 * Return the pointer to the stringshared alias for the given found device.
 *
 * @return stringshared pointer, or @c NULL if unknown.
 */
const char *
e_bluez_devicefound_alias_get(const E_Bluez_Device_Found *device)
{
   E_Bluez_Element_Dict_Entry *entry;

   EINA_SAFETY_ON_NULL_RETURN_VAL(device, NULL);

   entry = e_bluez_element_array_dict_find_stringshared
         (device->array, e_bluez_prop_alias);

   if ((entry) && (entry->type == DBUS_TYPE_STRING))
      return entry->value.str;

   return NULL;
}

