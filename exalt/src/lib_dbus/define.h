/** @file define.h */

#ifndef  DEFINE_INC
#define  DEFINE_INC

/**
 * @defgroup Defines
 * @brief Some defines used by the DBus library
 * @ingroup Exalt_DBus
 * @{
 */

#ifndef PATH_MAX
    /** Define a maximum path if undefined */
    #define PATH_MAX 1024
#endif


/** The exalt-daemon DBus service */
#define EXALTD_SERVICE "org.e.Exalt"
/** The exalt-daemon DBus path */
#define EXALTD_PATH "/org/e/Exalt"


/** Print a DBusError message if the flag error is set */
#define EXALT_DBUS_ERROR_PRINT(error) if(error && dbus_error_is_set(error)) { EXALT_LOG_ERR("%s",error->name);return; }

/** Path of the DBus object which allows to manage the DNS */
#define EXALTD_PATH_DNS "/org/exalt/dns"
/** Path of the DBus interface which allows to manage the DNS */
#define EXALTD_INTERFACE_DNS "org.exalt.dns"


/** Path of the DBus object which allows to manage the list of interfaces */
#define EXALTD_PATH_IFACE "/org/exalt/interface"
/** Path of the DBus interface which allows to manage the list of interfaces */
#define EXALTD_INTERFACE_IFACE "org.exalt.interface"

/** Path of the DBus object which allows to manage a wired interface */
#define EXALTD_PATH_IFACES_WIRED "/org/exalt/interfaces/wired"
/** Path of the DBus interface which allows to manage a wired interface */
#define EXALTD_INTERFACE_IFACES_WIRED "org.exalt.interfaces.wired"

/** Path of the DBus object which allows to manage a wireless interface */
#define EXALTD_PATH_IFACES_WIRELESS "/org/exalt/interfaces/wireless"
/** Path of the DBus interface which allows to manage a wireless interface */
#define EXALTD_INTERFACE_IFACES_WIRELESS "org.exalt.interfaces.wireless"

/** Path of the DBus object which allows to manage the wireless networks saved */
#define EXALTD_PATH_NETWORK "/org/exalt/network"
/** Path of the DBus interface which allows to manage the wireless networks saved */
#define EXALTD_INTERFACE_NETWORK "org.exalt.network"

/** Path of the DBus object which allows to retrieve all the notifications from the daemon */
#define EXALTD_PATH_NOTIFY "/org/exalt/notify"
/** Path of the DBus interface which allows to retrieve all the notifications from the daemon */
#define EXALTD_INTERFACE_NOTIFY "org.exalt.notify"

/** Create a new method to access to the DNS DBus interface */
#define exalt_dbus_dns_call_new(member) dbus_message_new_method_call(EXALTD_SERVICE, \
        EXALTD_PATH_DNS, EXALTD_INTERFACE_DNS, member)

/** Create a new method to access to the interfaces DBus interface */
#define exalt_dbus_iface_call_new(member,path,interface) dbus_message_new_method_call(EXALTD_SERVICE, \
        path, interface, member)

/** Create a new method to access to the wired interface DBus interface */
#define exalt_dbus_ifaces_wired_call_new(member) dbus_message_new_method_call(EXALTD_SERVICE, \
        EXALTD_PATH_IFACES_WIRED, EXALTD_INTERFACE_IFACES_WIRED, member)

/** Create a new method to access to the wireless interface DBus interface */
#define exalt_dbus_ifaces_wireless_call_new(member) dbus_message_new_method_call(EXALTD_SERVICE, \
        EXALTD_PATH_IFACES_WIRELESS, EXALTD_INTERFACE_IFACES_WIRELESS, member)

/** Create a new method to access to the networks DBus interface */
#define exalt_dbus_ifaces_network_call_new(member) dbus_message_new_method_call(EXALTD_SERVICE, \
        EXALTD_PATH_NETWORK, EXALTD_INTERFACE_NETWORK, member)

#endif   /* ----- #ifndef DEFINE_INC  ----- */

/** @} */


