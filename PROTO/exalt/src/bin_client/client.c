/*
 * =====================================================================================
 *
 *       Filename:  client.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  11/02/09 12:06:00 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "main.h"
#include "config.h"

    EAPI int
elm_main(int argc, char **argv)
{
    Evas_Object*bg;

    /*
     * INITIALISATION
     */

    exalt_dbus_init();

    conn = exalt_dbus_connect();
    if(!conn)
        return -88;
    setlocale(LC_ALL, "");
    bindtextdomain("exalt",PACKAGE_LOCALE_DIR);
    textdomain("exalt-client");

    /*
     * Creation of the window
     */
    win = elm_win_add(NULL,"exalt_client",ELM_WIN_BASIC);
    elm_win_title_set(win, "Exalt - network manager");

    //evas_object_smart_callback_add(win, "delete-request", my_win_del, NULL);

    elm_win_autodel_set(win, 1);
    bg = elm_bg_add(win);
    evas_object_size_hint_weight_set(bg, 1.0, 1.0);
    elm_win_resize_object_add(win, bg);
    evas_object_show(bg);

    table = elm_table_add(win);
    elm_win_resize_object_add(win, table);
    evas_object_size_hint_weight_set(table, 1.0, 1.0);
    evas_object_show(table);


    iface_list_create();

    pnl_right_create();

    hover_wired_create();

    exalt_dbus_response_notify_set(conn,response_cb,NULL);
    exalt_dbus_notify_set(conn,notify_cb,NULL);

    exalt_dbus_eth_list_get(conn);
    exalt_dbus_wireless_list_get(conn);


    evas_object_resize(win, 600, 300);
    evas_object_show(win);


    elm_run();
    elm_shutdown();
    return 0;
}

ELM_MAIN()



void response_cb(Exalt_DBus_Response* response, void* data )
{
    char* iface;
    printf("Question id: %d\n",exalt_dbus_response_msg_id_get(response));
    switch(exalt_dbus_response_type_get(response))
    {
        case EXALT_DBUS_RESPONSE_DNS_LIST_GET:
            printf("DNS list:\n");

            {
                /*Ecore_List* l = exalt_dbus_response_list_get(response);
                char* dns;
                ecore_list_first_goto(l);
                while( (dns=ecore_list_next(l)) )
                    printf("%s\n",dns);
                    */
            }
            break;
        case EXALT_DBUS_RESPONSE_DNS_ADD:
            printf("DNS added\n");
            break;
        case EXALT_DBUS_RESPONSE_DNS_DEL:
            printf("DNS deleted\n");

            break;
        case EXALT_DBUS_RESPONSE_DNS_REPLACE:
            printf("DNS replaced\n");

            break;
        case EXALT_DBUS_RESPONSE_IFACE_WIRED_LIST:
            iface_list_response(response);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_WIRELESS_LIST:
            iface_list_response(response);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_IP_GET:
            hover_wired_response(response);
            //popup_update(inst,response);
            //if_wired_dialog_update(inst,response);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_NETMASK_GET:
            hover_wired_response(response);
            //if_wired_dialog_update(inst,response);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_GATEWAY_GET:
            hover_wired_response(response);
            //if_wired_dialog_update(inst,response);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_WIRELESS_IS:
            printf("%s is a wireless interface:\n",exalt_dbus_response_iface_get(response));

            printf("%s\n",(exalt_dbus_response_is_get(response)>0?"yes":"no"));
            break;
        case EXALT_DBUS_RESPONSE_IFACE_LINK_IS:
            iface_list_response(response);
            hover_wired_response(response);
            //if_wired_dialog_update(inst,response);
            //popup_update(inst,response);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_DHCP_IS:
            //if_wired_dialog_update(inst,response);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_UP_IS:
            iface_list_response(response);
            hover_wired_response(response);
            //if_wired_dialog_update(inst,response);
            //popup_update(inst,response);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_CMD_GET:
            //if_wired_dialog_update(inst,response);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_CMD_SET:
            printf("%s command:\n",exalt_dbus_response_iface_get(response));

            printf("The new command is supposed to be set\n");
            break;
        case EXALT_DBUS_RESPONSE_IFACE_UP:
            iface_list_response(response);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_DOWN:
            iface_list_response(response);
            break;
        case EXALT_DBUS_RESPONSE_WIRELESS_ESSID_GET:
            printf("%s essid:\n",exalt_dbus_response_iface_get(response));

            printf("%s\n",exalt_dbus_response_string_get(response));
            break;
        case EXALT_DBUS_RESPONSE_WIRELESS_WPASUPPLICANT_DRIVER_GET:
            printf("%s wpa_supplicant driver:\n",exalt_dbus_response_iface_get(response));

            printf("%s\n",exalt_dbus_response_string_get(response));
            break;
        case EXALT_DBUS_RESPONSE_WIRELESS_WPASUPPLICANT_DRIVER_SET:

            printf("The new driver is supposed to be set to the interface %s\n",exalt_dbus_response_iface_get(response));
            break;
        default: ;
    }
}


void notify_cb(char* eth, Exalt_Enum_Action action, void* user_data)
{
    switch(action)
    {
        case EXALT_ETH_CB_ACTION_NEW:
        case EXALT_ETH_CB_ACTION_ADD:
            break;
        case EXALT_ETH_CB_ACTION_REMOVE:
            break;
        case EXALT_ETH_CB_ACTION_UP:
        case EXALT_ETH_CB_ACTION_DOWN:
            exalt_dbus_eth_up_is(conn,eth);
            break;
        case EXALT_ETH_CB_ACTION_LINK:
        case EXALT_ETH_CB_ACTION_UNLINK:
            exalt_dbus_eth_link_is(conn,eth);
            break;
        case EXALT_WIRELESS_CB_ACTION_ESSIDCHANGE:
            break;
        case EXALT_ETH_CB_ACTION_ADDRESS_NEW:
            exalt_dbus_eth_ip_get(conn,eth);
            break;
        case EXALT_ETH_CB_ACTION_NETMASK_NEW:
            exalt_dbus_eth_netmask_get(conn,eth);
            break;
        case EXALT_ETH_CB_ACTION_GATEWAY_NEW:
            exalt_dbus_eth_gateway_get(conn,eth);
            break;
        case EXALT_ETH_CB_ACTION_CONN_APPLY_START:
            break;
        case EXALT_ETH_CB_ACTION_CONN_APPLY_DONE:
            break;
        default: ;
    }
}
