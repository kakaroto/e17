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

typedef struct _E_Bluez_Element_Dict_Entry   E_Bluez_Element_Dict_Entry;

struct _E_Bluez_Element_Dict_Entry
{
   const char *name;
   int         type;
   union {
      Eina_Bool      boolean;
      const char    *str;
      short          i16;
      unsigned short u16;
      unsigned int   u32;
      unsigned char  byte;
      const char    *path;
   } value;
};

#ifndef EINA_LOG_DEFAULT_COLOR
#define EINA_LOG_DEFAULT_COLOR EINA_COLOR_CYAN
#endif

#undef DBG
#undef INF
#undef WRN
#undef ERR

#define DBG(...) EINA_LOG_DOM_DBG(_e_dbus_bluez_log_dom, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_e_dbus_bluez_log_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_e_dbus_bluez_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_e_dbus_bluez_log_dom, __VA_ARGS__)

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

extern E_DBus_Connection *e_bluez_conn;

const char *                    e_bluez_system_bus_name_get(void);

void                            e_bluez_manager_clear_elements(void);

void                            e_bluez_elements_init(void);
void                            e_bluez_elements_shutdown(void);

E_Bluez_Element *               e_bluez_element_register(const char *path, const char *interface);
void                            e_bluez_element_unregister(E_Bluez_Element *element);

Eina_Bool                       e_bluez_element_message_send(E_Bluez_Element *element, const char *method_name, E_DBus_Method_Return_Cb cb, DBusMessage *msg, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);
E_Bluez_Array *                 e_bluez_element_iter_get_array(DBusMessageIter *itr, const char *key);
void                            e_bluez_element_event_add(int event_type, E_Bluez_Element *element);
E_Bluez_Element_Dict_Entry *    e_bluez_element_array_dict_find_stringshared(const E_Bluez_Array *array, const char *key);
void                            e_bluez_element_array_free(E_Bluez_Array *array, E_Bluez_Array *new __UNUSED__);

Eina_Bool                       e_bluez_element_call_full(E_Bluez_Element *element, const char *method_name, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);
Eina_Bool                       e_bluez_element_call_with_path(E_Bluez_Element *element, const char *method_name, const char *string, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);
Eina_Bool                       e_bluez_element_call_with_string(E_Bluez_Element *element, const char *method_name, const char *string, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);
Eina_Bool                       e_bluez_element_call_with_path_and_string(E_Bluez_Element *element, const char *method_name, const char *path, const char *string, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);
