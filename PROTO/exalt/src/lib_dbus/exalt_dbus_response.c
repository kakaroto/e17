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

Exalt_DBus_Response_Type exalt_dbus_response_type_get(Exalt_DBus_Response* response)
{
    return response->type;
}

Ecore_List* exalt_dbus_response_list_get(Exalt_DBus_Response* response)
{
    switch(response->type)
    {
        case EXALT_DBUS_RESPONSE_DNS_GET_LIST:
            return response->l;
        default:
            print_error(__FILE__,__func__,__LINE__,"This type of response doesn't has a list\n");
            return NULL;
    }
}

