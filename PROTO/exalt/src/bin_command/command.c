/*
 * =====================================================================================
 *
 *       Filename:  command.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/10/08 17:20:12 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */


#define print_response_error() \
    if(!exalt_dbus_response_error_is(response)) \
    { \
        printf("Response error: (%d), %s\n", \
            exalt_dbus_response_error_id_get(response), \
            exalt_dbus_response_error_msg_get(response)); \
        break;\
    }

#include "command.h"

exalt_dbus_conn* conn;

void response(Exalt_DBus_Response* response, void* data)
{
    printf("Question id: %d\n",exalt_dbus_response_msg_id_get(response));
    switch(exalt_dbus_response_type_get(response))
    {
        case EXALT_DBUS_RESPONSE_DNS_LIST_GET:
            printf("DNS list:\n");
            print_response_error();
            {
                Ecore_List* l = exalt_dbus_response_list_get(response);
                char* dns;
                ecore_list_first_goto(l);
                while( (dns=ecore_list_next(l)) )
                    printf("%s\n",dns);
            }
            break;
        case EXALT_DBUS_RESPONSE_DNS_ADD:
            printf("DNS added\n");
            print_response_error();
            break;
        case EXALT_DBUS_RESPONSE_DNS_DEL:
            printf("DNS deleted\n");
            print_response_error();
            break;
        case EXALT_DBUS_RESPONSE_DNS_REPLACE:
            printf("DNS replaced\n");
            print_response_error();
            break;
        case EXALT_DBUS_RESPONSE_IFACE_WIRED_LIST:
            printf("Wired network interface list:\n");
            print_response_error();
            {
                Ecore_List* l = exalt_dbus_response_list_get(response);
                char* iface;
                ecore_list_first_goto(l);
                while( (iface=ecore_list_next(l)) )
                    printf("%s\n",iface);
            }
            break;
        case EXALT_DBUS_RESPONSE_IFACE_WIRELESS_LIST:
            printf("Wireless network interface list:\n");
            print_response_error();
            {
                Ecore_List* l = exalt_dbus_response_list_get(response);
                char* iface;
                ecore_list_first_goto(l);
                while( (iface=ecore_list_next(l)) )
                    printf("%s\n",iface);
            }
            break;
        case EXALT_DBUS_RESPONSE_IFACE_IP_GET:
            printf("%s IP address:\n",exalt_dbus_response_iface_get(response));
            print_response_error();
            printf("%s\n",exalt_dbus_response_address_get(response));
            break;
        case EXALT_DBUS_RESPONSE_IFACE_NETMASK_GET:
            printf("%s Netmask address:\n",exalt_dbus_response_iface_get(response));
            print_response_error();
            printf("%s\n",exalt_dbus_response_address_get(response));
            break;
        case EXALT_DBUS_RESPONSE_IFACE_GATEWAY_GET:
            printf("%s Gateway address:\n",exalt_dbus_response_iface_get(response));
            print_response_error();
            printf("%s\n",exalt_dbus_response_address_get(response));
            break;
        case EXALT_DBUS_RESPONSE_IFACE_WIRELESS_IS:
            printf("%s is a wireless interface:\n",exalt_dbus_response_iface_get(response));
            print_response_error();
            printf("%s\n",(exalt_dbus_response_is_get(response)>0?"yes":"no"));
            break;
        case EXALT_DBUS_RESPONSE_IFACE_LINK_IS:
            printf("%s is link:\n",exalt_dbus_response_iface_get(response));
            print_response_error();
            printf("%s\n",(exalt_dbus_response_is_get(response)>0?"yes":"no"));
            break;
        case EXALT_DBUS_RESPONSE_IFACE_DHCP_IS:
            printf("%s use a DHCP:\n",exalt_dbus_response_iface_get(response));
            print_response_error();
            printf("%s\n",(exalt_dbus_response_is_get(response)>0?"yes":"no"));
            break;
        case EXALT_DBUS_RESPONSE_IFACE_UP_IS:
            printf("%s is up:\n",exalt_dbus_response_iface_get(response));
            print_response_error();
            printf("%s\n",(exalt_dbus_response_is_get(response)>0?"yes":"no"));
            break;
        case EXALT_DBUS_RESPONSE_IFACE_CMD_GET:
            printf("%s command:\n",exalt_dbus_response_iface_get(response));
            print_response_error();
            printf("%s\n",exalt_dbus_response_string_get(response));
            break;
        case EXALT_DBUS_RESPONSE_IFACE_CMD_SET:
            printf("%s command:\n",exalt_dbus_response_iface_get(response));
            print_response_error();
            printf("The new command is supposed to be set\n");
            break;
        case EXALT_DBUS_RESPONSE_IFACE_UP:
            print_response_error();
            printf("The interface %s is supposed to be up\n",exalt_dbus_response_iface_get(response));
            break;
        case EXALT_DBUS_RESPONSE_IFACE_DOWN:
            print_response_error();
            printf("The interface %s is supposed to be down\n",exalt_dbus_response_iface_get(response));
            break;
        case EXALT_DBUS_RESPONSE_WIRELESS_ESSID_GET:
            printf("%s essid:\n",exalt_dbus_response_iface_get(response));
            print_response_error();
            printf("%s\n",exalt_dbus_response_string_get(response));
            break;
        case EXALT_DBUS_RESPONSE_WIRELESS_WPASUPPLICANT_DRIVER_GET:
            printf("%s wpa_supplicant driver:\n",exalt_dbus_response_iface_get(response));
            print_response_error();
            printf("%s\n",exalt_dbus_response_string_get(response));
            break;
        case EXALT_DBUS_RESPONSE_WIRELESS_WPASUPPLICANT_DRIVER_SET:
            print_response_error();
            printf("The new command is supposed to be set to the interface %s\n",exalt_dbus_response_iface_get(response));
            break;
    }
}

void dns(int argc, char** argv)
{
    char * dns, *dns2;

    if(strcmp(argv[1],"dns_get")==0)
    {
        exalt_dbus_dns_get_list(conn);
        return ;
    }

    if(argc<3)
    {
        fprintf(stderr,"We need an ip address\n");
        exit(EXIT_FAILURE);
    }

    if(strcmp(argv[1],"dns_add")==0)
    {
        dns = argv[2];
        if(!exalt_is_address(dns))
        {
            fprintf(stderr,"The value %s is not an valid address\n",dns);
            exit(EXIT_FAILURE);
        }
        exalt_dbus_dns_add(conn,dns);
    }
    else if(strcmp(argv[1],"dns_del")==0)
    {
        dns = argv[2];
        if(!exalt_is_address(dns))
        {
            fprintf(stderr,"The value %s is not an valid address\n",dns);
            exit(EXIT_FAILURE);
        }
        exalt_dbus_dns_delete(conn,dns);
    }
    else if(strcmp(argv[1],"dns_replace")==0)
    {
        if(argc<4)
        {
            fprintf(stderr,"We need a second ip address\n");
            exit(EXIT_FAILURE);
        }
        dns = argv[2];
        if(!exalt_is_address(dns))
        {
            fprintf(stderr,"The value %s is not an valid address\n",dns);
            exit(EXIT_FAILURE);
        }
        dns2 = argv[3];
        if(!exalt_is_address(dns2))
        {
            fprintf(stderr,"The value %s is not an valid address\n",dns);
            exit(EXIT_FAILURE);
        }

        exalt_dbus_dns_replace(conn,dns,dns2);
    }
}

void ethernet_list(int argc, char** argv)
{
    if(strcmp(argv[1],"list_get")==0)
    {
        exalt_dbus_eth_list_get(conn);
        exalt_dbus_wireless_list_get(conn);
    }
}

void ethernet(int argc, char** argv)
{
    char* iface;

    if(argc < 3)
    {
        fprintf(stderr, "We need an interface name (eth0, ipw1 ...)\n");
        exit(EXIT_FAILURE);
    }
    iface = argv[2];
    if(strcmp(argv[1],"ip_get")==0)
        exalt_dbus_eth_ip_get(conn,iface);
    else if(strcmp(argv[1],"netmask_get")==0)
        exalt_dbus_eth_netmask_get(conn,iface);
    else if(strcmp(argv[1],"gateway_get")==0)
        exalt_dbus_eth_gateway_get(conn,iface);
    else if(strcmp(argv[1],"wireless_is")==0)
        exalt_dbus_eth_wireless_is(conn,iface);
    else if(strcmp(argv[1],"link_is")==0)
        exalt_dbus_eth_link_is(conn,iface);
    else if(strcmp(argv[1],"up_is")==0)
        exalt_dbus_eth_up_is(conn,iface);
    else if(strcmp(argv[1],"dhcp_is")==0)
        exalt_dbus_eth_dhcp_is(conn,iface);
    else if(strcmp(argv[1],"cmd_get")==0)
        exalt_dbus_eth_command_get(conn,iface);
    else if(strcmp(argv[1],"cmd_set")==0)
    {
        if(argc<4)
        {
            fprintf(stderr,"We need a command. \n");
            exit (EXIT_FAILURE);
        }
        exalt_dbus_eth_command_set(conn,iface,argv[3]);
    }
    else if(strcmp(argv[1],"up")==0)
        exalt_dbus_eth_up(conn,iface);
    else if(strcmp(argv[1],"down")==0)
        exalt_dbus_eth_down(conn,iface);
}

void wireless(int argc, char** argv)
{
    char* iface;

    if(argc < 3)
    {
        fprintf(stderr, "We need an interface name (eth0, ipw1 ...)\n");
        exit(EXIT_FAILURE);
    }
    iface = argv[2];
    if(strcmp(argv[1],"essid_get")==0)
        exalt_dbus_wireless_essid_get(conn,iface);
    else if(strcmp(argv[1],"wpasupplicant_driver_get")==0)
        exalt_dbus_wireless_wpasupplicant_driver_get(conn,iface);
    else if(strcmp(argv[1],"wpasupplicant_driver_set")==0)
    {
        if(argc<4)
        {
            fprintf(stderr,"We need a driver. \n");
            exit (EXIT_FAILURE);
        }
        exalt_dbus_wireless_wpasupplicant_driver_set(conn,iface,argv[3]);
    }
}

int main(int argc, char** argv)
{
    exalt_dbus_init();
    conn = exalt_dbus_connect();

    exalt_dbus_response_notify_set(conn,response,conn);

    if(argc>1 && (strcmp(argv[1],"dns_add")==0
            || strcmp(argv[1],"dns_del")==0
            || strcmp(argv[1],"dns_replace")==0
            || strcmp(argv[1],"dns_get")==0
            ))
        dns(argc,argv);
    else if(argc>1 && ( strcmp(argv[1],"list_get")==0
                ))
        ethernet_list(argc,argv);
    else if(argc>1 && (strcmp(argv[1],"ip_get")==0
                || strcmp(argv[1],"netmask_get")==0
                || strcmp(argv[1],"gateway_get")==0
                || strcmp(argv[1],"wireless_is")==0
                || strcmp(argv[1],"up_is")==0
                || strcmp(argv[1],"link_is")==0
                || strcmp(argv[1],"dhcp_is")==0
                || strcmp(argv[1],"cmd_get")==0
                || strcmp(argv[1],"cmd_set")==0
                || strcmp(argv[1],"down")==0
                || strcmp(argv[1],"up")==0
            ))
        ethernet(argc ,argv);
    else if(argc>1 && (strcmp(argv[1],"essid_get")==0
                || strcmp(argv[1],"wpasupplicant_driver_get")==0
                || strcmp(argv[1],"wpasupplicant_driver_set")==0
                ))
        wireless(argc,argv);
    else
        help();

    ecore_main_loop_begin();

    exalt_dbus_free(&conn);
    exalt_dbus_shutdown();

    return 1;
}


void help()
{
    printf("exalt-command method args\n\n"
            "dns_get \t\t\t: print the dns list\n"
            "dns_add ip_address \t\t: add the ip_address as dns\n"
            "dns_del ip_addresse \t\t: remove the dns\n"
            "dns_replace ip1 ip2 \t\t: replace the dns ip1 by ip2\n"
            "\n"
            "list_get\t\t\t: print the list of interfaces (wired and wireless interface)\n"
            "\n"
            "ip_get eth0\t\t\t: get the ip address of the interface eth0\n"
            "netmask_get eth0\t\t: get the netmask address of the interface eth0\n"
            "gateway_get eth0\t\t: get the gateway address of the interface eth0\n"
            "wireless_is eth0\t\t: print yes if eth0 is a wireless interface, else print no\n"
            "link_is eth0\t\t\t: print yes if eth0 is link, else print no\n"
            "up_is eth0\t\t\t: print yes if eth0 is up, else print no\n"
            "dhcp_is eth0\t\t\t: print yes if eth0 use a DHCP mode, else print no\n"
            "up eth0\t\t\t\t: up eth0\n"
            "down eth0\t\t\t: down eth0\n"
            "cmd_set eth0\t\t\t: set the command run when eth0 get an IP address\n"
            "cmd_get eth0\t\t\t: print the command run when eth0 get an IP address\n"
            );

    exit(EXIT_FAILURE);
}

