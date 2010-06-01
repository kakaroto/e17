#ifndef E_UKIT_PRIVATE_H
#define E_UKIT_PRIVATE_H

#ifndef E_DBUS_COLOR_DEFAULT
# define E_DBUS_COLOR_DEFAULT EINA_COLOR_CYAN
#endif

extern int _e_dbus_ukit_log_dom;

#ifdef ERR
# undef ERR
#endif

#ifdef INF
# undef INF
#endif

#ifdef WARN
# undef WARN
#endif

#ifdef DBG
# undef DBG
#endif

#define e_ukit_call_new(udi, member) dbus_message_new_method_call(E_UKIT_BUS, udi, E_UKIT_BUS, member)
#define e_ukit_device_call_new(udi, member) dbus_message_new_method_call(E_UKIT_BUS, udi, E_UKIT_INTERFACE, member)
#define e_ukit_property_call_new(udi, member) dbus_message_new_method_call(E_UKIT_BUS, udi, E_DBUS_FDO_INTERFACE_PROPERTIES, member)

#define DBG(...)   EINA_LOG_DOM_DBG(_e_dbus_ukit_log_dom, __VA_ARGS__)
#define INFO(...)    EINA_LOG_DOM_INFO(_e_dbus_ukit_log_dom, __VA_ARGS__)
#define WARN(...) EINA_LOG_DOM_WARN(_e_dbus_ukit_log_dom, __VA_ARGS__)
#define ERR(...)   EINA_LOG_DOM_ERR(_e_dbus_ukit_log_dom, __VA_ARGS__)

void *unmarshal_property(DBusMessage *msg, DBusError *err);
void  free_property(void *data);
void *unmarshal_device_get_all_properties(DBusMessage *msg, DBusError *err);
void  free_device_get_all_properties(void *data);
void *unmarshal_string_list(DBusMessage *msg, DBusError *err);
void  free_string_list(void *data);


#endif
