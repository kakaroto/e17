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

int exalt_dbus_eth_ip_get(exalt_dbus_conn* conn,const char* eth);
int exalt_dbus_eth_netmask_get(exalt_dbus_conn* conn, const char* eth);
int exalt_dbus_eth_gateway_get(exalt_dbus_conn* conn, const char* eth);
int exalt_dbus_eth_list_get(exalt_dbus_conn* conn);
int exalt_dbus_eth_wireless_is(exalt_dbus_conn* conn, const char* eth);
int exalt_dbus_eth_link_is(exalt_dbus_conn* conn, const char* eth);
int exalt_dbus_eth_up_is(exalt_dbus_conn* conn, const char* eth);
int exalt_dbus_eth_dhcp_is(exalt_dbus_conn* conn, const char* eth);


int exalt_dbus_eth_up(const exalt_dbus_conn* conn, const char* eth);
int exalt_dbus_eth_down(const exalt_dbus_conn* conn, const char* eth);

int exalt_dbus_eth_conn_apply(exalt_dbus_conn* conn, const char* eth,Exalt_Connection*c);


char* exalt_dbus_eth_cmd_get(const exalt_dbus_conn* conn, const char* eth);
int exalt_dbus_eth_cmd_set(const exalt_dbus_conn* conn, const char* eth, const char* cmd);


#endif   /* ----- #ifndef EXALT_DBUS_ETHERNET_INC  ----- */

/** @} */

