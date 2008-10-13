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
void _exalt_dbus_eth_gateway_get_cb(void *data, DBusMessage *msg, DBusError *error);
void _exalt_dbus_eth_netmask_get_cb(void *data, DBusMessage *msg, DBusError *error);
void _exalt_dbus_eth_list_get_cb(void *data, DBusMessage *msg, DBusError *error);
void _exalt_dbus_eth_wireless_is_cb(void *data, DBusMessage *msg, DBusError *error);
void _exalt_dbus_eth_up_is_cb(void *data, DBusMessage *msg, DBusError *error);
void _exalt_dbus_eth_link_is_cb(void *data, DBusMessage *msg, DBusError *error);
void _exalt_dbus_eth_dhcp_is_cb(void *data, DBusMessage *msg, DBusError *error);
void _exalt_dbus_eth_cmd_get_cb(void *data, DBusMessage *msg, DBusError *error);
void _exalt_dbus_eth_up_cb(void *data, DBusMessage *msg, DBusError *error);
void _exalt_dbus_eth_down_cb(void *data, DBusMessage *msg, DBusError *error);

void _exalt_dbus_eth_cmd_set_cb(void *data, DBusMessage *msg, DBusError *error);

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
    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);

    snprintf(path,PATH_MAX,"%s/%s",EXALTD_PATH_IFACE,eth);
    snprintf(interface,PATH_MAX,"%s.%s",EXALTD_INTERFACE_IFACE,eth);
    msg = exalt_dbus_iface_call_new("ip_get",path,interface);


    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;
    EXALT_ASSERT_CUSTOM_RET(e_dbus_message_send (conn->e_conn, msg, _exalt_dbus_eth_ip_get_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
}


/**
 * @brief Get the netmask address of the interface eth
 * @param conn a connection
 * @param eth the interface name (eth0, ath32 ...)
 * @return Returns the netmask address
 */
int exalt_dbus_eth_netmask_get(exalt_dbus_conn* conn, const char* eth)
{
    DBusMessage *msg;
    char path[PATH_MAX];
    char interface[PATH_MAX];
    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);

    snprintf(path,PATH_MAX,"%s/%s",EXALTD_PATH_IFACE,eth);
    snprintf(interface,PATH_MAX,"%s.%s",EXALTD_INTERFACE_IFACE,eth);
    msg = exalt_dbus_iface_call_new("netmask_get",path,interface);


    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;
    EXALT_ASSERT_CUSTOM_RET(e_dbus_message_send (conn->e_conn, msg, _exalt_dbus_eth_netmask_get_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
}

/**
 * @brief Get the gateway address of the interface eth
 * @param conn a connection
 * @param eth the interface name (eth0, ath32 ...)
 * @return Returns the gateway address
 */
int exalt_dbus_eth_gateway_get(exalt_dbus_conn* conn, const char* eth)
{
    DBusMessage *msg;
    char path[PATH_MAX];
    char interface[PATH_MAX];
    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);

    snprintf(path,PATH_MAX,"%s/%s",EXALTD_PATH_IFACE,eth);
    snprintf(interface,PATH_MAX,"%s.%s",EXALTD_INTERFACE_IFACE,eth);
    msg = exalt_dbus_iface_call_new("gateway_get",path,interface);


    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;
    EXALT_ASSERT_CUSTOM_RET(e_dbus_message_send (conn->e_conn, msg, _exalt_dbus_eth_gateway_get_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
}

/**
 * @brief Get the list of wired interface
 * @param conn a connection
 * @return Returns the list of interface name (char *)
 */
int exalt_dbus_eth_list_get(exalt_dbus_conn* conn)
{
    DBusMessage *msg;
    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);

    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;

    msg = exalt_dbus_ifaces_wired_call_new("list");

    EXALT_ASSERT_CUSTOM_RET(e_dbus_message_send (conn->e_conn, msg, _exalt_dbus_eth_list_get_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
}

/**
 * @brief test if an interface is a wireless interface
 * @param conn a connection
 * @param eth the interface name
 * @return Returns 1 if yes, else 0
 */
int exalt_dbus_eth_wireless_is(exalt_dbus_conn* conn, const char* eth)
{
    DBusMessage *msg;
    char path[PATH_MAX];
    char interface[PATH_MAX];
    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);

    snprintf(path,PATH_MAX,"%s/%s",EXALTD_PATH_IFACE,eth);
    snprintf(interface,PATH_MAX,"%s.%s",EXALTD_INTERFACE_IFACE,eth);
    msg = exalt_dbus_iface_call_new("wireless_is",path,interface);


    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;
    EXALT_ASSERT_CUSTOM_RET(
            e_dbus_message_send (conn->e_conn, msg, _exalt_dbus_eth_wireless_is_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
}


/**
 * @brief test if an interface is up/activate
 * @param conn a connection
 * @param eth the interface name
 * @return Returns 1 if yes, else 0 the interface is down
 */
int exalt_dbus_eth_up_is(exalt_dbus_conn* conn, const char* eth)
{
    DBusMessage *msg;
    char path[PATH_MAX];
    char interface[PATH_MAX];
    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);

    snprintf(path,PATH_MAX,"%s/%s",EXALTD_PATH_IFACE,eth);
    snprintf(interface,PATH_MAX,"%s.%s",EXALTD_INTERFACE_IFACE,eth);
    msg = exalt_dbus_iface_call_new("up_is",path,interface);


    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;
    EXALT_ASSERT_CUSTOM_RET(
            e_dbus_message_send (conn->e_conn, msg, _exalt_dbus_eth_up_is_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
}

/**
 * @brief test if an interface use the dhcp mode (instead of static mode)
 * @param conn a connection
 * @param eth the interface name
 * @return Returns 1 if yes, else 0 the interface use the static mode
 */
int exalt_dbus_eth_dhcp_is(exalt_dbus_conn* conn, const char* eth)
{
    DBusMessage *msg;
    char path[PATH_MAX];
    char interface[PATH_MAX];
    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);

    snprintf(path,PATH_MAX,"%s/%s",EXALTD_PATH_IFACE,eth);
    snprintf(interface,PATH_MAX,"%s.%s",EXALTD_INTERFACE_IFACE,eth);
    msg = exalt_dbus_iface_call_new("dhcp_is",path,interface);


    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;
    EXALT_ASSERT_CUSTOM_RET(
            e_dbus_message_send (conn->e_conn, msg, _exalt_dbus_eth_dhcp_is_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
}

/**
 * @brief test if an interface is link,
 * a wireless interface is always link
 * @param conn a connection
 * @param eth the interface name
 * @return Returns 1 if the interface is link or if the interface is wireless, 0 if the interface is unlink
 */
int exalt_dbus_eth_link_is(exalt_dbus_conn* conn, const char* eth)
{
    DBusMessage *msg;
    char path[PATH_MAX];
    char interface[PATH_MAX];
    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);

    snprintf(path,PATH_MAX,"%s/%s",EXALTD_PATH_IFACE,eth);
    snprintf(interface,PATH_MAX,"%s.%s",EXALTD_INTERFACE_IFACE,eth);
    msg = exalt_dbus_iface_call_new("link_is",path,interface);


    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;
    EXALT_ASSERT_CUSTOM_RET(
            e_dbus_message_send (conn->e_conn, msg, _exalt_dbus_eth_link_is_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
}



/*
 * @brief Up/activate an interface
 * @param conn a connection
 * @param eth the interface name
 * @return Returns 1 if success, else 0
 */
int exalt_dbus_eth_up(exalt_dbus_conn* conn, const char* eth)
{
    DBusMessage *msg;
    char path[PATH_MAX];
    char interface[PATH_MAX];
    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);

    snprintf(path,PATH_MAX,"%s/%s",EXALTD_PATH_IFACE,eth);
    snprintf(interface,PATH_MAX,"%s.%s",EXALTD_INTERFACE_IFACE,eth);
    msg = exalt_dbus_iface_call_new("up",path,interface);


    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;
    EXALT_ASSERT_CUSTOM_RET(
            e_dbus_message_send (conn->e_conn, msg, _exalt_dbus_eth_up_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
}

/**
 * @brief Down/deactivate an interface
 * @param conn a connection
 * @param eth the interface name
 * @return Returns 1 if sucess, else 0
 */
int exalt_dbus_eth_down(exalt_dbus_conn* conn, const char* eth)
{
    DBusMessage *msg;
    char path[PATH_MAX];
    char interface[PATH_MAX];
    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);

    snprintf(path,PATH_MAX,"%s/%s",EXALTD_PATH_IFACE,eth);
    snprintf(interface,PATH_MAX,"%s.%s",EXALTD_INTERFACE_IFACE,eth);
    msg = exalt_dbus_iface_call_new("down",path,interface);

    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;
    EXALT_ASSERT_CUSTOM_RET(
            e_dbus_message_send (conn->e_conn, msg, _exalt_dbus_eth_down_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
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
int exalt_dbus_eth_command_get(exalt_dbus_conn* conn, const char* eth)
{
    DBusMessage *msg;
    char path[PATH_MAX];
    char interface[PATH_MAX];
    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);

    snprintf(path,PATH_MAX,"%s/%s",EXALTD_PATH_IFACE,eth);
    snprintf(interface,PATH_MAX,"%s.%s",EXALTD_INTERFACE_IFACE,eth);
    msg = exalt_dbus_iface_call_new("command_get",path,interface);


    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;
    EXALT_ASSERT_CUSTOM_RET(e_dbus_message_send (conn->e_conn, msg, _exalt_dbus_eth_cmd_get_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
}


/*
 * update docs!
 */
int exalt_dbus_eth_command_set(exalt_dbus_conn* conn, const char* eth, const char* cmd)
{
    DBusMessage *msg;
    DBusMessageIter iter;
    char path[PATH_MAX];
    char interface[PATH_MAX];
    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);
    EXALT_ASSERT_RETURN(cmd!=NULL);

    snprintf(path,PATH_MAX,"%s/%s",EXALTD_PATH_IFACE,eth);
    snprintf(interface,PATH_MAX,"%s.%s",EXALTD_INTERFACE_IFACE,eth);
    msg = exalt_dbus_iface_call_new("command_set",path,interface);

    dbus_message_iter_init_append(msg,&iter);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &cmd),
            dbus_message_unref(msg);return 0);

    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;
    EXALT_ASSERT_CUSTOM_RET(e_dbus_message_send (conn->e_conn, msg, _exalt_dbus_eth_cmd_set_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
}

/** @} */





void _exalt_dbus_eth_ip_get_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;

    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_IFACE_IP_GET;
    response-> iface = strdup(dbus_get_eth(msg));
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
        response-> address = strdup(exalt_dbus_response_string(msg,1));
    }
    if(id->conn->response_notify->cb)
        id->conn-> response_notify -> cb(response,id->conn->response_notify->user_data);
    EXALT_FREE(data);
}


void _exalt_dbus_eth_netmask_get_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;

    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_IFACE_NETMASK_GET;
    response-> iface = strdup(dbus_get_eth(msg));
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
        response-> address = strdup(exalt_dbus_response_string(msg,1));
    }
    if(id->conn->response_notify->cb)
        id->conn-> response_notify -> cb(response,id->conn->response_notify->user_data);
    EXALT_FREE(data);
}


void _exalt_dbus_eth_gateway_get_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;

    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_IFACE_GATEWAY_GET;
    response-> iface = strdup(dbus_get_eth(msg));
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
        response-> address = strdup(exalt_dbus_response_string(msg,1));
    }
    if(id->conn->response_notify->cb)
        id->conn-> response_notify -> cb(response,id->conn->response_notify->user_data);
    EXALT_FREE(data);
}

void _exalt_dbus_eth_list_get_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;

    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_IFACE_WIRED_LIST;
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

void _exalt_dbus_eth_wireless_is_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;

    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_IFACE_WIRELESS_IS;
    response-> iface = strdup(dbus_get_eth(msg));
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
        response-> is = exalt_dbus_response_boolean(msg,1);
    }
    if(id->conn->response_notify->cb)
        id->conn-> response_notify -> cb(response,id->conn->response_notify->user_data);
    EXALT_FREE(data);
}

void _exalt_dbus_eth_link_is_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;

    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_IFACE_LINK_IS;
    response-> iface = strdup(dbus_get_eth(msg));
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
        response-> is = exalt_dbus_response_boolean(msg,1);
    }
    if(id->conn->response_notify->cb)
        id->conn-> response_notify -> cb(response,id->conn->response_notify->user_data);
    EXALT_FREE(data);
}

void _exalt_dbus_eth_dhcp_is_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;

    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_IFACE_DHCP_IS;
    response-> iface = strdup(dbus_get_eth(msg));
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
        response-> is = exalt_dbus_response_boolean(msg,1);
    }
    if(id->conn->response_notify->cb)
        id->conn-> response_notify -> cb(response,id->conn->response_notify->user_data);
    EXALT_FREE(data);
}

void _exalt_dbus_eth_up_is_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;

    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_IFACE_UP_IS;
    response-> iface = strdup(dbus_get_eth(msg));
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
        response-> is = exalt_dbus_response_boolean(msg,1);
    }
    if(id->conn->response_notify->cb)
        id->conn-> response_notify -> cb(response,id->conn->response_notify->user_data);
    EXALT_FREE(data);
}


void _exalt_dbus_eth_cmd_get_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;

    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_IFACE_CMD_GET;
    response-> iface = strdup(dbus_get_eth(msg));
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
        response-> command = strdup(exalt_dbus_response_string(msg,1));
    }
    if(id->conn->response_notify->cb)
        id->conn-> response_notify -> cb(response,id->conn->response_notify->user_data);
    EXALT_FREE(data);
}

void _exalt_dbus_eth_cmd_set_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;

    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_IFACE_CMD_SET;
    response-> iface = strdup(dbus_get_eth(msg));
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
    }
    if(id->conn->response_notify->cb)
        id->conn-> response_notify -> cb(response,id->conn->response_notify->user_data);
    EXALT_FREE(data);
}






void _exalt_dbus_eth_up_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;

    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_IFACE_UP;
    response-> iface = strdup(dbus_get_eth(msg));
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
    }
    if(id->conn->response_notify->cb)
        id->conn-> response_notify -> cb(response,id->conn->response_notify->user_data);
    EXALT_FREE(data);
}

void _exalt_dbus_eth_down_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;

    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_IFACE_DOWN;
    response-> iface = strdup(dbus_get_eth(msg));
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
    }
    if(id->conn->response_notify->cb)
        id->conn-> response_notify -> cb(response,id->conn->response_notify->user_data);
    EXALT_FREE(data);
}


