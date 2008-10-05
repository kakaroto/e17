/*
 * =====================================================================================
 *
 *       Filename:  libexalt_dbus_private.c
 *
 *    Description:  private functions/types
 *
 *        Version:  1.0
 *        Created:  09/03/2007 09:35:27 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "libexalt_dbus_private.h"


void _exalt_dbus_notify(void *data, DBusMessage *msg)
{
    char* eth;
    int action;
    exalt_dbus_conn *conn;

    conn = (exalt_dbus_conn*)data;
    EXALT_ASSERT_RETURN_VOID(conn!=NULL);

    EXALT_ASSERT_ADV(exalt_dbus_valid_is(msg),
            ,
            "exalt_dbus_valid_is(msg) failed, error=%d (%s)",
            exalt_dbus_error_get_id(msg),
            exalt_dbus_error_get_msg(msg));

    EXALT_STRDUP(eth , exalt_dbus_response_string(msg,1));
    action = exalt_dbus_response_integer(msg,2);

    if(conn->notify->cb)
        conn->notify->cb(eth,action,conn->notify->user_data);
    EXALT_FREE(eth);
}

void _exalt_dbus_scan_notify(void *data, DBusMessage *msg)
{
    char* eth;
    Ecore_List* networks;
    exalt_dbus_conn *conn;

    conn = (exalt_dbus_conn*)data;
    EXALT_ASSERT_RETURN_VOID(conn!=NULL);

    EXALT_ASSERT_ADV(exalt_dbus_valid_is(msg),
            ,
            "exalt_dbus_valid_is(msg) failed, error=%d (%s)",
            exalt_dbus_error_get_id(msg),
            exalt_dbus_error_get_msg(msg));

    EXALT_STRDUP(eth,exalt_dbus_response_string(msg,1));

    networks = exalt_dbus_response_strings(msg,2);

    if(conn->scan_notify->cb)
        conn->scan_notify->cb(eth,networks,conn->scan_notify->user_data);

    EXALT_FREE(eth);
    ecore_list_destroy(networks);
}



const char* exalt_dbus_response_string(DBusMessage *msg, int pos)
{
    DBusMessageIter args;
    char* res;
    int i;

    if(!dbus_message_iter_init(msg, &args))
    {
        print_error(__FILE__, __func__,__LINE__, "no argument");
        return NULL;
    }

    for(i=0;i<pos;i++)
        dbus_message_iter_next(&args);



    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
    {
        print_error( __FILE__, __func__,__LINE__, "the argument is not a string");
        return NULL;
    }
    else
    {
        dbus_message_iter_get_basic(&args, &res);
        return res;
    }
}

Ecore_List* exalt_dbus_response_strings(DBusMessage *msg, int pos)
{
    DBusMessageIter iter;
    DBusMessageIter iter_array;
    Ecore_List* res;
    char* val;
    int i;

    res = ecore_list_new();
    res ->free_func = ECORE_FREE_CB(exalt_dbus_string_free);

    if(!dbus_message_iter_init(msg, &iter))
    {
        print_error( __FILE__, __func__,__LINE__, "no argument");
        return res;
    }

    for(i=0;i<pos;i++)
        dbus_message_iter_next(&iter);
    if (dbus_message_iter_get_arg_type (&iter) == DBUS_TYPE_ARRAY)
    {
        dbus_message_iter_recurse (&iter, &iter_array);

        while (dbus_message_iter_get_arg_type (&iter_array) == DBUS_TYPE_STRING)
        {
            dbus_message_iter_get_basic(&iter_array, &val);
            ecore_list_append(res,strdup(val));
            dbus_message_iter_next(&iter_array);
        }
    }

    return res;
}

int exalt_dbus_response_boolean(DBusMessage *msg, int pos)
{
    DBusMessageIter args;
    int res;
    int i;

    if(!dbus_message_iter_init(msg, &args))
    {
        print_error( __FILE__, __func__,__LINE__, "no argument");
        return 0;
    }
    for(i=0;i<pos;i++)
        dbus_message_iter_next(&args);


    if (DBUS_TYPE_BOOLEAN != dbus_message_iter_get_arg_type(&args))
    {
        if (DBUS_TYPE_STRING == dbus_message_iter_get_arg_type(&args))
        {
            char* error;
            dbus_message_iter_get_basic(&args, &error);
            print_error( __FILE__, __func__,__LINE__, "%s",error);
        }
        else
            print_error( __FILE__, __func__,__LINE__, "the argument is not a boolean");
        return 0;
    }
    else
    {
        dbus_message_iter_get_basic(&args, &res);
        return res;
    }
}

int exalt_dbus_response_integer(DBusMessage *msg, int pos)
{
    DBusMessageIter args;
    int res;
    int i;

    if(!dbus_message_iter_init(msg, &args))
    {
        print_error( __FILE__, __func__,__LINE__, "no argument");
        return 0;
    }

    for(i=0;i<pos;i++)
        dbus_message_iter_next(&args);

    if (DBUS_TYPE_INT32 != dbus_message_iter_get_arg_type(&args))
    {
        if (DBUS_TYPE_STRING == dbus_message_iter_get_arg_type(&args))
        {
            char* error;
            dbus_message_iter_get_basic(&args, &error);
            print_error( __FILE__, __func__,__LINE__, "%s",error);
        }
        else
            print_error( __FILE__, __func__,__LINE__, "the argument is not an integer");
        return 0;
    }
    else
    {
        dbus_message_iter_get_basic(&args, &res);
        return res;
    }
}


int exalt_dbus_valid_is(DBusMessage *msg)
{
    int id;
    id = exalt_dbus_response_integer(msg,0);
    return id == EXALT_DBUS_VALID;
}

int exalt_dbus_error_get_id(DBusMessage *msg)
{
    int id;
    id = exalt_dbus_response_integer(msg,1);
    return id;
}

const char* exalt_dbus_error_get_msg(DBusMessage *msg)
{
    const char* id;
    id = exalt_dbus_response_string(msg,2);
    return id;
}

void exalt_dbus_string_free(void* data)
{
    EXALT_FREE(data);
}

/**
 * @brief print an error
 * @param file the file
 * @param fct the function
 * @param line the line number
 * @param msg the message
 * @param ... a list of params
 */
void print_error(const char* file,const char* fct, int line, const char* msg, ...)
{
    va_list ap;
    va_start(ap,msg);
    fprintf(stderr,"%s: %s (%d)\n",file,fct,line);
    fprintf(stderr,"\t");
    vfprintf(stderr,msg,ap);
    fprintf(stderr,"\n\n");
    va_end(ap);
}




char* dbus_get_eth(DBusMessage* msg)
{
    Exalt_Ethernet* eth;
    const char* path;
    int i;

    path = dbus_message_get_path(msg);
    for(i=strlen(path)-1;i>0;i--)
    {
        if(path[i-1]=='/')
        {
            return path+i;
        }
    }

    return NULL;
}
