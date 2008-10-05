/*
 * =====================================================================================
 *
 *       Filename:  exalt_dbus_ethernet.c
 *
 *    Description:  all functions about a ethernet interface (no wireless)
 *
 *        Version:  1.0
 *        Created:  08/29/2007 02:19:12 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

/** @file exalt_dbus_ethernet.c */

#include "exalt_dbus_ethernet.h"
#include "libexalt_dbus_private.h"

void _exalt_dbus_eth_ip_get_cb(void *data, DBusMessage *msg, DBusError *error);


/**
 * @addtogroup Ethernet_interface
 * @{
 */

/**
 * @brief Get the ip address of the interface eth
 * @param conn a connection
 * @param eth the interface name (eth0, ath32 ...)
 * @return Returns the ip address
 */
int exalt_dbus_eth_ip_get(exalt_dbus_conn* conn, const char* eth)
{
    DBusMessage *msg;
    char path[PATH_MAX];
    char interface[PATH_MAX];

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);

    snprintf(path,PATH_MAX,"%s/%s",EXALTD_PATH_IFACE,eth);
    snprintf(interface,PATH_MAX,"%s.%s",EXALTD_INTERFACE_IFACE,eth);
    msg = exalt_dbus_iface_call_new("ip_get",path,interface);

    EXALT_ASSERT_CUSTOM_RET(e_dbus_message_send (conn->e_conn, msg, _exalt_dbus_eth_ip_get_cb,30,conn),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return 1;
}

/**
 * @brief Get the netmask of the interface eth
 * @param conn a connection
 * @param eth the interface name (eth0, ath32 ...)
 * @return Returns the netmask
 */
char* exalt_dbus_eth_get_netmask(const exalt_dbus_conn* conn, const char* eth)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    DBusMessageIter args;
    char* res;

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);

    msg = exalt_dbus_read_call_new("IFACE_GET_NETMASK");
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

    EXALT_ASSERT_RETURN(msg!=NULL);

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
 * @brief Get the default gateway of the interface eth
 * @param conn a connection
 * @param eth the interface name (eth0, ath32 ...)
 * @return Returns the default gateway
 */
char* exalt_dbus_eth_get_gateway(const exalt_dbus_conn* conn, const char* eth)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    DBusMessageIter args;
    char* res;

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);

    msg = exalt_dbus_read_call_new("IFACE_GET_GATEWAY");
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

    EXALT_ASSERT_RETURN(msg!=NULL);

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
 * @brief Get the list of interface
 * @param conn a connection
 * @return Returns the list of interface name (char *)
 */
Ecore_List* exalt_dbus_eth_get_list(const exalt_dbus_conn* conn)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    Ecore_List* res;

    EXALT_ASSERT_RETURN(conn!=NULL);

    msg = exalt_dbus_read_call_new("IFACE_GET_ETH_LIST");
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


    res = exalt_dbus_response_strings(msg,1);
    dbus_message_unref(msg);
    return res;
}

/**
 * @brief test if an interface is a wireless interface
 * @param conn a connection
 * @param eth the interface name
 * @return Returns 1 if yes, else 0
 */
int exalt_dbus_eth_is_wireless(const exalt_dbus_conn* conn, const char* eth)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    DBusMessageIter args;
    int res;

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);

    msg = exalt_dbus_read_call_new("IFACE_IS_WIRELESS");
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
    EXALT_ASSERT_RETURN(msg!=NULL);
    dbus_pending_call_unref(ret);

    EXALT_ASSERT_ADV(exalt_dbus_valid_is(msg),
            return 0,
            "exalt_dbus_valid_is(msg) failed, error=%d (%s)",
            exalt_dbus_error_get_id(msg),
            exalt_dbus_error_get_msg(msg));

    res = exalt_dbus_response_boolean(msg,1);
    dbus_message_unref(msg);
    return res;
}


/**
 * @brief test if an interface is up/activate
 * @param conn a connection
 * @param eth the interface name
 * @return Returns 1 if yes, else 0 the interface is down
 */
int exalt_dbus_eth_is_up(const exalt_dbus_conn* conn, const char* eth)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    DBusMessageIter args;
    int res;

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);

    msg = exalt_dbus_read_call_new("IFACE_IS_UP");
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
    EXALT_ASSERT_RETURN(msg!=NULL);
    dbus_pending_call_unref(ret);

    EXALT_ASSERT_ADV(exalt_dbus_valid_is(msg),
            return 0,
            "exalt_dbus_valid_is(msg) failed, error=%d (%s)",
            exalt_dbus_error_get_id(msg),
            exalt_dbus_error_get_msg(msg));

    res = exalt_dbus_response_boolean(msg,1);
    dbus_message_unref(msg);
    return res;
}

/**
 * @brief test if an interface use the dhcp mode (instead of static mode)
 * @param conn a connection
 * @param eth the interface name
 * @return Returns 1 if yes, else 0 the interface use the static mode
 */
int exalt_dbus_eth_is_dhcp(const exalt_dbus_conn* conn, const char* eth)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    DBusMessageIter args;
    int res;

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);

    msg = exalt_dbus_read_call_new("IFACE_IS_DHCP");
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
    EXALT_ASSERT_RETURN(msg!=NULL);
    dbus_pending_call_unref(ret);

    EXALT_ASSERT_ADV(exalt_dbus_valid_is(msg),
            return 0,
            "exalt_dbus_valid_is(msg) failed, error=%d (%s)",
            exalt_dbus_error_get_id(msg),
            exalt_dbus_error_get_msg(msg));

    res = exalt_dbus_response_boolean(msg,1);
    dbus_message_unref(msg);
    return res;
}

/**
 * @brief test if an interface is link,
 * a wireless interface is always link
 * @param conn a connection
 * @param eth the interface name
 * @return Returns 1 if the interface is link or if the interface is wireless, 0 if the interface is unlink
 */
int exalt_dbus_eth_is_link(const exalt_dbus_conn* conn, const char* eth)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    DBusMessageIter args;
    int res;

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);

    msg = exalt_dbus_read_call_new("IFACE_IS_LINK");
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
    EXALT_ASSERT_RETURN(msg!=NULL);
    dbus_pending_call_unref(ret);

    EXALT_ASSERT_ADV(exalt_dbus_valid_is(msg),
            return 0,
            "exalt_dbus_valid_is(msg) failed, error=%d (%s)",
            exalt_dbus_error_get_id(msg),
            exalt_dbus_error_get_msg(msg));

    res = exalt_dbus_response_boolean(msg,1);
    dbus_message_unref(msg);
    return res;
}



/*
 * @brief Up/activate an interface
 * @param conn a connection
 * @param eth the interface name
 * @return Returns 1 if success, else 0
 */
int exalt_dbus_eth_up(const exalt_dbus_conn* conn, const char* eth)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    DBusMessageIter args;

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);

    msg = exalt_dbus_write_call_new("IFACE_UP");
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
 * @brief Down/deactivate an interface
 * @param conn a connection
 * @param eth the interface name
 * @return Returns 1 if sucess, else 0
 */
int exalt_dbus_eth_down(const exalt_dbus_conn* conn, const char* eth)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    DBusMessageIter args;

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);

    msg = exalt_dbus_write_call_new("IFACE_DOWN");
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
 * @brief Apply a connection to the interface.
 * This function ask to the daemon for applying the configuration. When the connection will be apply a message will be notify (see Exalt_Enum_Action)
 * @param conn a connection
 * @param eth the interface name
 * @param c the connection
 * @return Returns 1 if success, else 0
 */
int exalt_dbus_eth_apply_conn(exalt_dbus_conn* conn, const char* eth, Exalt_Connection* c)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    DBusMessageIter args;
    int i;
    const char *s;

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);

    EXALT_ASSERT_RETURN(exalt_conn_is_valid(c));

    msg = exalt_dbus_write_call_new("IFACE_APPLY_CONN");
    dbus_message_iter_init_append(msg, &args);


    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &eth),
            dbus_message_unref(msg); return 0,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &eth");


    //add the connection
    i=exalt_conn_get_mode(c);
    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &i),
            dbus_message_unref(msg); return 0,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &i");


    if(!exalt_conn_is_dhcp(c))
    {
        s = exalt_conn_get_ip(c);
        if(!s)
            s="";
        EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &s),
                dbus_message_unref(msg); return 0,
                "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &s");


        s = exalt_conn_get_netmask(c);
        if(!s)
            s="";
        EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &s),
                dbus_message_unref(msg); return 0,
                "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &s");

        s = exalt_conn_get_gateway(c);
        if(!s)
            s="";
        EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &s),
                dbus_message_unref(msg); return 0,
                "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &s");
    }

    i=exalt_conn_is_wireless(c);
    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &i),
            dbus_message_unref(msg); return 0,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &i");


    if(exalt_conn_is_wireless(c))
    {
        s = exalt_conn_get_essid(c);
        if(!s)
            s="";
        EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &s),
                dbus_message_unref(msg); return 0,
                "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &s");

        i=exalt_conn_get_encryption_mode(c);
        dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &i);

        if(exalt_conn_get_encryption_mode(c)!=EXALT_ENCRYPTION_NONE)
        {
            s = exalt_conn_get_key(c);
            if(!s)
                s="";
            EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &s),
                    dbus_message_unref(msg); return 0,
                    "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &s");
        }
        i=exalt_conn_get_connection_mode(c);
        EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &i),
                dbus_message_unref(msg); return 0,
                "dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &i");
        i=exalt_conn_get_security_mode(c);
        EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &i),
                dbus_message_unref(msg); return 0,
                "dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &i");
    }

    char* cmd = exalt_conn_get_cmd(c);
    if(!cmd)
        cmd="";
    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &cmd),
                    dbus_message_unref(msg); return 0,
                    "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &cmd");


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
 * @brief Get the command which will be run after a connection is applied
 * @param conn a connection
 * @param eth a eth interface name
 * @return Returns the command
 */
char* exalt_dbus_eth_get_cmd(const exalt_dbus_conn* conn, const char* eth)
{
    DBusPendingCall * ret;
    DBusMessage *msg;
    DBusMessageIter args;
    char* res;
    const char* str;

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);


    msg = exalt_dbus_read_call_new("IFACE_CMD_GET");
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

/** @} */





void _exalt_dbus_eth_ip_get_cb(void *data, DBusMessage *msg, DBusError *error)
{
    exalt_dbus_conn* conn = (exalt_dbus_conn*)data;

    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_IFACE_IP_GET;
    response-> iface = strdup(dbus_get_eth(msg));

    if(!exalt_dbus_valid_is(msg))
    {
        response->is_valid = 0;
        response->error_id = exalt_dbus_error_get_id(msg);
        response->error_msg = exalt_dbus_error_get_msg(msg);
    }
    else
    {
        response -> is_valid = 1;
        response-> address = strdup(exalt_dbus_response_string(msg,1));
    }
    if(conn->response_notify->cb)
        conn-> response_notify -> cb(response,conn->response_notify->user_data);
}
