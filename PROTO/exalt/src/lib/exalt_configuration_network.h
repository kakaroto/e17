
#ifndef  EXALT_CONFIGURATION_NETWORK_INC
#define  EXALT_CONFIGURATION_NETWORK_INC

typedef struct Exalt_Configuration_Network Exalt_Configuration_Network;

#include "libexalt.h"

EAPI Exalt_Configuration_Network *exalt_conf_network_new();
EAPI void exalt_conf_network_free(Exalt_Configuration_Network **cn);
EAPI void exalt_conf_network_print(Exalt_Configuration_Network *cn);

EAPI void exalt_conf_network_essid_set(Exalt_Configuration_Network *cn, const char* essid);
EAPI void exalt_conf_network_encryption_set(Exalt_Configuration_Network *cn, int is);
EAPI void exalt_conf_network_mode_set(Exalt_Configuration_Network *cn, Exalt_Wireless_Network_Mode mode);
EAPI void exalt_conf_network_key_set(Exalt_Configuration_Network *cn, const char*);
EAPI void exalt_conf_network_login_set(Exalt_Configuration_Network *cn, const char*);

EAPI void exalt_conf_network_wep_set(Exalt_Configuration_Network *cn, int is);
EAPI void exalt_conf_network_wep_hexa_set(Exalt_Configuration_Network *cn, int is);

EAPI void exalt_conf_network_wpa_set(Exalt_Configuration_Network *cn, int is);
EAPI void exalt_conf_network_wpa_type_set(Exalt_Configuration_Network *cn, Exalt_Wireless_Network_Wpa_Type wpa_type);
EAPI void exalt_conf_network_group_cypher_set(Exalt_Configuration_Network *cn, Exalt_Wireless_Network_Cypher_Name cypher_name);
EAPI void exalt_conf_network_pairwise_cypher_set(Exalt_Configuration_Network *cn, Exalt_Wireless_Network_Cypher_Name pairwise_cypher);
EAPI void exalt_conf_network_auth_suites_set(Exalt_Configuration_Network *cn, Exalt_Wireless_Network_Auth_Suites auth_suites);
EAPI void exalt_conf_network_save_when_apply_set(Exalt_Configuration_Network *cn, int is);
EAPI void exalt_conf_network_favoris_set(Exalt_Configuration_Network *cn, int is);

EAPI const char* exalt_conf_network_essid_get(Exalt_Configuration_Network* nc);
EAPI int exalt_conf_network_encryption_is(Exalt_Configuration_Network* nc);
EAPI Exalt_Wireless_Network_Mode exalt_conf_network_mode_get(Exalt_Configuration_Network* nc);
EAPI const char* exalt_conf_network_key_get(Exalt_Configuration_Network* nc);
EAPI const char* exalt_conf_network_login_get(Exalt_Configuration_Network* nc);
EAPI int exalt_conf_network_wep_is(Exalt_Configuration_Network* nc);
EAPI int exalt_conf_network_wep_hexa_is(Exalt_Configuration_Network* nc);
EAPI int exalt_conf_network_wpa_is(Exalt_Configuration_Network* nc);
EAPI Exalt_Wireless_Network_Wpa_Type exalt_conf_network_wpa_type_get(Exalt_Configuration_Network* nc);
EAPI Exalt_Wireless_Network_Cypher_Name exalt_conf_network_group_cypher_get(Exalt_Configuration_Network* nc);
EAPI Exalt_Wireless_Network_Cypher_Name exalt_conf_network_pairwise_cypher_get(Exalt_Configuration_Network* nc);
EAPI Exalt_Wireless_Network_Auth_Suites exalt_conf_network_auth_suites_get(Exalt_Configuration_Network* nc);
EAPI int exalt_conf_network_save_when_apply_is(Exalt_Configuration_Network* nc);
EAPI int exalt_conf_network_favoris_is(Exalt_Configuration_Network* nc);

EAPI Eet_Data_Descriptor * exalt_conf_network_edd_new();

#endif   /* ----- #ifndef EXALT_CONFIGURATION_NETWORK_INC  ----- */

