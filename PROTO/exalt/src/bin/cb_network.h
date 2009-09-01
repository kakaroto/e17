
#ifndef  CB_NETWORK_INC
#define  CB_NETWORK_INC

#include "daemon.h"

DBusMessage * dbus_cb_network_configuration_get(E_DBus_Object *obj, DBusMessage *msg);
DBusMessage * dbus_cb_network_list_get(E_DBus_Object *obj, DBusMessage *msg);
DBusMessage * dbus_cb_network_favoris_set(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);
DBusMessage * dbus_cb_network_delete(E_DBus_Object *obj __UNUSED__, DBusMessage *msg);

#endif   /* ----- #ifndef CB_NETWORK_INC  ----- */

