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

int exalt_dbus_msg_id_next(exalt_dbus_conn* conn)
{
    int id = conn->msg_id;
    conn->msg_id =  ((conn->msg_id + 1) % 100 ) ;
    conn->msg_id = (conn->msg_id==0?1:conn->msg_id);
    return id;
}

int exalt_dbus_connection_encaps(Exalt_Connection* c, DBusMessage *msg)
{
    const char* s;
    int i;
    DBusMessageIter args;

    dbus_message_iter_init_append(msg, &args);
    //add the connection
    i=exalt_conn_mode_get(c);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &i),
            dbus_message_unref(msg); return 0);

    if(exalt_conn_mode_get(c)==EXALT_STATIC)
    {
        s = exalt_conn_ip_get(c);
        if(!s)
            s="";
        EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &s),
                dbus_message_unref(msg); return 0);


        s = exalt_conn_netmask_get(c);
        if(!s)
            s="";
        EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &s),
                dbus_message_unref(msg); return 0);

        s = exalt_conn_gateway_get(c);
        if(!s)
            s="";
        EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &s),
                dbus_message_unref(msg); return 0);
    }

    i=exalt_conn_wireless_is(c);
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &i),
            dbus_message_unref(msg); return 0);


    if(exalt_conn_wireless_is(c))
    {
        /*s = exalt_conn_get_essid(c);
          if(!s)
          s="";
          EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &s),
          dbus_message_unref(msg); return 0,
          "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &s");

          i=exalt_conn_get_encryption_mode(c);
          dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &i);

          if(exalt_conn_get_encryption_mode(c)!=EXALT_ENCRYPTION_NONE)
          {
          s = exalt_conn_get_key(c);
          if(!s)
          s="";
          EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &s),
          dbus_message_unref(msg); return 0,
          "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &s");
          }
          i=exalt_conn_get_connection_mode(c);
          EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &i),
          dbus_message_unref(msg); return 0,
          "dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &i");
          i=exalt_conn_get_security_mode(c);
          EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &i),
          dbus_message_unref(msg); return 0,
          "dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &i");
          */
    }

    const char* cmd = exalt_conn_cmd_after_apply_get(c);
    if(!cmd)
        cmd="";
    EXALT_ASSERT_CUSTOM_RET(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &cmd),
            dbus_message_unref(msg); return 0);

    return 1;
}


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
    Eina_List* networks = NULL;
    exalt_dbus_conn *conn;
    DBusMessageIter iter;

    conn = (exalt_dbus_conn*)data;
    EXALT_ASSERT_RETURN_VOID(conn!=NULL);

    EXALT_ASSERT_ADV(exalt_dbus_valid_is(msg),
            ,
            "exalt_dbus_valid_is(msg) failed, error=%d (%s)",
            exalt_dbus_error_get_id(msg),
            exalt_dbus_error_get_msg(msg));

    /*
     * the first argument is the interface name
     * next we have an array of array
        * the second array contains all information
        * about a network (essid, quality ..)
     */
    EXALT_STRDUP(eth,exalt_dbus_response_string(msg,1));

    dbus_message_iter_init(msg, &iter);
    //jump the type of response (valid or error)
    dbus_message_iter_next(&iter);
    //jump the interface name
    dbus_message_iter_next(&iter);

    if (dbus_message_iter_get_arg_type (&iter) == DBUS_TYPE_ARRAY)
    {
        DBusMessageIter iter_array;
        dbus_message_iter_recurse (&iter, &iter_array);

        while (dbus_message_iter_get_arg_type (&iter_array) == DBUS_TYPE_STRUCT)
        {
            char* string;
            int integer;
            Exalt_DBus_Wireless_Network* w;
            DBusMessageIter iter_w;
            DBusMessageIter iter_ie_array;
            DBusMessageIter iter_ie;
            DBusMessageIter iter_integer;
            int i;

            w = exalt_dbus_wireless_network_new();
            networks = eina_list_append(networks,w);
            dbus_message_iter_recurse (&iter_array, &iter_w);

            dbus_message_iter_get_basic(&iter_w, &string);
            exalt_dbus_wireless_network_address_set(w,string);

            dbus_message_iter_next(&iter_w);
            dbus_message_iter_get_basic(&iter_w, &string);
            exalt_dbus_wireless_network_essid_set(w,string);

            dbus_message_iter_next(&iter_w);
            dbus_message_iter_get_basic(&iter_w, &integer);
            exalt_dbus_wireless_network_encryption_set(w,integer);

            dbus_message_iter_next(&iter_w);
            dbus_message_iter_get_basic(&iter_w, &integer);
            exalt_dbus_wireless_network_security_mode_set(w,integer);

            dbus_message_iter_next(&iter_w);
            dbus_message_iter_get_basic(&iter_w, &integer);
            exalt_dbus_wireless_network_quality_set(w,integer);

            dbus_message_iter_next(&iter_w);
            dbus_message_iter_get_basic(&iter_w, &integer);
            exalt_dbus_wireless_network_mode_set(w,integer);

            //the list of WPA IE
            dbus_message_iter_next(&iter_w);
            dbus_message_iter_recurse (&iter_w, &iter_ie_array);

            while (dbus_message_iter_get_arg_type (&iter_ie_array) == DBUS_TYPE_STRUCT)
            {
                Exalt_Wireless_Network_IE* ie =
                    exalt_wireless_network_ie_new();
                Eina_List* l = exalt_dbus_wireless_network_ie_get(w);
                l = eina_list_append(l,ie);
                exalt_dbus_wireless_network_ie_set(w,l);

                dbus_message_iter_recurse (&iter_ie_array, &iter_ie);

                dbus_message_iter_get_basic(&iter_ie, &integer);
                exalt_wireless_network_ie_wpa_type_set(ie,integer);

                dbus_message_iter_next(&iter_ie);
                dbus_message_iter_get_basic(&iter_ie, &integer);
                exalt_wireless_network_ie_wpa_version_set(ie,integer);

                dbus_message_iter_next(&iter_ie);
                dbus_message_iter_get_basic(&iter_ie, &integer);
                exalt_wireless_network_ie_preauth_supported_set(ie,integer);

                dbus_message_iter_next(&iter_ie);
                dbus_message_iter_get_basic(&iter_ie, &integer);
                exalt_wireless_network_ie_group_cypher_set(ie,integer);

                dbus_message_iter_next(&iter_ie);
                dbus_message_iter_get_basic(&iter_ie, &integer);
                exalt_wireless_network_ie_pairwise_cypher_number_set(ie,integer);

                dbus_message_iter_next(&iter_ie);
                dbus_message_iter_recurse (&iter_ie, &iter_integer);
                for(i=0;i<exalt_wireless_network_ie_pairwise_cypher_number_get(ie);i++)
                {
                    dbus_message_iter_get_basic(&iter_integer, &integer);
                    exalt_wireless_network_ie_pairwise_cypher_set(ie,integer,i);
                    dbus_message_iter_next(&iter_integer);
                }

                dbus_message_iter_next(&iter_ie);
                dbus_message_iter_get_basic(&iter_ie, &integer);
                exalt_wireless_network_ie_auth_suites_number_set(ie,integer);

                dbus_message_iter_next(&iter_ie);
                dbus_message_iter_recurse (&iter_ie, &iter_integer);
                for(i=0;i<exalt_wireless_network_ie_auth_suites_number_get(ie);i++)
                {
                    dbus_message_iter_get_basic(&iter_integer, &integer);
                    exalt_wireless_network_ie_auth_suites_set(ie,integer,i);
                    dbus_message_iter_next(&iter_integer);
                }
                dbus_message_iter_next(&iter_ie_array);
            }

            dbus_message_iter_next(&iter_array);
        }
    }

    if(conn->scan_notify->cb)
        conn->scan_notify->cb(eth,networks,conn->scan_notify->user_data);

    EXALT_FREE(eth);
    eina_list_free(networks);
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




const char* dbus_get_eth(DBusMessage* msg)
{
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
