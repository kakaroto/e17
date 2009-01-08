/** @file exalt_sys_conf.h */
#ifndef EXALT_SYS_CONF_H
#define EXALT_SYS_CONF_H

/**
 * @defgroup Exalt_System_Configuration
 * @brief all functions to save or load a configuration
 * @{
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <Ecore_File.h>
#include "libexalt.h"
#include <Eet.h>
#include <Evas.h>

/** the directory of the file configuration of wpa_supplicant */
#define EXALT_WPA_CONF_FILE_DIR "/etc/wpa_supplicant"
/** the location of the wpa_supplicant configuration file */
#define EXALT_WPA_CONF_FILE  EXALT_WPA_CONF_FILE_DIR "/wpa_supplicant.conf"
/** the file with interfaces of the wpa_supplicant daemon */
#define EXALT_WPA_INTERFACE_DIR "/var/run/wpa_supplicant"
/** the header of the wpa_supplicant configuration file */
#define EXALT_WPA_CONF_HEADER "ctrl_interface=" EXALT_WPA_INTERFACE_DIR "\n" \
                        "ctrl_interface_group=0\n" \
                        "eapol_version=1\n" \
                        "ap_scan=1\n" \
                        "fast_reauth=1\n"


/**
 * @brief save the connection associated to an essid
 * @param file the file where save
 * @param c the connection
 * @return Return 1 if success, else 0
 */
int exalt_wireless_conn_save(const char* file, Exalt_Connection* c);
/**
 * @brief load the connection for an essid
 * @param file the configuration file
 * @param essid the essid
 * @return Returns the connection if success, else NULL
 */
Exalt_Connection* exalt_wireless_conn_load(const char* file, const char *essid);
/**
 * @brief save the wpa_supplicant configuration for a wireless interface
 * This configuration is saved in the wpa_supplicant configuration file
 * @param w the wireless card
 * @return Return 1 if success, else -0
 */
int exalt_conf_save_wpasupplicant(Exalt_Wireless *w);
/**
 * @brief save the configuration of a card
 * @param file the configuration file
 * @param eth the card
 * @return Return 1 if success, else 0
 */
int exalt_eth_save(const char* file, Exalt_Ethernet* eth);
/**
 * @brief Load the state (up/down) of an interface from the configuration file
 * @param file the configuration file
 * @param udi the hal udi of the interface
 * @return Returns the state
 */
Exalt_Enum_State exalt_eth_state_load(const char* file, const char* udi);
/**
 * @brief Load the configuration of an interface from the configuration file
 * @param file the configuration file
 * @param udi the hal udi of the interface
 * @return Returns the state
 */
Exalt_Connection *exalt_eth_conn_load(const char* file, const char* udi);

/**
 * @brief Load the driver of an <ireless interface from the configuration file
 * @param file the configuration file
 * @param udi the hal udi of the interface
 * @return Returns the state
 */
char* exalt_eth_driver_load(const char* file, const char* udi);
#endif

/** @} */
