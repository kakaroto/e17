#ifndef  EXALT_VPNC_INC
#define  EXALT_VPNC_INC

#include "Exalt.h"
#include <stdlib.h>
#include <string.h>

/* OLD CODE never used, need to be update before complete */

/**
 * @brief informations about a vpnc connection
 */
typedef struct Exalt_Vpnc Exalt_Vpnc;


EAPI Exalt_Vpnc* exalt_vpnc_new();
EAPI char* exalt_vpnc_get_name(Exalt_Vpnc* vpnc);
EAPI char* exalt_vpnc_get_ipsec_gateway(Exalt_Vpnc* vpnc);
EAPI char* exalt_vpnc_get_ipsec_id(Exalt_Vpnc* vpnc);
EAPI char* exalt_vpnc_get_ipsec_secret(Exalt_Vpnc* vpnc);
EAPI char* exalt_vpnc_get_xauth_username(Exalt_Vpnc* vpnc);
EAPI char* exalt_vpnc_get_xauth_password(Exalt_Vpnc* vpnc);
EAPI int exalt_vpnc_set_name(Exalt_Vpnc* vpnc, char* name);
EAPI int exalt_vpnc_set_ipsec_gateway(Exalt_Vpnc* vpnc, char* ipsec_gateway);
EAPI int exalt_vpnc_set_ipsec_id(Exalt_Vpnc* vpnc, char* ipsec_id);
EAPI int exalt_vpnc_set_ipsec_secret(Exalt_Vpnc* vpnc, char* ipsec_secret);
EAPI int exalt_vpnc_set_xauth_username(Exalt_Vpnc* vpnc, char* xauth_username);
EAPI int exalt_vpnc_set_xauth_password(Exalt_Vpnc* vpnc, char* xauth_password);



#endif   /* ----- #ifndef EXALT_VPNC_INC  ----- */

