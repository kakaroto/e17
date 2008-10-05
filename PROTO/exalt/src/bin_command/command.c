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
    if(!exalt_dbus_response_valid_is(response)) \
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
        case EXALT_DBUS_RESPONSE_IFACE_IP_GET:
            printf("%s IP address:\n",exalt_dbus_response_iface_get(response));
            print_response_error();
            printf("%s\n",exalt_dbus_response_address_get(response));
            break;
    }

    ecore_main_loop_quit();
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
    {
        exalt_dbus_eth_ip_get(conn,iface);
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
    else if(argc>1 && strcmp(argv[1],"ip_get")==0)
        ethernet(argc ,argv);
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
            "ip_get eth0\t\t\t: get the ip address of the interface eth0\n");
    exit(EXIT_FAILURE);
}

