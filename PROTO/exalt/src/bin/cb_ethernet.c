/*
 * =====================================================================================
 *
 *       Filename:  cb_functions.c
 *
 *    Description:  all callback functions are defined here
 *
 *        Version:  1.0
 *        Created:  08/29/2007 01:48:10 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "cb_ethernet.h"

DBusMessage * dbus_cb_eth_list_get(E_DBus_Object *obj , DBusMessage *msg)
{
    return dbus_cb_eth_wireless_list_get(obj,msg,0);
}


DBusMessage * dbus_cb_eth_wireless_list_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg, int is_wireless)
{
    DBusMessage *reply;
    DBusMessageIter iter;
    DBusMessageIter iter_array;
    Exalt_Ethernet* eth;
    const char* interface;
    Eina_List *interfaces,*l;

    reply = dbus_message_new_method_return(msg);

    interfaces = exalt_eth_list_get();
    EXALT_ASSERT_CUSTOM_RET(interfaces!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_LIST_ERROR_ID,
                EXALT_DBUS_INTERFACE_LIST_ERROR);
            return reply);


    dbus_args_valid_append(reply);
    dbus_message_iter_init_append(reply, &iter);
    EXALT_ASSERT_CUSTOM_RET(
            dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY,
                DBUS_TYPE_STRING_AS_STRING, &iter_array),
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_LIST_ERROR_ID,
                EXALT_DBUS_INTERFACE_LIST_ERROR);
            return reply);

    EINA_LIST_FOREACH(interfaces,l,eth)
    {
        if(is_wireless == exalt_eth_wireless_is(eth))
        {
            interface = exalt_eth_name_get(eth);
            EXALT_ASSERT_CUSTOM_RET(interface!=NULL,
                    dbus_args_error_append(reply,
                        EXALT_DBUS_INTERFACE_LIST_ERROR_ID,
                        EXALT_DBUS_INTERFACE_LIST_ERROR);
                    return reply);

            EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&iter_array, DBUS_TYPE_STRING,
                        &interface),
                    dbus_args_error_append(reply,
                        EXALT_DBUS_INTERFACE_LIST_ERROR_ID,
                        EXALT_DBUS_INTERFACE_LIST_ERROR);
                    return reply);
        }
    }
    dbus_message_iter_close_container (&iter, &iter_array);

    return reply;
}

DBusMessage * dbus_cb_eth_ip_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    Exalt_Ethernet* eth;
    char* ip;

    reply = dbus_message_new_method_return(msg);

    dbus_message_set_path(reply,dbus_message_get_path(msg));

    eth= dbus_get_eth(msg);
    EXALT_ASSERT_CUSTOM_RET(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply);

    ip = exalt_eth_ip_get(eth);

    if(!exalt_is_address(ip))
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_IP_ERROR_ID,
                EXALT_DBUS_IP_ERROR);
        return reply;
    }

    dbus_args_valid_append(reply);

    dbus_message_iter_init_append(reply, &args);

    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &ip),
            EXALT_FREE(ip);return reply);


    EXALT_FREE(ip);
    return reply;
}


DBusMessage * dbus_cb_eth_netmask_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    Exalt_Ethernet* eth;
    char* netmask;

    reply = dbus_message_new_method_return(msg);
    dbus_message_set_path(reply,dbus_message_get_path(msg));
    eth= dbus_get_eth(msg);
    EXALT_ASSERT_CUSTOM_RET(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply);

    netmask = exalt_eth_netmask_get(eth);

    if(!exalt_is_address(netmask))
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_NETMASK_ERROR_ID,
                EXALT_DBUS_NETMASK_ERROR);
        return reply;
    }

    dbus_args_valid_append(reply);

    dbus_message_iter_init_append(reply, &args);

    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &netmask),
            EXALT_FREE(netmask);return reply);


    EXALT_FREE(netmask);
    return reply;
}


DBusMessage * dbus_cb_eth_gateway_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    Exalt_Ethernet* eth;
    char* gateway;

    reply = dbus_message_new_method_return(msg);
    dbus_message_set_path(reply,dbus_message_get_path(msg));
    eth= dbus_get_eth(msg);
    EXALT_ASSERT_CUSTOM_RET(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply);

    gateway = exalt_eth_gateway_get(eth);

    if(!exalt_is_address(gateway))
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_GATEWAY_ERROR_ID,
                EXALT_DBUS_GATEWAY_ERROR);
        return reply;
    }

    dbus_args_valid_append(reply);

    dbus_message_iter_init_append(reply, &args);

    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &gateway),
            EXALT_FREE(gateway);return reply);


    EXALT_FREE(gateway);
    return reply;
}

DBusMessage * dbus_cb_eth_wireless_is(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    Exalt_Ethernet* eth;
    int is;

    reply = dbus_message_new_method_return(msg);
    dbus_message_set_path(reply,dbus_message_get_path(msg));
    eth= dbus_get_eth(msg);
    EXALT_ASSERT_CUSTOM_RET(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply);

    is = exalt_eth_wireless_is(eth);

    dbus_args_valid_append(reply);

    dbus_message_iter_init_append(reply, &args);

    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &is),
            return reply);

    return reply;
}

DBusMessage * dbus_cb_eth_link_is(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    Exalt_Ethernet* eth;
    int is;

    reply = dbus_message_new_method_return(msg);
    dbus_message_set_path(reply,dbus_message_get_path(msg));
    eth= dbus_get_eth(msg);
    EXALT_ASSERT_CUSTOM_RET(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply);

    is = exalt_eth_link_is(eth);

    dbus_args_valid_append(reply);

    dbus_message_iter_init_append(reply, &args);

    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &is),
            return reply);

    return reply;
}

DBusMessage * dbus_cb_eth_dhcp_is(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    Exalt_Ethernet* eth;
    int is;

    reply = dbus_message_new_method_return(msg);
    dbus_message_set_path(reply,dbus_message_get_path(msg));
    eth= dbus_get_eth(msg);
    EXALT_ASSERT_CUSTOM_RET(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply);

    is = exalt_eth_dhcp_is(eth);

    dbus_args_valid_append(reply);

    dbus_message_iter_init_append(reply, &args);

    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &is),
            return reply);

    return reply;
}

DBusMessage * dbus_cb_eth_up_is(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    Exalt_Ethernet* eth;
    int is;

    reply = dbus_message_new_method_return(msg);
    dbus_message_set_path(reply,dbus_message_get_path(msg));
    eth= dbus_get_eth(msg);
    EXALT_ASSERT_CUSTOM_RET(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply);

    is = exalt_eth_up_is(eth);

    dbus_args_valid_append(reply);

    dbus_message_iter_init_append(reply, &args);

    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &is),
            return reply);

    return reply;
}



DBusMessage * dbus_cb_eth_up(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    Exalt_Ethernet* eth;

    reply = dbus_message_new_method_return(msg);
    dbus_message_set_path(reply,dbus_message_get_path(msg));

    //search the interface
    eth = dbus_get_eth(msg);
    EXALT_ASSERT_CUSTOM_RET(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply);


    exalt_eth_up_without_apply(eth);
    dbus_args_valid_append(reply);

    return reply;
}

DBusMessage * dbus_cb_eth_down(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    Exalt_Ethernet* eth;


    reply = dbus_message_new_method_return(msg);
    dbus_message_set_path(reply,dbus_message_get_path(msg));

    //search the interface
    eth = dbus_get_eth(msg);
    EXALT_ASSERT_CUSTOM_RET(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply);

    exalt_eth_down(eth);

    dbus_args_valid_append(reply);

    return reply;
}

DBusMessage * dbus_cb_eth_conn_apply(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    Exalt_Ethernet* eth;
    Exalt_Connection* c;

    reply = dbus_message_new_method_return(msg);
    dbus_message_set_path(reply,dbus_message_get_path(msg));

    c = exalt_conn_new();
    EXALT_ASSERT_CUSTOM_RET(c!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_CONN_NEW_ERROR_ID,
                EXALT_DBUS_CONN_NEW_ERROR);
            return reply);

    eth = dbus_get_eth(msg);

    EXALT_ASSERT_CUSTOM_RET(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply);

     if( connection_from_dbusmessage(c,msg,reply) )
         return reply;

    if(!exalt_conn_valid_is(c))
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_CONNECTION_NOT_VALID_ID,
                EXALT_DBUS_CONNECTION_NOT_VALID);
        return reply;
    }
    else
        exalt_eth_conn_apply(eth, c);

    //if it's a wireless connection
    //we save the configuration for the essid
    if(exalt_conn_wireless_is(c))
        exalt_wireless_conn_save(CONF_FILE, c);

    dbus_args_valid_append(reply);

    return reply;
}

DBusMessage * dbus_cb_eth_cmd_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    Exalt_Ethernet* eth;
    const char* cmd;

    reply = dbus_message_new_method_return(msg);

    dbus_message_set_path(reply,dbus_message_get_path(msg));

    eth= dbus_get_eth(msg);
    EXALT_ASSERT_CUSTOM_RET(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply);

    cmd = exalt_conn_cmd_after_apply_get(exalt_eth_connection_get(eth));

    if(!cmd)
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_CMD_ERROR_ID,
                EXALT_DBUS_CMD_ERROR);
        return reply;
    }

    dbus_args_valid_append(reply);

    dbus_message_iter_init_append(reply, &args);

    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &cmd),
            return reply);


    return reply;
}


DBusMessage * dbus_cb_eth_cmd_set(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    Exalt_Ethernet* eth;
    char* cmd;

    reply = dbus_message_new_method_return(msg);

    dbus_message_set_path(reply,dbus_message_get_path(msg));

    eth= dbus_get_eth(msg);
    EXALT_ASSERT_CUSTOM_RET(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply);

    dbus_message_iter_init(msg, &args);
    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_ARGUMENT_NOT_STRING_ID,
                EXALT_DBUS_ARGUMENT_NOT_STRING);
        return reply;
    }
    else
        dbus_message_iter_get_basic(&args, &cmd);

    EXALT_ASSERT_CUSTOM_RET(cmd!=NULL,
         dbus_args_error_append(reply,
                EXALT_DBUS_ARGUMENT_NOT_STRING_ID,
                EXALT_DBUS_ARGUMENT_NOT_STRING);
        return reply;
        );

    exalt_conn_cmd_after_apply_set(exalt_eth_connection_get(eth),cmd);
    exalt_eth_save(CONF_FILE, eth);

    dbus_args_valid_append(reply);

    return reply;
}


