#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/** @file exalt_wireless.c */
#include "exalt_wireless.h"
#include "libexalt_private.h"
#include "../config.h"

/*
 * Private functions headers
 */

char* _exalt_wireless_save_essid_get(Exalt_Wireless* w);
int _exalt_wireless_save_essid_set(Exalt_Wireless* w,const char* essid);

int _exalt_wireless_scan(Exalt_Wireless *w);

void _exalt_wireless_wpa_connect(Exalt_Wireless *w);
Eina_Bool _exalt_wireless_wpa_cb(void *data, Ecore_Fd_Handler *fd_handler);

/**
 * @addtogroup Exalt_Wireless
 * @{
 */


struct Exalt_Wireless
{
    Exalt_Ethernet* eth;
    char* wpasupplicant_driver;

    Eina_List* networks;

    char* _save_essid;

    //use for scanning
    int scan_fd;
    int scanning;

    Ecore_Fd_Handler* fd_handler;

    struct wpa_ctrl* monitor;
};

Exalt_Wireless* exalt_wireless_new(Exalt_Ethernet* eth)
{
    Exalt_Wireless *w;
    char* str;

    EXALT_ASSERT_RETURN(eth!=NULL);

    w = calloc(1,sizeof(Exalt_Wireless));
    EXALT_ASSERT_RETURN(w!=NULL);

    w -> eth = eth;

    str = exalt_wireless_essid_get(w);
    _exalt_wireless_save_essid_set(w,str);
    EXALT_FREE(str);

    exalt_wireless_wpasupplicant_driver_set(w,"wext");

    _exalt_wireless_wpa_connect(w);

    return w;
}

void exalt_wireless_free(Exalt_Wireless** w)
{
    EXALT_ASSERT_RETURN_VOID(w!=NULL);
    Exalt_Wireless* w1 = *w;
    EXALT_ASSERT_RETURN_VOID(w1!=NULL);

    eina_list_free(w1->networks);

    EXALT_FREE(w1->_save_essid);
    EXALT_FREE(w1->wpasupplicant_driver);
    EXALT_CLOSE(w1->scan_fd);
    EXALT_FREE(w1);
}


/// @cond
#define EXALT_FCT_NAME exalt_wireless
#define EXALT_STRUCT_TYPE Exalt_Wireless
///@endcond

EXALT_STRING_SET(wpasupplicant_driver)
EXALT_GET(wpasupplicant_driver,const char*)

EXALT_GET(eth,Exalt_Ethernet*)
EXALT_GET(networks,Eina_List*)

#undef EXALT_FCT_NAME
#undef EXALT_STRUCT_TYPE

void exalt_wireless_down(Exalt_Wireless *w)
{
    EXALT_ASSERT_RETURN_VOID(w!=NULL);
    exalt_wireless_disconnect(w);
}

void exalt_wireless_disconnect(Exalt_Wireless *w)
{
    EXALT_ASSERT_RETURN_VOID(w!=NULL);
    exalt_wpa_disconnect(w);
}

char* exalt_wireless_essid_get(Exalt_Wireless* w)
{
    char essid[IW_ESSID_MAX_SIZE +1];
    struct iwreq wrq;
    Exalt_Ethernet *eth;

    memcpy(essid,"\0",strlen("\0"));
    EXALT_ASSERT_RETURN(w!=NULL);

    eth = exalt_wireless_eth_get(w);

    strncpy(wrq.ifr_name, exalt_eth_name_get(eth), sizeof(wrq.ifr_name));
    wrq.u.essid.pointer = (caddr_t) essid;
    wrq.u.essid.length = IW_ESSID_MAX_SIZE+1;
    wrq.u.essid.flags = 0;
    if(!exalt_ioctl(&wrq, SIOCGIWESSID))
        return NULL;

    if(wrq.u.essid.length>0)
    {
        return strdup((char*) wrq.u.essid.pointer);
    }
    else
        return NULL;
}


Exalt_Wireless_Network* exalt_wireless_network_get(Exalt_Wireless* w, int nb)
{
    EXALT_ASSERT_RETURN(w!=NULL);
    return (Exalt_Wireless_Network*)eina_list_nth(w->networks,nb);
}


Exalt_Wireless_Network* exalt_wireless_get_network_by_essid(Exalt_Wireless* w,const char *essid)
{
    Eina_List *l;
    Exalt_Wireless_Network* wi=NULL;

    EXALT_ASSERT_RETURN(w!=NULL);
    EXALT_ASSERT_RETURN(essid!=NULL);

    EXALT_ASSERT_RETURN(w->networks!=NULL);

    EINA_LIST_FOREACH(w->networks,l,wi)
    {
        if(!strcmp(essid, exalt_wireless_network_essid_get(wi)))
            return wi;
    }
    return NULL;
}



void exalt_wireless_scan_start(Exalt_Ethernet* eth)
{
    Exalt_Wireless* w;
    char buf[2048];
    size_t buf_len=sizeof(buf)-1;

    EXALT_ASSERT_RETURN_VOID(eth!=NULL);
    EXALT_ASSERT_RETURN_VOID(exalt_eth_wireless_is(eth));
    w = exalt_eth_wireless_get(eth);
    if(w->scanning)
        return ;

    buf_len=sizeof(buf)-1;
    exalt_wpa_ctrl_command(w->monitor,"SCAN",buf,buf_len);
    buf_len=sizeof(buf)-1;

    w->scanning = 1;
}


int exalt_wireless_radiobutton_on_is(Exalt_Wireless *w)
{
    struct iwreq wrq;
    EXALT_ASSERT_RETURN(w!=NULL);

    strncpy(wrq.ifr_name, exalt_eth_name_get(exalt_wireless_eth_get(w)), sizeof(wrq.ifr_name));
    if(!exalt_ioctl(&wrq, SIOCGIWNAME))
        return -1;

    if(strcmp("radio off",wrq.u.name)==0)
        return 0;
    else
        return 1;
}



int exalt_wireless_conf_apply(Exalt_Wireless *w)
{
    struct wpa_ctrl *ctrl_conn;
    Exalt_Ethernet* eth;
    Exalt_Configuration* c;
    char buf_res[1024];
    char buf_cmd[1024];
    size_t buf_len;
    Exalt_Configuration_Network *n;

    EXALT_ASSERT_RETURN(w!=NULL);
    eth = exalt_wireless_eth_get(w);

    c = exalt_eth_configuration_get(eth);
    EXALT_ASSERT_RETURN(exalt_conf_valid_is(c));
    EXALT_ASSERT_RETURN(exalt_conf_wireless_is(c));

    n = exalt_conf_network_get(c);

    //exalt_conf_network_print(n);

    //open a confection with wpa_supplicant
    //create a new network configuration
    //set the essid
    ctrl_conn = exalt_wpa_open_connection(exalt_eth_wireless_get(eth));
    EXALT_ASSERT_RETURN(ctrl_conn!=NULL);

    buf_len=sizeof(buf_res)-1;
    exalt_wpa_ctrl_command(ctrl_conn,"ADD_NETWORK",buf_res,buf_len);
    EXALT_ASSERT_RETURN(strcmp(buf_res,"FAIL")!=0);
    int network_id = atoi(buf_res);

    buf_len=sizeof(buf_res)-1;
    snprintf(buf_cmd,1024,"SET_NETWORK %d ssid \"%s\"",
            network_id,
            exalt_conf_network_essid_get(n));
    exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);



    if(!exalt_conf_network_encryption_is(n))
    {
        //printf("APPLY NO ENCRYPTION \n");

        buf_len=sizeof(buf_res)-1;
        snprintf(buf_cmd,1024,"SET_NETWORK %d key_mgmt NONE",
                network_id);
        exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);
    }
    else if(exalt_conf_network_wep_is(n))
    {
        //printf("APPLY WEP encryption\n");

        buf_len=sizeof(buf_res)-1;
        snprintf(buf_cmd,1024,"SET_NETWORK %d key_mgmt NONE",
                network_id);
        exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);

        buf_len=sizeof(buf_res)-1;
        if(exalt_conf_network_wep_hexa_is(n))
            snprintf(buf_cmd,1024,"SET_NETWORK %d wep_key0 %s",
                    network_id,
                    exalt_conf_network_key_get(n));
        else
            snprintf(buf_cmd,1024,"SET_NETWORK %d wep_key0 \"%s\"",
                    network_id,
                    exalt_conf_network_key_get(n));

        exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);

        buf_len=sizeof(buf_res)-1;
        snprintf(buf_cmd,1024,"SET_NETWORK %d wep_tx_keyidx 0",
                network_id);
        exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);
    }
    else
    {
        //
        char * s = NULL;
        switch(exalt_conf_network_wpa_type_get(n))
        {
            case WPA_TYPE_WPA: s = "WPA"; break;
            case WPA_TYPE_WPA2: s = "WPA2"; break;
            case WPA_TYPE_UNKNOWN: EXALT_LOG_WARN("WPA_TYPE_UNKNOWN"); break;
        }
        buf_len=sizeof(buf_res)-1;
        snprintf(buf_cmd,1024,"SET_NETWORK %d proto %s",
                network_id,
                s);

        exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);
        //

        //
        if(exalt_conf_network_mode_get(n) == MODE_IBSS)
            s = "WPA-NONE";
        else
        {
            switch(exalt_conf_network_auth_suites_get(n))
            {
                case AUTH_SUITES_PSK: s = "WPA-PSK"; break;
                case AUTH_SUITES_EAP: s = "WPA-EAP"; break;
                case AUTH_SUITES_UNKNOWN: EXALT_LOG_WARN("AUTH_SUITES_UNKNOWN"); break;
                case AUTH_SUITES_NONE: EXALT_LOG_WARN("AUTH_SUITES_NONE"); break;
            }
        }
        buf_len=sizeof(buf_res)-1;
        snprintf(buf_cmd,1024,"SET_NETWORK %d key_mgmt %s",
                network_id,
                s);
        //printf("# %s\n",buf_cmd);
        exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);
        //

        //
        if(exalt_conf_network_mode_get(n) == MODE_IBSS)
            s = "NONE";
        else
        {
            switch(exalt_conf_network_pairwise_cypher_get(n))
            {
                case CYPHER_NAME_TKIP: s = "TKIP"; break;
                case CYPHER_NAME_CCMP: s = "CCMP"; break;
                case CYPHER_NAME_UNKNOWN: EXALT_LOG_WARN("CYPHER_NAME_UNKNOWN"); break;
                case CYPHER_NAME_NONE: EXALT_LOG_WARN("CYPHER_NAME_NONE"); break;
            }
        }
        buf_len=sizeof(buf_res)-1;
        snprintf(buf_cmd,1024,"SET_NETWORK %d pairwise %s",
                network_id,
                s);
        //printf("# %s\n",buf_cmd);
        exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);
        //


        if(exalt_conf_network_auth_suites_get(n) == AUTH_SUITES_EAP)
        {
            //
            switch(exalt_conf_network_eap_get(n))
            {
                case EAP_TLS: s = "TLS"; break;
                case EAP_UNKNOWN: EXALT_LOG_WARN("EAP_UNKNOWN"); break;
            }

            buf_len=sizeof(buf_res)-1;
            snprintf(buf_cmd,1024,"SET_NETWORK %d eap %s",
                    network_id,
                    s);
            exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);
            //

            buf_len=sizeof(buf_res)-1;
            snprintf(buf_cmd,1024,"SET_NETWORK %d ca_cert %s",
                    network_id,
                    exalt_conf_network_ca_cert_get(n));
            exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);

            buf_len=sizeof(buf_res)-1;
            snprintf(buf_cmd,1024,"SET_NETWORK %d client_cert %s",
                    network_id,
                    exalt_conf_network_client_cert_get(n));
            exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);

            buf_len=sizeof(buf_res)-1;
            snprintf(buf_cmd,1024,"SET_NETWORK %d private_key %s",
                    network_id,
                    exalt_conf_network_private_key_get(n));
            exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);

            buf_len=sizeof(buf_res)-1;
            snprintf(buf_cmd,1024,"SET_NETWORK %d private_key_passwd %s",
                    network_id,
                    exalt_conf_network_key_get(n));
            exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);
        }

        //
        switch(exalt_conf_network_group_cypher_get(n))
        {
            case CYPHER_NAME_TKIP: s = "TKIP"; break;
            case CYPHER_NAME_CCMP: s = "CCMP"; break;
            case CYPHER_NAME_UNKNOWN: EXALT_LOG_WARN("CYPHER_NAME_UNKNOWN"); break;
            case CYPHER_NAME_NONE: EXALT_LOG_WARN("CYPHER_NAME_NONE"); break;
        }

        buf_len=sizeof(buf_res)-1;
        snprintf(buf_cmd,1024,"SET_NETWORK %d group %s",
                network_id,
                s);
        exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);
        //

        buf_len=sizeof(buf_res)-1;
        snprintf(buf_cmd,1024,"SET_NETWORK %d psk \"%s\"",
                network_id,
                exalt_conf_network_key_get(n));
        exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);
    }

    //tell to wpa_supplicant to use this new network
    //select the network
    //


    buf_len=sizeof(buf_res)-1;
    snprintf(buf_cmd,1024,"SET_NETWORK %d mode %d",
            network_id,
            exalt_conf_network_mode_get(n));
    exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);

    if(exalt_conf_network_mode_get(n) == MODE_IBSS)
    {
        buf_len=sizeof(buf_res)-1;
        snprintf(buf_cmd,1024,"AP_SCAN 2");
        exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);
    }
    else
    {
        buf_len=sizeof(buf_res)-1;
        snprintf(buf_cmd,1024,"AP_SCAN 1");
        exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);
    }

    buf_len=sizeof(buf_res)-1;
    snprintf(buf_cmd,1024,"SELECT_NETWORK %d",network_id);
    exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);

    buf_len=sizeof(buf_res)-1;
    snprintf(buf_cmd,1024,"REASSOCIATE");
    exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);

    buf_len=sizeof(buf_res)-1;
    snprintf(buf_cmd,1024,"RECONNECT");
    exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);



    //close the confection
    wpa_ctrl_close(ctrl_conn);
    ctrl_conn=NULL;

    return 1;
}



void exalt_wireless_printf(Exalt_Wireless *w)
{
    printf("\t## Connected essid: %s\n",exalt_wireless_essid_get(w));
    printf("\t## Radio button: %s\n",(exalt_wireless_radiobutton_on_is(w)>0?"on":"off"));
}



/** @} */



/*
 * Private functions bodies
 */

/**
 * @brief called by _exalt_rtlink_watch_cb when the kernel send an information
 */
int _exalt_rtlink_essid_change(Exalt_Wireless *w)
{
    char* essid, *save_essid;

    EXALT_ASSERT_RETURN(w!=NULL);

    essid = exalt_wireless_essid_get(w);
    save_essid = _exalt_wireless_save_essid_get(w);

    if(   (!essid && save_essid)
            || (essid && !save_essid)
            || (essid && save_essid && strcmp(essid,save_essid)!=0))
    {
        _exalt_wireless_save_essid_set(w,exalt_wireless_essid_get(w));
        if(exalt_eth_interfaces.eth_cb)
            exalt_eth_interfaces.eth_cb(exalt_wireless_eth_get(w),EXALT_WIRELESS_ACTION_ESSIDCHANGE,exalt_eth_interfaces.eth_cb_user_data);
        EXALT_FREE(essid);
        return 1;
    }

    EXALT_FREE(essid);
    return 0;
}

/**
 * @brief set the save essid of the interface "w"
 * @param w the interface
 * @param essid the save essid
 * @return Returns 1 if the save essid is apply, else -1
 */
int _exalt_wireless_save_essid_set(Exalt_Wireless* w,const char* essid)
{
    EXALT_ASSERT_RETURN(w!=NULL);
    EXALT_ASSERT_RETURN(essid!=NULL);

    EXALT_FREE(w->_save_essid);
    w->_save_essid=strdup(essid);
    return 1;
}

/**
 * @brief get the save essid of the interface "w"
 * @param w the interface
 * @return Returns the save essid
 */
char* _exalt_wireless_save_essid_get(Exalt_Wireless* w)
{
    EXALT_ASSERT_RETURN(w!=NULL);
    return w->_save_essid;
}

void _exalt_wireless_wpa_connect(Exalt_Wireless *w)
{
    EXALT_ASSERT_RETURN_VOID(w!=NULL);
    if(!w->monitor)
    {
        w->monitor = exalt_wpa_open_connection(w);
        if(w->monitor)
            w->fd_handler =
                ecore_main_fd_handler_add(wpa_ctrl_get_fd(w->monitor),
                        ECORE_FD_READ,_exalt_wireless_wpa_cb,w,NULL,NULL);
    }
}

/**
 * Retrieve notification from the wpa_supplicant daemon
 */
Eina_Bool _exalt_wireless_wpa_cb(void *data, Ecore_Fd_Handler *fd_handler __UNUSED__)
{
    Exalt_Wireless *w = data;
    Exalt_Ethernet *eth = exalt_wireless_eth_get(w);
    EXALT_ASSERT_RETURN(w!=NULL);
    size_t buf_len = 1024;
    char buf[1024];

    memcpy(buf,"\0",sizeof("\0"));

    _exalt_wireless_wpa_connect(w);
    wpa_ctrl_recv(w->monitor, buf, &buf_len);

    //printf("## %s\n",buf);
    const char *scan_result = "<2>CTRL-EVENT-SCAN-RESULTS";
    const char *connected = "<2>CTRL-EVENT-CONNECTED";
    const char *disconnected = "<2>CTRL-EVENT-DISCONNECTED";

    if(strlen(buf)>=strlen(scan_result)
            && strncmp(buf,scan_result,strlen(scan_result))==0)
    {
        _exalt_wireless_scan(w);
    }
    else if(strlen(buf)>=strlen(connected)
            && strncmp(buf,connected,strlen(connected))==0
            && !exalt_eth_connected_is(eth))
    {
        exalt_eth_connected_set(eth,1);
        if(exalt_eth_interfaces.eth_cb)
            exalt_eth_interfaces.eth_cb(eth,
                    EXALT_WIRELESS_ACTION_CONNECTED,
                    exalt_eth_interfaces.eth_cb_user_data);
    }
    else  if(strlen(buf)>=strlen(disconnected)
            && strncmp(buf,disconnected,strlen(disconnected))==0
            && exalt_eth_connected_is(eth))
    {
        exalt_eth_connected_set(eth,0);
        if(exalt_eth_interfaces.eth_cb)
            exalt_eth_interfaces.eth_cb(eth,
                    EXALT_WIRELESS_ACTION_DISCONNECTED,
                    exalt_eth_interfaces.eth_cb_user_data);

        //all interfaces are disconnected
        Eina_List *l;
        Exalt_Ethernet *_eth;
        int connected = 0;
        EINA_LIST_FOREACH(exalt_eth_interfaces.ethernets, l, _eth)
        {
            if(exalt_eth_connected_is(_eth))
            {
                connected = 1;
                break;
            }
        }
        if(!connected && exalt_eth_interfaces.eth_cb)
            exalt_eth_interfaces.eth_cb(eth,
                    EXALT_IFACE_ACTION_ALL_IFACES_DISCONNECTED,
                    exalt_eth_interfaces.eth_cb_user_data);

    }

    return EINA_TRUE;
}

/**
 * @brief scan networks
 * @param data the interface (Exalt_Wireless)
 * @return Returns 1 if success, else 0
 */
int _exalt_wireless_scan(Exalt_Wireless *w)
{
    char buf[100000];
    size_t buf_len;
    Exalt_Wireless_Network *n;

    memcpy(buf,"\0",sizeof("\0"));

    EXALT_ASSERT_RETURN(w!=NULL);

    _exalt_wireless_wpa_connect(w);

    EINA_LIST_FREE(w->networks,n);
    {
        if(n)
            exalt_wireless_network_free(&n);
    }


    buf_len=sizeof(buf)-1;
    exalt_wpa_ctrl_command(w->monitor,"SCAN_RESULTS",buf,buf_len);

    w->networks = exalt_wpa_parse_scan_results(w->monitor,buf,w);

    //TODO: Code used to add a custom networks to do some tests  !!!!
    /*Exalt_Wireless_Network *_n = exalt_wireless_network_new(w);
    exalt_wireless_network_address_set(_n, "TEST MAC ADDRESS");
    exalt_wireless_network_essid_set(_n, "Network used to do some tests");
    exalt_wireless_network_encryption_set(_n, 1);
    exalt_wireless_network_description_set(_n,"EAP tests");
    exalt_wireless_network_quality_set(_n, 87);
    exalt_wireless_network_mode_set(_n, MODE_INFRASTRUCTURE);

    Exalt_Wireless_Network_IE *ie = exalt_wireless_network_ie_new();
    Eina_List *l_ie = eina_list_append(NULL, ie);
    exalt_wireless_network_ie_set(_n, l_ie);
    exalt_wireless_network_ie_description_set(ie, "EAP test network");
    exalt_wireless_network_ie_wpa_type_set(ie, WPA_TYPE_WPA);
    exalt_wireless_network_ie_group_cypher_set(ie, CYPHER_NAME_TKIP);
    exalt_wireless_network_ie_pairwise_cypher_set(ie, CYPHER_NAME_TKIP, 0);
    exalt_wireless_network_ie_pairwise_cypher_number_set(ie, 1);
    exalt_wireless_network_ie_auth_suites_set(ie, AUTH_SUITES_EAP, 0);
    exalt_wireless_network_ie_auth_suites_number_set(ie, 1);
    exalt_wireless_network_ie_eap_set(ie, EAP_TLS);

    w->networks = eina_list_append(w->networks, _n);
    */
    //printf("# SCAN RESULT %d\n",eina_list_count(w->networks));

    w->scanning = 0;
    //send a broadcast
    if(exalt_eth_interfaces.wireless_scan_cb)
        exalt_eth_interfaces.wireless_scan_cb(exalt_wireless_eth_get(w),
                w->networks,
                exalt_eth_interfaces.wireless_scan_cb_user_data);

    return 0;
}



int exalt_wireless_scan_cb_set(Exalt_Wireless_Scan_Cb fct, void * user_data)
{
    exalt_eth_interfaces.wireless_scan_cb = fct;
    exalt_eth_interfaces.wireless_scan_cb_user_data = user_data;
    return 1;
}


