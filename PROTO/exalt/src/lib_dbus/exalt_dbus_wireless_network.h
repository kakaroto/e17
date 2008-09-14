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

int exalt_dbus_wirelessnetwork_get_quality(const exalt_dbus_conn* conn, const char* eth, const char* essid);
int exalt_dbus_wirelessnetwork_get_encryption(const exalt_dbus_conn* conn, const char* eth, const char* essid);
int exalt_dbus_wirelessnetwork_get_signallvl(const exalt_dbus_conn* conn, const char* eth, const char* essid);
int exalt_dbus_wirelessnetwork_get_noiselvl(const exalt_dbus_conn* conn, const char* eth, const char* essid);
char* exalt_dbus_wirelessnetwork_get_addr(const exalt_dbus_conn* conn, const char* eth, const char* essid);
char* exalt_dbus_wirelessnetwork_get_protocol(const exalt_dbus_conn* conn, const char* eth, const char* essid);
char* exalt_dbus_wirelessnetwork_get_mode(const exalt_dbus_conn* conn, const char* eth, const char* essid);
char* exalt_dbus_wirelessnetwork_get_bitrates(const exalt_dbus_conn* conn, const char* eth, const char* essid);
Exalt_Connection * exalt_dbus_wirelessnetwork_get_default_conn(const exalt_dbus_conn* conn, const char* eth, const char* essid);

/** @} */

#endif   /* ----- #ifndef EXALT_DBUS_WIRELESS_INFO_INC  ----- */

