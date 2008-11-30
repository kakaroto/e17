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


Exalt_Wireless* exalt_wireless_new(Exalt_Ethernet* eth);
void exalt_wireless_free(Exalt_Wireless* w);

int exalt_wireless_set_wpasupplicant_driver(Exalt_Wireless* w, const char* driver);
char* exalt_wireless_get_wpasupplicant_driver(Exalt_Wireless* w);

void exalt_wireless_scan_start(Exalt_Ethernet* eth);
int exalt_wireless_scan_wait(Exalt_Ethernet* eth);

Exalt_Ethernet* exalt_wireless_get_ethernet(Exalt_Wireless* w);
Eina_List* exalt_wireless_get_networks_list(Exalt_Wireless* w);

void exalt_set_button_state(Exalt_Wireless* w);
short exalt_wireless_radiobutton_ison(Exalt_Wireless* w);



Exalt_Wireless_Network* exalt_wireless_get_networkinfo(Exalt_Wireless* w, int nb);
Exalt_Wireless_Network* exalt_wireless_get_networkinfo_by_essid(Exalt_Wireless* w,const char *essid);


int exalt_wireless_apply_conn(Exalt_Wireless *w);

void exalt_wireless_printf(Exalt_Wireless *w);


char* exalt_wireless_get_essid(Exalt_Wireless* w);

/** @} */

#endif

