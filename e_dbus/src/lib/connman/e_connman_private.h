#include <Eina.h>
#include <eina_safety_checks.h>

#include "E_Connman.h"
#include <stdio.h>

static const char manager_path[] = "/";

extern const char *e_connman_iface_manager;
extern const char *e_connman_iface_network;
extern const char *e_connman_iface_profile;
extern const char *e_connman_iface_service;
extern const char *e_connman_iface_device;
extern const char *e_connman_iface_connection;

extern const char *e_connman_prop_available;
extern const char *e_connman_prop_connected;
extern const char *e_connman_prop_connections;
extern const char *e_connman_prop_default;
extern const char *e_connman_prop_device;
extern const char *e_connman_prop_devices;
extern const char *e_connman_prop_interface;
extern const char *e_connman_prop_ipv4_address;
extern const char *e_connman_prop_ipv4_method;
extern const char *e_connman_prop_name;
extern const char *e_connman_prop_network;
extern const char *e_connman_prop_networks;
extern const char *e_connman_prop_offline_mode;
extern const char *e_connman_prop_policy;
extern const char *e_connman_prop_powered;
extern const char *e_connman_prop_priority;
extern const char *e_connman_prop_profiles;
extern const char *e_connman_prop_remember;
extern const char *e_connman_prop_scan_interval;
extern const char *e_connman_prop_scanning;
extern const char *e_connman_prop_state;
extern const char *e_connman_prop_strengh;
extern const char *e_connman_prop_type;
extern const char *e_connman_prop_wifi_mode;
extern const char *e_connman_prop_wifi_passphrase;
extern const char *e_connman_prop_wifi_security;
extern const char *e_connman_prop_wifi_ssid;


#define DBG(...) EINA_ERROR_PDBG(__VA_ARGS__)
#define INF(...) EINA_ERROR_PINFO(__VA_ARGS__)
#define WRN(...) EINA_ERROR_PWARN(__VA_ARGS__)
#define ERR(...) EINA_ERROR_PERR(__VA_ARGS__)

static inline bool
__dbus_callback_check_and_init(const char *file, int line, const char *function, DBusMessage *msg, DBusMessageIter *itr, DBusError *err)
{
   if (!msg)
     {
	if (err)
	  eina_error_print(EINA_ERROR_LEVEL_ERR, file, function, line,
			   "an error was reported by server: "
			   "name=\"%s\", message=\"%s\"\n",
			   err->name, err->message);
	else
	  eina_error_print(EINA_ERROR_LEVEL_ERR, file, function, line,
			   "callback without message arguments!\n");

	return 0;
     }

   if (!dbus_message_iter_init(msg, itr))
     {
	eina_error_print(EINA_ERROR_LEVEL_ERR, file, function, line,
			 "could not init iterator.\n");
	return 0;
     }

   return 1;
}

#define _dbus_callback_check_and_init(msg, itr, err)			\
  __dbus_callback_check_and_init(__FILE__, __LINE__, __FUNCTION__,	\
				 msg, itr, err)

static inline bool
__dbus_iter_type_check(const char *file, int line, const char *function, int type, int expected, const char *expected_name)
{
   if (type == expected)
     return 1;

   eina_error_print(EINA_ERROR_LEVEL_ERR, file, function, line,
		    "expected type %s (%c) but got %c instead!\n",
		    expected_name, expected, type);

   return 0;
}
#define _dbus_iter_type_check(t, e) __dbus_iter_type_check(__FILE__, __LINE__, __FUNCTION__, t, e, #e)

extern E_DBus_Connection *e_connman_conn;

const char *e_connman_system_bus_name_get(void);

void e_connman_manager_clear_elements(void);

void e_connman_elements_init(void);
void e_connman_elements_shutdown(void);

E_Connman_Element *e_connman_element_register(const char *path, const char *interface);
void e_connman_element_unregister(E_Connman_Element *element);

bool e_connman_element_objects_array_get_stringshared(const E_Connman_Element *element, const char *property, unsigned int *count, E_Connman_Element ***elements);
unsigned char *e_connman_element_bytes_array_get_stringshared(const E_Connman_Element *element, const char *property, unsigned int *count);

bool e_connman_element_call_with_path(E_Connman_Element *element, const char *method_name, const char *string, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);

bool e_connman_element_message_send(E_Connman_Element *element, const char *method_name, E_DBus_Method_Return_Cb cb, DBusMessage *msg, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);

bool e_connman_element_call_full(E_Connman_Element *element, const char *method_name, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);
