/**  @file exalt_wireless_network.h */
#ifndef EXALT_WIRELESS_NETWORK_H
#define EXALT_WIRELESS_NETWORK_H

#include <stdio.h>
/**
 * @defgroup Exalt_Wireless_Network
 * @brief The Exalt_Wireless_Network contains all informations about a wireless network (essid, protocol ...)
 * @{
 */

/**
 * @brief informations about a wireless network
 * @structinfo
 */
typedef struct Exalt_Wireless_Network Exalt_Wireless_Network;

#define Exalt_Wireless_Network(x) (Exalt_Wireless_Network*)x

#include "exalt_wireless.h"
#include "libexalt.h"
#include "exalt_connection.h"

Exalt_Wireless_Network* exalt_wirelessnetwork_create(Exalt_Wireless* w);
void exalt_wirelessnetwork_free(void* data);

Exalt_Wireless* exalt_wirelessnetwork_get_wireless(Exalt_Wireless_Network* w);
void exalt_wirelessnetwork_set_address(Exalt_Wireless_Network* w, const char* address);
void exalt_wirelessnetwork_set_essid(Exalt_Wireless_Network* w, const char* essid);
void exalt_wirelessnetwork_set_mode(Exalt_Wireless_Network* w, const char* mode);
void exalt_wirelessnetwork_set_protocol(Exalt_Wireless_Network* w, const char* protocol);
void exalt_wirelessnetwork_set_channel(Exalt_Wireless_Network* w, const char* channel);
void exalt_wirelessnetwork_set_encryption(Exalt_Wireless_Network* w,int encryption);
void exalt_wirelessnetwork_set_bitrates(Exalt_Wireless_Network* w, const char* bit_rates);
void exalt_wirelessnetwork_set_quality(Exalt_Wireless_Network* w, int quality);
void exalt_wirelessnetwork_set_signalvl(Exalt_Wireless_Network* w, int signal_lvl);
void exalt_wirelessnetwork_set_noiselvl(Exalt_Wireless_Network* w, int noise_lvl);
void exalt_wirelessnetwork_set_scanok(Exalt_Wireless_Network *w,short ok);
void exalt_wirelessnetwork_set_known(Exalt_Wireless_Network* w,short known);

const char* exalt_wirelessnetwork_get_addr(Exalt_Wireless_Network* w);
const char* exalt_wirelessnetwork_get_essid(Exalt_Wireless_Network* w);
const char* exalt_wirelessnetwork_get_protocol(Exalt_Wireless_Network* w);
const char* exalt_wirelessnetwork_get_mode(Exalt_Wireless_Network* w);
const char* exalt_wirelessnetwork_get_channel(Exalt_Wireless_Network* w);
int exalt_wirelessnetwork_get_encryption(Exalt_Wireless_Network* w);
const char* exalt_wirelessnetwork_get_bitrates(Exalt_Wireless_Network* w);
int exalt_wirelessnetwork_get_quality(Exalt_Wireless_Network* w);
int exalt_wirelessnetwork_get_signallvl(Exalt_Wireless_Network* w);
int exalt_wirelessnetwork_get_noiselvl(Exalt_Wireless_Network* w);

int exalt_wirelessnetwork_is_known(Exalt_Wireless_Network* wi);
int exalt_wirelessnetwork_is_scan(Exalt_Wireless_Network* wi);

/** @} */

#endif

