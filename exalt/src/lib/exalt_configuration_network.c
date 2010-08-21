/** @file exalt_configuration_network.c */

#include "exalt_configuration_network.h"
#include "libexalt_private.h"

/**
 * @addtogroup Exalt_Configuration_Network
 * @{
 */


struct Exalt_Configuration_Network
{
    char *essid;
    int encryption;
    Exalt_Wireless_Network_Mode mode;

    char *key;

    int wep;
    int wep_hexa;

    int wpa;
    Exalt_Wireless_Network_Wpa_Type wpa_type;
    Exalt_Wireless_Network_Cypher_Name group_cypher;
    Exalt_Wireless_Network_Cypher_Name pairwise_cypher;
    Exalt_Wireless_Network_Auth_Suites auth_suites;

    Exalt_Wireless_Network_Eap eap;
    char *ca_cert;
    char *client_cert;
    char *private_key;

    int favoris;

    // if 1, the network will be saved in the configuration file
    int save_when_apply;
};

Exalt_Configuration_Network *exalt_conf_network_new()
{
    return calloc(1, sizeof(Exalt_Configuration_Network));
}

void exalt_conf_network_free(Exalt_Configuration_Network **cn)
{
    if(!cn) return ;
    if(!(*cn)) return ;

    eina_stringshare_del( (*cn)->essid );
    eina_stringshare_del( (*cn)->key );
    EXALT_FREE( *cn );
}

void exalt_conf_network_print(Exalt_Configuration_Network *cn)
{
    printf("essid\t\t:\t%s\n",exalt_conf_network_essid_get(cn));
    printf("encryption\t:\t%d\n",exalt_conf_network_encryption_is(cn));
    printf("mode\t\t:\t%d\n",exalt_conf_network_mode_get(cn));
    printf("key\t\t:\t%s\n",exalt_conf_network_key_get(cn));
    printf("WEP\t\t:\t%d\n",exalt_conf_network_wep_is(cn));
    printf("WEP Hexa\t:\t%d\n",exalt_conf_network_wep_hexa_is(cn));
    printf("WPA\t\t:\t%d\n",exalt_conf_network_wpa_is(cn));
    printf("WPA type\t:\t%d\n",exalt_conf_network_wpa_type_get(cn));
    printf("Group Cypher\t:\t%d\n",exalt_conf_network_group_cypher_get(cn));
    printf("Pairwise Cypher\t:\t%d\n",exalt_conf_network_pairwise_cypher_get(cn));
    printf("Auth Suites\t:\t%d\n",exalt_conf_network_auth_suites_get(cn));
    printf("EAP\t\t:\t%d\n",exalt_conf_network_eap_get(cn));
    printf("CA certificate\t:\t%s\n",exalt_conf_network_ca_cert_get(cn));
    printf("Client certificate:\t%s\n",exalt_conf_network_client_cert_get(cn));
    printf("Private key\t:\t%s\n",exalt_conf_network_private_key_get(cn));
    printf("Favoris\t\t:\t%d\n",exalt_conf_network_favoris_is(cn));
}
/// @cond
#define EXALT_FCT_NAME exalt_conf_network
#define EXALT_STRUCT_TYPE Exalt_Configuration_Network
/// @endcond

EXALT_STRING_SET(essid)
EXALT_SET(encryption, int)
EXALT_SET(mode, Exalt_Wireless_Network_Mode)
EXALT_STRING_SET(key)
EXALT_SET(wep, int)
EXALT_SET(wep_hexa, int)
EXALT_SET(wpa, int)
EXALT_SET(wpa_type, Exalt_Wireless_Network_Wpa_Type)
EXALT_SET(group_cypher, Exalt_Wireless_Network_Cypher_Name)
EXALT_SET(pairwise_cypher, Exalt_Wireless_Network_Cypher_Name)
EXALT_SET(auth_suites, Exalt_Wireless_Network_Auth_Suites)
EXALT_SET(eap, Exalt_Wireless_Network_Eap)
EXALT_STRING_SET(ca_cert)
EXALT_STRING_SET(client_cert)
EXALT_STRING_SET(private_key)
EXALT_SET(save_when_apply, int)
EXALT_SET(favoris, int)

EXALT_GET(essid, const char*)
EXALT_IS(encryption, int)
EXALT_GET(mode, Exalt_Wireless_Network_Mode)
EXALT_GET(key, const char*)
EXALT_IS(wep, int)
EXALT_IS(wep_hexa, int)
EXALT_IS(wpa, int)
EXALT_GET(wpa_type, Exalt_Wireless_Network_Wpa_Type)
EXALT_GET(group_cypher, Exalt_Wireless_Network_Cypher_Name)
EXALT_GET(pairwise_cypher, Exalt_Wireless_Network_Cypher_Name)
EXALT_GET(auth_suites, Exalt_Wireless_Network_Auth_Suites)
EXALT_GET(eap, Exalt_Wireless_Network_Eap)
EXALT_GET(ca_cert, const char*)
EXALT_GET(client_cert, const char*)
EXALT_GET(private_key, const char*)
EXALT_IS(save_when_apply, int)
EXALT_IS(favoris, int)

#undef EXALT_FCT_NAME
#undef EXALT_STRUCT_TYPE

Eet_Data_Descriptor * exalt_conf_network_edd_new()
{
    static Eet_Data_Descriptor_Class eddc;
    Eet_Data_Descriptor *edd;

    eet_eina_stream_data_descriptor_class_set(&eddc, sizeof (eddc), "Network", sizeof(Exalt_Configuration_Network));
    edd = eet_data_descriptor_stream_new(&eddc);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration_Network, "essid", essid, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration_Network, "encryption", encryption, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration_Network, "mode", mode, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration_Network, "key", key, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration_Network, "wep", wep, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration_Network, "wep_hexa", wep_hexa, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration_Network, "wpa", wpa, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration_Network, "wpa_type", wpa_type, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration_Network, "group_cypher", group_cypher, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration_Network, "pairwise_cypher", pairwise_cypher, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration_Network, "auth_suites", auth_suites, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration_Network, "eap", eap, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration_Network, "ca_cert", ca_cert, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration_Network, "client_cert", client_cert, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration_Network, "private_key", private_key, EET_T_STRING);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration_Network, "favoris", favoris, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration_Network, "save_when_apply", save_when_apply, EET_T_INT);

    return edd;
}

/** @} */
