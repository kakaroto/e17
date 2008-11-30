/** @file exalt_wireless.c */
#include "exalt_wireless.h"
#include "libexalt_private.h"
#include "../config.h"

/*
 * Private functions headers
 */

void _exalt_wireless_scan_free(wireless_scan **w);
char* _exalt_wireless_get_save_essid(Exalt_Wireless* w);
int _exalt_wireless_set_save_essid(Exalt_Wireless* w,const char* essid);

int _exalt_wireless_scan(void* data);

struct wpa_ctrl * _exalt_wpa_open_connection(const char *ifname);
int _exalt_wpa_ctrl_command(struct wpa_ctrl *ctrl, char *cmd);

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
    int retry;
};




/*
 * Constructor / Destructor
 */


/**
 * @brief create a new Exalt_Wireless
 * @return Returns the new Exalt_Wireless structure
 */
Exalt_Wireless* exalt_wireless_new(Exalt_Ethernet* eth)
{
    Exalt_Wireless *w;
    char* str;

    EXALT_ASSERT_RETURN(eth!=NULL);

    w = (Exalt_Wireless*)malloc((unsigned int)sizeof(Exalt_Wireless));
    EXALT_ASSERT_RETURN(w!=NULL);

    w -> eth = eth;

    w->wpasupplicant_driver = NULL;
    w -> _save_essid = NULL;

    str = exalt_wireless_get_essid(w);
    _exalt_wireless_set_save_essid(w,str);
    EXALT_FREE(str);

    w -> networks = NULL;

    w->scan_fd = iw_sockets_open();
    w->scanning = 0;
    w->retry = 0;
    //default driver
    EXALT_STRDUP(w->wpasupplicant_driver,"wext");
    return w;
}



/**
 * @brief free a Exalt_Wireless
 * @param w the Exalt_Wireless
 */
void exalt_wireless_free(Exalt_Wireless* w)
{
    EXALT_ASSERT_RETURN_VOID(w!=NULL);
    eina_list_free(w->networks);

    EXALT_FREE(w->_save_essid);
    EXALT_FREE(w->wpasupplicant_driver);
    EXALT_CLOSE(w->scan_fd);
    EXALT_FREE(w);
}




/*
 * Set/Get functions for Exalt_Wireless
 */




/**
 * @brief return the current essid
 * @param w the Exalt_Wireless
 * @return Returns the current essid (don't forget to free it), NULL if no essid is associated
 */
char* exalt_wireless_get_essid(Exalt_Wireless* w)
{
    char essid[IW_ESSID_MAX_SIZE +1];
    struct iwreq wrq;
    Exalt_Ethernet *eth;

    EXALT_ASSERT_RETURN(w!=NULL);

    eth = exalt_wireless_get_ethernet(w);

    strncpy(wrq.ifr_name, exalt_eth_get_name(eth), sizeof(wrq.ifr_name));
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

/**
 * @brief set the driver used by wpa_supplicant
 * @param w the Exalt_Wireless
 * @param driver the driver (wext ...)
 * @return Returns 1 if the driver is set, else 0
 */
int exalt_wireless_set_wpasupplicant_driver(Exalt_Wireless* w, const char* driver)
{
    EXALT_ASSERT_RETURN(w!=NULL);
    EXALT_ASSERT_RETURN(driver!=NULL);
    EXALT_FREE(w->wpasupplicant_driver);
    w->wpasupplicant_driver = strdup(driver);
    return 1;
}

/**
 * @brief return the driver used by wpa_supplicant
 * @param w the Exalt_Wireless
 * @return Returns the driver
 */
char* exalt_wireless_get_wpasupplicant_driver(Exalt_Wireless* w)
{
    EXALT_ASSERT_RETURN(w!=NULL);
    return w->wpasupplicant_driver;
}

/*
 * @brief get the ethernet structure
 * @param w the Exalt_Wireless
 * @return Returns the exalt_ethernet structure associated to w
 */
Exalt_Ethernet* exalt_wireless_get_ethernet(Exalt_Wireless* w)
{
    EXALT_ASSERT_RETURN(w!=NULL);
    return w->eth;
}



/**
 * @brief return the wireless networks list result of a scan
 * @param w the wireless interface
 * @return Returns a list of Exalt_Wireless_Network
 */
Eina_List* exalt_wireless_get_networks_list(Exalt_Wireless* w)
{
    EXALT_ASSERT_RETURN(w!=NULL);
    return w->networks;
}

/*
 * Set/get functions for Exalt_Wireless_Network
 */


/**
 * @brief get information about a wireless network (essid, quality ...)
 * @param w the Exalt_Wireless
 * @param nb the position of the wireless network in the scan list
 * @return Return information about the network
 */
Exalt_Wireless_Network* exalt_wireless_get_networkinfo(Exalt_Wireless* w, int nb)
{
    EXALT_ASSERT_RETURN(w!=NULL);
    return Exalt_Wireless_Network(eina_list_nth(w->networks,nb));
}



/**
 * @brief get information about the wireless network named essid
 * @param w the Exalt_Wireless
 * @param essid the essid
 * @return Returns information about this network
 */
Exalt_Wireless_Network* exalt_wireless_get_networkinfo_by_essid(Exalt_Wireless* w,const char *essid)
{
    Eina_List *l;
    Exalt_Wireless_Network* wi=NULL;

    EXALT_ASSERT_RETURN(w!=NULL);
    EXALT_ASSERT_RETURN(essid!=NULL);

    EXALT_ASSERT_RETURN(w->networks!=NULL);

    EINA_LIST_FOREACH(w->networks,l,wi)
    {
        if(!strcmp(essid, exalt_wirelessnetwork_essid_get(wi)))
            return wi;
    }
    return NULL;
}

/*
 * Scan functions
 */




/**
 * @brief start a scan
 * When this scan will be done, the callback function set by exalt_eth_set_scan_cb() will be called
 * @param eth the Exalt_Ethernet where scan
 */
void exalt_wireless_scan_start(Exalt_Ethernet* eth)
{
    Exalt_Wireless* w;

    EXALT_ASSERT_RETURN_VOID(eth!=NULL);
    EXALT_ASSERT_RETURN_VOID(exalt_eth_is_wireless(eth));
    w = exalt_eth_get_wireless(eth);
    if(w->scanning)
        return ;

    w->scanning = 1;
    ecore_timer_add(0, _exalt_wireless_scan, eth);
}


/*
 * Others
 */


/**
 * @brief get the radio button state
 * @param w the interface
 * @return Return -1 if a error is occurs, 0 if the button is off, 1 if it is on
 */
short exalt_wireless_radiobutton_ison(Exalt_Wireless *w)
{
    struct iwreq wrq;
    EXALT_ASSERT_RETURN(w!=NULL);

    strncpy(wrq.ifr_name, exalt_eth_get_name(exalt_wireless_get_ethernet(w)), sizeof(wrq.ifr_name));
    if(!exalt_ioctl(&wrq, SIOCGIWNAME))
        return -1;

    if(strcmp("radio off",wrq.u.name)==0)
        return 0;
    else
        return 1;
}



/**
 * @brief apply the connection (the current essid, current password ...)
 * @param w the interface
 * @return Return 1 if the configuration is apply, else 0
 */
int exalt_wireless_apply_conn(Exalt_Wireless *w)
{
    struct wpa_ctrl *ctrl_conn;
    Exalt_Ethernet* eth;
    Exalt_Connection* c;
    Exalt_Enum_Encryption_Mode encryption_mode;
    char essid[IW_ESSID_MAX_SIZE+1];
    struct iwreq wrq;
    unsigned char key[IW_ENCODING_TOKEN_MAX];
    char buf[1024];
    int keylen = 0;

    EXALT_ASSERT_RETURN(w!=NULL);
    eth = exalt_wireless_get_ethernet(w);

    c = exalt_eth_get_connection(eth);
    EXALT_ASSERT_RETURN(exalt_conn_is_valid(c));
    EXALT_ASSERT_RETURN(exalt_conn_is_wireless(c));

    encryption_mode = exalt_conn_get_encryption_mode(c);
    switch(encryption_mode)
    {
        case EXALT_ENCRYPTION_NONE:
        case EXALT_ENCRYPTION_WEP_ASCII:
        case EXALT_ENCRYPTION_WEP_HEXA:
#ifdef  HAVE_WPA_SUPPLICANT
            _exalt_wpa_stop(w);
#endif
            strncpy(wrq.ifr_name, exalt_eth_get_name(eth), sizeof(wrq.ifr_name));
            wrq.u.essid.flags = 0;
            wrq.u.data.length = 0;
            wrq.u.data.pointer = (caddr_t) NULL;

            //set the key and the security mode
            switch(encryption_mode)
            {
                case EXALT_ENCRYPTION_NONE:
                    keylen = 0;
                    wrq.u.data.flags |= IW_ENCODE_DISABLED;
                    break;
                case EXALT_ENCRYPTION_WEP_ASCII:
                    keylen = sprintf(buf,"s:%s", exalt_conn_get_key(c));
                    iw_in_key(buf,key);
                    break;
                case EXALT_ENCRYPTION_WEP_HEXA:
                    keylen = iw_in_key(exalt_conn_get_key(c),key);
                    break;
                default:
                    break;
            }

            if(keylen > 0)
            {
                wrq.u.data.length = keylen;
                wrq.u.data.pointer = (caddr_t) key;
            }
            else
                wrq.u.data.flags |= IW_ENCODE_NOKEY;

            switch(exalt_conn_get_security_mode(c))
            {
                case EXALT_SECURITY_OPEN:
                    wrq.u.data.flags |= IW_ENCODE_OPEN;
                    break;
                default:
                    wrq.u.data.flags |= IW_ENCODE_RESTRICTED;
                    break;
            }

            if(!exalt_ioctl(&wrq, SIOCSIWENCODE))
                return -1;

            //set the mode (Ad-hoc, Managed)
            switch(exalt_conn_get_connection_mode(c))
            {
                case EXALT_CONNECTION_ADHOC:
                    wrq.u.mode = 1;
                    break;
                case EXALT_CONNECTION_MANAGED:
                    wrq.u.mode = 2;
                    break;
            }
            if(!exalt_ioctl(&wrq, SIOCSIWMODE))
                return -1;


            //set the essid
            strncpy(essid, exalt_conn_get_essid(c), IW_ESSID_MAX_SIZE+1);
            wrq.u.essid.pointer = (caddr_t) essid;
            wrq.u.essid.length = strlen(essid)+1;
            if(!exalt_ioctl(&wrq, SIOCSIWESSID))
                return -1;

            break;
        default:
#ifdef  HAVE_WPA_SUPPLICANT
            exalt_conf_save_wpasupplicant(w);
            //reload wpa_supplicant configuration
            //we stop the current instance, because maybe we want use a different driver
            _exalt_wpa_stop(w);

            ctrl_conn = _exalt_wpa_open_connection(exalt_eth_get_name(eth));
            if(!ctrl_conn)
            {
                int status;
                Ecore_Exe *exe;
                print_error(__FILE__,__func__,__LINE__,"Could not connect to wpa_supplicant, try to launch it");
                sprintf(buf,COMMAND_WPA,
                        exalt_wireless_get_wpasupplicant_driver(exalt_eth_get_wireless(eth)),
                        exalt_eth_get_name(eth),
                        EXALT_WPA_CONF_FILE,
                        EXALT_WPA_INTERFACE_DIR);
                //printf("%s\n",buf);
                exe = ecore_exe_run(buf, NULL);
                waitpid(ecore_exe_pid_get(exe), &status, 0);
                ecore_exe_free(exe);
                print_error(__FILE__,__func__,__LINE__,"Re-try to connect to wpa_supplicant");
                ctrl_conn = _exalt_wpa_open_connection(exalt_eth_get_name(eth));
                EXALT_ASSERT_RETURN(ctrl_conn!=NULL);
                print_error(__FILE__,__func__,__LINE__,"Connection succesfull");
            }

            EXALT_ASSERT_RETURN(_exalt_wpa_ctrl_command(ctrl_conn, "RECONFIGURE"));

            //close the connection
            wpa_ctrl_close(ctrl_conn);
            ctrl_conn=NULL;
#else
            EXALT_ASSERT_ADV(0,,"Your build of libexalt doesn't support wpa_supplicant");
#endif
            break;
    }

    return 1;
}



/**
 * @brief Print all informations about the interface in stdout
 * @param w the Exalt_Wireless
 */
void exalt_wireless_printf(Exalt_Wireless *w)
{
    printf("\t## Connected essid: %s\n",exalt_wireless_get_essid(w));
    printf("\t## Radio button: %s\n",(exalt_wireless_radiobutton_ison(w)>0?"on":"off"));
}



/** @} */



/*
 * Private functions bodies
 */

/**
 * @brief open a connection with the wpa daemon
 * @param ifname the interface name (eth1 ...)
 * @return Return the connection else NULL
 */
struct wpa_ctrl * _exalt_wpa_open_connection(const char *ifname)
{
    char *cfile;
    int flen;
    struct wpa_ctrl* ctrl_conn;
    EXALT_ASSERT_RETURN(ifname!=NULL);

    flen = strlen(EXALT_WPA_IFACE_DIR) + strlen(ifname) + 2;
    cfile = malloc(flen);
    if (cfile == NULL)
        return NULL;
    snprintf(cfile, flen, "%s/%s", EXALT_WPA_IFACE_DIR, ifname);
    ctrl_conn = wpa_ctrl_open(cfile);
    free(cfile);
    return ctrl_conn;
}

/**
 * @brief kill a wpa_supplicant daemon if it exists
 * @param w the wireless interface
 */
void _exalt_wpa_stop(Exalt_Wireless* w)
{
#ifdef  HAVE_WPA_SUPPLICANT
    struct wpa_ctrl *ctrl_conn;
    Exalt_Ethernet* eth;
    EXALT_ASSERT_RETURN_VOID(w!=NULL);
    eth = exalt_wireless_get_ethernet(w);

    ctrl_conn = _exalt_wpa_open_connection(exalt_eth_get_name(eth));
    if(ctrl_conn)
    {
        _exalt_wpa_ctrl_command(ctrl_conn, "TERMINATE");
        wpa_ctrl_close(ctrl_conn);
        ctrl_conn=NULL;
        //the wpa_supplicant daemon deactivate the interface ...
        while(exalt_eth_is_up(eth))
            ;
        exalt_eth_up_without_apply(eth);
    }
#endif
}


/**
 * @brief send a command to the wpa_supplicant daemon
 * @param ctrl_conn the connection to the wpa_supplicant daemon
 * @param cmd the command (RECONFIGURE for example)
 * @return Return 1 if success, else 0
 */
int _exalt_wpa_ctrl_command(struct wpa_ctrl *ctrl_conn, char *cmd)
{
    char buf[2048];
    size_t len;
    int ret;

    EXALT_ASSERT_RETURN(ctrl_conn!=NULL);
    len = sizeof(buf) - 1;
    ret = wpa_ctrl_request(ctrl_conn, cmd, strlen(cmd), buf, &len,
            /*wpa_cli_msg_cb*/ NULL);

    EXALT_ASSERT_RETURN(ret>=0);

    return 1;
}

/**
 * @brief called by _exalt_rtlink_watch_cb when the kernel send an information
 */
int _exalt_rtlink_essid_change(Exalt_Wireless *w)
{
    char* essid, *save_essid;

    EXALT_ASSERT_RETURN(w!=NULL);

    essid = exalt_wireless_get_essid(w);
    save_essid = _exalt_wireless_get_save_essid(w);

    if(   (!essid && save_essid)
            || (essid && !save_essid)
            || (essid && save_essid && strcmp(essid,save_essid)!=0))
    {
        _exalt_wireless_set_save_essid(w,exalt_wireless_get_essid(w));
        if(exalt_eth_interfaces.eth_cb)
            exalt_eth_interfaces.eth_cb(exalt_wireless_get_ethernet(w),EXALT_WIRELESS_CB_ACTION_ESSIDCHANGE,exalt_eth_interfaces.eth_cb_user_data);
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
int _exalt_wireless_set_save_essid(Exalt_Wireless* w,const char* essid)
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
char* _exalt_wireless_get_save_essid(Exalt_Wireless* w)
{
    EXALT_ASSERT_RETURN(w!=NULL);
    return w->_save_essid;
}

/**
 * @brief free a scan result of iwlib
 * @param ws the first result of the list result
 */
void _exalt_wireless_scan_free(wireless_scan **ws)
{
    wireless_scan* result,*r_save;
    EXALT_ASSERT_RETURN_VOID(ws!=NULL);

    result = *ws;

    while(result)
    {
        r_save = result;
        result = result->next;
        free(r_save);
    }
    *ws=NULL;
}

/**
 * @brief scan networks
 * @param data the interface (Exalt_Wireless)
 * @return Returns 1 if success, else 0
 */
int _exalt_wireless_scan(void *data)
{
    int fd;
    int delay; //in ms
    Eina_List* networks = NULL;
    Exalt_Wireless* w;
    char* cpy;

    EXALT_ASSERT_RETURN(data!=NULL);
    Exalt_Ethernet* eth = data;
    w = exalt_eth_get_wireless(eth);
    EXALT_ASSERT_RETURN(w!=NULL);

    fd = w->scan_fd;
    EXALT_ASSERT_RETURN(fd>=0);
    cpy = strdup(exalt_eth_get_name(eth));
    delay = iw_process_scan(fd, cpy, exalt_eth_interfaces.we_version, &networks, w, &(w->retry));
    EXALT_FREE(cpy);
    if(delay<=0)
    {
        printf("scan done!! %d\n",eina_list_count(networks));
        w->networks = networks;
        w->scanning=0;
        w->retry = 0;

        //send a broadcast
        if(exalt_eth_interfaces.wireless_scan_cb)
            exalt_eth_interfaces.wireless_scan_cb(eth,
                    networks,
                    exalt_eth_interfaces.wireless_scan_cb_user_data);
        return 0;
    }
    return delay;
}




