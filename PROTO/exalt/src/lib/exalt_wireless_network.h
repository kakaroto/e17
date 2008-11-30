/**  @file exalt_wireless_network.h */
#ifndef EXALT_WIRELESS_NETWORK_H
#define EXALT_WIRELESS_NETWORK_H

#include <stdio.h>



#define EXALT_WIRELESSNETWORK_SET_HEADER(attribut,type) \
    void exalt_wirelessnetwork_##attribut##_set(        \
            Exalt_Wireless_Network *w,                  \
            type attribut);

#define EXALT_WIRELESSNETWORK_TAB_SET_HEADER(attribut,type)        \
    void exalt_wirelessnetwork_##attribut##_set(            \
            Exalt_Wireless_Network *w,                      \
            type attribut,                                  \
            int i);


#define EXALT_WIRELESSNETWORK_GET_HEADER(attribut,type)        \
    type exalt_wirelessnetwork_##attribut##_get(        \
            Exalt_Wireless_Network *w);

#define EXALT_WIRELESSNETWORK_IS_HEADER(attribut,type)         \
    type exalt_wirelessnetwork_##attribut##_is(         \
            Exalt_Wireless_Network *w);



#define EXALT_WIRELESSNETWORK_TAB_GET_HEADER(attribut,type)        \
    type exalt_wirelessnetwork_##attribut##_get(            \
            Exalt_Wireless_Network *w,                      \
            int i);

/**
 * @defgroup Exalt_Wireless_Network
 * @brief The Exalt_Wireless_Network contains all informations about a wireless network (essid, protocol ...)
 * @{
 */

/**
 * @brief informations about a wireless network
 * @structinfo
 */
typedef struct Exalt_Wireless_Network Exalt_Wireless_Network;
typedef enum Exalt_Wireless_Network_Wpa_Type Exalt_Wireless_Network_Wpa_Type;
typedef enum Exalt_Wireless_Network_Cypher_Name Exalt_Wireless_Network_Cypher_Name;
typedef enum Exalt_Wireless_Network_Auth_Suites Exalt_Wireless_Network_Auth_Suites;
typedef enum Exalt_Wireless_Network_Mode Exalt_Wireless_Network_Mode;
typedef enum Exalt_Wireless_Network_Security Exalt_Wireless_Network_Security;
#define Exalt_Wireless_Network(x) (Exalt_Wireless_Network*)x

#include "exalt_wireless.h"
#include "libexalt.h"
#include "exalt_connection.h"

enum Exalt_Wireless_Network_Wpa_Type
{
    WPA_TYPE_UNKNOWN,
    WPA_TYPE_WPA,
    WPA_TYPE_WPA2
};


enum Exalt_Wireless_Network_Cypher_Name
{
    CYPHER_NAME_NONE,
    CYPHER_NAME_WEP40,
    CYPHER_NAME_TKIP,
    CYPHER_NAME_WRAP,
    CYPHER_NAME_CCMP,
    CYPHER_NAME_WEP104,
    CYPHER_NAME_UNKNOWN,
    CYPHER_NAME_PROPRIETARY
};
#define EXALT_WIRELESS_NETWORK_CYPHER_NAME_NUM 8

enum Exalt_Wireless_Network_Auth_Suites
{
    AUTH_SUITES_NONE,
    AUTH_SUITES_8021X,
    AUTH_SUITES_PSK,
    AUTH_SUITES_PROPRIETARY,
    AUTH_SUITES_UNKNOWN
};
#define EXALT_WIRELESS_NETWORK_AUTH_SUITES_NUM 5

enum Exalt_Wireless_Network_Mode
{
    MODE_AUTO,
    MODE_AD_HOC,
    MODE_MANAGED,
    MODE_MASTER,
    MODE_REPEATER,
    MODE_SECONDARY,
    MODE_MONITOR,
    MODE_UNKNOW_BUG
};

enum Exalt_Wireless_Network_Security
{
    SECURITY_NONE,
    SECURITY_RESTRICTED,
    SECURITY_OPEN
};

Exalt_Wireless_Network* exalt_wirelessnetwork_create(
        Exalt_Wireless* w);
void exalt_wirelessnetwork_free(void* data);

EXALT_WIRELESSNETWORK_SET_HEADER(iface,Exalt_Wireless*)
EXALT_WIRELESSNETWORK_SET_HEADER(address,char*)
EXALT_WIRELESSNETWORK_SET_HEADER(essid,char*)
EXALT_WIRELESSNETWORK_SET_HEADER(encryption,int)
EXALT_WIRELESSNETWORK_SET_HEADER(quality,int)
EXALT_WIRELESSNETWORK_SET_HEADER(mode,Exalt_Wireless_Network_Mode)
EXALT_WIRELESSNETWORK_SET_HEADER(security_mode,Exalt_Wireless_Network_Security)

EXALT_WIRELESSNETWORK_GET_HEADER(iface,Exalt_Wireless*)
EXALT_WIRELESSNETWORK_GET_HEADER(address,const char*)
EXALT_WIRELESSNETWORK_GET_HEADER(essid,const char*)
EXALT_WIRELESSNETWORK_IS_HEADER(encryption,int)
EXALT_WIRELESSNETWORK_GET_HEADER(quality,int)
EXALT_WIRELESSNETWORK_GET_HEADER(mode,Exalt_Wireless_Network_Mode)
EXALT_WIRELESSNETWORK_GET_HEADER(security_mode,Exalt_Wireless_Network_Security)


EXALT_WIRELESSNETWORK_SET_HEADER(has_ie,int);
EXALT_WIRELESSNETWORK_SET_HEADER(wpa_type,
        Exalt_Wireless_Network_Wpa_Type)
EXALT_WIRELESSNETWORK_SET_HEADER(wpa_version,int)
EXALT_WIRELESSNETWORK_SET_HEADER(group_cypher,
        Exalt_Wireless_Network_Cypher_Name)
EXALT_WIRELESSNETWORK_TAB_SET_HEADER(pairwise_cypher,
        Exalt_Wireless_Network_Cypher_Name)
EXALT_WIRELESSNETWORK_SET_HEADER(pairwise_cypher_number,int)
EXALT_WIRELESSNETWORK_TAB_SET_HEADER(auth_suites,
        Exalt_Wireless_Network_Auth_Suites)
EXALT_WIRELESSNETWORK_SET_HEADER(auth_suites_number,int)
EXALT_WIRELESSNETWORK_SET_HEADER(preauth_supported,int)
EXALT_WIRELESSNETWORK_SET_HEADER(default_conn,
        Exalt_Connection*)

EXALT_WIRELESSNETWORK_IS_HEADER(has_ie,int);
EXALT_WIRELESSNETWORK_GET_HEADER(wpa_type,
        Exalt_Wireless_Network_Wpa_Type)
EXALT_WIRELESSNETWORK_GET_HEADER(wpa_version,int)
EXALT_WIRELESSNETWORK_GET_HEADER(group_cypher,
        Exalt_Wireless_Network_Cypher_Name)
EXALT_WIRELESSNETWORK_TAB_GET_HEADER(pairwise_cypher,
        Exalt_Wireless_Network_Cypher_Name)
EXALT_WIRELESSNETWORK_GET_HEADER(pairwise_cypher_number,int)
EXALT_WIRELESSNETWORK_TAB_GET_HEADER(auth_suites,
        Exalt_Wireless_Network_Auth_Suites)
EXALT_WIRELESSNETWORK_GET_HEADER(auth_suites_number,int)
EXALT_WIRELESSNETWORK_IS_HEADER(preauth_supported,int)
EXALT_WIRELESSNETWORK_GET_HEADER(default_conn,
        Exalt_Connection*)



const char* exalt_wirelessnetwork_name_from_id(int id);
const char* exalt_wirelessnetwork_name_from_mode(Exalt_Wireless_Network_Mode mode);
Exalt_Wireless_Network_Mode exalt_wirelessnetwork_mode_from_id(int id);


const char* exalt_wireless_network_name_from_wpa_type(Exalt_Wireless_Network_Wpa_Type wpa_type);
const char* exalt_wireless_network_name_from_cypher_name(Exalt_Wireless_Network_Cypher_Name cypher_name);
const char* exalt_wireless_network_name_from_auth_suites(Exalt_Wireless_Network_Auth_Suites auth_suites);
const char* exalt_wireless_network_name_from_security(Exalt_Wireless_Network_Security security);


/** @} */

#endif

