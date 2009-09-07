/* Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 * Description: A code example : How perform a wireless scan on an interface
 *      This example requires the daemon exalt-daemon.
 */

//gcc -o dbus_wireless_scan dbus_wireless_scan.c `pkg-config --libs exalt_dbus exalt ecore --cflags exalt_dbus exalt ecore` -Wall -g

#include <Exalt_DBus.h>

void network_list_notify_scan(char* iface, Eina_List* networks, void* user_data )
{
    Exalt_DBus_Conn *conn = user_data;
    conn = conn;
    Exalt_Wireless_Network* w;
    Eina_List *l;

    printf("## Scan on %s\n",iface);
    EINA_LIST_FOREACH(networks,l,w)
    {
        //search the network in the list
        const char* essid = exalt_wireless_network_essid_get(w);
        printf("\tEssid\t\t:\t%s\n",essid);
        printf("\tQuality\t\t:\t%d\n",exalt_wireless_network_quality_get(w));
        printf("\tAddress\t\t:\t%s\n",exalt_wireless_network_address_get(w));
        printf("\tEncryption\t:\t%s\n",(exalt_wireless_network_encryption_is(w)?"Yes":"No"));
        printf("\tMode\t:\t%s\n",exalt_wireless_network_name_from_mode(exalt_wireless_network_mode_get(w)));

        // if the network is encrypted and we don't have a wpa description (the ie list)
        //      the encryption is WEP
        if(exalt_wireless_network_encryption_is(w)
                && !exalt_wireless_network_ie_get(w))
            printf("\tEncryption\t:\tWEP\n");
        else if(exalt_wireless_network_encryption_is(w))
        {
            Eina_List *l;
            Exalt_Wireless_Network_IE *ie;
            int i;
            EINA_LIST_FOREACH(exalt_wireless_network_ie_get(w),l,ie)
            {
                printf("\t\tDescription\t:\t%s\n",exalt_wireless_network_ie_description_get(ie));
                printf("\t\t\tWPA type\t:\t%s\n", exalt_wireless_network_name_from_wpa_type(
                            exalt_wireless_network_ie_wpa_type_get(ie)));
                printf("\t\t\tGroup Cypher\t:\t%s\n", exalt_wireless_network_name_from_cypher_name(
                            exalt_wireless_network_ie_group_cypher_get(ie)));
                printf("\t\t\tPairwise Cypher\t:\t");
                for(i=0;i<exalt_wireless_network_ie_pairwise_cypher_number_get(ie);i++)
                {
                    printf("%s  ",exalt_wireless_network_name_from_cypher_name(
                                exalt_wireless_network_ie_pairwise_cypher_get(ie,i)));
                }
                printf("\n");
                printf("\t\t\tAuth. suite\t:\t");
                for(i=0;i<exalt_wireless_network_ie_auth_suites_number_get(ie);i++)
                {
                    printf("%s  ",exalt_wireless_network_name_from_auth_suites(
                                exalt_wireless_network_ie_auth_suites_get(ie,i)));
                }
                printf("\n");
            }
        }

        printf("\n");
    }
    printf("\n\n");
}

void response_cb(Exalt_DBus_Response* response, void* data)
{
    Exalt_DBus_Conn *conn = (Exalt_DBus_Conn*) data;
    Eina_List *l;
    char *s;

    switch(exalt_dbus_response_type_get(response))
    {
        case EXALT_DBUS_RESPONSE_IFACE_WIRELESS_LIST:
            EINA_LIST_FOREACH(exalt_dbus_response_list_get(response), l, s)
            {
                //perform a scan
                exalt_dbus_wireless_scan(conn, s);
            }
            break;
        default : ;
    }
}

int main()
{
    //init exalt_dbus
    exalt_dbus_init();

    //open a connection with dbus
    Exalt_DBus_Conn *conn;
    conn = exalt_dbus_connect();

    if(!conn)
    {
        printf("Are you sure dbus is launched ?");
        return -88;
    }

    //set 3 callback methods
    // This method is called to return the response of a question
    exalt_dbus_response_notify_set(conn,response_cb,conn);

    // This methid is called every time the daemon send a notification
    // For example if the ip address of an interface change
    // We don't need this method in this example
    //exalt_dbus_notify_set(conn,notify_cb,conn);

    // This method is called to return the result of a scan of wireless network
    exalt_dbus_scan_notify_set(conn,network_list_notify_scan,conn);

    //first we need the list of wireless interface
    exalt_dbus_wireless_list_get(conn);

    ecore_main_loop_begin();
    return 1;
}
