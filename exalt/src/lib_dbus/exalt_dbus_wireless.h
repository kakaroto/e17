/** @file exalt_dbus_wireless.h */

#ifndef  EXALT_DBUS_WIRELESS_INC
#define  EXALT_DBUS_WIRELESS_INC

#include <Exalt_dbus.h>

/**
 * @defgroup Wireless_Interface
 * @brief Methods specific to a wireless interface.
 * @ingroup Exalt_DBus
 * @{
 */

EAPI int exalt_dbus_wireless_list_get(Exalt_DBus_Conn* conn);

EAPI int exalt_dbus_wireless_essid_get(Exalt_DBus_Conn* conn, const char* eth);
EAPI int exalt_dbus_wireless_disconnect(Exalt_DBus_Conn* conn, const char* eth);
EAPI int exalt_dbus_wireless_wpasupplicant_driver_get(Exalt_DBus_Conn* conn, const char* eth);
EAPI int exalt_dbus_wireless_wpasupplicant_driver_set(Exalt_DBus_Conn* conn, const char* eth, const char* driver);

EAPI int exalt_dbus_wireless_scan(Exalt_DBus_Conn* conn, const char* eth);

#endif   /* ----- #ifndef EXALT_DBUS_WIRELESS_INC  ----- */

/** @} */

