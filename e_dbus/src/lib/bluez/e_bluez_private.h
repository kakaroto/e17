#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_ALLOCA_H
# include <alloca.h>
#elif defined __GNUC__
# define alloca __builtin_alloca
#elif defined _AIX
# define alloca __alloca
#elif defined _MSC_VER
# include <malloc.h>
# define alloca _alloca
#else
# include <stddef.h>
# ifdef  __cplusplus
extern "C"
# endif
void *alloca (size_t);
#endif

#include <stdio.h>

#include <Eina.h>
#include <eina_safety_checks.h>

#include "E_Bluez.h"

static const char manager_path[] = "/";

extern const char *e_bluez_iface_manager;
extern const char *e_bluez_iface_adapter;
extern const char *e_bluez_iface_device;
extern const char *e_bluez_prop_address;
extern const char *e_bluez_prop_name;
extern const char *e_bluez_prop_alias;
extern const char *e_bluez_prop_class;
extern const char *e_bluez_prop_icon;
extern const char *e_bluez_prop_paired;
extern const char *e_bluez_prop_trusted;
extern const char *e_bluez_prop_connected;
extern const char *e_bluez_prop_uuids;
extern const char *e_bluez_prop_powered;
extern const char *e_bluez_prop_discoverable;
extern const char *e_bluez_prop_pairable;
extern const char *e_bluez_prop_discoverabletimeout;
extern const char *e_bluez_prop_pairabletimeout;
extern const char *e_bluez_prop_discovering;
extern const char *e_bluez_prop_devices;

extern int _e_dbus_bluez_log_dom;

#ifndef EINA_LOG_DEFAULT_COLOR
#define EINA_LOG_DEFAULT_COLOR EINA_COLOR_CYAN
#endif

#undef DBG
#undef INF
#undef WRN
#undef ERR

#define DBG(...) EINA_LOG_DOM_DBG(_e_dbus_bluez_log_dom , __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_e_dbus_bluez_log_dom , __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_e_dbus_bluez_log_dom , __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_e_dbus_bluez_log_dom , __VA_ARGS__)

static inline bool
__dbus_callback_check_and_init(const char *file, int line, const char *function, DBusMessage *msg, DBusMessageIter *itr, DBusError *err)
{
   if (!msg)
     {
	if (err)
	  eina_log_print(_e_dbus_bluez_log_dom, EINA_LOG_LEVEL_ERR,
			 file, function, line,
			 "an error was reported by server: "
			 "name=\"%s\", message=\"%s\"",
			 err->name, err->message);
	else
	  eina_log_print(_e_dbus_bluez_log_dom, EINA_LOG_LEVEL_ERR,
			 file, function, line,
			 "callback without message arguments!");

	return 0;
     }

   if (!dbus_message_iter_init(msg, itr))
     {
	  eina_log_print(_e_dbus_bluez_log_dom, EINA_LOG_LEVEL_ERR,
			 file, function, line,
			 "could not init iterator.");
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

   ERR(file, function, line,
       "expected type %s (%c) but got %c instead!",
       expected_name, expected, type);

   return 0;
}
#define _dbus_iter_type_check(t, e) __dbus_iter_type_check(__FILE__, __LINE__, __FUNCTION__, t, e, #e)

extern E_DBus_Connection *e_bluez_conn;

const char *e_bluez_system_bus_name_get(void);

void e_bluez_manager_clear_elements(void);

void e_bluez_elements_init(void);
void e_bluez_elements_shutdown(void);

E_Bluez_Element *e_bluez_element_register(const char *path, const char *interface);
void e_bluez_element_unregister(E_Bluez_Element *element);

bool e_bluez_element_message_send(E_Bluez_Element *element, const char *method_name, E_DBus_Method_Return_Cb cb, DBusMessage *msg, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);

bool e_bluez_element_call_full(E_Bluez_Element *element, const char *method_name, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);
bool e_bluez_element_call_with_path(E_Bluez_Element *element, const char *method_name, const char *string, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);
bool e_bluez_element_call_with_string(E_Bluez_Element *element, const char *method_name, const char *string, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);
bool e_bluez_element_call_with_path_and_string(E_Bluez_Element *element, const char *method_name, const char *path, const char *string, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);
