/** @file exalt_ethernet.h */
#ifndef EXALT_ETHERNET_H
#define EXALT_ETHERNET_H

//The type of the main variable used by Exalt. It contains the list of interfaces, callbacks functions ...
typedef struct Exalt_Ethernets Exalt_Ethernets;
extern Exalt_Ethernets exalt_eth_interfaces;

/**
 * @defgroup Exalt_Ethernet
 * @brief The Exalt_Ethernet structure contains represents an ethernet interface. From this structure we can activate/deactivate an interface, get his IP address, test if the interface is linked or not ... An exalt_ethernet interface() can have the extension Exalt_Wireless() to defines a wireless interface.
 * @ingroup Exalt
 * @{
 */

/**
 * @brief Information about an interface.
 */
typedef struct Exalt_Ethernet Exalt_Ethernet;


#include "libexalt.h"
#include "exalt_wireless.h"
#include "exalt_wireless_network.h"
#include "exalt_configuration.h"
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


/** The list of actions notify when a change occurs. */
typedef enum Exalt_Enum_Action
{
    /** For each interface when we init exalt. These interfaces are new for exalt but not new for the system */
    EXALT_IFACE_ACTION_NEW ,
    /** When a new interface appears */
    EXALT_IFACE_ACTION_ADD,
    /** When an interface disappear */
    EXALT_IFACE_ACTION_REMOVE,
    /** When an interface is up/activate */
    EXALT_IFACE_ACTION_UP ,
    /** When an interface is down/deactivate */
    EXALT_IFACE_ACTION_DOWN,
    /** When an interface is linked */
    EXALT_IFACE_ACTION_LINK ,
    /** When an interface is unlinked */
    EXALT_IFACE_ACTION_UNLINK,
    /** When an essid change. Available only if the interface is wireless */
    EXALT_WIRELESS_ACTION_ESSIDCHANGE ,

    /** When a wired interface is connected */
    EXALT_IFACE_ACTION_CONNECTED,
    /** When a wired interface is disconnected */
    EXALT_IFACE_ACTION_DISCONNECTED,

    /** When all interfaces are disconnected */
    EXALT_IFACE_ACTION_ALL_IFACES_DISCONNECTED,

    /** When a wireless interface is connected */
    EXALT_WIRELESS_ACTION_CONNECTED,
    /** When a wireless interface is disconnected */
    EXALT_WIRELESS_ACTION_DISCONNECTED,

    /** When we have a new IP address */
    EXALT_IFACE_ACTION_ADDRESS_NEW ,
    /** >hen we have a new netmask address */
    EXALT_IFACE_ACTION_NETMASK_NEW ,
    /** When we have a new default gateway address */
    EXALT_IFACE_ACTION_GATEWAY_NEW,


    /** When we start applying a configuration */
    EXALT_IFACE_ACTION_CONF_APPLY_START,
    /** When the configuration is applied */
    EXALT_IFACE_ACTION_CONF_APPLY_DONE,

    /** When we configure if we want wait (or not) that the interface is init during the boot process (only used by the daemon exaltd) */
    EXALTD_IFACE_WAITINGBOOT_CHANGE,
    /** When the value of the timeout change (only used by the daemon exaltd) */
    EXALTD_IFACE_WAITINGBOOT_TIMEOUT_CHANGE
} Exalt_Enum_Action;

/** Callback function used for notification when a new interface is add, new essid ... */
typedef void (*Exalt_Eth_Cb) (Exalt_Ethernet* eth, Exalt_Enum_Action action, void* user_data);


/**
 * @brief Create an Exalt_Ethernet structure
 * @param name the name of the interface (eth0, ath3 ...)
 * @param device the device name retrieve from HAL
 * @return Returns a new Exalt_Ethernet structure or NULL.
 * @note Some wireless drivers create 2 interfaces for one device : a wireless and a wired. In this case we only keep the wireless interface.
 */
EAPI Exalt_Ethernet* exalt_eth_new(const char* name, const char* device);

/**
 * @brief Free exalt_eth_interfaces, the main variable which contains the list of interfaces, callbacks functions ...
 */
EAPI void exalt_eth_ethernets_free();

/**
 * @brief Free an Exalt_Ethernet structure
 * @param data an Exalt_Ethernet* structure
 */
EAPI void exalt_eth_free(void* data);

/**
 * @brief Test if an interface is an ethernet interface
 * @param name the name of the interface
 * @return Returns 1 if the interface is an ethernet interface, else 0
 */
EAPI int exalt_eth_ethernet_is(char* name);

/**
 * @brief Print the description of each interface in the standard output
 */
EAPI void exalt_eth_printf();

/**
 * @brief Get the list of all interfaces
 * @return Returns a list of Exalt_Ethernet* structure
 */
EAPI Eina_List* exalt_eth_list_get();
/**
 * @brief Get an interface by his name
 * @param name the name (eth0 ...)
 * @return Returns the interface
 */
EAPI Exalt_Ethernet* exalt_eth_get_ethernet_byname(const char* name);
/**
 * @brief Get an interface by his udi
 * @param udi the udi
 * @return Returns the interface
 * @note The UDI is an identification number created by hal for each device.
 */
EAPI Exalt_Ethernet* exalt_eth_get_ethernet_byudi(const char* udi);
/**
 * @brief Get an interface by his ifindex
 * @param ifindex the ifindex
 * @return Returns the interface
 * @note the ifindex is an identification number used by the kernel.
 */
EAPI Exalt_Ethernet* exalt_eth_get_ethernet_byifindex(int ifindex);

/**
 * @brief Get the name of the interface "eth"
 * @param eth the interface
 * @return Returns the name
 */
EAPI const char* exalt_eth_name_get(Exalt_Ethernet* eth);
/**
 * @brief Get the device name of the interface "eth"
 * @param eth the interface
 * @return Returns the device name
 */
EAPI const char* exalt_eth_device_get(Exalt_Ethernet* eth);
/**
 * @brief Get the udi of the interface "eth"
 * @param eth the interface
 * @return Returns the udi
 * @note The UDI is an identification number created by hal for each device.
 */
EAPI const char* exalt_eth_udi_get(Exalt_Ethernet* eth);
/**
 * @brief Get the ifindex of the interface "eth"
 * @param eth the interface
 * @return Returns the ifindex
 * @note the ifindex is an identification number used by the kernel.
 */
EAPI int exalt_eth_ifindex_get(Exalt_Ethernet* eth);


/**
 * @brief Test if the interface "eth" is a wireless interface
 * @param eth the interface
 * @return Returns 1 if the interface is a wireless interface, else 0
 */
EAPI int exalt_eth_wireless_is(Exalt_Ethernet* eth);
/**
 * @brief Get the wireless structure of the interface "eth"
 * @param eth the interface
 * @return Returns the wireless structure
 */
EAPI Exalt_Wireless* exalt_eth_wireless_get(Exalt_Ethernet* eth);



/**
 * @brief Up/activate an interface
 * @param eth the interface
 */
EAPI void exalt_eth_up(Exalt_Ethernet* eth);
/**
 * @brief Down/deactivate an interface
 * @param eth the interface
 */
EAPI void exalt_eth_down(Exalt_Ethernet* eth);
/**
 * @brief Test if an interface is up/activated
 * @param eth the interface
 * @return Returns 1 if the interface is activated, else 0
 */
EAPI int exalt_eth_up_is(Exalt_Ethernet * eth);



/**
 * @brief Test if the interface is connected
 * @param eth the interface
 * @return Returns 1 if the interface is connected, else 0
 * @note A wired interface is connected if :
 *  - the interface is up
 *  - the interface is linked
 *  - the interface has an IP address
 * @note A wireless interface is connected if :
 *  - wpa_supplicant tell than the interface is connected
 */
EAPI short exalt_eth_connected_is(Exalt_Ethernet * eth);

/**
 * @brief Set if the interface is connected
 * @param eth the interface
 * @param connected 1 if connected, else 0
 * @note This state is automatically updated by the library but you can force the value.
 */
EAPI void exalt_eth_connected_set(Exalt_Ethernet *eth, short connected);




/**
 * @brief Test if an interface is linked
 * @param eth the interface
 * @return Returns 1 if the interface is linked, else 0
 */
EAPI short exalt_eth_link_is(Exalt_Ethernet *eth);
/**
 * @brief Get the IP address of the interface "eth"
 * @param eth the interface
 * @return Returns the IP address
 * @note Do not forget to free the IP address
 */
EAPI char* exalt_eth_ip_get(Exalt_Ethernet* eth);
/**
 * @brief Get the netmask address of the interface "eth"
 * @param eth the interface
 * @return Returns the netmask address
 * @note Do not forget to free the address
 */
EAPI char* exalt_eth_netmask_get(Exalt_Ethernet* eth);
/**
 * @brief Get the default gateway address of the interface "eth"
 * @param eth the interface
 * @return Returns the gateway address
 * @note Do no forget to free the address
 */
EAPI char* exalt_eth_gateway_get(Exalt_Ethernet* eth);
/**
 * @brief Remove the default gateway of the interface eth
 * @param eth the interface
 * @return Returns 1 if all defaults gateways of the interface are removed, else 0
 */
EAPI int exalt_eth_gateway_delete(Exalt_Ethernet* eth);
/**
 * @brief Test if the interface "eth" use the DHCP or static mode
 * @param eth the interface
 * @return Returns 1 if the interface use the DHCP mode, 0 if static, -1 if unknown
 * @note The method look in the configuration file.
 */
EAPI int exalt_eth_dhcp_is(Exalt_Ethernet * eth);


/**
 * @brief Set the callback function <br>
 * This callback will be called when we have a new interface, new IP address ... <br>
 * See the type Exalt_Enum_Action for a list of notifications
 * @param fct function called
 * @param user_data user data
 * @return Returns 1 if success, else 0
 */
EAPI int exalt_eth_cb_set(Exalt_Eth_Cb fct, void* user_data);

/**
 * @brief Apply the configuration to the interface "eth"
 * @param eth the interface
 * @param c the configuration
 * @return Returns 1 if the configuration is applied, else 0
 */
EAPI int exalt_eth_conf_apply(Exalt_Ethernet* eth, Exalt_Configuration* c);
/**
 * @brief Get the current configuration of the interface "eth"
 * @param eth the interface
 * @return Returns the configuration
 */
EAPI Exalt_Configuration* exalt_eth_configuration_get(Exalt_Ethernet* eth);
/**
 * @brief Set the configuration of the interface "eth" <br>
 * This function doesn't apply the configuration, only set the variable
 * @param eth the interface
 * @param c the configuration
 * @return returns 1 if the connexion is set, else 0
 */
EAPI short exalt_eth_configuration_set(Exalt_Ethernet* eth, Exalt_Configuration* c);


/** @} */

#endif


