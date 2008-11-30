/*
 * =====================================================================================
 *
 *       Filename:  define.h
 *
 *    Description:  define some variables
 *
 *        Version:  1.0
 *        Created:  08/29/2007 01:40:41 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  DEFINE_INC
#define  DEFINE_INC

#undef __UNUSED__
#define __UNUSED__ __attribute__((unused))

#define EXALTD_SERVICE "org.e.Exalt"

#define EXALTD_PATH_DNS "/org/exalt/dns"
#define EXALTD_INTERFACE_DNS "org.exalt.dns"

#define EXALTD_PATH_IFACE "/org/exalt/interface"
#define EXALTD_INTERFACE_IFACE "org.exalt.interface"

#define EXALTD_PATH_WIREDS "/org/exalt/interfaces/wired"
#define EXALTD_INTERFACE_WIREDS "org.exalt.interfaces.wired"

#define EXALTD_PATH_WIRELESSS "/org/exalt/interfaces/wireless"
#define EXALTD_INTERFACE_WIRELESSS "org.exalt.interfaces.wireless"

#define EXALTD_PATH_NOTIFY "/org/exalt/notify"
#define EXALTD_INTERFACE_NOTIFY "org.exalt.notify"




#define EXALTD_PATH "test"
#define EXALTD_INTERFACE_READ "test"
#define EXALTD_INTERFACE_WRITE "test"

#define EXALTD_PIDFILE "/var/run/exaltd.pid"
#define EXALTD_LOGFILE "/var/log/exaltd.log"

#define CONF_FILE "/etc/exaltd.eet"




/*
 * when we return a valid response, next args are the response
 */
#define EXALT_DBUS_VALID 1;
/* when we return an error, the next args have to be an int32 with a error id
 * and the third args have to be a string with a description of the error
 */
#define EXALT_DBUS_ERROR 0;

#define EXALT_DBUS_NO_ARGUMENT "Received no argument"
#define EXALT_DBUS_NO_ARGUMENT_ID 0
#define EXALT_DBUS_ARGUMENT_NOT_STRING "One of arguments is not a string but should be a string"
#define EXALT_DBUS_ARGUMENT_NOT_STRING_ID 1
#define EXALT_DBUS_ARGUMENT_NOT_INT32 "One of arguments is not a int32 but should be an int32"
#define EXALT_DBUS_ARGUMENT_NOT_INT32_ID 2

#define EXALT_DBUS_DNS_ERROR "Exalt can not reads the list of DNS"
#define EXALT_DBUS_DNS_ERROR_ID 20
#define EXALT_DBUS_INTERFACE_LIST_ERROR "Exalt can not gets the list of interfaces"
#define EXALT_DBUS_INTERFACE_LIST_ERROR_ID 21
#define EXALT_DBUS_INTERFACE_ERROR "The interface doesn't exists"
#define EXALT_DBUS_INTERFACE_ERROR_ID 22
#define EXALT_DBUS_INTERFACE_NOT_WIRELESS_ERROR "The interface is not a wireless interface"
#define EXALT_DBUS_INTERFACE_NOT_WIRELESS_ERROR_ID 23

#define EXALT_DBUS_IP_ERROR "The interface has no ip address"
#define EXALT_DBUS_IP_ERROR_ID 24
#define EXALT_DBUS_GATEWAY_ERROR "The interface has no gateway address"
#define EXALT_DBUS_GATEWAY_ERROR_ID 25
#define EXALT_DBUS_NETMASK_ERROR "The interface has no netmask address"
#define EXALT_DBUS_NETMASK_ERROR_ID 26
#define EXALT_DBUS_CONN_NEW_ERROR "Exalt can't create a new connection, memory problem ?"
#define EXALT_DBUS_CONN_NEW_ERROR_ID 27
#define EXALT_DBUS_ESSID_ERROR "The interface has no essid"
#define EXALT_DBUS_ESSID_ERROR_ID 28
#define EXALT_DBUS_DRIVER_ERROR "The interface has no driver"
#define EXALT_DBUS_DRIVER_ERROR_ID 29

#define EXALT_DBUS_CMD_ERROR "The interface has no command"
#define EXALT_DBUS_CMD_ERROR_ID 100



#define EXALT_DBUS_WIRELESS_NETWORK_ERROR "The wireless network doesn't exists"
#define EXALT_DBUS_WIRELESS_NETWORK_ERROR_ID 30
#define EXALT_DBUS_WIRELESS_NETWORK_ADDR_ERROR "The wireless network has no mac address"
#define EXALT_DBUS_WIRELESS_NETWORK_ADDR_ERROR_ID 30
#define EXALT_DBUS_WIRELESS_NETWORK_PROTOCOL_ERROR "The wireless network has no protocol"
#define EXALT_DBUS_WIRELESS_NETWORK_PROTOCOL_ERROR_ID 30
#define EXALT_DBUS_WIRELESS_NETWORK_MODE_ERROR "The wireless network has no mode"
#define EXALT_DBUS_WIRELESS_NETWORK_MODE_ERROR_ID 30
#define EXALT_DBUS_WIRELESS_NETWORK_CHANNEL_ERROR "The wireless network has no channel"
#define EXALT_DBUS_WIRELESS_NETWORK_CHANNEL_ERROR_ID 30
#define EXALT_DBUS_WIRELESS_NETWORK_BITRATES_ERROR "The wireless network has no bit rates"
#define EXALT_DBUS_WIRELESS_NETWORK_BITRATES_ERROR_ID 30
#define EXALT_DBUS_WIRELESS_NETWORK_ENCRYPTION_ERROR "The wireless network has no encryption value"
#define EXALT_DBUS_WIRELESS_NETWORK_ENCRYPTION_ERROR_ID 30

#define EXALT_DBUS_CONNECTION_NOT_VALID "The wireless network has no encryption value"
#define EXALT_DBUS_CONNECTION_NOT_VALID_ID 70



#endif   /* ----- #ifndef DEFINE_INC  ----- */

