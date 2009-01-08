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


/**
 * @brief create a default connection
 * - no wireless
 * - dhcp mode
 * @return Returns the new connection
 */
Exalt_Connection* exalt_conn_new();

/**
 * @brief free a connection
 * @param c the connection
 */
void exalt_conn_free(Exalt_Connection** c);

/**
 * @brief test if a connection is valid (valid address ...)
 * @param c the connection
 * @return Returns 1 if the connection is valid, else 0
 */
short exalt_conn_valid_is(Exalt_Connection* c);

/**
 * @brief set the mode (dhcp or static)
 */
void exalt_conn_mode_set(
        Exalt_Connection* conn,
        Exalt_Enum_Mode mode);

/**
 * @brief set the IP address
 */
void exalt_conn_ip_set(
        Exalt_Connection* conn,
        const char* ip);

/**
 * @brief set the netmask address
 */
void exalt_conn_netmask_set(
        Exalt_Connection* conn,
        const char* netmask);

/**
 * @brief set the gateway address
 */
void exalt_conn_gateway_set(
        Exalt_Connection* conn,
        const char* gateway);

/**
 * @brief set the command execute after the connection is applied
 */
void exalt_conn_cmd_after_apply_set(
        Exalt_Connection* conn,
        const char* cmd);

/**
 * @brief set if the connection has a wireless extension
 */
void exalt_conn_wireless_set(
        Exalt_Connection* conn,
        int is_wireless);

/**
 * @brief set the essid
 */
void exalt_conn_essid_set(
        Exalt_Connection* conn,
        const char* essid);

/**
 * @brief set the key
 */
void exalt_conn_key_set(
        Exalt_Connection* conn,
        const char* key);

/**
 * @brief set the encryption mode
 */
void exalt_conn_encryption_mode_set(
        Exalt_Connection* conn,
        Exalt_Enum_Encryption_Mode encryption_mode);

/**
 * @brief set the connection mode
 */
void exalt_conn_connection_mode_set(
        Exalt_Connection* conn,
        Exalt_Enum_Connection_Mode connection_mode);
/**
 * @brief set the security mode
 */
void exalt_conn_security_mode_set(
        Exalt_Connection* conn,
        Exalt_Enum_Security_Mode security_mode);


/**
 * @brief return the mode of the connection
 */
Exalt_Enum_Mode exalt_conn_mode_get(Exalt_Connection* conn);

/**
 * @brief return the IP address of the connection
 */
const char* exalt_conn_ip_get(Exalt_Connection* conn);

/**
 * @brief return the netmask address of the connection
 */
const char* exalt_conn_netmask_get(Exalt_Connection* conn);

/**
 * @brief return the gateway address of the connection
 */
const char* exalt_conn_gateway_get(Exalt_Connection* conn);

/**
 * @brief return the command execute after the connection is applied
 */
const char* exalt_conn_cmd_after_apply_get(Exalt_Connection* conn);

/**
 * @brief return test if the connection has a wireless extension
 */
int exalt_conn_wireless_is(Exalt_Connection* conn);

/**
 * @brief return the essid of the connection
 */
const char* exalt_conn_essid_get(Exalt_Connection* conn);

/**
 * @brief return the key of the connection
 */
const char* exalt_conn_key_get(Exalt_Connection* conn);

/**
 * @brief return the encryption mode of the connection
 */
Exalt_Enum_Encryption_Mode exalt_conn_encryption_mode_get(Exalt_Connection* conn);

/**
 * @brief return the security mode of the connection
 */
Exalt_Enum_Security_Mode exalt_conn_security_mode_get(Exalt_Connection* conn);

/**
 * @brief return the connection mode of the connection
 */
Exalt_Enum_Connection_Mode exalt_conn_connection_mode_get(Exalt_Connection* conn);


/**
 * @brief create an eet descriptor of the structure Exalt_Connection
 * @return Returns the descriptor
 */
Eet_Data_Descriptor * exalt_conn_edd_new();


/** @} */

#endif   /* ----- #ifndef EXALT_CONNECTION_INC  ----- */

