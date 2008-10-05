/*
 * =====================================================================================
 *
 *       Filename:  cb_functions.h
 *
 *    Description:  All callbacks functions are defined here
 *
 *        Version:  1.0
 *        Created:  08/29/2007 01:48:40 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  CB_FUNCTIONS_INC
#define  CB_FUNCTIONS_INC

#include "daemon.h"
#include <libexalt.h>

DBusMessage * dbus_cb_eth_ip_get(E_DBus_Object *obj, DBusMessage *msg);
DBusMessage * dbus_cb_eth_netmask_get(E_DBus_Object *obj, DBusMessage *msg);
DBusMessage * dbus_cb_eth_gateway_get(E_DBus_Object *obj, DBusMessage *msg);
DBusMessage * dbus_cb_eth_get_eth_list(E_DBus_Object *obj, DBusMessage *msg);
DBusMessage * dbus_cb_eth_wireless_is(E_DBus_Object *obj, DBusMessage *msg);
DBusMessage * dbus_cb_eth_link_is(E_DBus_Object *obj, DBusMessage *msg);
DBusMessage * dbus_cb_eth_up_is(E_DBus_Object *obj, DBusMessage *msg);
DBusMessage * dbus_cb_eth_dhcp_is(E_DBus_Object *obj, DBusMessage *msg);


DBusMessage * dbus_cb_eth_up(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
DBusMessage * dbus_cb_eth_down(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);


DBusMessage * dbus_cb_eth_conn_aply(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);


DBusMessage * dbus_cb_eth_cmd_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
DBusMessage * dbus_cb_eth_cmd_set(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);

#endif   /* ----- #ifndef CB_FUNCTIONS_INC  ----- */

