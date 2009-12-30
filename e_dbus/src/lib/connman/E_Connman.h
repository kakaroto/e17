#ifndef E_CONNMAN_H
#define E_CONNMAN_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <Eina.h>
#include <Ecore.h>
#include <E_DBus.h>

#ifdef EAPI
#undef EAPI
#endif
#ifdef _MSC_VER
# ifdef BUILDING_DLL
#  define EAPI __declspec(dllexport)
# else
#  define EAPI __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

  /* Ecore Events */
  extern int E_CONNMAN_EVENT_MANAGER_IN;
  extern int E_CONNMAN_EVENT_MANAGER_OUT;
  extern int E_CONNMAN_EVENT_ELEMENT_ADD;
  extern int E_CONNMAN_EVENT_ELEMENT_DEL;
  extern int E_CONNMAN_EVENT_ELEMENT_UPDATED;

  typedef struct _E_Connman_Element E_Connman_Element;

  struct _E_Connman_Element
  {
     const char *path;
     const char *interface;
     E_DBus_Signal_Handler *signal_handler;
     Eina_Inlist *props;

     /* private */
     struct {
	Eina_Inlist *properties_get;
	Eina_Inlist *property_set;
	Eina_Inlist *agent_register;
	Eina_Inlist *agent_unregister;
	Eina_Inlist *request_scan;
	Eina_Inlist *technology_enable;
	Eina_Inlist *technology_disable;
	Eina_Inlist *profile_remove;
	Eina_Inlist *device_propose_scan;
	Eina_Inlist *service_connect;
	Eina_Inlist *service_disconnect;
	Eina_Inlist *service_remove;
	Eina_Inlist *service_move_before;
	Eina_Inlist *service_move_after;
	Eina_Inlist *service_clear_property;
     } _pending;
     struct {
	Ecore_Idler *changed;
     } _idler;
     Eina_Inlist *_listeners;
     int _references;
  };

  /* General Public API */
  EAPI unsigned int e_connman_system_init(E_DBus_Connection *edbus_conn) EINA_ARG_NONNULL(1);
  EAPI unsigned int e_connman_system_shutdown(void);

  /* Manager Methods */
  EAPI E_Connman_Element *e_connman_manager_get(void) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_manager_agent_register(const char *object_path, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_manager_agent_unregister(const char *object_path, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_manager_state_get(const char **state) EINA_PURE EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_manager_offline_mode_get(bool *offline) EINA_PURE EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_manager_offline_mode_set(bool offline, E_DBus_Method_Return_Cb cb, const void *data) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_manager_profiles_get(unsigned int *count, E_Connman_Element ***p_elements) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_manager_devices_get(unsigned int *count, E_Connman_Element ***p_elements) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_manager_services_get(unsigned int *count, E_Connman_Element ***p_elements) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_manager_request_scan(const char *type, E_DBus_Method_Return_Cb cb, const void *data) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_manager_technology_default_get(const char **type) EINA_ARG_NONNULL(1) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_manager_technology_enable(const char *type, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_manager_technology_disable(const char *type, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_manager_profile_remove(const E_Connman_Element *profile, E_DBus_Method_Return_Cb cb, const void *data) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_manager_profile_active_get(E_Connman_Element **profile) EINA_ARG_NONNULL(1) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_manager_profile_active_set(const E_Connman_Element *profile, E_DBus_Method_Return_Cb cb, const void *data) EINA_WARN_UNUSED_RESULT;


  // TODO: profile_create()
  // TODO: technologies_available_get()
  // TODO: technologies_enabled_get()
  // TODO: technologies_connected_get()
  // TODO: service_connect() (actually creates and connect)
  // TODO: signal E_CONNMAN_EVENT_MANAGER_STATE_CHANGED


  /* Profile Methods */
  EAPI E_Connman_Element *e_connman_profile_get(const char *path) EINA_ARG_NONNULL(1) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_profile_name_get(const E_Connman_Element *profile, const char **name) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_profile_name_set(E_Connman_Element *profile, const char *name, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_profile_offline_mode_get(const E_Connman_Element *profile, bool *offline) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_profile_offline_mode_set(E_Connman_Element *profile, bool offline, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_profile_services_get(const E_Connman_Element *profile, unsigned int *count, E_Connman_Element ***p_elements) EINA_ARG_NONNULL(1, 2, 3) EINA_PURE EINA_WARN_UNUSED_RESULT;


  /* Services Methods */
  EAPI E_Connman_Element *e_connman_service_get(const char *path) EINA_ARG_NONNULL(1) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_service_connect(E_Connman_Element *service, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_service_disconnect(E_Connman_Element *service, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_service_remove(E_Connman_Element *service, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_service_clear_property(E_Connman_Element *service, const char *property, E_DBus_Method_Return_Cb cb, const void *data);

  EAPI bool e_connman_service_move_before(E_Connman_Element *service, const char *object_path, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_service_move_after(E_Connman_Element *service, const char *object_path, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_service_state_get(const E_Connman_Element *service, const char **state) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_service_error_get(const E_Connman_Element *service, const char **error) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_service_name_get(const E_Connman_Element *service, const char **name) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_service_type_get(const E_Connman_Element *service, const char **type) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_service_mode_get(const E_Connman_Element *service, const char **mode) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_service_security_get(const E_Connman_Element *service, const char **security) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_service_passphrase_get(const E_Connman_Element *service, const char **passphrase) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_service_passphrase_set(E_Connman_Element *service, const char *passphrase, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_service_passphrase_required_get(const E_Connman_Element *service, bool *passphrase_required) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_service_strength_get(const E_Connman_Element *service, unsigned char *strength) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_service_favorite_get(const E_Connman_Element *service, bool *favorite) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_service_immutable_get(const E_Connman_Element *service, bool *immutable) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_service_auto_connect_get(const E_Connman_Element *service, bool *auto_connect) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_service_auto_connect_set(E_Connman_Element *service, bool auto_connect, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_service_setup_required_get(const E_Connman_Element *service, bool *setup_required) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_service_apn_get(const E_Connman_Element *service, const char **apn) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_service_apn_set(E_Connman_Element *service, const char *apn, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_service_mcc_get(const E_Connman_Element *service, const char **mcc) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_service_mnc_get(const E_Connman_Element *service, const char **mnc) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_service_roaming_get(const E_Connman_Element *service, bool *roaming) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_service_ipv4_method_get(const E_Connman_Element *service, const char **method) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_service_ipv4_address_get(const E_Connman_Element *service, const char **address) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  // TODO: ipv4_netmask_get
  // TODO: ipv4_gateway_get
  // TODO: ipv4_configuration_metod_get
  // TODO: ipv4_configuration_metod_set
  // TODO: ipv4_configuration_address_get
  // TODO: ipv4_configuration_address_set
  // TODO: ipv4_configuration_netmask_get
  // TODO: ipv4_configuration_netmask_set
  // TODO: ipv4_configuration_gateway_get
  // TODO: ipv4_configuration_gateway_set
  // TODO: ethernet_method_get
  // TODO: ethernet_address_get
  // TODO: ethernet_mtu_get
  // TODO: ethernet_speed_get
  // TODO: ethernet_duplex_get

  /* Low-Level API:
   *
   * Should just be used to work around problems until proper solution
   * is made into e_connman.
   */
  EAPI bool e_connman_manager_sync_elements(void);

  EAPI bool e_connman_elements_get_all(unsigned int *count, E_Connman_Element ***p_elements) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_elements_get_all_type(const char *type, unsigned int *count, E_Connman_Element ***p_elements) EINA_ARG_NONNULL(1, 2, 3) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI E_Connman_Element *e_connman_element_get(const char *path);

  EAPI void e_connman_element_listener_add(E_Connman_Element *element, void (*cb)(void *data, const E_Connman_Element *element), const void *data, void (*free_data)(void *data)) EINA_ARG_NONNULL(1, 2);
  EAPI void e_connman_element_listener_del(E_Connman_Element *element, void (*cb)(void *data, const E_Connman_Element *element), const void *data) EINA_ARG_NONNULL(1, 2);

  EAPI int e_connman_element_ref(E_Connman_Element *element) EINA_ARG_NONNULL(1);
  EAPI int e_connman_element_unref(E_Connman_Element *element) EINA_ARG_NONNULL(1);

  EAPI void e_connman_element_print(FILE *fp, const E_Connman_Element *element) EINA_ARG_NONNULL(1, 2);


  EAPI bool e_connman_element_properties_sync(E_Connman_Element *element) EINA_ARG_NONNULL(1);
  EAPI bool e_connman_element_properties_sync_full(E_Connman_Element *element, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1);

  EAPI bool e_connman_element_property_set(E_Connman_Element *element, const char *prop, int type, const void *value) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_element_property_set_full(E_Connman_Element *element, const char *prop, int type, const void *value, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;

  EAPI void e_connman_element_properties_list(const E_Connman_Element *element, bool (*cb)(void *data, const E_Connman_Element *element, const char *name, int type, const void *value), const void *data) EINA_ARG_NONNULL(1, 2);

  EAPI bool e_connman_element_property_type_get_stringshared(const E_Connman_Element *element, const char *name, int *type) EINA_ARG_NONNULL(1, 2, 3) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_element_property_type_get(const E_Connman_Element *element, const char *name, int *type) EINA_ARG_NONNULL(1, 2, 3) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_element_property_get_stringshared(const E_Connman_Element *element, const char *name, int *type, void *value) EINA_ARG_NONNULL(1, 2, 4) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_element_property_get(const E_Connman_Element *element, const char *name, int *type, void *value) EINA_ARG_NONNULL(1, 2, 4) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_element_is_manager(const E_Connman_Element *element) EINA_ARG_NONNULL(1) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_element_is_device(const E_Connman_Element *element) EINA_ARG_NONNULL(1) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_element_is_profile(const E_Connman_Element *element) EINA_ARG_NONNULL(1) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_element_is_connection(const E_Connman_Element *element) EINA_ARG_NONNULL(1) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_element_is_network(const E_Connman_Element *element) EINA_ARG_NONNULL(1) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_element_is_service(const E_Connman_Element *element) EINA_ARG_NONNULL(1) EINA_PURE EINA_WARN_UNUSED_RESULT;


  /* Device Methods: Low-Level API
   *
   * Fine tune device, such as turn off and change scan interval.
   */
  EAPI E_Connman_Element *e_connman_device_get(const char *path) EINA_ARG_NONNULL(1) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_device_propose_scan(E_Connman_Element *device, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_device_address_get(const E_Connman_Element *device, const char **address);

  EAPI bool e_connman_device_name_get(const E_Connman_Element *device, const char **name) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_device_type_get(const E_Connman_Element *device, const char **type) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_device_interface_get(const E_Connman_Element *device, const char **interface) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_device_powered_get(const E_Connman_Element *device, bool *powered) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_device_powered_set(E_Connman_Element *device, bool powered, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_device_scan_interval_get(const E_Connman_Element *device, unsigned short *scan_interval) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_device_scan_interval_set(E_Connman_Element *device, unsigned short scan_interval, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_device_scanning_get(const E_Connman_Element *device, bool *scanning) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_device_networks_get(const E_Connman_Element *device, unsigned int *count, E_Connman_Element ***p_elements) EINA_ARG_NONNULL(1, 2, 3) EINA_PURE EINA_WARN_UNUSED_RESULT;


  /* Network Methods: Low-Level API
   *
   * The network API is informational only, to set these parameters,
   * use the services API instead.
   */
  EAPI E_Connman_Element *e_connman_network_get(const char *path) EINA_ARG_NONNULL(1) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_address_get(const E_Connman_Element *network, const char **address) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_name_get(const E_Connman_Element *network, const char **name) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_connected_get(const E_Connman_Element *network, bool *connected) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_strength_get(const E_Connman_Element *network, unsigned char *strength) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_frequency_get(const E_Connman_Element *network, unsigned short *frequency) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_device_get(const E_Connman_Element *network, E_Connman_Element **element) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_wifi_ssid_get(const E_Connman_Element *network, unsigned int *count, unsigned char **wifi_ssid) EINA_ARG_NONNULL(1, 2, 3) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_wifi_mode_get(const E_Connman_Element *network, const char **wifi_mode) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_wifi_security_get(const E_Connman_Element *network, const char **wifi_security) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_wifi_passphrase_get(const E_Connman_Element *network, const char **wifi_passphrase) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_wifi_channel_get(const E_Connman_Element *network, unsigned short *wifi_channel) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_wifi_eap_get(const E_Connman_Element *network, const char **wifi_eap) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;


#ifdef __cplusplus
}
#endif
#endif /* E_CONNMAN_H */
