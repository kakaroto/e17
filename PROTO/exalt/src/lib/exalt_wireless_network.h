/**  @file exalt_wireless_network.h */
#ifndef EXALT_WIRELESS_NETWORK_H
#define EXALT_WIRELESS_NETWORK_H

#include <stdio.h>



/**
 * @defgroup Exalt_Wireless_Network
 * @brief The Exalt_Wireless_Network contains all informations about a wireless network (essid, protocol ...)
 * @{
 */

/**
 * @brief informations about a wireless network
 * @structinfo
 */
typedef struct Exalt_Wireless_Network Exalt_Wireless_Network;
typedef struct Exalt_Wireless_Network_IE Exalt_Wireless_Network_IE;
typedef enum Exalt_Wireless_Network_Wpa_Type Exalt_Wireless_Network_Wpa_Type;
typedef enum Exalt_Wireless_Network_Cypher_Name Exalt_Wireless_Network_Cypher_Name;
typedef enum Exalt_Wireless_Network_Auth_Suites Exalt_Wireless_Network_Auth_Suites;
typedef enum Exalt_Wireless_Network_Mode Exalt_Wireless_Network_Mode;
typedef enum Exalt_Wireless_Network_Security Exalt_Wireless_Network_Security;

#include "exalt_wireless.h"
#include "libexalt.h"
#include "exalt_connection.h"
#include <Eina.h>

enum Exalt_Wireless_Network_Wpa_Type
{
    WPA_TYPE_UNKNOWN,
    WPA_TYPE_WPA,
    WPA_TYPE_WPA2
};


enum Exalt_Wireless_Network_Cypher_Name
{
    CYPHER_NAME_NONE,
    CYPHER_NAME_WEP40,
    CYPHER_NAME_TKIP,
    CYPHER_NAME_WRAP,
    CYPHER_NAME_CCMP,
    CYPHER_NAME_WEP104,
    CYPHER_NAME_UNKNOWN,
    CYPHER_NAME_PROPRIETARY
};
#define EXALT_WIRELESS_NETWORK_CYPHER_NAME_NUM 8

enum Exalt_Wireless_Network_Auth_Suites
{
    AUTH_SUITES_NONE,
    AUTH_SUITES_8021X,
    AUTH_SUITES_PSK,
    AUTH_SUITES_PROPRIETARY,
    AUTH_SUITES_UNKNOWN
};
#define EXALT_WIRELESS_NETWORK_AUTH_SUITES_NUM 5

enum Exalt_Wireless_Network_Mode
{
    MODE_AUTO,
    MODE_AD_HOC,
    MODE_MANAGED,
    MODE_MASTER,
    MODE_REPEATER,
    MODE_SECONDARY,
    MODE_MONITOR,
    MODE_UNKNOW_BUG
};

enum Exalt_Wireless_Network_Security
{
    SECURITY_NONE,
    SECURITY_RESTRICTED,
    SECURITY_OPEN
};

Exalt_Wireless_Network* exalt_wireless_network_new(
        Exalt_Wireless* wn2);
void exalt_wireless_network_free(
        Exalt_Wireless_Network** wn);

Exalt_Wireless_Network_IE* exalt_wireless_network_ie_new();
void exalt_wireless_network_ie_free(
        Exalt_Wireless_Network_IE **ie);

/**
 * @brief set the interface
 */
void exalt_wireless_network_iface_set(
        Exalt_Wireless_Network *wn,
        Exalt_Wireless* w);

/**
 * @brief set the interface
 */
void exalt_wireless_network_iface_set(
        Exalt_Wireless_Network *wn,
        Exalt_Wireless* w);

/**
 * @brief set the MAC address
 */
void exalt_wireless_network_address_set(
        Exalt_Wireless_Network *wn,
        const char* address);

/**
 * @brief set the essid
 */
void exalt_wireless_network_essid_set(
        Exalt_Wireless_Network *wn,
        const char* essid);

/**
 * @brief set if the wireless network is encrypted
 */
void exalt_wireless_network_encryption_set(
        Exalt_Wireless_Network *wn,
        int encryption_is);

/**
 * @brief set the quality
 */
void exalt_wireless_network_quality_set(
        Exalt_Wireless_Network *wn,
        int quality);

/**
 * @brief set the mode
 */
void exalt_wireless_network_mode_set(
        Exalt_Wireless_Network *wn,
        Exalt_Wireless_Network_Mode mode);

/**
 * @brief set the security mode
 */
void exalt_wireless_network_security_mode_set(
        Exalt_Wireless_Network *wn,
        Exalt_Wireless_Network_Security security_mode);

/**
 * @brief set the list of ie
 * @param ie a list of Exalt_Wireless_Network_IE*
 */
void exalt_wireless_network_ie_set(
        Exalt_Wireless_Network *wn,
        Eina_List* ie);

/**
 * @brief get the interface
 */
Exalt_Wireless* exalt_wireless_network_iface_get(
        Exalt_Wireless_Network *wn);

/**
 * @brief get the MAC address
 */
const char* exalt_wireless_network_address_get(
        Exalt_Wireless_Network *wn);

/**
 * @brief get the essid
 */
const char* exalt_wireless_network_essid_get(
        Exalt_Wireless_Network *wn);

/**
 * @brief get if the wireless network is encrypted
 */
int exalt_wireless_network_encryption_is(
        Exalt_Wireless_Network *wn);

/**
 * @brief get the quality
 */
int exalt_wireless_network_quality_get(
        Exalt_Wireless_Network *wn);

/**
 * @brief get the mode
 */
Exalt_Wireless_Network_Mode exalt_wireless_network_mode_get(
        Exalt_Wireless_Network *wn);

/**
 * @brief get the security mode
 */
Exalt_Wireless_Network_Security exalt_wireless_network_security_mode_get(
        Exalt_Wireless_Network *wn);

/**
 * @brief get the ie list
 * @return a list of Exalt_Wireless_Network_IE*
 */
Eina_List* exalt_wireless_network_ie_get(
        Exalt_Wireless_Network *wn);


/**
 * @brief set the wpa type
 */
void exalt_wireless_network_ie_wpa_type_set(
        Exalt_Wireless_Network_IE *ie,
        Exalt_Wireless_Network_Wpa_Type wpa_type);

/**
 * @brief set the wpa version
 */
void exalt_wireless_network_ie_wpa_version_set(
        Exalt_Wireless_Network_IE *ie,
        int wpa_version);

/**
 * @brief set the group cypher
 */
void exalt_wireless_network_ie_group_cypher_set(
        Exalt_Wireless_Network_IE *ie,
        Exalt_Wireless_Network_Cypher_Name group_cypher);

/**
 * @brief set a pairwise cypher
 */
void exalt_wireless_network_ie_pairwise_cypher_set(
        Exalt_Wireless_Network_IE *ie,
        Exalt_Wireless_Network_Cypher_Name pairwise_cypher,
        int i);

/**
 * @brief set the number of pairwise cypher
 */
void exalt_wireless_network_ie_pairwise_cypher_number_set(
        Exalt_Wireless_Network_IE *ie,
        int number);

/**
 * @brief set a authentification suite
 */
void exalt_wireless_network_ie_auth_suites_set(
        Exalt_Wireless_Network_IE *ie,
        Exalt_Wireless_Network_Auth_Suites auth_suites,
        int i);

/**
 * @brief set the number of auth suites
 */
void exalt_wireless_network_ie_auth_suites_number_set(
        Exalt_Wireless_Network_IE *ie,
        int number);

/**
 * @brief set if the preauthentification is supported
 */
void exalt_wireless_network_ie_preauth_supported_set(
        Exalt_Wireless_Network_IE *ie,
        int is_supported);

/**
 * @brief set the default connection for this wireless network
 */
void exalt_wireless_network_ie_default_conn_set(
        Exalt_Wireless_Network_IE *ie,
        Exalt_Connection* conn);

/**
 * @brief retrieve the wpa type
 */
Exalt_Wireless_Network_Wpa_Type exalt_wireless_network_ie_wpa_type_get(
        Exalt_Wireless_Network_IE *ie);

/**
 * @brief retrieve the wpa version
 */
int exalt_wireless_network_ie_wpa_version_get(
        Exalt_Wireless_Network_IE *ie);

/**
 * @brief retrieve group cypher
 */
Exalt_Wireless_Network_Cypher_Name exalt_wireless_network_ie_group_cypher_get(
        Exalt_Wireless_Network_IE *ie);

/**
 * @brief retrieve a pairwise cypher
 */
Exalt_Wireless_Network_Cypher_Name exalt_wireless_network_ie_pairwise_cypher_get(
        Exalt_Wireless_Network_IE *ie,
        int i);

/**
 * @brief retrieve the number of pairwise cypher
 */
int exalt_wireless_network_ie_pairwise_cypher_number_get(
        Exalt_Wireless_Network_IE *ie);

/**
 * @brief retrieve a auth suites
 */
Exalt_Wireless_Network_Auth_Suites exalt_wireless_network_ie_auth_suites_get(
        Exalt_Wireless_Network_IE *ie,
        int i);

/**
 * @brief retrieve the number of auth suites
 */
int exalt_wireless_network_ie_auth_suites_number_get(
        Exalt_Wireless_Network_IE *ie);

/**
 * @brief test if the preauthentification is supported
 */
int exalt_wireless_network_ie_preauth_supported_is(
        Exalt_Wireless_Network_IE *ie);

/**
 * @brief retrieve the default connection
 */
Exalt_Connection* exalt_wireless_network_ie_default_conn_get(
        Exalt_Wireless_Network_IE *ie);

/**
 * @brief get a string from a mode id
 */
const char* exalt_wireless_network_name_from_mode_id(
        int id);

/*
 * @brief get a string from a mode
 */
const char* exalt_wireless_network_name_from_mode(
        Exalt_Wireless_Network_Mode mode);

/*
 * @brief get a mode from a mode id
 */
Exalt_Wireless_Network_Mode exalt_wireless_network_mode_from_mode_id(
        int id);

/*
 * @brief get a string from a wpa type
 */
const char* exalt_wireless_network_name_from_wpa_type(
        Exalt_Wireless_Network_Wpa_Type wpa_type);

/*
 * @brief get a string from a cypher name
 */
const char* exalt_wireless_network_name_from_cypher_name(
        Exalt_Wireless_Network_Cypher_Name cypher_name);

/*
 * @brief get a string from a authentification suites
 */
const char* exalt_wireless_network_name_from_auth_suites(
        Exalt_Wireless_Network_Auth_Suites auth_suites);

/*
 * @brief get a string from a security mode
 */
const char* exalt_wireless_network_name_from_security(
        Exalt_Wireless_Network_Security security);


/** @} */

#endif

