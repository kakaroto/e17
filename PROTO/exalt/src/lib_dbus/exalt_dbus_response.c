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

int exalt_dbus_response_valid_is(Exalt_DBus_Response *response)
{
    return response->is_valid;
}

int exalt_dbus_response_error_id_get(Exalt_DBus_Response* response)
{
    if(response->is_valid)
    {
        print_error(__FILE__,__func__,__LINE__,"This response is not an error\n");
        return -1;
    }
    return response->error_id;
}

char* exalt_dbus_response_error_msg_get(Exalt_DBus_Response* response)
{
    if(response->is_valid)
    {
        print_error(__FILE__,__func__,__LINE__,"This response is not an error\n");
        return NULL;
    }
    return response->error_msg;
}

Exalt_DBus_Response_Type exalt_dbus_response_type_get(Exalt_DBus_Response* response)
{
    return response->type;
}

Ecore_List* exalt_dbus_response_list_get(Exalt_DBus_Response* response)
{
    switch(response->type)
    {
        case EXALT_DBUS_RESPONSE_DNS_LIST_GET:
            return response->l;
        default:
            print_error(__FILE__,__func__,__LINE__,"This type of response doesn't has a list\n");
            return NULL;
    }
}

char* exalt_dbus_response_address_get(Exalt_DBus_Response* response)
{
    switch(response->type)
    {
        case EXALT_DBUS_RESPONSE_IFACE_IP_GET:
            return response->address;
        default:
            print_error(__FILE__,__func__,__LINE__,"This type of response doesn't has an address\n");
            return NULL;
    }
}


char* exalt_dbus_response_iface_get(Exalt_DBus_Response* response)
{
    switch(response->type)
    {
        case EXALT_DBUS_RESPONSE_IFACE_IP_GET:
            return response->iface;
        default:
            print_error(__FILE__,__func__,__LINE__,"This type of response doesn't has an interface\n");
            return NULL;
    }
}

