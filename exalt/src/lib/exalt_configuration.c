/** @file exalt_configuration.c */

#include "exalt_configuration.h"
#include "libexalt_private.h"



struct Exalt_Configuration
{
    Exalt_Enum_Mode mode;

    char* ip;
    char* netmask;
    char* gateway;

    int wireless;
    Exalt_Configuration_Network* network;

    char* cmd_after_apply; //a command call after exalt_conf_apply()
};

/**
 * @addtogroup Exalt_Configuration
 * @{
 */

Exalt_Configuration* exalt_conf_new()
{
    Exalt_Configuration* c;

    c = calloc(1,sizeof(Exalt_Configuration));
    EXALT_ASSERT_RETURN(c!=NULL);

    c->mode = EXALT_DHCP;

    c->cmd_after_apply = NULL;
    return c;
}

void exalt_conf_free(Exalt_Configuration** conf)
{
    Exalt_Configuration *c;
    EXALT_ASSERT_RETURN_VOID(conf!=NULL);

    c = *conf;
    EXALT_ASSERT_RETURN_VOID(c!=NULL);

    EXALT_FREE(c->ip);
    EXALT_FREE(c->gateway);
    EXALT_FREE(c->netmask);

    EXALT_FREE(c->cmd_after_apply);

    exalt_conf_network_free( &( c->network));
    EXALT_FREE(c);
}

short exalt_conf_valid_is(Exalt_Configuration* c)
{
    short valid = 1;
    EXALT_ASSERT_RETURN(c!=NULL);

    if(exalt_conf_mode_get(c) == EXALT_STATIC)
    {
        if(!exalt_address_is(exalt_conf_ip_get(c))
                || (exalt_conf_gateway_get(c)!=  NULL && !exalt_address_is(exalt_conf_gateway_get(c)))
                || !exalt_address_is(exalt_conf_netmask_get(c)))
            valid = 0;
    }

    /*if(valid && exalt_conf_wireless_is(c))
    {
        if(!exalt_is_essid(exalt_conf_essid_get(c))
                || !exalt_is_key(exalt_conf_key_get(c),exalt_conf_encryption_mode_get(c)))
            valid = 0;
    }
    */

    return valid;
}




/// @cond
#define EXALT_FCT_NAME exalt_conf
#define EXALT_STRUCT_TYPE Exalt_Configuration
/// @endcond

EXALT_SET(mode,Exalt_Enum_Mode)
EXALT_STRING_SET(ip)
EXALT_STRING_SET(netmask)
EXALT_STRING_SET(gateway)
EXALT_STRING_SET(cmd_after_apply)

EXALT_SET(wireless,int)
EXALT_SET(network,Exalt_Configuration_Network*)

EXALT_GET(mode,Exalt_Enum_Mode)
EXALT_GET(ip,const char*)
EXALT_GET(gateway,const char*)
EXALT_GET(netmask,const char*)
EXALT_GET(cmd_after_apply,const char*)

EXALT_IS(wireless,int)
EXALT_GET(network,Exalt_Configuration_Network*)

#undef EXALT_FCT_NAME
#undef EXALT_STRUCT_TYPE

Eet_Data_Descriptor * exalt_conf_edd_new(Eet_Data_Descriptor* edd_network)
{
    Eet_Data_Descriptor_Class eddc;
    Eet_Data_Descriptor *edd;

    EXALT_ASSERT_RETURN(edd_network!=NULL);

    eet_eina_stream_data_descriptor_class_set(&eddc, sizeof (eddc), "Configuration", sizeof(Exalt_Configuration));
    edd = eet_data_descriptor_stream_new(&eddc);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration, "dhcp", mode, EET_T_INT);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration, "ip", ip, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration, "netmask", netmask, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration, "gateway", gateway, EET_T_STRING);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration, "wireless", wireless, EET_T_SHORT);
    EET_DATA_DESCRIPTOR_ADD_SUB(edd, Exalt_Configuration, "network", network, edd_network);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Exalt_Configuration, "cmd_after_apply", cmd_after_apply, EET_T_STRING);
    return edd;
}

/** @} */
