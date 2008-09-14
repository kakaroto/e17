/*
 * =====================================================================================
 *
 *       Filename:  exalt_dbus_bootprocess.c
 *
 *    Description:  manage the boot process, during the boot process the system can wait a card.
 *
 *        Version:  1.0
 *        Created:  01/04/2008 01:00:19 PM CET
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

/** @file exalt_dbus_bootprocess.c */

#include "exalt_dbus_bootprocess.h"
#include "libexalt_dbus_private.h"

/**
 * @addtogroup Boot_process
 * @{
 */

/**
 * @brief Add an interface in the list
 * @param conn a connection
 * @param interface the interface
 * @return Returns 1 if success, else 0
 */
int exalt_dbus_bootprocess_iface_add(const exalt_dbus_conn* conn, const char* interface)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    DBusMessageIter args;

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(interface!=NULL);

    msg = exalt_dbus_write_call_new("BOOTPROCESS_IFACE_ADD");
    dbus_message_iter_init_append(msg, &args);

    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &interface),
            dbus_message_unref(msg); return 0,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &interface");


    EXALT_ASSERT_ADV(dbus_connection_send_with_reply (conn->conn, msg, &ret, -1),
            dbus_message_unref(msg); return 0,
            "dbus_connection_send_with_reply (conn->conn, msg, &ret, -1)");


    dbus_message_unref(msg);

    dbus_pending_call_block(ret);
    msg = dbus_pending_call_steal_reply(ret);
    EXALT_ASSERT_RETURN(msg!=NULL);

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
 * @brief Remove an interface of the list
 * @param conn a connection
 * @param interface the interface
 * @return Returns 1 if success, else 0
 */
int exalt_dbus_bootprocess_iface_remove(const exalt_dbus_conn* conn, const char* interface)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    DBusMessageIter args;

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(interface!=NULL);

    msg = exalt_dbus_write_call_new("BOOTPROCESS_IFACE_REMOVE");
    dbus_message_iter_init_append(msg, &args);

    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &interface),
            dbus_message_unref(msg); return 0,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &interface");


    EXALT_ASSERT_ADV(dbus_connection_send_with_reply (conn->conn, msg, &ret, -1),
            dbus_message_unref(msg); return 0,
            "dbus_connection_send_with_reply (conn->conn, msg, &ret, -1)");


    dbus_message_unref(msg);

    dbus_pending_call_block(ret);
    msg = dbus_pending_call_steal_reply(ret);
    EXALT_ASSERT_RETURN(msg!=NULL);
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
 * @brief Test if an interface is in the list
 * @param conn a connection
 * @param interface the interface
 * @return Returns 1 if yes, else 0
 */
int exalt_dbus_bootprocess_iface_is(const exalt_dbus_conn* conn, const char* interface)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    DBusMessageIter args;
    int res;

     EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(interface!=NULL);

    msg = exalt_dbus_read_call_new("BOOTPROCESS_IFACE_IS");
    dbus_message_iter_init_append(msg, &args);

    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &interface),
            dbus_message_unref(msg); return 0,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &interface");


    EXALT_ASSERT_ADV(dbus_connection_send_with_reply (conn->conn, msg, &ret, -1),
            dbus_message_unref(msg); return 0,
            "dbus_connection_send_with_reply (conn->conn, msg, &ret, -1)");

    dbus_message_unref(msg);

    dbus_pending_call_block(ret);
    msg = dbus_pending_call_steal_reply(ret);
    EXALT_ASSERT_RETURN(msg!=NULL);
    dbus_pending_call_unref(ret);

    EXALT_ASSERT_ADV(exalt_dbus_valid_is(msg),
            return 0,
            "exalt_dbus_valid_is(msg) failed, error=%d (%s)",
            exalt_dbus_error_get_id(msg),
            exalt_dbus_error_get_msg(msg));


    res=exalt_dbus_response_boolean(msg,1);
    dbus_message_unref(msg);

    return res;
}

/**
 * @brief Change the value of the timeout
 * @param conn a connection
 * @param timeout the new value in second
 * @return Returns 1 if success, else 0
 */
int exalt_dbus_bootprocess_timeout_set(const exalt_dbus_conn* conn, int timeout)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    DBusMessageIter args;


     EXALT_ASSERT_RETURN(conn!=NULL);

    msg = exalt_dbus_write_call_new("BOOTPROCESS_TIMEOUT_SET");
    dbus_message_iter_init_append(msg, &args);

    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &timeout),
            dbus_message_unref(msg); return 0,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &timeout");

    EXALT_ASSERT_ADV(dbus_connection_send_with_reply (conn->conn, msg, &ret, -1),
            dbus_message_unref(msg); return 0,
            "dbus_connection_send_with_reply (conn->conn, msg, &ret, -1)");

    dbus_message_unref(msg);

    dbus_pending_call_block(ret);
    msg = dbus_pending_call_steal_reply(ret);
    EXALT_ASSERT_RETURN(msg!=NULL);
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
 * @brief Get the value of thetimeout
 * @param conn a connection
 * @return Returns the value, 0 if error
 */
int exalt_dbus_bootprocess_timeout_get(const exalt_dbus_conn* conn)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    int res;

     EXALT_ASSERT_RETURN(conn!=NULL);

    msg = exalt_dbus_read_call_new("BOOTPROCESS_TIMEOUT_GET");

    EXALT_ASSERT_ADV(dbus_connection_send_with_reply (conn->conn, msg, &ret, -1),
            dbus_message_unref(msg); return 0,
            "dbus_connection_send_with_reply (conn->conn, msg, &ret, -1)");

    dbus_message_unref(msg);

    dbus_pending_call_block(ret);
    msg = dbus_pending_call_steal_reply(ret);
    EXALT_ASSERT_RETURN(msg!=NULL);
    dbus_pending_call_unref(ret);

    EXALT_ASSERT_ADV(exalt_dbus_valid_is(msg),
            return 0,
            "exalt_dbus_valid_is(msg) failed, error=%d (%s)",
            exalt_dbus_error_get_id(msg),
            exalt_dbus_error_get_msg(msg));

    res=exalt_dbus_response_integer(msg,1);
    dbus_message_unref(msg);

    return res;
}

/** @} */

