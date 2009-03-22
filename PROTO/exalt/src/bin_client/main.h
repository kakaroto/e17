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

typedef struct _hover_wired Hover_Wired;
typedef struct _pnl_right Pnl_Right;
typedef enum _iface_type Iface_Type;


enum _iface_type
{
    IFACE_WIRED,
    IFACE_WIRELESS,
    IFACE_UNKNOWN
} ;

struct _hover_wired
{
    char* iface;
    Evas_Object* hover;

    Evas_Object *btn_title;
    Evas_Object *lbl_ip;
    Evas_Object *lbl_netmask;
    Evas_Object *lbl_gateway;

    Evas_Object *icon;
};

Evas_Object *win;
Evas_Object *table;


Hover_Wired hover_wired;



#include "iface_list.h"
#include "pnl_right.h"
#include "hover_wired.h"

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

