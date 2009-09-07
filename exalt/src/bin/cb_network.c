#include "cb_network.h"

#define EXALT_LOG_DOMAIN exaltd_log_domain

DBusMessage * dbus_cb_network_configuration_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    Exalt_Configuration *c;
    Exalt_Configuration_Network *cn;
    char* essid;
    const char *string;
    int integer;

    reply = dbus_message_new_method_return(msg);
    dbus_message_set_path(reply,dbus_message_get_path(msg));

    dbus_message_iter_init(msg, &args);

    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_ARGUMENT_NOT_STRING_ID,
                EXALT_DBUS_ARGUMENT_NOT_STRING);
        return reply;
    }
    else
        dbus_message_iter_get_basic(&args, &essid);

    EXALT_ASSERT_CUSTOM_RET(essid!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_ARGUMENT_NOT_STRING_ID,
                EXALT_DBUS_ARGUMENT_NOT_STRING);
            return reply;
            );

    c = exalt_conf_network_load(CONF_FILE, essid);
    EXALT_ASSERT_CUSTOM_RET(c!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_NETWORK_UNKNOWN_ID,
                EXALT_DBUS_NETWORK_UNKNOWN);
            return reply;
            );
    cn = exalt_conf_network_get(c);

    dbus_args_valid_append(reply);
    dbus_message_iter_init_append(reply, &args);


    integer = exalt_conf_mode_get(c);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &integer),
            return reply);

    string = exalt_conf_ip_get(c);
    if(!string) string = "";
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &string),
            return reply);

    string = exalt_conf_netmask_get(c);
    if(!string) string = "";
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &string),
            return reply);

    string = exalt_conf_gateway_get(c);
    if(!string) string = "";
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &string),
            return reply);

    string = exalt_conf_cmd_after_apply_get(c);
    if(!string) string = "";
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &string),
            return reply);

    string = exalt_conf_network_essid_get(cn);
    if(!string) string = "";
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &string),
            return reply);

    integer = exalt_conf_network_encryption_is(cn);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &integer),
            return reply);

    integer = exalt_conf_network_mode_get(cn);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &integer),
            return reply);

    string = exalt_conf_network_key_get(cn);
    if(!string) string = "";
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &string),
            return reply);

    integer = exalt_conf_network_wep_is(cn);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &integer),
            return reply);

    integer = exalt_conf_network_wep_hexa_is(cn);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &integer),
            return reply);

    integer = exalt_conf_network_wpa_is(cn);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &integer),
            return reply);

    integer = exalt_conf_network_wpa_type_get(cn);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &integer),
            return reply);

    integer = exalt_conf_network_group_cypher_get(cn);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &integer),
            return reply);

    integer = exalt_conf_network_pairwise_cypher_get(cn);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &integer),
            return reply);

    integer = exalt_conf_network_auth_suites_get(cn);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &integer),
            return reply);

    integer = exalt_conf_network_eap_get(cn);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &integer),
            return reply);

    string = exalt_conf_network_ca_cert_get(cn);
    if(!string) string = "";
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &string),
            return reply);

    string = exalt_conf_network_client_cert_get(cn);
    if(!string) string = "";
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &string),
            return reply);

    string = exalt_conf_network_private_key_get(cn);
    if(!string) string = "";
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &string),
            return reply);

    integer = exalt_conf_network_favoris_is(cn);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &integer),
            return reply);

    integer = exalt_conf_network_save_when_apply_is(cn);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &integer),
            return reply);

    return reply;
}


DBusMessage * dbus_cb_network_list_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args, iter_array;
    Eina_List *l_networks, *l;
    const char *network;

    reply = dbus_message_new_method_return(msg);
    dbus_message_set_path(reply,dbus_message_get_path(msg));

    l_networks = exalt_conf_network_list_load(CONF_FILE);

    dbus_args_valid_append(reply);
    dbus_message_iter_init_append(reply, &args);

    dbus_message_iter_open_container(&args,
                    DBUS_TYPE_ARRAY,
                    "s",
                    &iter_array);

    EINA_LIST_FOREACH(l_networks,l,network)
    {
        if(!network) network = "";
        EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&iter_array, DBUS_TYPE_STRING, &network),
                return reply);
    }

    dbus_message_iter_close_container (&args,&iter_array);

    return reply;
}

DBusMessage * dbus_cb_network_favoris_set(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    const char *essid;
    int favoris;
    Exalt_Configuration *c;
    Exalt_Configuration_Network *cn;

    reply = dbus_message_new_method_return(msg);
    dbus_message_set_path(reply,dbus_message_get_path(msg));


    dbus_message_iter_init(msg, &args);

    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_ARGUMENT_NOT_STRING_ID,
                EXALT_DBUS_ARGUMENT_NOT_STRING);
        return reply;
    }
    else
        dbus_message_iter_get_basic(&args, &essid);

    EXALT_ASSERT_CUSTOM_RET(essid!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_ARGUMENT_NOT_STRING_ID,
                EXALT_DBUS_ARGUMENT_NOT_STRING);
            return reply;
            );

    dbus_message_iter_next(&args);
    if (DBUS_TYPE_INT32 != dbus_message_iter_get_arg_type(&args))
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_ARGUMENT_NOT_INT32_ID,
                EXALT_DBUS_ARGUMENT_NOT_INT32);
        return reply;
    }
    else
        dbus_message_iter_get_basic(&args, &favoris);


    c = exalt_conf_network_load(CONF_FILE, essid);
    EXALT_ASSERT_CUSTOM_RET(c!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_NETWORK_UNKNOWN_ID,
                EXALT_DBUS_NETWORK_UNKNOWN);
            return reply;
            );

    cn = exalt_conf_network_get(c);
    exalt_conf_network_favoris_set(cn, favoris);
    exalt_conf_network_save(CONF_FILE, c);

    dbus_args_valid_append(reply);

    return reply;
}

DBusMessage * dbus_cb_network_delete(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    const char *essid;

    reply = dbus_message_new_method_return(msg);
    dbus_message_set_path(reply,dbus_message_get_path(msg));


    dbus_message_iter_init(msg, &args);

    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_ARGUMENT_NOT_STRING_ID,
                EXALT_DBUS_ARGUMENT_NOT_STRING);
        return reply;
    }
    else
        dbus_message_iter_get_basic(&args, &essid);

    EXALT_ASSERT_CUSTOM_RET(essid!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_ARGUMENT_NOT_STRING_ID,
                EXALT_DBUS_ARGUMENT_NOT_STRING);
            return reply;
            );

    exalt_conf_network_delete(CONF_FILE, essid);

    dbus_args_valid_append(reply);

    return reply;
}
