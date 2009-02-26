/*
 * =====================================================================================
 *
 *       Filename:  libexalt_private.h
 *
 *    Description:  private functions
 *
 *        Version:  1.0
 *        Created:  09/03/2007 09:00:35 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  LIBEXALT_PRIVATE_INC
#define  LIBEXALT_PRIVATE_INC

#include "../config.h"
#include "libexalt.h"
#include <string.h>
#include "nettools/proc.h"
#include "iw/iwlib.h"
#include "wpa_supplicant/wpa_ctrl.h"
#include "exalt_regexp.h"
#include <Eet.h>
#include <E_DBus.h>
#include "exalt_ethernet.h"



// type use in the list exalt_eth_interfaces.default_routes
// which is a list of the old default route
// Exalt uses this list when the current default route become invalid (the interface is down for example) and try to find a previous yet available route.
typedef struct default_route
{
    char* interface;
    char* gateway;
}Default_Route;

struct Exalt_Ethernets
{
    Eina_List* ethernets;

    int we_version;

    short admin;
    short is_launch ;

    E_DBus_Connection *dbus_conn;
    Ecore_Fd_Handler *rtlink_watch;
    int rtlink_sock;

    Eina_List* default_routes;

    Exalt_Eth_Cb eth_cb;
    void * eth_cb_user_data;

    Exalt_Wifi_Scan_Cb wireless_scan_cb;
    void* wireless_scan_cb_user_data;
};

typedef struct exalt_ioctl_key
{
    int key;
    char* value;
} Exalt_Ioctl_Key;

/** Command to launch a dhcp */
#ifdef DHCP_COMMAND_PATH
    #define COMMAND_DHCLIENT DHCP_COMMAND_PATH " %s -1 -q"
    #define DHCLIENT_PID_FILE "/var/run/dhcp/dhclient.pid"
#endif

/** Command to lauch the wpa_supplicant daemon */
#ifdef WPA_SUPPLICANT_COMMAND_PATH
    #define COMMAND_WPA WPA_SUPPLICANT_COMMAND_PATH " -D%s -i%s -c%s -P%s -B"
#endif


//define in exalt_ethernet.c but use in libexalt.c
void _exalt_cb_signal_device_added(void *data, DBusMessage *msg);
void _exalt_cb_signal_device_removed(void *data, DBusMessage *msg);
void _exalt_cb_find_device_by_capability_net(void *user_data, void *reply_data, DBusError *error);
int _exalt_rtlink_watch_cb(void *data, Ecore_Fd_Handler *fd_handler);


char *str_remove (const char *s, const char *ct);
char* exalt_addr_hexa_to_dec(const char* addr);
short exalt_ioctl(void* argp, int request);

int _exalt_rtlink_essid_change(Exalt_Wireless *w);


void _exalt_wpa_stop(Exalt_Wireless* w);


#endif   /* ----- #ifndef LIBEXALT_PRIVATE_INC  ----- */

