/*
 * =====================================================================================
 *
 *       Filename:  exalt_connection.h
 *
 *    Description:  functions to manage a connexion
 *
 *        Version:  1.0
 *        Created:  10/03/2007 03:37:17 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

/** @file exalt_connection.h */

#ifndef  EXALT_CONNECTION_INC
#define  EXALT_CONNECTION_INC

/**
 * @defgroup Exalt_Connection
 * @brief The Exalt_Connection struct contains all informations about a configuration (dhcp or static mode, ip, essid ...)
 * @{
 */

/**
 * @brief Information about a connection
 * @structinfo
 */
typedef struct Exalt_Connection Exalt_Connection;

/** Define the list of state */
typedef enum Exalt_Enum_State
{
    /** down/deactivate */
    EXALT_DOWN,
    /** up/activate */
    EXALT_UP,
} Exalt_Enum_State;

/** Define the list of encryption mode */
typedef enum Exalt_Enum_Encryption_Mode
{
    /** no encryption */
    EXALT_ENCRYPTION_NONE,
    /** WEP ASCII encryption */
    EXALT_ENCRYPTION_WEP_ASCII,
    /** WEP Hexadecimal encryption */
    EXALT_ENCRYPTION_WEP_HEXA,
    /** WPA PSK CCMP (ASCII mode) */
    EXALT_ENCRYPTION_WPA_PSK_CCMP_ASCII,
    /** WPA PSK TKIP (ASCII mode) */
    EXALT_ENCRYPTION_WPA_PSK_TKIP_ASCII,
    /** WPA2 PSK CCMP (ASCII mode) */
    EXALT_ENCRYPTION_WPA2_PSK_CCMP_ASCII,
    /** WPA2 PSK TKIP (ASCII mode) */
    EXALT_ENCRYPTION_WPA2_PSK_TKIP_ASCII
} Exalt_Enum_Encryption_Mode;

/** Define the list f connection mode*/
typedef enum Exalt_Enum_Connection_Mode
{
    /** Ad-Hoc connection mode*/
    EXALT_CONNECTION_ADHOC,
    /** Managed connection mode*/
    EXALT_CONNECTION_MANAGED
} Exalt_Enum_Connection_Mode;

/** Define the list of security mode */
typedef enum Exalt_Enum_Security_Mode
{
    /** Open security mode */
    EXALT_SECURITY_OPEN,
    /** Shared security mode */
    EXALT_SECURITY_SHARED
} Exalt_Enum_Security_Mode;

/** Define the list of mode */
typedef enum Exalt_Enum_Mode
{
    /** Static mode */
    EXALT_STATIC,
    /** DHCP mode */
    EXALT_DHCP
} Exalt_Enum_Mode;



#include "libexalt.h"
#include <Eet.h>



Exalt_Connection* exalt_conn_new();
short exalt_conn_is_dhcp(Exalt_Connection* c);
Exalt_Connection* exalt_conn_custom_new(Exalt_Enum_Mode mode,
        const char* ip, const char* netmask, const char* gateway,
        short is_wireless, const char* essid,
        Exalt_Enum_Encryption_Mode encryption_mode,
        const char* key,
        Exalt_Enum_Connection_Mode connection_mode,
        Exalt_Enum_Security_Mode security_mode);
void exalt_conn_free(Exalt_Connection* c);

short exalt_conn_is_valid(Exalt_Connection* c);

short exalt_conn_set_mode(Exalt_Connection* c, Exalt_Enum_Mode mode);
short exalt_conn_set_ip(Exalt_Connection* c, const char* ip);
short exalt_conn_set_netmask(Exalt_Connection* c, const char* netmask);
short exalt_conn_set_gateway(Exalt_Connection* c, const char* gateway);
short exalt_conn_set_wireless(Exalt_Connection* c, short is_wireless);
short exalt_conn_set_essid(Exalt_Connection* c, const char* essid);
short exalt_conn_set_key(Exalt_Connection* c, const char* key);
short exalt_conn_set_encryption_mode(Exalt_Connection* c, Exalt_Enum_Encryption_Mode encryption_mode);
short exalt_conn_set_connection_mode(Exalt_Connection* c, Exalt_Enum_Connection_Mode connection_mode);
short exalt_conn_set_security_mode(Exalt_Connection* c, Exalt_Enum_Security_Mode security_mode);

short exalt_conn_is_dhcp(Exalt_Connection* c);
Exalt_Enum_Mode exalt_conn_get_mode(Exalt_Connection* c);
const char* exalt_conn_get_ip(Exalt_Connection *c);
const char* exalt_conn_get_gateway(Exalt_Connection *c);
const char* exalt_conn_get_netmask(Exalt_Connection *c);
short exalt_conn_is_wireless(Exalt_Connection* c);
const char* exalt_conn_get_essid(Exalt_Connection *c);
const char* exalt_conn_get_key(Exalt_Connection *c);
Exalt_Enum_Encryption_Mode exalt_conn_get_encryption_mode(Exalt_Connection* c);
Exalt_Enum_Security_Mode exalt_conn_get_security_mode(Exalt_Connection* c);
Exalt_Enum_Connection_Mode exalt_conn_get_connection_mode(Exalt_Connection* c);


Eet_Data_Descriptor * exalt_conn_edd_new();

int exalt_conn_set_cmd(Exalt_Connection *c, const char* cmd);
const char* exalt_conn_get_cmd(Exalt_Connection* c);

/** @} */

#endif   /* ----- #ifndef EXALT_CONNECTION_INC  ----- */

