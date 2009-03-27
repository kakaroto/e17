/*
 * =====================================================================================
 *
 *       Filename:  main.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  11/02/09 20:22:59 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  MAIN_INC
#define  MAIN_INC

#include <libexalt_dbus.h>
#include <Elementary.h>

#define ICONS_ETHERNET_ACTIVATE PACKAGE_DATA_DIR"/icons/ethernet.png"
#define ICONS_ETHERNET_ACTIVATE_SMALL PACKAGE_DATA_DIR"/icons/ethernet_small.png"

#define ICONS_WIRELESS_ACTIVATE PACKAGE_DATA_DIR"/icons/wireless.png"
#define ICONS_WIRELESS_ACTIVATE_SMALL PACKAGE_DATA_DIR"/icons/wireless_small.png"

#define ICONS_ETHERNET_NOT_ACTIVATE PACKAGE_DATA_DIR"/icons/ethernet_not_activate_link.png"
#define ICONS_ETHERNET_NOT_ACTIVATE_SMALL PACKAGE_DATA_DIR"/icons/ethernet_not_activate_link_small.png"

#define ICONS_WIRELESS_NOT_ACTIVATE PACKAGE_DATA_DIR"/icons/wireless_not_activate_link.png"
#define ICONS_WIRELESS_NOT_ACTIVATE_SMALL PACKAGE_DATA_DIR"/icons/wireless_not_activate_link_small.png"


#define ICONS_LOGO PACKAGE_DATA_DIR"/icons/logo.png"

#ifndef __UNUSED__
    #define __UNUSED__ __attribute__((unused))
#endif

typedef enum _iface_type Iface_Type;

enum _iface_type
{
    IFACE_WIRED,
    IFACE_WIRELESS,
    IFACE_UNKNOWN
} ;

Evas_Object *win;
Evas_Object *iface_list; 
#define D_(str) gettext(str)


/*
 * The exalt dbus connection is use to communicate with the daemon
 */
Exalt_DBus_Conn* conn;


/*
 * 2 callbacks
 * The first is used to get the response of a question (exalt_dbus_ip_get ....)
 * The second is used to get some notifications (the ip address change ...)
 */
void response_cb(Exalt_DBus_Response* response, void* data );
void notify_cb(char* eth, Exalt_Enum_Action action, void* user_data);


#endif   /* ----- #ifndef MAIN_INC  ----- */

