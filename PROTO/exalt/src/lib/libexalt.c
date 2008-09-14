/** @file libexalt.c */
#include "libexalt.h"
#include "libexalt_private.h"
#include "config.h"
#include <E_Hal.h>

/**
 * @addtogroup Exalt_General
 * @{
 */

Exalt_Ethernets exalt_eth_interfaces;


/**
 * @brief initialise the library
 */
int exalt_init()
{
    exalt_eth_interfaces.is_launch = 0;
    exalt_eth_interfaces.ethernets = ecore_list_new();
    exalt_eth_interfaces.ethernets->free_func =  ECORE_FREE_CB(exalt_eth_free);
    ecore_list_init(exalt_eth_interfaces.ethernets);

    exalt_eth_interfaces.default_routes = ecore_list_new();


    exalt_eth_interfaces.eth_cb = NULL;
    exalt_eth_interfaces.eth_cb_user_data = NULL;

    exalt_eth_interfaces.wireless_scan_cb = NULL;
    exalt_eth_interfaces.wireless_scan_cb_user_data = NULL;

    exalt_eth_interfaces.we_version = iw_get_kernel_we_version();

    //test if we have the administrator right
    if(getuid()==0)
        exalt_eth_interfaces.admin = 1;
    else
        exalt_eth_interfaces.admin = 0;

    return 1;
}

/**
 * @brief Launch the library
 * load cards and watch events
 */
int exalt_main()
{
    struct sockaddr_nl addr;

    EXALT_ASSERT_ADV(exalt_eth_interfaces.is_launch<=0,return 0,"Exalt is launch");

    e_dbus_init();
    exalt_eth_interfaces.dbus_conn = e_dbus_bus_get(DBUS_BUS_SYSTEM);
   EXALT_ASSERT_RETURN(exalt_eth_interfaces.dbus_conn!=NULL);
    e_hal_manager_find_device_by_capability(exalt_eth_interfaces.dbus_conn, "net", _exalt_cb_find_device_by_capability_net, NULL);
    e_dbus_signal_handler_add(exalt_eth_interfaces.dbus_conn, "org.freedesktop.Hal", "/org/freedesktop/Hal/Manager", "org.freedesktop.Hal.Manager", "DeviceAdded", _exalt_cb_signal_device_added, NULL);
    e_dbus_signal_handler_add(exalt_eth_interfaces.dbus_conn, "org.freedesktop.Hal", "/org/freedesktop/Hal/Manager", "org.freedesktop.Hal.Manager", "DeviceRemoved", _exalt_cb_signal_device_removed, NULL);


    /* set up a rtnetlink socket */
    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV4_ROUTE | RTMGRP_NOTIFY | RTMGRP_NEIGH | RTMGRP_TC;

    exalt_eth_interfaces.rtlink_sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);

    if(exalt_eth_interfaces.rtlink_sock < 0) {
        perror("socket()");
        return -1;
    }

    if(bind(exalt_eth_interfaces.rtlink_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind()");
        return -1;
    }

    exalt_eth_interfaces.rtlink_watch = ecore_main_fd_handler_add(exalt_eth_interfaces.rtlink_sock, ECORE_FD_READ,_exalt_rtlink_watch_cb, NULL,NULL,NULL);

    exalt_eth_interfaces.is_launch = 1;

    return 1;
}




/**
 * @brief test if a string is a valid address
 * @param ip the string ip
 * return Return 1 if sucess, else 0
 */
short exalt_is_address(const char* ip)
{
    Exalt_Regex *r;
    short res;

    if(!ip) return 0;

    r = exalt_regex_new(ip,REGEXP_IS_IP,0);
    res = exalt_regex_execute(r);
    exalt_regex_free(&r);
    return res;
}

/**
 * @brief test if a string is a valid essid
 * @param essid the essid
 * @return Return 1 if success, else 0
 */
short exalt_is_essid(const char* essid)
{
    if(!essid) return 0;
    return strlen(essid);
}

/**
 * @brief test if a string is a valid key
 * @param key the key
 * @param encryption_mode the password mode (WEP, WPA ...)
 * @return Return 1 if success, else 0
 */
short exalt_is_key(const char* key, Exalt_Enum_Encryption_Mode encryption_mode)
{
    Exalt_Regex *r;
    short res;
    if(encryption_mode == EXALT_ENCRYPTION_NONE)
        return 1;

    if(!key)
        return -0;

    if(encryption_mode == EXALT_ENCRYPTION_WEP_ASCII)
    {
        if(strlen(key)>0)
            return 1;
        else
            return 0;
    }

    if(encryption_mode == EXALT_ENCRYPTION_WEP_HEXA)
    {
        r = exalt_regex_new(strdup(key), REGEXP_IS_WEP_HEXA, 0);
        res = exalt_regex_execute(r);
        exalt_regex_free(&r);
        return res;
    }

    if(encryption_mode == EXALT_ENCRYPTION_WPA_PSK_TKIP_ASCII
            || encryption_mode ==EXALT_ENCRYPTION_WPA_PSK_CCMP_ASCII
            || encryption_mode == EXALT_ENCRYPTION_WPA2_PSK_TKIP_ASCII
            || encryption_mode == EXALT_ENCRYPTION_WPA2_PSK_CCMP_ASCII)
    {
        if(strlen(key)>=8 && strlen(key)<=63)
            return 1;
        else
            return 0;
    }

    return 0;
}

/**
 * @brief return if you have the administrator right
 * @return Return 1 if yes, else 0
 */
short exalt_is_admin()
{
    return exalt_eth_interfaces.admin;
}

/**
 * @brief return if libexalt is built with the support of wpa_supplicant
 * @return Return 1 if yes, else 0
 */
short exalt_wpasupplicant_is_support()
{
#ifdef HAVE_WPA_SUPPLICANT
    return 1;
#else
    return 0;
#endif
}


/**
 * @brief return if libexalt is built with the support of dhcp
 * @return Return 1 if yes, else 0
 */
short exalt_dhcp_is_support()
{
#ifdef HAVE_DHCP
    return 1;
#else
    return 0;
#endif
}


/**
 * @brief return if libexalt is built with the support of vpnc
 * @return Return 1 if yes, else 0
 */
short exalt_vpnc_is_support()
{
#ifdef HAVE_VPNC
    return 1;
#else
    return 0;
#endif
}



/**
 * @brief print an error
 * @param file the file
 * @param fct the function
 * @param line the line number
 * @param msg the message
 * @param ... a list of params
 */
void print_error(const char* file,const char* fct, int line, const char* msg, ...)
{
    va_list ap;
    va_start(ap,msg);
    fprintf(stderr,"%s: %s (%d)\n",file,fct,line);
    fprintf(stderr,"\t");
    vfprintf(stderr,msg,ap);
    fprintf(stderr,"\n\n");
    va_end(ap);
}

/** @} */

