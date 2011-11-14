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
void *    alloca (size_t);
#endif

#include <stdio.h>

#include <Eina.h>
#include <eina_safety_checks.h>

#include "E_Connman.h"

static const char manager_path[] = "/";

extern const char *e_connman_iface_manager;
extern const char *e_connman_iface_profile;
extern const char *e_connman_iface_service;
extern const char *e_connman_iface_connection;
extern const char *e_connman_iface_technology;

extern const char *e_connman_prop_available;
extern const char *e_connman_prop_connections;
extern const char *e_connman_prop_default;
extern const char *e_connman_prop_ipv4;
extern const char *e_connman_prop_ipv4_configuration;
extern const char *e_connman_prop_ethernet;
extern const char *e_connman_prop_interface;
extern const char *e_connman_prop_speed;
extern const char *e_connman_prop_duplex;
extern const char *e_connman_prop_method;
extern const char *e_connman_prop_address;
extern const char *e_connman_prop_gateway;
extern const char *e_connman_prop_netmask;
extern const char *e_connman_prop_mtu;
extern const char *e_connman_prop_name;
extern const char *e_connman_prop_offline_mode;
extern const char *e_connman_prop_policy;
extern const char *e_connman_prop_priority;
extern const char *e_connman_prop_profiles;
extern const char *e_connman_prop_profile_active;
extern const char *e_connman_prop_services;
extern const char *e_connman_prop_technologies;
extern const char *e_connman_prop_remember;
extern const char *e_connman_prop_state;
extern const char *e_connman_prop_strength;
extern const char *e_connman_prop_type;
extern const char *e_connman_prop_error;
extern const char *e_connman_prop_security;
extern const char *e_connman_prop_passphrase;
extern const char *e_connman_prop_passphrase_required;
extern const char *e_connman_prop_favorite;
extern const char *e_connman_prop_immutable;
extern const char *e_connman_prop_auto_connect;
extern const char *e_connman_prop_roaming;
extern const char *e_connman_prop_technology_default;
extern const char *e_connman_prop_technologies_available;
extern const char *e_connman_prop_technologies_enabled;
extern const char *e_connman_prop_technologies_connected;
extern const char *e_connman_prop_login_required;
extern const char *e_connman_prop_nameservers;
extern const char *e_connman_prop_domains;
extern const char *e_connman_prop_proxy;
extern const char *e_connman_prop_url;
extern const char *e_connman_prop_servers;
extern const char *e_connman_prop_excludes;

extern int _e_dbus_connman_log_dom;

#ifndef EINA_LOG_DEFAULT_COLOR
#define EINA_LOG_DEFAULT_COLOR EINA_COLOR_CYAN
#endif

#undef DBG
#undef INF
#undef WRN
#undef ERR

#define DBG(...) EINA_LOG_DOM_DBG(_e_dbus_connman_log_dom, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_e_dbus_connman_log_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_e_dbus_connman_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_e_dbus_connman_log_dom, __VA_ARGS__)

static inline Eina_Bool
_dbus_callback_check_and_init(DBusMessage *msg, DBusMessageIter *itr, DBusError *err)
{
   if (!msg)
     {
        if (err && (err->name[0] == 'C'))
          return EINA_FALSE;
        if (err)
           ERR("an error was reported by server: "
               "name=\"%s\", message=\"%s\"",
               err->name, err->message);
        else
           ERR("callback without message arguments!");

        return EINA_FALSE;
     }

   if (!dbus_message_iter_init(msg, itr))
     {
        ERR("could not init iterator.");
        return EINA_FALSE;
     }

   return EINA_TRUE;
}

static inline Eina_Bool
__dbus_iter_type_check(int type, int expected, const char *expected_name)
{
   if (type == expected)
      return EINA_TRUE;

   ERR("expected type %s (%c) but got %c instead!",
       expected_name, expected, type);

   return EINA_FALSE;
}

#define _dbus_iter_type_check(t, e) __dbus_iter_type_check(t, e, # e)

extern E_DBus_Connection *e_connman_conn;

const char *           e_connman_system_bus_name_get(void);

void                   e_connman_manager_clear_elements(void);

void                   e_connman_elements_init(void);
void                   e_connman_elements_shutdown(void);

E_Connman_Element *    e_connman_element_register(const char *path, const char *interface);
void                   e_connman_element_unregister(E_Connman_Element *element);

Eina_Bool              e_connman_element_objects_array_get_stringshared(const E_Connman_Element *element, const char *property, unsigned int *count, E_Connman_Element ***elements);
Eina_Bool              e_connman_element_strings_array_get_stringshared(const E_Connman_Element *element, const char *property, unsigned int *count, const char ***strings);
unsigned char *        e_connman_element_bytes_array_get_stringshared(const E_Connman_Element *element, const char *property, unsigned int *count);

Eina_Bool              e_connman_element_message_send(E_Connman_Element *element, const char *method_name, E_DBus_Method_Return_Cb cb, DBusMessage *msg, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);

Eina_Bool              e_connman_element_call_full(E_Connman_Element *element, const char *method_name, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);
Eina_Bool              e_connman_element_call_with_path(E_Connman_Element *element, const char *method_name, const char *string, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);
Eina_Bool              e_connman_element_call_with_string(E_Connman_Element *element, const char *method_name, const char *string, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);
