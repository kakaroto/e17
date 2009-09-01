#include "exalt_dbus_network.h"
#include "libexalt_dbus_private.h"


static void _exalt_dbus_network_configuration_get_cb(void *data, DBusMessage *msg, DBusError *error);


/**
 * @brief Get the configuration of a wireless network
 * @param conn a Connection
 * @param essid the wireless network name
 * @return Returns the configuration or NULL
 */
int exalt_dbus_network_configuration_get(Exalt_DBus_Conn *conn, const char *essid)
{
    DBusMessage *msg;
    DBusMessageIter iter;
    Exalt_DBus_Msg_Id *msg_id= malloc(sizeof(Exalt_DBus_Msg_Id));

    EXALT_ASSERT_RETURN(conn!=NULL);

    msg_id->id = exalt_dbus_msg_id_next(conn);
    msg_id->conn = conn;

    msg = exalt_dbus_ifaces_wireless_call_new("network_configuration_get");

    dbus_message_iter_init_append(msg,&iter);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &essid),
            dbus_message_unref(msg);return 0);

    EXALT_ASSERT_CUSTOM_RET(
            e_dbus_message_send (conn->e_conn, msg,_exalt_dbus_network_configuration_get_cb,30,msg_id),
            dbus_message_unref(msg); return 0);

    dbus_message_unref(msg);

    return msg_id->id;
}






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
        exalt_conf_network_login_set(cn, exalt_dbus_response_string(msg, ++i));

        exalt_conf_network_wep_set(cn, exalt_dbus_response_integer(msg, ++i));
        exalt_conf_network_wep_hexa_set(cn, exalt_dbus_response_integer(msg, ++i));

        exalt_conf_network_wpa_set(cn, exalt_dbus_response_integer(msg, ++i));
        exalt_conf_network_wpa_type_set(cn, exalt_dbus_response_integer(msg, ++i));
        exalt_conf_network_group_cypher_set(cn, exalt_dbus_response_integer(msg, ++i));
        exalt_conf_network_pairwise_cypher_set(cn, exalt_dbus_response_integer(msg, ++i));
        exalt_conf_network_auth_suites_set(cn, exalt_dbus_response_integer(msg, ++i));

        exalt_conf_network_favoris_set(cn, exalt_dbus_response_integer(msg, ++i));
        exalt_conf_network_save_when_apply_set(cn, exalt_dbus_response_integer(msg, ++i));
    }

    if(id->conn->response_notify->cb)
        id->conn-> response_notify -> cb(response,id->conn->response_notify->user_data);
    EXALT_FREE(data);
}


