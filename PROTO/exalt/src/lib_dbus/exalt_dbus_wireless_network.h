/*
 * =====================================================================================
 *
 *       Filename:  exalt_dbus_wireless_network.h
 *
 *    Description:  All functions about a wireless network
 *
 *        Version:  1.0
 *        Created:  08/31/2007 08:58:01 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

/** @file exalt_dbus_wireless_network.h */

#ifndef  EXALT_DBUS_WIRELESS_INFO_INC
#define  EXALT_DBUS_WIRELESS_INFO_INC

/**
 * @defgroup Wireless_network
 * @brief Functions about a wireless network, how to get his quality ...
 * A wireless network is link with a interface because (for example) 2 interfaces can detect the same wireless network with a different quality.
 * @{
 */

#include "libexalt_dbus.h"

typedef struct Exalt_DBus_Wireless_Network Exalt_DBus_Wireless_Network;

int exalt_dbus_wirelessnetwork_get_quality(const exalt_dbus_conn* conn, const char* eth, const char* essid);
int exalt_dbus_wirelessnetwork_get_encryption(const exalt_dbus_conn* conn, const char* eth, const char* essid);
int exalt_dbus_wirelessnetwork_get_signallvl(const exalt_dbus_conn* conn, const char* eth, const char* essid);
int exalt_dbus_wirelessnetwork_get_noiselvl(const exalt_dbus_conn* conn, const char* eth, const char* essid);
char* exalt_dbus_wirelessnetwork_get_addr(const exalt_dbus_conn* conn, const char* eth, const char* essid);
char* exalt_dbus_wirelessnetwork_get_mode(const exalt_dbus_conn* conn, const char* eth, const char* essid);
Exalt_Connection * exalt_dbus_wirelessnetwork_get_default_conn(const exalt_dbus_conn* conn, const char* eth, const char* essid);

#define EXALT_DBUS_WIRELESS_NETWORK_SET_HEADER(attribut,type) \
    void exalt_dbus_wireless_network_##attribut##_set(        \
            Exalt_DBus_Wireless_Network *w,                  \
            type attribut)

#define EXALT_DBUS_WIRELESS_NETWORK_TAB_SET_HEADER(attribut,type)        \
    void exalt_dbus_wireless_network_##attribut##_set(            \
            Exalt_DBus_Wireless_Network *w,                      \
            type attribut,                                  \
            int i)


#define EXALT_DBUS_WIRELESS_NETWORK_GET_HEADER(attribut,type)        \
    type exalt_dbus_wireless_network_##attribut##_get(        \
            Exalt_DBus_Wireless_Network *w)

#define EXALT_DBUS_WIRELESS_NETWORK_IS_HEADER(attribut,type)         \
    type exalt_dbus_wireless_network_##attribut##_is(         \
            Exalt_DBus_Wireless_Network *w)



#define EXALT_DBUS_WIRELESS_NETWORK_TAB_GET_HEADER(attribut,type)        \
    type exalt_dbus_wireless_network_##attribut##_get(            \
            Exalt_DBus_Wireless_Network *w,                      \
            int i)



Exalt_DBus_Wireless_Network* exalt_dbus_wireless_network_new();
void exalt_dbus_wireless_network_free(Exalt_DBus_Wireless_Network** w);


EXALT_DBUS_WIRELESS_NETWORK_GET_HEADER(essid,const char*);
EXALT_DBUS_WIRELESS_NETWORK_GET_HEADER(quality,int);
EXALT_DBUS_WIRELESS_NETWORK_GET_HEADER(address,const char*);
EXALT_DBUS_WIRELESS_NETWORK_IS_HEADER(encryption,int);
EXALT_DBUS_WIRELESS_NETWORK_GET_HEADER(noise_lvl,int);
EXALT_DBUS_WIRELESS_NETWORK_GET_HEADER(signal_lvl,int);
EXALT_DBUS_WIRELESS_NETWORK_GET_HEADER(mode,Exalt_Wireless_Network_Mode);
EXALT_DBUS_WIRELESS_NETWORK_GET_HEADER(security_mode,Exalt_Wireless_Network_Security);


EXALT_DBUS_WIRELESS_NETWORK_SET_HEADER(essid,char*);
EXALT_DBUS_WIRELESS_NETWORK_SET_HEADER(quality,int);
EXALT_DBUS_WIRELESS_NETWORK_SET_HEADER(address,char*);
EXALT_DBUS_WIRELESS_NETWORK_SET_HEADER(encryption,int);
EXALT_DBUS_WIRELESS_NETWORK_SET_HEADER(noise_lvl,int);
EXALT_DBUS_WIRELESS_NETWORK_SET_HEADER(signal_lvl,int);
EXALT_DBUS_WIRELESS_NETWORK_SET_HEADER(mode,Exalt_Wireless_Network_Mode);
EXALT_DBUS_WIRELESS_NETWORK_SET_HEADER(security_mode,Exalt_Wireless_Network_Security);

EXALT_DBUS_WIRELESS_NETWORK_SET_HEADER(has_ie,int);
EXALT_DBUS_WIRELESS_NETWORK_SET_HEADER(wpa_type,
        Exalt_Wireless_Network_Wpa_Type);
EXALT_DBUS_WIRELESS_NETWORK_SET_HEADER(wpa_version,int);
EXALT_DBUS_WIRELESS_NETWORK_SET_HEADER(group_cypher,
        Exalt_Wireless_Network_Cypher_Name);
EXALT_DBUS_WIRELESS_NETWORK_TAB_SET_HEADER(pairwise_cypher,
        Exalt_Wireless_Network_Cypher_Name);
EXALT_DBUS_WIRELESS_NETWORK_SET_HEADER(pairwise_cypher_number,int);
EXALT_DBUS_WIRELESS_NETWORK_TAB_SET_HEADER(auth_suites,
        Exalt_Wireless_Network_Auth_Suites);
EXALT_DBUS_WIRELESS_NETWORK_SET_HEADER(auth_suites_number,int);
EXALT_DBUS_WIRELESS_NETWORK_SET_HEADER(preauth_supported,int);

EXALT_DBUS_WIRELESS_NETWORK_IS_HEADER(has_ie,int);
EXALT_DBUS_WIRELESS_NETWORK_GET_HEADER(wpa_type,
        Exalt_Wireless_Network_Wpa_Type);
EXALT_DBUS_WIRELESS_NETWORK_GET_HEADER(wpa_version,int);
EXALT_DBUS_WIRELESS_NETWORK_GET_HEADER(group_cypher,
        Exalt_Wireless_Network_Cypher_Name);
EXALT_DBUS_WIRELESS_NETWORK_TAB_GET_HEADER(pairwise_cypher,
        Exalt_Wireless_Network_Cypher_Name);
EXALT_DBUS_WIRELESS_NETWORK_GET_HEADER(pairwise_cypher_number,int);
EXALT_DBUS_WIRELESS_NETWORK_TAB_GET_HEADER(auth_suites,
        Exalt_Wireless_Network_Auth_Suites);
EXALT_DBUS_WIRELESS_NETWORK_GET_HEADER(auth_suites_number,int);
EXALT_DBUS_WIRELESS_NETWORK_IS_HEADER(preauth_supported,int);


/** @} */

#endif   /* ----- #ifndef EXALT_DBUS_WIRELESS_INFO_INC  ----- */

