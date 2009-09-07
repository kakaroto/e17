/*
 * =====================================================================================
 *
 *       Filename:  cb_bootprocess.h
 *
 *    Description:  cb callback about the boot process
 *
 *        Version:  1.0
 *        Created:  01/02/2008 06:44:36 PM CET
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  CB_BOOTPROCESS_INC
#define  CB_BOOTPROCESS_INC

#include "daemon.h"
#include "boot_process.h"

DBusMessage * dbus_cb_bootprocess_iface_add(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
DBusMessage * dbus_cb_bootprocess_iface_remove(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
DBusMessage * dbus_cb_bootprocess_iface_is(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);

DBusMessage * dbus_cb_bootprocess_timeout_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
DBusMessage * dbus_cb_bootprocess_timeout_set(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
#endif   /* ----- #ifndef CB_BOOTPROCESS_INC  ----- */

