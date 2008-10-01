/*
 * =====================================================================================
 *
 *       Filename:  libexalt_dbus.h
 *
 *    Description:  include all headers and define generals functions
 *
 *        Version:  0.0.1
 *        Created:  08/28/2007 04:15:01 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr
 *        Company:
 *
 * =====================================================================================
 */
/** @file libexalt_dbus.h */

#ifndef  LIBEXALT_DBUS_INC
#define  LIBEXALT_DBUS_INC

#include <E_DBus.h>
#include <Ecore_Data.h>
#include <Ecore_Data.h>
#include <string.h>
#include <exalt/libexalt.h>


/**
 * @defgroup General
 * @brief Generals functions
 * @{
 */

/**
 * @brief An exalt dbus connection
 * Contains a DBus connection and the callback functions.
 * @structinfo
 */
typedef struct _exalt_dbus_conn exalt_dbus_conn;

/** type of the callback function used when an ip address change, a new interface is add ... */
typedef void (exalt_notify_cb) (char* eth, Exalt_Enum_Action action, void* user_data);
/** type of the callback function used to return the result of a network scan
 * networks is a list of essid (char*)
 */
typedef void (exalt_scan_notify_cb) (char* eth, Ecore_List* networks, void* user_data);

#include "define.h"
#include "exalt_dbus_ethernet.h"
#include "exalt_dbus_wireless.h"
#include "exalt_dbus_wireless_network.h"
#include "exalt_dbus_dns.h"
#include "exalt_dbus_bootprocess.h"

void exalt_dbus_init();
exalt_dbus_conn*  exalt_dbus_connect();

void exalt_dbus_free(exalt_dbus_conn** conn);
void exalt_dbus_shutdown();

void exalt_dbus_notify_set(exalt_dbus_conn* conn, exalt_notify_cb* cb, void* user_data);
void exalt_dbus_scan_notify_set(exalt_dbus_conn* conn, exalt_scan_notify_cb* cb, void* user_data);

/** @} */

#endif   /* ----- #ifndef LIBEXALT_DBUS_INC  ----- */


