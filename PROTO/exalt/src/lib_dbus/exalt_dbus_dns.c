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

void _exalt_dbus_dns_add_cb(void *data, DBusMessage *msg, DBusError *error);
void _exalt_dbus_dns_del_cb(void *data, DBusMessage *msg, DBusError *error);
void _exalt_dbus_dns_replace_cb(void *data, DBusMessage *msg, DBusError *error);
void _exalt_dbus_dns_get_list_cb(void *data, DBusMessage *msg, DBusError *error);




/**
 * @addtogroup DNS
 * @{
 */

/**
 * @brief get the DNS list
 * @param conn a connection
 * @return Returns 1 if success, else 0
 */
int exalt_dbus_dns_get_list(exalt_dbus_conn* conn)
{
    DBusMessage *msg;

    EXALT_ASSERT_RETURN(conn!=NULL);

    msg = exalt_dbus_dns_call_new("get");
    EXALT_ASSERT_CUSTOM_RET(
            e_dbus_message_send(conn->e_conn,msg,_exalt_dbus_dns_get_list_cb,30,conn),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return 1;
}


/**
 * @brief Add a new DNS
 * @param conn a connection
 * @param dns the new DNS ( a valid ip address)
 * @return Returns 1 if sucess, else 0
 */
int exalt_dbus_dns_add(exalt_dbus_conn* conn, const char* dns)
{
    DBusMessage *msg;
    DBusMessageIter args;

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(dns!=NULL);

    msg = exalt_dbus_dns_call_new("add");
    dbus_message_iter_init_append(msg, &args);

    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &dns),
            dbus_message_unref(msg); return 0);

    EXALT_ASSERT_CUSTOM_RET(e_dbus_message_send(conn->e_conn,msg,_exalt_dbus_dns_add_cb,30,conn),
            dbus_message_unref(msg); return 0);

    return 1;
}

/**
 * @brief Delete a DNS
 * @param conn a connection
 * @param dns a DNS ( a valid ip address)
 * @return Returns 1 if sucess, else 0
 */
int exalt_dbus_dns_delete(exalt_dbus_conn* conn, const char* dns)
{
    DBusMessage *msg;
    DBusMessageIter args;
    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(dns!=NULL);

    msg = exalt_dbus_dns_call_new("delete");
    dbus_message_iter_init_append(msg, &args);

    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &dns),
            dbus_message_unref(msg); return 0);

    EXALT_ASSERT_CUSTOM_RET(e_dbus_message_send(conn->e_conn,msg,_exalt_dbus_dns_del_cb,30,conn),
            dbus_message_unref(msg); return 0);

    return 1;
}

/**
 * @brief Replace a DNS by a new
 * @param conn a connection
 * @param old_dns the old DNS (a valid ip address)
 * @param new_dns the new DNS (a valid ip address)
 * @return Returns 1 if sucess, else 0
 */
int exalt_dbus_dns_replace(exalt_dbus_conn* conn, const char* old_dns, const char* new_dns)
{
    DBusMessage *msg;
    DBusMessageIter args;

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(old_dns!=NULL);
    EXALT_ASSERT_RETURN(new_dns!=NULL);

    msg = exalt_dbus_dns_call_new("replace");
    dbus_message_iter_init_append(msg, &args);

    EXALT_ASSERT_CUSTOM_RET(
            dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &old_dns),
            dbus_message_unref(msg); return 0);

    EXALT_ASSERT_CUSTOM_RET(
            dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &new_dns),
            dbus_message_unref(msg); return 0);

    EXALT_ASSERT_CUSTOM_RET(
            e_dbus_message_send(conn->e_conn,msg,_exalt_dbus_dns_replace_cb,30,conn),
            dbus_message_unref(msg); return 0);

    return 1;
}



/** @} */



void _exalt_dbus_dns_add_cb(void *data, DBusMessage *msg, DBusError *error)
{
    exalt_dbus_conn* conn = (exalt_dbus_conn*)data;

    EXALT_DBUS_ERROR_PRINT(error);

    EXALT_ASSERT_ADV(exalt_dbus_valid_is(msg),
            return ,
            "exalt_dbus_valid_is(msg) failed, error=%d (%s)",
                exalt_dbus_error_get_id(msg),
                exalt_dbus_error_get_msg(msg));

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_DNS_ADD;
    if(conn->response_notify->cb)
        conn-> response_notify -> cb(response,conn->response_notify->user_data);
}

void _exalt_dbus_dns_del_cb(void *data, DBusMessage *msg, DBusError *error)
{
    exalt_dbus_conn* conn = (exalt_dbus_conn*)data;

    EXALT_DBUS_ERROR_PRINT(error);

    EXALT_ASSERT_ADV(exalt_dbus_valid_is(msg),
            return ,
            "exalt_dbus_valid_is(msg) failed, error=%d (%s)",
                exalt_dbus_error_get_id(msg),
                exalt_dbus_error_get_msg(msg));

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_DNS_DEL;
    if(conn->response_notify->cb)
        conn-> response_notify -> cb(response,conn->response_notify->user_data);
}

void _exalt_dbus_dns_replace_cb(void *data, DBusMessage *msg, DBusError *error)
{
    exalt_dbus_conn* conn = (exalt_dbus_conn*)data;

    EXALT_DBUS_ERROR_PRINT(error);

    EXALT_ASSERT_ADV(exalt_dbus_valid_is(msg),
            return ,
            "exalt_dbus_valid_is(msg) failed, error=%d (%s)",
                exalt_dbus_error_get_id(msg),
                exalt_dbus_error_get_msg(msg));

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_DNS_REPLACE;
    if(conn->response_notify->cb)
        conn-> response_notify -> cb(response,conn->response_notify->user_data);
}


void _exalt_dbus_dns_get_list_cb(void *data, DBusMessage *msg, DBusError *error)
{
    exalt_dbus_conn* conn = (exalt_dbus_conn*)data;

    EXALT_DBUS_ERROR_PRINT(error);

    EXALT_ASSERT_ADV(exalt_dbus_valid_is(msg),
            return ,
            "exalt_dbus_valid_is(msg) failed, error=%d (%s)",
                exalt_dbus_error_get_id(msg),
                exalt_dbus_error_get_msg(msg));

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_DNS_GET_LIST;
    response->l = exalt_dbus_response_strings(msg,1);
    if(conn->response_notify->cb)
        conn-> response_notify -> cb(response,conn->response_notify->user_data);
}



