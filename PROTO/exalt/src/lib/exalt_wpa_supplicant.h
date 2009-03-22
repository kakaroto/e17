/*
 * =====================================================================================
 *
 *       Filename:  exalt_wpa_supplicant.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  03/04/2009 08:30:40 PM CET
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  EXALT_WPA_SUPPLICANT_INC
#define  EXALT_WPA_SUPPLICANT_INC

#include "libexalt_private.h"

struct wpa_ctrl * exalt_wpa_open_connection(Exalt_Wireless *w);
int exalt_wpa_ctrl_command(struct wpa_ctrl *ctrl, char *cmd, char*buf, size_t buf_len);
void exalt_wpa_stop(Exalt_Wireless* w);

Eina_List* exalt_wpa_parse_scan_results(struct wpa_ctrl* ctrl,char *buf, Exalt_Wireless *w);
void exalt_wpa_parse_1_result(struct wpa_ctrl* ctrl,int i, Exalt_Wireless_Network *wn);
void exalt_wpa_parse_BBS_key(char* key, char* value, Exalt_Wireless_Network *wn);
void exalt_wpa_parse_flags(char* buf, Exalt_Wireless_Network *wn);
void exalt_wpa_parse_flag(char* buf, Exalt_Wireless_Network_IE *ie);

#endif   /* ----- #ifndef EXALT_WPA_SUPPLICANT_INC  ----- */

