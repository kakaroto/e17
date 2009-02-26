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

DBusMessage * dbus_cb_wireless_essid_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    Exalt_Ethernet* eth;
    char* essid;

    reply = dbus_message_new_method_return(msg);
    dbus_message_set_path(reply,dbus_message_get_path(msg));

    eth= dbus_get_eth(msg);
    EXALT_ASSERT_CUSTOM_RET(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply);

    EXALT_ASSERT_CUSTOM_RET(exalt_eth_wireless_is(eth),
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_NOT_WIRELESS_ERROR_ID,
                EXALT_DBUS_INTERFACE_NOT_WIRELESS_ERROR);
            return reply);

    essid = exalt_wireless_essid_get(exalt_eth_wireless_get(eth));
    EXALT_ASSERT_CUSTOM_RET(essid!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_ESSID_ERROR_ID,
                EXALT_DBUS_ESSID_ERROR);
            return reply);

    dbus_args_valid_append(reply);
    dbus_message_iter_init_append(reply, &args);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &essid),
            EXALT_FREE(essid);return reply);

    EXALT_FREE(essid);
    return reply;
}

DBusMessage * dbus_cb_wireless_wpasupplicant_driver_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    Exalt_Ethernet* eth;
    const char* essid;

    reply = dbus_message_new_method_return(msg);
    dbus_message_set_path(reply,dbus_message_get_path(msg));

    eth= dbus_get_eth(msg);
    EXALT_ASSERT_CUSTOM_RET(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply);

    EXALT_ASSERT_CUSTOM_RET(exalt_eth_wireless_is(eth),
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_NOT_WIRELESS_ERROR_ID,
                EXALT_DBUS_INTERFACE_NOT_WIRELESS_ERROR);
            return reply);

    essid = exalt_wireless_wpasupplicant_driver_get(exalt_eth_wireless_get(eth));
    EXALT_ASSERT_CUSTOM_RET(essid!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_ESSID_ERROR_ID,
                EXALT_DBUS_ESSID_ERROR);
            return reply);

    dbus_args_valid_append(reply);
    dbus_message_iter_init_append(reply, &args);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &essid),
            ;return reply);

    return reply;
}

DBusMessage * dbus_cb_wireless_wpasupplicant_driver_set(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    Exalt_Ethernet* eth;
    const char* driver;

    reply = dbus_message_new_method_return(msg);
    dbus_message_set_path(reply,dbus_message_get_path(msg));
    eth= dbus_get_eth(msg);
    EXALT_ASSERT_CUSTOM_RET(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply);

    EXALT_ASSERT_CUSTOM_RET(exalt_eth_wireless_is(eth),
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_NOT_WIRELESS_ERROR_ID,
                EXALT_DBUS_INTERFACE_NOT_WIRELESS_ERROR);
            return reply);


    //retrieve the driver
    dbus_message_iter_init(msg, &args);

    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_ARGUMENT_NOT_STRING_ID,
                EXALT_DBUS_ARGUMENT_NOT_STRING);
        return reply;
    }
    else
        dbus_message_iter_get_basic(&args, &driver);

    EXALT_ASSERT_CUSTOM_RET(driver!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_ARGUMENT_NOT_STRING_ID,
                EXALT_DBUS_ARGUMENT_NOT_STRING);
            return reply;
            );

    exalt_wireless_wpasupplicant_driver_set(exalt_eth_wireless_get(eth),driver);
    exalt_eth_save(CONF_FILE, eth);

    dbus_args_valid_append(reply);
    return reply;
}



DBusMessage * dbus_cb_wireless_scan(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    Exalt_Ethernet* eth;

    reply = dbus_message_new_method_return(msg);
    dbus_message_set_path(reply,dbus_message_get_path(msg));
    eth= dbus_get_eth(msg);
    EXALT_ASSERT_CUSTOM_RET(eth!=NULL,
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_ERROR_ID,
                EXALT_DBUS_INTERFACE_ERROR);
            return reply);

    EXALT_ASSERT_CUSTOM_RET(exalt_eth_wireless_is(eth),
            dbus_args_error_append(reply,
                EXALT_DBUS_INTERFACE_NOT_WIRELESS_ERROR_ID,
                EXALT_DBUS_INTERFACE_NOT_WIRELESS_ERROR);
            return reply);

    exalt_wireless_scan_start(eth);

    dbus_args_valid_append(reply);
    return reply;
}
