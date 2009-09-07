/** @file exalt_dbus_network.c */

#include "exalt_dbus_network.h"
#include "libexalt_dbus_private.h"


static void _exalt_dbus_network_configuration_get_cb(void *data, DBusMessage *msg, DBusError *error);
static void _exalt_dbus_network_list_get_cb(void *data, DBusMessage *msg, DBusError *error);
static void _exalt_dbus_network_favoris_set_cb(void *data, DBusMessage *msg, DBusError *error);
static void _exalt_dbus_network_delete_cb(void *data, DBusMessage *msg, DBusError *error);

/**
 * @addtogroup Network
 * @{
 */



/**
 * @brief Get the configuration of a wireless network
 * @param conn a Connection
 * @param essid the wireless network name
 * @return Returns the request id
 * @note the response will be EXALT_DBUS_RESPONSE_NETWORK_CONFIGURATION_GET() and the data will be a configuration
 */
int exalt_dbus_network_configuration_get(Exalt_DBus_Conn *conn, const char *essid)
{
    DBusMessage *msg;
    DBusMessageIter iter;
    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);

    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;

    msg = exalt_dbus_ifaces_network_call_new("configuration_get");

    dbus_message_iter_init_append(msg,&iter);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &essid),
            dbus_message_unref(msg);return 0);

    EXALT_ASSERT_CUSTOM_RET(
            e_dbus_message_send (conn->e_conn, msg,_exalt_dbus_network_configuration_get_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
}

/**
 * @brief Get the list of wireless network configuration saved
 * @param conn a Connection
 * @return Returns the request id
 * @note the response will be EXALT_DBUS_RESPONSE_NETWORK_LIST_GET() and the data will be a list of essid (char*)
 */
int exalt_dbus_network_list_get(Exalt_DBus_Conn *conn)
{
    DBusMessage *msg;
    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);

    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;

    msg = exalt_dbus_ifaces_network_call_new("list");

    EXALT_ASSERT_CUSTOM_RET(
            e_dbus_message_send (conn->e_conn, msg,_exalt_dbus_network_list_get_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
}


/**
 * @brief Set if a configuration is favorite or not
 * @param conn a Connection
 * @param essid the wireless network name
 * @param favoris 1 or 0
 * @return Returns the request id
 * @note the response will be EXALT_DBUS_RESPONSE_NETWORK_FAVORIS_SET() and no data will be set
 */
int exalt_dbus_network_favoris_set(Exalt_DBus_Conn *conn, const char *essid, int favoris)
{
    DBusMessage *msg;
    DBusMessageIter iter;
    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);

    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;

    msg = exalt_dbus_ifaces_network_call_new("favoris_set");

    dbus_message_iter_init_append(msg,&iter);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &essid),
            dbus_message_unref(msg);return 0);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &favoris),
            dbus_message_unref(msg);return 0);

    EXALT_ASSERT_CUSTOM_RET(
            e_dbus_message_send (conn->e_conn, msg,_exalt_dbus_network_favoris_set_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
}


/**
 * @brief Delete the configuration of a wireless network
 * @param conn a Connection
 * @param essid the wireless network name
 * @return Returns the request id
 * @note the response will be EXALT_DBUS_RESPONSE_NETWORK_DELETE() and no data will be set
 */
int exalt_dbus_network_delete(Exalt_DBus_Conn *conn, const char *essid)
{
    DBusMessage *msg;
    DBusMessageIter iter;
    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);

    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;

    msg = exalt_dbus_ifaces_network_call_new("delete");

    dbus_message_iter_init_append(msg,&iter);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &essid),
            dbus_message_unref(msg);return 0);

    EXALT_ASSERT_CUSTOM_RET(
            e_dbus_message_send (conn->e_conn, msg,_exalt_dbus_network_delete_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
}

/** @} */


static void _exalt_dbus_network_configuration_get_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;
    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_NETWORK_CONFIGURATION_GET;
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
        int i = 0;
        response -> is_error = 0;
        Exalt_Configuration *c = exalt_conf_new();
        Exalt_Configuration_Network *cn = exalt_conf_network_new();
        exalt_conf_wireless_set(c, 1);
        exalt_conf_network_set(c,cn);
        response->c = c;

        exalt_conf_mode_set(c, exalt_dbus_response_integer(msg, ++i));
        exalt_conf_ip_set(c, exalt_dbus_response_string(msg, ++i));
        exalt_conf_netmask_set(c, exalt_dbus_response_string(msg, ++i));
        exalt_conf_gateway_set(c, exalt_dbus_response_string(msg, ++i));
        exalt_conf_cmd_after_apply_set(c, exalt_dbus_response_string(msg, ++i));

        exalt_conf_network_essid_set(cn, exalt_dbus_response_string(msg, ++i));
        exalt_conf_network_encryption_set(cn, exalt_dbus_response_integer(msg, ++i));
        exalt_conf_network_mode_set(cn, exalt_dbus_response_integer(msg, ++i));
        exalt_conf_network_key_set(cn, exalt_dbus_response_string(msg, ++i));

        exalt_conf_network_wep_set(cn, exalt_dbus_response_integer(msg, ++i));
        exalt_conf_network_wep_hexa_set(cn, exalt_dbus_response_integer(msg, ++i));

        exalt_conf_network_wpa_set(cn, exalt_dbus_response_integer(msg, ++i));
        exalt_conf_network_wpa_type_set(cn, exalt_dbus_response_integer(msg, ++i));
        exalt_conf_network_group_cypher_set(cn, exalt_dbus_response_integer(msg, ++i));
        exalt_conf_network_pairwise_cypher_set(cn, exalt_dbus_response_integer(msg, ++i));
        exalt_conf_network_auth_suites_set(cn, exalt_dbus_response_integer(msg, ++i));
        exalt_conf_network_eap_set(cn, exalt_dbus_response_integer(msg, ++i));
        exalt_conf_network_ca_cert_set(cn, exalt_dbus_response_string(msg, ++i));
        exalt_conf_network_client_cert_set(cn, exalt_dbus_response_string(msg, ++i));
        exalt_conf_network_private_key_set(cn, exalt_dbus_response_string(msg, ++i));

        exalt_conf_network_favoris_set(cn, exalt_dbus_response_integer(msg, ++i));
        exalt_conf_network_save_when_apply_set(cn, exalt_dbus_response_integer(msg, ++i));
    }

    if(id->conn->response_notify->cb)
        id->conn-> response_notify -> cb(response,id->conn->response_notify->user_data);
    EXALT_FREE(data);
}

static void _exalt_dbus_network_list_get_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;
    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_NETWORK_LIST_GET;
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
        DBusMessageIter iter, iter_array;
        Eina_List *l = NULL;
        response -> is_error = 0;
        const char* string;

        dbus_message_iter_init(msg, &iter);
        dbus_message_iter_next(&iter);
        dbus_message_iter_recurse (&iter, &iter_array);

        while (dbus_message_iter_get_arg_type (&iter_array) == DBUS_TYPE_STRING)
        {
            dbus_message_iter_get_basic(&iter_array, &string);
            l = eina_list_append(l, string);
            dbus_message_iter_next(&iter_array);
        }
        response->l = l;
    }

    if(id->conn->response_notify->cb)
        id->conn-> response_notify -> cb(response,id->conn->response_notify->user_data);
    EXALT_FREE(data);
}

static void _exalt_dbus_network_favoris_set_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;
    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_NETWORK_FAVORIS_SET;
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

static void _exalt_dbus_network_delete_cb(void *data, DBusMessage *msg, DBusError *error)
{
    Exalt_DBus_Msg_Id *id = data;
    EXALT_DBUS_ERROR_PRINT(error);

    Exalt_DBus_Response* response = calloc(1,sizeof(Exalt_DBus_Response));
    response->type = EXALT_DBUS_RESPONSE_NETWORK_DELETE;
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

