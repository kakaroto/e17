#ifndef  EXALT_DBUS_NETWORK_INC
#define  EXALT_DBUS_NETWORK_INC

#include <libexalt_dbus.h>

int exalt_dbus_network_configuration_get(Exalt_DBus_Conn *conn, const char* essid);
int exalt_dbus_network_list_get(Exalt_DBus_Conn *conn);
int exalt_dbus_network_favoris_set(Exalt_DBus_Conn *conn, const char *essid, int favoris);
int exalt_dbus_network_delete(Exalt_DBus_Conn *conn, const char* essid);
#endif   /* ----- #ifndef EXALT_DBUS_NETWORK_INC  ----- */

