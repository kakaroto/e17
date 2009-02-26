/*
 * =====================================================================================
 *
 *       Filename:  wireless_dialog.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  18/02/09 19:19:52 CET
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "e_mod_main.h"

void if_network_dialog_init(Instance* inst)
{
    inst->network.network = NULL;
}

void if_network_dialog_create(Instance* inst)
{
    Evas* evas;
    Evas_Object *list, *flist, *o, *lbl;
    E_Radio_Group* rg;
    int mh,mw;
    char buf[4096];

    inst->network.dialog = e_dialog_new(inst->gcc->gadcon->zone->container, "e", "exalt_wireless_dialog");
    e_dialog_title_set(inst->network.dialog, D_("Wireless Connection Settings"));
    inst->network.dialog->data = inst;

    evas = e_win_evas_get(inst->network.dialog->win);

    inst->network.table = e_widget_table_add(evas,0);

    flist = e_widget_frametable_add(evas, D_("Wireless network information"), 0);

    lbl = e_widget_label_add(evas,D_("Essid: "));
    e_widget_frametable_object_append(flist, lbl, 0, 0, 1, 1, 1, 0, 1, 0);
    inst->network.lbl_essid = e_widget_label_add(evas,"hehe");
    e_widget_frametable_object_append(flist, inst->network.lbl_essid, 1, 0, 1, 1, 1, 0, 1, 0);

    lbl = e_widget_label_add(evas,D_("Address: "));
    e_widget_frametable_object_append(flist, lbl, 0, 1, 1, 1, 1, 0, 1, 0);
    inst->network.lbl_address = e_widget_label_add(evas,"00:00:00:00:00:00");
    e_widget_frametable_object_append(flist, inst->network.lbl_address, 1, 1, 1, 1, 1, 0, 1, 0);

    lbl = e_widget_label_add(evas,D_("Quality: "));
    e_widget_frametable_object_append(flist, lbl, 0, 2, 1, 1, 1, 0, 1, 0);
    inst->network.lbl_quality = e_widget_label_add(evas,"hehe");
    e_widget_frametable_object_append(flist, inst->network.lbl_quality, 1, 2, 1, 1, 1, 0, 1, 0);

    lbl = e_widget_label_add(evas,D_("Authentification: "));
    e_widget_frametable_object_append(flist, lbl, 0, 3, 1, 1, 1, 0, 1, 0);
    inst->network.lbl_auth = e_widget_label_add(evas,"hehe");
    e_widget_frametable_object_append(flist, inst->network.lbl_auth, 1, 3, 1, 1, 1, 0, 1, 0);

    e_widget_table_object_append(inst->network.table,flist,0,0,1,1,1,1,1,0);


    //IFACE frame
    flist = e_widget_frametable_add(evas, D_("Wireless interface configuration"), 0);

    inst->network.icon = edje_object_add(evas);
    snprintf(buf,1024,"%s/e-module-exalt.edj",exalt_conf->module->dir);
    edje_object_file_set(inst->network.icon, buf,"modules/exalt/icons/wireless");
    evas_object_show(inst->network.icon);
    o = e_widget_image_add_from_object(evas,inst->network.icon,40,40);
    e_widget_frametable_object_append(flist, o, 0, 0, 1, 1, 1, 0, 1, 0);

    inst->network.btn_activate = e_widget_button_add(evas,D_("Activate"),NULL,if_network_dialog_cb_activate,inst,NULL);
    e_widget_frametable_object_append(flist, inst->network.btn_activate, 1, 0, 1, 1, 1, 0, 1, 0);

    inst->network.btn_deactivate = e_widget_button_add(evas,D_("Deactivate"),NULL,if_network_dialog_cb_deactivate,inst,NULL);
    e_widget_frametable_object_append(flist, inst->network.btn_deactivate, 2, 0, 1, 1, 1, 0, 1, 0);

    rg = e_widget_radio_group_new(&(inst->network.dhcp));
    o = e_widget_radio_add(evas, D_("DHCP (Dynamic host configuration protocol)"), 0, rg);
    evas_object_smart_callback_add(o, "changed", if_network_dialog_cb_dhcp, inst);
    e_widget_frametable_object_append(flist, o, 0, 1, 3, 1, 1, 0, 1, 0);
    inst->network.radio_dhcp = o;

    o = e_widget_radio_add(evas, D_("Static"), 1, rg);
    evas_object_smart_callback_add(o, "changed", if_network_dialog_cb_dhcp, inst);
    e_widget_frametable_object_append(flist, o, 0, 2, 3, 1, 1, 0, 1, 0);
    inst->network.radio_static = o;

    lbl = e_widget_label_add(evas,D_("IP address: "));
    e_widget_frametable_object_append(flist, lbl, 0, 3, 2, 1, 1, 0, 1, 0);
    inst->network.entry_ip = e_widget_entry_add(evas,&(inst->network.ip),if_network_dialog_cb_entry,inst,NULL);
    e_widget_frametable_object_append(flist, inst->network.entry_ip, 2, 3, 1, 1, 1, 0, 1, 0);

    lbl = e_widget_label_add(evas,D_("Netmask address: "));
    e_widget_frametable_object_append(flist, lbl, 0, 4, 2, 1, 1, 0, 1, 0);
    inst->network.entry_netmask = e_widget_entry_add(evas,&(inst->network.netmask),if_network_dialog_cb_entry,inst,NULL);
    e_widget_frametable_object_append(flist, inst->network.entry_netmask, 2, 4, 1, 1, 1, 0, 1, 0);

    lbl = e_widget_label_add(evas,D_("Gateway address: "));
    e_widget_frametable_object_append(flist, lbl, 0, 5, 2, 1, 1, 0, 1, 0);
    inst->network.entry_gateway = e_widget_entry_add(evas,&(inst->network.gateway),if_network_dialog_cb_entry,inst,NULL);
    e_widget_frametable_object_append(flist, inst->network.entry_gateway, 2, 5, 1, 1, 1, 0, 1, 0);

    lbl = e_widget_label_add(evas,D_("Command: "));
    e_widget_frametable_object_append(flist, lbl, 0, 6, 2, 1, 1, 0, 1, 0);
    inst->network.entry_cmd = e_widget_entry_add(evas,&(inst->network.cmd),if_network_dialog_cb_entry,inst,NULL);
    e_widget_frametable_object_append(flist, inst->network.entry_cmd, 2, 6, 1, 1, 1, 0, 1, 0);

    //an empty as separator
    lbl = e_widget_label_add(evas,"");
    e_widget_frametable_object_append(flist, lbl, 0, 7, 2, 1, 1, 0, 1, 0);

    lbl = e_widget_label_add(evas,D_("Login: "));
    e_widget_frametable_object_append(flist, lbl, 0, 8, 2, 1, 1, 0, 1, 0);
    inst->network.lbl_login = lbl;
    inst->network.entry_login = e_widget_entry_add(evas,&(inst->network.login),if_network_dialog_cb_entry,inst,NULL);
    e_widget_frametable_object_append(flist, inst->network.entry_login, 2, 8, 1, 1, 1, 0, 1, 0);

    lbl = e_widget_label_add(evas,D_("Password: "));
    e_widget_frametable_object_append(flist, lbl, 0, 9, 2, 1, 1, 0, 1, 0);
    inst->network.lbl_pwd = lbl;
    inst->network.entry_pwd = e_widget_entry_add(evas,&(inst->network.pwd),if_network_dialog_cb_entry,inst,NULL);
    e_widget_frametable_object_append(flist, inst->network.entry_pwd, 2, 9, 1, 1, 1, 0, 1, 0);

    e_widget_table_object_append(inst->network.table,flist,1,0,1,2,1,1,1,0);

    e_dialog_button_add(inst->network.dialog, D_("OK"), NULL,if_network_dialog_cb_ok, inst);
    e_dialog_button_add(inst->network.dialog, D_("Apply"), NULL, if_network_dialog_cb_apply, inst);
    e_dialog_button_add(inst->network.dialog, D_("Cancel"), NULL, if_network_dialog_cb_cancel, inst);

    e_win_delete_callback_set(inst->network.dialog->win, if_network_dialog_cb_del);

    e_dialog_button_focus_num(inst->network.dialog, 1);
    e_win_centered_set(inst->network.dialog->win, 1);
}



void if_network_dialog_show(Instance* inst)
{
    if(inst->network.dialog)
        if_network_dialog_hide(inst);
    if_network_dialog_create(inst);
    e_dialog_show(inst->network.dialog);
}

void if_network_dialog_set(Instance *inst, Popup_Elt* network)
{
    char buf[1024];

    if(inst->network.network)
    {
        inst->network.network->nb_use--;
        if(inst->network.network->nb_use<=0)
            popup_elt_free(inst->network.network);
    }

    inst->network.network = network;
    network->nb_use++;

    e_widget_label_text_set(inst->network.lbl_essid,exalt_dbus_wireless_network_essid_get(network->n));
    e_widget_label_text_set(inst->network.lbl_address,exalt_dbus_wireless_network_address_get(network->n));

    snprintf(buf,1024,"%d %%",exalt_dbus_wireless_network_quality_get(network->n));
    e_widget_label_text_set(inst->network.lbl_quality,buf);

    const char* auth = exalt_wireless_network_name_from_mode(exalt_dbus_wireless_network_mode_get(network->n));
    e_widget_label_text_set(inst->network.lbl_auth,auth);

    //by default the login and password entry are disabled
    evas_object_hide(inst->network.lbl_login);
    evas_object_hide(inst->network.entry_login);
    evas_object_hide(inst->network.lbl_pwd);
    evas_object_hide(inst->network.entry_pwd);

    if(exalt_dbus_wireless_network_encryption_is(network->n))
    {
        Eina_List* l_ie;

        evas_object_show(inst->network.lbl_pwd);
        evas_object_show(inst->network.entry_pwd);

        if( (l_ie=exalt_dbus_wireless_network_ie_get(network->n))==NULL)
        {
            //WEP encryption
            Evas_Object* fr = if_network_dialog_wep_new(inst,network->n);
            e_widget_table_object_append(inst->network.table,fr,0,1,1,1,1,0,1,0);
        }
        else
        {
            Eina_List* l;
            Exalt_Wireless_Network_IE *ie;
            int i = 1;
            EINA_LIST_FOREACH(l_ie,l,ie)
            {
                Evas_Object *fr = NULL;
                switch(exalt_wireless_network_ie_wpa_type_get(ie))
                {
                    case WPA_TYPE_WPA:
                        fr = if_network_dialog_wpa_new(inst,ie,D_("WPA information"));
                        break;
                    case WPA_TYPE_WPA2:
                        fr = if_network_dialog_wpa_new(inst,ie,D_("WPA2 information"));
                        break;
                    default:
                        printf("(%s) unknow WPA information :(\n",exalt_dbus_wireless_network_essid_get(network->n));
                        break;
                }

                if(fr)
                {
                    e_widget_table_object_append(inst->network.table,fr,0,i,1,1,1,0,1,0);
                    i++;
                }
            }
        }
    }

    int mw,mh;
    e_widget_min_size_get(inst->network.table, &mw, &mh);
    e_dialog_content_set(inst->network.dialog, inst->network.table, mw, mh);


    exalt_dbus_eth_ip_get(inst->conn,network->iface);
    exalt_dbus_eth_gateway_get(inst->conn,network->iface);
    exalt_dbus_eth_command_get(inst->conn,network->iface);
    exalt_dbus_eth_dhcp_is(inst->conn,network->iface);
    exalt_dbus_eth_up_is(inst->conn,network->iface);
    exalt_dbus_eth_link_is(inst->conn,network->iface);

    e_dialog_show(inst->network.dialog);
}

Evas_Object* if_network_dialog_wep_new(Instance* inst,Exalt_DBus_Wireless_Network* n)
{
    Evas_Object* lbl;

    Evas*evas = e_win_evas_get(inst->network.dialog->win);

    Evas_Object* flist = e_widget_frametable_add(evas, D_("WEP information"), 0);

    lbl = e_widget_label_add(evas,D_("Authentication: "));
    e_widget_frametable_object_append(flist, lbl, 0, 0, 1, 1, 1, 0, 1, 0);
    const char* security = exalt_wireless_network_name_from_security(exalt_dbus_wireless_network_security_mode_get(n));
    lbl = e_widget_label_add(evas,security);
    e_widget_frametable_object_append(flist, lbl, 1, 0, 1, 1, 1, 0, 1, 0);

    return flist;
}

Evas_Object* if_network_dialog_wpa_new(Instance* inst,Exalt_Wireless_Network_IE* ie,const char* title)
{
    Evas_Object* lbl;
    const char* s;
    char buf[1024];
    int i;
    Evas*evas = e_win_evas_get(inst->network.dialog->win);

    Evas_Object* flist = e_widget_frametable_add(evas, title, 0);

#define INFO_ADD(title,value,pos) \
    lbl = e_widget_label_add(evas,title); \
    e_widget_frametable_object_append(flist, lbl, 0, pos, 1, 1, 1, 0, 1, 0); \
    lbl = e_widget_label_add(evas,value); \
    e_widget_frametable_object_append(flist, lbl, 1, pos, 1, 1, 1, 0, 1, 0)

    Exalt_Wireless_Network_Wpa_Type wpa_type =
        exalt_wireless_network_ie_wpa_type_get(ie);
    s=exalt_wireless_network_name_from_wpa_type(wpa_type);
    INFO_ADD(D_("Type: "),s,0);

    i=exalt_wireless_network_ie_wpa_version_get(ie);
    snprintf(buf,1024,"%d",i);
    INFO_ADD(D_("Version: "),buf,1);

    Exalt_Wireless_Network_Cypher_Name name = exalt_wireless_network_ie_group_cypher_get(ie);
    s=exalt_wireless_network_name_from_cypher_name(name);
    INFO_ADD(D_("Group Cypher: "),s,2);

    buf[0]='\0';
    for(i=0;i<exalt_wireless_network_ie_pairwise_cypher_number_get(ie);i++)
    {
        if(i!=0)
            strcat(buf,", ");
        Exalt_Wireless_Network_Cypher_Name name;
        name = exalt_wireless_network_ie_pairwise_cypher_get(ie,i);
        strcat(buf,exalt_wireless_network_name_from_cypher_name(name));
    }
    INFO_ADD(D_("Pairwise Cyphers: "),buf,3);

    buf[0]='\0';
    int use_8021X = 0;
    for(i=0;i<exalt_wireless_network_ie_auth_suites_number_get(ie);i++)
    {
        if(i!=0)
            strcat(buf,", ");
        Exalt_Wireless_Network_Auth_Suites name;
        name = exalt_wireless_network_ie_auth_suites_get(ie,i);
        strcat(buf,exalt_wireless_network_name_from_auth_suites(name));
        if(name == AUTH_SUITES_8021X)
            use_8021X=1;
    }
    INFO_ADD(D_("Authentifications Suites: "),buf,4);

    if(use_8021X)
    {
        evas_object_show(inst->network.lbl_login);
        evas_object_show(inst->network.entry_login);
    }


    i = exalt_wireless_network_ie_preauth_supported_is(ie);
    if(i) s=D_("Yes");
    else s= D_("No");
    INFO_ADD(D_("Pre-authentification: "),s,5);

#undef INFO_ADD
    return flist;
}

void if_network_dialog_hide(Instance *inst)
{
    if(inst->network.dialog)
    {
        e_object_del(E_OBJECT(inst->network.dialog));
        inst->network.dialog=NULL;
        if(inst->network.network)
        {
            inst->network.network->nb_use--;
            if(inst->network.network->nb_use<=0)
            {
                popup_elt_free(inst->network.network);
            }
            inst->network.network = NULL;
        }
    }
}

void if_network_dialog_cb_del(E_Win *win)
{
    E_Dialog *dialog;
    Instance *inst;

    dialog = win->data;
    inst = dialog->data;
    if_network_dialog_hide(inst);
}


void if_network_dialog_update(Instance* inst,Exalt_DBus_Response *response)
{
    char* string;
    int boolean;
    if(!inst->network.dialog)
        return ;

    string = exalt_dbus_response_iface_get(response);
    if(!inst->network.network->iface || !string && !strcmp(inst->network.network->iface,string)==0)
        return;

    switch(exalt_dbus_response_type_get(response))
    {
        case EXALT_DBUS_RESPONSE_IFACE_IP_GET:
            string = exalt_dbus_response_address_get(response);
            e_widget_entry_text_set(inst->network.entry_ip,string);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_NETMASK_GET:
            string = exalt_dbus_response_address_get(response);
            e_widget_entry_text_set(inst->network.entry_netmask,string);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_GATEWAY_GET:
            string = exalt_dbus_response_address_get(response);
            e_widget_entry_text_set(inst->network.entry_gateway,string);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_CMD_GET:
            string = exalt_dbus_response_string_get(response);
            e_widget_entry_text_set(inst->network.entry_cmd,string);
            break;

        case EXALT_DBUS_RESPONSE_IFACE_DHCP_IS:
            boolean = exalt_dbus_response_is_get(response);
            if(boolean)
                e_widget_radio_toggle_set(inst->network.radio_dhcp,1);
            else
                e_widget_radio_toggle_set(inst->network.radio_static,1);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_UP_IS:
            boolean = exalt_dbus_response_is_get(response);
            e_widget_disabled_set(inst->network.btn_activate,boolean);
            e_widget_disabled_set(inst->network.btn_deactivate,!boolean);
            inst->network.network->is_up = boolean;
            if_network_dialog_icon_update(inst);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_LINK_IS:
            boolean = exalt_dbus_response_is_get(response);
            inst->network.network->is_link = boolean;
            if_network_dialog_icon_update(inst);
            break;

        default: break;
    }
    if_network_disabled_update(inst);
}


void if_network_dialog_icon_update(Instance *inst)
{
    char buf[1024];
    char* group;

    if(!inst->network.dialog)
        return ;

    if(!inst->network.network->is_link)
        edje_object_signal_emit(inst->network.icon,"notLink","exalt");
    else if(!inst->network.network->is_up)
        edje_object_signal_emit(inst->network.icon,"notActivate","exalt");
    else
        edje_object_signal_emit(inst->network.icon,"default","exalt");
}



void if_network_dialog_cb_activate(void *data, void*data2)
{
    Instance *inst = data;
    exalt_dbus_eth_up(inst->conn, inst->network.network->iface);
}

void if_network_dialog_cb_deactivate(void *data, void*data2)
{
    Instance *inst = data;
    exalt_dbus_eth_down(inst->conn, inst->network.network->iface);
}


void if_network_disabled_update(Instance *inst)
{
    int disabled;
    int is_ip;

    if(!inst->network.dialog)
        return;

    e_widget_disabled_set(inst->network.entry_cmd,0);
    disabled = 0;
    if(!e_widget_disabled_get(inst->network.btn_activate))
    {
        e_widget_disabled_set(inst->network.radio_dhcp,1);
        e_widget_disabled_set(inst->network.radio_static,1);

        e_widget_disabled_set(inst->network.entry_ip,1);
        e_widget_disabled_set(inst->network.entry_netmask,1);
        e_widget_disabled_set(inst->network.entry_gateway,1);

        e_dialog_button_disable_num_set(inst->network.dialog,0,1);
        e_dialog_button_disable_num_set(inst->network.dialog,1,1);

        return ;
    }

    e_widget_disabled_set(inst->network.radio_dhcp,0);
    e_widget_disabled_set(inst->network.radio_static,0);

    if(inst->network.dhcp == 0)
        disabled = 1;
    else
        disabled = 0;

    e_widget_disabled_set(inst->network.entry_ip,disabled);
    e_widget_disabled_set(inst->network.entry_netmask,disabled);
    e_widget_disabled_set(inst->network.entry_gateway,disabled);

    e_dialog_button_disable_num_set(inst->network.dialog,0,0);
    e_dialog_button_disable_num_set(inst->network.dialog,1,0);
    if(inst->network.dhcp != 0)
    {
        const char* string;
        string = e_widget_entry_text_get(inst->network.entry_ip);
        is_ip = exalt_is_address(string);

        if(is_ip)
        {
            string = e_widget_entry_text_get(inst->network.entry_netmask);
            is_ip = exalt_is_address(string);
        }

        if(is_ip)
        {
            string = e_widget_entry_text_get(inst->network.entry_gateway);
            is_ip = exalt_is_address(string);
        }
        e_dialog_button_disable_num_set(inst->network.dialog,0,!is_ip);
        e_dialog_button_disable_num_set(inst->network.dialog,1,!is_ip);
    }
}



void if_network_dialog_cb_dhcp(void *data, Evas_Object *obj, void *event_info)
{
   Instance *inst;

   inst = data;

   if_network_disabled_update(inst);
}

void if_network_dialog_cb_entry(void *data, void* data2)
{
    Instance* inst = data;
    if_network_disabled_update(inst);
}



void if_network_dialog_cb_cancel(void *data, E_Dialog *dialog)
{
    Instance* inst = data;
    if_network_dialog_hide(inst);
}

void if_network_dialog_cb_ok(void *data, E_Dialog *dialog)
{
    Instance* inst = data;
    if_network_dialog_cb_apply(data,dialog);
    if_network_dialog_hide(inst);
}

void if_network_dialog_cb_apply(void *data, E_Dialog *dialog)
{
    Instance* inst = data;

    /*Exalt_Connection* conn = exalt_conn_new();

    exalt_conn_wireless_set(conn,0);
    if(inst->wired.dhcp != 0)
    {
        exalt_conn_mode_set(conn,EXALT_STATIC);
        exalt_conn_ip_set(conn,e_widget_entry_text_get(inst->wired.entry_ip));
        exalt_conn_netmask_set(conn,e_widget_entry_text_get(inst->wired.entry_netmask));
        exalt_conn_gateway_set(conn,e_widget_entry_text_get(inst->wired.entry_gateway));
    }
    else
        exalt_conn_mode_set(conn,EXALT_DHCP);
    exalt_conn_cmd_after_apply_set(conn,e_widget_entry_text_get(inst->wired.entry_cmd));

    exalt_dbus_eth_conn_apply(inst->conn,inst->wired.iface->iface,conn);
    exalt_conn_free(&conn);
    */
    printf("APPLY\n");
}


