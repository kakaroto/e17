/*
 * =====================================================================================
 *
 *       Filename:  exalt_vpnc.c
 *
 *    Description:  Manage a vpn conenction using vpnc
 *
 *        Version:  1.0
 *        Created:  11/21/2007 02:35:15 PM CET
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "exalt_vpnc.h"
#include "libexalt_private.h"


struct Exalt_Vpnc
{
    char* name;
    char* ipsec_gateway;
    char* ipsec_id;
    char* ipsec_secret;
    char* xauth_username;
    char* xauth_password;
};


Exalt_Vpnc* exalt_vpnc_new()
{
#ifdef HAVE_VPNC
    Exalt_Vpnc* vpnc;
    vpnc = malloc(sizeof(Exalt_Vpnc));

    EXALT_ASSERT_RETURN(vpnc!=NULL);

    vpnc->name=NULL;
    vpnc->ipsec_gateway = NULL;
    vpnc->ipsec_id = NULL;
    vpnc->ipsec_secret = NULL;
    vpnc->xauth_username = NULL;
    vpnc->xauth_password=NULL;

    return vpnc;
#else
  EXALT_ASSERT_ADV(0,return NULL,"You can not use this function if you don't ahve the support of vpnc");
#endif
}

char* exalt_vpnc_get_name(Exalt_Vpnc* vpnc)
{
#ifdef HAVE_VPNC
    EXALT_ASSERT_RETURN(vpnc!=NULL);

    return vpnc->name;
#else
    EXALT_ASSERT_ADV(0,return NULL,"You can not use this function if you don't ahve the support of vpnc");
#endif
}

char* exalt_vpnc_get_ipsec_gateway(Exalt_Vpnc* vpnc)
{
#ifdef HAVE_VPNC
 EXALT_ASSERT_RETURN(vpnc!=NULL);

    return vpnc->ipsec_gateway;
#else
  EXALT_ASSERT_ADV(0,return NULL,"You can not use this function if you don't ahve the support of vpnc");
#endif
}

char* exalt_vpnc_get_ipsec_id(Exalt_Vpnc* vpnc)
{
#ifdef HAVE_VPNC
 EXALT_ASSERT_RETURN(vpnc!=NULL);
    return vpnc->ipsec_id;
#else
  EXALT_ASSERT_ADV(0,return NULL,"You can not use this function if you don't ahve the support of vpnc");
#endif
}

char* exalt_vpnc_get_ipsec_secret(Exalt_Vpnc* vpnc)
{
#ifdef HAVE_VPNC
    EXALT_ASSERT_RETURN(vpnc!=NULL);
    return vpnc->ipsec_secret;
#else
  EXALT_ASSERT_ADV(0,return NULL,"You can not use this function if you don't ahve the support of vpnc");
#endif
}

char* exalt_vpnc_get_xauth_username(Exalt_Vpnc* vpnc)
{
#ifdef HAVE_VPNC
    EXALT_ASSERT_RETURN(vpnc!=NULL);

    return vpnc->xauth_username;
#else
 EXALT_ASSERT_ADV(0,return NULL,"You can not use this function if you don't ahve the support of vpnc");
#endif
}

char* exalt_vpnc_get_xauth_password(Exalt_Vpnc* vpnc)
{
#ifdef HAVE_VPNC
 EXALT_ASSERT_RETURN(vpnc!=NULL);

    return vpnc->xauth_password;
#else
 EXALT_ASSERT_ADV(0,return NULL,"You can not use this function if you don't ahve the support of vpnc");
#endif
}

int exalt_vpnc_set_name(Exalt_Vpnc* vpnc, char* name)
{
#ifdef HAVE_VPNC
 EXALT_ASSERT_RETURN(vpnc!=NULL);
    EXALT_FREE(vpnc->name);
    vpnc->name = strdup(name);
    return 1;
#else
  EXALT_ASSERT_ADV(0,return 0,"You can not use this function if you don't ahve the support of vpnc");
#endif
}

int exalt_vpnc_set_ipsec_gateway(Exalt_Vpnc* vpnc, char* ipsec_gateway)
{
#ifdef HAVE_VPNC
    EXALT_ASSERT_RETURN(vpnc!=NULL);
    EXALT_FREE(vpnc->ipsec_gateway);
    vpnc->ipsec_gateway = strdup(ipsec_gateway);
    return 1;
#else
  EXALT_ASSERT_ADV(0,return 0,"You can not use this function if you don't ahve the support of vpnc");
#endif
}


int exalt_vpnc_set_ipsec_id(Exalt_Vpnc* vpnc, char* ipsec_id)
{
#ifdef HAVE_VPNC
    EXALT_ASSERT_RETURN(vpnc!=NULL);
    EXALT_FREE(vpnc->ipsec_id);
    vpnc->ipsec_id = strdup(ipsec_id);
    return 1;
#else
  EXALT_ASSERT_ADV(0,return 0,"You can not use this function if you don't ahve the support of vpnc");
#endif
}

int exalt_vpnc_set_ipsec_secret(Exalt_Vpnc* vpnc, char* ipsec_secret)
{
#ifdef HAVE_VPNC
    EXALT_ASSERT_RETURN(vpnc!=NULL);
    EXALT_FREE(vpnc->ipsec_secret);
    vpnc->ipsec_secret = strdup(ipsec_secret);
    return 1;
#else
  EXALT_ASSERT_ADV(0,return 0,"You can not use this function if you don't ahve the support of vpnc");
#endif
}

int exalt_vpnc_set_xauth_username(Exalt_Vpnc* vpnc, char* xauth_username)
{
#ifdef HAVE_VPNC
    EXALT_ASSERT_RETURN(vpnc!=NULL);
    EXALT_FREE(vpnc->xauth_username);
    vpnc->xauth_username = strdup(xauth_username);
    return 1;
#else
  EXALT_ASSERT_ADV(0,return 0,"You can not use this function if you don't ahve the support of vpnc");
#endif
}

int exalt_vpnc_set_xauth_password(Exalt_Vpnc* vpnc, char* xauth_password)
{
#ifdef HAVE_VPNC
    EXALT_ASSERT_RETURN(vpnc!=NULL);
    EXALT_FREE(vpnc->xauth_password);
    vpnc->xauth_password = strdup(xauth_password);
    return 1;
#else
  EXALT_ASSERT_ADV(0,return 0,"You can not use this function if you don't ahve the support of vpnc");
#endif
}


