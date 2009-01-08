/*
 * =====================================================================================
 *
 *       Filename:  exalt_vpnc.h
 *
 *    Description:  manage a vpn connection using vpnc
 *
 *        Version:  1.0
 *        Created:  11/21/2007 02:34:23 PM CET
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  EXALT_VPNC_INC
#define  EXALT_VPNC_INC

#include <stdlib.h>
#include <string.h>

/* OLD CODE never used, need to be update before complete */

/**
 * @brief informations about a vpnc connection
 * @structinfo
 */
typedef struct Exalt_Vpnc Exalt_Vpnc;


Exalt_Vpnc* exalt_vpnc_new();
char* exalt_vpnc_get_name(Exalt_Vpnc* vpnc);
char* exalt_vpnc_get_ipsec_gateway(Exalt_Vpnc* vpnc);
char* exalt_vpnc_get_ipsec_id(Exalt_Vpnc* vpnc);
char* exalt_vpnc_get_ipsec_secret(Exalt_Vpnc* vpnc);
char* exalt_vpnc_get_xauth_username(Exalt_Vpnc* vpnc);
char* exalt_vpnc_get_xauth_password(Exalt_Vpnc* vpnc);
int exalt_vpnc_set_name(Exalt_Vpnc* vpnc, char* name);
int exalt_vpnc_set_ipsec_gateway(Exalt_Vpnc* vpnc, char* ipsec_gateway);
int exalt_vpnc_set_ipsec_id(Exalt_Vpnc* vpnc, char* ipsec_id);
int exalt_vpnc_set_ipsec_secret(Exalt_Vpnc* vpnc, char* ipsec_secret);
int exalt_vpnc_set_xauth_username(Exalt_Vpnc* vpnc, char* xauth_username);
int exalt_vpnc_set_xauth_password(Exalt_Vpnc* vpnc, char* xauth_password);



#endif   /* ----- #ifndef EXALT_VPNC_INC  ----- */

