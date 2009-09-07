/** @file exalt_dbus_wireless.c */

#include "exalt_dbus_wireless.h"
#include "libexalt_dbus_private.h"


static void _exalt_dbus_wireless_list_get_cb(void *data, DBusMessage *msg, DBusError *error);
static void _exalt_dbus_wireless_essid_get_cb(void *data, DBusMessage *msg, DBusError *error);
static void _exalt_dbus_wireless_disconnect_cb(void *data, DBusMessage *msg, DBusError *error);
static void _exalt_dbus_wireless_wpasupplicant_driver_get_cb(void *data, DBusMessage *msg, DBusError *error);
static void _exalt_dbus_wireless_wpasupplicant_driver_set_cb(void *data, DBusMessage *msg, DBusError *error);
static void _exalt_dbus_wireless_scan_cb(void *data, DBusMessage *msg, DBusError *error);

/**
 * @addtogroup Wireless_Interface
 * @{
 */


/**
 * @brief Get the list of wireless interface
 * @param conn a Configuration
 * @return Returns the request id
 * @note the response will be EXALT_DBUS_RESPONSE_IFACE_WIRELESS_LIST() and the data will be a list of interface (char*)
 */
int exalt_dbus_wireless_list_get(Exalt_DBus_Conn* conn)
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
 * @brief Start a wireless scan
 * The result will be notify (see exalt_dbus_scan_notify_set())
 * @param conn a Configuration
 * @param iface a wireless interface name
 * @return Returns the request id
 * @note the response will be EXALT_DBUS_RESPONSE_WIRELESS_SCAN() and no data will be set
 */
int exalt_dbus_wireless_scan(Exalt_DBus_Conn* conn, const char* iface)
{
    DBusMessage *msg;
    char path [PATH_MAX];
    char interface [PATH_MAX];

    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(iface!=NULL);

    snprintf(path,PATH_MAX,"%s/%s",EXALTD_PATH_IFACE,iface);
    snprintf(interface,PATH_MAX,"%s.%s",EXALTD_INTERFACE_IFACE,iface);
    msg = exalt_dbus_iface_call_new("scan",path,interface);


    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;

    EXALT_ASSERT_CUSTOM_RET(e_dbus_message_send (conn->e_conn, msg, _exalt_dbus_wireless_scan_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
}

/**
 * @brief Get the current essid of the interface iface
 * @param conn a Configuration
 * @param iface a wireless interface name
 * @return Returns the request id
 * @note the response will be EXALT_DBUS_RESPONSE_WIRELESS_ESSID_GET() and the data will be a string.
 */
int exalt_dbus_wireless_essid_get(Exalt_DBus_Conn* conn, const char* iface)
{
    DBusMessage *msg;
    char path[PATH_MAX];
    char interface[PATH_MAX];
    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(iface!=NULL);

    snprintf(path,PATH_MAX,"%s/%s",EXALTD_PATH_IFACE,iface);
    snprintf(interface,PATH_MAX,"%s.%s",EXALTD_INTERFACE_IFACE,iface);
    msg = exalt_dbus_iface_call_new("essid_get",path,interface);

    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;
    EXALT_ASSERT_CUSTOM_RET(e_dbus_message_send (conn->e_conn, msg, _exalt_dbus_wireless_essid_get_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
}

/**
 * @brief disconnect the wireless interface
 * @param conn a Configuration
 * @param iface a wireless interface name
 * @return Returns the request id
 * @note the response will be EXALT_DBUS_RESPONSE_WIRELESS_DISCONNECT() and no data will be set
 */
int exalt_dbus_wireless_disconnect(Exalt_DBus_Conn* conn, const char* iface)
{
    DBusMessage *msg;
    char path[PATH_MAX];
    char interface[PATH_MAX];
    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(iface!=NULL);

    snprintf(path,PATH_MAX,"%s/%s",EXALTD_PATH_IFACE,iface);
    snprintf(interface,PATH_MAX,"%s.%s",EXALTD_INTERFACE_IFACE,iface);
    msg = exalt_dbus_iface_call_new("disconnect",path,interface);

    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;
    EXALT_ASSERT_CUSTOM_RET(e_dbus_message_send (conn->e_conn, msg, _exalt_dbus_wireless_disconnect_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
}

/**
 * @brief Get the current wpa_supplicant driver used by the interface
 * @param conn a Configuration
 * @param iface a wireless interface name
 * @return Returns the request id
 * @note the response will be EXALT_DBUS_RESPONSE_WIRELESS_WPASUPPLICANT_DRIVER_GET() and the data will be a string
 */
int exalt_dbus_wireless_wpasupplicant_driver_get(Exalt_DBus_Conn* conn, const char* iface)
{
    DBusMessage *msg;
    char path[PATH_MAX];
    char interface[PATH_MAX];
    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(iface!=NULL);

    snprintf(path,PATH_MAX,"%s/%s",EXALTD_PATH_IFACE,iface);
    snprintf(interface,PATH_MAX,"%s.%s",EXALTD_INTERFACE_IFACE,iface);
    msg = exalt_dbus_iface_call_new("wpasupplicant_driver_get",path,interface);

    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;
    EXALT_ASSERT_CUSTOM_RET(e_dbus_message_send (conn->e_conn, msg, _exalt_dbus_wireless_wpasupplicant_driver_get_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
}


/**
 * @brief Set the wpa_supplicant driver used by the interface
 * @param conn a Configuration
 * @param iface a wirelss interface name
 * @param driver a driver (wext, hostap ...)
 * @return Returns the request id
 * @note the response will be EXALT_DBUS_RESPONSE_WIRELESS_WPASUPPLICANT_DRIVER_SET() and no data will be set
 */
int exalt_dbus_wireless_wpasupplicant_driver_set(Exalt_DBus_Conn* conn, const char* eth, const char* driver)
{
    DBusMessage *msg;
    DBusMessageIter iter;
    char path[PATH_MAX];
    char interface[PATH_MAX];
    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);
    EXALT_ASSERT_RETURN(eth!=NULL);
    EXALT_ASSERT_RETURN(driver!=NULL);

    snprintf(path,PATH_MAX,"%s/%s",EXALTD_PATH_IFACE,eth);
    snprintf(interface,PATH_MAX,"%s.%s",EXALTD_INTERFACE_IFACE,eth);
    msg = exalt_dbus_iface_call_new("wpasupplicant_driver_set",path,interface);

    dbus_message_iter_init_append(msg,&iter);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &driver),
            dbus_message_unref(msg);return 0);

    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;
    EXALT_ASSERT_CUSTOM_RET(e_dbus_message_send (conn->e_conn, msg, _exalt_dbus_wireless_wpasupplicant_driver_set_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
}

/** @} */





static void _exalt_dbus_wireless_list_get_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;

    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_IFACE_WIRELESS_LIST;
    response->msg_id = id->id;
    if(!exalt_dbus_valid_is(msg))
    {
        response->is_error = 1;
        response->error_id = exalt_dbus_error_get_id(msg);
        response->error_msg = strdup(exalt_dbus_error_get_msg(msg));
        EXALT_LOG_INFO("%d: %s",
                exalt_dbus_error_get_id(msg),exalt_dbus_error_get_msg(msg));
    }
    else
    {
        response -> is_error = 0;
        response->l = exalt_dbus_response_strings(msg,1);
    }

    if(id->conn->response_notify->cb)
        id->conn-> response_notify -> cb(response,id->conn->response_notify->user_data);
    EXALT_FREE(data);
}


static void _exalt_dbus_wireless_essid_get_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;

    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_WIRELESS_ESSID_GET;
    response-> iface = strdup(dbus_get_eth(msg));
    response->msg_id = id->id;

    if(!exalt_dbus_valid_is(msg))
    {
        response->is_error = 1;
        response->error_id = exalt_dbus_error_get_id(msg);
        response->error_msg = strdup(exalt_dbus_error_get_msg(msg));
        EXALT_LOG_INFO("%d: %s",
                exalt_dbus_error_get_id(msg),exalt_dbus_error_get_msg(msg));
    }
    else
    {
        response -> is_error = 0;
        response-> string = strdup(exalt_dbus_response_string(msg,1));
    }
    if(id->conn->response_notify->cb)
        id->conn-> response_notify -> cb(response,id->conn->response_notify->user_data);
    EXALT_FREE(data);
}



static void _exalt_dbus_wireless_disconnect_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;

    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_WIRELESS_DISCONNECT;
    response-> iface = strdup(dbus_get_eth(msg));
    response->msg_id = id->id;

    if(!exalt_dbus_valid_is(msg))
    {
        response->is_error = 1;
        response->error_id = exalt_dbus_error_get_id(msg);
        response->error_msg = strdup(exalt_dbus_error_get_msg(msg));
        EXALT_LOG_INFO("%d: %s",
                exalt_dbus_error_get_id(msg),exalt_dbus_error_get_msg(msg));
    }
    else
    {
        response -> is_error = 0;
    }
    if(id->conn->response_notify->cb)
        id->conn-> response_notify -> cb(response,id->conn->response_notify->user_data);
    EXALT_FREE(data);
}


static void _exalt_dbus_wireless_wpasupplicant_driver_get_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;

    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_WIRELESS_WPASUPPLICANT_DRIVER_GET;
    response-> iface = strdup(dbus_get_eth(msg));
    response->msg_id = id->id;

    if(!exalt_dbus_valid_is(msg))
    {
        response->is_error = 1;
        response->error_id = exalt_dbus_error_get_id(msg);
        response->error_msg = strdup(exalt_dbus_error_get_msg(msg));
        EXALT_LOG_INFO("%d: %s",
                exalt_dbus_error_get_id(msg),exalt_dbus_error_get_msg(msg));
    }
    else
    {
        response -> is_error = 0;
        response-> string = strdup(exalt_dbus_response_string(msg,1));
    }
    if(id->conn->response_notify->cb)
        id->conn-> response_notify -> cb(response,id->conn->response_notify->user_data);
    EXALT_FREE(data);
}

static void _exalt_dbus_wireless_wpasupplicant_driver_set_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;

    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_WIRELESS_WPASUPPLICANT_DRIVER_SET;
    response-> iface = strdup(dbus_get_eth(msg));
    response->msg_id = id->id;

    if(!exalt_dbus_valid_is(msg))
    {
        response->is_error = 1;
        response->error_id = exalt_dbus_error_get_id(msg);
        response->error_msg = strdup(exalt_dbus_error_get_msg(msg));
        EXALT_LOG_INFO("%d: %s",
                exalt_dbus_error_get_id(msg),exalt_dbus_error_get_msg(msg));
    }
    else
    {
        response -> is_error = 0;
    }
    if(id->conn->response_notify->cb)
        id->conn-> response_notify -> cb(response,id->conn->response_notify->user_data);
    EXALT_FREE(data);
}

static void _exalt_dbus_wireless_scan_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;

    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_WIRELESS_SCAN;
    response-> iface = strdup(dbus_get_eth(msg));
    response->msg_id = id->id;

    if(!exalt_dbus_valid_is(msg))
    {
        response->is_error = 1;
        response->error_id = exalt_dbus_error_get_id(msg);
        response->error_msg = strdup(exalt_dbus_error_get_msg(msg));
        EXALT_LOG_INFO("%d: %s",
                exalt_dbus_error_get_id(msg),exalt_dbus_error_get_msg(msg));
    }
    else
    {
        response -> is_error = 0;
    }
    if(id->conn->response_notify->cb)
        id->conn-> response_notify -> cb(response,id->conn->response_notify->user_data);
    EXALT_FREE(data);
}

