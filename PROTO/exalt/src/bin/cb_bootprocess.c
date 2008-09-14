/*
 * =====================================================================================
 *
 *       Filename:  cb_bootprocess.c
 *
 *    Description:  dbus callback about the boot process
 *
 *        Version:  1.0
 *        Created:  01/02/2008 06:44:24 PM CET
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */
#include "cb_bootprocess.h"

DBusMessage * dbus_cb_bootprocess_iface_add(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    char* iface;

    reply = dbus_message_new_method_return(msg);

    if(!dbus_message_iter_init(msg, &args))
    {
            dbus_args_error_append(reply,
                    EXALT_DBUS_NO_ARGUMENT_ID,
                    EXALT_DBUS_NO_ARGUMENT);
            return reply;
    }

    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
    {
        dbus_args_error_append(reply,
                    EXALT_DBUS_ARGUMENT_NOT_STRING_ID,
                    EXALT_DBUS_ARGUMENT_NOT_STRING);
            return reply;
    }
    else
        dbus_message_iter_get_basic(&args, &iface);

    waiting_iface_add(iface,CONF_FILE);

    dbus_args_valid_append(reply);

    return reply;
}


DBusMessage * dbus_cb_bootprocess_iface_remove(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    char* iface;

    reply = dbus_message_new_method_return(msg);

    if(!dbus_message_iter_init(msg, &args))
    {
        dbus_args_error_append(reply,
                    EXALT_DBUS_NO_ARGUMENT_ID,
                    EXALT_DBUS_NO_ARGUMENT);
        return reply;
    }
    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
    {
        dbus_args_error_append(reply,
                    EXALT_DBUS_ARGUMENT_NOT_STRING_ID,
                    EXALT_DBUS_ARGUMENT_NOT_STRING);
        return reply;
    }
    else
        dbus_message_iter_get_basic(&args, &iface);

    waiting_iface_remove(iface,CONF_FILE);

    dbus_args_valid_append(reply);

    return reply;
}

DBusMessage * dbus_cb_bootprocess_iface_is(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    char* iface;
    int is =  0;

    reply = dbus_message_new_method_return(msg);

    if(!dbus_message_iter_init(msg, &args))
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_NO_ARGUMENT_ID,
                EXALT_DBUS_NO_ARGUMENT);
        return reply;
    }
    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_ARGUMENT_NOT_STRING_ID,
                EXALT_DBUS_ARGUMENT_NOT_STRING);
        return reply;
    }
    else
        dbus_message_iter_get_basic(&args, &iface);


    is = waiting_iface_is_inconf(iface,CONF_FILE);

    dbus_args_valid_append(reply);
    dbus_message_iter_init_append(reply, &args);

    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &is),
        return reply,
        "dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &is) failed");

    return reply;
}

DBusMessage * dbus_cb_bootprocess_timeout_get(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    int timeout;

    reply = dbus_message_new_method_return(msg);

    timeout = waiting_timeout_get(CONF_FILE);

    dbus_args_valid_append(reply);

    dbus_message_iter_init_append(reply, &args);
    EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &timeout),
        return reply,
        "dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &timeout) failed");

    return reply;
}

DBusMessage * dbus_cb_bootprocess_timeout_set(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
    DBusMessage *reply;
    DBusMessageIter args;
    int timeout;

    reply = dbus_message_new_method_return(msg);

    if(!dbus_message_iter_init(msg, &args))
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_NO_ARGUMENT_ID,
                EXALT_DBUS_NO_ARGUMENT);
        return reply;
    }

    if (DBUS_TYPE_INT32 != dbus_message_iter_get_arg_type(&args))
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_ARGUMENT_NOT_INT32_ID,
                EXALT_DBUS_ARGUMENT_NOT_INT32);
        return reply;
    }
    else
        dbus_message_iter_get_basic(&args, &timeout);

    waiting_timeout_set(timeout,CONF_FILE);

    dbus_args_valid_append(reply);

    return reply;
}
