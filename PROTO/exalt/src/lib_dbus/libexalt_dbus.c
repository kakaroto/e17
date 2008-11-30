/*
 * =====================================================================================
 *
 *       Filename:  libexalt_dbus.c
 *
 *    Description:  defines generals functions
 *
 *        Version:  1.0
 *        Created:  08/28/2007 04:17:06 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */
/** @file libexalt_dbus.c */
#include "libexalt_dbus.h"
#include "libexalt_dbus_private.h"

void _exalt_dbus_notify(void *data,DBusMessage *msg);
void _exalt_dbus_scan_notify(void *data, DBusMessage *msg);

/**
 * @addtogroup General
 * @{
 */

/**
 * @brief Initialise the library
 * Don't forget to create a connection with exalt_dbus_connect() after
 */
void exalt_dbus_init()
{
    ecore_init();
    e_dbus_init();
}

/**
 * @brief Create a connection
 * @return Returns a new DBus connection
 */
exalt_dbus_conn* exalt_dbus_connect()
{
    exalt_dbus_conn* conn;
    DBusError err;
    conn = calloc(1,sizeof(exalt_dbus_conn));
    //initialise the errors
    dbus_error_init(&err);
    // connect to the bus
    conn->conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    conn->e_conn = e_dbus_connection_setup(conn->conn);
    conn->msg_id = 1;
    return conn;
}

/**
 * @brief Free a exalt DBus connection
 * @param conn a connection
 */
void exalt_dbus_free(exalt_dbus_conn** conn)
{
    e_dbus_connection_close((*conn)->e_conn);
    EXALT_FREE((*conn)->notify);
    EXALT_FREE((*conn)->scan_notify);
    EXALT_FREE(*conn);
}

/**
 * @brief shutdown the library
 */
void exalt_dbus_shutdown()
{
    e_dbus_shutdown();
    ecore_shutdown();
}

/**
 * @brief set the callback function which will be called when an ip address change, an interface is add ...
 * See Exalt_Enum_Action for more information
 * @param conn a connection
 * @param cb the callback function
 * @param user_data the user data
 */
void exalt_dbus_notify_set(exalt_dbus_conn* conn, exalt_notify_cb *cb, void* user_data)
{
    EXALT_ASSERT_RETURN_VOID(conn!=NULL);
    //better to delete the handler
    //but e_dbus doesn't have a function ...

    if(!conn->notify)
    {
        conn -> notify = malloc(sizeof(exalt_dbus_notify_data));
        conn -> notify -> cb = cb;
        conn -> notify -> user_data = user_data;

        e_dbus_signal_handler_add(conn->e_conn, EXALTD_SERVICE, EXALTD_PATH_NOTIFY,
                EXALTD_INTERFACE_NOTIFY, "notify",
                _exalt_dbus_notify, conn);
    }
    else
    {
        conn -> notify -> cb = cb;
        conn -> notify -> user_data = user_data;
    }
}

/**
 * @brief set the callback function which will be call to return the result of a network scan
 * @param conn
 * @param cb the callback function
 * @param user_data the user data
 */
void exalt_dbus_scan_notify_set(exalt_dbus_conn* conn, exalt_scan_notify_cb *cb, void* user_data)
{
    EXALT_ASSERT_RETURN_VOID(conn!=NULL);

    //better to delete the handler
    //but e_dbus doesn't have a function ...

    if(!conn->scan_notify)
    {
        conn -> scan_notify = malloc(sizeof(exalt_dbus_scan_notify_data));
        conn -> scan_notify -> cb = cb;
        conn -> scan_notify -> user_data = user_data;

        e_dbus_signal_handler_add(conn->e_conn, EXALTD_SERVICE, EXALTD_PATH_NOTIFY,
                EXALTD_INTERFACE_NOTIFY, "scan_notify",
                _exalt_dbus_scan_notify, conn);
    }
    else
    {
        conn -> scan_notify -> cb = cb;
        conn -> scan_notify -> user_data = user_data;
    }
}

/**
 * @brief set the callback function which will be call to return a response
 * @param conn
 * @param cb the callback function
 * @param user_data the user data
 */
void exalt_dbus_response_notify_set(exalt_dbus_conn* conn, exalt_response_notify_cb *cb, void* user_data)
{
    EXALT_ASSERT_RETURN_VOID(conn!=NULL);

    if(!conn->response_notify)
    {
        conn -> response_notify = calloc(1,sizeof(exalt_dbus_response_data));
        conn -> response_notify -> cb = cb;
        conn -> response_notify -> user_data = user_data;
    }
    else
    {
        conn -> response_notify -> cb = cb;
        conn -> response_notify -> user_data = user_data;
    }
}


/** @} */

