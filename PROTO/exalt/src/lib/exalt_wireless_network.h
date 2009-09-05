/**  @file exalt_wireless_network.h */
#ifndef EXALT_WIRELESS_NETWORK_H
#define EXALT_WIRELESS_NETWORK_H

#include <stdio.h>



/**
 * @defgroup Exalt_Wireless_Network
 * @brief The Exalt_Wireless_Network contains all information about a wireless network (essid, protocol ...)
 * @ingroup Exalt
 * @{
 */

/**
 * @brief Information about a wireless network
 * @structinfo
 */
typedef struct Exalt_Wireless_Network Exalt_Wireless_Network;
/**
 * @brief Information about a wpa element
 */
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

/**
 * Type of WPA encryption : 1 or 2
 */
enum Exalt_Wireless_Network_Wpa_Type
{
    WPA_TYPE_UNKNOWN,
    WPA_TYPE_WPA,
    WPA_TYPE_WPA2
};

/**
 * @brief The list of available cypher
 */
enum Exalt_Wireless_Network_Cypher_Name
{
    CYPHER_NAME_NONE,
    CYPHER_NAME_TKIP,
    CYPHER_NAME_CCMP,
    CYPHER_NAME_UNKNOWN
};
#define EXALT_WIRELESS_NETWORK_CYPHER_NAME_NUM 4

/**
 * @brief The list of available authentication suites
 */
enum Exalt_Wireless_Network_Auth_Suites
{
    AUTH_SUITES_NONE,
    AUTH_SUITES_EAP,
    AUTH_SUITES_PSK,
    AUTH_SUITES_UNKNOWN
};
#define EXALT_WIRELESS_NETWORK_AUTH_SUITES_NUM 4

/**
 * @brief The list of security mode
 */
enum Exalt_Wireless_Network_Security
{
    SECURITY_NONE,
    SECURITY_RESTRICTED,
    SECURITY_OPEN
};

/**
 * @brief The list of mode (IBSS = ad-hoc)
 */
enum Exalt_Wireless_Network_Mode
{
    MODE_INFRASTRUCTURE,
    MODE_IBSS,
    MODE_AP,
    MODE_UNKNOWN
};

/**
 * @brief Create a new wireless network structure
 * @param wn2 the wireless interface associated to this network
 * @return Returns the new wireless network structure
 */
EAPI Exalt_Wireless_Network* exalt_wireless_network_new(
        Exalt_Wireless* wn2);
/**
 * @brief Free a wireless network
 * @param wn the wireless network
 */
EAPI void exalt_wireless_network_free(
        Exalt_Wireless_Network** wn);

/**
 * @brief Create a new WPA element
 * @return Returns the new element
 */
EAPI Exalt_Wireless_Network_IE* exalt_wireless_network_ie_new();
/**
 * @brief Free a wpa element
 * @param ie the element
 */
EAPI void exalt_wireless_network_ie_free(
        Exalt_Wireless_Network_IE **ie);

/**
 * @brief Set the wireless interface associated to the wireless network
 * @param wn the network
 * @param w the interface
 */
EAPI void exalt_wireless_network_iface_set(
        Exalt_Wireless_Network *wn,
        Exalt_Wireless* w);


/**
 * @brief Set the MAC address
 * @param wn the network
 * @param address the new MAC address
 */
EAPI void exalt_wireless_network_address_set(
        Exalt_Wireless_Network *wn,
        const char* address);

/**
 * @brief Set the essid
 * @param wn the network
 * @param essid the new essid
 */
EAPI void exalt_wireless_network_essid_set(
        Exalt_Wireless_Network *wn,
        const char* essid);

/**
 * @brief Set the mode
 * @param wn the network
 * @param mode the new mode
 */
EAPI void exalt_wireless_network_mode_set(
        Exalt_Wireless_Network *wn,
        Exalt_Wireless_Network_Mode mode);



/**
 * @brief Set if the wireless network is encrypted
 * @param wn the network
 * @param encryption_is 1 if the wireless network is encrypted, else 0
 */
EAPI void exalt_wireless_network_encryption_set(
        Exalt_Wireless_Network *wn,
        int encryption_is);

/**
 * @brief Set a string description (WPA/TKip ...)
 * @param wn the network
 * @param desc the new description
 * @note When we performs a wireless scan, the description is retrieve from wpa_supplicant.
 */
EAPI void exalt_wireless_network_description_set(
        Exalt_Wireless_Network *wn,
        const char* desc);

/**
 * @brief Set the network quality
 * @param wn the network
 * @param the new quality
 */
EAPI void exalt_wireless_network_quality_set(
        Exalt_Wireless_Network *wn,
        int quality);

/**
 * @brief Set the list of wpa element
 * @param wn the network
 * @param ie a list of Exalt_Wireless_Network_IE*
 */
EAPI void exalt_wireless_network_ie_set(
        Exalt_Wireless_Network *wn,
        Eina_List* ie);

/**
 * @brief Get the wireless interface associated to the network
 * @param wn the network
 * @return Returns the interface
 */
EAPI Exalt_Wireless* exalt_wireless_network_iface_get(
        Exalt_Wireless_Network *wn);

/**
 * @brief Get the MAC address
 * @param wn the network
 * @return Returns the MAC address
 */
EAPI const char* exalt_wireless_network_address_get(
        Exalt_Wireless_Network *wn);

/**
 * @brief Get the essid
 * @param wn the network
 * @return Returns the essid
 */
EAPI const char* exalt_wireless_network_essid_get(
        Exalt_Wireless_Network *wn);

/**
 * @brief Get the mode
 * @param wn the network
 * @return Returns the mode
 */
EAPI Exalt_Wireless_Network_Mode exalt_wireless_network_mode_get(
        Exalt_Wireless_Network *wn);



/**
 * @brief Test if the wireless network is encrypted
 * @param wn the network
 * @return Returns 1 if the network is encrypted, else 0
 */
EAPI int exalt_wireless_network_encryption_is(
        Exalt_Wireless_Network *wn);

/**
 * @brief Get the description
 * @param wn the network
 * @return Returns the description
 */
EAPI const char* exalt_wireless_network_description_get(
        Exalt_Wireless_Network *wn);


/**
 * @brief Get the quality
 * @param wn the network
 * @return Returns the quality
 */
EAPI int exalt_wireless_network_quality_get(
        Exalt_Wireless_Network *wn);

/**
 * @brief Get the list of wpa element
 * @param wn the network
 * @return Returns the list of wpa element
 */
EAPI Eina_List* exalt_wireless_network_ie_get(
        Exalt_Wireless_Network *wn);



/**
 * @brief Set the description
 * @param ie the wpa element
 * @param desc the new description
 */
EAPI void exalt_wireless_network_ie_description_set(
        Exalt_Wireless_Network_IE *ie,
        const char* desc);

/**
 * @brief Set the wpa type
 * @param ie the wpa element
 * @param wpa_type the new wpa type
 */
EAPI void exalt_wireless_network_ie_wpa_type_set(
        Exalt_Wireless_Network_IE *ie,
        Exalt_Wireless_Network_Wpa_Type wpa_type);

/**
 * @brief Get the cypher group
 * @param ie the wpa element
 * @param group_cypher the new cypher group
 */
EAPI void exalt_wireless_network_ie_group_cypher_set(
        Exalt_Wireless_Network_IE *ie,
        Exalt_Wireless_Network_Cypher_Name group_cypher);

/**
 * @brief Set a pairwise cypher
 * @param ie the wpa element
 * @param pairwise_cypher the new pairwise cypher
 * @param i the id of the pairwise cypher
 * @note A wpa element can have more than 1 pairwise cypher, the maximum is EXALT_WIRELESS_NETWORK_CYPHER_NAME_NUM().
 * @note Do not forget to set the number of pairwise cypher with exalt_wireless_network_ie_pairwise_cypher_number_set()
 */
EAPI void exalt_wireless_network_ie_pairwise_cypher_set(
        Exalt_Wireless_Network_IE *ie,
        Exalt_Wireless_Network_Cypher_Name pairwise_cypher,
        int i);

/**
 * @brief Set the number of pairwise cypher
 * @param ie the wpa element
 * @param number the number of pairwise cypher
 */
EAPI void exalt_wireless_network_ie_pairwise_cypher_number_set(
        Exalt_Wireless_Network_IE *ie,
        int number);

/**
 * @brief Set a authentication suite
 * @param ie the wpa element
 * @param auth_suites the new authentication suite
 * @param i the id of the authentication suite
 * @note A wpa element can have more than 1 authentication suite, the maximum is EXALT_WIRELESS_NETWORK_AUTH_SUITES_NUM().
 * @note Do not forget to set the number of authentication suite with exalt_wireless_network_ie_auth_suites_number_set()
 */
EAPI void exalt_wireless_network_ie_auth_suites_set(
        Exalt_Wireless_Network_IE *ie,
        Exalt_Wireless_Network_Auth_Suites auth_suites,
        int i);

/**
 * @brief Set the number of authentication suites
 * @param ie the wpa element
 * @param number the number of authentication suite
 */
EAPI void exalt_wireless_network_ie_auth_suites_number_set(
        Exalt_Wireless_Network_IE *ie,
        int number);

/**
 * @brief Get the description
 * @param ie the wpa element
 * @return Returns the description
 */
EAPI const char* exalt_wireless_network_ie_description_get(
        Exalt_Wireless_Network_IE *ie);



/**
 * @brief Get the wpa type
 * @param ie the wpa element
 * @return Returns the wpa type
 */
EAPI Exalt_Wireless_Network_Wpa_Type exalt_wireless_network_ie_wpa_type_get(
        Exalt_Wireless_Network_IE *ie);

/**
 * @brief Get the cypher group
 * @param ie the wpa element
 * @return Returns the cypher group
 */
EAPI Exalt_Wireless_Network_Cypher_Name exalt_wireless_network_ie_group_cypher_get(
        Exalt_Wireless_Network_IE *ie);

/**
 * @brief Get a pairwise cypher
 * @param ie the wpa element
 * @param i the pairwise cypher position
 * @return Returns the pairwise cypher
 */
EAPI Exalt_Wireless_Network_Cypher_Name exalt_wireless_network_ie_pairwise_cypher_get(
        Exalt_Wireless_Network_IE *ie,
        int i);

/**
 * @brief Get the number of pairwise cypher
 * @param ie the wpa element
 * @return Returns the number of pairwise cypher
 */
EAPI int exalt_wireless_network_ie_pairwise_cypher_number_get(
        Exalt_Wireless_Network_IE *ie);

/**
 * @brief Get a authentication suites
 * @param ie the wpa element
 * @param i the authentication suite position
 * @return Returns the authentication suite
 */
EAPI Exalt_Wireless_Network_Auth_Suites exalt_wireless_network_ie_auth_suites_get(
        Exalt_Wireless_Network_IE *ie,
        int i);

/**
 * @brief Get the number of authentication suites
 * @param ie the wpa element
 * @return Returns the number of authentication suites
 */
EAPI int exalt_wireless_network_ie_auth_suites_number_get(
        Exalt_Wireless_Network_IE *ie);


/**
 * @brief Get a string from a mode id
 * @param id the mode id
 * @return Returns a string which describes the mode
 */
EAPI const char* exalt_wireless_network_name_from_mode_id(
        int id);

/**
 * @brief Get a string from a mode
 * @param id the mode
 * @return Returns a string which describes the mode
 */
EAPI const char* exalt_wireless_network_name_from_mode(
        Exalt_Wireless_Network_Mode mode);

/**
 * @brief Get a mode from a mode id
 * @param id the mode id
 * @return Returns the mode
 */
EAPI Exalt_Wireless_Network_Mode exalt_wireless_network_mode_from_mode_id(
        int id);

/**
 * @brief Get a string from a wpa type
 * @param wpa_type the wpa_type
 * @return Returns a string which describes the wpa type
 */
EAPI const char* exalt_wireless_network_name_from_wpa_type(
        Exalt_Wireless_Network_Wpa_Type wpa_type);

/**
 * @brief Get a string from a cypher name
 * @param cypher_name the cypher name
 * @return Returns a string which describes the cypher name
 */
EAPI const char* exalt_wireless_network_name_from_cypher_name(
        Exalt_Wireless_Network_Cypher_Name cypher_name);

/**
 * @brief Get a string from a authentication suites
 * @param auth_suites the authentication suite
 * @return Returns a string which describes the authentication suite
 */
EAPI const char* exalt_wireless_network_name_from_auth_suites(
        Exalt_Wireless_Network_Auth_Suites auth_suites);

/**
 * @brief Get a string from a security mode
 * @param security the security mode
 * @return Returns a string which describes the security mode
 */
EAPI const char* exalt_wireless_network_name_from_security(
        Exalt_Wireless_Network_Security security);


/**
 * @brief Create an eet descriptor for a wpa element
 * @return Returns the eet descriptor
 */
EAPI Eet_Data_Descriptor * exalt_wireless_network_ie_edd_new();

/**
 * @brief Create an eet descriptor for a wireless network
 * @param edd_ie the eet descriptor of a wpa element
 * @return Returns the eet descriptor
 */
EAPI Eet_Data_Descriptor * exalt_wireless_network_edd_new(Eet_Data_Descriptor* edd_ie);

/** @} */

#endif

