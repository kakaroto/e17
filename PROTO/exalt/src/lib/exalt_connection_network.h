
#ifndef  EXALT_CONNECTION_NETWORK_INC
#define  EXALT_CONNECTION_NETWORK_INC

typedef struct Exalt_Connection_Network Exalt_Connection_Network;

#include "libexalt.h"

Exalt_Connection_Network *exalt_conn_network_new();
void exalt_conn_network_free(Exalt_Connection_Network **cn);
void exalt_conn_network_print(Exalt_Connection_Network *cn);

void exalt_conn_network_essid_set(Exalt_Connection_Network *cn, const char* essid);
void exalt_conn_network_encryption_set(Exalt_Connection_Network *cn, int is);
void exalt_conn_network_mode_set(Exalt_Connection_Network *cn, Exalt_Wireless_Network_Mode mode);
void exalt_conn_network_key_set(Exalt_Connection_Network *cn, const char*);
void exalt_conn_network_login_set(Exalt_Connection_Network *cn, const char*);

void exalt_conn_network_wep_set(Exalt_Connection_Network *cn, int is);
void exalt_conn_network_wep_hexa_set(Exalt_Connection_Network *cn, int is);

void exalt_conn_network_wpa_set(Exalt_Connection_Network *cn, int is);
void exalt_conn_network_wpa_type_set(Exalt_Connection_Network *cn, Exalt_Wireless_Network_Wpa_Type wpa_type);
void exalt_conn_network_group_cypher_set(Exalt_Connection_Network *cn, Exalt_Wireless_Network_Cypher_Name cypher_name);
void exalt_conn_network_pairwise_cypher_set(Exalt_Connection_Network *cn, Exalt_Wireless_Network_Cypher_Name pairwise_cypher);
void exalt_conn_network_auth_suites_set(Exalt_Connection_Network *cn, Exalt_Wireless_Network_Auth_Suites auth_suites);
void exalt_conn_network_save_when_apply_set(Exalt_Connection_Network *cn, int is);

const char* exalt_conn_network_essid_get(Exalt_Connection_Network* nc);
int exalt_conn_network_encryption_is(Exalt_Connection_Network* nc);
Exalt_Wireless_Network_Mode exalt_conn_network_mode_get(Exalt_Connection_Network* nc);
const char* exalt_conn_network_key_get(Exalt_Connection_Network* nc);
const char* exalt_conn_network_login_get(Exalt_Connection_Network* nc);
int exalt_conn_network_wep_is(Exalt_Connection_Network* nc);
int exalt_conn_network_wep_hexa_is(Exalt_Connection_Network* nc);
int exalt_conn_network_wpa_is(Exalt_Connection_Network* nc);
Exalt_Wireless_Network_Wpa_Type exalt_conn_network_wpa_type_get(Exalt_Connection_Network* nc);
Exalt_Wireless_Network_Cypher_Name exalt_conn_network_group_cypher_get(Exalt_Connection_Network* nc);
Exalt_Wireless_Network_Cypher_Name exalt_conn_network_pairwise_cypher_get(Exalt_Connection_Network* nc);
Exalt_Wireless_Network_Auth_Suites exalt_conn_network_auth_suites_get(Exalt_Connection_Network* nc);
int exalt_conn_network_save_when_apply_is(Exalt_Connection_Network* nc);

Eet_Data_Descriptor * exalt_conn_network_edd_new();

#endif   /* ----- #ifndef EXALT_CONNECTION_NETWORK_INC  ----- */

