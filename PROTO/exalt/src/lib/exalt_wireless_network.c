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
    char* description;
    int quality;

    int ie_choice;
    Eina_List* ie;
};

struct Exalt_Wireless_Network_IE
{
    char* description;

    int auth_choice;
    int pairwise_choice;

    Exalt_Wireless_Network_Wpa_Type wpa_type;

    Exalt_Wireless_Network_Cypher_Name group_cypher;

    Exalt_Wireless_Network_Cypher_Name
        pairwise_cypher[EXALT_WIRELESS_NETWORK_CYPHER_NAME_NUM];
    int pairwise_cypher_number;

    Exalt_Wireless_Network_Auth_Suites
        auth_suites[EXALT_WIRELESS_NETWORK_AUTH_SUITES_NUM];
    int auth_suites_number;

};

Eet_Data_Descriptor * exalt_wireless_network_ie_edd_new()
{
    Eet_Data_Descriptor *edd;
    Exalt_Wireless_Network_IE ie;
    edd = eet_data_descriptor_new("ie", sizeof(Exalt_Wireless_Network_IE),
            (void*(*)(void*))eina_list_next,
            (void*(*)(void*,void*))eina_list_append,
            (void*(*)(void*))eina_list_data_get,
            (void*(*)(void*))eina_list_free,
            (void(*)(void*,int(*)(void*,const char*,void*,void*),void*))evas_hash_foreach,
            (void*(*)(void*,const char*,void*))evas_hash_add,
            (void(*)(void*))evas_hash_free);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Wireless_Network_IE, "description", description, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Wireless_Network_IE, "auth_choice", auth_choice, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Wireless_Network_IE, "pairwise_choice", pairwise_choice, EET_T_INT);
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
    Eet_Data_Descriptor *edd;

    EXALT_ASSERT_RETURN(edd_ie!=NULL);

    edd = eet_data_descriptor_new("Network", sizeof(Exalt_Wireless_Network),
            (void*(*)(void*))eina_list_next,
            (void*(*)(void*,void*))eina_list_append,
            (void*(*)(void*))eina_list_data_get,
            (void*(*)(void*))eina_list_free,
            (void(*)(void*,int(*)(void*,const char*,void*,void*),void*))evas_hash_foreach,
            (void*(*)(void*,const char*,void*))evas_hash_add,
            (void(*)(void*))evas_hash_free);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Wireless_Network, "address", address, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Wireless_Network, "essid", essid, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Wireless_Network, "encryption", encryption, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Wireless_Network, "description", description, EET_T_STRING);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Wireless_Network, "quality", quality, EET_T_INT);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Wireless_Network, "ie_choice", ie_choice, EET_T_INT);

    EET_DATA_DESCRIPTOR_ADD_LIST(edd, Exalt_Wireless_Network, "ies", ie, edd_ie);
    return edd;
}

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
    {AUTH_SUITES_EAP,"EAP"},
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
    EXALT_FREE((*ie)->description);
    EXALT_FREE(*ie);
}

#define EXALT_FCT_NAME exalt_wireless_network
#define EXALT_STRUCT_TYPE Exalt_Wireless_Network

EXALT_STRING_SET(essid)
EXALT_SET(ie_choice,int);
EXALT_STRING_SET(address);
EXALT_SET(iface,Exalt_Wireless*)
EXALT_SET(encryption,int)
EXALT_STRING_SET(description)
EXALT_SET(quality,int)
EXALT_SET(ie,Eina_List*)

EXALT_GET(iface,Exalt_Wireless*)
EXALT_GET(ie_choice,int);
EXALT_GET(address,const char*)
EXALT_GET(essid,const char*)
EXALT_IS(encryption,int)
EXALT_GET(description,const char*)
EXALT_GET(quality,int)
EXALT_GET(ie,Eina_List*)

#undef EXALT_FCT_NAME
#undef EXALT_STRUCT_TYPE

#define EXALT_FCT_NAME exalt_wireless_network_ie
#define EXALT_STRUCT_TYPE Exalt_Wireless_Network_IE

EXALT_STRING_SET(description)
EXALT_SET(auth_choice,int)
EXALT_SET(pairwise_choice,int)
EXALT_SET(wpa_type,Exalt_Wireless_Network_Wpa_Type)
EXALT_SET(group_cypher,Exalt_Wireless_Network_Cypher_Name)
EXALT_TAB_SET(pairwise_cypher,Exalt_Wireless_Network_Cypher_Name)
EXALT_SET(pairwise_cypher_number,int)
EXALT_TAB_SET(auth_suites,Exalt_Wireless_Network_Auth_Suites)
EXALT_SET(auth_suites_number,int)

EXALT_GET(description,const char*)
EXALT_GET(auth_choice,int)
EXALT_GET(pairwise_choice,int)
EXALT_GET(wpa_type,Exalt_Wireless_Network_Wpa_Type)
EXALT_GET(group_cypher,Exalt_Wireless_Network_Cypher_Name)
EXALT_TAB_GET(pairwise_cypher,Exalt_Wireless_Network_Cypher_Name)
EXALT_GET(pairwise_cypher_number,int)
EXALT_TAB_GET(auth_suites,Exalt_Wireless_Network_Auth_Suites)
EXALT_GET(auth_suites_number,int)

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

