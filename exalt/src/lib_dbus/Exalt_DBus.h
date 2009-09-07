/** @file Exalt_DBus.h */

#ifndef  EXALT_DBUS_INC
#define  EXALT_DBUS_INC

#include <E_DBus.h>
#include <Ecore_Data.h>
#include <string.h>
#include <Exalt.h>

/**
 * @defgroup Exalt_DBus
 * @brief Exalt_DBus library.
 * This library allows to communicate with the exalt daemon by using DBus
 * The source code of exalt_command can be used to understand how using Exalt_DBus.
 */

/**
 * @defgroup General
 * @brief Generals functions
 * @ingroup Exalt_DBus
 * @{
 */

/**
 * @brief An exalt dbus connection
 * Contains a DBus connection and the callbacks functions.
 */
typedef struct _Exalt_DBus_Conn Exalt_DBus_Conn;


/** type of the callback function used to notify a change, for example if an IP address change. */
typedef void (exalt_notify_cb) (char* eth, Exalt_Enum_Action action, void* user_data);
/** type of the callback function used to return the result of a network scan <br>
 * networks is a list of Exalt_Wireless_Network*
 */
typedef void (exalt_scan_notify_cb) (char* eth, Eina_List* networks, void* user_data);


#include "define.h"
#include "exalt_dbus_ethernet.h"
#include "exalt_dbus_wireless.h"
#include "exalt_dbus_dns.h"
#include "exalt_dbus_response.h"

/** type of the callback function used to return a response */
typedef void (exalt_response_notify_cb) (Exalt_DBus_Response* response, void* user_data);



EAPI int exalt_dbus_init();
EAPI Exalt_DBus_Conn*  exalt_dbus_connect();

EAPI int exalt_dbus_exalt_service_exists(Exalt_DBus_Conn *conn);
EAPI int exalt_dbus_service_exists(Exalt_DBus_Conn *conn, const char* service_name);


EAPI void exalt_dbus_free(Exalt_DBus_Conn** conn);
EAPI void exalt_dbus_shutdown();

EAPI int exalt_dbus_notify_set(Exalt_DBus_Conn* conn, exalt_notify_cb* cb, void* user_data);
EAPI int exalt_dbus_scan_notify_set(Exalt_DBus_Conn* conn, exalt_scan_notify_cb* cb, void* user_data);
EAPI void exalt_dbus_response_notify_set(Exalt_DBus_Conn* conn, exalt_response_notify_cb* cb, void* user_data);


/** @} */

#endif   /* ----- #ifndef EXALT_DBUS_INC  ----- */


