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

    #include "E_Ofono.h"

    static const char manager_path[] = "/";

    extern const char *e_ofono_iface_manager;
    extern const char *e_ofono_prop_modems;
    extern const char *e_ofono_iface_modem;
    extern const char *e_ofono_prop_name;
    extern const char *e_ofono_prop_powered;
    extern const char *e_ofono_prop_interfaces;
    extern const char *e_ofono_iface_network_reg;
    extern const char *e_ofono_prop_mode;
    extern const char *e_ofono_prop_status;
    extern const char *e_ofono_prop_operator;
    extern const char *e_ofono_prop_strength;

    extern int _e_dbus_ofono_log_dom;

    #ifndef EINA_LOG_DEFAULT_COLOR
    #define EINA_LOG_DEFAULT_COLOR EINA_COLOR_CYAN
    #endif

    #undef DBG
    #undef INF
    #undef WRN
    #undef ERR

    #define DBG(...) EINA_LOG_DOM_DBG(_e_dbus_ofono_log_dom , __VA_ARGS__)
    #define INF(...) EINA_LOG_DOM_INFO(_e_dbus_ofono_log_dom , __VA_ARGS__)
    #define WRN(...) EINA_LOG_DOM_WARN(_e_dbus_ofono_log_dom , __VA_ARGS__)
    #define ERR(...) EINA_LOG_DOM_ERR(_e_dbus_ofono_log_dom , __VA_ARGS__)

    typedef struct _E_Ofono_Array E_Ofono_Array;

    struct _E_Ofono_Array
    {
       int type;
       Eina_Array *array;
    };

    static inline bool
    __dbus_callback_check_and_init(const char *file, int line, const char *function, DBusMessage *msg, DBusMessageIter *itr, DBusError *err)
    {
       if (!msg)
         {
        if (err)
          eina_log_print(_e_dbus_ofono_log_dom, EINA_LOG_LEVEL_ERR,
                 file, function, line,
                 "an error was reported by server: "
                 "name=\"%s\", message=\"%s\"",
                 err->name, err->message);
        else
          eina_log_print(_e_dbus_ofono_log_dom, EINA_LOG_LEVEL_ERR,
                 file, function, line,
                 "callback without message arguments!");

        return 0;
         }

       if (!dbus_message_iter_init(msg, itr))
         {
          eina_log_print(_e_dbus_ofono_log_dom, EINA_LOG_LEVEL_ERR,
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

    extern E_DBus_Connection *e_ofono_conn;

    const char *e_ofono_system_bus_name_get(void);

    void e_ofono_manager_clear_elements(void);

    void e_ofono_elements_init(void);
    void e_ofono_elements_shutdown(void);

    E_Ofono_Element *e_ofono_element_register(const char *path, const char *interface);
    void e_ofono_element_unregister(E_Ofono_Element *element);

    bool e_ofono_element_message_send(E_Ofono_Element *element, const char *method_name, const char *interface, E_DBus_Method_Return_Cb cb, DBusMessage *msg, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);

    bool e_ofono_element_call_full(E_Ofono_Element *element, const char *method_name, const char *interface, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);
    bool e_ofono_element_call_with_path(E_Ofono_Element *element, const char *method_name, const char *string, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);
    bool e_ofono_element_call_with_string(E_Ofono_Element *element, const char *method_name, const char *string, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);
bool e_ofono_element_call_with_path_and_string(E_Ofono_Element *element, const char *method_name, const char *path, const char *string, E_DBus_Method_Return_Cb cb, Eina_Inlist **pending, E_DBus_Method_Return_Cb user_cb, const void *user_data);
