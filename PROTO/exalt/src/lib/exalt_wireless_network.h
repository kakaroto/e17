/**  @file exalt_wireless_network.h */
#ifndef EXALT_WIRELESS_NETWORK_H
#define EXALT_WIRELESS_NETWORK_H

#include <stdio.h>



/**
 * @defgroup Exalt_Wireless_Network
 * @brief The Exalt_Wireless_Network contains all informations about a wireless network (essid, protocol ...)
 * @ingroup Exalt
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
typedef enum Exalt_Wireless_Network_Security Exalt_Wireless_Network_Security;
typedef enum Exalt_Wireless_Network_Mode Exalt_Wireless_Network_Mode;

#include "exalt_wireless.h"
#include "libexalt.h"
#include "exalt_configuration.h"
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
    CYPHER_NAME_TKIP,
    CYPHER_NAME_CCMP,
    CYPHER_NAME_UNKNOWN
};
#define EXALT_WIRELESS_NETWORK_CYPHER_NAME_NUM 8

enum Exalt_Wireless_Network_Auth_Suites
{
    AUTH_SUITES_NONE,
    AUTH_SUITES_EAP,
    AUTH_SUITES_PSK,
    AUTH_SUITES_UNKNOWN
};
#define EXALT_WIRELESS_NETWORK_AUTH_SUITES_NUM 5

enum Exalt_Wireless_Network_Security
{
    SECURITY_NONE,
    SECURITY_RESTRICTED,
    SECURITY_OPEN
};

enum Exalt_Wireless_Network_Mode
{
    MODE_INFRASTRUCTURE,
    MODE_IBSS,
    MODE_AP,
    MODE_UNKNOWN
};

EAPI Exalt_Wireless_Network* exalt_wireless_network_new(
        Exalt_Wireless* wn2);
EAPI void exalt_wireless_network_free(
        Exalt_Wireless_Network** wn);

EAPI Exalt_Wireless_Network_IE* exalt_wireless_network_ie_new();
EAPI void exalt_wireless_network_ie_free(
        Exalt_Wireless_Network_IE **ie);

/**
 * @brief set the interface
 */
EAPI void exalt_wireless_network_iface_set(
        Exalt_Wireless_Network *wn,
        Exalt_Wireless* w);

/**
 * @brief set the interface
 */
EAPI void exalt_wireless_network_iface_set(
        Exalt_Wireless_Network *wn,
        Exalt_Wireless* w);

/**
 * @brief set the MAC address
 */
EAPI void exalt_wireless_network_address_set(
        Exalt_Wireless_Network *wn,
        const char* address);

/**
 * @brief set the essid
 */
EAPI void exalt_wireless_network_essid_set(
        Exalt_Wireless_Network *wn,
        const char* essid);

/**
 * @brief set the mode
 */
EAPI void exalt_wireless_network_mode_set(
        Exalt_Wireless_Network *wn,
        Exalt_Wireless_Network_Mode mode);



/**
 * @brief set if the wireless network is encrypted
 */
EAPI void exalt_wireless_network_encryption_set(
        Exalt_Wireless_Network *wn,
        int encryption_is);

/**
 * @brief set the description (WPA/TKip ...))
 */
EAPI void exalt_wireless_network_description_set(
        Exalt_Wireless_Network *wn,
        const char* desc);

/**
 * @brief set the quality
 */
EAPI void exalt_wireless_network_quality_set(
        Exalt_Wireless_Network *wn,
        int quality);

/**
 * @brief set the list of ie
 * @param ie a list of Exalt_Wireless_Network_IE*
 */
EAPI void exalt_wireless_network_ie_set(
        Exalt_Wireless_Network *wn,
        Eina_List* ie);

/**
 * @brief get the interface
 */
EAPI Exalt_Wireless* exalt_wireless_network_iface_get(
        Exalt_Wireless_Network *wn);

/**
 * @brief get the MAC address
 */
EAPI const char* exalt_wireless_network_address_get(
        Exalt_Wireless_Network *wn);

/**
 * @brief get the essid
 */
EAPI const char* exalt_wireless_network_essid_get(
        Exalt_Wireless_Network *wn);

/**
 * @brief get the mode
 */
EAPI Exalt_Wireless_Network_Mode exalt_wireless_network_mode_get(
        Exalt_Wireless_Network *wn);



/**
 * @brief get if the wireless network is encrypted
 */
EAPI int exalt_wireless_network_encryption_is(
        Exalt_Wireless_Network *wn);

/**
 * @brief get the description
 */
EAPI const char* exalt_wireless_network_description_get(
        Exalt_Wireless_Network *wn);


/**
 * @brief get the quality
 */
EAPI int exalt_wireless_network_quality_get(
        Exalt_Wireless_Network *wn);

/**
 * @brief get the ie list
 * @return a list of Exalt_Wireless_Network_IE*
 */
EAPI Eina_List* exalt_wireless_network_ie_get(
        Exalt_Wireless_Network *wn);



/**
 * @brief set the description
 */
EAPI void exalt_wireless_network_ie_description_set(
        Exalt_Wireless_Network_IE *ie,
        const char* desc);

/**
 * @brief set the wpa type
 */
EAPI void exalt_wireless_network_ie_wpa_type_set(
        Exalt_Wireless_Network_IE *ie,
        Exalt_Wireless_Network_Wpa_Type wpa_type);

/**
 * @brief set the group cypher
 */
EAPI void exalt_wireless_network_ie_group_cypher_set(
        Exalt_Wireless_Network_IE *ie,
        Exalt_Wireless_Network_Cypher_Name group_cypher);

/**
 * @brief set a pairwise cypher
 */
EAPI void exalt_wireless_network_ie_pairwise_cypher_set(
        Exalt_Wireless_Network_IE *ie,
        Exalt_Wireless_Network_Cypher_Name pairwise_cypher,
        int i);

/**
 * @brief set the number of pairwise cypher
 */
EAPI void exalt_wireless_network_ie_pairwise_cypher_number_set(
        Exalt_Wireless_Network_IE *ie,
        int number);

/**
 * @brief set a authentification suite
 */
EAPI void exalt_wireless_network_ie_auth_suites_set(
        Exalt_Wireless_Network_IE *ie,
        Exalt_Wireless_Network_Auth_Suites auth_suites,
        int i);

/**
 * @brief set the number of auth suites
 */
EAPI void exalt_wireless_network_ie_auth_suites_number_set(
        Exalt_Wireless_Network_IE *ie,
        int number);

/**
 * @brief retrieve the ie description
 */
EAPI const char* exalt_wireless_network_ie_description_get(
        Exalt_Wireless_Network_IE *ie);



/**
 * @brief retrieve the wpa type
 */
EAPI Exalt_Wireless_Network_Wpa_Type exalt_wireless_network_ie_wpa_type_get(
        Exalt_Wireless_Network_IE *ie);

/**
 * @brief retrieve group cypher
 */
EAPI Exalt_Wireless_Network_Cypher_Name exalt_wireless_network_ie_group_cypher_get(
        Exalt_Wireless_Network_IE *ie);

/**
 * @brief retrieve a pairwise cypher
 */
EAPI Exalt_Wireless_Network_Cypher_Name exalt_wireless_network_ie_pairwise_cypher_get(
        Exalt_Wireless_Network_IE *ie,
        int i);

/**
 * @brief retrieve the number of pairwise cypher
 */
EAPI int exalt_wireless_network_ie_pairwise_cypher_number_get(
        Exalt_Wireless_Network_IE *ie);

/**
 * @brief retrieve a auth suites
 */
EAPI Exalt_Wireless_Network_Auth_Suites exalt_wireless_network_ie_auth_suites_get(
        Exalt_Wireless_Network_IE *ie,
        int i);

/**
 * @brief retrieve the number of auth suites
 */
EAPI int exalt_wireless_network_ie_auth_suites_number_get(
        Exalt_Wireless_Network_IE *ie);

/**
 * @brief get a string from a mode id
 */
EAPI const char* exalt_wireless_network_name_from_mode_id(
        int id);

/*
 * @brief get a string from a mode
 */
EAPI const char* exalt_wireless_network_name_from_mode(
        Exalt_Wireless_Network_Mode mode);

/*
 * @brief get a mode from a mode id
 */
EAPI Exalt_Wireless_Network_Mode exalt_wireless_network_mode_from_mode_id(
        int id);

/*
 * @brief get a string from a wpa type
 */
EAPI const char* exalt_wireless_network_name_from_wpa_type(
        Exalt_Wireless_Network_Wpa_Type wpa_type);

/*
 * @brief get a string from a cypher name
 */
EAPI const char* exalt_wireless_network_name_from_cypher_name(
        Exalt_Wireless_Network_Cypher_Name cypher_name);

/*
 * @brief get a string from a authentification suites
 */
EAPI const char* exalt_wireless_network_name_from_auth_suites(
        Exalt_Wireless_Network_Auth_Suites auth_suites);

/*
 * @brief get a string from a security mode
 */
EAPI const char* exalt_wireless_network_name_from_security(
        Exalt_Wireless_Network_Security security);


EAPI Eet_Data_Descriptor * exalt_wireless_network_ie_edd_new();

EAPI Eet_Data_Descriptor * exalt_wireless_network_edd_new(Eet_Data_Descriptor* edd_ie);

/** @} */

#endif

