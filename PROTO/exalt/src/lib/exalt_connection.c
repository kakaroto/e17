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

    char* essid;
    Exalt_Enum_Encryption_Mode encryption_mode;
    char* key;
    Exalt_Enum_Connection_Mode connection_mode;
    Exalt_Enum_Security_Mode security_mode;

    char* cmd_after_apply; //a command call after exalt_conn_apply()
};

/**
 * @addtogroup Exalt_Connection
 * @{
 */

Exalt_Connection* exalt_conn_new()
{
    Exalt_Connection* c;

    c = malloc(sizeof(Exalt_Connection));
    EXALT_ASSERT_RETURN(c!=NULL);

    c->mode = EXALT_DHCP;

    c->ip = NULL;
    c->netmask = NULL;
    c->gateway = NULL;

    c->wireless= 0;
    c->essid = NULL;
    c->key=NULL;
    c->encryption_mode = EXALT_ENCRYPTION_NONE;
    c->connection_mode = EXALT_CONNECTION_ADHOC;
    c->security_mode = EXALT_SECURITY_OPEN;

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
    EXALT_FREE(c->essid);
    EXALT_FREE(c->key);
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

    if(valid && exalt_conn_wireless_is(c))
    {
        if(!exalt_is_essid(exalt_conn_essid_get(c))
                || !exalt_is_key(exalt_conn_key_get(c),exalt_conn_encryption_mode_get(c)))
            valid = 0;
    }

    return valid;
}





#define EXALT_FCT_NAME exalt_conn
#define EXALT_STRUCT_TYPE Exalt_Connection

EXALT_SET(mode,Exalt_Enum_Mode)
EXALT_STRING_SET(ip)
EXALT_STRING_SET(netmask)
EXALT_STRING_SET(gateway)
EXALT_STRING_SET(cmd_after_apply)

EXALT_SET(wireless,int)
EXALT_STRING_SET(essid)
EXALT_STRING_SET(key);
EXALT_SET(encryption_mode,Exalt_Enum_Encryption_Mode)
EXALT_SET(connection_mode,Exalt_Enum_Connection_Mode)
EXALT_SET(security_mode,Exalt_Enum_Security_Mode)

EXALT_GET(mode,Exalt_Enum_Mode)
EXALT_GET(ip,const char*)
EXALT_GET(gateway,const char*)
EXALT_GET(netmask,const char*)
EXALT_GET(cmd_after_apply,const char*)

EXALT_IS(wireless,int)
EXALT_GET(essid,const char*)
EXALT_GET(key,const char*)
EXALT_GET(encryption_mode,Exalt_Enum_Encryption_Mode)
EXALT_GET(security_mode,Exalt_Enum_Security_Mode)
EXALT_GET(connection_mode,Exalt_Enum_Connection_Mode)

#undef EXALT_FCT_NAME
#undef EXALT_STRUCT_TYPE

Eet_Data_Descriptor * exalt_conn_edd_new()
{
    Eet_Data_Descriptor *edd;

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

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection, "essid", essid, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection, "encryption_mode", encryption_mode, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection, "key", key, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection, "connection_mode", connection_mode, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection, "security_mode", security_mode, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection, "cmd_after_apply", cmd_after_apply, EET_T_STRING);


    return edd;
}

/** @} */
