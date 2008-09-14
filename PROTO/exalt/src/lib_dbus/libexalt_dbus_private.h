/*
 * =====================================================================================
 *
 *       Filename:  libexalt_dbus_private.h
 *
 *    Description:  private functions/types
 *
 *        Version:  1.0
 *        Created:  09/03/2007 09:34:07 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  LIBEXALT_DBUS_PRIVATE_INC
#define  LIBEXALT_DBUS_PRIVATE_INC

#include "libexalt_dbus.h"
#include <libexalt.h>

/*
 * when we return a valid response, next args are the response
 */
#define EXALT_DBUS_VALID 1;
/* when we return an error, the next args have to be an int32 with a error id
 * and the third args have to be a string with a description of the error
 */
#define EXALT_DBUS_ERROR 0;



typedef struct _exalt_dbus_notify_data exalt_dbus_notify_data;
typedef struct _exalt_dbus_notify_conn_applied_data exalt_dbus_notify_conn_applied_data;
typedef struct _exalt_dbus_scan_notify_data exalt_dbus_scan_notify_data;


struct _exalt_dbus_conn
{
    E_DBus_Connection *e_conn;
    DBusConnection* conn;
    exalt_dbus_notify_data* notify;
    exalt_dbus_scan_notify_data* scan_notify;
};

struct _exalt_dbus_notify_data
{
    exalt_notify_cb* cb;
    void* user_data;
};

struct _exalt_dbus_scan_notify_data
{
    exalt_scan_notify_cb* cb;
    void* user_data;
};



const char* exalt_dbus_response_string(DBusMessage *msg, int pos);
Ecore_List* exalt_dbus_response_strings(DBusMessage *msg, int pos);
void exalt_dbus_string_free(void* data);
int exalt_dbus_response_boolean(DBusMessage *msg, int pos);
int exalt_dbus_response_integer(DBusMessage *msg, int pos);

int exalt_dbus_valid_is(DBusMessage *msg);
int exalt_dbus_error_get_id(DBusMessage *msg);
const char* exalt_dbus_error_get_msg(DBusMessage *msg);


#endif   /* ----- #ifndef LIBEXALT_DBUS_PRIVATE_INC  ----- */

