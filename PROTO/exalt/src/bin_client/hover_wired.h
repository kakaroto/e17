/*
 * =====================================================================================
 *
 *       Filename:  hover_wired.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  12/02/09 18:22:17 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  HOVER_WIRED_INC
#define  HOVER_WIRED_INC

#include "main.h"

void hover_wired_create();
void hover_wired_iface_set(char* iface);
void hover_wired_response(Exalt_DBus_Response* response);
void hover_wired_icon_update(char* iface, Exalt_DBus_Response* response);

#endif   /* ----- #ifndef HOVER_WIRED_INC  ----- */

