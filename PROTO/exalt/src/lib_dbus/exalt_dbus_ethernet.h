/*
 * =====================================================================================
 *
 *       Filename:  exalt_dbus_ethernet.h
 *
 *    Description:  All functions about an ethernet device (no wireless)
 *
 *        Version:  1.0
 *        Created:  08/29/2007 02:19:50 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

/** @file exalt_dbus_ethernet.h */

#ifndef  EXALT_DBUS_ETHERNET_INC
#define  EXALT_DBUS_ETHERNET_INC

#include "libexalt_dbus.h"
#include "define.h"

/**
 * @defgroup Ethernet_interface
 * @brief Functions about an ethernet interface (get ip address, apply a connection ...)
 * @{
 */

int exalt_dbus_eth_list_get(Exalt_DBus_Conn* conn);

int exalt_dbus_eth_ip_get(Exalt_DBus_Conn* conn,const char* eth);
int exalt_dbus_eth_netmask_get(Exalt_DBus_Conn* conn, const char* eth);
int exalt_dbus_eth_gateway_get(Exalt_DBus_Conn* conn, const char* eth);
int exalt_dbus_eth_wireless_is(Exalt_DBus_Conn* conn, const char* eth);
int exalt_dbus_eth_link_is(Exalt_DBus_Conn* conn, const char* eth);
int exalt_dbus_eth_up_is(Exalt_DBus_Conn* conn, const char* eth);
int exalt_dbus_eth_dhcp_is(Exalt_DBus_Conn* conn, const char* eth);


int exalt_dbus_eth_up(Exalt_DBus_Conn* conn, const char* eth);
int exalt_dbus_eth_down(Exalt_DBus_Conn* conn, const char* eth);

int exalt_dbus_eth_conn_apply(Exalt_DBus_Conn* conn, const char* eth,Exalt_Connection*c);


int exalt_dbus_eth_command_get(Exalt_DBus_Conn* conn, const char* eth);
int exalt_dbus_eth_command_set(Exalt_DBus_Conn* conn, const char* eth, const char* cmd);


#endif   /* ----- #ifndef EXALT_DBUS_ETHERNET_INC  ----- */

/** @} */

