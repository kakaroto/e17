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

DBusMessage * dbus_cb_eth_get_ip(E_DBus_Object *obj, DBusMessage *msg);
DBusMessage * dbus_cb_eth_get_netmask(E_DBus_Object *obj, DBusMessage *msg);
DBusMessage * dbus_cb_eth_get_gateway(E_DBus_Object *obj, DBusMessage *msg);
DBusMessage * dbus_cb_eth_get_eth_list(E_DBus_Object *obj, DBusMessage *msg);
DBusMessage * dbus_cb_eth_is_wireless(E_DBus_Object *obj, DBusMessage *msg);
DBusMessage * dbus_cb_eth_is_link(E_DBus_Object *obj, DBusMessage *msg);
DBusMessage * dbus_cb_eth_is_up(E_DBus_Object *obj, DBusMessage *msg);
DBusMessage * dbus_cb_eth_is_dhcp(E_DBus_Object *obj, DBusMessage *msg);


DBusMessage * dbus_cb_eth_up(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
DBusMessage * dbus_cb_eth_down(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);


DBusMessage * dbus_cb_eth_apply_conn(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);


DBusMessage * dbus_cb_eth_get_cmd(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
DBusMessage * dbus_cb_eth_set_cmd(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);

#endif   /* ----- #ifndef CB_FUNCTIONS_INC  ----- */

