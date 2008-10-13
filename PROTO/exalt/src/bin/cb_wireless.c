/*
 * =====================================================================================
 *
 *       Filename:  cb_wireless.c
 *
 *    Description:  All CB about a wireless interface
 *
 *        Version:  1.0
 *        Created:  09/01/2007 12:05:24 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "cb_wireless.h"

DBusMessage * dbus_cb_wireless_list_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    return dbus_cb_eth_wireless_list_get(obj,msg,1);
}

DBusMessage * dbus_cb_wireless_scan_wait(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    Exalt_Ethernet* eth;
    Ecore_List* l;
    Exalt_Wireless *w;
    void* data;
    char* essid;
    Exalt_Wireless_Network* wi;
    reply = dbus_message_new_method_return(msg);

    eth = dbus_get_eth(msg);
    EXALT_ASSERT_ADV(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply,
            "eth!=NULL failed");

    EXALT_ASSERT_ADV(exalt_eth_is_wireless(eth),
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_NOT_WIRELESS_ERROR_ID,
                EXALT_DBUS_INTERFACE_NOT_WIRELESS_ERROR);
            return reply,
            "exalt_eth_is_wireless(eth) failed");

    w = exalt_eth_get_wireless(eth);

    dbus_args_valid_append(reply);
    dbus_message_iter_init_append(reply, &args);

    exalt_wireless_scan_wait(eth);
    l = exalt_wireless_get_networks_list(w);

    // by bentejuy
    EXALT_ASSERT_RETURN(l!=NULL);

    ecore_list_first_goto(l);

    while( (data=ecore_list_next(l)))
    {
        wi = Exalt_Wireless_Network(data);
        essid = strdup(exalt_wirelessnetwork_get_essid(wi));
        EXALT_ASSERT_ADV(essid!=NULL,
                return reply,
                "essid!=NULL failed");
        EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &essid),
                return reply,
                "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &essid) failed");
        EXALT_FREE(essid);
    }
    return reply;
}

DBusMessage * dbus_cb_wireless_get_essid(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    Exalt_Ethernet* eth;
    char* essid;

    reply = dbus_message_new_method_return(msg);

    eth= dbus_get_eth(msg);
    EXALT_ASSERT_ADV(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply,
            "eth!=NULL failed");

    EXALT_ASSERT_ADV(exalt_eth_is_wireless(eth),
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_NOT_WIRELESS_ERROR_ID,
                EXALT_DBUS_INTERFACE_NOT_WIRELESS_ERROR);
            return reply,
            "exalt_eth_is_wireless(eth) failed");


    essid = exalt_wireless_get_essid(exalt_eth_get_wireless(eth));
    EXALT_ASSERT_ADV(essid!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_ESSID_ERROR_ID,
                EXALT_DBUS_ESSID_ERROR);
            return reply,
            "essid!=NULL failed");

    dbus_args_valid_append(reply);
    dbus_message_iter_init_append(reply, &args);
    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &essid),
            EXALT_FREE(essid);return reply,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &essid) failed");

    EXALT_FREE(essid);
    return reply;
}

DBusMessage * dbus_cb_wireless_get_wpasupplicant_driver(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    Exalt_Ethernet* eth;
    const char* driver;

    reply = dbus_message_new_method_return(msg);

    eth= dbus_get_eth(msg);
    EXALT_ASSERT_ADV(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply,
            "eth!=NULL failed");

    EXALT_ASSERT_ADV(exalt_eth_is_wireless(eth),
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_NOT_WIRELESS_ERROR_ID,
                EXALT_DBUS_INTERFACE_NOT_WIRELESS_ERROR);
            return reply,
            "exalt_eth_is_wireless(eth) failed");


    driver = exalt_wireless_get_wpasupplicant_driver(exalt_eth_get_wireless(eth));
    EXALT_ASSERT_ADV(driver!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_DRIVER_ERROR_ID,
                EXALT_DBUS_DRIVER_ERROR);
            return reply,
            "driver!=NULL failed");


    dbus_args_valid_append(reply);
    dbus_message_iter_init_append(reply, &args);
    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &driver),
            return reply,
            "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &driver) failed");

    return reply;
}

DBusMessage * dbus_cb_wireless_set_wpasupplicant_driver(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    Exalt_Ethernet* eth;
    const char* driver;

    reply = dbus_message_new_method_return(msg);

    eth= dbus_get_eth(msg);
    EXALT_ASSERT_ADV(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply,
            "eth!=NULL failed");

    EXALT_ASSERT_ADV(exalt_eth_is_wireless(eth),
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_NOT_WIRELESS_ERROR_ID,
                EXALT_DBUS_INTERFACE_NOT_WIRELESS_ERROR);
            return reply,
            "exalt_eth_is_wireless(eth) failed");


    //retrieve the driver
    if(!dbus_message_iter_init(msg, &args))
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_NO_ARGUMENT_ID,
                EXALT_DBUS_NO_ARGUMENT);
        return reply;
    }

    dbus_message_iter_next(&args);
    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_ARGUMENT_NOT_STRING_ID,
                EXALT_DBUS_ARGUMENT_NOT_STRING);
        return reply;
    }
    else
        dbus_message_iter_get_basic(&args, &driver);

    exalt_wireless_set_wpasupplicant_driver(exalt_eth_get_wireless(eth),driver);

    dbus_args_valid_append(reply);
    return reply;
}



DBusMessage * dbus_cb_wireless_scan_start(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
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

    EXALT_ASSERT_ADV(exalt_eth_is_wireless(eth),
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_NOT_WIRELESS_ERROR_ID,
                EXALT_DBUS_INTERFACE_NOT_WIRELESS_ERROR);
            return reply,
            "exalt_eth_is_wireless(eth) failed");


    exalt_wireless_scan_start(eth);
    dbus_args_valid_append(reply);

    return reply;
}

