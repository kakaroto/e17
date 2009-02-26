/** @file exalt_ethernet.c */
#include "exalt_ethernet.h"
#include "libexalt_private.h"
#include "../config.h"

/*
 * Private functions headers
 */

/*
 * TODO: remove up_without_apply if the new wpa_supplicant allows to do it
 * change exalt_eth_free to take a Exalt_Ethernet** and exalt_wireless_free to take a Exalt_Wireless**
 */

const char* _exalt_eth_save_ip_get(Exalt_Ethernet* eth);
const char* _exalt_eth_save_gateway_get(Exalt_Ethernet* eth);
const char* _exalt_eth_save_netmask_get(Exalt_Ethernet* eth);
int _exalt_eth_save_link_get(Exalt_Ethernet* eth);
int _exalt_eth_save_up_get(Exalt_Ethernet* eth);

int _exalt_eth_save_ip_set(Exalt_Ethernet* eth,const char* ip);
int _exalt_eth_save_netmask_set(Exalt_Ethernet* eth,const char* netmask);
int _exalt_eth_save_gateway_set(Exalt_Ethernet* eth,const char* gateway);
int _exalt_eth_save_link_set(Exalt_Ethernet* eth,short link);
int _exalt_eth_save_up_set(Exalt_Ethernet* eth,short up);

int _exalt_eth_udi_set(Exalt_Ethernet* eth,const char* udi);
int _exalt_eth_ifindex_set(Exalt_Ethernet* eth,int ifindex);
int _exalt_eth_name_set(Exalt_Ethernet* eth,const char* name);

void _exalt_cb_is_net(void *user_data, void *reply_data, DBusError *error);

void _exalt_cb_net_properties(void *data, void *reply_data, DBusError *error);

int _exalt_eth_udi_remove(const char* udi);


int _exalt_apply_timer(void *data);
int _exalt_eth_apply_dhcp(Exalt_Ethernet* eth);
int _exalt_eth_apply_static(Exalt_Ethernet *eth);



/**
 * @addtogroup Exalt_Ethernet
 */

struct Exalt_Ethernet
{
    char* name; //eth0, eth1...
    char* udi;
    int ifindex;

    Exalt_Connection *connection;
    Exalt_Wireless* wireless; //if null, the interface is not wireless

    char* _save_ip;
    char* _save_netmask;
    char* _save_gateway;
    short _save_link;
    short _save_up;

    pid_t apply_pid;
    Ecore_Timer *apply_timer;

    time_t dont_apply_after_up;
};


Exalt_Ethernet* exalt_eth_new(const char* name)
{
    struct iwreq wrq;
    Exalt_Ethernet* eth;

    eth = (Exalt_Ethernet*)calloc(1, sizeof(Exalt_Ethernet));
    EXALT_ASSERT_RETURN(eth!=NULL);

    _exalt_eth_name_set(eth,name);

    //test if the interface has a wireless extension
    strncpy(wrq.ifr_name, exalt_eth_name_get(eth), sizeof(wrq.ifr_name));
    if(exalt_ioctl(&wrq, SIOCGIWNAME))
        eth->wireless = exalt_wireless_new(eth);

    return eth;
}


void exalt_eth_ethernets_free()
{
    e_dbus_connection_close(exalt_eth_interfaces.dbus_conn);
    eina_list_free(exalt_eth_interfaces.ethernets);
}


void exalt_eth_free(void *data)
{
    Exalt_Ethernet* eth = data;
    EXALT_FREE(eth->name);
    EXALT_FREE(eth->udi);
    EXALT_FREE(eth->_save_ip);
    EXALT_FREE(eth->_save_netmask);
    EXALT_FREE(eth->_save_gateway);
    if(eth->connection)
        exalt_conn_free(&(eth->connection));
    if(exalt_eth_wireless_is(eth))
        exalt_wireless_free(&(eth->wireless));
    EXALT_FREE(eth);
}



int exalt_eth_ethernet_is(char* name)
{
    struct ifreq ifr;

    EXALT_ASSERT_RETURN(name!=NULL);

    strncpy(ifr.ifr_name,name,sizeof(ifr.ifr_name));
    if(!exalt_ioctl(&ifr, SIOCGIFHWADDR))
        return -1;

    return ifr.ifr_hwaddr.sa_family == ARPHRD_ETHER;
}


void exalt_eth_up(Exalt_Ethernet* eth)
{
    struct ifreq ifr;

    EXALT_ASSERT_RETURN_VOID(eth!=NULL);

    strncpy(ifr.ifr_name,exalt_eth_name_get(eth),sizeof(ifr.ifr_name));

    if( !exalt_ioctl(&ifr, SIOCGIFFLAGS))
        return ;

    ifr.ifr_flags |= IFF_UP;
    if( !exalt_ioctl(&ifr, SIOCSIFFLAGS))
        return ;
}

void exalt_eth_up_without_apply(Exalt_Ethernet* eth)
{
    struct ifreq ifr;

    EXALT_ASSERT_RETURN_VOID(eth!=NULL);

    strncpy(ifr.ifr_name,exalt_eth_name_get(eth),sizeof(ifr.ifr_name));

    if( !exalt_ioctl(&ifr, SIOCGIFFLAGS))
        return ;

    ifr.ifr_flags |= IFF_UP;
    if( !exalt_ioctl(&ifr, SIOCSIFFLAGS))
        return ;

    eth->dont_apply_after_up =  (int)time(NULL);
}


void exalt_eth_dontapplyafterup_set(Exalt_Ethernet * eth, time_t t)
{
    EXALT_ASSERT(eth!=NULL);
    eth->dont_apply_after_up = t;
}

time_t exalt_eth_dontapplyafterup_get(Exalt_Ethernet* eth)
{
    EXALT_ASSERT(eth!=NULL);
    return eth->dont_apply_after_up;
}


void exalt_eth_down(Exalt_Ethernet* eth)
{
    struct ifreq ifr;

    EXALT_ASSERT_RETURN_VOID(eth!=NULL);

    if(exalt_eth_wireless_is(eth))
        _exalt_wpa_stop(exalt_eth_wireless_get(eth));

    strncpy(ifr.ifr_name,exalt_eth_name_get(eth),sizeof(ifr.ifr_name));

    if( !exalt_ioctl(&ifr, SIOCGIFFLAGS))
        return ;

    ifr.ifr_flags &= ~IFF_UP;
    if( !exalt_ioctl(&ifr, SIOCSIFFLAGS))
        return ;
}



Eina_List* exalt_eth_list_get()
{
    return exalt_eth_interfaces.ethernets;
}




Exalt_Ethernet* exalt_eth_get_ethernet_bypos(int pos)
{
    return eina_list_nth(exalt_eth_interfaces.ethernets,pos);
}



Exalt_Ethernet* exalt_eth_get_ethernet_byname(const char* name)
{
    Exalt_Ethernet* eth;
    Eina_List *l;
    EXALT_ASSERT_RETURN(name!=NULL);

    EINA_LIST_FOREACH(exalt_eth_interfaces.ethernets,l,eth)
    {
        if(!strcmp(exalt_eth_name_get(eth),name))
            return eth;
    }

    return NULL;
}

Exalt_Ethernet* exalt_eth_get_ethernet_byudi(const char* udi)
{
    Exalt_Ethernet* eth;
    Eina_List *l;
    EXALT_ASSERT_RETURN(udi!=NULL);

    EINA_LIST_FOREACH(exalt_eth_interfaces.ethernets,l,eth)
    {
        if(!strcmp(exalt_eth_udi_get(eth),udi))
            return eth;
    }

    return NULL;
}


Exalt_Ethernet* exalt_eth_get_ethernet_byifindex(int ifindex)
{
    Exalt_Ethernet* eth;
    Eina_List* l;

    EINA_LIST_FOREACH(exalt_eth_interfaces.ethernets,l,eth)
   	{
		if(ifindex == exalt_eth_ifindex_get(eth))
	       	return eth;
	}

    return NULL;
}


#define EXALT_FCT_NAME exalt_eth
#define EXALT_STRUCT_TYPE Exalt_Ethernet

EXALT_GET(name,const char*)
EXALT_GET(udi,const char*)
EXALT_GET(ifindex,int)
EXALT_GET(connection,Exalt_Connection*)
EXALT_GET(wireless,Exalt_Wireless*)

#undef EXALT_FCT_NAME
#undef EXALT_STRUCT_TYPE


short exalt_eth_link_is(Exalt_Ethernet* eth)
{
    struct ifreq ifr;
    struct ethtool_value edata;
    int res;


    EXALT_ASSERT_RETURN(eth!=NULL);

    if(exalt_eth_wireless_is(eth))
        return 1;

    strncpy(ifr.ifr_name,exalt_eth_name_get(eth),sizeof(ifr.ifr_name));
    memset(&edata, 0, sizeof(edata));
    edata.cmd = ETHTOOL_GLINK;
    ifr.ifr_data = (caddr_t)&edata;

    if( !(res = exalt_ioctl(&ifr, SIOCETHTOOL)))
        return res; //if an interface doesn't support SIOCETHTOOL, we return true

    return edata.data ? 1 : 0;
}

short exalt_eth_connection_set(Exalt_Ethernet* eth, Exalt_Connection* c)
{
    EXALT_ASSERT_RETURN(eth!=NULL);
    EXALT_ASSERT_RETURN(c!=NULL);

    if(eth->connection && eth->connection != c)
        exalt_conn_free(&(eth->connection));
    eth->connection = c;
    return 1;
}

char* exalt_eth_ip_get(Exalt_Ethernet* eth)
{
    struct sockaddr_in sin = { AF_INET };
    struct ifreq ifr;

    EXALT_ASSERT_RETURN(eth!=NULL);

    strncpy(ifr.ifr_name,exalt_eth_name_get(eth),sizeof(ifr.ifr_name));
    ifr.ifr_addr = *(struct sockaddr *) &sin;

    if( !exalt_ioctl(&ifr, SIOCGIFADDR))
        return NULL;

    sin = *(struct sockaddr_in*) &ifr.ifr_addr;

    return strdup(inet_ntoa(sin.sin_addr));
}



char* exalt_eth_netmask_get(Exalt_Ethernet* eth)
{
    struct sockaddr_in sin = { AF_INET };
    struct ifreq ifr;

    EXALT_ASSERT_RETURN(eth!=NULL);

    strncpy(ifr.ifr_name,exalt_eth_name_get(eth),sizeof(ifr.ifr_name));
    ifr.ifr_addr = *(struct sockaddr *) &sin;
    if( !exalt_ioctl(&ifr, SIOCGIFNETMASK) )
        return NULL;

    sin = *(struct sockaddr_in*) &ifr.ifr_addr;
    return strdup(inet_ntoa(sin.sin_addr));
}



char* exalt_eth_gateway_get(Exalt_Ethernet* eth)
{
    FILE* f;
    char buf[1024];
    char* fmt;
    int num, iflags, metric, refcnt, use, mss, window, irtt;
    char iface[16];
    char gate_addr[128], net_addr[128];
    char mask_addr[128];
    short find = 0;

    EXALT_ASSERT_RETURN(eth!=NULL);

    f = fopen(EXALT_PATH_ROUTE,"r");
    EXALT_ASSERT_RETURN(f!=NULL);
    fmt = proc_gen_fmt(EXALT_PATH_ROUTE, 0, f,
            "Iface", "%16s",
            "Destination", "%128s",
            "Gateway", "%128s",
            "Flags", "%X",
            "RefCnt", "%d",
            "Use", "%d",
            "Metric", "%d",
            "Mask", "%128s",
            "MTU", "%d",
            "Window", "%d",
            "IRTT", "%d",
            NULL);

    while(fgets(buf,1024,f) && !find)
    {
        num = sscanf(buf, fmt,
                iface, net_addr, gate_addr,
                &iflags, &refcnt, &use, &metric, mask_addr,
                &mss, &window, &irtt);
        if(strcmp(iface,exalt_eth_name_get(eth))==0 && strcmp(net_addr,"00000000")==0)
            find = 1;
    }
    fclose(f);
    EXALT_FREE(fmt);
    if(find)
        return exalt_addr_hexa_to_dec(gate_addr);
    else
        return NULL;
}

int exalt_eth_gateway_delete(Exalt_Ethernet* eth)
{
    Default_Route* route;
    int done = 0;
    struct rtentry rt;
    struct sockaddr_in sin = { AF_INET };
    Eina_List *l;

    memset((char *) &rt, 0, sizeof(struct rtentry));
    rt.rt_flags = ( RTF_UP | RTF_GATEWAY );
    sin.sin_addr.s_addr = inet_addr ("0.0.0.0");
    rt.rt_dst = *(struct sockaddr *) &sin;
    rt.rt_dev = NULL;
    EXALT_STRDUP(rt.rt_dev , exalt_eth_name_get(eth));
    while (exalt_ioctl(&rt, SIOCDELRT))
        usleep(100);

    EXALT_FREE(rt.rt_dev);

    //And we remove this route from the list, the route is supposed to be the first
    route = eina_list_data_get(exalt_eth_interfaces.default_routes);
    if(route && strcmp(exalt_eth_name_get(eth),route->interface)==0)
    {
        exalt_eth_interfaces.default_routes =
            eina_list_remove(exalt_eth_interfaces.default_routes,route);
        EXALT_FREE(route->interface);
        EXALT_FREE(route->gateway);
        EXALT_FREE(route);
    }



    //search an old valid route
    //a route is valid if:
    //- the interface is up
    //- the interface has a ip address
    l=exalt_eth_interfaces.default_routes;
    while(!done && (route = eina_list_data_get(l)))
    {
        Exalt_Ethernet *eth;
        struct rtentry rt;
        eth = exalt_eth_get_ethernet_byname(route->interface);
        if(eth && exalt_eth_up_is(eth) && exalt_is_address(exalt_eth_ip_get(eth)))
        {
            //we will use this route
            done = 1;

            memset((char *) &rt, 0, sizeof(struct rtentry));
            rt.rt_flags = ( RTF_UP | RTF_GATEWAY );
            sin.sin_addr.s_addr = inet_addr (route->gateway);
            rt.rt_gateway = *(struct sockaddr *) &sin;
            sin.sin_addr.s_addr = inet_addr ("0.0.0.0");
            rt.rt_dst = *(struct sockaddr *) &sin;
            rt.rt_metric = 0;
            rt.rt_dev = strdup(route->interface);

            if ( !exalt_ioctl(&rt, SIOCADDRT))
                return -1;

            EXALT_FREE(rt.rt_dev);
        }

        //we remove this route from the list
        exalt_eth_interfaces.default_routes =
            eina_list_remove(exalt_eth_interfaces.default_routes,route);

        EXALT_FREE(route->interface);
        EXALT_FREE(route->gateway);
        EXALT_FREE(route);

        l=eina_list_next(l);
    }
    return 1;
}

int exalt_eth_dhcp_is(Exalt_Ethernet* eth)
{
    Exalt_Connection *c;
    EXALT_ASSERT_RETURN(eth!=NULL);

    if( (c=exalt_eth_connection_get(eth)))
        return exalt_conn_mode_get(c) == EXALT_DHCP;
    else
        return -1;
}



int exalt_eth_up_is(Exalt_Ethernet* eth)
{
    struct ifreq ifr;

    EXALT_ASSERT_RETURN(eth!=NULL);

    strncpy(ifr.ifr_name,exalt_eth_name_get(eth),sizeof(ifr.ifr_name));

    if( !exalt_ioctl(&ifr,SIOCGIFFLAGS) )
        return 0;

    if( (ifr.ifr_flags & IFF_UP))
        return 1;
    else if( !(ifr.ifr_flags & IFF_UP))
        return 0;
    else
        return 0;
}



int exalt_eth_wireless_is(Exalt_Ethernet* eth)
{
    EXALT_ASSERT_RETURN(eth!=NULL);
    return eth->wireless != NULL;
}




int exalt_eth_scan_cb_set(Exalt_Wifi_Scan_Cb fct, void * user_data)
{
    exalt_eth_interfaces.wireless_scan_cb = fct;
    exalt_eth_interfaces.wireless_scan_cb_user_data = user_data;
    return 1;
}

int exalt_eth_cb_set(Exalt_Eth_Cb fct, void * user_data)
{
    exalt_eth_interfaces.eth_cb = fct;
    exalt_eth_interfaces.eth_cb_user_data = user_data;
    return 1;
}




int exalt_eth_conn_apply(Exalt_Ethernet* eth, Exalt_Connection *c)
{
    int res;

    EXALT_ASSERT_RETURN(eth!=NULL);
    //if the connection is not valid, we send the information as the configuration is done
    //else a application will wait a very long time the end of the configuration
    //(_exalt_apply_timer())
    EXALT_ASSERT_CUSTOM_RET(exalt_conn_valid_is(c),
            eth->apply_pid = -1; _exalt_apply_timer(eth));

    //apply start
    if(exalt_eth_interfaces.eth_cb)
        exalt_eth_interfaces.eth_cb(eth,EXALT_ETH_CB_ACTION_CONN_APPLY_START,exalt_eth_interfaces.eth_cb_user_data);

    exalt_eth_connection_set(eth,c);

    eth->apply_pid = fork();
    EXALT_ASSERT_ADV(eth->apply_pid>=0,
            eth->apply_pid = -1; _exalt_apply_timer(eth),
            "eth->apply_pid>=0 failed");

    if(eth->apply_pid == 0)
    {
        if(exalt_eth_wireless_is(eth))
            exalt_wireless_conn_apply(exalt_eth_wireless_get(eth));

        //remove the old gateway
        exalt_eth_gateway_delete(eth);

        if(exalt_conn_mode_get(c) == EXALT_DHCP)
            res = _exalt_eth_apply_dhcp(eth);
        else
            res = _exalt_eth_apply_static(eth);

        exit(res);
    }

    eth->apply_timer = ecore_timer_add(1 ,_exalt_apply_timer,eth);

    return 1;
}


void exalt_eth_printf()
{
    void *data;
    Exalt_Ethernet* eth;
    Eina_List *l;

    EINA_LIST_FOREACH(exalt_eth_interfaces.ethernets,l,eth)
    {
        eth = data;
        printf("###   %s   ###\n",eth->name);
        printf("Up: %d\n",exalt_eth_up_is(eth));
        if(exalt_eth_dhcp_is(eth))
            printf("-- DHCP mode --\n");

        printf("ip: %s\n",exalt_eth_ip_get(eth));
        printf("mask: %s\n",exalt_eth_netmask_get(eth));
        printf("gateway: %s\n",exalt_eth_gateway_get(eth));
        printf("Wifi: %s\n",(eth->wireless==NULL?"no":"yes"));
    }
}




/** @} */



/*
 * Private functions bodies
 */

/**
 * @brief get the save ip address of the interface "eth"
 * @param eth the interface
 * @return Returns the ip address
 */
const char* _exalt_eth_save_ip_get(Exalt_Ethernet* eth)
{
    EXALT_ASSERT_RETURN(eth!=NULL);
    return eth->_save_ip;
}

/**
 * @brief get the save default gateway of the interface "eth"
 * @param eth the interface
 * @return Returns the gateway
 */
const char* _exalt_eth_save_gateway_get(Exalt_Ethernet* eth)
{
    EXALT_ASSERT_RETURN(eth!=NULL);
    return eth->_save_gateway;
}

/**
 * @brief get the save netmask of the interface "eth"
 * @param eth the interface
 * @return Returns the netmask
 */
const char* _exalt_eth_save_netmask_get(Exalt_Ethernet* eth)
{
    EXALT_ASSERT_RETURN(eth!=NULL);
    return eth->_save_netmask;
}

/**
 * @brief get the save link state of the interface "eth"
 * @param eth the interface
 * @return Returns 1 or 0
 */
int _exalt_eth_save_link_get(Exalt_Ethernet* eth)
{
    EXALT_ASSERT_RETURN(eth!=NULL);
    return eth->_save_link;
}

/**
 * @brief get the save up stat of the interface "eth"
 * @param eth the interface
 * @return Returns 1 or 0
 */
int _exalt_eth_save_up_get(Exalt_Ethernet* eth)
{
    EXALT_ASSERT_RETURN(eth!=NULL);
    return eth->_save_up;
}


/**
 * @brief set the save ip address of the interface "eth"
 * @param eth the interface
 * @param ip the new ip address
 * @return Returns 1 if the save ip address is apply, else 0
 */
int _exalt_eth_save_ip_set(Exalt_Ethernet* eth,const char* ip)
{
    EXALT_ASSERT_RETURN(eth!=NULL);
    EXALT_ASSERT_RETURN(exalt_is_address(ip) || ip==NULL);

    EXALT_FREE(eth->_save_ip);
    if(ip)
        eth->_save_ip=strdup(ip);
    return 1;
}

/**
 * @brief set the save netmask of the interface "eth"
 * @param eth the interface
 * @param netmask the new netmask
 * @return Returns 1 if the netmask is apply, else 0
 */
int _exalt_eth_save_netmask_set(Exalt_Ethernet* eth,const char* netmask)
{
    EXALT_ASSERT_RETURN(eth!=NULL);
    EXALT_ASSERT_RETURN(exalt_is_address(netmask) || netmask==NULL);

    EXALT_FREE(eth->_save_netmask);
    if(netmask)
        eth->_save_netmask=strdup(netmask);
    return 1;
}

/**
 * @brief set the save gateway of the interface "eth"
 * @param eth the interface
 * @param gateway the new gateway
 * @return Returns 1 if the new gateway is apply, else 0
 */
int _exalt_eth_save_gateway_set(Exalt_Ethernet* eth,const char* gateway)
{
    EXALT_ASSERT_RETURN(eth!=NULL);
    EXALT_ASSERT_RETURN(exalt_is_address(gateway) || gateway==NULL);

    EXALT_FREE(eth->_save_gateway);
    if(gateway)
        eth->_save_gateway=strdup(gateway);
    return 1;
}

/**
 * @brief set the save link state of the interface "eth"
 * @param eth the interface
 * @param link the state link (1 or 0)
 * @return Returns 1 if the new stat is apply,else 0
 */
int _exalt_eth_save_link_set(Exalt_Ethernet* eth,short link)
{
    EXALT_ASSERT_RETURN(eth!=NULL);
    eth->_save_link=link;
    return 1;
}
/**
 * @brief set the save up state of the interface "eth"
 * @param eth the interface
 * @param up the up state (1 or 0)
 * @return Returns 1 if the new state is apply,else 0
 */
int _exalt_eth_save_up_set(Exalt_Ethernet* eth,short up)
{
    EXALT_ASSERT_RETURN(eth!=NULL);
    eth->_save_up=up;
    return 1;
}


/**
 * @brief set the name of the interface "eth"
 * @param eth the interface
 * @param name the new name
 * @return Returns 1 if the new name is apply, else 0
 */
int _exalt_eth_name_set(Exalt_Ethernet* eth, const char* name)
{
    EXALT_ASSERT_RETURN(eth!=NULL);
    EXALT_ASSERT_RETURN(name!=NULL);

    EXALT_FREE(eth->name);
    eth->name=strdup(name);
    return 1;
}


/**
 * @brief set the udi of the interface "eth"
 * @param eth the interface
 * @param udi the new udi
 * @return Returns 1 if udi is apply, else -1
 */
int _exalt_eth_udi_set(Exalt_Ethernet* eth,const char* udi)
{
    EXALT_ASSERT_RETURN(eth!=NULL);
    EXALT_ASSERT_RETURN(udi!=NULL);

    EXALT_FREE(eth->udi);
    eth->udi=strdup(udi);
    return 1;
}

/**
 * @brief set the ifindex of the interface "eth"
 * @param eth the interface
 * @param ifindex the ifindex
 * @return Returns 1 if the new ifindex is apply,else -1
 */
int _exalt_eth_ifindex_set(Exalt_Ethernet* eth,int ifindex)
{
    EXALT_ASSERT_RETURN(eth!=NULL);
    eth->ifindex=ifindex;
    return 1;
}


/**
 * @brief remove an interface by his udi
 * @param udi the udi
 * @return Returns 1 if the interface is remove, else -1
 */
int _exalt_eth_udi_remove(const char* udi)
{
    Eina_List *l;
    Exalt_Ethernet* eth;

    EINA_LIST_FOREACH(exalt_eth_interfaces.ethernets,l,eth)
    {
        if(udi && !strcmp(exalt_eth_udi_get(eth),udi))
        {
            if(exalt_eth_interfaces.eth_cb)
                exalt_eth_interfaces.eth_cb(eth,EXALT_ETH_CB_ACTION_REMOVE,exalt_eth_interfaces.eth_cb_user_data);

            exalt_eth_interfaces.ethernets =
                eina_list_remove(exalt_eth_interfaces.ethernets,eth);

            return 1;
        }
    }

    return -1;
}


/**
 * @brief called when the kernel send an piece of information
 * An interface is link, has a new address ...
 */
int _exalt_rtlink_watch_cb(void *data, Ecore_Fd_Handler *fd_handler)
{
    int fd;
    ssize_t bin;
    struct sockaddr_nl addr;
    unsigned short rtlen;
    char buf[4096];
    struct ifinfomsg *ifmsg;
    struct ifaddrmsg *ifaddr;
    struct rtmsg *ifroute;
    struct nlmsghdr *nh;
    struct iovec iov = { buf, sizeof(buf) };
    struct msghdr msg = { (void *)&addr, sizeof(addr), &iov, 1, NULL, 0, 0 };
    Exalt_Ethernet* eth;
    void* data_l;
    Eina_List* l, *l2;
    char* str;
    const char* str2;

    fd = exalt_eth_interfaces.rtlink_sock;

    bin = recvmsg(fd, &msg, 0);
    EXALT_ASSERT_RETURN(bin>=0);

    for(nh = (struct nlmsghdr *)buf; NLMSG_OK(nh, bin);
            nh = NLMSG_NEXT(nh, bin))
    {
        switch(nh->nlmsg_type)
        {
            case RTM_NEWLINK:
            case RTM_DELLINK:
                ifmsg = NLMSG_DATA(nh);
                rtlen = RTM_PAYLOAD(nh);
                EXALT_ASSERT_ADV(rtlen>=sizeof(struct ifinfomsg),break,
                        "Bad rtnetlink packet (%d < %d)", rtlen,
                        sizeof(struct ifinfomsg));

                //printf("ifmsg: { %d, %d, %d, 0x%x, 0x%x }\n",
                //        ifmsg->ifi_family, ifmsg->ifi_type,
                //        ifmsg->ifi_index, ifmsg->ifi_flags,
                //        ifmsg->ifi_change);

                eth = exalt_eth_get_ethernet_byifindex(ifmsg->ifi_index);
                if(!eth)
                    break;

                if(_exalt_eth_save_up_get(eth) != exalt_eth_up_is(eth))
                {
                    _exalt_eth_save_up_set(eth, exalt_eth_up_is(eth));
                    if(exalt_eth_up_is(eth) && exalt_eth_interfaces.eth_cb)
                        exalt_eth_interfaces.eth_cb(eth,EXALT_ETH_CB_ACTION_UP,exalt_eth_interfaces.eth_cb_user_data);
                    else if(exalt_eth_interfaces.eth_cb)
                        exalt_eth_interfaces.eth_cb(eth,EXALT_ETH_CB_ACTION_DOWN,exalt_eth_interfaces.eth_cb_user_data);
                }

                if(_exalt_eth_save_link_get(eth) != exalt_eth_link_is(eth))
                {
                    _exalt_eth_save_link_set(eth, exalt_eth_link_is(eth));

                    if(exalt_eth_link_is(eth) && exalt_eth_interfaces.eth_cb)
                        exalt_eth_interfaces.eth_cb(eth,EXALT_ETH_CB_ACTION_LINK,exalt_eth_interfaces.eth_cb_user_data);
                    else if(exalt_eth_interfaces.eth_cb)
                        exalt_eth_interfaces.eth_cb(eth,EXALT_ETH_CB_ACTION_UNLINK,exalt_eth_interfaces.eth_cb_user_data);
                }

                if(exalt_eth_wireless_is(eth))
                {
                    _exalt_rtlink_essid_change(exalt_eth_wireless_get(eth));
                }
                break;
            case RTM_NEWADDR:
            case RTM_DELADDR:
                ifaddr = NLMSG_DATA(nh);
                rtlen = RTM_PAYLOAD(nh);
                EXALT_ASSERT_ADV(rtlen>=sizeof(struct ifaddrmsg),break,
                        "Bad rtnetlink packet (%d < %d)", rtlen,
                        sizeof(struct ifaddrmsg));

                eth = exalt_eth_get_ethernet_byifindex(ifaddr->ifa_index);
                if(!eth)
                    break;

                str = exalt_eth_ip_get(eth);
                str2 = _exalt_eth_save_ip_get(eth);
                if((!str && str2)
                        || (str && !str2)
                        || (str && str2 && strcmp(str2,str) != 0))
                {
                    _exalt_eth_save_ip_set(eth, str);
                    if(exalt_eth_interfaces.eth_cb)
                        exalt_eth_interfaces.eth_cb(eth,EXALT_ETH_CB_ACTION_ADDRESS_NEW,exalt_eth_interfaces.eth_cb_user_data);
                }
                EXALT_FREE(str);

                str = exalt_eth_netmask_get(eth);
                str2 = _exalt_eth_save_netmask_get(eth);
                if((!str && str2)
                        || (str && !str2)
                        || (str && str2 && strcmp(str2,str) != 0))
                {
                    _exalt_eth_save_netmask_set(eth, str);
                    if(exalt_eth_interfaces.eth_cb)
                        exalt_eth_interfaces.eth_cb(eth,EXALT_ETH_CB_ACTION_NETMASK_NEW,exalt_eth_interfaces.eth_cb_user_data);
                }
                EXALT_FREE(str);
            case RTM_NEWROUTE:
            case RTM_DELROUTE:
                ifroute = NLMSG_DATA(nh);
                rtlen = RTM_PAYLOAD(nh);
                EXALT_ASSERT_ADV(rtlen>=sizeof(struct rtmsg),break,
                        "Bad rtnetlink packet (%d < %d)", rtlen,
                        sizeof(struct rtmsg));

                //test for each interface if the route has been modified
                l = exalt_eth_list_get();
                EINA_LIST_FOREACH(l,l2,data_l)
                {
                    eth = data_l;
                    str = exalt_eth_gateway_get(eth);
                    str2 = _exalt_eth_save_gateway_get(eth);
                    if((!str && str2)
                            || (str && !str2)
                            || (str && str2 &&strcmp(str2,str ) != 0))
                    {
                        //if we have a new gateway, the gateway exist
                        if(exalt_is_address(exalt_eth_gateway_get(eth)))
                        {
                            Default_Route* route;
                            struct rtentry rt;
                            Eina_List* l3 = exalt_eth_list_get();
                            Eina_List *l4;
                            Exalt_Ethernet* eth2;
                            struct sockaddr_in sin = { AF_INET };

                            //we have a new route for this interface
                            //first: we remove the others default route because
                            //the system doesn't accept more than 1 default route
                            //foreach interface we get the current gateway
                            //    and removed it if it exist
                            EINA_LIST_FOREACH(l3,l4,eth2)
                            {
                                if(strcmp(exalt_eth_name_get(eth2),exalt_eth_name_get(eth))!=0)
                                {
                                    if(exalt_is_address(exalt_eth_gateway_get(eth2)))
                                    {
                                        memset((char *) &rt, 0, sizeof(struct rtentry));
                                        rt.rt_flags = ( RTF_UP | RTF_GATEWAY );
                                        sin.sin_addr.s_addr = inet_addr ("0.0.0.0");
                                        rt.rt_dst = *(struct sockaddr *) &sin;
                                        rt.rt_dev = NULL;
                                        EXALT_STRDUP(rt.rt_dev , exalt_eth_name_get(eth2));
                                        while (exalt_ioctl(&rt, SIOCDELRT))
                                            usleep(100);

                                        EXALT_FREE(rt.rt_dev);
                                    }
                                }
                            }


                            //second: we add the new route in the route list
                            route = malloc(sizeof(Default_Route));
                            EXALT_STRDUP(route->interface,exalt_eth_name_get(eth));
                            EXALT_STRDUP(route->gateway,exalt_eth_gateway_get(eth));
                            exalt_eth_interfaces.default_routes =
                                eina_list_prepend(exalt_eth_interfaces.default_routes, route);
                        }

                        //third: we update the current route of the interface
                        //and send a broadcast message
                        _exalt_eth_save_gateway_set(eth, str);
                        if(exalt_eth_interfaces.eth_cb)
                            exalt_eth_interfaces.eth_cb(eth,EXALT_ETH_CB_ACTION_GATEWAY_NEW,exalt_eth_interfaces.eth_cb_user_data);
                    }
                    EXALT_FREE(str);
                }
                break;
            default: /*printf("hd cb default!\n");*/break;
        }
    }
    return 1;
}

/**
 * @brief Check every secs if the configuration is apply on an interface
 * When it's done, the callback function is called
 * @param data the Exalt_Ethernet struct of the interface
 */
int _exalt_apply_timer(void *data)
{
    Exalt_Ethernet* eth = data;
    int res;
    int status;
    res = waitpid(eth->apply_pid,&status,WNOHANG);

    if(eth->apply_pid!=-1 && res == 0)
    {
        return 1;
    }


    Exalt_Connection* c = exalt_eth_connection_get(eth);
    const char *cmd = exalt_conn_cmd_after_apply_get(c);
    if(cmd && strcmp(cmd,"")!=0)
    {
        Ecore_Exe * exe;
        int status;

        EXALT_ASSERT_RETURN(eth!=NULL);

        exe = ecore_exe_run(cmd, NULL);
        waitpid(ecore_exe_pid_get(exe), &status, 0);
        ecore_exe_free(exe);
    }


    //apply done
    if(exalt_eth_interfaces.eth_cb)
        exalt_eth_interfaces.eth_cb(eth,EXALT_ETH_CB_ACTION_CONN_APPLY_DONE,exalt_eth_interfaces.eth_cb_user_data);

    return 0;
}

/**
 * @brief apply static address for the interface "eth"
 * @param eth the interface
 * @return Returns 1 if static address are apply, else 0
 */
int _exalt_eth_apply_static(Exalt_Ethernet *eth)
{
    struct sockaddr_in sin = { AF_INET };
    struct ifreq ifr;
    struct rtentry rt;
    Exalt_Connection *c;

    EXALT_ASSERT_RETURN(eth!=NULL);
    c = exalt_eth_connection_get(eth);
    EXALT_ASSERT_RETURN(c!=NULL);

    strncpy(ifr.ifr_name,exalt_eth_name_get(eth),sizeof(ifr.ifr_name));

    //apply the ip
    sin.sin_addr.s_addr = inet_addr (exalt_conn_ip_get(c));
    ifr.ifr_addr = *(struct sockaddr *) &sin;
    if( !exalt_ioctl(&ifr, SIOCSIFADDR) )
        return -1;

    //apply the netmask
    sin.sin_addr.s_addr = inet_addr (exalt_conn_netmask_get(c));
    ifr.ifr_addr = *(struct sockaddr *) &sin;
    if( !exalt_ioctl(&ifr, SIOCSIFNETMASK ) )
        return -1;


    if(!exalt_conn_gateway_get(c))
        return 1;

    //apply the new default gateway
    memset((char *) &rt, 0, sizeof(struct rtentry));
    rt.rt_flags = ( RTF_UP | RTF_GATEWAY );
    sin.sin_addr.s_addr = inet_addr (exalt_conn_gateway_get(c));
    rt.rt_gateway = *(struct sockaddr *) &sin;
    sin.sin_addr.s_addr = inet_addr ("0.0.0.0");
    rt.rt_dst = *(struct sockaddr *) &sin;
    rt.rt_metric = 0;
    rt.rt_dev = strdup(exalt_eth_name_get(eth));

    if ( !exalt_ioctl(&rt, SIOCADDRT))
        return -1;

    EXALT_FREE(rt.rt_dev);
    return 1;
}



/**
 * @brief apply the dhcp mode for the interface "eth"
 * @param eth the interface
 * @return Returns 1 if the dhcp is apply, else 0
 */
int _exalt_eth_apply_dhcp(Exalt_Ethernet* eth)
{
#ifdef HAVE_DHCP
    char* ret;
    Ecore_Exe * exe;
    int status;
    char command[1024];
    FILE* f;
    char buf[1024];
    pid_t pid;

    EXALT_ASSERT_RETURN(eth!=NULL);

    sprintf(command,COMMAND_DHCLIENT,exalt_eth_name_get(eth));
    exe = ecore_exe_run(command, NULL);
    waitpid(ecore_exe_pid_get(exe), &status, 0);
    ecore_exe_free(exe);

    //dhclient create a background process, we don't need it, we kill it.
    usleep(500);
    f = fopen(DHCLIENT_PID_FILE,"r");
    EXALT_ASSERT_RETURN(f!=NULL);
    ret = fgets(buf,1024,f);
    pid = atoi(buf);
    if(pid!=getpid());
        kill(pid,SIGKILL);
    fclose(f);
    remove(DHCLIENT_PID_FILE);
    return 1;
#else
    EXALT_ASSERT_ADV(0,return 0,"Your build of libexalt doesn't support dhcp");
#endif
}


/**
 * Hal functions
 */

/**
 * test if a hal device is a net device
 */
void _exalt_cb_is_net(void *user_data, void *reply_data, DBusError *error)
{
    char *udi = user_data;
    E_Hal_Device_Query_Capability_Return *ret = reply_data;
    int *action = malloc(sizeof(int));
    *action = EXALT_ETH_CB_ACTION_ADD;


    EXALT_ASSERT_RETURN_VOID(!dbus_error_is_set(error));

    if (ret && ret->boolean)
        e_hal_device_get_all_properties(exalt_eth_interfaces.dbus_conn, udi, _exalt_cb_net_properties, action);
}

/**
 * load the property of a net device from hal
 */
void _exalt_cb_net_properties(void *data, void *reply_data, DBusError *error)
{
    int action = *((int*)data);
    E_Hal_Properties *ret = reply_data;
    int err = 0;
    Exalt_Ethernet* eth;
    char* str;

    EXALT_ASSERT_RETURN_VOID(!dbus_error_is_set(error));
    str = e_hal_property_string_get(ret,"net.interface", &err);
    eth = exalt_eth_new(str);
    EXALT_FREE(str);

    str = e_hal_property_string_get(ret,"info.udi", &err);
    _exalt_eth_udi_set(eth,str);
    EXALT_FREE(str);

    _exalt_eth_ifindex_set(eth,e_hal_property_int_get(ret,"net.linux.ifindex", &err));

    str = exalt_eth_ip_get(eth);
    _exalt_eth_save_ip_set(eth,str);
    EXALT_FREE(str);

    str = exalt_eth_netmask_get(eth);
    _exalt_eth_save_netmask_set(eth,str);
    EXALT_FREE(str);

    str = exalt_eth_gateway_get(eth);
    _exalt_eth_save_gateway_set(eth,str);
    EXALT_FREE(str);

    _exalt_eth_save_link_set(eth, exalt_eth_link_is(eth));
    _exalt_eth_save_up_set(eth, exalt_eth_up_is(eth));

    //add the interface in the list
    exalt_eth_interfaces.ethernets =
        eina_list_append(exalt_eth_interfaces.ethernets, eth);

    if(exalt_eth_interfaces.eth_cb)
        exalt_eth_interfaces.eth_cb(eth,action,exalt_eth_interfaces.eth_cb_user_data);
}


/**
 * @brief load the net device list from Hal
 */
void _exalt_cb_find_device_by_capability_net(void *user_data, void *reply_data, DBusError *error)
{
    E_Hal_Manager_Find_Device_By_Capability_Return *ret = reply_data;
    char *device;
    int *action = malloc(sizeof(int));
    Eina_List *l;
    *action = EXALT_ETH_CB_ACTION_NEW;

    EXALT_ASSERT_RETURN_VOID(ret!=NULL);
    EXALT_ASSERT_RETURN_VOID(ret->strings!=NULL);
    EXALT_ASSERT_RETURN_VOID(!dbus_error_is_set(error));

    EINA_LIST_FOREACH(ret->strings,l,device)
    {
        e_hal_device_get_all_properties(exalt_eth_interfaces.dbus_conn, device, _exalt_cb_net_properties, action);
    }

    //EXALT_FREE(action);
}

/**
 * @brief call when a new device is added
 */
void _exalt_cb_signal_device_added(void *data, DBusMessage *msg)
{
    DBusError err;
    char *udi;
    int ret;

    dbus_error_init(&err);
    dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &udi, DBUS_TYPE_INVALID);
    ret = e_hal_device_query_capability(exalt_eth_interfaces.dbus_conn, udi, "net", _exalt_cb_is_net, strdup(udi));
}

/**
 * @brief call when a device is removed
 */
void _exalt_cb_signal_device_removed(void *data, DBusMessage *msg)
{
    DBusError err;
    char *udi;

    dbus_error_init(&err);
    dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &udi, DBUS_TYPE_INVALID);
    _exalt_eth_udi_remove(udi);
}

