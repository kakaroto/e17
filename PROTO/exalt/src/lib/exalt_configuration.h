/** @file exalt_configuration.h */

#ifndef  EXALT_CONFIGURATION_INC
#define  EXALT_CONFIGURATION_INC

/**
 * @defgroup Exalt_Configuration
 * @brief The Exalt_Configuration struct contains all information about a configuration (dhcp or static mode, ip, essid ...)
 * @ingroup Exalt
 * @{
 */

/**
 * @brief Information about a Configuration
 * @structinfo
 */
typedef struct Exalt_Configuration Exalt_Configuration;

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

/** Define the list of Configuration mode*/
typedef enum Exalt_Enum_Configuration_Mode
{
    /** Ad-Hoc Configuration mode*/
    EXALT_Configuration_ADHOC,
    /** Managed Configuration mode*/
    EXALT_Configuration_MANAGED
} Exalt_Enum_Configuration_Mode;

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
#include "exalt_configuration_network.h"

/**
 * @brief create a default Configuration
 * - no wireless
 * - dhcp mode
 * @return Returns the new Configuration
 */
EAPI Exalt_Configuration* exalt_conf_new();

/**
 * @brief free a Configuration
 * Warning: This method doesn't free the network inside the Configuration.
 * @param c the Configuration
 */
EAPI void exalt_conf_free(Exalt_Configuration** c);

/**
 * @brief test if a Configuration is valid (valid address ...)
 * @param c the Configuration
 * @return Returns 1 if the Configuration is valid, else 0
 */
EAPI short exalt_conf_valid_is(Exalt_Configuration* c);

/**
 * @brief set the mode (dhcp or static)
 */
EAPI void exalt_conf_mode_set(
        Exalt_Configuration* conf,
        Exalt_Enum_Mode mode);

/**
 * @brief set the IP address
 */
EAPI void exalt_conf_ip_set(
        Exalt_Configuration* conf,
        const char* ip);

/**
 * @brief set the netmask address
 */
EAPI void exalt_conf_netmask_set(
        Exalt_Configuration* conf,
        const char* netmask);

/**
 * @brief set the gateway address
 */
EAPI void exalt_conf_gateway_set(
        Exalt_Configuration* conf,
        const char* gateway);

/**
 * @brief set the command execute after the Configuration is applied
 */
EAPI void exalt_conf_cmd_after_apply_set(
        Exalt_Configuration* conf,
        const char* cmd);

/**
 * @brief set if the Configuration has a wireless extension
 */
EAPI void exalt_conf_wireless_set(
        Exalt_Configuration* conf,
        int is_wireless);

/**
 * @brief set the essid
 */
EAPI void exalt_conf_network_set(
        Exalt_Configuration* conf,
        Exalt_Configuration_Network* n);

/**
 * @brief Returns the mode of the Configuration
 */
EAPI Exalt_Enum_Mode exalt_conf_mode_get(Exalt_Configuration* conf);


/**
 * @brief return if the wep key is a hexa key
 * (else it is a plain text)
 */
EAPI int exalt_conf_wireless_is(Exalt_Configuration* conf);



/**
 * @brief return the IP address of the Configuration
 */
EAPI const char* exalt_conf_ip_get(Exalt_Configuration* conf);

/**
 * @brief return the netmask address of the Configuration
 */
EAPI const char* exalt_conf_netmask_get(Exalt_Configuration* conf);

/**
 * @brief return the gateway address of the Configuration
 */
EAPI const char* exalt_conf_gateway_get(Exalt_Configuration* conf);

/**
 * @brief return the command execute after the Configuration is applied
 */
EAPI const char* exalt_conf_cmd_after_apply_get(Exalt_Configuration* conf);

/**
 * @brief return test if the Configuration has a wireless extension
 */
EAPI int exalt_conf_wireless_is(Exalt_Configuration* conf);

/**
 * @brief return the network information (essid, encyption ...)
 */
EAPI Exalt_Configuration_Network* exalt_conf_network_get(Exalt_Configuration* conf);

/**
 * @brief return the Configuration mode of the Configuration
 */
EAPI Exalt_Enum_Configuration_Mode exalt_conf_Configuration_mode_get(Exalt_Configuration* conf);


/**
 * @brief create an eet descriptor of the structure Exalt_Configuration
 * @return Returns the descriptor
 */
EAPI Eet_Data_Descriptor * exalt_conf_edd_new(Eet_Data_Descriptor* eed_network);


/** @} */

#endif   /* ----- #ifndef EXALT_CONFIGURATION_INC  ----- */

