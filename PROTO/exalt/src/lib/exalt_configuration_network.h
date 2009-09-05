/** @file exalt_configuration.h */

#ifndef  EXALT_CONFIGURATION_NETWORK_INC
#define  EXALT_CONFIGURATION_NETWORK_INC

/**
 * @defgroup Exalt_Configuration_Network
 * @brief The Exalt configuration network structure contains the information about a wireless network configuration (essid, encryption ...). This structure is an extension of the structure Exalt_Configuration() and can not be used alone.
 * @ingroup Exalt
 * @{
 */

/**
 * @brief Information about a wireless network configuration (essid ...)
 */
typedef struct Exalt_Configuration_Network Exalt_Configuration_Network;

#include "libexalt.h"


/**
 * @brief Create a new network configuration
 * @return Returns the new network configuration
 */
EAPI Exalt_Configuration_Network *exalt_conf_network_new();
/**
 * @brief Free a network configuration
 * @param cn the network configuration
 */
EAPI void exalt_conf_network_free(Exalt_Configuration_Network **cn);
/**
 * @brief Print a network configuration in the standard output
 * @param cn the network configuration
 */
EAPI void exalt_conf_network_print(Exalt_Configuration_Network *cn);
/**
 * @brief Set the essid
 * @param cn the network configuration
 * @param essid the new essid
 */
EAPI void exalt_conf_network_essid_set(Exalt_Configuration_Network *cn, const char* essid);
/**
 * @brief Set if an encryption is set
 * @param cn the network configuration
 * @param is 1: the configuration use an encryption, else 0
 */
EAPI void exalt_conf_network_encryption_set(Exalt_Configuration_Network *cn, int is);
/**
 * @brief Set the mode (IBSS, INFRASTRUCTURE ...)
 * @param cn the network configuration
 * @param mode the new mode
 */
EAPI void exalt_conf_network_mode_set(Exalt_Configuration_Network *cn, Exalt_Wireless_Network_Mode mode);
/**
 * @brief Set the key
 * @param cn the network configuration
 * @param essid the new encryption key
 */
EAPI void exalt_conf_network_key_set(Exalt_Configuration_Network *cn, const char* key);
/**
 * @brief Set the login
 * @param cn the network configuration
 * @param login the new login
 */
EAPI void exalt_conf_network_login_set(Exalt_Configuration_Network *cn, const char* login);





/**
 * @brief Set if the encryption is WEP
 * @param cn the network configuration
 * @param is 1 if the encryption is WEP, else 0
 */
EAPI void exalt_conf_network_wep_set(Exalt_Configuration_Network *cn, int is);
/**
 * @brief Set the key is a hexadecimal key
 * @param cn the network configuration
 * @param is 1 if yes, else 0
 * @note this field is only used in the case of a WEP encryption
 */
EAPI void exalt_conf_network_wep_hexa_set(Exalt_Configuration_Network *cn, int is);







/**
 * @brief Set if the encryption is WPA/WPA2
 * @param cn the network configuration
 * @param is 1 if the encryption is WPA/WPA2, else 0
 */
EAPI void exalt_conf_network_wpa_set(Exalt_Configuration_Network *cn, int is);
/**
 * @brief Set wpa type : WPA or WPA2
 * @param cn the network configuration
 * @param wpa_type the new wpa type
 * @note Used only if the encryption is WPA or WPA2
 */
EAPI void exalt_conf_network_wpa_type_set(Exalt_Configuration_Network *cn, Exalt_Wireless_Network_Wpa_Type wpa_type);
/**
 * @brief Set the cypher group
 * @param cn the network configuration
 * @param cypher_name the new cypher group
 * @note Used only if the encryption is WPA or WPA2
 */
EAPI void exalt_conf_network_group_cypher_set(Exalt_Configuration_Network *cn, Exalt_Wireless_Network_Cypher_Name cypher_name);
/**
 * @brief Set the pairwise cypher
 * @param cn the network configuration
 * @param pairwise_cypher the new pairwise cypher
 * @note Used only if the encryption is WPA or WPA2
 */
EAPI void exalt_conf_network_pairwise_cypher_set(Exalt_Configuration_Network *cn, Exalt_Wireless_Network_Cypher_Name pairwise_cypher);
/**
 * @brief Set the authentication suites
 * @param cn the network configuration
 * @param auth_suites the new authentication suites
 * @note Used only if the encryption is WPA or WPA2
 */
EAPI void exalt_conf_network_auth_suites_set(Exalt_Configuration_Network *cn, Exalt_Wireless_Network_Auth_Suites auth_suites);








/**
 * @brief Set if the configuration should be saved after being applied
 * @param cn the network configuration
 * @param is 1 if the configuration should be saved, else 0
 * @note Saving the configuration can be used to easily retrieves it (password, encryption mode ...). Saving a wired configuration is useless that's why this field is in the wireless network configuration.
 */
EAPI void exalt_conf_network_save_when_apply_set(Exalt_Configuration_Network *cn, int is);
/**
 * @brief Set if the configuration is a favorite.
 * @param cn the network configuration
 * @param is 1 to set the configuration as favorite, else 0.
 * @note This flag can be used to auto connect a wireless interface to a network.
 */
EAPI void exalt_conf_network_favoris_set(Exalt_Configuration_Network *cn, int is);


/**
 * @brief Get the essid
 * @param cn the network configuration
 * @return Returns the essid
 */
EAPI const char* exalt_conf_network_essid_get(Exalt_Configuration_Network* cn);
/**
 * @brief Get if the configuration use a encryption
 * @param cn the network configuration
 * @return Returns 1 if the configuration use an encryption, else 0
 */
EAPI int exalt_conf_network_encryption_is(Exalt_Configuration_Network* cn);
/**
 * @brief get the mode (IBSS, INFRASTRACTURE ... )
 * @param cn the network configuration
 * @return Returns the mode
 */
EAPI Exalt_Wireless_Network_Mode exalt_conf_network_mode_get(Exalt_Configuration_Network* cn);
/**
 * @brief Get the key
 * @param cn the network configuration
 * @return Returns the key
 */
EAPI const char* exalt_conf_network_key_get(Exalt_Configuration_Network* cn);
/**
 * @brief Get the login
 * @param cn the network configuration
 * @return Returns the login
 */
EAPI const char* exalt_conf_network_login_get(Exalt_Configuration_Network* cn);


/**
 * @brief Get if the encryption used is WEP
 * @param cn the network configuration
 * @return Returns 1 if the encryption is WEP, else 0
 */
EAPI int exalt_conf_network_wep_is(Exalt_Configuration_Network* cn);
/**
 * @brief Get if the key is an hexadecimal key
 * @param cn the network configuration
 * @return Returns 1 if the key is an hexadecimal key, else 0 (ASCII key)
 * @note this field is only used in the case of a WEP encryption
 */
EAPI int exalt_conf_network_wep_hexa_is(Exalt_Configuration_Network* cn);





/**
 * @brief Get if the encryption used is WPA or WPA2
 * @param cn the network configuration
 * @return Returns 1 if the encryption used is WPA or WPA2
 */
EAPI int exalt_conf_network_wpa_is(Exalt_Configuration_Network* cn);
/**
 * @brief Get the wpa type used (WPA1 or WPA2)
 * @param cn the network configuration
 * @return Returns the wpa type
 * @note Used only if the encryption is WPA or WPA2
 */
EAPI Exalt_Wireless_Network_Wpa_Type exalt_conf_network_wpa_type_get(Exalt_Configuration_Network* cn);
/**
 * @brief Get the cypher group
 * @param cn the network configuration
 * @return Returns the cypher group
 * @note Used only if the encryption is WPA or WPA2
 */
EAPI Exalt_Wireless_Network_Cypher_Name exalt_conf_network_group_cypher_get(Exalt_Configuration_Network* cn);
/**
 * @brief Get the cypher pairwise
 * @param cn the network configuration
 * @return Returns the cypher pairwise
 * @note Used only if the encryption is WPA or WPA2
 */
EAPI Exalt_Wireless_Network_Cypher_Name exalt_conf_network_pairwise_cypher_get(Exalt_Configuration_Network* cn);

/**
 * @brief Get the authentication suites
 * @param cn the network configuration
 * @return Returns the authentication suites
 * @note Used only if the encryption is WPA or WPA2
 */
EAPI Exalt_Wireless_Network_Auth_Suites exalt_conf_network_auth_suites_get(Exalt_Configuration_Network* cn);
/**
 * @brief Get if the configuration is saved after being apply
 * @param cn the network configuration
 * @return Returns 1 if the configuration is saved, else 0
 */
EAPI int exalt_conf_network_save_when_apply_is(Exalt_Configuration_Network* cn);
/**
 * @brief Get if the configuration is a favorite
 * @param cn the network configuration
 * @return Returns 1 if the configuration is a favorite, else 0
 */
EAPI int exalt_conf_network_favoris_is(Exalt_Configuration_Network* cn);




/**
 * @brief Get an eet descriptor
 * @param cn the network configuration
 * @return Returns a new eet descriptor
 */
EAPI Eet_Data_Descriptor * exalt_conf_network_edd_new();

/** @} */

#endif   /* ----- #ifndef EXALT_CONFIGURATION_NETWORK_INC  ----- */

