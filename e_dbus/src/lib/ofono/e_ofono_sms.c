#include "e_ofono_private.h"

/**
 * Get property "ServiceCenterAddress" value.
 *
 * @param sca where to store the property value, must be a pointer
 *        to string (const char **), it will not be allocated or
 *        copied and references will be valid until element changes,
 *        so copy it if you want to use it later.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_ofono_sms_sca_get(const E_Ofono_Element *element, const char **sca)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(sca, EINA_FALSE);

   return e_ofono_element_property_get_stringshared
             (element, e_ofono_prop_sca, NULL, sca);
}

/**
 * Call method SetProperty("ServiceCenterAddress", powered) at the given
 * element on server.
 *
 *
 * @param sca value to set.
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_ofono_sms_sca_set(E_Ofono_Element *element, const char *sca, E_DBus_Method_Return_Cb cb, const void *data)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, EINA_FALSE);

   return e_ofono_element_property_set_full
             (element, e_ofono_prop_sca, DBUS_TYPE_STRING, sca, cb, data);
}

/**
 * Send SMS message.
 *
 * Call method SendMessage(number, text) to send a new SMS message.
 *
 * @param number the destination of the message
 * @param message text of message body
 * @param cb function to call when server replies or some error happens.
 * @param data data to give to cb when it is called.
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
Eina_Bool
e_ofono_sms_send_message(E_Ofono_Element *element, const char *number, const char *message, E_DBus_Method_Return_Cb cb, const void *data)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(element, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(number, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(message, EINA_FALSE);

   DBusMessage *m;
   DBusMessageIter i;

   if (!(m = dbus_message_new_method_call(e_ofono_system_bus_name_get(),
                                          element->path, element->interface,
                                          e_ofono_method_send_sms)))
      return EINA_FALSE;

   dbus_message_iter_init_append(m, &i);
   dbus_message_iter_append_basic(&i, DBUS_TYPE_STRING, &number);
   dbus_message_iter_append_basic(&i, DBUS_TYPE_STRING, &message);

   return e_ofono_element_message_send(element, e_ofono_method_send_sms,
                                       e_ofono_iface_sms, NULL, m,
                                       &element->_pending.send_sms, cb, data);
}

