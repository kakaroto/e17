/*
 * =====================================================================================
 *
 *       Filename:  exalt_wpa_supplicant.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  03/04/2009 08:30:21 PM CET
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "exalt_wpa_supplicant.h"


/**
 * @brief open a connection with the wpa daemon
 * @param ifname the interface name (eth1 ...)
 * @return Return the connection else NULL
 */
struct wpa_ctrl * exalt_wpa_open_connection(Exalt_Wireless *w)
{
    char *cfile;
    int flen;
    struct wpa_ctrl* ctrl_conn;
    char buf[2048];
    Exalt_Ethernet* eth;

    EXALT_ASSERT_RETURN(w!=NULL);
    eth = exalt_wireless_eth_get(w);

    flen = strlen(EXALT_WPA_IFACE_DIR) + strlen(exalt_eth_name_get(eth)) + 2;
    cfile = malloc(flen);
    if (cfile == NULL)
        return NULL;
    snprintf(cfile, flen, "%s/%s", EXALT_WPA_IFACE_DIR, exalt_eth_name_get(eth));
    ctrl_conn = wpa_ctrl_open(cfile);

    if(!ctrl_conn)
    {
        int status;
        Ecore_Exe *exe;
        print_error(__FILE__,__func__,__LINE__,"Could not connect to wpa_supplicant, try to launch it");

        if(!ecore_file_exists(EXALT_WPA_CONF_FILE))
        {
            //create the file
            FILE *f;
            f = fopen(EXALT_WPA_CONF_FILE,"w");
            fprintf(f,"ctrl_interface=%s",EXALT_WPA_INTERFACE_DIR);
            fclose(f);
        }

        sprintf(buf,COMMAND_WPA,
                exalt_wireless_wpasupplicant_driver_get(exalt_eth_wireless_get(eth)),
                exalt_eth_name_get(eth),
                EXALT_WPA_CONF_FILE,
                EXALT_WPA_INTERFACE_DIR);

        exe = ecore_exe_run(buf, NULL);
        waitpid(ecore_exe_pid_get(exe), &status, 0);
        ecore_exe_free(exe);
        print_error(__FILE__,__func__,__LINE__,"Re-try to connect to wpa_supplicant");
        ctrl_conn = wpa_ctrl_open(cfile);
        EXALT_ASSERT_CUSTOM_RET(ctrl_conn!=NULL,free(cfile);return NULL);
        print_error(__FILE__,__func__,__LINE__,"Connection succesfull");
    }

    free(cfile);
    return ctrl_conn;
}

/**
 * @brief kill a wpa_supplicant daemon if it exists
 * @param w the wireless interface
 */
void exalt_wpa_stop(Exalt_Wireless* w)
{
#ifdef  HAVE_WPA_SUPPLICANT
    struct wpa_ctrl *ctrl_conn;
    Exalt_Ethernet* eth;
    char buf[2048];
    int buf_len = sizeof(buf)-1;
    EXALT_ASSERT_RETURN_VOID(w!=NULL);

    ctrl_conn = exalt_wpa_open_connection(w);
    if(ctrl_conn)
    {
        exalt_wpa_ctrl_command(ctrl_conn, "TERMINATE",buf,buf_len);
        wpa_ctrl_close(ctrl_conn);
        ctrl_conn=NULL;
        //the wpa_supplicant daemon deactivate the interface ...
        while(exalt_eth_up_is(eth))
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
int exalt_wpa_ctrl_command(struct wpa_ctrl *ctrl_conn, char *cmd, char*buf, size_t buf_len)
{
    int ret;

    EXALT_ASSERT_RETURN(ctrl_conn!=NULL);
    ret = wpa_ctrl_request(ctrl_conn, cmd, strlen(cmd), buf, &buf_len,
            /*wpa_cli_msg_cb*/ NULL);

    EXALT_ASSERT_RETURN(ret>=0);

    return 1;
}


Eina_List* exalt_wpa_parse_scan_results(struct wpa_ctrl* ctrl,char *buf, Exalt_Wireless *w)
{
    Eina_List *networks=NULL;
    int len = strlen(buf);
    char *p;

    char *c = &(buf[0]);
    do
    {
        if(*c == '\n')
            *c='\0';
    } while( *(++c)!='\0');

    int i = 0;
    //jump the first line
    p = buf+strlen(buf)+1;
    while( p<buf+len-1 )
    {
        Exalt_Wireless_Network *wn =
            exalt_wireless_network_new(w);

        //printf("RESULT: %s\n",p);
        exalt_wpa_parse_1_result(ctrl,i,wn);

        //we add the network only if we have
        //no network with the same essid in the list
        if(exalt_wireless_network_essid_get(wn))
        {
            int find = 0;
            Eina_List *l_;
            Exalt_Wireless_Network *wn_;
            EINA_LIST_FOREACH(networks,l_,wn_)
            {
                if(strcmp( exalt_wireless_network_essid_get(wn_),
                            exalt_wireless_network_essid_get(wn)) == 0)
                {
                    find = 1;
                    break;
                }
            }
            if(!find)
            {
                networks = eina_list_append(networks,wn);
            }
            else
                exalt_wireless_network_free(&wn);
        }
        else
            exalt_wireless_network_free(&wn);
        i++;
        p=p+strlen(p)+1;
    }

    return networks;
}

void exalt_wpa_parse_1_result(struct wpa_ctrl* ctrl,
        int id, Exalt_Wireless_Network *wn)
{
    char buf2[2048];
    char buf3[1024];
    size_t buf_len;
    char *p2;
    int len2;
    char *c;

    buf_len=sizeof(buf2)-1;
    snprintf(buf3,1024,"BSS %d",id);
    exalt_wpa_ctrl_command(ctrl,buf3,buf2,buf_len);


    len2 = strlen(buf2);
    c = &(buf2[0]);
    do
    {
        if(*c == '\n')
            *c='\0';
    } while( *(++c)!='\0');

    p2 = buf2;
    while( p2<buf2+len2 )
    {
        char *p2_next=p2+strlen(p2)+1;
        char* key = p2;
        char* value = p2;

        while(value[0] != '=' && value[0]!='\0')
            value++;

        //sometimes we have an invalid line (without '=')
        if(value[0]!='\0')
        {
            value[0]='\0';
            value++;

            //printf("key %s value %s\n",key,value);
            exalt_wpa_parse_BBS_key(key,value,wn);
        }
        p2=p2_next;
    }
}

void exalt_wpa_parse_BBS_key(char* key, char* value, Exalt_Wireless_Network *wn)
{
    if(strcmp(key,"bssid")==0)
        exalt_wireless_network_address_set(wn,value);
    else if(strcmp(key,"ssid")==0)
        exalt_wireless_network_essid_set(wn,value);
    else if(strcmp(key,"qual")==0)
    {
        int qual = atoi(value);
        exalt_wireless_network_quality_set(wn,qual);
    }
    else if(strcmp(key,"flags") == 0)
    {
        exalt_wireless_network_description_set(wn,value);
        exalt_wpa_parse_flags(value,wn);
    }
}

void exalt_wpa_parse_flags(char* buf, Exalt_Wireless_Network *wn)
{
    char *p, *p_end;
    char *end;


    if(strcmp(buf,"") == 0)
        exalt_wireless_network_encryption_set(wn,0);
    else if( strcmp(buf,"[WEP]") ==  0)
        exalt_wireless_network_encryption_set(wn,1);
    else
    {
        exalt_wireless_network_encryption_set(wn,1);
        end=buf+strlen(buf);
        // jump [
        p=buf+1;
        p_end=p;
        while(p<end)
        {
            while(p_end[0]!=']')
                p_end++;
            p_end[0]='\0';

            Exalt_Wireless_Network_IE *ie =
                exalt_wireless_network_ie_new();
            exalt_wireless_network_ie_description_set(ie,p);
            exalt_wpa_parse_flag(p,ie);

            Eina_List *l;
            l = exalt_wireless_network_ie_get(wn);
            l = eina_list_append(l,ie);
            exalt_wireless_network_ie_set(wn,l);


            //jump ][
            p=p_end+2;
        }
    }
}

void exalt_wpa_parse_flag(char* buf, Exalt_Wireless_Network_IE *ie)
{
    char *p, *p_end;
    char *end;
    int param_id = 0;

    end=buf+strlen(buf);
    // jump [
    p=buf;
    p_end=p;
    while(p<end)
    {
        while(p_end[0]!='-')
        {
            if(p_end[0]=='[')
                break;
            p_end++;
        }
        p_end[0]='\0';
        //printf("HAHA %d %s\n",param_id,p);
        if(param_id==0 && strcmp(p,"WPA")==0)
        {
            exalt_wireless_network_ie_wpa_type_set(ie,WPA_TYPE_WPA);
            exalt_wireless_network_ie_group_cypher_set(ie,CYPHER_NAME_TKIP);
        }
        else if(param_id==0 && strcmp(p,"WPA2")==0)
        {
            exalt_wireless_network_ie_wpa_type_set(ie,WPA_TYPE_WPA2);
            exalt_wireless_network_ie_group_cypher_set(ie,CYPHER_NAME_CCMP);
        }
        else if(param_id==1 && strcmp(p,"PSK")==0)
        {
            exalt_wireless_network_ie_auth_suites_set(ie,AUTH_SUITES_PSK,0);
            exalt_wireless_network_ie_auth_suites_number_set(ie,1);
        }
        else if(param_id==1 && strcmp(p,"EAP")==0)
        {
            exalt_wireless_network_ie_auth_suites_set(ie,AUTH_SUITES_EAP,0);
            exalt_wireless_network_ie_auth_suites_number_set(ie,1);
        }
        else if(param_id==2 && strcmp(p,"TKIP+CCMP")==0)
        {
            exalt_wireless_network_ie_pairwise_cypher_set(ie,CYPHER_NAME_TKIP,0);
            exalt_wireless_network_ie_pairwise_cypher_set(ie,CYPHER_NAME_CCMP,1);
            exalt_wireless_network_ie_pairwise_cypher_number_set(ie,2);
        }
        else if(param_id==2 && strcmp(p,"TKIP")==0)
        {
            exalt_wireless_network_ie_pairwise_cypher_set(ie,CYPHER_NAME_TKIP,0);
            exalt_wireless_network_ie_pairwise_cypher_number_set(ie,1);
        }
        else if(param_id==2 && strcmp(p,"CCMP")==0)
        {
            exalt_wireless_network_ie_pairwise_cypher_set(ie,CYPHER_NAME_CCMP,0);
            exalt_wireless_network_ie_pairwise_cypher_number_set(ie,1);
        }
        param_id++;
        //jump -
        p=p_end+1;
    }
}

