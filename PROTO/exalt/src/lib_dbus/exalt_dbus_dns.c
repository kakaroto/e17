/*
 * =====================================================================================
 *
 *       Filename:  exalt_dbus_dns.c
 *
 *    Description:  manage the dns list
 *
 *        Version:  1.0
 *        Created:  09/21/2007 09:03:33 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

/** @file exalt_dbus_dns.c */

#include "exalt_dbus_dns.h"
#include "libexalt_dbus_private.h"

/**
 * @addtogroup DNS
 * @{
 */

/**
 * @brief get the DNS list
 * @param conn a connection
 * @return Returns the DNS list (char*)
 */
Ecore_List* exalt_dbus_dns_get_list(const exalt_dbus_conn* conn)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    Ecore_List* res;

    EXALT_ASSERT_RETURN(conn!=NULL);

    msg = exalt_dbus_read_call_new("DNS_GET_LIST");
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
 * @brief Add a new DNS
 * @param conn a connection
 * @param dns the new DNS ( a valid ip address)
 * @return Returns 1 if sucess, else 0
 */
int exalt_dbus_dns_add(const exalt_dbus_conn* conn, const char* dns)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    DBusMessageIter args;

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(dns!=NULL);

    msg = exalt_dbus_write_call_new("DNS_ADD");
    dbus_message_iter_init_append(msg, &args);

    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &dns),
            dbus_message_unref(msg); return 0,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &dns");

    EXALT_ASSERT_ADV(dbus_connection_send_with_reply (conn->conn, msg, &ret, -1),
            dbus_message_unref(msg); return 0,
            "dbus_connection_send_with_reply (conn->conn, msg, &ret, -1)");

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
 * @brief Delete a DNS
 * @param conn a connection
 * @param dns a DNS ( a valid ip address)
 * @return Returns 1 if sucess, else 0
 */
int exalt_dbus_dns_delete(const exalt_dbus_conn* conn, const char* dns)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    DBusMessageIter args;
    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(dns!=NULL);

    msg = exalt_dbus_write_call_new("DNS_DELETE");
    dbus_message_iter_init_append(msg, &args);

    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &dns),
            dbus_message_unref(msg); return 0,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &dns");

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
 * @brief Replace a DNS by a new
 * @param conn a connection
 * @param old_dns the old DNS (a valid ip address)
 * @param new_dns the new DNS (a valid ip address)
 * @return Returns 1 if sucess, else 0
 */
int exalt_dbus_dns_replace(const exalt_dbus_conn* conn, const char* old_dns, const char* new_dns)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    DBusMessageIter args;

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(old_dns!=NULL);
    EXALT_ASSERT_RETURN(new_dns!=NULL);

    msg = exalt_dbus_write_call_new("DNS_REPLACE");
    dbus_message_iter_init_append(msg, &args);
    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &old_dns),
            dbus_message_unref(msg); return 0,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &dns");
    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &new_dns),
            dbus_message_unref(msg); return 0,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &dns");

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



/** @} */
