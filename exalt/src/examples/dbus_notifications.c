/* Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 * Description: A code example : How retrieve the notifications sent by the daemon.
 *      For example when an IP address change
 *      This example requires the daemon exalt-daemon.
 */

//gcc -o dbus_notifications dbus_notifications.c `pkg-config --libs exalt_dbus exalt ecore --cflags exalt_dbus exalt ecore` -Wall -g

#include <Exalt_DBus.h>

void notify_cb(char* iface, Exalt_Enum_Action action, void* data)
{
    Exalt_DBus_Conn *conn = (Exalt_DBus_Conn*) data;
    conn = conn;

    switch(action)
    {
        case EXALT_IFACE_ACTION_NEW:
        case EXALT_IFACE_ACTION_ADD:
            printf("New interface : %s\n", iface);
            break;
        case EXALT_IFACE_ACTION_REMOVE:
            printf("Interface removed : %s\n", iface);
            break;
        case EXALT_IFACE_ACTION_UP:
            printf("Interface activated : %s\n",iface);
            break;
        case EXALT_IFACE_ACTION_DOWN:
            printf("Interface desactivated : %s\n", iface);
            break;
        case EXALT_IFACE_ACTION_LINK:
            printf("Interface linked : %s\n",iface);
            break;
        case EXALT_IFACE_ACTION_UNLINK:
            printf("Interface unlinked : %s\n",iface);
            break;
        case EXALT_WIRELESS_ACTION_ESSIDCHANGE:
            printf("The interface %s has a new essid\n",iface);
            break;
        case EXALT_IFACE_ACTION_ADDRESS_NEW:
            printf("The interface %s has a new IP address\n",iface);
            break;
        case EXALT_IFACE_ACTION_NETMASK_NEW:
            printf("The interface %s has a new netmask address\n",iface);
            break;
        case EXALT_IFACE_ACTION_GATEWAY_NEW:
            printf("The interface %s has a new default gateway address\n",iface);
            break;
        case EXALT_IFACE_ACTION_CONF_APPLY_START:
            printf("The interface %s start to apply a new configuration\n",iface);
            break;
        case EXALT_IFACE_ACTION_CONF_APPLY_DONE:
            printf("The new configuration of the interface %s is applied\n",iface);
            break;
        case EXALT_IFACE_ACTION_DISCONNECTED:
        case EXALT_WIRELESS_ACTION_DISCONNECTED:
            printf("The interface %s is disconnected\n",iface);
            break;
        case EXALT_IFACE_ACTION_CONNECTED:
        case EXALT_WIRELESS_ACTION_CONNECTED:
            printf("The interface %s is connected\n",iface);
            break;
        default: ;
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
    // For example if you ask the ip address exalt_dbus_eth_ip_get()
    // We don't need this method in this example
    //exalt_dbus_response_notify_set(conn,response_cb,conn);

    // This methid is called every time the daemon send a notification
    // For example if the ip address of an interface change
    exalt_dbus_notify_set(conn,notify_cb,conn);

    // This method is called to return the result of a scan of wireless network
    // We don't need this method in this example
    //exalt_dbus_scan_notify_set(conn,network_list_notify_scan,conn);


    ecore_main_loop_begin();
    return 1;
}
