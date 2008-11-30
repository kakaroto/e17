/** @file exalt_wireless_network.c */
#include "exalt_wireless_network.h"
#include "libexalt_private.h"

#define EXALT_WIRELESSNETWORK_STRING_SET(attribut)      \
    void exalt_wirelessnetwork_##attribut##_set(        \
            Exalt_Wireless_Network *w,                  \
            char* attribut)                             \
    {                                                   \
        EXALT_ASSERT_RETURN_VOID(w!=NULL);              \
        if(attribut!=NULL)                              \
            w->attribut = strdup(attribut);             \
        else                                            \
            w->attribut=NULL;                           \
    }


#define EXALT_WIRELESSNETWORK_SET(attribut,type) \
    void exalt_wirelessnetwork_##attribut##_set(        \
            Exalt_Wireless_Network *w,                  \
            type attribut)                              \
    {                                                   \
        EXALT_ASSERT_RETURN_VOID(w!=NULL);              \
        w->attribut = attribut;                         \
    }

#define EXALT_WIRELESSNETWORK_TAB_SET(attribut,type)        \
    void exalt_wirelessnetwork_##attribut##_set(            \
            Exalt_Wireless_Network *w,                      \
            type attribut,                                  \
            int i)                                          \
    {                                                       \
        EXALT_ASSERT_RETURN_VOID(w!=NULL);                  \
        w->attribut[i] = attribut;                          \
    }


#define EXALT_WIRELESSNETWORK_GET(attribut,type)        \
    type exalt_wirelessnetwork_##attribut##_get(        \
            Exalt_Wireless_Network *w)                  \
    {                                                   \
        EXALT_ASSERT_RETURN(w!=NULL);                   \
        return w->attribut;                             \
    }

#define EXALT_WIRELESSNETWORK_IS(attribut,type)         \
    type exalt_wirelessnetwork_##attribut##_is(         \
            Exalt_Wireless_Network *w)                  \
    {                                                   \
        EXALT_ASSERT_RETURN(w!=NULL);                   \
        return w->attribut;                             \
    }



#define EXALT_WIRELESSNETWORK_TAB_GET(attribut,type)        \
    type exalt_wirelessnetwork_##attribut##_get(            \
            Exalt_Wireless_Network *w,                      \
            int i)                                          \
    {                                                       \
        EXALT_ASSERT_RETURN(w!=NULL);                       \
        return w->attribut[i];                              \
    }



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

    //if the wireless network has an IE definition
    int has_ie;
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

    Exalt_Connection* default_conn;
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

Exalt_Wireless_Network_Mode_List exalt_wirelessnetwork_mode_tab[] =
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
 * @param w the interfacewhich be associated to the new network
 * @return Returns the new network
 */
Exalt_Wireless_Network* exalt_wirelessnetwork_create(Exalt_Wireless* w)
{
    Exalt_Wireless_Network* wi = calloc(1,sizeof(Exalt_Wireless_Network));
    EXALT_ASSERT_RETURN(wi!=NULL);
    wi->iface = w;
    wi->security_mode = SECURITY_NONE;
    return wi;
}

/**
 * @brief free Exalt_Wireless_Network
 * @param data the Exalt_Wireless_Network
 */
void exalt_wirelessnetwork_free(void* data)
{
    Exalt_Wireless_Network* wi = Exalt_Wireless_Network(data);
    EXALT_FREE(wi->address);
    EXALT_FREE(wi->essid);
}

EXALT_WIRELESSNETWORK_SET(iface,Exalt_Wireless*)
EXALT_WIRELESSNETWORK_STRING_SET(address)
EXALT_WIRELESSNETWORK_STRING_SET(essid)
EXALT_WIRELESSNETWORK_SET(encryption,int)
EXALT_WIRELESSNETWORK_SET(quality,int)
EXALT_WIRELESSNETWORK_SET(mode,Exalt_Wireless_Network_Mode)
EXALT_WIRELESSNETWORK_SET(security_mode,Exalt_Wireless_Network_Security)

EXALT_WIRELESSNETWORK_GET(iface,Exalt_Wireless*)
EXALT_WIRELESSNETWORK_GET(address,const char*)
EXALT_WIRELESSNETWORK_GET(essid,const char*)
EXALT_WIRELESSNETWORK_IS(encryption,int)
EXALT_WIRELESSNETWORK_GET(quality,int)
EXALT_WIRELESSNETWORK_GET(mode,Exalt_Wireless_Network_Mode)
EXALT_WIRELESSNETWORK_GET(security_mode,Exalt_Wireless_Network_Security)


EXALT_WIRELESSNETWORK_SET(has_ie,int);
EXALT_WIRELESSNETWORK_SET(wpa_type,
        Exalt_Wireless_Network_Wpa_Type)
EXALT_WIRELESSNETWORK_SET(wpa_version,int)
EXALT_WIRELESSNETWORK_SET(group_cypher,
        Exalt_Wireless_Network_Cypher_Name)
EXALT_WIRELESSNETWORK_TAB_SET(pairwise_cypher,
        Exalt_Wireless_Network_Cypher_Name)
EXALT_WIRELESSNETWORK_SET(pairwise_cypher_number,int)
EXALT_WIRELESSNETWORK_TAB_SET(auth_suites,
        Exalt_Wireless_Network_Auth_Suites)
EXALT_WIRELESSNETWORK_SET(auth_suites_number,int)
EXALT_WIRELESSNETWORK_SET(preauth_supported,int)
EXALT_WIRELESSNETWORK_SET(default_conn,
        Exalt_Connection*)

EXALT_WIRELESSNETWORK_IS(has_ie,int);
EXALT_WIRELESSNETWORK_GET(wpa_type,
        Exalt_Wireless_Network_Wpa_Type)
EXALT_WIRELESSNETWORK_GET(wpa_version,int)
EXALT_WIRELESSNETWORK_GET(group_cypher,
        Exalt_Wireless_Network_Cypher_Name)
EXALT_WIRELESSNETWORK_TAB_GET(pairwise_cypher,
        Exalt_Wireless_Network_Cypher_Name)
EXALT_WIRELESSNETWORK_GET(pairwise_cypher_number,int)
EXALT_WIRELESSNETWORK_TAB_GET(auth_suites,
        Exalt_Wireless_Network_Auth_Suites)
EXALT_WIRELESSNETWORK_GET(auth_suites_number,int)
EXALT_WIRELESSNETWORK_IS(preauth_supported,int)
EXALT_WIRELESSNETWORK_GET(default_conn,
        Exalt_Connection*)


const char* exalt_wirelessnetwork_name_from_id(int id)
{
    int i;
    for(i = 0; i < sizeof(exalt_wirelessnetwork_mode_tab) / sizeof(Exalt_Wireless_Network_Mode_List);i++)
        if(exalt_wirelessnetwork_mode_tab[i].id == id)
            return exalt_wirelessnetwork_mode_tab[i].name;

    return exalt_wirelessnetwork_name_from_mode(MODE_UNKNOW_BUG);
}

Exalt_Wireless_Network_Mode exalt_wirelessnetwork_mode_from_id(int id)
{
    int i;
    for(i = 0; i < sizeof(exalt_wirelessnetwork_mode_tab) / sizeof(Exalt_Wireless_Network_Mode_List);i++)
        if(exalt_wirelessnetwork_mode_tab[i].id == id)
            return exalt_wirelessnetwork_mode_tab[i].mode;

    return MODE_UNKNOW_BUG;
}

const char* exalt_wirelessnetwork_name_from_mode(Exalt_Wireless_Network_Mode mode)
{
    int i;
    for(i = 0; i < sizeof(exalt_wirelessnetwork_mode_tab) / sizeof(Exalt_Wireless_Network_Mode_List);i++)
        if(exalt_wirelessnetwork_mode_tab[i].mode == mode)
            return exalt_wirelessnetwork_mode_tab[i].name;

    return exalt_wirelessnetwork_name_from_mode(MODE_UNKNOW_BUG);
}


const char* exalt_wireless_network_name_from_wpa_type(Exalt_Wireless_Network_Wpa_Type wpa_type)
{
    int i;
    for(i = 0; i < sizeof(exalt_wireless_network_wpa_type_tab) / sizeof(Exalt_Wireless_Network_Wpa_Type_List);i++)
        if(exalt_wireless_network_wpa_type_tab[i].wpa_type == wpa_type)
            return exalt_wireless_network_wpa_type_tab[i].name;

    return exalt_wireless_network_name_from_wpa_type(WPA_TYPE_UNKNOWN);
}

const char* exalt_wireless_network_name_from_cypher_name(Exalt_Wireless_Network_Cypher_Name cypher_name)
{
    int i;
    for(i = 0; i < sizeof(exalt_wireless_network_cypher_name_tab) / sizeof(Exalt_Wireless_Network_Cypher_Name_List);i++)
        if(exalt_wireless_network_cypher_name_tab[i].cypher_name == cypher_name)
            return exalt_wireless_network_cypher_name_tab[i].name;

    return exalt_wireless_network_name_from_cypher_name(CYPHER_NAME_UNKNOWN);
}

const char* exalt_wireless_network_name_from_auth_suites(Exalt_Wireless_Network_Auth_Suites auth_suites)
{
    int i;
    for(i = 0; i < sizeof(exalt_wireless_network_auth_suites_tab) / sizeof(Exalt_Wireless_Network_Auth_Suites_List);i++)
        if(exalt_wireless_network_auth_suites_tab[i].auth_suites == auth_suites)
            return exalt_wireless_network_auth_suites_tab[i].name;

    return exalt_wireless_network_name_from_auth_suites(AUTH_SUITES_UNKNOWN);
}


const char* exalt_wireless_network_name_from_security(Exalt_Wireless_Network_Security security)
{
    int i;
    for(i = 0; i < sizeof(exalt_wireless_network_security_tab) / sizeof(Exalt_Wireless_Network_Security_List);i++)
        if(exalt_wireless_network_security_tab[i].security == security)
            return exalt_wireless_network_security_tab[i].name;

    return exalt_wireless_network_name_from_security(SECURITY_NONE);
}

/** @} */

