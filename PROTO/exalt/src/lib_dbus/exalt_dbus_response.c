/*
 * =====================================================================================
 *
 *       Filename:  exalt_dbus_response.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  03/10/08 16:58:55 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "exalt_dbus_response.h"
#include "libexalt_dbus_private.h"

/**
 * @addtogroup Response
 * @{
 */


/**
 * @brief Returns the id of the message.
 */
int exalt_dbus_response_msg_id_get(Exalt_DBus_Response *response)
{
    return response->msg_id;
}

/**
 * @brief Returns true if a errors occurs
 */
int exalt_dbus_response_error_is(Exalt_DBus_Response *response)
{
    return response->is_error;
}

/**
 * @brief Returns the id of the error
 */
int exalt_dbus_response_error_id_get(Exalt_DBus_Response* response)
{
    if(response->is_error)
    {
        print_error(__FILE__,__func__,__LINE__,"This response is not an error\n");
        return -1;
    }
    return response->error_id;
}

/**
 * @brief Returns the error as a string
 */
char* exalt_dbus_response_error_msg_get(Exalt_DBus_Response* response)
{
    if(response->is_error)
    {
        print_error(__FILE__,__func__,__LINE__,"This response is not an error\n");
        return NULL;
    }
    return response->error_msg;
}

/**
 * @brief Returns the type of the response (IP_GET ....)
 */
Exalt_DBus_Response_Type exalt_dbus_response_type_get(Exalt_DBus_Response* response)
{
    return response->type;
}

/**
 * @brief Returns an Eina_List if the response is a list (for example if the question is exalt_dbus_eth_list_get())
 */
Eina_List* exalt_dbus_response_list_get(Exalt_DBus_Response* response)
{
    switch(response->type)
    {
        case EXALT_DBUS_RESPONSE_DNS_LIST_GET:
        case EXALT_DBUS_RESPONSE_IFACE_WIRED_LIST:
        case EXALT_DBUS_RESPONSE_IFACE_WIRELESS_LIST:
            return response->l;
        default:
            print_error(__FILE__,__func__,__LINE__,"This type of response doesn't has a list\n");
            return NULL;
    }
}

/**
 * @brief Returns an address if the response is an address (for example if the question is exalt_dbus_eth_ip_get())
 */
char* exalt_dbus_response_address_get(Exalt_DBus_Response* response)
{
    switch(response->type)
    {
        case EXALT_DBUS_RESPONSE_IFACE_IP_GET:
        case EXALT_DBUS_RESPONSE_IFACE_NETMASK_GET:
        case EXALT_DBUS_RESPONSE_IFACE_GATEWAY_GET:
            return response->address;
        default:
            print_error(__FILE__,__func__,__LINE__,"This type of response doesn't has an address\n");
            return NULL;
    }
}

/**
 * @brief Returns a simple string if the response is a string ( for example if the question is exalt_dbus_eth_command_get())
 */
char* exalt_dbus_response_string_get(Exalt_DBus_Response* response)
{
    switch(response->type)
    {
        case EXALT_DBUS_RESPONSE_IFACE_CMD_GET:
        case EXALT_DBUS_RESPONSE_WIRELESS_ESSID_GET:
        case EXALT_DBUS_RESPONSE_WIRELESS_WPASUPPLICANT_DRIVER_GET:
            return response->string;
        default:
            print_error(__FILE__,__func__,__LINE__,"This type of response doesn't has a simple string\n");
            return NULL;
    }
}

/**
 * @brief Returns the interface about the question was (force example eth0 if you ask the ip of the interface eth0)
 */
char* exalt_dbus_response_iface_get(Exalt_DBus_Response* response)
{
    switch(response->type)
    {
        case EXALT_DBUS_RESPONSE_IFACE_IP_GET:
        case EXALT_DBUS_RESPONSE_IFACE_NETMASK_GET:
        case EXALT_DBUS_RESPONSE_IFACE_GATEWAY_GET:
        case EXALT_DBUS_RESPONSE_IFACE_WIRELESS_IS:
        case EXALT_DBUS_RESPONSE_IFACE_LINK_IS:
        case EXALT_DBUS_RESPONSE_IFACE_DHCP_IS:
        case EXALT_DBUS_RESPONSE_IFACE_UP_IS:
        case EXALT_DBUS_RESPONSE_IFACE_UP:
        case EXALT_DBUS_RESPONSE_IFACE_DOWN:
        case EXALT_DBUS_RESPONSE_IFACE_CMD_GET:
        case EXALT_DBUS_RESPONSE_IFACE_CMD_SET:
        case EXALT_DBUS_RESPONSE_WIRELESS_ESSID_GET:
        case EXALT_DBUS_RESPONSE_WIRELESS_WPASUPPLICANT_DRIVER_GET:
        case EXALT_DBUS_RESPONSE_WIRELESS_WPASUPPLICANT_DRIVER_SET:
        case EXALT_DBUS_RESPONSE_WIRELESS_SCAN:
        case EXALT_DBUS_RESPONSE_IFACE_CONNECTED_IS:
            return response->iface;
        default:
            print_error(__FILE__,__func__,__LINE__,"This type of response doesn't has an interface\n");
            return NULL;
    }
}

/**
 * @brief Returns a boolean if the response is a boolean (for example if the question is exalt_dbus_eth_up_is())
 */
int exalt_dbus_response_is_get(Exalt_DBus_Response* response)
{
    switch(response->type)
    {
        case EXALT_DBUS_RESPONSE_IFACE_WIRELESS_IS:
        case EXALT_DBUS_RESPONSE_IFACE_LINK_IS:
        case EXALT_DBUS_RESPONSE_IFACE_DHCP_IS:
        case EXALT_DBUS_RESPONSE_IFACE_UP_IS:
        case EXALT_DBUS_RESPONSE_IFACE_CONNECTED_IS:
            return response->is;
        default:
            print_error(__FILE__,__func__,__LINE__,"This type of response doesn't has an boolean\n");
            return 0;
    }
}

/**
 * @brief Returns a boolean the network configuration
 */
Exalt_Configuration *exalt_dbus_response_configuration_get(Exalt_DBus_Response* response)
{
    switch(response->type)
    {
        case EXALT_DBUS_RESPONSE_NETWORK_CONFIGURATION_GET:
            return response->c;
        default:
            print_error(__FILE__,__func__,__LINE__,"This type of response doesn't has an network configuration\n");
            return 0;
    }
}


/** @} */

