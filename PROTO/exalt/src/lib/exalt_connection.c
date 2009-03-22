/*
 * =====================================================================================
 *
 *       Filename:  exalt_connection.c
 *
 *    Description:  functions to manage a connexion
 *
 *        Version:  1.0
 *        Created:  10/03/2007 03:36:25 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

/** @file exalt_connection.c */

#include "exalt_connection.h"
#include "libexalt_private.h"

struct Exalt_Connection
{
    Exalt_Enum_Mode mode;

    char* ip;
    char* netmask;
    char* gateway;

    int wireless;

    Exalt_Wireless_Network* network;
    char* key;
    char* login;

    int wep_key_hexa;

    char* cmd_after_apply; //a command call after exalt_conn_apply()
};

/**
 * @addtogroup Exalt_Connection
 * @{
 */

Exalt_Connection* exalt_conn_new()
{
    Exalt_Connection* c;

    c = calloc(1,sizeof(Exalt_Connection));
    EXALT_ASSERT_RETURN(c!=NULL);

    c->mode = EXALT_DHCP;

    c->cmd_after_apply = NULL;
    return c;
}

void exalt_conn_free(Exalt_Connection** conn)
{
    Exalt_Connection *c;
    EXALT_ASSERT_RETURN_VOID(conn!=NULL);

    c = *conn;
    EXALT_ASSERT_RETURN_VOID(c!=NULL);

    EXALT_FREE(c->ip);
    EXALT_FREE(c->gateway);
    EXALT_FREE(c->netmask);
    EXALT_FREE(c->key);
    EXALT_FREE(c->login);

    EXALT_FREE(c->cmd_after_apply);

    EXALT_FREE(c);
}

short exalt_conn_valid_is(Exalt_Connection* c)
{
    short valid = 1;
    EXALT_ASSERT_RETURN(c!=NULL);

    if(exalt_conn_mode_get(c) == EXALT_STATIC)
    {
        if(!exalt_is_address(exalt_conn_ip_get(c))
                || (exalt_conn_gateway_get(c)!=  NULL && !exalt_is_address(exalt_conn_gateway_get(c)))
                || !exalt_is_address(exalt_conn_netmask_get(c)))
            valid = 0;
    }

    /*if(valid && exalt_conn_wireless_is(c))
    {
        if(!exalt_is_essid(exalt_conn_essid_get(c))
                || !exalt_is_key(exalt_conn_key_get(c),exalt_conn_encryption_mode_get(c)))
            valid = 0;
    }
    */

    return valid;
}





#define EXALT_FCT_NAME exalt_conn
#define EXALT_STRUCT_TYPE Exalt_Connection

EXALT_SET(mode,Exalt_Enum_Mode)
EXALT_SET(wep_key_hexa,int);
EXALT_STRING_SET(ip)
EXALT_STRING_SET(netmask)
EXALT_STRING_SET(gateway)
EXALT_STRING_SET(cmd_after_apply)

EXALT_SET(wireless,int)
EXALT_STRING_SET(key)
EXALT_STRING_SET(login)
EXALT_SET(network,Exalt_Wireless_Network*)

EXALT_GET(mode,Exalt_Enum_Mode)
EXALT_IS(wep_key_hexa,int)
EXALT_GET(ip,const char*)
EXALT_GET(gateway,const char*)
EXALT_GET(netmask,const char*)
EXALT_GET(cmd_after_apply,const char*)

EXALT_IS(wireless,int)
EXALT_GET(key,const char*)
EXALT_GET(login,const char*)
EXALT_GET(network,Exalt_Wireless_Network*)

#undef EXALT_FCT_NAME
#undef EXALT_STRUCT_TYPE

Eet_Data_Descriptor * exalt_conn_edd_new(Eet_Data_Descriptor* edd_network)
{
    Eet_Data_Descriptor *edd;

    EXALT_ASSERT_RETURN(edd_network!=NULL);

    edd = eet_data_descriptor_new("Connection", sizeof(Exalt_Connection),
            (void*(*)(void*))eina_list_next,
            (void*(*)(void*,void*))eina_list_append,
            (void*(*)(void*))eina_list_data_get,
            (void*(*)(void*))eina_list_free,
            (void(*)(void*,int(*)(void*,const char*,void*,void*),void*))evas_hash_foreach,
            (void*(*)(void*,const char*,void*))evas_hash_add,
            (void(*)(void*))evas_hash_free);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection, "dhcp", mode, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection, "ip", ip, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection, "netmask", netmask, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection, "gateway", gateway, EET_T_STRING);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection, "wireless", wireless, EET_T_SHORT);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection, "key", key, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection, "login", key, EET_T_STRING);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection, "cmd_after_apply", cmd_after_apply, EET_T_STRING);

    EET_DATA_DESCRIPTOR_ADD_SUB(edd, Exalt_Connection, "network", network, edd_network);
    return edd;
}

/** @} */
