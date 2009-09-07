/** @file exalt_configuration.h */

#ifndef  EXALT_CONFIGURATION_INC
#define  EXALT_CONFIGURATION_INC

/**
 * @defgroup Exalt_Configuration
 * @brief The Exalt_Configuration structure contains the information about a configuration (DHCP or static mode, IP address ...). This structure is used to apply a new configuration to an interface.
 * @ingroup Exalt
 * @{
 */

/**
 * @brief Information about a Configuration
 */
typedef struct Exalt_Configuration Exalt_Configuration;

/** The both mode : DHCP or static */
typedef enum Exalt_Enum_Mode
{
    /** Static mode */
    EXALT_STATIC,
    /** DHCP mode */
    EXALT_DHCP
} Exalt_Enum_Mode;



#include "Exalt.h"
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
 * @param c the Configuration
 */
EAPI void exalt_conf_free(Exalt_Configuration** c);

/**
 * @brief test if a configuration is valid (valid address ...)
 * @param c the Configuration
 * @return Returns 1 if the Configuration is valid, else 0
 * TODO: UPDATE this method to take care of the wireless configuration
 */
EAPI short exalt_conf_valid_is(Exalt_Configuration* c);

/**
 * @brief set the mode (dhcp or static)
 * @param c the configuration
 * @param mode the new mode
 */
EAPI void exalt_conf_mode_set(
        Exalt_Configuration* c,
        Exalt_Enum_Mode mode);

/**
 * @brief set the IP address
 * @param c the configuration
 * @param ip the new IP address
 */
EAPI void exalt_conf_ip_set(
        Exalt_Configuration* c,
        const char* ip);

/**
 * @brief set the netmask address
 * @param c the configuration
 * @param netmask the new netmask address
 */
EAPI void exalt_conf_netmask_set(
        Exalt_Configuration* c,
        const char* netmask);

/**
 * @brief set the gateway address
 * @param c the configuration
 * @param gateway the new gateway address
 */
EAPI void exalt_conf_gateway_set(
        Exalt_Configuration* c,
        const char* gateway);

/**
 * @brief set the command execute after the Configuration is applied
 * @param c the configuration
 * @param cmd the new commmand
 */
EAPI void exalt_conf_cmd_after_apply_set(
        Exalt_Configuration* c,
        const char* cmd);

/**
 * @brief set if the Configuration has a wireless extension
 * @param c the configuration
 * @param is_wireless 1 or 0.
 */
EAPI void exalt_conf_wireless_set(
        Exalt_Configuration* c,
        int is_wireless);

/**
 * @brief set the network extension
 * @param c the configuration
 * @param n the wireless network configuration
 */
EAPI void exalt_conf_network_set(
        Exalt_Configuration* c,
        Exalt_Configuration_Network* n);

/**
 * @brief Returns the mode of the Configuration
 * @param c the configuration
 * @return Returns the mode : DHCP or static
 */
EAPI Exalt_Enum_Mode exalt_conf_mode_get(Exalt_Configuration* c);


/**
 * @brief Test if the configuration has a wireless extension
 * @param c the configuration
 * @return Returns 1 if the configuration has a wireless extension, else 0
 */
EAPI int exalt_conf_wireless_is(Exalt_Configuration* conf);



/**
 * @brief Returns the IP address of the Configuration
 * @param c the configuration
 * @return Returns the IP address
 */
EAPI const char* exalt_conf_ip_get(Exalt_Configuration* c);

/**
 * @brief Returns the netmask address of the Configuration
 * @param c the configuration
 * @return Returns the netmask address
 */
EAPI const char* exalt_conf_netmask_get(Exalt_Configuration* c);

/**
 * @brief Returns the gateway address of the Configuration
 * @param c the configuration
 * @return Returns the gateway address
 */
EAPI const char* exalt_conf_gateway_get(Exalt_Configuration* c);

/**
 * @brief Returns the command execute after the Configuration is applied
 * @param c the configuration
 * @return Returns the command
 */
EAPI const char* exalt_conf_cmd_after_apply_get(Exalt_Configuration* c);

/**
 * @brief Returns the wireless network information (essid, encryption ...)
 * @param c the configuration
 * @param Returns the wireless network extension
 */
EAPI Exalt_Configuration_Network* exalt_conf_network_get(Exalt_Configuration* c);

/**
 * @brief Create an eet descriptor of the structure Exalt_Configuration
 * @param edd_network the eet dsecriptor of the wireless network extension
 * @return Returns the descriptor
 */
EAPI Eet_Data_Descriptor * exalt_conf_edd_new(Eet_Data_Descriptor* eed_network);


/** @} */

#endif   /* ----- #ifndef EXALT_CONFIGURATION_INC  ----- */

