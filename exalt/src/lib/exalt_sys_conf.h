/** @file exalt_sys_conf.h */
#ifndef EXALT_SYS_CONF_H
#define EXALT_SYS_CONF_H

/**
 * @defgroup Exalt_System_Configuration
 * @brief Methods to save or load a configuration from the configuration file
 * @ingroup Exalt
 * @{
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <Ecore_File.h>
#include "Exalt.h"
#include <Eet.h>

/** the directory of the file configuration of wpa_supplicant */
#define EXALT_WPA_CONF_FILE_DIR "/etc/wpa_supplicant"
/** the location of the wpa_supplicant configuration file */
#define EXALT_WPA_CONF_FILE  EXALT_WPA_CONF_FILE_DIR "/wpa_supplicant.conf"
/** the file with interfaces of the wpa_supplicant daemon */
#define EXALT_WPA_INTERFACE_DIR "/var/run/wpa_supplicant"


/**
 * @brief Save the configuration of an interface
 * @param file the configuration file
 * @param eth the interface
 * @return Return 1 if success, else 0
 */
EAPI int exalt_eth_save(const char* file, Exalt_Ethernet* eth);
/**
 * @brief Load the state (up/down) of an interface from the configuration file
 * @param file the configuration file
 * @param udi the hal udi of the interface
 * @return Returns the state
 */
EAPI int exalt_eth_state_load(const char* file, const char* udi);
/**
 * @brief Load the configuration of an interface from the configuration file
 * @param file the configuration file
 * @param udi the hal udi of the interface
 * @return Returns the configuration
 */
EAPI Exalt_Configuration *exalt_eth_conf_load(const char* file, const char* udi);

/**
 * @brief Load the driver of an wireless interface from the configuration file
 * @param file the configuration file
 * @param udi the hal udi of the interface
 * @return Returns the driver
 * @note If the result is NULL, you should use the driver "wext".
 */
EAPI char* exalt_eth_driver_load(const char* file, const char* udi);

/**
 * @brief Save a wireless network configuration
 * @param file the configuration file
 * @param cn the network configuration
 * @return Returns 0 if an error occurs, else 1
 */
EAPI int exalt_conf_network_save(const char* file, Exalt_Configuration *c);

/**
 * @brief Load a wireless network configuration
 * @param file the configuration file
 * @param network the network name (essid)
 * @return Returns the network configuration or null if failed
 */
EAPI Exalt_Configuration *exalt_conf_network_load(const char *file,const char *network);

/**
 * @brief Load the list of wireless network saved
 * @param file the configuration file
 * @return Returns a list of essid (char *)
 */
EAPI Eina_List *exalt_conf_network_list_load(const char *file);

/**
 * @brief Delete a wireless network configuration
 * @param file the configuration file
 * @param network the network's name (essid)
 */
EAPI void exalt_conf_network_delete(const char* file, const char *network);

#endif

/** @} */
