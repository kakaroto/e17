/*
 * =====================================================================================
 *
 *       Filename:  cb_dns.h
 *
 *    Description:  DNS functions
 *
 *        Version:  1.0
 *        Created:  09/21/2007 09:20:24 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  CB_DNS_INC
#define  CB_DNS_INC

#include "daemon.h"
#include <libexalt.h>

DBusMessage * dbus_cb_dns_list_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
DBusMessage * dbus_cb_dns_add(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
DBusMessage * dbus_cb_dns_delete(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
DBusMessage * dbus_cb_dns_replace(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);

#endif   /* ----- #ifndef CB_DNS_INC  ----- */

