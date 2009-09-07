#ifndef  CB_DNS_INC
#define  CB_DNS_INC

#include "daemon.h"
#include <Exalt.h>

DBusMessage * dbus_cb_dns_list_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
DBusMessage * dbus_cb_dns_add(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
DBusMessage * dbus_cb_dns_delete(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
DBusMessage * dbus_cb_dns_replace(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);

#endif   /* ----- #ifndef CB_DNS_INC  ----- */

