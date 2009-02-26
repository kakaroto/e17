/** @file exalt_ethernet.h */
#ifndef EXALT_ETHERNET_H
#define EXALT_ETHERNET_H

/**
 * @brief informations about an interface
 * @structinfo
 */
typedef struct Exalt_Ethernet Exalt_Ethernet;

/**
 * @defgroup Exalt_Ethernet
 * @brief the Exalt_Ethernet struct contains all informations about an ethernet interface.
 * @{
 */

/**
 * @brief all interfaces
 * @structinfo
 */
typedef struct Exalt_Ethernets Exalt_Ethernets;

#include "libexalt.h"
#include "exalt_wireless.h"
#include "exalt_wireless_network.h"
#include "exalt_connection.h"
#include <Ecore_Data.h>
#include <Ecore.h>
#include <E_Hal.h>
#include <sys/socket.h>

#include <linux/types.h>
#include <linux/ethtool.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include <sys/wait.h>
#include <time.h>

extern Exalt_Ethernets exalt_eth_interfaces;

/** Define the list of notify action */
typedef enum Exalt_Enum_Action
{
    /** when we load the interface list */
    EXALT_ETH_CB_ACTION_NEW ,
    /** when we have a new interface */
    EXALT_ETH_CB_ACTION_ADD,
    /** when we have a remove interface */
    EXALT_ETH_CB_ACTION_REMOVE,
    /** when a known interface is up */
    EXALT_ETH_CB_ACTION_UP ,
    /** when a known interface is down */
    EXALT_ETH_CB_ACTION_DOWN,
    /** when a interface is link */
    EXALT_ETH_CB_ACTION_LINK ,
    /** when a interface is unlink */
    EXALT_ETH_CB_ACTION_UNLINK,
    /** when an essid change */
    EXALT_WIRELESS_CB_ACTION_ESSIDCHANGE ,

    /** when we have a new address */
    EXALT_ETH_CB_ACTION_ADDRESS_NEW ,
    /** when we have a new netmask */
    EXALT_ETH_CB_ACTION_NETMASK_NEW ,
    /** when we have a new gateway */
    EXALT_ETH_CB_ACTION_GATEWAY_NEW,


    /** when we start applying a connection */
    EXALT_ETH_CB_ACTION_CONN_APPLY_START,
    /** when the connection is applied */
    EXALT_ETH_CB_ACTION_CONN_APPLY_DONE,

    /** when we configure if we want wait (or not) that the interface is init during the boot process (only used by the daemon exaltd) */
    EXALTD_ETH_CB_WAITINGBOOT_CHANGE,
    /** when the value of the timeout change (only used by the daemon exaltd) */
    EXALTD_ETH_CB_WAITINGBOOT_TIMEOUT_CHANGE
} Exalt_Enum_Action;

/** callback function used for notification when a new interface is add, new essid ... */
typedef void (*Exalt_Eth_Cb) (Exalt_Ethernet* eth, Exalt_Enum_Action action, void* user_data);


/** callback function used for notification during a scan */
typedef void (*Exalt_Wifi_Scan_Cb) (Exalt_Ethernet* eth, Eina_List* networks, void* user_data);


/**
 * @brief create a Exalt_Ethernet structure
 * @param name the name of the interface (eth0, ath3 ...)
 * @return Return a new Exalt_Ethernet structure
 */
Exalt_Ethernet* exalt_eth_new(const char* name);

/**
 * @brief free exalt_eth_interfaces
 */
void exalt_eth_ethernets_free();

/**
 * @brief free a Exalt_Ethernet structure
 * @param data a Exalt_Ethernet* structure
 */
void exalt_eth_free(void* data);

/**
 * @brief up the interface "eth"
 * @param eth the interface
 */
void exalt_eth_up(Exalt_Ethernet* eth);
/**
 * @brief down the interface eth"
 * @param eth the interface
 */
void exalt_eth_down(Exalt_Ethernet* eth);
/**
 * @brief get if the interface is activated
 * @param eth the interface
 * @return Returns 1 if the interface is activated, else 0
 */
int exalt_eth_up_is(Exalt_Ethernet * eth);

/**
 * @brief test if an interface is an ethernet interface
 * @param name the name of the interface
 * @return Return 1 if yes else 0
 */
int exalt_eth_ethernet_is(char* name);

/**
 * @brief print interface informations in the standard output
 */
void exalt_eth_printf();

/**
 * @brief get the list of all interfaces
 * @return Return a list of Exalt_Ethernet structure
 */
Eina_List* exalt_eth_list_get();
/**
 * @brief get an interface by his name
 * @param name the name
 * @return Returns the interface
 */
Exalt_Ethernet* exalt_eth_get_ethernet_byname(const char* name);
/**
 * @brief get an interface by his position in the interface list
 * @param pos the position
 * @return Returns the interface
 */
Exalt_Ethernet* exalt_eth_get_ethernet_bypos(int pos);
/**
 * @brief get an interface by his udi
 * @param udi the udi
 * @return Returns the interface
 */
Exalt_Ethernet* exalt_eth_get_ethernet_byudi(const char* udi);
/**
 * @brief get an interface by his ifindex
 * @param ifindex the ifindex
 * @return Returns the interface
 */
Exalt_Ethernet* exalt_eth_get_ethernet_byifindex(int ifindex);

/**
 * @brief test if an interface is link
 * @param eth the interface
 * @return Return 1 if yes, else 0
 */
short exalt_eth_link_is(Exalt_Ethernet *eth);
/**
 * @brief get the name of the interface "eth" (eth0, eth1 ...)
 * @param eth the interface
 * @return Returns the name
 */
const char* exalt_eth_name_get(Exalt_Ethernet* eth);
/**
 * @brief get the ip address of the interface "eth"
 * @param eth the interface
 * @return Returns the ip address (don't forget to free the IP address)
 */
char* exalt_eth_ip_get(Exalt_Ethernet* eth);
/**
 * @brief get the netmask address of the interface "eth"
 * @param eth the interface
 * @return Returns the netmask address (dont forget to free the address)
 */
char* exalt_eth_netmask_get(Exalt_Ethernet* eth);
/**
 * @brief get the default gateway address of the interface "eth"
 * @param eth the interface
 * @return Returns the gateway address (don't forget to free the address)
 */
char* exalt_eth_gateway_get(Exalt_Ethernet* eth);
/**
 * @brief remove the default gateway of the interface eth
 * @param eth the interface
 * @return Returns 1 if all defaults gateways of the interface are removed, else 0
 */
int exalt_eth_gateway_delete(Exalt_Ethernet* eth);
/**
 * @brief get the udi of the interface "eth" (eth0, eth1 ...)
 * @param eth the interface
 * @return Returns the udi
 */
const char* exalt_eth_udi_get(Exalt_Ethernet* eth);
/**
 * @brief get the ifindex of the interface "eth" (eth0, eth1 ...)
 * @param eth the interface
 * @return Returns the ifindex
 */
int exalt_eth_ifindex_get(Exalt_Ethernet* eth);

/**
 * @brief get if the interface "eth" use DHCP or static (look the configuration file)
 * @param eth the interface
 * @return Returns 1 if the interface use DHCP, 0 if static, -1 if unknow
 */
int exalt_eth_dhcp_is(Exalt_Ethernet * eth);

/**
 * @brief get if the interface "eth" is a wireless interface
 * @param eth the interface
 * @return Returns 1 if the interface is a wireless interface, else 0
 */
int exalt_eth_wireless_is(Exalt_Ethernet* eth);
/**
 * @brief get the wireless structure of the interface "eth"
 * @param eth the interface
 * @return Returns the wireless structure
 */
Exalt_Wireless* exalt_eth_wireless_get(Exalt_Ethernet* eth);

/**
 * @brief set the callback function
 * This callback will be called when we have a new interface, new ip address ..
 * see the type Exalt_Enum_Action for a list of notifications
 * @param fct function called
 * @param user_data user data
 * @return Returns 1 if success, else 0
 */
int exalt_eth_cb_set(Exalt_Eth_Cb fct, void* user_data;);
/**
 * @brief set the callback scan function
 * this callback is called when a scan is finish,
 * the scan must be started with the function exalt_wireless_scan_start()
 * @param fct function called
 * @param user_data user data
 * @return Returns 1 if success, else 0
 */
int exalt_eth_scan_cb_set(Exalt_Wifi_Scan_Cb fct, void* user_data);

/**
 * @brief apply the connection for the interface "eth"
 * @param eth the interface
 * @param c the connection
 * @return Returns 1 if the configuration is apply, else 0
 */
int exalt_eth_conn_apply(Exalt_Ethernet* eth, Exalt_Connection* c);
/**
 * @brief get the connection of the interface "eth"
 * @param eth the interface
 * @return Returns the connection
 */
Exalt_Connection* exalt_eth_connection_get(Exalt_Ethernet* eth);
/**
 * @brief set the connection of the interface "eth"
 * this function doesn't apply the configuration, only set the variable
 * @param eth the interface
 * @param c the connection
 * @return returns 1 if the connexion is set, else 0
 */
short exalt_eth_connection_set(Exalt_Ethernet* eth, Exalt_Connection* c);

/**
 * @brief up the interface "eth" but tell to the daemon "don't apply a connection",
 * sometimes we need upping an interface when we want apply a connection, this method avoid an infinite loop
 * @param eth the interface
 */
void exalt_eth_up_without_apply(Exalt_Ethernet* eth);
/**
 * @brief set the time in seconds when you up the interface
 * then the daemon will use this value to know if it will apply or not the connection when it will get the notification from the kernel
 * the daemon use a timeout of x secondes
 * if(current_time - value > x) apply a connection
 * @param eth the interface
 * @param the value in seconds
 */
void exalt_eth_dontapplyafterup_set(Exalt_Ethernet * eth, time_t t);
/**
 * @brief see exalt_eth_set_dontapplyafterup
 * @param eth the interface
 * @return Returns the value
 */
time_t exalt_eth_dontapplyafterup_get(Exalt_Ethernet* eth);

/** @} */

#endif


