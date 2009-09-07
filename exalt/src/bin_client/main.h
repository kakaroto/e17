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


/*
 * TODO FIXME
 * disabled widgets when the conf is invalid (wait the possibility in elementary)
 * add wireless panels
 *      - an easy panel (show network conf, ask password)
 *      - advanced panel (can choose dhcp, change command, activate ...)
 *      - a panel to create a new network
 * add a dns configuration panel
 */

#ifndef  MAIN_INC
#define  MAIN_INC

typedef enum _iface_type Iface_Type;
typedef struct _pager Pager;

enum _iface_type
{
    IFACE_WIRED,
    IFACE_WIRELESS,
    IFACE_UNKNOWN
} ;

#ifndef __UNUSED__
    #define __UNUSED__ __attribute__((unused))
#endif



#include <libexalt_dbus.h>
#include <Elementary.h>
#include "wired.h"
#include "wireless.h"
#include "iface_list.h"

#define DEFAULT_THEME PACKAGE_DATA_DIR"/icons/theme.edj"

struct _pager
{
    Evas_Object *pager;
    Evas_Object *p_list;
    Evas_Object *p_wired;
    Evas_Object *p_wireless;

    Evas_Object *iface_list;
    Wired* wired;
    Wireless *wireless;
};


Pager pager;
Evas_Object *win;
#define D_(str) gettext(str)


/*
 * The exalt dbus connection is used to communicate with the daemon
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

