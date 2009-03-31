#ifndef E_CONNMAN_H
#define E_CONNMAN_H

#include <E_DBus.h>
#include <Ecore_Data.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

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
	Eina_Inlist *network_create;
	Eina_Inlist *network_remove;
	Eina_Inlist *network_connect;
	Eina_Inlist *network_disconnect;
	Eina_Inlist *agent_register;
	Eina_Inlist *agent_unregister;
	Eina_Inlist *device_propose_scan;
     } _pending;
     struct {
	Ecore_Idler *changed;
     } _idler;
     Eina_Inlist *_listeners;
     int _references;
  };

  EAPI unsigned int e_connman_system_init(E_DBus_Connection *edbus_conn) EINA_ARG_NONNULL(1);
  EAPI unsigned int e_connman_system_shutdown(void);

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


  /* Manager Methods */

  EAPI E_Connman_Element *e_connman_manager_get(void) EINA_ARG_NONNULL(1) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_manager_register_agent(const char *object_path, E_DBus_Method_Return_Cb cb, const void *data);
  EAPI bool e_connman_manager_unregister_agent(const char *object_path, E_DBus_Method_Return_Cb cb, const void *data);

  EAPI bool e_connman_manager_state_get(const char **state);

  EAPI bool e_connman_manager_policy_get(const char **policy);
  EAPI bool e_connman_manager_policy_set(const char *policy, E_DBus_Method_Return_Cb cb, const void *data);

  EAPI bool e_connman_manager_offline_mode_get(bool *offline);
  EAPI bool e_connman_manager_offline_mode_set(bool offline, E_DBus_Method_Return_Cb cb, const void *data);

  EAPI bool e_connman_manager_profiles_get(unsigned int *count, E_Connman_Element ***p_elements);
  EAPI bool e_connman_manager_devices_get(unsigned int *count, E_Connman_Element ***p_elements);
  EAPI bool e_connman_manager_connections_get(unsigned int *count, E_Connman_Element ***p_elements);

  // TODO: profile_add (not implemented in connman right now)
  // TODO: profile_remove (not implemented in connman right now)
  // TODO: profile_active_get (not implemented in connman right now)
  // TODO: profile_active_set (not implemented in connman right now)
  // TODO: services_get (not implemented in connman right now)


  /* Device Methods */
  EAPI E_Connman_Element *e_connman_device_get(const char *path) EINA_ARG_NONNULL(1) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_device_network_create(E_Connman_Element *device, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_device_network_remove(E_Connman_Element *device, const char *network_path, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_device_propose_scan(E_Connman_Element *device, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_device_name_get(const E_Connman_Element *device, const char **name) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_device_type_get(const E_Connman_Element *device, const char **type) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_device_interface_get(const E_Connman_Element *device, const char **interface) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_device_policy_get(const E_Connman_Element *device, const char **policy) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_device_policy_set(E_Connman_Element *device, const char *policy, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1, 2) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_device_priority_get(const E_Connman_Element *device, unsigned char *priority) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_device_priority_set(E_Connman_Element *device, unsigned char priority, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_device_powered_get(const E_Connman_Element *device, bool *powered) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_device_powered_set(E_Connman_Element *device, bool powered, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_device_scan_interval_get(const E_Connman_Element *device, unsigned short *scan_interval) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_device_scan_interval_set(E_Connman_Element *device, unsigned short scan_interval, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_device_scanning_get(const E_Connman_Element *device, bool *scanning) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_device_networks_get(const E_Connman_Element *device, unsigned int *count, E_Connman_Element ***p_elements) EINA_ARG_NONNULL(1, 2, 3) EINA_PURE EINA_WARN_UNUSED_RESULT;

  // TODO: network_join (harder, need to think of good api that do not suck)
  // TODO: address_get


  /* Profile Methods */

  EAPI E_Connman_Element *e_connman_profile_get(const char *path) EINA_ARG_NONNULL(1) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_profile_name_get(const E_Connman_Element *profile, const char **name) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  // TODO: services_get (not implemented in connman right now)

  /* Connection Methods */

  EAPI E_Connman_Element *e_connman_connection_get(const char *path) EINA_ARG_NONNULL(1) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_connection_type_get(const E_Connman_Element *connection, const char **type) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_connection_interface_get(const E_Connman_Element *connection, const char **interface) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_connection_strength_get(const E_Connman_Element *connection, unsigned char *strength) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_connection_default_get(const E_Connman_Element *connection, bool *connection_default) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_connection_device_get(const E_Connman_Element *connection, E_Connman_Element **element) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_connection_network_get(const E_Connman_Element *connection, E_Connman_Element **element) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_connection_ipv4_method_get(const E_Connman_Element *connection, const char **method) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_connection_ipv4_address_get(const E_Connman_Element *connection, const char **address) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  // TODO: ipv4_gateway_get
  // TODO: ipv4_netmask_get


  /* Network Methods */
  EAPI E_Connman_Element *e_connman_network_get(const char *path) EINA_ARG_NONNULL(1) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_connect(E_Connman_Element *network, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_network_disconnect(E_Connman_Element *network, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_name_get(const E_Connman_Element *network, const char **name) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_available_get(const E_Connman_Element *network, bool *available) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_connected_get(const E_Connman_Element *network, bool *connected) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_remember_get(const E_Connman_Element *network, bool *remember) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_network_remember_set(E_Connman_Element *network, bool remember, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_strength_get(const E_Connman_Element *network, unsigned char *strength) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_device_get(const E_Connman_Element *network, E_Connman_Element **element) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_wifi_ssid_get(const E_Connman_Element *network, unsigned int *count, unsigned char **wifi_ssid) EINA_ARG_NONNULL(1, 2, 3) EINA_PURE EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_wifi_mode_get(const E_Connman_Element *network, const char **wifi_mode) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_network_wifi_mode_set(E_Connman_Element *network, const char *wifi_mode, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_wifi_security_get(const E_Connman_Element *network, const char **wifi_security) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_network_wifi_security_set(E_Connman_Element *network, const char *wifi_security, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  EAPI bool e_connman_network_wifi_passphrase_get(const E_Connman_Element *network, const char **wifi_passphare) EINA_ARG_NONNULL(1, 2) EINA_PURE EINA_WARN_UNUSED_RESULT;
  EAPI bool e_connman_network_wifi_passphrase_set(E_Connman_Element *network, const char *wifi_passphrase, E_DBus_Method_Return_Cb cb, const void *data) EINA_ARG_NONNULL(1) EINA_WARN_UNUSED_RESULT;

  // TODO: address_get

#ifdef __cplusplus
}
#endif
#endif /* E_CONNMAN_H */
