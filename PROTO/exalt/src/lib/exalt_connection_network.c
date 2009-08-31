
#include "exalt_connection_network.h"

struct Exalt_Connection_Network
{
    char *essid;
    int encryption;
    Exalt_Wireless_Network_Mode mode;

    char *key;
    char *login;

    int wep;
    int wep_hexa;

    int wpa;
    Exalt_Wireless_Network_Wpa_Type wpa_type;
    Exalt_Wireless_Network_Cypher_Name group_cypher;
    Exalt_Wireless_Network_Cypher_Name pairwise_cypher;
    Exalt_Wireless_Network_Auth_Suites auth_suites;

    // if 1, the network will be saved in the configuration file
    int save_when_apply;
};

Exalt_Connection_Network *exalt_conn_network_new()
{
    return calloc(1, sizeof(Exalt_Connection_Network));
}

void exalt_conn_network_free(Exalt_Connection_Network **cn)
{
    if(!cn) return ;
    if(!(*cn)) return ;

    EXALT_FREE( (*cn)->essid );
    EXALT_FREE( (*cn)->key );
    EXALT_FREE( *cn );
}

void exalt_conn_network_print(Exalt_Connection_Network *cn)
{
    printf("essid\t\t:\t%s\n",exalt_conn_network_essid_get(cn));
    printf("encryption\t:\t%d\n",exalt_conn_network_encryption_is(cn));
    printf("mode\t\t:\t%d\n",exalt_conn_network_mode_get(cn));
    printf("key\t\t:\t%s\n",exalt_conn_network_key_get(cn));
    printf("login\t\t:\t%s\n",exalt_conn_network_login_get(cn));
    printf("WEP\t\t:\t%d\n",exalt_conn_network_wep_is(cn));
    printf("WEP Hexa\t:\t%d\n",exalt_conn_network_wep_hexa_is(cn));
    printf("WPA\t\t:\t%d\n",exalt_conn_network_wpa_is(cn));
    printf("WPA type\t:\t%d\n",exalt_conn_network_wpa_type_get(cn));
    printf("Group Cypher\t:\t%d\n",exalt_conn_network_group_cypher_get(cn));
    printf("Pairwise Cypher\t:\t%d\n",exalt_conn_network_pairwise_cypher_get(cn));
    printf("Auth Suites\t:\t%d\n",exalt_conn_network_auth_suites_get(cn));
}

#define EXALT_FCT_NAME exalt_conn_network
#define EXALT_STRUCT_TYPE Exalt_Connection_Network

EXALT_STRING_SET(essid)
EXALT_SET(encryption, int)
EXALT_SET(mode, Exalt_Wireless_Network_Mode)
EXALT_STRING_SET(key)
EXALT_STRING_SET(login)
EXALT_SET(wep, int)
EXALT_SET(wep_hexa, int)
EXALT_SET(wpa, int)
EXALT_SET(wpa_type, Exalt_Wireless_Network_Wpa_Type)
EXALT_SET(group_cypher, Exalt_Wireless_Network_Cypher_Name)
EXALT_SET(pairwise_cypher, Exalt_Wireless_Network_Cypher_Name)
EXALT_SET(auth_suites, Exalt_Wireless_Network_Auth_Suites)
EXALT_SET(save_when_apply, int)

EXALT_GET(essid, const char*)
EXALT_IS(encryption, int)
EXALT_GET(mode, Exalt_Wireless_Network_Mode)
EXALT_GET(key, const char*)
EXALT_GET(login, const char*)
EXALT_IS(wep, int)
EXALT_IS(wep_hexa, int)
EXALT_IS(wpa, int)
EXALT_GET(wpa_type, Exalt_Wireless_Network_Wpa_Type)
EXALT_GET(group_cypher, Exalt_Wireless_Network_Cypher_Name)
EXALT_GET(pairwise_cypher, Exalt_Wireless_Network_Cypher_Name)
EXALT_GET(auth_suites, Exalt_Wireless_Network_Auth_Suites)
EXALT_IS(save_when_apply, int)

#undef EXALT_FCT_NAME
#undef EXALT_STRUCT_TYPE

Eet_Data_Descriptor * exalt_conn_network_edd_new()
{
    Eet_Data_Descriptor *edd;
    edd = eet_data_descriptor_new("network", sizeof(Exalt_Connection_Network),
            (void*(*)(void*))eina_list_next,
            (void*(*)(void*,void*))eina_list_append,
            (void*(*)(void*))eina_list_data_get,
            (void*(*)(void*))eina_list_free,
            (void(*)(void*,int(*)(void*,const char*,void*,void*),void*))eina_hash_foreach,
            (void*(*)(void*,const char*,void*))eina_hash_add,
            (void(*)(void*))eina_hash_free);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection_Network, "essid", essid, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection_Network, "encryption", encryption, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection_Network, "mode", mode, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection_Network, "key", key, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection_Network, "wep", wep, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection_Network, "wep_hexa", wep_hexa, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection_Network, "wpa", wpa, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection_Network, "wpa_type", wpa_type, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection_Network, "group_cypher", group_cypher, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection_Network, "pairwise_cypher", pairwise_cypher, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Connection_Network, "auth_suites", auth_suites, EET_T_INT);

    return edd;
}


