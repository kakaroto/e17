/* Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 * Description: A code example : How to retrieve the configuration of each interface
 *      This example requires the daemon exalt-daemon.
 */

//gcc -o dbus_get_configuration dbus_get_conguration.c `pkg-config --libs exalt_dbus exalt ecore --cflags exalt_dbus exalt ecore` -Wall -g

#include <Exalt_DBus.h>

// The configuration of one interface
typedef struct configuration
{
    // Each time we get a response (the ip, the netmask ....)
    // we increment nb_response
    // When nb_response = 3, we display the result
    int nb_response;
    // 1 if the interface is a wireless interface
    int is_wireless;
    char *iface;
    char *ip;
    char *netmask;
    char *gateway;
    char *essid;
} Configuration;

// The list of interface
// Each element is of type Configuration
Eina_List *l_iface = NULL;

void response_cb(Exalt_DBus_Response* response, void* data)
{
    Exalt_DBus_Conn *conn = (Exalt_DBus_Conn*) data;
    Eina_List *l;
    char *s;
    const char *iface = NULL;
    Configuration *c = NULL;

    // If the response is one of these types
    // We search the configuration of the interface
    switch(exalt_dbus_response_type_get(response))
    {
        case EXALT_DBUS_RESPONSE_IFACE_IP_GET:
        case EXALT_DBUS_RESPONSE_IFACE_NETMASK_GET:
        case EXALT_DBUS_RESPONSE_IFACE_GATEWAY_GET:
        case EXALT_DBUS_RESPONSE_WIRELESS_ESSID_GET:
            iface = exalt_dbus_response_iface_get(response);
            EINA_LIST_FOREACH(l_iface, l, c)
            {
                if(strcmp(c->iface, iface) == 0)
                    break;
            }
            break;
        default: ;
    }
    switch(exalt_dbus_response_type_get(response))
    {
        case EXALT_DBUS_RESPONSE_IFACE_WIRED_LIST:
            EINA_LIST_FOREACH(exalt_dbus_response_list_get(response), l, s)
            {
                Configuration *c = calloc(1,sizeof(Configuration));
                l_iface = eina_list_append(l_iface, c);

                c->iface = strdup(s);
                exalt_dbus_eth_ip_get(conn, s);
                exalt_dbus_eth_netmask_get(conn, s);
                exalt_dbus_eth_gateway_get(conn, s);
            }
            break;
        case EXALT_DBUS_RESPONSE_IFACE_WIRELESS_LIST:
            EINA_LIST_FOREACH(exalt_dbus_response_list_get(response), l, s)
            {
                Configuration *c = calloc(1,sizeof(Configuration));
                l_iface = eina_list_append(l_iface, c);

                c->iface = strdup(s);
                c->is_wireless = 1;
                exalt_dbus_eth_ip_get(conn, s);
                exalt_dbus_eth_netmask_get(conn, s);
                exalt_dbus_eth_gateway_get(conn, s);
                exalt_dbus_wireless_essid_get(conn ,s);
            }
            break;
        case EXALT_DBUS_RESPONSE_IFACE_IP_GET:
            if(exalt_dbus_response_address_get(response))
                c->ip = strdup(exalt_dbus_response_address_get(response));
            c->nb_response++;
            break;
        case EXALT_DBUS_RESPONSE_IFACE_NETMASK_GET:
            if(exalt_dbus_response_address_get(response))
                c->netmask = strdup(exalt_dbus_response_address_get(response));
            c->nb_response++;
            break;
        case EXALT_DBUS_RESPONSE_IFACE_GATEWAY_GET:
            if(exalt_dbus_response_address_get(response))
                c->gateway = strdup(exalt_dbus_response_address_get(response));
            c->nb_response++;
            break;
    case EXALT_DBUS_RESPONSE_WIRELESS_ESSID_GET:
            if(exalt_dbus_response_string_get(response))
                c->essid = strdup(exalt_dbus_response_string_get(response));
            c->nb_response++;
        default : ;
    }

    if(c && !c->is_wireless && c->nb_response == 3)
    {
        printf("\n ## IFACE %s\n", c->iface);
        printf("\tIP address : %s\n", c->ip);
        printf("\tNetmask    : %s\n", c->netmask);
        printf("\tGateway    : %s\n", c->gateway);
    }
    else if(c && c->nb_response == 4)
    {
        printf("\n ## WIRELESS IFACE %s\n", c->iface);
        printf("\tEssid      : %s\n", c->essid);
        printf("\tIP address : %s\n", c->ip);
        printf("\tNetmask    : %s\n", c->netmask);
        printf("\tGateway    : %s\n", c->gateway);
    }
}

/*
 * This method is called when we are connected to the daemon or
 * if the connection failed. You can't ask something to the daemon
 * before you are connected.
 */
void _connect_cb(void *data, Exalt_DBus_Conn *conn, Eina_Bool success)
{
    // connection failed for some reasons
    if(!success) return ;

    // Ask the list of interface to the daemon
    // The list of wired interface
    exalt_dbus_eth_list_get(conn);
    // The list of wireless interface
    exalt_dbus_wireless_list_get(conn);
}

int main()
{
    //init exalt_dbus
    exalt_dbus_init();

    //open a connection with dbus
    Exalt_DBus_Conn *conn;
    conn = exalt_dbus_connect(_connect_cb, NULL, NULL);

    if(!conn)
    {
        printf("Are you sure dbus is launched ?");
        return -88;
    }

    //set 3 callbacks methods
    // This method is called to return the response of a question
    // For example if you ask the ip address exalt_dbus_eth_ip_get()
    exalt_dbus_response_notify_set(conn,response_cb,conn);

    // This methid is called every time the daemon send a notification
    // For example if the ip address of an interface change
    // We don't need this method in this example
    //exalt_dbus_notify_set(conn,notify_cb,NULL);

    // This method is called to return the result of a scan of wireless network
    // We don't need this method in this example
    //exalt_dbus_scan_notify_set(conn,network_list_notify_scan,iface_list);

    ecore_main_loop_begin();
    return 1;
}

