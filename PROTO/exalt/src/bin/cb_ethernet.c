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

DBusMessage * dbus_cb_eth_get_eth_list(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    Exalt_Ethernet* eth;
    const char* interface;
    void* data;
    Ecore_List *interfaces;

    reply = dbus_message_new_method_return(msg);

    interfaces = exalt_eth_get_list();
    EXALT_ASSERT_ADV(interfaces!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_LIST_ERROR_ID,
                EXALT_DBUS_INTERFACE_LIST_ERROR);
            return reply,
            "interfaces!=NULL failed");


    dbus_args_valid_append(reply);
    dbus_message_iter_init_append(reply, &args);
    ecore_list_first_goto(interfaces);
    while( (data=ecore_list_next(interfaces)))
    {
        eth = data;
        interface = exalt_eth_get_name(eth);
        EXALT_ASSERT_ADV(interface!=NULL,
                return reply,
                "interface!=NULL failed");

        EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &interface),
                return reply,
                "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &interface) failed");

    }
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

    ip = exalt_eth_get_ip(eth);

    EXALT_ASSERT_CUSTOM_RET(ip!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_IP_ERROR_ID,
                EXALT_DBUS_IP_ERROR);
            return reply);

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

    eth= dbus_get_eth(msg);
    EXALT_ASSERT_ADV(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply,
            "eth!=NULL failed");

    netmask = exalt_eth_get_netmask(eth);

    EXALT_ASSERT_ADV(netmask!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_NETMASK_ERROR_ID,
                EXALT_DBUS_NETMASK_ERROR);
            return reply,
            "netmask!=NULL failed");

    dbus_args_valid_append(reply);

    dbus_message_iter_init_append(reply, &args);

    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &netmask),
            EXALT_FREE(netmask);return reply,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &netmask) failed");


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

    eth= dbus_get_eth(msg);
    EXALT_ASSERT_ADV(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply,
            "eth!=NULL failed");

    gateway = exalt_eth_get_gateway(eth);

    EXALT_ASSERT_ADV(gateway!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_GATEWAY_ERROR_ID,
                EXALT_DBUS_GATEWAY_ERROR);
            return reply,
            "gateway!=NULL failed");

    dbus_args_valid_append(reply);

    dbus_message_iter_init_append(reply, &args);

    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &gateway),
            EXALT_FREE(gateway);return reply,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &gateway) failed");


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

    //search the interface
    eth = dbus_get_eth(msg);
    EXALT_ASSERT_ADV(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply,
            "eth!=NULL failed");

    dbus_args_valid_append(reply);

    dbus_message_iter_init_append(reply, &args);
    is = exalt_eth_is_wireless(eth);
    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &is),
            return reply,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_NOOLEAN, &is) failed");

    return reply;
}

DBusMessage * dbus_cb_eth_link_is(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    Exalt_Ethernet* eth;
    int is;


    reply = dbus_message_new_method_return(msg);

    //search the interface
    eth = dbus_get_eth(msg);
    EXALT_ASSERT_ADV(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply,
            "eth!=NULL failed");

    dbus_args_valid_append(reply);

    dbus_message_iter_init_append(reply, &args);
    is = exalt_eth_is_link(eth);
    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &is),
            return reply,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_NOOLEAN, &is) failed");

    return reply;
}

DBusMessage * dbus_cb_eth_dhcp_is(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    Exalt_Ethernet* eth;
    int is;


    reply = dbus_message_new_method_return(msg);

    //search the interface
    eth = dbus_get_eth(msg);
    EXALT_ASSERT_ADV(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply,
            "eth!=NULL failed");

    dbus_args_valid_append(reply);

    dbus_message_iter_init_append(reply, &args);
    is = exalt_eth_is_dhcp(eth);
    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &is),
            return reply,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_NOOLEAN, &is) failed");

    return reply;
}

DBusMessage * dbus_cb_eth_up_is(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    Exalt_Ethernet* eth;
    int is;


    reply = dbus_message_new_method_return(msg);

    //search the interface
    eth = dbus_get_eth(msg);
    EXALT_ASSERT_ADV(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply,
            "eth!=NULL failed");

    dbus_args_valid_append(reply);

    dbus_message_iter_init_append(reply, &args);
    is = exalt_eth_is_up(eth);
    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &is),
            return reply,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_NOOLEAN, &is) failed");

    return reply;
}

DBusMessage * dbus_cb_eth_up(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    Exalt_Ethernet* eth;


    reply = dbus_message_new_method_return(msg);

    //search the interface
    eth = dbus_get_eth(msg);
    EXALT_ASSERT_ADV(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply,
            "eth!=NULL failed");


    exalt_eth_up_without_apply(eth);
    dbus_args_valid_append(reply);

    return reply;
}

DBusMessage * dbus_cb_eth_down(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    Exalt_Ethernet* eth;


    reply = dbus_message_new_method_return(msg);

    //search the interface
    eth = dbus_get_eth(msg);
    EXALT_ASSERT_ADV(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply,
            "eth!=NULL failed");

    exalt_eth_down(eth);

    dbus_args_valid_append(reply);

    return reply;
}

DBusMessage * dbus_cb_eth_conn_apply(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    Exalt_Ethernet* eth;
    Exalt_Connection* c;
    reply = dbus_message_new_method_return(msg);
    int i;
    char* s;

    c = exalt_conn_new();
    EXALT_ASSERT_ADV(c!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_CONN_NEW_ERROR_ID,
                EXALT_DBUS_CONN_NEW_ERROR);
            return reply,
            "c!=NULL failed");


    eth = dbus_get_eth(msg);

    EXALT_ASSERT_ADV(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply,
            "eth!=NULL failed");

    //retrieve the connection
    if(!dbus_message_iter_init(msg, &args))
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_NO_ARGUMENT_ID,
                EXALT_DBUS_NO_ARGUMENT);
        return reply;
    }


    dbus_message_iter_next(&args);
    if (DBUS_TYPE_INT32 != dbus_message_iter_get_arg_type(&args))
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_ARGUMENT_NOT_INT32_ID,
                EXALT_DBUS_ARGUMENT_NOT_INT32);
        return reply;
    }
    else
        dbus_message_iter_get_basic(&args, &i);
    exalt_conn_set_mode(c,i);
    dbus_message_iter_next(&args);

    if(!exalt_conn_is_dhcp(c))
    {
        if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
        {
            dbus_args_error_append(reply,
                    EXALT_DBUS_ARGUMENT_NOT_STRING_ID,
                    EXALT_DBUS_ARGUMENT_NOT_STRING);
            return reply;
        }
        else
            dbus_message_iter_get_basic(&args, &s);
        exalt_conn_set_ip(c,s);
        dbus_message_iter_next(&args);

        if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
        {
            dbus_args_error_append(reply,
                    EXALT_DBUS_ARGUMENT_NOT_STRING_ID,
                    EXALT_DBUS_ARGUMENT_NOT_STRING);
            return reply;
        }
        else
            dbus_message_iter_get_basic(&args, &s);
        exalt_conn_set_netmask(c,s);
        dbus_message_iter_next(&args);

        if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
        {
            dbus_args_error_append(reply,
                    EXALT_DBUS_ARGUMENT_NOT_STRING_ID,
                    EXALT_DBUS_ARGUMENT_NOT_STRING);
            return reply;
        }
        else
            dbus_message_iter_get_basic(&args, &s);
        exalt_conn_set_gateway(c,s);
        dbus_message_iter_next(&args);
    }

    if (DBUS_TYPE_INT32 != dbus_message_iter_get_arg_type(&args))
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_ARGUMENT_NOT_STRING_ID,
                EXALT_DBUS_ARGUMENT_NOT_STRING);
        return reply;
    }
    else
        dbus_message_iter_get_basic(&args, &i);
    exalt_conn_set_wireless(c,i);
    dbus_message_iter_next(&args);

    if(exalt_conn_is_wireless(c))
    {
        if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
        {
            dbus_args_error_append(reply,
                    EXALT_DBUS_ARGUMENT_NOT_STRING_ID,
                    EXALT_DBUS_ARGUMENT_NOT_STRING);
            return reply;
        }
        else
            dbus_message_iter_get_basic(&args, &s);
        exalt_conn_set_essid(c,s);
        dbus_message_iter_next(&args);


        if (DBUS_TYPE_INT32 != dbus_message_iter_get_arg_type(&args))
        {
            dbus_args_error_append(reply,
                    EXALT_DBUS_ARGUMENT_NOT_STRING_ID,
                    EXALT_DBUS_ARGUMENT_NOT_STRING);
            return reply;
        }
        else
            dbus_message_iter_get_basic(&args, &i);
        exalt_conn_set_encryption_mode(c,i);
        dbus_message_iter_next(&args);

        if(exalt_conn_get_encryption_mode(c)!= EXALT_ENCRYPTION_NONE)
        {
            if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
            {
                dbus_args_error_append(reply,
                        EXALT_DBUS_ARGUMENT_NOT_STRING_ID,
                        EXALT_DBUS_ARGUMENT_NOT_STRING);
                return reply;
            }
            else
                dbus_message_iter_get_basic(&args, &s);
            exalt_conn_set_key(c,s);
            dbus_message_iter_next(&args);
        }

        if (DBUS_TYPE_INT32 != dbus_message_iter_get_arg_type(&args))
        {
            dbus_args_error_append(reply,
                    EXALT_DBUS_ARGUMENT_NOT_INT32_ID,
                    EXALT_DBUS_ARGUMENT_NOT_INT32);
            return reply;
        }
        else
            dbus_message_iter_get_basic(&args, &i);
        exalt_conn_set_connection_mode(c,i);
        dbus_message_iter_next(&args);


        if (DBUS_TYPE_INT32 != dbus_message_iter_get_arg_type(&args))
        {
            dbus_args_error_append(reply,
                    EXALT_DBUS_ARGUMENT_NOT_INT32_ID,
                    EXALT_DBUS_ARGUMENT_NOT_INT32);
            return reply;
        }
        else
            dbus_message_iter_get_basic(&args, &i);
        exalt_conn_set_security_mode(c,i);
        dbus_message_iter_next(&args);
    }

    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_ARGUMENT_NOT_STRING_ID,
                EXALT_DBUS_ARGUMENT_NOT_STRING);
        return reply;
    }
    else
        dbus_message_iter_get_basic(&args, &s);
    exalt_conn_set_cmd(c,s);


    //retrieve the connection
    if(!exalt_conn_is_valid(c))
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_CONNECTION_NOT_VALID_ID,
                EXALT_DBUS_CONNECTION_NOT_VALID);
        return reply;
    }
    else
        exalt_eth_apply_conn(eth, c);

    //if it's a wireless connection
    //we save the configuration for the essid
    if(exalt_conn_is_wireless(c))
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

    eth= dbus_get_eth(msg);
    EXALT_ASSERT_ADV(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply,
            "eth!=NULL failed");

    cmd = exalt_conn_get_cmd(exalt_eth_get_connection(eth));
    EXALT_ASSERT_ADV(cmd!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_CMD_ERROR_ID,
                EXALT_DBUS_CMD_ERROR);
            return reply,
            "cmd!=NULL failed");

    dbus_args_valid_append(reply);
    dbus_message_iter_init_append(reply, &args);
    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &cmd),
            return reply,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &cmd) failed");

    return reply;
}

