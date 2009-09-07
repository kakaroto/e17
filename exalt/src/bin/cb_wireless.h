#ifndef  CB_WIRELESS_INC
#define  CB_WIRELESS_INC

#include "daemon.h"

DBusMessage * dbus_cb_wireless_list_get(E_DBus_Object *obj, DBusMessage *msg);

DBusMessage * dbus_cb_wireless_essid_get(E_DBus_Object *obj, DBusMessage *msg);
DBusMessage * dbus_cb_wireless_disconnect(E_DBus_Object *obj, DBusMessage *msg);

DBusMessage * dbus_cb_wireless_wpasupplicant_driver_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
DBusMessage * dbus_cb_wireless_wpasupplicant_driver_set(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);

DBusMessage * dbus_cb_wireless_scan(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);

#endif   /* ----- #ifndef CB_WIRELESS_INC  ----- */

