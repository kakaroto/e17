/** @file exalt_wireless_network.c */
#include "exalt_wireless_network.h"
#include "libexalt_private.h"

/*
 * TODO: update exalt_wireless_network_free()
 * */

/**
 * @addtogroup Exalt_Wireless_Network
 * @{
 */

typedef struct Exalt_Wireless_Network_Mode_List Exalt_Wireless_Network_Mode_List;
typedef struct Exalt_Wireless_Network_Security_List Exalt_Wireless_Network_Security_List;
typedef struct Exalt_Wireless_Network_Auth_Suites_List Exalt_Wireless_Network_Auth_Suites_List;
typedef struct Exalt_Wireless_Network_Wpa_Type_List Exalt_Wireless_Network_Wpa_Type_List;
typedef struct Exalt_Wireless_Network_Cypher_Name_List Exalt_Wireless_Network_Cypher_Name_List;


struct Exalt_Wireless_Network
{
    Exalt_Wireless* iface;

    char* address;
    char* essid;
    int encryption;
    Exalt_Wireless_Network_Security security_mode;
    int quality;
    Exalt_Wireless_Network_Mode mode;

    Eina_List* ie;

    Exalt_Connection* default_conn;
};

struct Exalt_Wireless_Network_IE
{
    Exalt_Wireless_Network_Wpa_Type wpa_type;
    int wpa_version;

    Exalt_Wireless_Network_Cypher_Name group_cypher;

    Exalt_Wireless_Network_Cypher_Name
        pairwise_cypher[EXALT_WIRELESS_NETWORK_CYPHER_NAME_NUM];
    int pairwise_cypher_number;

    Exalt_Wireless_Network_Auth_Suites
        auth_suites[EXALT_WIRELESS_NETWORK_AUTH_SUITES_NUM];
    int auth_suites_number;

    int preauth_supported;
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

Exalt_Wireless_Network_Mode_List exalt_wireless_network_mode_tab[] =
{
    {0,"Auto",MODE_AUTO},
    {1,"Ad-Hoc",MODE_AD_HOC},
    {2,"Managed",MODE_MANAGED},
    {3,"Master",MODE_MASTER},
    {4,"Repeater",MODE_REPEATER},
    {5,"Secondary",MODE_SECONDARY},
    {6,"Monitor",MODE_MONITOR},
    {7,"Unknow/bug",MODE_UNKNOW_BUG}
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
    {AUTH_SUITES_8021X,"8021.x"},
    {AUTH_SUITES_PSK,"PSK"},
    {AUTH_SUITES_PROPRIETARY,"proprietary"},
    {AUTH_SUITES_UNKNOWN,"unknown"}
};

Exalt_Wireless_Network_Cypher_Name_List exalt_wireless_network_cypher_name_tab[]=
{
    {CYPHER_NAME_NONE,"none"},
    {CYPHER_NAME_WEP40,"WEP40"},
    {CYPHER_NAME_TKIP,"TKIP"},
    {CYPHER_NAME_WRAP,"WRAP"},
    {CYPHER_NAME_CCMP,"CCMP"},
    {CYPHER_NAME_WEP104,"WEP104"},
    {CYPHER_NAME_UNKNOWN,"unknown"},
    {CYPHER_NAME_PROPRIETARY,"proprietary"}
};

Exalt_Wireless_Network_Wpa_Type_List exalt_wireless_network_wpa_type_tab[]=
{
    {WPA_TYPE_UNKNOWN,"unknown"},
    {WPA_TYPE_WPA,"WPA"},
    {WPA_TYPE_WPA2,"WPA2"}
};



/**
 * @brief Create a new exalt_Wireless_Network structure
 * @param w the interface which be associated to the new network
 * @return Returns the new network
 */
Exalt_Wireless_Network* exalt_wireless_network_new(
        Exalt_Wireless* w)
{
    Exalt_Wireless_Network* wn = calloc(1,sizeof(Exalt_Wireless_Network));
    EXALT_ASSERT_RETURN(wn!=NULL);
    wn->iface = w;
    wn->security_mode = SECURITY_NONE;
    wn->ie = NULL;
    return wn;
}

/**
 * @brief free Exalt_Wireless_Network
 * @param data the Exalt_Wireless_Network
 */
void exalt_wireless_network_free(
        Exalt_Wireless_Network** wn)
{
    Exalt_Wireless_Network* wn2;
    EXALT_ASSERT_RETURN_VOID(wn!=NULL);
    wn2 = *wn;
    EXALT_ASSERT_RETURN_VOID(wn2!=NULL);
    EXALT_FREE(wn2->address);
    EXALT_FREE(wn2->essid);
    //eina_list_destroy(wi->ie);
}

/**
 * @brief Create a new exalt_Wireless_Network_IE structure
 * @return Returns the new network IE structure
 */
Exalt_Wireless_Network_IE* exalt_wireless_network_ie_new()
{
    Exalt_Wireless_Network_IE* ie = calloc(1,sizeof(Exalt_Wireless_Network_IE));
    EXALT_ASSERT_RETURN(ie!=NULL);
    return ie;
}

/**
 * @brief free Exalt_Wireless_Network_IE
 * @param data the Exalt_Wireless_Network_IE
 */
void exalt_wireless_network_ie_free(
        Exalt_Wireless_Network_IE** ie)
{
    EXALT_ASSERT_RETURN_VOID(ie!=NULL);
    EXALT_FREE(*ie);
}

#define EXALT_FCT_NAME exalt_wireless_network
#define EXALT_STRUCT_TYPE Exalt_Wireless_Network

EXALT_STRING_SET(essid)
EXALT_STRING_SET(address);
EXALT_SET(iface,Exalt_Wireless*)
EXALT_SET(encryption,int)
EXALT_SET(quality,int)
EXALT_SET(mode,Exalt_Wireless_Network_Mode)
EXALT_SET(security_mode,Exalt_Wireless_Network_Security)
EXALT_SET(ie,Eina_List*)
EXALT_SET(default_conn,Exalt_Connection*)

EXALT_GET(iface,Exalt_Wireless*)
EXALT_GET(address,const char*)
EXALT_GET(essid,const char*)
EXALT_IS(encryption,int)
EXALT_GET(quality,int)
EXALT_GET(mode,Exalt_Wireless_Network_Mode)
EXALT_GET(security_mode,Exalt_Wireless_Network_Security)
EXALT_GET(ie,Eina_List*)
EXALT_GET(default_conn,Exalt_Connection*)

#undef EXALT_FCT_NAME
#undef EXALT_STRUCT_TYPE

#define EXALT_FCT_NAME exalt_wireless_network_ie
#define EXALT_STRUCT_TYPE Exalt_Wireless_Network_IE

EXALT_SET(wpa_type,Exalt_Wireless_Network_Wpa_Type)
EXALT_SET(wpa_version,int)
EXALT_SET(group_cypher,Exalt_Wireless_Network_Cypher_Name)
EXALT_TAB_SET(pairwise_cypher,Exalt_Wireless_Network_Cypher_Name)
EXALT_SET(pairwise_cypher_number,int)
EXALT_TAB_SET(auth_suites,Exalt_Wireless_Network_Auth_Suites)
EXALT_SET(auth_suites_number,int)
EXALT_SET(preauth_supported,int)

EXALT_GET(wpa_type,Exalt_Wireless_Network_Wpa_Type)
EXALT_GET(wpa_version,int)
EXALT_GET(group_cypher,Exalt_Wireless_Network_Cypher_Name)
EXALT_TAB_GET(pairwise_cypher,Exalt_Wireless_Network_Cypher_Name)
EXALT_GET(pairwise_cypher_number,int)
EXALT_TAB_GET(auth_suites,Exalt_Wireless_Network_Auth_Suites)
EXALT_GET(auth_suites_number,int)
EXALT_IS(preauth_supported,int)

#undef EXALT_FCT_NAME
#undef EXALT_STRUCT_TYPE


const char* exalt_wireless_network_name_from_mode_id(
        int id)
{
    int i;
    for(i = 0; i < sizeof(exalt_wireless_network_mode_tab) / sizeof(Exalt_Wireless_Network_Mode_List);i++)
        if(exalt_wireless_network_mode_tab[i].id == id)
            return exalt_wireless_network_mode_tab[i].name;

    return exalt_wireless_network_name_from_mode(MODE_UNKNOW_BUG);
}

Exalt_Wireless_Network_Mode exalt_wireless_network_mode_from_mode_id(
        int id)
{
    int i;
    for(i = 0; i < sizeof(exalt_wireless_network_mode_tab) / sizeof(Exalt_Wireless_Network_Mode_List);i++)
        if(exalt_wireless_network_mode_tab[i].id == id)
            return exalt_wireless_network_mode_tab[i].mode;

    return MODE_UNKNOW_BUG;
}

const char* exalt_wireless_network_name_from_mode(
        Exalt_Wireless_Network_Mode mode)
{
    int i;
    for(i = 0; i < sizeof(exalt_wireless_network_mode_tab) / sizeof(Exalt_Wireless_Network_Mode_List);i++)
        if(exalt_wireless_network_mode_tab[i].mode == mode)
            return exalt_wireless_network_mode_tab[i].name;

    return exalt_wireless_network_name_from_mode(MODE_UNKNOW_BUG);
}


const char* exalt_wireless_network_name_from_wpa_type(
        Exalt_Wireless_Network_Wpa_Type wpa_type)
{
    int i;
    for(i = 0; i < sizeof(exalt_wireless_network_wpa_type_tab) / sizeof(Exalt_Wireless_Network_Wpa_Type_List);i++)
        if(exalt_wireless_network_wpa_type_tab[i].wpa_type == wpa_type)
            return exalt_wireless_network_wpa_type_tab[i].name;

    return exalt_wireless_network_name_from_wpa_type(WPA_TYPE_UNKNOWN);
}

const char* exalt_wireless_network_name_from_cypher_name(
        Exalt_Wireless_Network_Cypher_Name cypher_name)
{
    int i;
    for(i = 0; i < sizeof(exalt_wireless_network_cypher_name_tab) / sizeof(Exalt_Wireless_Network_Cypher_Name_List);i++)
        if(exalt_wireless_network_cypher_name_tab[i].cypher_name == cypher_name)
            return exalt_wireless_network_cypher_name_tab[i].name;

    return exalt_wireless_network_name_from_cypher_name(CYPHER_NAME_UNKNOWN);
}

const char* exalt_wireless_network_name_from_auth_suites(
        Exalt_Wireless_Network_Auth_Suites auth_suites)
{
    int i;
    for(i = 0; i < sizeof(exalt_wireless_network_auth_suites_tab) / sizeof(Exalt_Wireless_Network_Auth_Suites_List);i++)
        if(exalt_wireless_network_auth_suites_tab[i].auth_suites == auth_suites)
            return exalt_wireless_network_auth_suites_tab[i].name;

    return exalt_wireless_network_name_from_auth_suites(AUTH_SUITES_UNKNOWN);
}


const char* exalt_wireless_network_name_from_security(
        Exalt_Wireless_Network_Security security)
{
    int i;
    for(i = 0; i < sizeof(exalt_wireless_network_security_tab) / sizeof(Exalt_Wireless_Network_Security_List);i++)
        if(exalt_wireless_network_security_tab[i].security == security)
            return exalt_wireless_network_security_tab[i].name;

    return exalt_wireless_network_name_from_security(SECURITY_NONE);
}

/** @} */

