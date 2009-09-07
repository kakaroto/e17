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

