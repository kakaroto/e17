/*
 * =====================================================================================
 *
 *       Filename:  daemon.c
 *
 *    Description:  defines globals funtions
 *
 *        Version:  1.0
 *        Created:  08/28/2007 04:27:45 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "daemon.h"

//list of dbus object, 1 per interface
Eina_List* dbus_object_list = NULL;



void setup_del_iface(E_DBus_Connection *conn __UNUSED__,Exalt_Ethernet* eth)
{
    Eina_List *l;
    DBus_Object_Item *dbus_item;

    EINA_LIST_FOREACH(dbus_object_list,l,dbus_item)
        if(strcmp(exalt_eth_name_get(eth),dbus_item->iface)==0)
            break;
    e_dbus_object_free(dbus_item->o);

    dbus_object_list = eina_list_remove(dbus_object_list, dbus_item);
    EXALT_FREE(dbus_item->iface);
    EXALT_FREE(dbus_item);
}

void setup_new_iface(E_DBus_Connection *conn,Exalt_Ethernet* eth)
{
    char bpath[PATH_MAX];
    char binterface[PATH_MAX];
    E_DBus_Object *obj;
    E_DBus_Interface *iface;

    snprintf(bpath,PATH_MAX,"%s/%s",EXALTD_PATH_IFACE,exalt_eth_name_get(eth));
    obj = e_dbus_object_add(conn, bpath, NULL);

    snprintf(binterface,PATH_MAX,"%s.%s",EXALTD_INTERFACE_IFACE,exalt_eth_name_get(eth));
    iface = e_dbus_interface_new(binterface);


    if(exalt_eth_wireless_is(eth))
    {
        e_dbus_interface_method_add(iface, "essid_get", NULL, "s", dbus_cb_wireless_essid_get);
        e_dbus_interface_method_add(iface, "wpasupplicant_driver_get", NULL, "s", dbus_cb_wireless_wpasupplicant_driver_get);
        e_dbus_interface_method_add(iface, "wpasupplicant_driver_set", "s", NULL, dbus_cb_wireless_wpasupplicant_driver_set);
        e_dbus_interface_method_add(iface, "scan", NULL, NULL, dbus_cb_wireless_scan);
    }


    e_dbus_interface_method_add(iface, "up", NULL, NULL, dbus_cb_eth_up);
    e_dbus_interface_method_add(iface, "down", NULL, NULL, dbus_cb_eth_down);

    e_dbus_interface_method_add(iface, "ip_get", NULL, "s", dbus_cb_eth_ip_get);

    e_dbus_interface_method_add(iface, "netmask_get", NULL, "s", dbus_cb_eth_netmask_get);

    e_dbus_interface_method_add(iface, "gateway_get", NULL, "s", dbus_cb_eth_gateway_get);

    e_dbus_interface_method_add(iface, "link_is", NULL, "b", dbus_cb_eth_link_is);
    e_dbus_interface_method_add(iface, "up_is", NULL, "b", dbus_cb_eth_up_is);
    e_dbus_interface_method_add(iface, "dhcp_is", NULL, "b", dbus_cb_eth_dhcp_is);
    e_dbus_interface_method_add(iface, "wireless_is", NULL, "b", dbus_cb_eth_wireless_is);
    e_dbus_interface_method_add(iface, "command_set", NULL, "s", dbus_cb_eth_cmd_set);
    e_dbus_interface_method_add(iface, "command_get", NULL, "s", dbus_cb_eth_cmd_get);
    e_dbus_interface_method_add(iface, "apply", NULL, NULL, dbus_cb_eth_conn_apply);

    e_dbus_object_interface_attach(obj, iface);

    DBus_Object_Item* dbus_item = calloc(1,sizeof(DBus_Object_Item));
    dbus_item->o = obj;
    dbus_item->iface = strdup(exalt_eth_name_get(eth));
    dbus_object_list = eina_list_append(dbus_object_list,dbus_item);
}

int setup(E_DBus_Connection *conn)
{
    E_DBus_Object *obj;
    E_DBus_Interface *iface;
    e_dbus_request_name(conn, EXALTD_SERVICE , 0, NULL, NULL);

    obj = e_dbus_object_add(conn, EXALTD_PATH_DNS, NULL);
    iface = e_dbus_interface_new(EXALTD_INTERFACE_DNS);
    e_dbus_interface_method_add(iface, "get", NULL, "a(s)", dbus_cb_dns_list_get);
    e_dbus_interface_method_add(iface, "add", "s", NULL, dbus_cb_dns_add);
    e_dbus_interface_method_add(iface, "replace", "ss", NULL, dbus_cb_dns_replace);
    e_dbus_interface_method_add(iface, "delete", "s", NULL, dbus_cb_dns_delete);
    e_dbus_object_interface_attach(obj, iface);

    obj = e_dbus_object_add(conn, EXALTD_PATH_WIREDS, NULL);
    iface = e_dbus_interface_new(EXALTD_INTERFACE_WIREDS);
    e_dbus_interface_method_add(iface, "list", "", "a(s)", dbus_cb_eth_list_get);
    e_dbus_object_interface_attach(obj, iface);


    obj = e_dbus_object_add(conn, EXALTD_PATH_WIRELESSS, NULL);
    iface = e_dbus_interface_new(EXALTD_INTERFACE_WIRELESSS);
    e_dbus_interface_method_add(iface, "list", "", "a(s)", dbus_cb_wireless_list_get);
    e_dbus_object_interface_attach(obj, iface);



    return 1;
}


int main(int argc, char** argv)
{
    int daemon = 1;
    FILE *fp;
    int size;
    char buf[PATH_MAX];

    argc--;
    argv++;
    while(argc)
    {
        if(strcmp(*argv, "--nodaemon")==0)
        {
            daemon = 0;
        }
        else if(strcmp(*argv,"--help")==0)
        {
            printf("Usage exalt-daemon [OPTION]\n" \
                    "--nodaemon    doesn't run as a daemon\n" \
                    "--help        display this help and exit\n");
            exit(1);
        }
        argc--;
        argv++;
    }

    if(daemon)
    {
        //redirect stderr and stdout >> /var/log/exald.log
        remove(EXALTD_LOGFILE);
        if ((fp = fopen(EXALTD_LOGFILE, "w+")))
        {
            stderr = fp;
            stdout = fp;
        }
        else
            print_error(__FILE__,__func__, __LINE__,"Can not create the log file: %s\n",EXALTD_LOGFILE);
    }

    e_dbus_init();
    ecore_init();
    exalt_init();

    if(!exalt_is_admin())
    {
        print_error(__FILE__,__func__, __LINE__,"Please run as root. \n");
        e_dbus_shutdown();
        ecore_shutdown();
        return 1;
    }


    exaltd_conn = e_dbus_bus_get(DBUS_BUS_SYSTEM);
    if(!exaltd_conn)
    {
        print_error(__FILE__,__func__, __LINE__,"main(): can not exaltd_connect to DBUS, maybe the daemon is not launch ?\n");
        e_dbus_shutdown();
        ecore_shutdown();
        return -1;
    }

    setup(exaltd_conn);

    exalt_eth_cb_set(eth_cb,exaltd_conn);
    exalt_eth_scan_cb_set(wireless_scan_cb,exaltd_conn);

    exalt_main();

    if(daemon)
    {
        //if we need waiting 1 or more card
        waiting_iface_list = NULL;
        waiting_iface_timer = NULL;
        waiting_iface_list = waiting_iface_load(CONF_FILE);
        if ( waiting_iface_list->l )
        {
            //start the timer for the timeout
            waiting_iface_timer = ecore_timer_add(waiting_iface_list->timeout, waiting_iface_stop, waiting_iface_list);
            ecore_main_loop_begin();  // <-- mal optimizado ?
            // I need an ecore_loop for ecore_timer :)
            // this part is very strange I know, maybe I ll rewrite it

            waiting_iface_list = NULL;
            waiting_iface_timer = NULL;

        }
        else
        {
            EXALT_FREE(waiting_iface_list);
        }
        //all waiting card are set (or timeout)
        //we create the child and then quit
        if(fork()!=0)
            exit(0);

        //create the pid file
        size = snprintf(buf, PATH_MAX, "%d\n", getpid());
        if ((fp = fopen(EXALTD_PIDFILE, "w+")))
        {
            int ret;
            ret = fwrite(buf, sizeof(char), size, fp);
            fclose(fp);
        }
        else
            print_error(__FILE__,__func__, __LINE__, "Can not create the pid file: %s\n", EXALTD_PIDFILE);
    }

    ecore_main_loop_begin();
    e_dbus_shutdown();
    ecore_shutdown();
    return 1;
}

void eth_cb(Exalt_Ethernet* eth, Exalt_Enum_Action action, void* data)
{
    E_DBus_Connection *conn;
    DBusMessage* msg;
    DBusMessageIter args;
    const char* name;
    char* str;

    conn = (E_DBus_Connection*) data;

    if(action == EXALT_ETH_CB_ACTION_NEW || action == EXALT_ETH_CB_ACTION_ADD)
    {
        setup_new_iface(conn,eth);

        //first we load the driver if eth is a wireless connection
        if(exalt_eth_wireless_is(eth))
        {
            Exalt_Wireless* w = exalt_eth_wireless_get(eth);
            str = exalt_eth_driver_load(CONF_FILE,exalt_eth_udi_get(eth));
            if(!str)
                str=strdup("wext");
            exalt_wireless_wpasupplicant_driver_set(w,str);
            EXALT_FREE(str);
        }
        //then we load the command which will be run after a configuration is applied

        //apply or not apply a connection ?
        //load a connection
        //if no connection is load, we create one
        //if we didn't have load a connection or if the card is save as "up"
        //  if is not up
        //      we up the card
        //  we wait than the card is up (with a timeout of 5 ms)
        //  if the card is link
        //      we apply the connection
        //else
        //  we down the card

        Exalt_Connection *c = exalt_eth_conn_load(CONF_FILE, exalt_eth_udi_get(eth));
        short not_c = 0;
        if(!c)
        {
            c = exalt_conn_new();
            not_c = 1;
        }
        exalt_eth_connection_set(eth,c);

        if(not_c || exalt_eth_state_load(CONF_FILE, exalt_eth_udi_get(eth)) == EXALT_UP)
        {
            int i = 0;
            if(!exalt_eth_up_is(eth))
                exalt_eth_up(eth);
            while(!exalt_eth_up_is(eth) && i<10)
            {
                usleep(500);
                i++;
            }

            if(exalt_eth_link_is(eth) && exalt_eth_up_is(eth))
                exalt_eth_conn_apply(eth, c);
        }
        else
        {
            exalt_eth_down(eth);
        }
    }

    if(action == EXALT_ETH_CB_ACTION_REMOVE)
        setup_del_iface(conn,eth);

    if ( action == EXALT_ETH_CB_ACTION_LINK && exalt_eth_up_is(eth))
    {
        Exalt_Connection *c = exalt_eth_conn_load(CONF_FILE, exalt_eth_udi_get(eth));
        if(!c)
            c = exalt_conn_new();
        if(exalt_eth_wireless_is(eth))
            exalt_conn_wireless_set(c, 1);
        else
            exalt_conn_wireless_set(c, 0);

        exalt_eth_conn_apply(eth, c);
        exalt_eth_save(CONF_FILE,eth);
    }


    if ( action == EXALT_ETH_CB_ACTION_UP && exalt_eth_link_is(eth) && (time(NULL) - exalt_eth_dontapplyafterup_get(eth)>2) )
    {
        Exalt_Connection *c = exalt_eth_conn_load(CONF_FILE, exalt_eth_udi_get(eth));
        if(!c)
            c = exalt_conn_new();
        if(exalt_eth_wireless_is(eth))
            exalt_conn_wireless_set(c, 1);
        else
            exalt_conn_wireless_set(c, 0);

        exalt_eth_conn_apply(eth, c);
        exalt_eth_save(CONF_FILE,eth);
    }

    if (action == EXALT_ETH_CB_ACTION_UNLINK || action == EXALT_ETH_CB_ACTION_DOWN)
        //remove the default gateway
        exalt_eth_gateway_delete(eth);

    if( action == EXALT_ETH_CB_ACTION_UP || action == EXALT_ETH_CB_ACTION_DOWN)
        exalt_eth_save(CONF_FILE, eth);


    if( action==EXALT_ETH_CB_ACTION_CONN_APPLY_DONE)
    {
        //printf("apply DONE !!!\n");
        //save the new configuration
        exalt_eth_save(CONF_FILE, eth);
    }

    //if(action == EXALT_ETH_CB_ACTION_CONN_APPLY_START)
    //    printf("apply start !!\n");

    //waiting card
    if(!waiting_iface_is_done(waiting_iface_list) && waiting_iface_is(waiting_iface_list, eth) && action == EXALT_ETH_CB_ACTION_ADDRESS_NEW )
    {
        const char *ip = exalt_eth_ip_get(eth);

        if(ip && strcmp(ip,"0.0.0.0") != 0)
        {
            waiting_iface_done(waiting_iface_list, eth);
            if(waiting_iface_is_done(waiting_iface_list))
            {
                //stop the timeout
                EXALT_DELETE_TIMER(waiting_iface_timer);
                waiting_iface_stop(waiting_iface_list);
            }
        }
    }

    //send a broadcast
    msg = dbus_message_new_signal(EXALTD_PATH_NOTIFY,EXALTD_INTERFACE_NOTIFY, "notify");
    EXALT_ASSERT_RETURN_VOID(msg!=NULL);

    name = exalt_eth_name_get(eth);
    EXALT_ASSERT_RETURN_VOID(name!=NULL);

    dbus_args_valid_append(msg);

    dbus_message_iter_init_append(msg, &args);
    EXALT_ASSERT(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &name));
    EXALT_ASSERT(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &action));

    e_dbus_message_send(conn, msg, NULL, 3,NULL);
    dbus_message_unref(msg);
}

void wireless_scan_cb(Exalt_Ethernet* eth,Eina_List* networks, void* data)
{
    E_DBus_Connection *conn;
    DBusMessage* msg;
    DBusMessageIter args;
    DBusMessageIter iter_array;
    const char* name;

    Exalt_Wireless_Network *wi;
    Eina_List* l;

    conn = (E_DBus_Connection*) data;
    EXALT_ASSERT_RETURN_VOID(conn!=NULL);

    //send a broadcast
    msg = dbus_message_new_signal(EXALTD_PATH_NOTIFY,EXALTD_INTERFACE_NOTIFY, "scan_notify");
    EXALT_ASSERT_RETURN_VOID(msg!=NULL);

    name = exalt_eth_name_get(eth);
    EXALT_ASSERT_ADV(name!=NULL,
            dbus_message_unref(msg);return,
            "name!=NULL failed");

    dbus_args_valid_append(msg);

    dbus_message_iter_init_append(msg, &args);
    EXALT_ASSERT_RETURN_VOID(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &name));


    if(networks!=NULL)
    {
        EXALT_ASSERT_RETURN_VOID(dbus_message_iter_open_container(&args,
                    DBUS_TYPE_ARRAY,
                    "(ssisiia(siiiiiaiiai))",
                    &iter_array));

        EINA_LIST_FOREACH(networks, l, wi)
        {
            int integer;
            const char* string;
            DBusMessageIter iter_w;
            DBusMessageIter iter_integer;
            DBusMessageIter iter_array_ie;
            DBusMessageIter iter_ie;
            int i;
            Eina_List* l_ie,*l_ie1;
            Exalt_Wireless_Network_IE* ie;

            EXALT_ASSERT_RETURN_VOID(
                    dbus_message_iter_open_container(&iter_array,
                        DBUS_TYPE_STRUCT,
                        NULL,
                        &iter_w));

            //add the address
            string = exalt_wireless_network_address_get(wi);
            if(!string)
                string="";
            EXALT_ASSERT_CUSTOM_RET(
                    dbus_message_iter_append_basic(&iter_w,
                        DBUS_TYPE_STRING,
                        &string),
                    dbus_message_unref(msg);return );

            //add the essid
            string = exalt_wireless_network_essid_get(wi);
            if(!string)
                string="";
            EXALT_ASSERT_CUSTOM_RET(
                    dbus_message_iter_append_basic(&iter_w,
                        DBUS_TYPE_STRING,
                        &string),
                    dbus_message_unref(msg);return );

            //add the encryption (yes or no)
            integer = exalt_wireless_network_encryption_is(wi);
            EXALT_ASSERT_CUSTOM_RET(
                    dbus_message_iter_append_basic(&iter_w,
                        DBUS_TYPE_INT32,
                        &integer),
                    dbus_message_unref(msg);return );

            //add the description
            string = exalt_wireless_network_description_get(wi);
            if(!string)
                string="";
            EXALT_ASSERT_CUSTOM_RET(
                    dbus_message_iter_append_basic(&iter_w,
                        DBUS_TYPE_STRING,
                        &string),
                    dbus_message_unref(msg);return );


            //add the quality
            integer = exalt_wireless_network_quality_get(wi);
            EXALT_ASSERT_CUSTOM_RET(
                    dbus_message_iter_append_basic(&iter_w,
                        DBUS_TYPE_INT32,
                        &integer),
                    dbus_message_unref(msg);return );

            integer = exalt_wireless_network_ie_choice_get(wi);
            EXALT_ASSERT_CUSTOM_RET(
                    dbus_message_iter_append_basic(&iter_w,
                        DBUS_TYPE_INT32,
                        &integer),
                    dbus_message_unref(msg);return );


            EXALT_ASSERT_RETURN_VOID(dbus_message_iter_open_container(&iter_w,
                        DBUS_TYPE_ARRAY,
                        "(siiiiiaiiai)",
                        &iter_array_ie));

            l_ie = exalt_wireless_network_ie_get(wi);
            EINA_LIST_FOREACH(l_ie,l_ie1,ie)
            {
                EXALT_ASSERT_RETURN_VOID(
                        dbus_message_iter_open_container(&iter_array_ie,
                            DBUS_TYPE_STRUCT,
                            NULL,
                            &iter_ie));

                //the description
                string = exalt_wireless_network_ie_description_get(ie);
                if(!string)
                    string="";
                EXALT_ASSERT_CUSTOM_RET(
                        dbus_message_iter_append_basic(&iter_ie,
                            DBUS_TYPE_STRING,
                            &string),
                        dbus_message_unref(msg);return );

                integer = exalt_wireless_network_ie_auth_choice_get(ie);
                EXALT_ASSERT_CUSTOM_RET(
                        dbus_message_iter_append_basic(&iter_ie,
                            DBUS_TYPE_INT32,
                            &integer),
                        dbus_message_unref(msg);return );

                integer =
                    exalt_wireless_network_ie_pairwise_choice_get(ie);
                EXALT_ASSERT_CUSTOM_RET(
                        dbus_message_iter_append_basic(&iter_ie,
                            DBUS_TYPE_INT32,
                            &integer),
                        dbus_message_unref(msg);return );


                //the wpa type
                integer = exalt_wireless_network_ie_wpa_type_get(ie);
                EXALT_ASSERT_CUSTOM_RET(
                        dbus_message_iter_append_basic(&iter_ie,
                            DBUS_TYPE_INT32,
                            &integer),
                        dbus_message_unref(msg);return );


                //the cypher group
                integer = exalt_wireless_network_ie_group_cypher_get(ie);
                EXALT_ASSERT_CUSTOM_RET(
                        dbus_message_iter_append_basic(&iter_ie,
                            DBUS_TYPE_INT32,
                            &integer),
                        dbus_message_unref(msg);return );


                //the number of pairwise cypher
                integer = exalt_wireless_network_ie_pairwise_cypher_number_get(ie);
                EXALT_ASSERT_CUSTOM_RET(
                        dbus_message_iter_append_basic(&iter_ie,
                            DBUS_TYPE_INT32,
                            &integer),
                        dbus_message_unref(msg);return );

                //add the pairwise cypher list
                EXALT_ASSERT_RETURN_VOID(
                        dbus_message_iter_open_container(&iter_ie,
                            DBUS_TYPE_ARRAY,
                            "i",
                            &iter_integer));

                for(i=0;i<exalt_wireless_network_ie_pairwise_cypher_number_get(ie);i++)
                {
                    integer = exalt_wireless_network_ie_pairwise_cypher_get(ie,i);
                    EXALT_ASSERT_CUSTOM_RET(
                            dbus_message_iter_append_basic(&iter_integer,
                                DBUS_TYPE_INT32,
                                &integer),
                            dbus_message_unref(msg);return );
                }
                dbus_message_iter_close_container (&iter_ie,&iter_integer);

                //the number of auth suites
                integer = exalt_wireless_network_ie_auth_suites_number_get(ie);
                EXALT_ASSERT_CUSTOM_RET(
                        dbus_message_iter_append_basic(&iter_ie,
                            DBUS_TYPE_INT32,
                            &integer),
                        dbus_message_unref(msg);return );

                //add the auth suites list
                EXALT_ASSERT_RETURN_VOID(
                        dbus_message_iter_open_container(&iter_ie,
                            DBUS_TYPE_ARRAY,
                            "i",
                            &iter_integer));

                for(i=0;i<exalt_wireless_network_ie_auth_suites_number_get(ie);i++)
                {
                    integer = exalt_wireless_network_ie_auth_suites_get(ie,i);
                    EXALT_ASSERT_CUSTOM_RET(
                            dbus_message_iter_append_basic(&iter_integer,
                                DBUS_TYPE_INT32,
                                &integer),
                            dbus_message_unref(msg);return );
                }
                dbus_message_iter_close_container (&iter_ie,&iter_integer);

                dbus_message_iter_close_container (&iter_array_ie,&iter_ie);
            }
            dbus_message_iter_close_container (&iter_w,&iter_array_ie);
            dbus_message_iter_close_container (&iter_array,&iter_w);
        }
        dbus_message_iter_close_container (&args,&iter_array);
    }
    e_dbus_message_send(conn, msg, NULL, -1,NULL);
    dbus_message_unref(msg);
}


Exalt_Ethernet* dbus_get_eth(DBusMessage* msg)
{
    Exalt_Ethernet* eth;
    const char* path;
    int i;

    path = dbus_message_get_path(msg);
    for(i=strlen(path)-1;i>0;i--)
    {
        if(path[i-1]=='/')
        {
            eth = exalt_eth_get_ethernet_byname(path+i);
            return eth;
        }
    }

    return NULL;
}

Exalt_Wireless_Network* dbus_get_wirelessnetwork(DBusMessage* msg)
{
    DBusMessageIter args;
    char* essid = NULL;
    Exalt_Ethernet* eth;
    Exalt_Wireless_Network* wi;

    if(!dbus_message_iter_init(msg, &args))
        return NULL;

    //search the interface
    if(!(eth = dbus_get_eth(msg)))
        return NULL;

    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
        return NULL;
    else
        dbus_message_iter_get_basic(&args, &essid);

    //search the interface
    wi = get_wirelessnetwork(eth,essid);

    return wi;
}

Exalt_Wireless_Network* get_wirelessnetwork(Exalt_Ethernet* eth, char* essid)
{
    Exalt_Wireless* w;
    Exalt_Wireless_Network* wi;
    Eina_List *networks,*l;

    EXALT_ASSERT_RETURN(eth!=NULL);
    EXALT_ASSERT_RETURN(essid!=NULL);

    EXALT_ASSERT_RETURN(exalt_eth_wireless_is(eth)!=0);
    w = exalt_eth_wireless_get(eth);

    networks = exalt_wireless_networks_get(w);

    EINA_LIST_FOREACH(networks,l,wi)
    {
        if(strcmp(essid,exalt_wireless_network_essid_get(wi))==0 )
            return wi;
    }
    return NULL;
}


DBusMessage* connection_from_dbusmessage(Exalt_Connection* c,Exalt_Ethernet* eth,DBusMessage *msg,DBusMessage *reply)
{
    DBusMessageIter args;
    int i;
    char* s;

    dbus_message_iter_init(msg, &args);

    if (DBUS_TYPE_INT32 != dbus_message_iter_get_arg_type(&args))
    {
        dbus_args_error_append(reply,
                EXALT_DBUS_ARGUMENT_NOT_INT32_ID,
                EXALT_DBUS_ARGUMENT_NOT_INT32);
        return reply;
    }
    else
        dbus_message_iter_get_basic(&args, &i);
    exalt_conn_mode_set(c,i);
    dbus_message_iter_next(&args);

    if(exalt_conn_mode_get(c)==EXALT_STATIC)
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
        exalt_conn_ip_set(c,s);
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
        exalt_conn_netmask_set(c,s);
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
        exalt_conn_gateway_set(c,s);
        printf("gateway: %s\n",s);
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
    exalt_conn_wireless_set(c,i);
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
    exalt_conn_cmd_after_apply_set(c,s);

    if(exalt_conn_wireless_is(c))
    {
        char* string;
        int integer;
        Exalt_Wireless_Network* w;
        DBusMessageIter iter_w;
        DBusMessageIter iter_ie_array;
        DBusMessageIter iter_ie;
        DBusMessageIter iter_integer;
        int i;

        w = exalt_wireless_network_new(exalt_eth_wireless_get(eth));
        exalt_conn_network_set(c,w);

        dbus_message_iter_next(&args);
        dbus_message_iter_get_basic(&args, &string);
        exalt_conn_login_set(c,string);

        dbus_message_iter_next(&args);
        dbus_message_iter_get_basic(&args, &string);
        exalt_conn_key_set(c,string);

        dbus_message_iter_next(&args);
        dbus_message_iter_get_basic(&args, &integer);
        exalt_conn_wep_key_hexa_set(c,integer);

        dbus_message_iter_next(&args);
        dbus_message_iter_recurse (&args, &iter_w);

        dbus_message_iter_get_basic(&iter_w, &string);
        exalt_wireless_network_address_set(w,string);

        dbus_message_iter_next(&iter_w);
        dbus_message_iter_get_basic(&iter_w, &string);
        exalt_wireless_network_essid_set(w,string);

        dbus_message_iter_next(&iter_w);
        dbus_message_iter_get_basic(&iter_w, &integer);
        exalt_wireless_network_encryption_set(w,integer);

        dbus_message_iter_next(&iter_w);
        dbus_message_iter_get_basic(&iter_w, &string);
        exalt_wireless_network_description_set(w,string);

        dbus_message_iter_next(&iter_w);
        dbus_message_iter_get_basic(&iter_w, &integer);
        exalt_wireless_network_quality_set(w,integer);

        dbus_message_iter_next(&iter_w);
        dbus_message_iter_get_basic(&iter_w, &integer);
        exalt_wireless_network_ie_choice_set(w,integer);


        //the list of WPA IE
        dbus_message_iter_next(&iter_w);
        dbus_message_iter_recurse (&iter_w, &iter_ie_array);

        while (dbus_message_iter_get_arg_type (&iter_ie_array) == DBUS_TYPE_STRUCT)
        {
            Exalt_Wireless_Network_IE* ie =
                exalt_wireless_network_ie_new();
            Eina_List* l = exalt_wireless_network_ie_get(w);
            l = eina_list_append(l,ie);
            exalt_wireless_network_ie_set(w,l);

            dbus_message_iter_recurse (&iter_ie_array, &iter_ie);

            dbus_message_iter_get_basic(&iter_ie, &string);
            exalt_wireless_network_ie_description_set(ie,string);

            dbus_message_iter_next(&iter_ie);
            dbus_message_iter_get_basic(&iter_ie, &integer);
            exalt_wireless_network_ie_auth_choice_set(ie,integer);

            dbus_message_iter_next(&iter_ie);
            dbus_message_iter_get_basic(&iter_ie, &integer);
            exalt_wireless_network_ie_pairwise_choice_set(ie,integer);

            dbus_message_iter_next(&iter_ie);
            dbus_message_iter_get_basic(&iter_ie, &integer);
            exalt_wireless_network_ie_wpa_type_set(ie,integer);

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


    }


    return NULL;
}

int dbus_args_error_append(DBusMessage *msg, int id_error, const char* error)
{
    DBusMessageIter args;
    int err = EXALT_DBUS_ERROR;

    EXALT_ASSERT_RETURN(msg!=NULL);
    EXALT_ASSERT_RETURN(error!=NULL);

    dbus_message_iter_init_append(msg, &args);
    dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &err);
    dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &id_error);
    dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &error);

    return 1;
}

int dbus_args_valid_append(DBusMessage *msg)
{
    DBusMessageIter args;
    int err = EXALT_DBUS_VALID;

    dbus_message_iter_init_append(msg, &args);
    dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &err);

    return 1;
}
