/** @file exalt_wireless.c */
#include "exalt_wireless.h"
#include "libexalt_private.h"
#include "../config.h"

/*
 * Private functions headers
 */

char* _exalt_wireless_save_essid_get(Exalt_Wireless* w);
int _exalt_wireless_save_essid_set(Exalt_Wireless* w,const char* essid);

int _exalt_wireless_scan(void *data, Ecore_Fd_Handler *fd_handler);

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



#define EXALT_FCT_NAME exalt_wireless
#define EXALT_STRUCT_TYPE Exalt_Wireless

EXALT_STRING_SET(wpasupplicant_driver)
EXALT_GET(wpasupplicant_driver,const char*)

EXALT_GET(eth,Exalt_Ethernet*)
EXALT_GET(networks,Eina_List*)

#undef EXALT_FCT_NAME
#undef EXALT_STRUCT_TYPE




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
        return strdup((char*) wrq.u.essid.pointer);
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

    w->monitor = exalt_wpa_open_connection(exalt_eth_wireless_get(eth));
    wpa_ctrl_attach(w->monitor);
    buf_len=sizeof(buf)-1;
    exalt_wpa_ctrl_command(w->monitor,"AP_SCAN 1",buf,buf_len);
    buf_len=sizeof(buf)-1;
    exalt_wpa_ctrl_command(w->monitor,"SCAN",buf,buf_len);
    buf_len=sizeof(buf)-1;

    w->scanning = 1;
    ecore_main_fd_handler_add(wpa_ctrl_get_fd(w->monitor), ECORE_FD_READ,_exalt_wireless_scan,eth,NULL,NULL);
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



int exalt_wireless_conn_apply(Exalt_Wireless *w)
{
    struct wpa_ctrl *ctrl_conn;
    Exalt_Ethernet* eth;
    Exalt_Connection* c;
    char buf_res[1024];
    char buf_cmd[1024];
    size_t buf_len;
    Exalt_Wireless_Network *n;
    Eina_List *l_ie;

    EXALT_ASSERT_RETURN(w!=NULL);
    eth = exalt_wireless_eth_get(w);

    c = exalt_eth_connection_get(eth);
    EXALT_ASSERT_RETURN(exalt_conn_valid_is(c));
    EXALT_ASSERT_RETURN(exalt_conn_wireless_is(c));

    n = exalt_conn_network_get(c);
    l_ie = exalt_wireless_network_ie_get(n);

    //open a connection with wpa_supplicant
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
            exalt_wireless_network_essid_get(n));
    exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);



    if(!exalt_wireless_network_encryption_is(n))
    {
        //printf("APPLY NO ENCRYPTION \n");

        buf_len=sizeof(buf_res)-1;
        snprintf(buf_cmd,1024,"SET_NETWORK %d key_mgmt NONE",
                network_id);
        exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);
    }
    else if(!l_ie)
    {
        //printf("APPLY WEP encryption\n");

        buf_len=sizeof(buf_res)-1;
        snprintf(buf_cmd,1024,"SET_NETWORK %d key_mgmt NONE",
                network_id);
        exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);

        buf_len=sizeof(buf_res)-1;
        if(exalt_conn_wep_key_hexa_is(c))
            snprintf(buf_cmd,1024,"SET_NETWORK %d wep_key0 %s",
                    network_id,
                    exalt_conn_key_get(c));
        else
            snprintf(buf_cmd,1024,"SET_NETWORK %d wep_key0 \"%s\"",
                    network_id,
                    exalt_conn_key_get(c));

        exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);

        buf_len=sizeof(buf_res)-1;
        snprintf(buf_cmd,1024,"SET_NETWORK %d wep_tx_keyidx 0",
                network_id);
        exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);
    }
    else
    {
        Exalt_Wireless_Network_IE* ie;

        int ie_choice = exalt_wireless_network_ie_choice_get(n);
        ie = eina_list_nth(l_ie,ie_choice);
        int auth_choice = exalt_wireless_network_ie_auth_choice_get(ie);
        int pairwise_choice = exalt_wireless_network_ie_pairwise_choice_get(ie);

        //
        char * s;
        switch(exalt_wireless_network_ie_wpa_type_get(ie))
        {
            case WPA_TYPE_WPA: s = "WPA"; break;
            case WPA_TYPE_WPA2: s = "WPA2"; break;
        }
        buf_len=sizeof(buf_res)-1;
        snprintf(buf_cmd,1024,"SET_NETWORK %d proto %s",
                network_id,
                s);

        exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);
        //

        //
        switch(exalt_wireless_network_ie_auth_suites_get(ie,auth_choice))
        {
            case AUTH_SUITES_PSK: s = "WPA-PSK"; break;
            case AUTH_SUITES_EAP: s = "WPA-EAP"; break;
        }
        buf_len=sizeof(buf_res)-1;
        snprintf(buf_cmd,1024,"SET_NETWORK %d key_mgmt %s",
                network_id,
                s);
        //printf("# %s\n",buf_cmd);
        exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);
        //

        //
        switch(exalt_wireless_network_ie_pairwise_cypher_get(ie,pairwise_choice))
        {
            case CYPHER_NAME_TKIP: s = "TKIP"; break;
            case CYPHER_NAME_CCMP: s = "CCMP"; break;
        }
        buf_len=sizeof(buf_res)-1;
        snprintf(buf_cmd,1024,"SET_NETWORK %d pairwise %s",
                network_id,
                s);
        //printf("# %s\n",buf_cmd);
        exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);
        //

        //
        switch(exalt_wireless_network_ie_group_cypher_get(ie))
        {
            case CYPHER_NAME_TKIP: s = "TKIP"; break;
            case CYPHER_NAME_CCMP: s = "CCMP"; break;
        }
        buf_len=sizeof(buf_res)-1;
        snprintf(buf_cmd,1024,"SET_NETWORK %d group %s",
                network_id,
                s);
        //printf("# %s\n",buf_cmd);
        exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);
        //

        buf_len=sizeof(buf_res)-1;
        snprintf(buf_cmd,1024,"SET_NETWORK %d psk \"%s\"",
                network_id,
                exalt_conn_key_get(c));
        exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);
    }

    //tell to wpa_supplicant to use this new network
    //select the network
    buf_len=sizeof(buf_res)-1;
    snprintf(buf_cmd,1024,"SELECT_NETWORK %d",network_id);
    exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);

    buf_len=sizeof(buf_res)-1;
    snprintf(buf_cmd,1024,"REASSOCIATE");
    exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);

    buf_len=sizeof(buf_res)-1;
    snprintf(buf_cmd,1024,"RECONNECT");
    exalt_wpa_ctrl_command(ctrl_conn,buf_cmd,buf_res,buf_len);



    //close the connection
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
            exalt_eth_interfaces.eth_cb(exalt_wireless_eth_get(w),EXALT_WIRELESS_CB_ACTION_ESSIDCHANGE,exalt_eth_interfaces.eth_cb_user_data);
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

/**
 * @brief scan networks
 * @param data the interface (Exalt_Wireless)
 * @return Returns 1 if success, else 0
 */
int _exalt_wireless_scan(void *data, Ecore_Fd_Handler *fd_handler)
{
    Exalt_Wireless* w;

    char buf[100000];
    size_t buf_len;
    Exalt_Wireless_Network *n;
    Eina_List *l, *l_next;

    memcpy(buf,"\0",sizeof("\0"));

    EXALT_ASSERT_RETURN(data!=NULL);
    Exalt_Ethernet* eth = data;
    w = exalt_eth_wireless_get(eth);
    EXALT_ASSERT_RETURN(w!=NULL);

    EINA_LIST_FOREACH_SAFE(w->networks,l,l_next,n);
    {
        if(n)
            exalt_wireless_network_free(&n);
        w->networks = eina_list_remove_list(w->networks, l);
    }


    buf_len=sizeof(buf)-1;
    exalt_wpa_ctrl_command(w->monitor,"SCAN_RESULTS",buf,buf_len);

    w->networks = exalt_wpa_parse_scan_results(w->monitor,buf,w);

    //printf("# SCAN RESULT %d\n",eina_list_count(w->networks));

    buf_len=sizeof(buf)-1;
    exalt_wpa_ctrl_command(w->monitor,"AP_SCAN 1",buf,buf_len);


    wpa_ctrl_close(w->monitor);

    ecore_main_fd_handler_del(fd_handler);


    w->scanning = 0;
    //send a broadcast
    if(exalt_eth_interfaces.wireless_scan_cb)
        exalt_eth_interfaces.wireless_scan_cb(eth,
                w->networks,
                exalt_eth_interfaces.wireless_scan_cb_user_data);

    return 0;
}




