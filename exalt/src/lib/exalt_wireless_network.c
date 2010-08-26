/** @file exalt_wireless_network.c */
#include "exalt_wireless_network.h"
#include "libexalt_private.h"


/*
 * TODO: update exalt_wireless_network_free()
 * */

typedef struct Exalt_Wireless_Network_Mode_List Exalt_Wireless_Network_Mode_List;
typedef struct Exalt_Wireless_Network_Security_List Exalt_Wireless_Network_Security_List;
typedef struct Exalt_Wireless_Network_Auth_Suites_List Exalt_Wireless_Network_Auth_Suites_List;
typedef struct Exalt_Wireless_Network_Wpa_Type_List Exalt_Wireless_Network_Wpa_Type_List;
typedef struct Exalt_Wireless_Network_Cypher_Name_List Exalt_Wireless_Network_Cypher_Name_List;
typedef struct Exalt_Wireless_Network_Eap_List Exalt_Wireless_Network_Eap_List;

struct Exalt_Wireless_Network
{
    Exalt_Wireless* iface;

    char* address;
    char* essid;
    int encryption;
    char* description;
    int quality;

    Exalt_Wireless_Network_Mode mode;

    Eina_List* ie;
};

struct Exalt_Wireless_Network_IE
{
    char* description;

    Exalt_Wireless_Network_Wpa_Type wpa_type;

    Exalt_Wireless_Network_Cypher_Name group_cypher;

    Exalt_Wireless_Network_Cypher_Name
        pairwise_cypher[EXALT_WIRELESS_NETWORK_CYPHER_NAME_NUM];
    int pairwise_cypher_number;

    Exalt_Wireless_Network_Auth_Suites
        auth_suites[EXALT_WIRELESS_NETWORK_AUTH_SUITES_NUM];
    int auth_suites_number;

    Exalt_Wireless_Network_Eap eap;
};

struct Exalt_Wireless_Network_Mode_List
{
    int id;
    char* name;
    Exalt_Wireless_Network_Mode mode;
};

struct Exalt_Wireless_Network_Security_List
{
    Exalt_Wireless_Network_Security security;
    char* name;
};

struct Exalt_Wireless_Network_Auth_Suites_List
{
    Exalt_Wireless_Network_Auth_Suites auth_suites;
    char* name;
};

struct Exalt_Wireless_Network_Wpa_Type_List
{
    Exalt_Wireless_Network_Wpa_Type wpa_type;
    char* name;
};

struct Exalt_Wireless_Network_Cypher_Name_List
{
    Exalt_Wireless_Network_Cypher_Name cypher_name;
    char* name;
};

struct Exalt_Wireless_Network_Eap_List
{
    Exalt_Wireless_Network_Eap eap;
    char* name;
};

Exalt_Wireless_Network_Mode_List exalt_wireless_network_mode_tab[] =
{
    {0,"Infrastructure",MODE_INFRASTRUCTURE},
    {1,"IBSS",MODE_IBSS},
    {2,"AP",MODE_AP},
    {3, "Unknown", MODE_UNKNOWN}
};

Exalt_Wireless_Network_Security_List exalt_wireless_network_security_tab[]=
{
    {SECURITY_NONE,"none"},
    {SECURITY_RESTRICTED,"restricted"},
    {SECURITY_RESTRICTED,"open"}
};

Exalt_Wireless_Network_Auth_Suites_List exalt_wireless_network_auth_suites_tab[]=
{
    {AUTH_SUITES_NONE,"none"},
    {AUTH_SUITES_EAP,"EAP"},
    {AUTH_SUITES_PSK,"PSK"},
    {AUTH_SUITES_UNKNOWN,"unknown"}
};

Exalt_Wireless_Network_Cypher_Name_List exalt_wireless_network_cypher_name_tab[]=
{
    {CYPHER_NAME_NONE,"none"},
    {CYPHER_NAME_TKIP,"TKIP"},
    {CYPHER_NAME_CCMP,"CCMP"},
    {CYPHER_NAME_UNKNOWN,"unknown"}
};

Exalt_Wireless_Network_Wpa_Type_List exalt_wireless_network_wpa_type_tab[]=
{
    {WPA_TYPE_UNKNOWN,"unknown"},
    {WPA_TYPE_WPA,"WPA"},
    {WPA_TYPE_WPA2,"WPA2"},
};

Exalt_Wireless_Network_Eap_List exalt_wireless_network_eap_tab[]=
{
    {EAP_UNKNOWN,"unknown"},
    {EAP_TLS,"TLS"}
};

/**
 * @addtogroup Exalt_Wireless_Network
 * @{
 */


Eet_Data_Descriptor * exalt_wireless_network_ie_edd_new()
{
    static Eet_Data_Descriptor_Class eddc;
    Eet_Data_Descriptor *edd;
    Exalt_Wireless_Network_IE ie;

    eet_eina_stream_data_descriptor_class_set(&eddc, sizeof (eddc), "ie", sizeof(Exalt_Wireless_Network_IE));
    edd = eet_data_descriptor_stream_new(&eddc);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Wireless_Network_IE, "description", description, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Wireless_Network_IE, "wpa_type", wpa_type, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Wireless_Network_IE, "group_cypher", group_cypher, EET_T_INT);

    eet_data_descriptor_element_add(edd,"pairwise_cypher",EET_T_INT,
            EET_G_ARRAY, (char *)(&(ie.pairwise_cypher)) - (char *)(&(ie)),
            sizeof(ie.pairwise_cypher)/sizeof(ie.pairwise_cypher[0]),
            NULL, NULL);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Wireless_Network_IE, "pairwise_cypher_number", pairwise_cypher_number, EET_T_INT);

    eet_data_descriptor_element_add(edd,"auth_suites",EET_T_INT,
            EET_G_ARRAY, (char *)(&(ie.auth_suites)) - (char *)(&(ie)),
            sizeof(ie.auth_suites)/sizeof(ie.auth_suites[0]),
            NULL, NULL);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Wireless_Network_IE, "auth_suites_number", auth_suites_number, EET_T_INT);

    return edd;
}

Eet_Data_Descriptor * exalt_wireless_network_edd_new(Eet_Data_Descriptor* edd_ie)
{
    static Eet_Data_Descriptor_Class eddc;
    Eet_Data_Descriptor *edd;

    EXALT_ASSERT_RETURN(edd_ie!=NULL);

    eet_eina_stream_data_descriptor_class_set(&eddc, sizeof (eddc), "Network", sizeof(Exalt_Wireless_Network));
    edd = eet_data_descriptor_stream_new(&eddc);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Wireless_Network, "address", address, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Wireless_Network, "essid", essid, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Wireless_Network, "encryption", encryption, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Wireless_Network, "mode", mode, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Wireless_Network, "description", description, EET_T_STRING);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Wireless_Network, "quality", quality, EET_T_INT);

    EET_DATA_DESCRIPTOR_ADD_LIST(edd, Exalt_Wireless_Network, "ies", ie, edd_ie);
    return edd;
}


Exalt_Wireless_Network* exalt_wireless_network_new(
        Exalt_Wireless* w)
{
    Exalt_Wireless_Network* wn = calloc(1,sizeof(Exalt_Wireless_Network));
    EXALT_ASSERT_RETURN(wn!=NULL);
    wn->iface = w;
    wn->ie = NULL;
    return wn;
}

void exalt_wireless_network_free(
        Exalt_Wireless_Network** wn)
{
    Exalt_Wireless_Network* wn2;
    Exalt_Wireless_Network_IE *ie;
    Eina_List *l,*l_next;

    EXALT_ASSERT_RETURN_VOID(wn!=NULL);
    wn2 = *wn;
    EXALT_ASSERT_RETURN_VOID(wn2!=NULL);
    EXALT_FREE(wn2->address);
    EXALT_FREE(wn2->essid);
    EXALT_FREE(wn2->description);

    EINA_LIST_FOREACH_SAFE(wn2->ie,l,l_next,ie);
    {
        if(ie)
            exalt_wireless_network_ie_free(&ie);
        wn2->ie = eina_list_remove_list(wn2->ie, l);
    }

    EXALT_FREE(wn2);
}

Exalt_Wireless_Network_IE* exalt_wireless_network_ie_new()
{
    Exalt_Wireless_Network_IE* ie = calloc(1,sizeof(Exalt_Wireless_Network_IE));
    EXALT_ASSERT_RETURN(ie!=NULL);
    return ie;
}

void exalt_wireless_network_ie_free(
        Exalt_Wireless_Network_IE** ie)
{
    EXALT_ASSERT_RETURN_VOID(ie!=NULL);
    EXALT_FREE((*ie)->description);
    EXALT_FREE(*ie);
}
/// @cond
#define EXALT_FCT_NAME exalt_wireless_network
#define EXALT_STRUCT_TYPE Exalt_Wireless_Network

EXALT_STRING_SET(essid)
EXALT_STRING_SET(address);
EXALT_SET(iface,Exalt_Wireless*)
EXALT_SET(encryption,int)
EXALT_STRING_SET(description)
EXALT_SET(quality,int)
EXALT_SET(ie,Eina_List*)
EXALT_SET(mode,Exalt_Wireless_Network_Mode)

EXALT_GET(iface,Exalt_Wireless*)
EXALT_GET(address,const char*)
EXALT_GET(essid,const char*)
EXALT_IS(encryption,int)
EXALT_GET(description,const char*)
EXALT_GET(quality,int)
EXALT_GET(ie,Eina_List*)
EXALT_GET(mode,Exalt_Wireless_Network_Mode)

#undef EXALT_FCT_NAME
#undef EXALT_STRUCT_TYPE

#define EXALT_FCT_NAME exalt_wireless_network_ie
#define EXALT_STRUCT_TYPE Exalt_Wireless_Network_IE

EXALT_STRING_SET(description)
EXALT_SET(wpa_type,Exalt_Wireless_Network_Wpa_Type)
EXALT_SET(group_cypher,Exalt_Wireless_Network_Cypher_Name)
EXALT_TAB_SET(pairwise_cypher,Exalt_Wireless_Network_Cypher_Name)
EXALT_SET(pairwise_cypher_number,int)
EXALT_TAB_SET(auth_suites,Exalt_Wireless_Network_Auth_Suites)
EXALT_SET(auth_suites_number,int)
EXALT_SET(eap, Exalt_Wireless_Network_Eap)

EXALT_GET(description,const char*)
EXALT_GET(wpa_type,Exalt_Wireless_Network_Wpa_Type)
EXALT_GET(group_cypher,Exalt_Wireless_Network_Cypher_Name)
EXALT_TAB_GET(pairwise_cypher,Exalt_Wireless_Network_Cypher_Name)
EXALT_GET(pairwise_cypher_number,int)
EXALT_TAB_GET(auth_suites,Exalt_Wireless_Network_Auth_Suites)
EXALT_GET(auth_suites_number,int)
EXALT_GET(eap,Exalt_Wireless_Network_Eap)

#undef EXALT_FCT_NAME
#undef EXALT_STRUCT_TYPE
/// @endcond

const char* exalt_wireless_network_name_from_mode_id(
        int id)
{
    unsigned int i;
    for(i = 0; i < sizeof(exalt_wireless_network_mode_tab) / sizeof(Exalt_Wireless_Network_Mode_List);i++)
        if(exalt_wireless_network_mode_tab[i].id == id)
            return exalt_wireless_network_mode_tab[i].name;

    return exalt_wireless_network_name_from_mode(MODE_UNKNOWN);
}

Exalt_Wireless_Network_Mode exalt_wireless_network_mode_from_mode_id(
        int id)
{
    unsigned int i;
    for(i = 0; i < sizeof(exalt_wireless_network_mode_tab) / sizeof(Exalt_Wireless_Network_Mode_List);i++)
        if(exalt_wireless_network_mode_tab[i].id == id)
            return exalt_wireless_network_mode_tab[i].mode;

    return MODE_UNKNOWN;
}

const char* exalt_wireless_network_name_from_mode(
        Exalt_Wireless_Network_Mode mode)
{
    unsigned int i;
    for(i = 0; i < sizeof(exalt_wireless_network_mode_tab) / sizeof(Exalt_Wireless_Network_Mode_List);i++)
        if(exalt_wireless_network_mode_tab[i].mode == mode)
            return exalt_wireless_network_mode_tab[i].name;

    return exalt_wireless_network_name_from_mode(MODE_UNKNOWN);
}


const char* exalt_wireless_network_name_from_wpa_type(
        Exalt_Wireless_Network_Wpa_Type wpa_type)
{
    unsigned int i;
    for(i = 0; i < sizeof(exalt_wireless_network_wpa_type_tab) / sizeof(Exalt_Wireless_Network_Wpa_Type_List);i++)
        if(exalt_wireless_network_wpa_type_tab[i].wpa_type == wpa_type)
            return exalt_wireless_network_wpa_type_tab[i].name;

    return exalt_wireless_network_name_from_wpa_type(WPA_TYPE_UNKNOWN);
}

const char* exalt_wireless_network_name_from_cypher_name(
        Exalt_Wireless_Network_Cypher_Name cypher_name)
{
    unsigned int i;
    for(i = 0; i < sizeof(exalt_wireless_network_cypher_name_tab) / sizeof(Exalt_Wireless_Network_Cypher_Name_List);i++)
        if(exalt_wireless_network_cypher_name_tab[i].cypher_name == cypher_name)
            return exalt_wireless_network_cypher_name_tab[i].name;

    return exalt_wireless_network_name_from_cypher_name(CYPHER_NAME_UNKNOWN);
}

const char* exalt_wireless_network_name_from_auth_suites(
        Exalt_Wireless_Network_Auth_Suites auth_suites)
{
    unsigned int i;
    for(i = 0; i < sizeof(exalt_wireless_network_auth_suites_tab) / sizeof(Exalt_Wireless_Network_Auth_Suites_List);i++)
        if(exalt_wireless_network_auth_suites_tab[i].auth_suites == auth_suites)
            return exalt_wireless_network_auth_suites_tab[i].name;

    return exalt_wireless_network_name_from_auth_suites(AUTH_SUITES_UNKNOWN);
}


const char* exalt_wireless_network_name_from_security(
        Exalt_Wireless_Network_Security security)
{
    unsigned int i;
    for(i = 0; i < sizeof(exalt_wireless_network_security_tab) / sizeof(Exalt_Wireless_Network_Security_List);i++)
        if(exalt_wireless_network_security_tab[i].security == security)
            return exalt_wireless_network_security_tab[i].name;

    return exalt_wireless_network_name_from_security(SECURITY_NONE);
}

const char* exalt_wireless_network_name_from_eap(
        Exalt_Wireless_Network_Eap eap)
{
    unsigned int i;
    for(i = 0; i < sizeof(exalt_wireless_network_eap_tab) / sizeof(Exalt_Wireless_Network_Eap_List);i++)
        if(exalt_wireless_network_eap_tab[i].eap == eap)
            return exalt_wireless_network_eap_tab[i].name;

    return exalt_wireless_network_name_from_eap(EAP_UNKNOWN);
}


/** @} */

