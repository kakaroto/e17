/*
 * =====================================================================================
 *
 *       Filename:  exalt_dbus_wireless.c
 *
 *    Description:  All functions about a wireless device
 *
 *        Version:  1.0
 *        Created:  08/31/2007 12:34:48 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

/** @file exalt_dbus_wireless.c */

#include "exalt_dbus_wireless.h"
#include "libexalt_dbus_private.h"


void _exalt_dbus_wireless_list_get_cb(void *data, DBusMessage *msg, DBusError *error);


/**
 * @addtogroup Wireless_interface
 * @{
 */


/**
 * @brief Get the list of wireless interface
 * @param conn a connection
 * @return Returns the list of interface name (char *)
 */
int exalt_dbus_wireless_list_get(exalt_dbus_conn* conn)
{
    DBusMessage *msg;
    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);

    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;

    msg = exalt_dbus_ifaces_wireless_call_new("list");

    EXALT_ASSERT_CUSTOM_RET(e_dbus_message_send (conn->e_conn, msg, _exalt_dbus_wireless_list_get_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
}

/**
 * @brief Scan wireless networks and return the list of network
 * This function can freeze your application during a few secondes
 * @param conn a connection
 * @param eth a wireless interface name
 * @return Returns the list of wireless networks essid (char *)
 */
Ecore_List* exalt_dbus_wireless_scan_wait(const exalt_dbus_conn* conn, const char* eth)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    DBusMessageIter args;
    Ecore_List* res;

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);

    msg = exalt_dbus_read_call_new("IFACE_SCAN_WAIT");
    dbus_message_iter_init_append(msg, &args);
    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &eth),
            dbus_message_unref(msg); return 0,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &eth");

    EXALT_ASSERT_ADV(dbus_connection_send_with_reply (conn->conn, msg, &ret, -1),
            dbus_message_unref(msg); return 0,
            "dbus_connection_send_with_reply (conn->conn, msg, &ret, -1)");

    dbus_message_unref(msg);

    dbus_pending_call_block(ret);
    msg = dbus_pending_call_steal_reply(ret);
    EXALT_ASSERT_RETURN(msg);
    dbus_pending_call_unref(ret);

    EXALT_ASSERT_ADV(exalt_dbus_valid_is(msg),
            return 0,
            "exalt_dbus_valid_is(msg) failed, error=%d (%s)",
            exalt_dbus_error_get_id(msg),
            exalt_dbus_error_get_msg(msg));



    res = exalt_dbus_response_strings(msg,1);
    dbus_message_unref(msg);
    return res;
}

/**
 * @brief Start a scan
 * The result will be notify (see exalt_dbus_scan_notify_set())
 * @param conn a connection
 * @param eth a wireless interface name
 * @return Returns 1 if success, else 0
 */
int exalt_dbus_wireless_scan_start(const exalt_dbus_conn* conn, const char* eth)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    DBusMessageIter args;

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);


    msg = exalt_dbus_read_call_new("IFACE_SCAN_START");
    dbus_message_iter_init_append(msg, &args);
    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &eth),
            dbus_message_unref(msg); return 0,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &eth");
    EXALT_ASSERT_ADV(dbus_connection_send_with_reply (conn->conn, msg, &ret, -1),
            dbus_message_unref(msg); return 0,
            "dbus_connection_send_with_reply (conn->conn, msg, &ret, -1)");

    dbus_message_unref(msg);

    dbus_pending_call_block(ret);
    msg = dbus_pending_call_steal_reply(ret);
    EXALT_ASSERT_RETURN(msg);
    dbus_pending_call_unref(ret);
    EXALT_ASSERT_ADV(exalt_dbus_valid_is(msg),
            return 0,
            "exalt_dbus_valid_is(msg) failed, error=%d (%s)",
            exalt_dbus_error_get_id(msg),
            exalt_dbus_error_get_msg(msg));
    dbus_message_unref(msg);
    return 1;
}

/**
 * @brief Get the current essid of the interface eth
 * @param conn a connection
 * @param eth a wireless interface name
 * @return Returns the essid
 */
char* exalt_dbus_wireless_get_essid(const exalt_dbus_conn* conn, const char* eth)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    DBusMessageIter args;
    char* res;

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);


    msg = exalt_dbus_read_call_new("IFACE_GET_ESSID");
    dbus_message_iter_init_append(msg, &args);
    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &eth),
            dbus_message_unref(msg); return 0,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &eth");
    EXALT_ASSERT_ADV(dbus_connection_send_with_reply (conn->conn, msg, &ret, -1),
            dbus_message_unref(msg); return 0,
            "dbus_connection_send_with_reply (conn->conn, msg, &ret, -1)");

    dbus_message_unref(msg);

    dbus_pending_call_block(ret);
    msg = dbus_pending_call_steal_reply(ret);
    EXALT_ASSERT_RETURN(msg);
    dbus_pending_call_unref(ret);

    EXALT_ASSERT_ADV(exalt_dbus_valid_is(msg),
            return 0,
            "exalt_dbus_valid_is(msg) failed, error=%d (%s)",
            exalt_dbus_error_get_id(msg),
            exalt_dbus_error_get_msg(msg));


    EXALT_STRDUP(res , exalt_dbus_response_string(msg,1));
    dbus_message_unref(msg);
    return res;
}

/**
 * @brief Get the current wpa_supplicant driver used by the interface
 * @param conn a connection
 * @param eth a wireless interface name
 * @return Returns a wpa_supplicant driver (wext, hostap ...)
 */
char* exalt_dbus_wireless_get_wpasupplicant_driver(const exalt_dbus_conn* conn, const char* eth)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    DBusMessageIter args;
    char* res;
    const char* str;

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);


    msg = exalt_dbus_read_call_new("IFACE_GET_WPASUPPLICANT_DRIVER");
    dbus_message_iter_init_append(msg, &args);
    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &eth),
            dbus_message_unref(msg); return 0,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &eth");
    EXALT_ASSERT_ADV(dbus_connection_send_with_reply (conn->conn, msg, &ret, -1),
            dbus_message_unref(msg); return 0,
            "dbus_connection_send_with_reply (conn->conn, msg, &ret, -1)");

    dbus_message_unref(msg);

    dbus_pending_call_block(ret);
    msg = dbus_pending_call_steal_reply(ret);
    EXALT_ASSERT_RETURN(msg);
    dbus_pending_call_unref(ret);

    EXALT_ASSERT_ADV(exalt_dbus_valid_is(msg),
            return 0,
            "exalt_dbus_valid_is(msg) failed, error=%d (%s)",
            exalt_dbus_error_get_id(msg),
            exalt_dbus_error_get_msg(msg));


    str = exalt_dbus_response_string(msg,1);
    EXALT_STRDUP(res,str);
    dbus_message_unref(msg);
    return res;
}

/**
 * @brief Set the wpa_supplicant driver used by the interface
 * @param conn a connection
 * @param eth a wirelss interface name
 * @param driver a driver (wext, hostap ...)
 * @return Returns 1 if success, else 0
 */
int exalt_dbus_wireless_set_wpasupplicant_driver(const exalt_dbus_conn* conn, const char* eth, const char* driver)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    DBusMessageIter args;

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);


    msg = exalt_dbus_write_call_new("IFACE_SET_WPASUPPLICANT_DRIVER");
    dbus_message_iter_init_append(msg, &args);
    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &eth),
            dbus_message_unref(msg); return 0,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &eth");
    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &driver),
            dbus_message_unref(msg); return 0,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &driver");

    EXALT_ASSERT_ADV(dbus_connection_send_with_reply (conn->conn, msg, &ret, -1),
            dbus_message_unref(msg); return 0,
            "dbus_connection_send_with_reply (conn->conn, msg, &ret, -1)");

    dbus_message_unref(msg);

    dbus_pending_call_block(ret);
    msg = dbus_pending_call_steal_reply(ret);
    EXALT_ASSERT_RETURN(msg);
    dbus_pending_call_unref(ret);

    EXALT_ASSERT_ADV(exalt_dbus_valid_is(msg),
            return 0,
            "exalt_dbus_valid_is(msg) failed, error=%d (%s)",
            exalt_dbus_error_get_id(msg),
            exalt_dbus_error_get_msg(msg));


    dbus_message_unref(msg);
    return 1;
}

/** @} */





void _exalt_dbus_wireless_list_get_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;

    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_IFACE_WIRELESS_LIST;
    response->msg_id = id->id;
    if(!exalt_dbus_valid_is(msg))
    {
        response->is_error = 0;
        response->error_id = exalt_dbus_error_get_id(msg);
        response->error_msg = strdup(exalt_dbus_error_get_msg(msg));
    }
    else
    {
        response -> is_error = 1;
        response->l = exalt_dbus_response_strings(msg,1);
    }

    if(id->conn->response_notify->cb)
        id->conn-> response_notify -> cb(response,id->conn->response_notify->user_data);
    EXALT_FREE(data);
}

