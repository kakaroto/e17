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

    short is_wireless;

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

/**
 * @brief create a default connection
 * - no wireless
 * - dhcp mode
 * @return Returns the new connection
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

    c->is_wireless= 0;
    c->essid = NULL;
    c->key=NULL;
    c->encryption_mode = EXALT_ENCRYPTION_NONE;
    c->connection_mode = EXALT_CONNECTION_ADHOC;
    c->security_mode = EXALT_SECURITY_OPEN;

    c->cmd_after_apply = NULL;
    return c;
}

/**
 * @brief create a custom connection
 * @param mode the mode, dhcp or static
 * @param ip the ip address
 * @param netmask the netmask address
 * @param gateway the default gateway address
 * @param is_wireless 1 if the connection is a wireless, else 0
 * @param essid the essid
 * @param encryption_mode the encryption mode
 * @param key the key
 * @param connection_mode the connection mode
 * @param security_mode the security mode
 * @return Returns the new connection
 */
Exalt_Connection* exalt_conn_custom_new(Exalt_Enum_Mode mode,
        const char* ip, const char* netmask, const char* gateway,
        short is_wireless, const char* essid,
        Exalt_Enum_Encryption_Mode encryption_mode,
        const char* key,
        Exalt_Enum_Connection_Mode connection_mode,
        Exalt_Enum_Security_Mode security_mode)
{
    Exalt_Connection* c;

    c = exalt_conn_new();
    EXALT_ASSERT_RETURN(c!=NULL);

    exalt_conn_set_mode(c,mode);
    exalt_conn_set_ip(c,ip);
    exalt_conn_set_netmask(c,netmask);
    exalt_conn_set_gateway(c,gateway);


    exalt_conn_set_wireless(c,is_wireless);
    exalt_conn_set_essid(c,essid);
    exalt_conn_set_encryption_mode(c,encryption_mode);
    exalt_conn_set_key(c,key);
    exalt_conn_set_connection_mode(c, connection_mode);

    c->cmd_after_apply = NULL;
    exalt_conn_set_security_mode(c,security_mode);
    return c;
}

/**
 * @brief free a connection
 * @param c the connection
 */
void exalt_conn_free(Exalt_Connection* c)
{
    EXALT_ASSERT_RETURN_VOID(c!=NULL);

    EXALT_FREE(c->ip);
    EXALT_FREE(c->gateway);
    EXALT_FREE(c->netmask);
    EXALT_FREE(c->essid);
    EXALT_FREE(c->key);
    EXALT_FREE(c->cmd_after_apply);

    EXALT_FREE(c);
}

/**
 * @brief test if a connection is valid (valid address ...)
 * @param c the connection
 * @return Returns 1 if the connection is valid, else 0
 */
short exalt_conn_is_valid(Exalt_Connection* c)
{
    short valid = 1;
    EXALT_ASSERT_RETURN(c!=NULL);

    if(!exalt_conn_is_dhcp(c))
    {
        if(!exalt_is_address(exalt_conn_get_ip(c))
                || (exalt_conn_get_gateway(c)!=  NULL && !exalt_is_address(exalt_conn_get_gateway(c)))
                || !exalt_is_address(exalt_conn_get_netmask(c)))
            valid = 0;
    }

    if(valid && exalt_conn_is_wireless(c))
    {
        if(!exalt_is_essid(exalt_conn_get_essid(c))
                || !exalt_is_key(exalt_conn_get_key(c),exalt_conn_get_encryption_mode(c)))
            valid = 0;
    }

    return valid;
}





/**
 * @brief set the mode of a connection
 * @param c the connection
 * @param mode the new mode
 * @return Returns 1 if the mode is apply, else 0
 */
short exalt_conn_set_mode(Exalt_Connection* c, Exalt_Enum_Mode mode)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    c->mode = mode;
    return 1;
}



/**
 * @brief set a command which will be run after a configuration is applied
 * @param c the connection
 * @param cmd the command
 * @return Returns 1 if the new command is apply, else 0
 */
int exalt_conn_set_cmd(Exalt_Connection *c, const char* cmd)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    EXALT_ASSERT_RETURN(cmd!=NULL);

    EXALT_FREE(c->cmd_after_apply);
    c->cmd_after_apply=strdup(cmd);
    return 1;
}


/**
 * @brief get the command which will be run after a configuration is applied
 * @param c the connection
 * @return Returns the command or NULL
 */
const char* exalt_conn_get_cmd(Exalt_Connection* c)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    return c->cmd_after_apply;
}



/**
 * @brief set the ip address of a connection
 * @param c the connection
 * @param ip the new ip address
 * @return Returns 1 if the ip address is apply, else 0
 */
short exalt_conn_set_ip(Exalt_Connection* c, const char* ip)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    EXALT_ASSERT_RETURN(exalt_is_address(ip));

    EXALT_FREE(c->ip);
    c->ip=strdup(ip);
    return 1;
}


/**
 * @brief set the netmask address of a connection
 * @param c the connection
 * @param netmask the new netmask address
 * @return Returns 1 if the netmask address is apply, else 0
 */
short exalt_conn_set_netmask(Exalt_Connection* c, const char* netmask)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    EXALT_ASSERT_RETURN(exalt_is_address(netmask));

    EXALT_FREE(c->netmask);
    c->netmask=strdup(netmask);
    return 1;
}



/**
 * @brief set the gateway address of a connection
 * @param c the connection
 * @param gateway the new gateway address
 * @return Returns 1 if the gateway address is apply, else 0
 */
short exalt_conn_set_gateway(Exalt_Connection* c, const char* gateway)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    EXALT_ASSERT_RETURN(exalt_is_address(gateway));
    EXALT_FREE(c->gateway);
    c->gateway=strdup(gateway);
    return 1;
}


/**
 * @brief set the connection as a wireless connection
 * @param c the connection
 * @param is_wireless 1 if the connection will be a wireless connection, else 0
 * @return Returns 1 if the connection is modified, else 0
 */
short exalt_conn_set_wireless(Exalt_Connection* c, short is_wireless)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    c->is_wireless = is_wireless;
    return 1;
}

/**
 * @brief set the essid of a connection
 * @param c the connection
 * @param essid the new essid
 * @return Returns 1 if the essid is apply, else 0
 */
short exalt_conn_set_essid(Exalt_Connection* c, const char* essid)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    EXALT_ASSERT_RETURN(exalt_conn_is_wireless(c));
    EXALT_ASSERT_RETURN(exalt_is_essid(essid));

    EXALT_FREE(c->essid);
    c->essid=strdup(essid);
    return 1;
}



/**
 * @brief set the key of a connection
 * @param c the connection
 * @param key the new key
 * @return Returns 1 if the key is apply, else 0
 */
short exalt_conn_set_key(Exalt_Connection* c, const char* key)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    EXALT_ASSERT_RETURN(exalt_conn_is_wireless(c));
    EXALT_ASSERT_RETURN(exalt_is_key(key,exalt_conn_get_encryption_mode(c)));

    EXALT_FREE(c->key);
    c->key=strdup(key);
    return 1;
}




/**
 * @brief set the encryption_mode of a connection
 * @param c the connection
 * @param encryption_mode the new encryption_mode
 * @return Returns 1 if the encryption_mode is apply, else 0
 */
short exalt_conn_set_encryption_mode(Exalt_Connection* c, Exalt_Enum_Encryption_Mode encryption_mode)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    EXALT_ASSERT_RETURN(exalt_conn_is_wireless(c));

    c->encryption_mode = encryption_mode;
    return 1;
}


/**
 * @brief set the connection_mode of a connection
 * @param c the connection
 * @param connection_mode the new connection_mode
 * @return Returns 1 if the connection_mode is apply, else 0
 */
short exalt_conn_set_connection_mode(Exalt_Connection* c, Exalt_Enum_Connection_Mode connection_mode)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    EXALT_ASSERT_RETURN(exalt_conn_is_wireless(c));

    c->connection_mode = connection_mode;
    return 1;
}


/**
 * @brief set the security_mode of a connection
 * @param c the connection
 * @param security_mode the new security_mode
 * @return Returns 1 if the security_mode is apply, else 0
 */
short exalt_conn_set_security_mode(Exalt_Connection* c, Exalt_Enum_Security_Mode security_mode)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    EXALT_ASSERT_RETURN(exalt_conn_is_wireless(c));

    c->security_mode = security_mode;
    return 1;
}






/**
 * @brief return if a connection use the dhcp mode or static
 * @param c the connection
 * @return Returns 1 if the mode is dhcp, 0 if the mode is static, else -1
 */
short exalt_conn_is_dhcp(Exalt_Connection* c)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    return c->mode == EXALT_DHCP;
}

/**
 * @brief return the mode of a connection
 * @param c the connection
 * @return Returns the mode (dhcp or static)
 */
Exalt_Enum_Mode exalt_conn_get_mode(Exalt_Connection* c)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    return c->mode;
}

/**
 * @brief return the ip address of a connection
 * @param c the connection
 * @return the ip address, NULL if no ip
 */
const char* exalt_conn_get_ip(Exalt_Connection *c)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    return c->ip;
}


/**
 * @brief return the gateway address of a connection
 * @param c the connection
 * @return the gateway address, NULL if no gateway
 */
const char* exalt_conn_get_gateway(Exalt_Connection *c)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    return c->gateway;
}

/**
 * @brief return the netmask address of a connection
 * @param c the connection
 * @return the netmask address, NULL if no netmask
 */
const char* exalt_conn_get_netmask(Exalt_Connection *c)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    return c->netmask;
}




/**
 * @brief return if a connection is a wireless connection
 * @param c the connection
 * @return Returns 1 if the conenction is a wireless conenction, 0 if not, else -1
 */
short exalt_conn_is_wireless(Exalt_Connection* c)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    return c->is_wireless;
}

/**
 * @brief return the essid of a connection
 * @param c the connection
 * @return the essid, NULL if no essid
 */
const char* exalt_conn_get_essid(Exalt_Connection *c)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    EXALT_ASSERT_RETURN(exalt_conn_is_wireless(c));
    return c->essid;
}

/**
 * @brief return the key of a connection
 * @param c the connection
 * @return the key, NULL if no key
 */
const char* exalt_conn_get_key(Exalt_Connection *c)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    EXALT_ASSERT_RETURN(exalt_conn_is_wireless(c));
    return c->key;
}

/**
 * @brief return the encryption mode of a connection
 * @param c the connection
 * @return Returns the encryption mode
 */
Exalt_Enum_Encryption_Mode exalt_conn_get_encryption_mode(Exalt_Connection* c)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    EXALT_ASSERT_RETURN(exalt_conn_is_wireless(c));
    return c->encryption_mode;
}

/**
 * @brief return the security mode of a connection
 * @param c the connection
 * @return Returns the security mode
 */
Exalt_Enum_Security_Mode exalt_conn_get_security_mode(Exalt_Connection* c)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    EXALT_ASSERT_RETURN(exalt_conn_is_wireless(c));
    return c->security_mode;
}

/**
 * @brief return the connection mode of a connection
 * @param c the connection
 * @return Returns the connection mode
 */
Exalt_Enum_Connection_Mode exalt_conn_get_connection_mode(Exalt_Connection* c)
{
    EXALT_ASSERT_RETURN(c!=NULL);
    EXALT_ASSERT_RETURN(exalt_conn_is_wireless(c));
    return c->connection_mode;
}

/**
 * @brief create a eet descriptor of the structure Exalt_Connection
 * @return Returns the descriptor
 */
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

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection, "is_wireless", is_wireless, EET_T_SHORT);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection, "essid", essid, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection, "encryption_mode", encryption_mode, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection, "key", key, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection, "connection_mode", connection_mode, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection, "security_mode", security_mode, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection, "cmd_after_apply", cmd_after_apply, EET_T_STRING);


    return edd;
}

/** @} */
