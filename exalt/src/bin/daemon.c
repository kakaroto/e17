#include "daemon.h"
#include <Ecore_Getopt.h>
#include "config.h"

#define EXALT_LOG_DOMAIN exaltd_log_domain

int exaltd_log_domain;

//list of dbus object, 1 per interface
Eina_List *dbus_object_list = NULL;

//list of interface which we search a favorite network
//When the daemon is start, we launch a wireless network scan on each interface
//and when we get the result we connect the interface to a favorite detected network
//This list is used to tell: we search a favorite network for this interface
//elt : Exalt_Ethernet *
Eina_List *iface_wireless_search_favorite = NULL;


static const Ecore_Getopt options = {
    "exalt",
    NULL,
    VERSION,
    "(C) 2009 Exalt, see AUTHORS.",
    "LGPL with advertisement, see COPYING",
    "Launch exalt, a network manager daemon\n\n",
        1,
        {
            ECORE_GETOPT_VERSION('V', "version"),
            ECORE_GETOPT_COPYRIGHT('R', "copyright"),
            ECORE_GETOPT_LICENSE('L', "license"),
            ECORE_GETOPT_STORE_BOOL('d', "nodaemon", "Do not run the daemon in the background"),
            ECORE_GETOPT_STORE_STR('l', "logfile", "Specify the log file. Default : "EXALTD_LOGFILE),
            ECORE_GETOPT_STORE_STR('p', "pidfile", "Specify the pid file. Default : "EXALTD_PIDFILE),
            ECORE_GETOPT_HELP('h', "help"),
            ECORE_GETOPT_SENTINEL
        }
};

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
        e_dbus_interface_method_add(iface, "disconnect", NULL, "s", dbus_cb_wireless_disconnect);
        e_dbus_interface_method_add(iface, "wpasupplicant_driver_get", NULL, "s", dbus_cb_wireless_wpasupplicant_driver_get);
        e_dbus_interface_method_add(iface, "wpasupplicant_driver_set", "s", NULL, dbus_cb_wireless_wpasupplicant_driver_set);
        e_dbus_interface_method_add(iface, "scan", NULL, NULL, dbus_cb_wireless_scan);
    }


    e_dbus_interface_method_add(iface, "connected_is", NULL, "b", dbus_cb_eth_connected_is);
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
    e_dbus_interface_method_add(iface, "apply", NULL, NULL, dbus_cb_eth_conf_apply);

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
    e_dbus_interface_method_add(iface, "all_disconnected", "", "b", dbus_cb_eth_all_disconnected_is);
    e_dbus_object_interface_attach(obj, iface);


    obj = e_dbus_object_add(conn, EXALTD_PATH_WIRELESSS, NULL);
    iface = e_dbus_interface_new(EXALTD_INTERFACE_WIRELESSS);
    e_dbus_interface_method_add(iface, "list", "", "a(s)", dbus_cb_wireless_list_get);
    e_dbus_object_interface_attach(obj, iface);


    obj = e_dbus_object_add(conn, EXALTD_PATH_NETWORK, NULL);
    iface = e_dbus_interface_new(EXALTD_INTERFACE_NETWORK);
    e_dbus_interface_method_add(iface, "list", "", "as", dbus_cb_network_list_get);
    e_dbus_interface_method_add(iface, "configuration_get", "s", "isssssiissiiiiiiii", dbus_cb_network_configuration_get);
    e_dbus_interface_method_add(iface, "favoris_set", "si", "", dbus_cb_network_favoris_set);
    e_dbus_interface_method_add(iface, "delete", "s", "", dbus_cb_network_delete);
    e_dbus_object_interface_attach(obj, iface);

    return 1;
}


int main(int argc, char** argv)
{
    unsigned char nodaemon = 0;
    unsigned char exit_option = 0;
    char *logfile = EXALTD_LOGFILE;
    char *pidfile = EXALTD_PIDFILE;
    FILE *fp;
    int size;
    char buf[PATH_MAX];

    e_dbus_init();
    ecore_init();
    exalt_init();
    exaltd_log_domain = eina_log_domain_register("EXALT-DAEMON",EINA_COLOR_RED);

    //ecore_getopt
    Ecore_Getopt_Value values[] = {
        ECORE_GETOPT_VALUE_BOOL(exit_option),
        ECORE_GETOPT_VALUE_BOOL(exit_option),
        ECORE_GETOPT_VALUE_BOOL(exit_option),
        ECORE_GETOPT_VALUE_BOOL(nodaemon),
        ECORE_GETOPT_VALUE_STR(logfile),
        ECORE_GETOPT_VALUE_STR(pidfile),
        ECORE_GETOPT_VALUE_BOOL(exit_option),
    };
    ecore_app_args_set(argc, (const char **) argv);
    int nonargs = ecore_getopt_parse(&options, values, argc, argv);
    if (nonargs < 0)
        return 1;
    else if (nonargs != argc)
    {
        fputs("Invalid non-option argument", stderr);
        ecore_getopt_help(stderr, &options);
        return 1;
    }

    if(exit_option)
        return 0;
    //

    if(!nodaemon)
    {
        //redirect stderr and stdout >> logfile
        remove(logfile);
        if ((fp = fopen(logfile, "w+")))
        {
            stderr = fp;
            stdout = fp;
        }
        else
            EXALT_LOG_WARN("Can not create the log file: %s\n",logfile);
    }

    if(!exalt_admin_is())
    {
        EXALT_LOG_CRIT("Please run as root.");
        e_dbus_shutdown();
        ecore_shutdown();
        return 1;
    }


    exaltd_conn = e_dbus_bus_get(DBUS_BUS_SYSTEM);
    if(!exaltd_conn)
    {
        EXALT_LOG_CRIT("Can not connect to DBUS, maybe the daemon is not launch ?");
        e_dbus_shutdown();
        ecore_shutdown();
        return -1;
    }

    setup(exaltd_conn);

    exalt_eth_cb_set(eth_cb,exaltd_conn);
    exalt_wireless_scan_cb_set(wireless_scan_cb,exaltd_conn);

    exalt_main();

    if(!nodaemon)
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
        if ((fp = fopen(pidfile, "w+")))
        {
            int ret;
            ret = fwrite(buf, sizeof(char), size, fp);
            fclose(fp);
        }
        else
            EXALT_LOG_WARN("Can not create the pid file: %s", pidfile);
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

    if(action == EXALT_IFACE_ACTION_NEW || action == EXALT_IFACE_ACTION_ADD)
    {
        setup_new_iface(conn,eth);

        //first we load the driver if eth is a wireless connection
        if(exalt_eth_wireless_is(eth))
        {
            Exalt_Wireless* w = exalt_eth_wireless_get(eth);
            str = exalt_eth_driver_load(CONF_FILE,exalt_eth_udi_get(eth));
            if(!str)
                str=eina_stringshare_add("wext");
            exalt_wireless_wpasupplicant_driver_set(w,str);
           eina_stringshare_del(str);

            if( exalt_eth_state_load(CONF_FILE, exalt_eth_udi_get(eth)) == 1) //up
            {
                if(!exalt_eth_up_is(eth))
                    exalt_eth_up(eth);
                int i = 0;
                while(!exalt_eth_up_is(eth) && i<10)
                {
                    usleep(500);
                    i++;
                }
                //search a favorite network
                iface_wireless_search_favorite = eina_list_append(iface_wireless_search_favorite,eth);
                exalt_wireless_scan_start(eth);
            }
            else
                exalt_eth_down(eth);
        }
        else
        {
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

            Exalt_Configuration *c = exalt_eth_conf_load(CONF_FILE, exalt_eth_udi_get(eth));
            short not_c = 0;
            if(!c)
            {
                c = exalt_conf_new();
                not_c = 1;
            }
            exalt_eth_configuration_set(eth,c);

            if(not_c || exalt_eth_state_load(CONF_FILE, exalt_eth_udi_get(eth)) == 1) //up
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
                    exalt_eth_conf_apply(eth, c);
            }
            else
            {
                exalt_eth_down(eth);
            }
        }
    }

    if(action == EXALT_IFACE_ACTION_REMOVE)
        setup_del_iface(conn,eth);

    if ( action == EXALT_IFACE_ACTION_LINK && exalt_eth_up_is(eth))
    {
        Exalt_Configuration *c = exalt_eth_conf_load(CONF_FILE, exalt_eth_udi_get(eth));
        if(!c)
            c = exalt_conf_new();
        if(exalt_eth_wireless_is(eth))
            exalt_conf_wireless_set(c, 1);
        else
            exalt_conf_wireless_set(c, 0);

        exalt_eth_conf_apply(eth, c);
        exalt_eth_save(CONF_FILE,eth);
    }


    if ( action == EXALT_IFACE_ACTION_UP && exalt_eth_link_is(eth) )
    {
        Exalt_Configuration *c = exalt_eth_conf_load(CONF_FILE, exalt_eth_udi_get(eth));
        if(!c)
            c = exalt_conf_new();
        if(exalt_eth_wireless_is(eth))
            exalt_conf_wireless_set(c, 1);
        else
            exalt_conf_wireless_set(c, 0);

        exalt_eth_conf_apply(eth, c);
        exalt_eth_save(CONF_FILE,eth);
    }

    if (action == EXALT_IFACE_ACTION_UNLINK || action == EXALT_IFACE_ACTION_DOWN)
        //remove the default gateway
        exalt_eth_gateway_delete(eth);

    if( action == EXALT_IFACE_ACTION_UP || action == EXALT_IFACE_ACTION_DOWN)
        exalt_eth_save(CONF_FILE, eth);


    if( action==EXALT_IFACE_ACTION_CONF_APPLY_DONE || action==EXALT_IFACE_ACTION_CONF_APPLY_START)
    {
        //printf("apply DONE !!!\n");
        //save the new configuration
        exalt_eth_save(CONF_FILE, eth);
    }

    //if(action == EXALT_IFACE_ACTION_CONF_APPLY_START)
    //    printf("apply start !!\n");

    //waiting card
    if(!waiting_iface_is_done(waiting_iface_list) && waiting_iface_is(waiting_iface_list, eth) && action == EXALT_IFACE_ACTION_ADDRESS_NEW )
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
    EXALT_ASSERT_RETURN_VOID(!!msg);

    name = exalt_eth_name_get(eth);
    EXALT_ASSERT_RETURN_VOID(!!name);

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
    Eina_List *l, *l2;

    conn = (E_DBus_Connection*) data;
    EXALT_ASSERT_RETURN_VOID(!!conn);

    //search if we search a favorite network for this interface
    if(eina_list_data_find(iface_wireless_search_favorite, eth))
    {
        iface_wireless_search_favorite = eina_list_remove(iface_wireless_search_favorite, eth);

        //first we look if the previous network where we was connected is detected
        Exalt_Configuration *c = exalt_eth_conf_load(CONF_FILE, exalt_eth_udi_get(eth));
        const char *essid = exalt_conf_network_essid_get(exalt_conf_network_get(c));
        int find = 0;
        if(c && essid)
        {
            EINA_LIST_FOREACH(networks, l, wi)
            {
                if(strcmp(essid, exalt_wireless_network_essid_get(wi)) == 0)
                {
                    find = 1;
                    break;
                }
            }
            if(find)
                exalt_eth_conf_apply(eth, c);
        }

        //then we look in the favorite list
        if(!find)
        {
            Eina_List *l_network = exalt_conf_network_list_load(CONF_FILE);
            EINA_LIST_FOREACH(networks, l, wi)
            {
                EINA_LIST_FOREACH(l_network, l2, essid)
                {
                    if(strcmp(essid, exalt_wireless_network_essid_get(wi)) == 0)
                    {
                        Exalt_Configuration *c = exalt_conf_network_load(CONF_FILE, essid);
                        if(exalt_conf_network_favoris_is(exalt_conf_network_get(c)))
                        {
                            find = 1;
                            exalt_eth_conf_apply(eth, exalt_conf_network_load(CONF_FILE, essid));
                            break;
                        }
                    }
                }
                if(find)
                    break;
            }
        }
    }

    //send a broadcast
    msg = dbus_message_new_signal(EXALTD_PATH_NOTIFY,EXALTD_INTERFACE_NOTIFY, "scan_notify");
    EXALT_ASSERT_RETURN_VOID(!!msg);

    name = exalt_eth_name_get(eth);
    EXALT_ASSERT_ADV(!!name,
            dbus_message_unref(msg);return,
            "name!=NULL failed\n");

    dbus_args_valid_append(msg);

    dbus_message_iter_init_append(msg, &args);
    EXALT_ASSERT_RETURN_VOID(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &name));


    if(networks)
    {
        EXALT_ASSERT_RETURN_VOID(dbus_message_iter_open_container(&args,
                    DBUS_TYPE_ARRAY,
                    "(ssiisia(siiiaiiaii))",
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

            //add the mode
            integer = exalt_wireless_network_mode_get(wi);
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

            EXALT_ASSERT_RETURN_VOID(dbus_message_iter_open_container(&iter_w,
                        DBUS_TYPE_ARRAY,
                        "(siiiaiiaii)",
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

                integer = exalt_wireless_network_ie_eap_get(ie);
                EXALT_ASSERT_CUSTOM_RET(
                        dbus_message_iter_append_basic(&iter_ie,
                            DBUS_TYPE_INT32,
                            &integer),
                        dbus_message_unref(msg);return );

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

DBusMessage* conf_from_dbusmessage(Exalt_Configuration* c,DBusMessage *msg,DBusMessage *reply)
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
    exalt_conf_mode_set(c,i);
    dbus_message_iter_next(&args);

    if(exalt_conf_mode_get(c)==EXALT_STATIC)
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
        exalt_conf_ip_set(c,s);
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
        exalt_conf_netmask_set(c,s);
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
        exalt_conf_gateway_set(c,s);
        //printf("gateway: %s\n",s);
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
    exalt_conf_wireless_set(c,i);
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
    exalt_conf_cmd_after_apply_set(c,s);

    if(exalt_conf_wireless_is(c))
    {
        char* string;
        int integer;
        Exalt_Configuration_Network* n;

        n = exalt_conf_network_new();
        exalt_conf_network_set(c, n);

        dbus_message_iter_next(&args);
        dbus_message_iter_get_basic(&args, &string);
        exalt_conf_network_essid_set(n,string);

        dbus_message_iter_next(&args);
        dbus_message_iter_get_basic(&args, &integer);
        exalt_conf_network_encryption_set(n,integer);

        dbus_message_iter_next(&args);
        dbus_message_iter_get_basic(&args, &integer);
        exalt_conf_network_mode_set(n,integer);

        dbus_message_iter_next(&args);
        dbus_message_iter_get_basic(&args, &string);
        exalt_conf_network_key_set(n,string);

        dbus_message_iter_next(&args);
        dbus_message_iter_get_basic(&args, &integer);
        exalt_conf_network_wep_set(n,integer);

        dbus_message_iter_next(&args);
        dbus_message_iter_get_basic(&args, &integer);
        exalt_conf_network_wep_hexa_set(n,integer);

        dbus_message_iter_next(&args);
        dbus_message_iter_get_basic(&args, &integer);
        exalt_conf_network_wpa_set(n,integer);

        dbus_message_iter_next(&args);
        dbus_message_iter_get_basic(&args, &integer);
        exalt_conf_network_wpa_type_set(n,integer);

        dbus_message_iter_next(&args);
        dbus_message_iter_get_basic(&args, &integer);
        exalt_conf_network_group_cypher_set(n,integer);

        dbus_message_iter_next(&args);
        dbus_message_iter_get_basic(&args, &integer);
        exalt_conf_network_pairwise_cypher_set(n,integer);

        dbus_message_iter_next(&args);
        dbus_message_iter_get_basic(&args, &integer);
        exalt_conf_network_auth_suites_set(n,integer);

        dbus_message_iter_next(&args);
        dbus_message_iter_get_basic(&args, &integer);
        exalt_conf_network_eap_set(n,integer);

        dbus_message_iter_next(&args);
        dbus_message_iter_get_basic(&args, &string);
        exalt_conf_network_ca_cert_set(n,string);

        dbus_message_iter_next(&args);
        dbus_message_iter_get_basic(&args, &string);
        exalt_conf_network_client_cert_set(n,string);

        dbus_message_iter_next(&args);
        dbus_message_iter_get_basic(&args, &string);
        exalt_conf_network_private_key_set(n,string);

        dbus_message_iter_next(&args);
        dbus_message_iter_get_basic(&args, &integer);
        exalt_conf_network_favoris_set(n,integer);

        dbus_message_iter_next(&args);
        dbus_message_iter_get_basic(&args, &integer);
        exalt_conf_network_save_when_apply_set(n,integer);
    }


    return NULL;
}

int dbus_args_error_append(DBusMessage *msg, int id_error, const char* error)
{
    DBusMessageIter args;
    int err = EXALT_DBUS_ERROR;

    EXALT_ASSERT_RETURN(!!msg);
    EXALT_ASSERT_RETURN(!!error);

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


