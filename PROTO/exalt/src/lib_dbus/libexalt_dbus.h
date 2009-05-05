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
#include <string.h>
#include <libexalt.h>

/**
 * @defgroup Exalt_DBus
 * @brief Exalt_DBus library.
 * This library allows to communicate with the exalt daemon by using DBus
 * The source code of exalt_command can be use to understand how using Exalt_CBus.
 */

/**
 * @defgroup General
 * @brief Generals functions
 * @ingroup Exalt_DBus
 * @{
 */

/**
 * @brief An exalt dbus connection
 * Contains a DBus connection and the callback functions.
 * @structinfo
 */
typedef struct _Exalt_DBus_Conn Exalt_DBus_Conn;


/** type of the callback function used when an ip address change, a new interface is add ... */
typedef void (exalt_notify_cb) (char* eth, Exalt_Enum_Action action, void* user_data);
/** type of the callback function used to return the result of a network scan
 * networks is a list of essid (char*)
 */
typedef void (exalt_scan_notify_cb) (char* eth, Eina_List* networks, void* user_data);


#include "define.h"
#include "exalt_dbus_ethernet.h"
#include "exalt_dbus_wireless.h"
#include "exalt_dbus_dns.h"
#include "exalt_dbus_response.h"

/** type of the callback function used when we receive a response */
typedef void (exalt_response_notify_cb) (Exalt_DBus_Response* response, void* user_data);



int exalt_dbus_init();
Exalt_DBus_Conn*  exalt_dbus_connect();

int exalt_dbus_exalt_service_exists(Exalt_DBus_Conn *conn);
int exalt_dbus_service_exists(Exalt_DBus_Conn *conn, const char* service_name);


void exalt_dbus_free(Exalt_DBus_Conn** conn);
void exalt_dbus_shutdown();

int exalt_dbus_notify_set(Exalt_DBus_Conn* conn, exalt_notify_cb* cb, void* user_data);
int exalt_dbus_scan_notify_set(Exalt_DBus_Conn* conn, exalt_scan_notify_cb* cb, void* user_data);
void exalt_dbus_response_notify_set(Exalt_DBus_Conn* conn, exalt_response_notify_cb* cb, void* user_data);


/** @} */

#endif   /* ----- #ifndef LIBEXALT_DBUS_INC  ----- */


