#ifndef E_CONNMAN_H
#define E_CONNMAN_H

#include <stdint.h>
#include <stdio.h>

#include <Eina.h>
#include <Ecore.h>
#include <E_DBus.h>

/**
 * @defgroup EConnman_Group EConnman
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Ecore Events */
extern int E_CONNMAN_EVENT_MANAGER_IN;
extern int E_CONNMAN_EVENT_MANAGER_OUT;
extern int E_CONNMAN_EVENT_ELEMENT_ADD;
extern int E_CONNMAN_EVENT_ELEMENT_DEL;
extern int E_CONNMAN_EVENT_ELEMENT_UPDATED;

typedef struct _E_Connman_Element   E_Connman_Element;

struct _E_Connman_Element
{
   const char            *path;
   const char            *interface;
   E_DBus_Signal_Handler *signal_handler;
   Eina_Inlist           *props;

   /* private */
   struct
   {
      Eina_Inlist *properties_get;
      Eina_Inlist *property_set;
      Eina_Inlist *agent_register;
      Eina_Inlist *agent_unregister;
      Eina_Inlist *request_scan;
      Eina_Inlist *technology_enable;
      Eina_Inlist *technology_disable;
      Eina_Inlist *profile_remove;
      Eina_Inlist *service_connect;
      Eina_Inlist *service_disconnect;
      Eina_Inlist *service_remove;
      Eina_Inlist *service_move_before;
      Eina_Inlist *service_move_after;
      Eina_Inlist *service_clear_property;
   } _pending;
   struct
   {
      Ecore_Idler *changed;
   } _idler;
   Eina_Inlist *_listeners;
   int          _references;
};

typedef struct _E_Connman_Array E_Connman_Array;
struct _E_Connman_Array
{
   int         type;
   Eina_Array *array;
};

/* General Public API */
EAPI unsigned int           e_connman_system_init(E_DBus_Connection *edbus_conn) EINA_ARG_NONNULL(1);
EAPI unsigned int           e_connman_system_shutdown(void);

/* Manager Methods */
EAPI E_Connman_Element *    e_connman_manager_get(void) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_manager_agent_register(const char *object_path, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_manager_agent_unregister(const char *object_path, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_manager_state_get(const char **state) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_manager_offline_mode_get(Eina_Bool *offline) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_manager_offline_mode_set(Eina_Bool offline, E_DBus_Method_Return_Cb cb, const void *data) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_manager_profiles_get(unsigned int *count, E_Connman_Element ***p_elements) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_manager_services_get(unsigned int *count, E_Connman_Element ***p_elements) EINA_ARG_NONNULL(1, 2)  EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_manager_technologies_get(unsigned int *count, E_Connman_Element ***p_elements) EINA_ARG_NONNULL(1, 2)  EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_manager_request_scan(const char *type, E_DBus_Method_Return_Cb cb, const void *data) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_manager_technology_default_get(const char **type) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_manager_technology_enable(const char *type, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_manager_technology_disable(const char *type, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_manager_technologies_available_get(unsigned int *count, const char ***strings) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_manager_technologies_enabled_get(unsigned int *count, const char ***strings) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_manager_technologies_connected_get(unsigned int *count, const char ***strings) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_manager_profile_remove(const E_Connman_Element *profile, E_DBus_Method_Return_Cb cb, const void *data) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_manager_profile_active_get(E_Connman_Element **profile) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_manager_profile_active_set(const E_Connman_Element *profile, E_DBus_Method_Return_Cb cb, const void *data) EINA_WARN_UNUSED_RESULT;

// TODO: profile_create()
// TODO: service_connect() (actually creates and connect)
// TODO: signal E_CONNMAN_EVENT_MANAGER_STATE_CHANGED

/* Profile Methods */
EAPI E_Connman_Element *    e_connman_profile_get(const char *path) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_profile_name_get(const E_Connman_Element *profile, const char **name) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_profile_name_set(E_Connman_Element *profile, const char *name, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_profile_offline_mode_get(const E_Connman_Element *profile, Eina_Bool *offline) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_profile_offline_mode_set(E_Connman_Element *profile, Eina_Bool offline, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_profile_services_get(const E_Connman_Element *profile, unsigned int *count, E_Connman_Element ***p_elements) EINA_ARG_NONNULL(1, 2, 3) EINA_WARN_UNUSED_RESULT;

/* Services Methods */
EAPI E_Connman_Element *    e_connman_service_get(const char *path) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_service_clear_property(E_Connman_Element *service, const char *property, E_DBus_Method_Return_Cb cb, const void *data);

EAPI Eina_Bool              e_connman_service_remove(E_Connman_Element *service, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_service_connect(E_Connman_Element *service, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_disconnect(E_Connman_Element *service, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_service_move_before(E_Connman_Element *service, const char *object_path, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_move_after(E_Connman_Element *service, const char *object_path, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_service_state_get(const E_Connman_Element *service, const char **state) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_error_get(const E_Connman_Element *service, const char **error) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_name_get(const E_Connman_Element *service, const char **name) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_type_get(const E_Connman_Element *service, const char **type) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_security_get(const E_Connman_Element *service, const E_Connman_Array **security) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_service_passphrase_get(const E_Connman_Element *service, const char **passphrase) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_passphrase_set(E_Connman_Element *service, const char *passphrase, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_service_strength_get(const E_Connman_Element *service, unsigned char *strength) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_service_favorite_get(const E_Connman_Element *service, Eina_Bool *favorite) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_immutable_get(const E_Connman_Element *service, Eina_Bool *immutable) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_service_auto_connect_get(const E_Connman_Element *service, Eina_Bool *auto_connect) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_auto_connect_set(E_Connman_Element *service, Eina_Bool auto_connect, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_service_passphrase_required_get(const E_Connman_Element *service, Eina_Bool *passphrase_required) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_login_required_get(const E_Connman_Element *service, Eina_Bool *login_required) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_service_roaming_get(const E_Connman_Element *service, Eina_Bool *roaming) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_nameservers_get(const E_Connman_Element *service, unsigned int *count, const char ***nameserver) EINA_ARG_NONNULL(1, 2, 3) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_domains_get(const E_Connman_Element *service, unsigned int *count, const char ***domains) EINA_ARG_NONNULL(1, 2, 3) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_service_ipv4_method_get(const E_Connman_Element *service, const char **method) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_ipv4_address_get(const E_Connman_Element *service, const char **address) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_ipv4_gateway_get(const E_Connman_Element *service, const char **gateway) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_ipv4_netmask_get(const E_Connman_Element *service, const char **netmask) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_service_ipv4_configuration_method_get(const E_Connman_Element *service, const char **method) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_ipv4_configuration_address_get(const E_Connman_Element *service, const char **address) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_ipv4_configuration_gateway_get(const E_Connman_Element *service, const char **gateway) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_ipv4_configuration_netmask_get(const E_Connman_Element *service, const char **netmask) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_service_ethernet_method_get(const E_Connman_Element *service, const char **method) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_ethernet_interface_get(const E_Connman_Element *service, const char **iface) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_ethernet_address_get(const E_Connman_Element *service, const char **address) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_ethernet_mtu_get(const E_Connman_Element *service, unsigned short *mtu) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_ethernet_speed_get(const E_Connman_Element *service, unsigned short *speed) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_ethernet_duplex_get(const E_Connman_Element *service, const char **duplex) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;

/* Methods to configure IPv4 service */
EAPI Eina_Bool              e_connman_service_ipv4_configure_dhcp(E_Connman_Element *service, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_service_ipv4_configure_manual(E_Connman_Element *service, const char *address, const char *netmask, const char *gateway, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;

/* Technologies methods */
EAPI E_Connman_Element *    e_connman_technology_get(const char *path) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_technology_state_get(const E_Connman_Element *technology, const char **state) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_technology_name_get(const E_Connman_Element *technology, const char **state) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_technology_type_get(const E_Connman_Element *technology, const char **state) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;

/* Low-Level API:
 *
 * Should just be used to work around problems until proper solution
 * is made into e_connman.
 */
EAPI Eina_Bool              e_connman_manager_sync_elements(void);

EAPI Eina_Bool              e_connman_elements_get_all(unsigned int *count, E_Connman_Element ***p_elements) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_elements_get_all_type(const char *type, unsigned int *count, E_Connman_Element ***p_elements) EINA_ARG_NONNULL(1, 2, 3) EINA_WARN_UNUSED_RESULT;
EAPI E_Connman_Element *    e_connman_element_get(const char *path);

EAPI void                   e_connman_element_listener_add(E_Connman_Element *element, void (*cb)(void *data, const E_Connman_Element *element), const void *data, void (*free_data)(void *data)) EINA_ARG_NONNULL(1, 2);
EAPI void                   e_connman_element_listener_del(E_Connman_Element *element, void (*cb)(void *data, const E_Connman_Element *element), const void *data) EINA_ARG_NONNULL(1, 2);

EAPI int                    e_connman_element_ref(E_Connman_Element *element) EINA_ARG_NONNULL(1);
EAPI int                    e_connman_element_unref(E_Connman_Element *element) EINA_ARG_NONNULL(1);

EAPI void                   e_connman_element_print(FILE *fp, const E_Connman_Element *element) EINA_ARG_NONNULL(1, 2);

EAPI Eina_Bool              e_connman_element_properties_sync(E_Connman_Element *element) EINA_ARG_NONNULL(1);
EAPI Eina_Bool              e_connman_element_properties_sync_full(E_Connman_Element *element, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1);
EAPI void                   e_connman_element_properties_list(const E_Connman_Element *element, Eina_Bool (*cb)(void *data, const E_Connman_Element *element, const char *name, int type, const void *value), const void *data) EINA_ARG_NONNULL(1, 2);

EAPI Eina_Bool              e_connman_element_property_set(E_Connman_Element *element, const char *prop, int type, const void *value) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_element_property_set_full(E_Connman_Element *element, const char *prop, int type, const void *value, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_element_property_dict_set_full(E_Connman_Element *element, const char *prop, const char *key, int type, const void *value, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1, 2, 3) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_element_property_type_get_stringshared(const E_Connman_Element *element, const char *name, int *type) EINA_ARG_NONNULL(1, 2, 3) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_element_property_type_get(const E_Connman_Element *element, const char *name, int *type) EINA_ARG_NONNULL(1, 2, 3) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_element_property_dict_get_stringshared(const E_Connman_Element *element, const char *dict_name, const char *key_name, int *type, void *value) EINA_ARG_NONNULL(1, 2, 4) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_element_property_get_stringshared(const E_Connman_Element *element, const char *name, int *type, void *value) EINA_ARG_NONNULL(1, 2, 4) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_element_property_get(const E_Connman_Element *element, const char *name, int *type, void *value) EINA_ARG_NONNULL(1, 2, 4) EINA_WARN_UNUSED_RESULT;

EAPI Eina_Bool              e_connman_element_is_manager(const E_Connman_Element *element) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_element_is_profile(const E_Connman_Element *element) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_element_is_service(const E_Connman_Element *element) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;
EAPI Eina_Bool              e_connman_element_is_technology(const E_Connman_Element *element) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* E_CONNMAN_H */
