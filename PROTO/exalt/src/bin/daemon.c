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

int setup(E_DBus_Connection *conn)
{
    E_DBus_Object *obj;
    E_DBus_Interface *iface;
    e_dbus_request_name(conn, EXALTD_SERVICE , 0, NULL, NULL);
    obj = e_dbus_object_add(conn, EXALTD_PATH, NULL);
    iface = e_dbus_interface_new(EXALTD_INTERFACE_READ);


    e_dbus_interface_method_add(iface, "IFACE_GET_IP", NULL, NULL, dbus_cb_eth_get_ip);
    e_dbus_interface_method_add(iface, "IFACE_GET_NETMASK", NULL, NULL, dbus_cb_eth_get_netmask);
    e_dbus_interface_method_add(iface, "IFACE_GET_GATEWAY", NULL, NULL, dbus_cb_eth_get_gateway);
    e_dbus_interface_method_add(iface, "IFACE_GET_ETH_LIST", NULL, NULL, dbus_cb_eth_get_eth_list);
    e_dbus_interface_method_add(iface, "IFACE_IS_WIRELESS", NULL, NULL, dbus_cb_eth_is_wireless);
    e_dbus_interface_method_add(iface, "IFACE_IS_LINK", NULL, NULL, dbus_cb_eth_is_link);
    e_dbus_interface_method_add(iface, "IFACE_IS_UP", NULL, NULL, dbus_cb_eth_is_up);
    e_dbus_interface_method_add(iface, "IFACE_IS_DHCP", NULL, NULL, dbus_cb_eth_is_dhcp);


    e_dbus_interface_method_add(iface, "IFACE_GET_ESSID", NULL, NULL, dbus_cb_wireless_get_essid);
    e_dbus_interface_method_add(iface, "IFACE_SCAN_START", NULL, NULL, dbus_cb_wireless_scan_start);
    e_dbus_interface_method_add(iface, "IFACE_SCAN_WAIT", NULL, NULL, dbus_cb_wireless_scan_wait);
    e_dbus_interface_method_add(iface, "IFACE_GET_WPASUPPLICANT_DRIVER", NULL, NULL, dbus_cb_wireless_get_wpasupplicant_driver);


    e_dbus_interface_method_add(iface, "NETWORK_GET_QUALITY", NULL, NULL, dbus_cb_wirelessnetwork_get_quality);
    e_dbus_interface_method_add(iface, "NETWORK_GET_ADDR", NULL, NULL, dbus_cb_wirelessnetwork_get_addr);
    e_dbus_interface_method_add(iface, "NETWORK_GET_PROTOCOL", NULL, NULL, dbus_cb_wirelessnetwork_get_protocol);
    e_dbus_interface_method_add(iface, "NETWORK_GET_MODE", NULL, NULL, dbus_cb_wirelessnetwork_get_mode);
    e_dbus_interface_method_add(iface, "NETWORK_GET_CHANNEL", NULL, NULL, dbus_cb_wirelessnetwork_get_channel);
    e_dbus_interface_method_add(iface, "NETWORK_GET_BITRATES", NULL, NULL, dbus_cb_wirelessnetwork_get_bitrates);
    e_dbus_interface_method_add(iface, "NETWORK_GET_ENCRYPTION", NULL, NULL, dbus_cb_wirelessnetwork_get_encryption);
    e_dbus_interface_method_add(iface, "NETWORK_GET_SIGNALLVL", NULL, NULL, dbus_cb_wirelessnetwork_get_signallvl);
    e_dbus_interface_method_add(iface, "NETWORK_GET_NOISELVL", NULL, NULL, dbus_cb_wirelessnetwork_get_noiselvl);

    e_dbus_interface_method_add(iface, "NETWORK_GET_DEFAULT_CONN", NULL, NULL, dbus_cb_wirelessnetwork_get_default_conn);

    e_dbus_interface_method_add(iface, "DNS_GET_LIST", NULL, NULL, dbus_cb_dns_get_list);

    e_dbus_interface_method_add(iface, "BOOTPROCESS_IFACE_IS", NULL, NULL, dbus_cb_bootprocess_iface_is);
    e_dbus_interface_method_add(iface, "BOOTPROCESS_TIMEOUT_GET", NULL, NULL, dbus_cb_bootprocess_timeout_get);

    e_dbus_interface_method_add(iface, "IFACE_CMD_GET", NULL, NULL, dbus_cb_eth_get_cmd);


    e_dbus_object_interface_attach(obj, iface);

    iface = e_dbus_interface_new(EXALTD_INTERFACE_WRITE);
    e_dbus_interface_method_add(iface, "IFACE_UP", NULL, NULL, dbus_cb_eth_up);
    e_dbus_interface_method_add(iface, "IFACE_DOWN", NULL, NULL, dbus_cb_eth_down);

    e_dbus_interface_method_add(iface, "IFACE_SET_WPASUPPLICANT_DRIVER", NULL, NULL, dbus_cb_wireless_set_wpasupplicant_driver);

    e_dbus_interface_method_add(iface, "DNS_ADD", NULL, NULL, dbus_cb_dns_add);
    e_dbus_interface_method_add(iface, "DNS_REPLACE", NULL, NULL, dbus_cb_dns_replace);
    e_dbus_interface_method_add(iface, "DNS_DELETE", NULL, NULL, dbus_cb_dns_delete);

    e_dbus_interface_method_add(iface, "IFACE_APPLY_CONN", NULL, NULL, dbus_cb_eth_apply_conn);

    e_dbus_interface_method_add(iface, "BOOTPROCESS_IFACE_ADD", NULL, NULL, dbus_cb_bootprocess_iface_add);
    e_dbus_interface_method_add(iface, "BOOTPROCESS_IFACE_REMOVE", NULL, NULL, dbus_cb_bootprocess_iface_remove);
    e_dbus_interface_method_add(iface, "BOOTPROCESS_TIMEOUT_SET", NULL, NULL, dbus_cb_bootprocess_timeout_set);



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

    exalt_eth_set_cb(eth_cb,exaltd_conn);
    exalt_eth_set_scan_cb(wireless_scan_cb,exaltd_conn);

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
            ecore_main_loop_begin();

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
            fwrite(buf, sizeof(char), size, fp);
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
        //first we load the driver if eth is a wireless connection
        if(exalt_eth_is_wireless(eth))
        {
            Exalt_Wireless* w = exalt_eth_get_wireless(eth);
            str = exalt_eth_driver_load(CONF_FILE,exalt_eth_get_udi(eth));
            exalt_wireless_set_wpasupplicant_driver(w,str);
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

        Exalt_Connection *c = exalt_eth_conn_load(CONF_FILE, exalt_eth_get_udi(eth));
        short not_c = 0;
        if(!c)
        {
            c = exalt_conn_new();
            not_c = 1;
        }

        if(not_c || exalt_eth_state_load(CONF_FILE, exalt_eth_get_udi(eth)) == EXALT_UP)
        {
            int i = 0;
            if(!exalt_eth_is_up(eth))
                exalt_eth_up(eth);
            while(!exalt_eth_is_up(eth) && i<10)
            {
                usleep(500);
                i++;
            }

            if(exalt_eth_is_link(eth) && exalt_eth_is_up(eth))
                exalt_eth_apply_conn(eth, c);
        }
        else
        {
            exalt_eth_down(eth);
        }
    }

    if ( action == EXALT_ETH_CB_ACTION_LINK && exalt_eth_is_up(eth))
    {
        Exalt_Connection *c = exalt_eth_conn_load(CONF_FILE, exalt_eth_get_udi(eth));
        if(!c)
            c = exalt_conn_new();
        if(exalt_eth_is_wireless(eth))
            exalt_conn_set_wireless(c, 1);
        else
            exalt_conn_set_wireless(c, 0);

        exalt_eth_apply_conn(eth, c);
        exalt_eth_save(CONF_FILE,eth);
    }


    if ( action == EXALT_ETH_CB_ACTION_UP && exalt_eth_is_link(eth) && (time(NULL) - exalt_eth_get_dontapplyafterup(eth)>2) )
    {
        Exalt_Connection *c = exalt_eth_conn_load(CONF_FILE, exalt_eth_get_udi(eth));
        if(!c)
            c = exalt_conn_new();
        if(exalt_eth_is_wireless(eth))
            exalt_conn_set_wireless(c, 1);
        else
            exalt_conn_set_wireless(c, 0);

        exalt_eth_apply_conn(eth, c);
        exalt_eth_save(CONF_FILE,eth);
    }

    if (action == EXALT_ETH_CB_ACTION_UNLINK || action == EXALT_ETH_CB_ACTION_DOWN)
        //remove the default gateway
        exalt_eth_delete_gateway(eth);

    if( action == EXALT_ETH_CB_ACTION_UP || action == EXALT_ETH_CB_ACTION_DOWN)
        exalt_eth_save(CONF_FILE, eth);


    if( action==EXALT_ETH_CB_ACTION_CONN_APPLY_DONE)
        //save the new configuration
        exalt_eth_save(CONF_FILE, eth);



    //waiting card
    if(!waiting_iface_is_done(waiting_iface_list) && waiting_iface_is(waiting_iface_list, eth) && action == EXALT_ETH_CB_ACTION_ADDRESS_NEW )
    {
        const char *ip = exalt_eth_get_ip(eth);

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
    msg = dbus_message_new_signal(EXALTD_PATH,EXALTD_INTERFACE_READ, "NOTIFY");
    EXALT_ASSERT_RETURN_VOID(msg!=NULL);

    name = exalt_eth_get_name(eth);
    EXALT_ASSERT_RETURN_VOID(name!=NULL);

    dbus_args_valid_append(msg);

    dbus_message_iter_init_append(msg, &args);
    EXALT_ASSERT(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &name));
    EXALT_ASSERT(dbus_message_iter_append_basic(&args, DBUS_TYPE_INT32, &action));

    e_dbus_message_send(conn, msg, NULL, 3,NULL);
    dbus_message_unref(msg);
}

void wireless_scan_cb(Exalt_Ethernet* eth,Ecore_List* networks, void* data)
{
    E_DBus_Connection *conn;
    DBusMessage* msg;
    DBusMessageIter args;
    const char* name;
    const char* essid;
    Exalt_Wireless_Network *wi;

    Ecore_List* l;

    EXALT_ASSERT_RETURN_VOID(networks!=NULL);

    conn = (E_DBus_Connection*) data;
    EXALT_ASSERT_RETURN_VOID(conn!=NULL);

    //send a broadcast
    msg = dbus_message_new_signal(EXALTD_PATH,EXALTD_INTERFACE_READ, "SCAN_NOTIFY");
    EXALT_ASSERT_RETURN_VOID(msg!=NULL);

    name = exalt_eth_get_name(eth);
    EXALT_ASSERT_ADV(name!=NULL,
            dbus_message_unref(msg);return,
            "name!=NULL failed");

    dbus_args_valid_append(msg);

    dbus_message_iter_init_append(msg, &args);
    EXALT_ASSERT_RETURN_VOID(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &name));

    l=networks;
    ecore_list_first_goto(l);
    while ( (wi =  ecore_list_next(l)))
    {
        essid = exalt_wirelessnetwork_get_essid(wi);
        EXALT_ASSERT_ADV(essid!=NULL,
                dbus_message_unref(msg);return ,
                "essud!=NULL failed");

        EXALT_ASSERT_ADV(dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &essid),
                dbus_message_unref(msg);return ,
                "dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &essid) failed");
    }

    e_dbus_message_send(conn, msg, NULL, -1,NULL);
    dbus_message_unref(msg);
}


Exalt_Ethernet* dbus_get_eth(DBusMessage* msg)
{
    DBusMessageIter args;
    const char* interface= NULL;
    Exalt_Ethernet* eth;

    if(!dbus_message_iter_init(msg, &args))
        return NULL;

    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
        return NULL;
    else
        dbus_message_iter_get_basic(&args, &interface);
    //search the interface
    eth = exalt_eth_get_ethernet_byname(interface);
    return eth;
}

Exalt_Wireless_Network* dbus_get_wirelessnetwork(DBusMessage* msg)
{
    DBusMessageIter args;
    char* interface = NULL;
    Exalt_Ethernet* eth;
    char* essid = NULL;
    Exalt_Wireless_Network* wi;

    if(!dbus_message_iter_init(msg, &args))
        return NULL;

    if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
        return NULL;
    else
        dbus_message_iter_get_basic(&args, &interface);

    //search the interface
    eth = exalt_eth_get_ethernet_byname(interface);
    if(!eth)
        return NULL;

    dbus_message_iter_next(&args);
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
    Ecore_List *l;
    void *data;

    EXALT_ASSERT_RETURN(eth!=NULL);
    EXALT_ASSERT_RETURN(essid!=NULL);

    EXALT_ASSERT_RETURN(exalt_eth_is_wireless(eth)!=0);
    w = exalt_eth_get_wireless(eth);

    l = exalt_wireless_get_networks_list(w);
    ecore_list_first_goto(l);

    while( (data=ecore_list_next(l)))
    {
        wi = Exalt_Wireless_Network(data);
        if(strcmp(essid,exalt_wirelessnetwork_get_essid(wi))==0 )
            return wi;
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
