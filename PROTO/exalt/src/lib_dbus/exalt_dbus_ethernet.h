/** @file exalt_dbus_ethernet.h */

#ifndef  EXALT_DBUS_ETHERNET_INC
#define  EXALT_DBUS_ETHERNET_INC

#include "libexalt_dbus.h"
#include "define.h"

/**
 * @defgroup Ethernet_Interface
 * @brief These methods allows to get or set information about a wired or wireless interface. For example you can get the IP address of eth0, or activate an other itnerface.
 * @ingroup Exalt_DBus
 * @{
 */

EAPI int exalt_dbus_eth_list_get(Exalt_DBus_Conn* conn);
EAPI int exalt_dbus_eth_all_disconnected_is(Exalt_DBus_Conn* conn);

EAPI int exalt_dbus_eth_ip_get(Exalt_DBus_Conn* conn,const char* iface);
EAPI int exalt_dbus_eth_netmask_get(Exalt_DBus_Conn* conn, const char* iface);
EAPI int exalt_dbus_eth_gateway_get(Exalt_DBus_Conn* conn, const char* iface);
EAPI int exalt_dbus_eth_wireless_is(Exalt_DBus_Conn* conn, const char* iface);
EAPI int exalt_dbus_eth_link_is(Exalt_DBus_Conn* conn, const char* iface);
EAPI int exalt_dbus_eth_up_is(Exalt_DBus_Conn* conn, const char* iface);
EAPI int exalt_dbus_eth_dhcp_is(Exalt_DBus_Conn* conn, const char* iface);
EAPI int exalt_dbus_eth_connected_is(Exalt_DBus_Conn* conn, const char* iface);


EAPI int exalt_dbus_eth_up(Exalt_DBus_Conn* conn, const char* eth);
EAPI int exalt_dbus_eth_down(Exalt_DBus_Conn* conn, const char* eth);

EAPI int exalt_dbus_eth_conf_apply(Exalt_DBus_Conn* conn, const char* eth,Exalt_Configuration*c);


EAPI int exalt_dbus_eth_command_get(Exalt_DBus_Conn* conn, const char* eth);
EAPI int exalt_dbus_eth_command_set(Exalt_DBus_Conn* conn, const char* eth, const char* cmd);


#endif   /* ----- #ifndef EXALT_DBUS_ETHERNET_INC  ----- */

/** @} */

