/* Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
  * Description: A dialog which allows to create a new wireless network
  */

#include "e_mod_main.h"

void if_network_dialog_new_init(Instance* inst)
{
    inst->network_new.iface = NULL;
    inst->network_new.dialog = NULL;
}

void if_network_dialog_new_create(Instance* inst)
{
    Evas* evas;
    Evas_Object *list, *flist, *o, *lbl;
    E_Radio_Group* rg;
    int mh,mw;
    char buf[4096];

    inst->network_new.dialog = e_dialog_new(inst->gcc->gadcon->zone->container, "e", "exalt_network_dialog_new");
    e_dialog_title_set(inst->network_new.dialog, D_("Create a new network"));
    inst->network_new.dialog->data = inst;

    evas = e_win_evas_get(inst->network_new.dialog->win);
    list = e_widget_list_add(evas, 0, 1);

    flist = e_widget_frametable_add(evas, D_("Network"), 0);
    inst->network_new.flist_enc = flist;

    //
    rg = e_widget_radio_group_new(&(inst->network_new.enc));
    o = e_widget_radio_add(evas, D_("No encryption (danger)"), EXALT_ENCRYPTION_NONE, rg);
    evas_object_smart_callback_add(o, "changed", if_network_dialog_new_disabled_update, inst);
    e_widget_frametable_object_append(flist, o, 0, 0, 3, 1, 1, 0, 1, 0);
    inst->network_new.radio_noenc = o;

    o = e_widget_radio_add(evas, D_("WEP plain text"), EXALT_ENCRYPTION_WEP_ASCII, rg);
    evas_object_smart_callback_add(o, "changed", if_network_dialog_new_disabled_update, inst);
    e_widget_frametable_object_append(flist, o, 0, 1, 3, 1, 1, 0, 1, 0);
    inst->network_new.radio_wep_ascii = o;

    o = e_widget_radio_add(evas, D_("WEP hexadecimal"), EXALT_ENCRYPTION_WEP_HEXA, rg);
    evas_object_smart_callback_add(o, "changed", if_network_dialog_new_disabled_update, inst);
    e_widget_frametable_object_append(flist, o, 0, 2, 3, 1, 1, 0, 1, 0);
    inst->network_new.radio_wep_hexa = o;

    /*
     * WPA IBSS (ad-hoc) removed because it seems not working very well with wpa_supplicant
     * In my case (Watchwolf) the driver freeze the system
     * o = e_widget_radio_add(evas, D_("WPA personnal"), EXALT_ENCRYPTION_WPA_PSK_TKIP_ASCII, rg);
    evas_object_smart_callback_add(o, "changed", if_network_dialog_new_disabled_update, inst);
    e_widget_frametable_object_append(flist, o, 0, 3, 3, 1, 1, 0, 1, 0);
    inst->network_new.radio_wpa_personnal = o;
    */

    //

    //
    lbl = e_widget_label_add(evas,D_("Name: "));
    e_widget_frametable_object_append(flist, lbl, 0, 5, 1, 1, 1, 0, 1, 0);
    inst->network_new.entry_essid = e_widget_entry_add(evas,&(inst->network_new.essid),if_network_dialog_new_cb_entry,inst,NULL);
    e_widget_frametable_object_append(flist, inst->network_new.entry_essid, 1, 5, 2, 1, 1, 0, 1, 0);

    lbl = e_widget_label_add(evas,D_("Password: "));
    inst->network_new.lbl_pwd = lbl;
    e_widget_frametable_object_append(flist, lbl, 0, 6, 1, 1, 1, 0, 1, 0);
    inst->network_new.entry_pwd = e_widget_entry_add(evas,&(inst->network_new.pwd),if_network_dialog_new_cb_entry,inst,NULL);
    e_widget_frametable_object_append(flist, inst->network_new.entry_pwd, 1, 6, 2, 1, 1, 0, 1, 0);

    lbl = e_widget_label_add(evas,D_("Login: "));
    inst->network_new.lbl_login = lbl;
    e_widget_frametable_object_append(flist, lbl, 0, 7, 1, 1, 1, 0, 1, 0);
    inst->network_new.entry_login = e_widget_entry_add(evas,&(inst->network_new.login),if_network_dialog_new_cb_entry,inst,NULL);
    e_widget_frametable_object_append(flist, inst->network_new.entry_login, 1, 7, 2, 1, 1, 0, 1, 0);
    //
    e_widget_list_object_append(list, flist, 1, 0, 0.5);

    flist = e_widget_frametable_add(evas, D_("Interface"), 0);
    inst->network_new.flist_iface = flist;

    //
    lbl = e_widget_label_add(evas,"                                                     ");
    e_widget_frametable_object_append(flist, lbl, 0, 0, 3, 1, 1, 0, 1, 0);

    lbl = e_widget_label_add(evas,D_("IP address: "));
    e_widget_frametable_object_append(flist, lbl, 0, 1, 1, 1, 1, 0, 1, 0);
    inst->network_new.entry_ip = e_widget_entry_add(evas,&(inst->network_new.ip),if_network_dialog_new_cb_entry,inst,NULL);
    e_widget_frametable_object_append(flist, inst->network_new.entry_ip, 1, 1, 2, 1, 1, 0, 1, 0);

    lbl = e_widget_label_add(evas,D_("Netmask: "));
    e_widget_frametable_object_append(flist, lbl, 0, 2, 1, 1, 1, 0, 1, 0);
    inst->network_new.entry_netmask = e_widget_entry_add(evas,&(inst->network_new.netmask),if_network_dialog_new_cb_entry,inst,NULL);
    e_widget_frametable_object_append(flist, inst->network_new.entry_netmask, 1, 2, 2, 1, 1, 0, 1, 0);

    lbl = e_widget_label_add(evas,D_("Gateway: "));
    e_widget_frametable_object_append(flist, lbl, 0, 3, 1, 1, 1, 0, 1, 0);
    inst->network_new.entry_gateway = e_widget_entry_add(evas,&(inst->network_new.gateway),if_network_dialog_new_cb_entry,inst,NULL);
    e_widget_frametable_object_append(flist, inst->network_new.entry_gateway, 1, 3, 2, 1, 1, 0, 1, 0);

    lbl = e_widget_label_add(evas,D_("Command: "));
    inst->network_new.lbl_cmd = lbl;
    e_widget_frametable_object_append(flist, lbl, 0, 4, 1, 1, 1, 0, 1, 0);
    inst->network_new.entry_cmd = e_widget_entry_add(evas,&(inst->network_new.cmd),if_network_dialog_new_cb_entry,inst,NULL);
    e_widget_frametable_object_append(flist, inst->network_new.entry_cmd, 1, 4, 2, 1, 1, 0, 1, 0);

    //
    e_widget_list_object_append(list, flist, 1, 0, 0.5);

    e_widget_min_size_get(list, &mw, &mh);
    e_dialog_content_set(inst->network_new.dialog, list, mw, mh);

    e_win_delete_callback_set(inst->network_new.dialog->win, if_network_dialog_new_cb_del);

    e_dialog_button_add(inst->network_new.dialog, D_("OK"), NULL, if_network_dialog_new_cb_ok, inst);
    e_dialog_button_add(inst->network_new.dialog, D_("Apply"), NULL, if_network_dialog_new_cb_apply, inst);
    e_dialog_button_add(inst->network_new.dialog, D_("Cancel"), NULL, if_network_dialog_new_cb_cancel, inst);

    e_dialog_button_focus_num(inst->network_new.dialog, 1);
    e_win_centered_set(inst->network_new.dialog->win, 1);
}

void if_network_dialog_new_show(Instance* inst)
{
    if(!inst->network_new.dialog)
        if_network_dialog_new_create(inst);
    e_dialog_show(inst->network_new.dialog);
}

void if_network_dialog_new_set(Instance *inst, Popup_Elt* iface)
{
    if(inst->network_new.iface)
    {
        inst->network_new.iface->nb_use--;
        if(inst->network_new.iface->nb_use<=0)
            popup_elt_free(inst->network_new.iface);
    }

    inst->network_new.iface = iface;
    iface->nb_use++;

    e_widget_radio_toggle_set(inst->network_new.radio_wep_ascii,1);
    e_widget_entry_text_set(inst->network_new.entry_essid,"");
    e_widget_entry_text_set(inst->network_new.entry_pwd,"");
    e_widget_entry_text_set(inst->network_new.entry_login,"");
    e_widget_entry_text_set(inst->network_new.entry_ip,"192.168.0.1");
    e_widget_entry_text_set(inst->network_new.entry_netmask,"255.255.255.0");
    e_widget_entry_text_set(inst->network_new.entry_gateway,"192.168.0.254");
    e_widget_entry_text_set(inst->network_new.entry_cmd,"");

    if_network_dialog_new_disabled_update(inst,NULL,NULL);

    if(exalt_conf->mode)
    {
        evas_object_show(inst->network_new.lbl_cmd);
        evas_object_show(inst->network_new.entry_cmd);
    }
    else
    {
        evas_object_hide(inst->network_new.lbl_cmd);
        evas_object_hide(inst->network_new.entry_cmd);
    }

    exalt_dbus_eth_command_get(inst->conn, iface->iface);
}

void if_network_dialog_new_hide(Instance *inst)
{
    if(inst->network_new.dialog)
    {
        e_object_del(E_OBJECT(inst->network_new.dialog));
        inst->network_new.dialog=NULL;
        if(inst->network_new.iface)
        {
            inst->network_new.iface->nb_use--;
            if(inst->network_new.iface->nb_use<=0)
            {
                popup_elt_free(inst->network_new.iface);
            }
            inst->network_new.iface = NULL;
        }
    }
}

void if_network_dialog_new_update(Instance* inst,Exalt_DBus_Response *response)
{
    char* string;
    int boolean;
    if(!inst->network_new.dialog)
        return ;

    string = exalt_dbus_response_iface_get(response);
    if(!inst->network_new.iface->iface || !string || !strcmp(inst->network_new.iface->iface,string)==0)
        return;

    switch(exalt_dbus_response_type_get(response))
    {
        case EXALT_DBUS_RESPONSE_IFACE_CMD_GET:
            e_widget_entry_text_set(inst->network_new.entry_cmd,
                    exalt_dbus_response_string_get(response));
            break;
        default : ;
    }
}

void if_network_dialog_new_cb_ok(void *data, E_Dialog *dialog)
{
    Instance* inst = data;
    if_network_dialog_new_cb_apply(data,dialog);
    if_network_dialog_new_cb_cancel(data,dialog);
}

void if_network_dialog_new_cb_apply(void *data, E_Dialog *dialog)
{
    Instance* inst = data;
    Exalt_Wireless_Network_IE *ie;
    Eina_List *l;

    Exalt_Connection* conn = exalt_conn_new();

    exalt_conn_wireless_set(conn,1);
    exalt_conn_mode_set(conn,EXALT_STATIC);
    exalt_conn_ip_set(conn, inst->network_new.ip);
    exalt_conn_netmask_set(conn, inst->network_new.netmask);
    exalt_conn_gateway_set(conn, inst->network_new.gateway);
    exalt_conn_cmd_after_apply_set(conn, inst->network_new.cmd);

    //wireless part
    Exalt_Wireless_Network *n = exalt_wireless_network_new(NULL);
    exalt_conn_network_set(conn,n);

    exalt_conn_wep_key_hexa_set(conn, 0);
    exalt_wireless_network_essid_set(n, inst->network_new.essid);
    exalt_wireless_network_mode_set(n, MODE_IBSS);
    switch(inst->network_new.enc)
    {
        case EXALT_ENCRYPTION_NONE:

            break;
        case EXALT_ENCRYPTION_WEP_HEXA:
            exalt_conn_wep_key_hexa_set(conn, 1);
        case EXALT_ENCRYPTION_WEP_ASCII:
            exalt_wireless_network_encryption_set(n, 1);
            exalt_conn_key_set(conn, inst->network_new.pwd);
            break;
        case EXALT_ENCRYPTION_WPA_PSK_TKIP_ASCII:
            exalt_wireless_network_encryption_set(n, 1);
            exalt_conn_key_set(conn, inst->network_new.pwd);

            exalt_wireless_network_ie_choice_set(n,0);
            ie = exalt_wireless_network_ie_new();
            exalt_wireless_network_ie_auth_choice_set(ie,0);
            exalt_wireless_network_ie_pairwise_choice_set(ie,0);

            exalt_wireless_network_ie_wpa_type_set(ie, WPA_TYPE_WPA);
            exalt_wireless_network_ie_group_cypher_set(ie, CYPHER_NAME_TKIP);
            exalt_wireless_network_ie_pairwise_cypher_set(ie, CYPHER_NAME_TKIP, 0);
            exalt_wireless_network_ie_pairwise_cypher_number_set(ie, 1);
            exalt_wireless_network_ie_auth_suites_set(ie, AUTH_SUITES_PSK, 0);
            exalt_wireless_network_ie_auth_suites_number_set(ie, 1);

            l = exalt_wireless_network_ie_get(n);
            l = eina_list_append(l, ie);
            exalt_wireless_network_ie_set(n, l);
            break;
        default: ;
    }

    exalt_dbus_eth_conn_apply(inst->conn,inst->network_new.iface->iface,conn);
    exalt_conn_free(&conn);
}

void if_network_dialog_new_cb_cancel(void *data, E_Dialog *dialog)
{
    Instance* inst = data;
    if_network_dialog_new_hide(inst);
}

void if_network_dialog_new_cb_del(E_Win *win)
{
    E_Dialog *dialog;
    Instance *inst;

    dialog = win->data;
    inst = dialog->data;
    if_network_dialog_new_hide(inst);
}

void if_network_dialog_new_disabled_update(void *data, Evas_Object *obj, void *event_info)
{
    Instance *inst;
    inst = data;
    short is_pwd = 0;
    short is_login = 0;

    evas_object_hide(inst->network_new.lbl_pwd);
    evas_object_hide(inst->network_new.entry_pwd);
    evas_object_hide(inst->network_new.lbl_login);
    evas_object_hide(inst->network_new.entry_login);

    switch(inst->network_new.enc)
    {
        case EXALT_ENCRYPTION_WEP_ASCII:
        case EXALT_ENCRYPTION_WEP_HEXA:
        case EXALT_ENCRYPTION_WPA_PSK_TKIP_ASCII:
            evas_object_show(inst->network_new.lbl_pwd);
            evas_object_show(inst->network_new.entry_pwd);
            is_pwd = 1;
            break;
        default: ;
    }

    e_dialog_button_disable_num_set(inst->network_new.dialog,0,1);
    e_dialog_button_disable_num_set(inst->network_new.dialog,1,1);
    if(exalt_address_is(inst->network_new.ip)
            && exalt_address_is(inst->network_new.netmask)
            && (!is_pwd || strlen(inst->network_new.pwd)>0)
            && (!is_login || strlen(inst->network_new.login)>0)
            && strlen(inst->network_new.essid)>0)
    {
        e_dialog_button_disable_num_set(inst->network_new.dialog,0,0);
        e_dialog_button_disable_num_set(inst->network_new.dialog,1,0);
    }
}

void if_network_dialog_new_cb_entry(void *data, void* data2)
{
    Instance* inst = data;
    if_network_dialog_new_disabled_update(inst,NULL,NULL);
}

