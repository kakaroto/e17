/**  @file exalt_wireless.h */
#ifndef WIRELESS_H
#define WIRELESS_H

/**
 * @defgroup Exalt_Wireless
 * @brief The Exalt_Wireless contains all informations about your wireless card as name, essid, list of scanning networks ...
 * @{
 */

/**
 * @brief informations about a wireless card
 * @structinfo
 */
typedef struct Exalt_Wireless Exalt_Wireless;


#include "libexalt.h"
#include "exalt_wireless_network.h"
#include <Ecore_Data.h>
#include <Ecore.h>
#include <Eina.h>
#include <stdio.h>

/**
 * @brief create a new Exalt_Wireless
 * @return Returns the new Exalt_Wireless structure
 */
Exalt_Wireless* exalt_wireless_new(Exalt_Ethernet* eth);
/**
 * @brief free a Exalt_Wireless
 * @param w the Exalt_Wireless
 */
void exalt_wireless_free(Exalt_Wireless** w);
/**
 * @brief set the driver used by wpa_supplicant
 * @param w the Exalt_Wireless
 * @param driver the driver (wext ...)
 * @return Returns 1 if the driver is set, else 0
 */
void exalt_wireless_wpasupplicant_driver_set(Exalt_Wireless* w, const char* driver);
/**
 * @brief return the driver used by wpa_supplicant
 * @param w the Exalt_Wireless
 * @return Returns the driver
 */
const char* exalt_wireless_wpasupplicant_driver_get(Exalt_Wireless* w);

/**
 * @brief return the current essid
 * @param w the Exalt_Wireless
 * @return Returns the current essid (don't forget to free it), NULL if no essid is associated
 */
char* exalt_wireless_essid_get(Exalt_Wireless* w);



/*
 * @brief get the ethernet structure
 * @param w the Exalt_Wireless
 * @return Returns the exalt_ethernet structure associated to w
 */
Exalt_Ethernet* exalt_wireless_eth_get(Exalt_Wireless* w);
/**
 * @brief return the wireless networks list result of a scan
 * @param w the wireless interface
 * @return Returns a list of Exalt_Wireless_Network
 */
Eina_List* exalt_wireless_networks_get(Exalt_Wireless* w);

void exalt_button_state_set(Exalt_Wireless* w);
/**
 * @brief get the radio button state
 * @param w the interface
 * @return Return -1 if a error is occurs, 0 if the button is off, 1 if it is on
 */
int exalt_wireless_radiobutton_on_is(Exalt_Wireless* w);


/**
 * @brief get information about a wireless network (essid, quality ...)
 * @param w the Exalt_Wireless
 * @param nb the position of the wireless network in the scan list
 * @return Return information about the network
 */
Exalt_Wireless_Network* exalt_wireless_network_get(Exalt_Wireless* w, int nb);
/**
 * @brief get information about the wireless network named essid
 * @param w the Exalt_Wireless
 * @param essid the essid
 * @return Returns information about this network
 */
Exalt_Wireless_Network* exalt_wireless_network_get_byessid(Exalt_Wireless* w,const char *essid);

/**
 * @brief apply the connection (the current essid, current password ...)
 * @param w the interface
 * @return Return 1 if the configuration is apply, else 0
 */
int exalt_wireless_conn_apply(Exalt_Wireless *w);
/**
 * @brief Print all informations about the interface in stdout
 * @param w the Exalt_Wireless
 */
void exalt_wireless_printf(Exalt_Wireless *w);

/**
 * @brief start a scan
 * When this scan will be done, the callback function set by exalt_eth_set_scan_cb() will be called
 * @param eth the Exalt_Ethernet where scan
 */
void exalt_wireless_scan_start(Exalt_Ethernet* eth);


/** @} */

#endif

