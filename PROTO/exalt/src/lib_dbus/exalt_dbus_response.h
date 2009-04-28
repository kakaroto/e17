/*
 * =====================================================================================
 *
 *       Filename:  exalt_dbus_response.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  03/10/08 16:58:46 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  EXALT_DBUS_RESPONSE_INC
#define  EXALT_DBUS_RESPONSE_INC

/**
 * @defgroup Response
 * @brief This part of exalt_dbus defines the struct Exalt_DBus_Response. <br>
 * This structure contains a response from the exalt daemon. <br>
 * For example if you ask an IP address with exalt_dbus_eth_ip_get(),
 * you will get a response of type EXALT_DBUS_RESPONSE_IFACE_IP_GET and the response will contains the ip address
 * @ingroup Exalt_DBus
 * @{
 */

typedef enum _exalt_dbus_response_type Exalt_DBus_Response_Type;
typedef struct _exalt_dbus_response Exalt_DBus_Response;

#include "libexalt_dbus.h"

/**
 * The type of response
 */
enum _exalt_dbus_response_type
{
    /** Response of exalt_dbus_dns_add() */
    EXALT_DBUS_RESPONSE_DNS_ADD,
    /** Response of exalt_dbus_dns_delete() */
    EXALT_DBUS_RESPONSE_DNS_DEL,
    /** Response of exalt_dbus_dns_replace() */
    EXALT_DBUS_RESPONSE_DNS_REPLACE,
    /** Response of exalt_dbus_dns_list_get() */
    EXALT_DBUS_RESPONSE_DNS_LIST_GET,
    /** Response of exalt_dbus_eth_ip_get() */
    EXALT_DBUS_RESPONSE_IFACE_IP_GET,
    /** Response of exalt_dbus_eth_netmask_get() */
    EXALT_DBUS_RESPONSE_IFACE_NETMASK_GET,
    /** Response of exalt_dbus_eth_gateway_get() */
    EXALT_DBUS_RESPONSE_IFACE_GATEWAY_GET,
    /** Response of exalt_dbus_eth_list_get() */
    EXALT_DBUS_RESPONSE_IFACE_WIRED_LIST,
    /** Response of exalt_dbus_wireless_list_get() */
    EXALT_DBUS_RESPONSE_IFACE_WIRELESS_LIST,
    /** Response of exalt_dbus_eth_wireless_is() */
    EXALT_DBUS_RESPONSE_IFACE_WIRELESS_IS,
    /** Response of exalt_dbus_eth_link_is() */
    EXALT_DBUS_RESPONSE_IFACE_LINK_IS,
    /** Response of exalt_dbus_eth_up_is() */
    EXALT_DBUS_RESPONSE_IFACE_UP_IS,
    /** Response of exalt_dbus_eth_dhcp_is() */
    EXALT_DBUS_RESPONSE_IFACE_DHCP_IS,
    /** Response of exalt_dbus_eth_command_get() */
    EXALT_DBUS_RESPONSE_IFACE_CMD_GET,
    /** Response of exalt_dbus_eth_up() */
    EXALT_DBUS_RESPONSE_IFACE_UP,
    /** Response of exalt_dbus_eth_down() */
    EXALT_DBUS_RESPONSE_IFACE_DOWN,
    /** Response of exalt_dbus_eth_command_set() */
    EXALT_DBUS_RESPONSE_IFACE_CMD_SET,
    /** Response of exalt_dbus_eth_conn_apply() */
    EXALT_DBUS_RESPONSE_IFACE_APPLY,
    /** Response of exalt_dbus_wireless_essid_get() */
    EXALT_DBUS_RESPONSE_WIRELESS_ESSID_GET,
    /** Response of exalt_dbus_wireless_wpasupplicant_driver_get() */
    EXALT_DBUS_RESPONSE_WIRELESS_WPASUPPLICANT_DRIVER_GET,
    /** Response of exalt_dbus_wireless_wpasupplicant_driver_set() */
    EXALT_DBUS_RESPONSE_WIRELESS_WPASUPPLICANT_DRIVER_SET,
    /** Response of exalt_dbus_wireless_scan() */
    EXALT_DBUS_RESPONSE_WIRELESS_SCAN
};

int exalt_dbus_response_msg_id_get(Exalt_DBus_Response *response);
int exalt_dbus_response_error_is(Exalt_DBus_Response *response);
Exalt_DBus_Response_Type exalt_dbus_response_type_get(Exalt_DBus_Response* response);

int exalt_dbus_response_error_id_get(Exalt_DBus_Response* response);
char* exalt_dbus_response_error_msg_get(Exalt_DBus_Response* response);

Eina_List* exalt_dbus_response_list_get(Exalt_DBus_Response* response);
char* exalt_dbus_response_address_get(Exalt_DBus_Response* response);
char* exalt_dbus_response_string_get(Exalt_DBus_Response* response);
char* exalt_dbus_response_iface_get(Exalt_DBus_Response* response);
int exalt_dbus_response_is_get(Exalt_DBus_Response* response);

/** @} */

#endif   /* ----- #ifndef EXALT_DBUS_RESPONSE_INC  ----- */

