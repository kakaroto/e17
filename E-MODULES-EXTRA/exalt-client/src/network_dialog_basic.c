 /* Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
  * Description: The network dialog used in the basic mode
  *              (Only allows to conect/disconnect to a wireless network
  *              and launch a DHCP)
  */

#include "e_mod_main.h"

void if_network_dialog_basic_init(Instance* inst)
{
    inst->network_basic.network = NULL;
    inst->network_basic.dialog = NULL;
}

void if_network_dialog_basic_create(Instance* inst)
{
    Evas* evas;
    Evas_Object *list, *flist, *o, *lbl;
    E_Radio_Group* rg;
    int mh,mw;
    char buf[4096];

    inst->network_basic.dialog = e_dialog_new(inst->gcc->gadcon->zone->container, "e", "exalt_network_dialog_basic");

    e_dialog_title_set(inst->network_basic.dialog, D_("Wireless Configuration Settings"));
    inst->network_basic.dialog->data = inst;

    evas = e_win_evas_get(inst->network_basic.dialog->win);
    list = e_widget_list_add(evas, 0, 0);
    inst->network_basic.list = list;

    flist = e_widget_frametable_add(evas, D_("Wireless interface"), 0);
    inst->network_basic.flist = flist;

    inst->network_basic.icon = edje_object_add(evas);
    snprintf(buf,1024,"%s/e-module-exalt.edj",exalt_conf->module->dir);
    edje_object_file_set(inst->network_basic.icon, buf,"modules/exalt/icons/wireless");
    evas_object_show(inst->network_basic.icon);
    o = e_widget_image_add_from_object(evas,inst->network_basic.icon,40,40);
    e_widget_frametable_object_append(flist, o, 0, 0, 1, 1, 1, 0, 1, 0);

    inst->network_basic.btn = e_widget_button_add(evas,D_("Disconnect the interface"),NULL,if_network_dialog_basic_cb_connect,inst,NULL);
    e_widget_frametable_object_append(flist, inst->network_basic.btn, 1, 0, 1, 1, 1, 0, 1, 0);


    e_widget_list_object_append(list, flist, 1, 0, 0.5);

    e_widget_size_min_get(list, &mw, &mh);
    e_dialog_content_set(inst->network_basic.dialog, list, mw, mh);

    e_win_delete_callback_set(inst->network_basic.dialog->win, if_network_dialog_basic_cb_del);

    e_dialog_button_add(inst->network_basic.dialog, D_("Close"), NULL, if_network_dialog_basic_cb_close, inst);
    e_dialog_button_focus_num(inst->network_basic.dialog, 1);
    e_win_centered_set(inst->network_basic.dialog->win, 1);
}

void if_network_dialog_basic_show(Instance* inst)
{
    if(!inst->network_basic.dialog)
        if_network_dialog_basic_create(inst);
    e_dialog_show(inst->network_basic.dialog);
}

Evas_Object* if_network_dialog_basic_wep_new(Instance* inst,Exalt_Wireless_Network* n)
{
    Evas_Object* lbl, *o;

    Evas *evas = e_win_evas_get(inst->network_basic.dialog->win);

    Evas_Object* flist = e_widget_frametable_add(evas, D_("WEP information"), 0);

    int i = 0;
    inst->network_basic.entry_ca_cert = NULL;
    inst->network_basic.entry_client_cert = NULL;
    inst->network_basic.entry_private_key = NULL;

    inst->network_basic.wep_key_hexa = 1;
    E_Radio_Group* rg = e_widget_radio_group_new(&(inst->network_basic.wep_key_hexa));
    o = e_widget_radio_add(evas, D_("Hexadecimal Key"), 1, rg);
    e_widget_frametable_object_append(flist, o, 0, i, 2, 1, 1, 0, 1, 0);

    o = e_widget_radio_add(evas, D_("ASCII Key"), 0, rg);
    e_widget_frametable_object_append(flist, o, 2, i, 2, 1, 1, 0, 1, 0);


    i++;

    lbl = e_widget_label_add(evas,D_("Password: "));
    e_widget_frametable_object_append(flist, lbl, 0, i, 1, 1, 1, 0, 0, 0);
    inst->network_basic.lbl_pwd = lbl;
    inst->network_basic.entry_pwd = e_widget_entry_add(evas,&(inst->network_basic.pwd),NULL,NULL,NULL);
    e_widget_frametable_object_append(flist, inst->network_basic.entry_pwd, 2, i, 1, 1, 1, 0, 1, 0);

    return flist;
}

Evas_Object* if_network_dialog_basic_wpa_new(Instance* inst,Exalt_Wireless_Network* n)
{
    Evas_Object* lbl;
    const char* s;
    char buf[1024];
    E_Radio_Group* rg;
    Evas_Object* radio;
    int i;
    Eina_List *l_ie,*l;
    Exalt_Wireless_Network_IE *ie;


    Evas*evas = e_win_evas_get(inst->network_basic.dialog->win);

    Evas_Object* flist = e_widget_frametable_add(evas, D_("WPA information"), 0);


    l_ie=exalt_wireless_network_ie_get(n);
    int ie_number = 0;
    int radio_number = 0;
    int is_eap =  0;
    ie = eina_list_data_get(l_ie);
    if(ie)
    {
        if(exalt_wireless_network_ie_auth_suites_get(ie,0)
                == AUTH_SUITES_EAP)
            is_eap = 1;
    }

    i = 0;
    inst->network_basic.entry_ca_cert = NULL;
    inst->network_basic.entry_client_cert = NULL;
    inst->network_basic.entry_private_key = NULL;


    lbl = e_widget_label_add(evas,D_("Password: "));
    e_widget_frametable_object_append(flist, lbl, 0, i, 1, 1, 1, 0, 0, 0);
    inst->network_basic.lbl_pwd = lbl;
    inst->network_basic.entry_pwd = e_widget_entry_add(evas,&(inst->network_basic.pwd),NULL,NULL,NULL);
    e_widget_frametable_object_append(flist, inst->network_basic.entry_pwd, 2, i, 1, 1, 1, 0, 1, 0);
    i++;

    if(is_eap)
    {
        Evas_Object *bt;

        lbl = e_widget_label_add(evas,D_("CA certificate: "));
        e_widget_frametable_object_append(flist, lbl, 0, i, 2, 1, 1, 0, 1, 0);
        inst->network_basic.lbl_ca_cert = lbl;
        inst->network_basic.entry_ca_cert = e_widget_entry_add(evas,&(inst->network_basic.ca_cert),NULL,inst,NULL);
        e_widget_frametable_object_append(flist, inst->network_basic.entry_ca_cert, 2, i, 1, 1, 1, 0, 1, 0);
        e_widget_entry_readonly_set(inst->network_basic.entry_ca_cert, 1);

        bt = e_widget_button_add(evas, "...", NULL, if_network_basic_dialog_cb_ca_cert, inst, inst);
        e_widget_frametable_object_append(flist, bt, 3, i, 1, 1, 1, 0, 1, 0);

        i++;

        lbl = e_widget_label_add(evas,D_("Client certificate: "));
        e_widget_frametable_object_append(flist, lbl, 0, i, 2, 1, 1, 0, 1, 0);
        inst->network_basic.lbl_client_cert = lbl;
        inst->network_basic.entry_client_cert = e_widget_entry_add(evas,&(inst->network_basic.client_cert),NULL,inst,NULL);
        e_widget_disabled_set(inst->network_basic.entry_client_cert, 1);
        e_widget_frametable_object_append(flist, inst->network_basic.entry_client_cert, 2, i, 1, 1, 1, 0, 1, 0);
        e_widget_entry_readonly_set(inst->network_basic.entry_client_cert, 1);

        bt = e_widget_button_add(evas, "...", NULL, if_network_basic_dialog_cb_client_cert, inst, inst);
        e_widget_frametable_object_append(flist, bt, 3, i, 1, 1, 1, 0, 1, 0);

        i++;

        lbl = e_widget_label_add(evas,D_("Private key: "));
        e_widget_frametable_object_append(flist, lbl, 0, i, 2, 1, 1, 0, 1, 0);
        inst->network_basic.lbl_private_key = lbl;
        inst->network_basic.entry_private_key = e_widget_entry_add(evas,&(inst->network_basic.private_key),NULL,inst,NULL);
        e_widget_disabled_set(inst->network_basic.entry_private_key, 1);
        e_widget_frametable_object_append(flist, inst->network_basic.entry_private_key, 2, i, 1, 1, 1, 0, 1, 0);
        e_widget_entry_readonly_set(inst->network_basic.entry_private_key, 1);

        bt = e_widget_button_add(evas, "...", NULL, if_network_basic_dialog_cb_private_key, inst, inst);
        e_widget_frametable_object_append(flist, bt, 3, i, 1, 1, 1, 0, 1, 0);
        i++;

        lbl = e_widget_label_add(evas,D_("                       "));
        e_widget_frametable_object_append(flist, lbl, 2, i, 1, 1, 1, 0, 1, 0);

        i++;
    }

    return flist;
}

void if_network_dialog_basic_set(Instance *inst, Popup_Elt* network)
{
    if(inst->network_basic.network)
    {
        inst->network_basic.network->nb_use--;
        if(inst->network_basic.network->nb_use<=0)
            popup_elt_free(inst->network_basic.network);
    }

    inst->network_basic.network = network;
    network->nb_use++;

    if(inst->network_basic.frame)
        evas_object_del(inst->network_basic.frame);

    if(exalt_wireless_network_encryption_is(network->n))
    {
        Eina_List* l_ie;

        if( !(l_ie = exalt_wireless_network_ie_get(network->n)))
        {
            //WEP encryption
            Evas_Object* fr = if_network_dialog_basic_wep_new(inst,network->n);
            inst->network_basic.frame = fr;
            e_widget_list_object_append(inst->network_basic.list, fr, 1, 0, 0.5);
        }
        else
        {
            Evas_Object* fr = if_network_dialog_basic_wpa_new(inst,network->n);
            inst->network_basic.frame = fr;
            if(fr)
            {
                e_widget_list_object_append(inst->network_basic.list, fr, 1, 0, 0.5);
            }
        }
    }

    int mw, mh;
    e_widget_size_min_get(inst->network_basic.list, &mw, &mh);
    e_dialog_content_set(inst->network_basic.dialog, inst->network_basic.list, mw, mh);
    e_dialog_show(inst->network_basic.dialog);

    exalt_dbus_eth_up_is(inst->conn,network->iface);
    exalt_dbus_eth_link_is(inst->conn,network->iface);
    exalt_dbus_wireless_essid_get(inst->conn,network->iface);
    exalt_dbus_eth_connected_is(inst->conn,network->iface);
    exalt_dbus_network_configuration_get(inst->conn, exalt_wireless_network_essid_get(network->n));
}

void if_network_dialog_basic_hide(Instance *inst)
{
    if(inst->network_basic.dialog)
    {
        e_object_del(E_OBJECT(inst->network_basic.dialog));
        inst->network_basic.dialog=NULL;
        if(inst->network_basic.network)
        {
            inst->network_basic.network->nb_use--;
            if(inst->network_basic.network->nb_use<=0)
            {
                popup_elt_free(inst->network_basic.network);
            }
            inst->network_basic.network = NULL;
            if(inst->network_basic.fsel_win)
            {
                e_object_del(E_OBJECT(inst->network_basic.fsel_win));
                inst->network_basic.fsel_win = NULL;
            }
        }
    }
}

void if_network_dialog_basic_update(Instance* inst,Exalt_DBus_Response *response)
{
    const char* string;
    int boolean;
    char buf[1024];
    Exalt_Configuration_Network *cn;
    Exalt_Configuration *c;

    if(!inst->network_basic.dialog)
        return ;

    if(exalt_dbus_response_type_get(response)!=EXALT_DBUS_RESPONSE_NETWORK_CONFIGURATION_GET)
    {
        string = exalt_dbus_response_iface_get(response);
        if( !inst->network_basic.network->iface || !string
                || !strcmp(inst->network_basic.network->iface,string)==0)
            return;
    }

    switch(exalt_dbus_response_type_get(response))
    {
        case EXALT_DBUS_RESPONSE_IFACE_UP_IS:
            boolean = exalt_dbus_response_is_get(response);
            inst->network_basic.network->is_up = boolean;
            if_network_dialog_basic_icon_update(inst);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_LINK_IS:
            boolean = exalt_dbus_response_is_get(response);
            inst->network_basic.network->is_link = boolean;
            if_network_dialog_basic_icon_update(inst);
            break;
        case EXALT_DBUS_RESPONSE_WIRELESS_ESSID_GET:
            string = exalt_dbus_response_string_get(response);
            EXALT_FREE(inst->network_basic.current_essid);
            inst->network_basic.current_essid = strdup(string);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_CONNECTED_IS:
            boolean = exalt_dbus_response_is_get(response);
            inst->network_basic.network->is_connected = boolean;
            if_network_dialog_basic_icon_update(inst);
            break;
        case EXALT_DBUS_RESPONSE_NETWORK_CONFIGURATION_GET:
            c = exalt_dbus_response_configuration_get(response);
            cn = exalt_conf_network_get(c);
            e_widget_entry_text_set(inst->network_basic.entry_pwd, exalt_conf_network_key_get(cn));
            e_widget_entry_text_set(inst->network_basic.entry_ca_cert, exalt_conf_network_ca_cert_get(cn));
            e_widget_entry_text_set(inst->network_basic.entry_client_cert, exalt_conf_network_client_cert_get(cn));
            e_widget_entry_text_set(inst->network_basic.entry_private_key, exalt_conf_network_private_key_get(cn));
        default: break;
    }

    const char *essid = exalt_wireless_network_essid_get(inst->network_basic.network->n);
    if(inst->network_basic.network->is_up
            && inst->network_basic.network->is_link
            && inst->network_basic.network->is_connected
            && inst->network_basic.current_essid && essid
            && strcmp(inst->network_basic.current_essid, essid) == 0)
        e_widget_button_label_set(inst->network_basic.btn, D_("Disconnect the interface"));
    else
    {
        snprintf(buf,1024,D_("Network: %s"),essid);
        e_widget_frametable_label_set(inst->network_basic.flist, buf);
        e_widget_button_label_set(inst->network_basic.btn, D_("Connect to the network"));
    }
}

void if_network_basic_dialog_cb_ca_cert(void *data, void *data2)
{
    Instance *inst;
    inst = data;
    Evas *evas;
    Evas_Object *fsel;

    inst->network_basic.fsel_win = e_dialog_new(inst->gcc->gadcon->zone->container, "e", "exalt_fsel");
    e_dialog_title_set(inst->network_basic.fsel_win, D_("Select the company certificate"));
    inst->network_basic.fsel_win->data = inst;

    evas =  e_win_evas_get(inst->network_basic.fsel_win->win);

    //
    fsel = e_widget_fsel_add(evas, "~/", "/", inst->network_basic.fs_ca_cert, NULL, if_network_basic_dialog_cb_fs_ca_cert, inst, NULL, NULL, 0);
    inst->network_basic.fsel = fsel;
    //

    //
    e_dialog_button_add(inst->network_basic.fsel_win, D_("OK"), NULL, if_network_basic_dialog_cb_fs_ca_cert_ok, inst);
    e_dialog_button_add(inst->network_basic.fsel_win, D_("Cancel"), NULL, if_network_basic_dialog_cb_fs_cancel, inst);
    //

    int mw,mh;
    e_widget_size_min_get(fsel, &mw, &mh);
    e_dialog_content_set(inst->network_basic.fsel_win, fsel, mw, mh);

    e_dialog_resizable_set(inst->network_basic.fsel_win, 1);
    e_dialog_show(inst->network_basic.fsel_win);
}

void if_network_basic_dialog_cb_fs_ca_cert_ok(void *data, E_Dialog *dialog)
{
    Instance *inst = data;
    if_network_basic_dialog_cb_fs_ca_cert(inst, inst->network_basic.fsel);
}

void if_network_basic_dialog_cb_fs_ca_cert(void *data, Evas_Object *obj)
{
    Instance *inst = data;
    e_widget_entry_text_set(inst->network_basic.entry_ca_cert, e_widget_fsel_selection_path_get(obj));

    e_object_del(E_OBJECT(inst->network_basic.fsel_win));
    inst->network_basic.fsel_win = NULL;
}

void if_network_basic_dialog_cb_client_cert(void *data, void *data2)
{
    Instance *inst;
    inst = data;
    Evas *evas;
    Evas_Object *fsel;

    inst->network_basic.fsel_win = e_dialog_new(inst->gcc->gadcon->zone->container, "e", "exalt_fsel");
    e_dialog_title_set(inst->network_basic.fsel_win, D_("Select the client certificate"));
    inst->network_basic.fsel_win->data = inst;

    evas =  e_win_evas_get(inst->network_basic.fsel_win->win);

    //
    fsel = e_widget_fsel_add(evas, "~/", "/", inst->network_basic.fs_client_cert, NULL, if_network_basic_dialog_cb_fs_client_cert, inst, NULL, NULL, 0);
    inst->network_basic.fsel = fsel;
    //

    //
    e_dialog_button_add(inst->network_basic.fsel_win, D_("OK"), NULL, if_network_basic_dialog_cb_fs_client_cert_ok, inst);
    e_dialog_button_add(inst->network_basic.fsel_win, D_("Cancel"), NULL, if_network_basic_dialog_cb_fs_cancel, inst);
    //

    int mw,mh;
    e_widget_size_min_get(fsel, &mw, &mh);
    e_dialog_content_set(inst->network_basic.fsel_win, fsel, mw, mh);

    e_dialog_resizable_set(inst->network_basic.fsel_win, 1);
    e_dialog_show(inst->network_basic.fsel_win);
}

void if_network_basic_dialog_cb_fs_client_cert_ok(void *data, E_Dialog *dialog)
{
    Instance *inst = data;
    if_network_basic_dialog_cb_fs_client_cert(inst, inst->network_basic.fsel);
}


void if_network_basic_dialog_cb_fs_client_cert(void *data, Evas_Object *obj)
{
    Instance *inst = data;
    e_widget_entry_text_set(inst->network_basic.entry_client_cert, e_widget_fsel_selection_path_get(obj));

    e_object_del(E_OBJECT(inst->network_basic.fsel_win));
    inst->network_basic.fsel_win = NULL;
}


void if_network_basic_dialog_cb_private_key(void *data, void *data2)
{
    Instance *inst;
    inst = data;
    Evas *evas;
    Evas_Object *fsel;

    inst->network_basic.fsel_win = e_dialog_new(inst->gcc->gadcon->zone->container, "e", "exalt_fsel");
    e_dialog_title_set(inst->network_basic.fsel_win, D_("Select the client certificate"));
    inst->network_basic.fsel_win->data = inst;

    evas =  e_win_evas_get(inst->network_basic.fsel_win->win);

    //
    fsel = e_widget_fsel_add(evas, "~/", "/", inst->network_basic.fs_private_key, NULL, if_network_basic_dialog_cb_fs_private_key, inst, NULL, NULL, 0);
    inst->network_basic.fsel = fsel;
    //

    //
    e_dialog_button_add(inst->network_basic.fsel_win, D_("OK"), NULL, if_network_basic_dialog_cb_fs_private_key_ok, inst);
    e_dialog_button_add(inst->network_basic.fsel_win, D_("Cancel"), NULL, if_network_basic_dialog_cb_fs_cancel, inst);
    //

    int mw,mh;
    e_widget_size_min_get(fsel, &mw, &mh);
    e_dialog_content_set(inst->network_basic.fsel_win, fsel, mw, mh);

    e_dialog_resizable_set(inst->network_basic.fsel_win, 1);
    e_dialog_show(inst->network_basic.fsel_win);
}

void if_network_basic_dialog_cb_fs_private_key_ok(void *data, E_Dialog *dialog)
{
    Instance *inst = data;
    if_network_basic_dialog_cb_fs_private_key(inst, inst->network_basic.fsel);
}


void if_network_basic_dialog_cb_fs_private_key(void *data, Evas_Object *obj)
{
    Instance *inst = data;
    e_widget_entry_text_set(inst->network_basic.entry_private_key, e_widget_fsel_selection_path_get(obj));

    e_object_del(E_OBJECT(inst->network_basic.fsel_win));
    inst->network_basic.fsel_win = NULL;
}


void if_network_basic_dialog_cb_fs_cancel(void *data, E_Dialog *dialog)
{
    Instance *inst = data;
    e_object_del(E_OBJECT(inst->network_basic.fsel_win));
    inst->network_basic.fsel_win = NULL;
}


void if_network_dialog_basic_icon_update(Instance *inst)
{
    char buf[1024];
    char* group;

    if(!inst->network_basic.dialog)
        return ;

    if(!inst->network_basic.network->is_link)
        edje_object_signal_emit(inst->network_basic.icon,"notLink","exalt");
    else if(!inst->network_basic.network->is_up)
        edje_object_signal_emit(inst->network_basic.icon,"notActivate","exalt");
    else if(!inst->network_basic.network->is_connected)
        edje_object_signal_emit(inst->network_basic.icon,"notConnected","exalt");
    else
        edje_object_signal_emit(inst->network_basic.icon,"default","exalt");
}

void if_network_dialog_basic_cb_del(E_Win *win)
{
    E_Dialog *dialog;
    Instance *inst;

    dialog = win->data;
    inst = dialog->data;
    if_network_dialog_basic_hide(inst);
}

void if_network_dialog_basic_cb_close(void *data, E_Dialog *dialog)
{
    Instance* inst = data;
    if_network_dialog_basic_hide(inst);
}

void if_network_dialog_basic_cb_connect(void *data, void*data2)
{
    Instance *inst = data;

    const char *essid = exalt_wireless_network_essid_get(inst->network_basic.network->n);
    if(inst->network_basic.network->is_up
            && inst->network_basic.network->is_link
            && inst->network_basic.network->is_connected
            && inst->network_basic.current_essid && essid
            && strcmp(inst->network_basic.current_essid, essid) == 0)

    {
        if_network_dialog_basic_cb_disconnect(inst,NULL);
        return ;
    }

    //else apply a new configuration

    Exalt_Configuration* conf = exalt_conf_new();

    exalt_conf_wireless_set(conf,1);
    exalt_conf_mode_set(conf,EXALT_DHCP);

    //wireless part
    Exalt_Configuration_Network *n = exalt_conf_network_new();
    exalt_conf_network_set(conf,n);

    exalt_conf_network_key_set(n,
            e_widget_entry_text_get(inst->network_basic.entry_pwd));

    exalt_conf_network_ca_cert_set(n,
            e_widget_entry_text_get(inst->network_basic.entry_ca_cert));
    exalt_conf_network_client_cert_set(n,
            e_widget_entry_text_get(inst->network_basic.entry_client_cert));
    exalt_conf_network_private_key_set(n,
            e_widget_entry_text_get(inst->network_basic.entry_private_key));


    exalt_conf_network_essid_set(n,
            exalt_wireless_network_essid_get(inst->network_basic.network->n));
    exalt_conf_network_encryption_set(n,
            exalt_wireless_network_encryption_is(inst->network_basic.network->n));
    exalt_conf_network_wep_hexa_set(n,inst->network_basic.wep_key_hexa);
    exalt_conf_network_mode_set(n,
            exalt_wireless_network_mode_get(inst->network_basic.network->n));

    Exalt_Wireless_Network_IE *ie = eina_list_nth(
            exalt_wireless_network_ie_get(inst->network_basic.network->n),0);

    if(ie)
    {
        exalt_conf_network_wpa_set(n, 1);
        exalt_conf_network_wpa_type_set(n,
                exalt_wireless_network_ie_wpa_type_get(ie));
        exalt_conf_network_group_cypher_set(n,
                exalt_wireless_network_ie_group_cypher_get(ie));
        exalt_conf_network_pairwise_cypher_set(n,
                exalt_wireless_network_ie_pairwise_cypher_get(ie, 0));
        exalt_conf_network_auth_suites_set(n,
                exalt_wireless_network_ie_auth_suites_get(ie, 0));
        exalt_conf_network_eap_set(n,
                exalt_wireless_network_ie_eap_get(ie));
    }
    else if(exalt_conf_network_encryption_is(n))
        exalt_conf_network_wep_set(n, 1);
    if(exalt_conf->save_network)
        exalt_conf_network_save_when_apply_set(n,1);

    exalt_dbus_eth_conf_apply(inst->conn,inst->network_basic.network->iface,conf);
    exalt_conf_free(&conf);
}

void if_network_dialog_basic_cb_disconnect(void *data, void*data2)
{
    Instance *inst = data;
    exalt_dbus_wireless_disconnect(inst->conn, inst->network_basic.network->iface);
}
