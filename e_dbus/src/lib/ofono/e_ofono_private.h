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

#include "E_Ofono.h"

static const char manager_path[] = "/";

extern const char *e_ofono_iface_manager;
extern const char *e_ofono_prop_modems;
extern const char *e_ofono_iface_modem;
extern const char *e_ofono_prop_name;
extern const char *e_ofono_prop_powered;
extern const char *e_ofono_prop_interfaces;
extern const char *e_ofono_iface_netreg;
extern const char *e_ofono_prop_mode;
extern const char *e_ofono_prop_status;
extern const char *e_ofono_prop_operator;
extern const char *e_ofono_prop_strength;
extern const char *e_ofono_iface_sms;
extern const char *e_ofono_prop_sca;
extern const char *e_ofono_method_send_sms;

extern int _e_dbus_ofono_log_dom;

#ifndef EINA_LOG_DEFAULT_COLOR
#define EINA_LOG_DEFAULT_COLOR EINA_COLOR_CYAN
#endif

#undef DBG
#undef INF
#undef WRN
#undef ERR

#define DBG(...) EINA_LOG_DOM_DBG(_e_dbus_ofono_log_dom, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_e_dbus_ofono_log_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_e_dbus_ofono_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_e_dbus_ofono_log_dom, __VA_ARGS__)

typedef struct _E_Ofono_Array   E_Ofono_Array;

struct _E_Ofono_Array
{
   int         type;
   Eina_Array *array;
};

static inline Eina_Bool
_dbus_callback_check_and_init(DBusMessage *msg, DBusMessageIter *itr, DBusError *err)
{
   if (!msg)
     {
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

extern E_DBus_Connection *e_ofono_conn;

const char *         e_ofono_system_bus_name_get(void);

void                 e_ofono_manager_clear_elements(void);

void                 e_ofono_elements_init(void);
void                 e_ofono_elements_shutdown(void);

E_Ofono_Element *    e_ofono_element_register(const char *path, const char *interface);
void                 e_ofono_element_unregister(E_Ofono_Element *element);

Eina_Bool            e_ofono_element_message_send(E_Ofono_Element *element, const char *method_name, const char *interface, E_DBus_Method_Return_Cb cb, DBusMessage *msg, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);

Eina_Bool            e_ofono_element_call_full(E_Ofono_Element *element, const char *method_name, const char *interface, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);
Eina_Bool            e_ofono_element_call_with_path(E_Ofono_Element *element, const char *method_name, const char *string, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);
Eina_Bool            e_ofono_element_call_with_string(E_Ofono_Element *element, const char *method_name, const char *string, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);
Eina_Bool            e_ofono_element_call_with_path_and_string(E_Ofono_Element *element, const char *method_name, const char *path, const char *string, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);
