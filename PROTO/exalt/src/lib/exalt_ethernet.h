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

Exalt_Ethernet* exalt_eth_new(const char* name);

void exalt_eth_ethernets_free();
void exalt_eth_free(void* data);

void exalt_eth_up(Exalt_Ethernet* eth);
void exalt_eth_down(Exalt_Ethernet* eth);
short exalt_eth_is_up(Exalt_Ethernet * eth);

short exalt_eth_is_ethernet(char* name);


void exalt_eth_printf();

Ecore_List* exalt_eth_get_list();
Exalt_Ethernet* exalt_eth_get_ethernet_byname(const char* name);
Exalt_Ethernet* exalt_eth_get_ethernet_bypos(int pos);
Exalt_Ethernet* exalt_eth_get_ethernet_byudi(const char* udi);
Exalt_Ethernet* exalt_eth_get_ethernet_byifindex(int ifindex);


short exalt_eth_is_link(Exalt_Ethernet *eth);
const char* exalt_eth_get_name(const Exalt_Ethernet* eth);
char* exalt_eth_get_ip(const Exalt_Ethernet* eth);
char* exalt_eth_get_netmask(Exalt_Ethernet* eth);
char* exalt_eth_get_gateway(Exalt_Ethernet* eth);
int exalt_eth_delete_gateway(Exalt_Ethernet* eth);

const char* exalt_eth_get_udi(Exalt_Ethernet* eth);
int exalt_eth_get_ifindex(Exalt_Ethernet* eth);

short exalt_eth_is_dhcp(Exalt_Ethernet * eth);

short exalt_eth_is_wireless(Exalt_Ethernet* eth);
Exalt_Wireless* exalt_eth_get_wireless(Exalt_Ethernet* eth);

int exalt_eth_set_cb(Exalt_Eth_Cb fct, void* user_data;);
int exalt_eth_set_scan_cb(Exalt_Wifi_Scan_Cb fct, void* user_data);

int exalt_eth_apply_conn(Exalt_Ethernet* eth, Exalt_Connection* c);
Exalt_Connection* exalt_eth_get_connection(Exalt_Ethernet* eth);
short exalt_eth_set_connection(Exalt_Ethernet* eth, Exalt_Connection* c);


void exalt_eth_up_without_apply(Exalt_Ethernet* eth);
void exalt_eth_set_dontapplyafterup(Exalt_Ethernet * eth, time_t t);
time_t exalt_eth_get_dontapplyafterup(Exalt_Ethernet* eth);

/** @} */

#endif


