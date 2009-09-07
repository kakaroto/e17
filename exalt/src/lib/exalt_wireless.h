/**  @file exalt_wireless.h */
#ifndef WIRELESS_H
#define WIRELESS_H

/**
 * @defgroup Exalt_Wireless
 * @brief The Exalt_Wireless contains all informations about a wireless interface as essid, list of scanning networks ...
 * @ingroup Exalt
 * @{
 */

/**
 * @brief Information about a wireless interface. This structure is an extension of Exalt_Ethernet()
 */
typedef struct Exalt_Wireless Exalt_Wireless;



#include "Exalt.h"
#include "exalt_wireless_network.h"
#include <Ecore_Data.h>
#include <Ecore.h>
#include <Eina.h>
#include <stdio.h>
#include "exalt_configuration_network.h"

/** Callback function used to return the result of a wireless scan */
typedef void (*Exalt_Wireless_Scan_Cb) (Exalt_Ethernet* eth, Eina_List* networks, void* user_data);



/**
 * @brief create a new Exalt_Wireless
 * @return Returns the new Exalt_Wireless structure
 */
EAPI Exalt_Wireless* exalt_wireless_new(Exalt_Ethernet* eth);
/**
 * @brief Free a Exalt_Wireless
 * @param w the Exalt_Wireless
 */
EAPI void exalt_wireless_free(Exalt_Wireless** w);
/**
 * @brief Set the driver used by wpa_supplicant
 * @param w the Exalt_Wireless
 * @param driver the driver (wext ...)
 * @return Returns 1 if the driver is set, else 0
 */
EAPI void exalt_wireless_wpasupplicant_driver_set(Exalt_Wireless* w, const char* driver);
/**
 * @brief Returns the driver used by wpa_supplicant
 * @param w the Exalt_Wireless
 * @return Returns the driver
 */
EAPI const char* exalt_wireless_wpasupplicant_driver_get(Exalt_Wireless* w);

/**
 * @brief Returns the current essid
 * @param w the Exalt_Wireless
 * @return Returns the current essid, NULL if no essid is associated
 * @note Do no forget to free it
 */
EAPI char* exalt_wireless_essid_get(Exalt_Wireless* w);

/*
 * @brief Tell to the wpa_supplicant daemon to disconnect the interface
 * @param w the wireless interface
 */
EAPI void exalt_wireless_disconnect(Exalt_Wireless *w);

/*
 * @brief Get the ethernet structure
 * @param w the Exalt_Wireless
 * @return Returns the exalt_ethernet structure associated to w
 */
EAPI Exalt_Ethernet* exalt_wireless_eth_get(Exalt_Wireless* w);

/**
 * @brief Get the radio button state
 * @param w the interface
 * @return Returns -1 if a error occurs, 0 if the button is off, 1 if it is on
 */
EAPI int exalt_wireless_radiobutton_on_is(Exalt_Wireless* w);


/**
 * @brief Print all wireless information about the interface in stdout
 * @param w the Exalt_Wireless
 */
EAPI void exalt_wireless_printf(Exalt_Wireless *w);

/**
 * @brief Start a scan. <br>
 * When this scan will be done, the callback function set by exalt_wireless_set_scan_cb() will be called
 * @param eth the interface
 */
EAPI void exalt_wireless_scan_start(Exalt_Ethernet* eth);


/**
 * @brief Set the callback scan function <br>
 * This callback is called when a wireless scan is finish,
 * the scan must be started with the function exalt_wireless_scan_start()
 * @param fct function called
 * @param user_data user data
 * @return Returns 1 if success, else 0
 */
EAPI int exalt_wireless_scan_cb_set(Exalt_Wireless_Scan_Cb fct, void* user_data);

/** @} */

#endif

