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



int exalt_wireless_conn_save(const char* file, Exalt_Connection* c);
Exalt_Connection* exalt_wireless_conn_load(const char* file, const char *essid);

int exalt_conf_save_wpasupplicant(Exalt_Wireless *w);

int exalt_eth_save(const char* file, Exalt_Ethernet* eth);
Exalt_Enum_State exalt_eth_state_load(const char* file, const char* udi);
Exalt_Connection *exalt_eth_conn_load(const char* file, const char* udi);
char* exalt_eth_driver_load(const char* file, const char* udi);
#endif

/** @} */
