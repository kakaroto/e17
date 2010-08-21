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
    e_dialog_title_set(inst->network.dialog, D_("Wireless Configuration Settings"));
    snprintf(buf, sizeof(buf), "%s/e-module-exalt.edj", exalt_conf->module->dir);
    inst->network.dialog->data = inst;

    evas = e_win_evas_get(inst->network.dialog->win);

    inst->network.table = e_widget_table_add(evas,0);

    flist = e_widget_frametable_add(evas, D_("Network information"), 0);

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

    lbl = e_widget_label_add(evas,D_("Add to favorites: "));
    e_widget_frametable_object_append(flist, lbl, 0, 7, 2, 1, 1, 0, 1, 0);
    inst->network.check_favoris = e_widget_check_add(evas, "", &(inst->network.favoris));
    e_widget_frametable_object_append(flist, inst->network.check_favoris, 2, 7, 1, 1, 1, 0, 1, 0);

    inst->network.f_iface = flist;

    e_dialog_button_add(inst->network.dialog, D_("OK"), NULL,if_network_dialog_cb_ok, inst);
    e_dialog_button_add(inst->network.dialog, D_("Apply"), NULL, if_network_dialog_cb_apply, inst);
    e_dialog_button_add(inst->network.dialog, D_("Cancel"), NULL, if_network_dialog_cb_cancel, inst);

    //e_win_delete_callback_set(inst->network.dialog->win, if_network_dialog_cb_del);

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
    int nb_fr = 1;

    if(inst->network.network)
    {
        inst->network.network->nb_use--;
        if(inst->network.network->nb_use<=0)
            popup_elt_free(inst->network.network);
    }

    inst->network.network = network;
    network->nb_use++;

    e_widget_label_text_set(inst->network.lbl_essid,exalt_wireless_network_essid_get(network->n));
    e_widget_label_text_set(inst->network.lbl_address,exalt_wireless_network_address_get(network->n));

    snprintf(buf,1024,"%d %%",exalt_wireless_network_quality_get(network->n));
    e_widget_label_text_set(inst->network.lbl_quality,buf);

    //entry hidden by default
    evas_object_hide(inst->network.lbl_ca_cert);
    evas_object_hide(inst->network.entry_ca_cert);
    evas_object_hide(inst->network.lbl_client_cert);
    evas_object_hide(inst->network.entry_client_cert);
    evas_object_hide(inst->network.lbl_private_key);
    evas_object_hide(inst->network.entry_private_key);
    evas_object_hide(inst->network.lbl_pwd);
    evas_object_hide(inst->network.entry_pwd);

    if(exalt_wireless_network_encryption_is(network->n))
    {
        Eina_List* l_ie;

        evas_object_show(inst->network.lbl_pwd);
        evas_object_show(inst->network.entry_pwd);

        if( !(l_ie = exalt_wireless_network_ie_get(network->n)))
        {
            //WEP encryption
            Evas_Object* fr = if_network_dialog_wep_new(inst,network->n);
            e_widget_table_object_append(inst->network.table,fr,0,1,1,1,1,1,1,0);
            nb_fr++;
        }
        else
        {
            Evas_Object* fr = if_network_dialog_wpa_new(inst,network->n);

            if(fr)
            {
                e_widget_table_object_append(inst->network.table,
                        fr,0,nb_fr,1,1,1,1,1,0);
                nb_fr++;
            }
        }
    }


    e_widget_table_object_append(inst->network.table,inst->network.f_iface,1,0,1,2,1,1,1,0);

    int mw,mh;
    e_widget_size_min_get(inst->network.table, &mw, &mh);
    e_dialog_content_set(inst->network.dialog, inst->network.table, mw, mh);


    exalt_dbus_eth_ip_get(inst->conn,network->iface);
    exalt_dbus_eth_gateway_get(inst->conn,network->iface);
    exalt_dbus_eth_netmask_get(inst->conn,network->iface);
    exalt_dbus_eth_command_get(inst->conn,network->iface);
    exalt_dbus_eth_dhcp_is(inst->conn,network->iface);
    exalt_dbus_eth_up_is(inst->conn,network->iface);
    exalt_dbus_eth_link_is(inst->conn,network->iface);
    exalt_dbus_eth_connected_is(inst->conn, network->iface);

    exalt_dbus_network_configuration_get(inst->conn, exalt_wireless_network_essid_get(network->n));
    e_dialog_show(inst->network.dialog);
}

Evas_Object* if_network_dialog_wep_new(Instance* inst,Exalt_Wireless_Network* n)
{
    Evas_Object* lbl, *o;

    Evas*evas = e_win_evas_get(inst->network.dialog->win);

    Evas_Object* flist = e_widget_frametable_add(evas, D_("WEP information"), 0);

    lbl = e_widget_label_add(evas,D_("Description: "));
    e_widget_frametable_object_append(flist, lbl, 0, 0, 1, 1, 1, 0, 1, 0);
    lbl = e_widget_label_add(evas,D_("WEP"));
    e_widget_frametable_object_append(flist, lbl, 1, 0, 1, 1, 1, 0, 1, 0);



    int i = 7;
    inst->network.entry_ca_cert = NULL;
    inst->network.entry_client_cert = NULL;
    inst->network.entry_private_key = NULL;

    //empty block
    lbl = e_widget_label_add(evas,"");
    e_widget_frametable_object_append(inst->network.f_iface, lbl, 0, i, 2, 1, 1, 0, 1, 0);

    i++;

    inst->network.wep_key_hexa = 1;
    E_Radio_Group* rg = e_widget_radio_group_new(&(inst->network.wep_key_hexa));
    o = e_widget_radio_add(evas, D_("Hexadecimal Key"), 1, rg);
    e_widget_frametable_object_append(inst->network.f_iface, o, 0, i, 2, 1, 1, 0, 1, 0);
    inst->network.radio_wep_hexa = o;

    o = e_widget_radio_add(evas, D_("ASCII Key"), 0, rg);
    e_widget_frametable_object_append(inst->network.f_iface, o, 2, i, 2, 1, 1, 0, 1, 0);
    inst->network.radio_wep_ascii = o;

    i++;

    lbl = e_widget_label_add(evas,D_("Password: "));
    e_widget_frametable_object_append(inst->network.f_iface, lbl, 0, i, 1, 1, 1, 0, 0, 0);
    inst->network.lbl_pwd = lbl;
    inst->network.entry_pwd = e_widget_entry_add(evas,&(inst->network.pwd),if_network_dialog_cb_entry,inst,NULL);
    e_widget_frametable_object_append(inst->network.f_iface, inst->network.entry_pwd, 2, i, 1, 1, 1, 0, 1, 0);

    return flist;
}

Evas_Object* if_network_dialog_wpa_new(Instance* inst,Exalt_Wireless_Network* n)
{
    Evas_Object* lbl;
    const char* s;
    char buf[1024];
    E_Radio_Group* rg;
    Evas_Object* radio;
    int i;
    Eina_List *l_ie,*l;
    Exalt_Wireless_Network_IE *ie;


    Evas*evas = e_win_evas_get(inst->network.dialog->win);

    Evas_Object* flist = e_widget_frametable_add(evas, D_("WPA information"), 0);


    rg = e_widget_radio_group_new(&(inst->network.ie_choice));

    l_ie=exalt_wireless_network_ie_get(n);
    int ie_number = 0;
    int radio_number = 0;
    int is_eap =  0;
    int radio_id = 0;
    EINA_LIST_FOREACH(l_ie,l,ie)
    {
        int i,j;
        const char* wpa = exalt_wireless_network_name_from_wpa_type(
                exalt_wireless_network_ie_wpa_type_get(ie));

        for(i=0;i<exalt_wireless_network_ie_auth_suites_number_get(ie);i++)
        {
            const char* auth = exalt_wireless_network_name_from_auth_suites(
                    exalt_wireless_network_ie_auth_suites_get(ie,i));

            if(exalt_wireless_network_ie_auth_suites_get(ie,i)
                    == AUTH_SUITES_EAP)
                is_eap = 1;
            for(j=0;j<exalt_wireless_network_ie_pairwise_cypher_number_get(ie);j++)
            {
                char buf[1024];
                const char* pairwise = exalt_wireless_network_name_from_cypher_name(
                        exalt_wireless_network_ie_pairwise_cypher_get(ie,j));

                snprintf(buf,1024,"%s-%s-%s",wpa,auth,pairwise);
                radio = e_widget_radio_add(evas, buf,
                        ie_number*100+i*10+j, rg);
                e_widget_frametable_object_append(flist,
                        radio, 0, radio_number, 1, 1, 1, 0, 1, 0);
                inst->network.radio_wpa[radio_id] = radio;
                radio_id++;

                radio_number++;
            }
        }
        ie_number++;
    }

    i = 7;
    inst->network.entry_ca_cert = NULL;
    inst->network.entry_client_cert = NULL;
    inst->network.entry_private_key = NULL;

    //empty block
    lbl = e_widget_label_add(evas,"");
    e_widget_frametable_object_append(inst->network.f_iface, lbl, 0, i, 2, 1, 1, 0, 1, 0);

    i++;

    lbl = e_widget_label_add(evas,D_("Password: "));
    e_widget_frametable_object_append(inst->network.f_iface, lbl, 0, i, 1, 1, 1, 0, 0, 0);
    inst->network.lbl_pwd = lbl;
    inst->network.entry_pwd = e_widget_entry_add(evas,&(inst->network.pwd),if_network_dialog_cb_entry,inst,NULL);
    e_widget_frametable_object_append(inst->network.f_iface, inst->network.entry_pwd, 2, i, 1, 1, 1, 0, 1, 0);
    i++;

    if(is_eap)
    {
        Evas_Object *bt;

        lbl = e_widget_label_add(evas,D_("CA certificate: "));
        e_widget_frametable_object_append(inst->network.f_iface, lbl, 0, i, 2, 1, 1, 0, 1, 0);
        inst->network.lbl_ca_cert = lbl;
        inst->network.entry_ca_cert = e_widget_entry_add(evas,&(inst->network.ca_cert),if_network_dialog_cb_entry,inst,NULL);
        e_widget_frametable_object_append(inst->network.f_iface, inst->network.entry_ca_cert, 2, i, 1, 1, 1, 0, 1, 0);
        e_widget_entry_readonly_set(inst->network.entry_ca_cert, 1);

        bt = e_widget_button_add(evas, "...", NULL, if_network_dialog_cb_ca_cert, inst, inst);
        e_widget_frametable_object_append(inst->network.f_iface, bt, 3, i, 1, 1, 1, 0, 1, 0);

        i++;

        lbl = e_widget_label_add(evas,D_("Client certificate: "));
        e_widget_frametable_object_append(inst->network.f_iface, lbl, 0, i, 2, 1, 1, 0, 1, 0);
        inst->network.lbl_client_cert = lbl;
        inst->network.entry_client_cert = e_widget_entry_add(evas,&(inst->network.client_cert),if_network_dialog_cb_entry,inst,NULL);
        e_widget_disabled_set(inst->network.entry_client_cert, 1);
        e_widget_frametable_object_append(inst->network.f_iface, inst->network.entry_client_cert, 2, i, 1, 1, 1, 0, 1, 0);
        e_widget_entry_readonly_set(inst->network.entry_client_cert, 1);

        bt = e_widget_button_add(evas, "...", NULL, if_network_dialog_cb_client_cert, inst, inst);
        e_widget_frametable_object_append(inst->network.f_iface, bt, 3, i, 1, 1, 1, 0, 1, 0);

        i++;

        lbl = e_widget_label_add(evas,D_("Private key: "));
        e_widget_frametable_object_append(inst->network.f_iface, lbl, 0, i, 2, 1, 1, 0, 1, 0);
        inst->network.lbl_private_key = lbl;
        inst->network.entry_private_key = e_widget_entry_add(evas,&(inst->network.private_key),if_network_dialog_cb_entry,inst,NULL);
        e_widget_disabled_set(inst->network.entry_private_key, 1);
        e_widget_frametable_object_append(inst->network.f_iface, inst->network.entry_private_key, 2, i, 1, 1, 1, 0, 1, 0);
        e_widget_entry_readonly_set(inst->network.entry_private_key, 1);

        bt = e_widget_button_add(evas, "...", NULL, if_network_dialog_cb_private_key, inst, inst);
        e_widget_frametable_object_append(inst->network.f_iface, bt, 3, i, 1, 1, 1, 0, 1, 0);

        i++;
    }

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
        if(inst->network.fsel_win)
        {
            e_object_del(E_OBJECT(inst->network.fsel_win));
            inst->network.fsel_win = NULL;
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
    const char* string;
    int boolean;
    Exalt_Configuration_Network *cn;
    Exalt_Configuration *c;

    if(!inst->network.dialog)
        return ;

    if(exalt_dbus_response_type_get(response)!=EXALT_DBUS_RESPONSE_NETWORK_CONFIGURATION_GET)
    {
        string = exalt_dbus_response_iface_get(response);
        if( !inst->network.network->iface || !string
                || !strcmp(inst->network.network->iface,string)==0)
            return;
    }

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
        case EXALT_DBUS_RESPONSE_IFACE_CONNECTED_IS:
            boolean = exalt_dbus_response_is_get(response);
            inst->network.network->is_connected = boolean;
            if_network_dialog_icon_update(inst);
            break;
        case EXALT_DBUS_RESPONSE_NETWORK_CONFIGURATION_GET:
            c = exalt_dbus_response_configuration_get(response);
            cn = exalt_conf_network_get(c);
            //exalt_conf_network_print(cn);
            if(exalt_conf_mode_get(c) == EXALT_DHCP)
                e_widget_radio_toggle_set(inst->network.radio_dhcp, 1);
            else
            {
                e_widget_radio_toggle_set(inst->network.radio_static, 1);
                e_widget_entry_text_set(inst->network.entry_ip, exalt_conf_ip_get(c));
                e_widget_entry_text_set(inst->network.entry_netmask, exalt_conf_netmask_get(c));
                e_widget_entry_text_set(inst->network.entry_gateway, exalt_conf_gateway_get(c));
            }
            if(exalt_conf_network_wep_hexa_is(cn))
                e_widget_radio_toggle_set(inst->network.radio_wep_hexa, 1);
            else
                e_widget_radio_toggle_set(inst->network.radio_wep_ascii, 1);

            e_widget_entry_text_set(inst->network.entry_pwd, exalt_conf_network_key_get(cn));
            e_widget_entry_text_set(inst->network.entry_ca_cert, exalt_conf_network_ca_cert_get(cn));
            e_widget_entry_text_set(inst->network.entry_client_cert, exalt_conf_network_client_cert_get(cn));
            e_widget_entry_text_set(inst->network.entry_private_key, exalt_conf_network_private_key_get(cn));

            e_widget_check_checked_set(inst->network.check_favoris, exalt_conf_network_favoris_is(cn));

            //select the radio button in the wpa list
            Eina_List *l_ie=exalt_wireless_network_ie_get(inst->network.network->n);
            Eina_List *l;
            Exalt_Wireless_Network_IE *ie;
            int radio_id = 0;
            int step1=0,step2=0,step3=0;
            int found = 0;
            EINA_LIST_FOREACH(l_ie,l,ie)
            {
                int i,j;
                if(exalt_conf_network_wpa_type_get(cn) == exalt_wireless_network_ie_wpa_type_get(ie))
                    step1=1;

                for(i=0;!found && i<exalt_wireless_network_ie_auth_suites_number_get(ie);i++)
                {
                    if(exalt_conf_network_auth_suites_get(cn) ==
                            exalt_wireless_network_ie_auth_suites_get(ie,i))
                        step2=1;

                    for(j=0;!found && j<exalt_wireless_network_ie_pairwise_cypher_number_get(ie);j++)
                    {
                        if(exalt_conf_network_pairwise_cypher_get(cn) ==
                                exalt_wireless_network_ie_pairwise_cypher_get(ie,j))
                            step3=1;

                        if(step1 && step2 && step3)
                        {
                            found = 1;
                            l=NULL;
                        }
                        else
                        {
                            radio_id++;
                        }
                        step3=0;
                    }
                    step2=0;
                }
                step1=0;
            }

            if(found)
                e_widget_radio_toggle_set(inst->network.radio_wpa[radio_id], 1);
            //

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
    else if(!inst->network.network->is_connected)
        edje_object_signal_emit(inst->network.icon,"notConnected","exalt");
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
        is_ip = exalt_address_is(string);

        if(is_ip)
        {
            string = e_widget_entry_text_get(inst->network.entry_netmask);
            is_ip = exalt_address_is(string);
        }

        if(is_ip)
        {
            string = e_widget_entry_text_get(inst->network.entry_gateway);
            is_ip = exalt_address_is(string);
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


void if_network_dialog_cb_fs_cancel(void *data, E_Dialog *dialog)
{
    Instance *inst = data;
    e_object_del(E_OBJECT(inst->network.fsel_win));
    inst->network.fsel_win = NULL;
}


void if_network_dialog_cb_ca_cert(void *data, void *data2)
{
    Instance *inst;
    inst = data;
    Evas *evas;
    Evas_Object *fsel;

    inst->network.fsel_win = e_dialog_new(inst->gcc->gadcon->zone->container, "e", "exalt_fsel");
    e_dialog_title_set(inst->network.fsel_win, D_("Select the company certificate"));
    inst->network.fsel_win->data = inst;

    evas =  e_win_evas_get(inst->network.fsel_win->win);

    //
    fsel = e_widget_fsel_add(evas, "~/", "/", inst->network.fs_ca_cert, NULL, if_network_dialog_cb_fs_ca_cert, inst, NULL, NULL, 0);
    inst->network.fsel = fsel;
    //

    //
    e_dialog_button_add(inst->network.fsel_win, D_("OK"), NULL, if_network_dialog_cb_fs_ca_cert_ok, inst);
    e_dialog_button_add(inst->network.fsel_win, D_("Cancel"), NULL, if_network_dialog_cb_fs_cancel, inst);
    //

    int mw,mh;
    e_widget_size_min_get(fsel, &mw, &mh);
    e_dialog_content_set(inst->network.fsel_win, fsel, mw, mh);

    e_dialog_resizable_set(inst->network.fsel_win, 1);
    e_dialog_show(inst->network.fsel_win);
}

void if_network_dialog_cb_fs_ca_cert_ok(void *data, E_Dialog *dialog)
{
    Instance *inst = data;
    if_network_dialog_cb_fs_ca_cert(inst, inst->network.fsel);
}

void if_network_dialog_cb_fs_ca_cert(void *data, Evas_Object *obj)
{
    Instance *inst = data;
    e_widget_entry_text_set(inst->network.entry_ca_cert, e_widget_fsel_selection_path_get(obj));

    e_object_del(E_OBJECT(inst->network.fsel_win));
    inst->network.fsel_win = NULL;
}

void if_network_dialog_cb_client_cert(void *data, void *data2)
{
    Instance *inst;
    inst = data;
    Evas *evas;
    Evas_Object *fsel;

    inst->network.fsel_win = e_dialog_new(inst->gcc->gadcon->zone->container, "e", "exalt_fsel");
    e_dialog_title_set(inst->network.fsel_win, D_("Select the client certificate"));
    inst->network.fsel_win->data = inst;

    evas =  e_win_evas_get(inst->network.fsel_win->win);

    //
    fsel = e_widget_fsel_add(evas, "~/", "/", inst->network.fs_client_cert, NULL, if_network_dialog_cb_fs_client_cert, inst, NULL, NULL, 0);
    inst->network.fsel = fsel;
    //

    //
    e_dialog_button_add(inst->network.fsel_win, D_("OK"), NULL, if_network_dialog_cb_fs_client_cert_ok, inst);
    e_dialog_button_add(inst->network.fsel_win, D_("Cancel"), NULL, if_network_dialog_cb_fs_cancel, inst);
    //

    int mw,mh;
    e_widget_size_min_get(fsel, &mw, &mh);
    e_dialog_content_set(inst->network.fsel_win, fsel, mw, mh);

    e_dialog_resizable_set(inst->network.fsel_win, 1);
    e_dialog_show(inst->network.fsel_win);
}

void if_network_dialog_cb_fs_client_cert_ok(void *data, E_Dialog *dialog)
{
    Instance *inst = data;
    if_network_dialog_cb_fs_client_cert(inst, inst->network.fsel);
}


void if_network_dialog_cb_fs_client_cert(void *data, Evas_Object *obj)
{
    Instance *inst = data;
    e_widget_entry_text_set(inst->network.entry_client_cert, e_widget_fsel_selection_path_get(obj));

    e_object_del(E_OBJECT(inst->network.fsel_win));
    inst->network.fsel_win = NULL;
}


void if_network_dialog_cb_private_key(void *data, void *data2)
{
    Instance *inst;
    inst = data;
    Evas *evas;
    Evas_Object *fsel;

    inst->network.fsel_win = e_dialog_new(inst->gcc->gadcon->zone->container, "e", "exalt_fsel");
    e_dialog_title_set(inst->network.fsel_win, D_("Select the client certificate"));
    inst->network.fsel_win->data = inst;

    evas =  e_win_evas_get(inst->network.fsel_win->win);

    //
    fsel = e_widget_fsel_add(evas, "~/", "/", inst->network.fs_private_key, NULL, if_network_dialog_cb_fs_private_key, inst, NULL, NULL, 0);
    inst->network.fsel = fsel;
    //

    //
    e_dialog_button_add(inst->network.fsel_win, D_("OK"), NULL, if_network_dialog_cb_fs_private_key_ok, inst);
    e_dialog_button_add(inst->network.fsel_win, D_("Cancel"), NULL, if_network_dialog_cb_fs_cancel, inst);
    //

    int mw,mh;
    e_widget_size_min_get(fsel, &mw, &mh);
    e_dialog_content_set(inst->network.fsel_win, fsel, mw, mh);

    e_dialog_resizable_set(inst->network.fsel_win, 1);
    e_dialog_show(inst->network.fsel_win);
}

void if_network_dialog_cb_fs_private_key_ok(void *data, E_Dialog *dialog)
{
    Instance *inst = data;
    if_network_dialog_cb_fs_private_key(inst, inst->network.fsel);
}


void if_network_dialog_cb_fs_private_key(void *data, Evas_Object *obj)
{
    Instance *inst = data;
    e_widget_entry_text_set(inst->network.entry_private_key, e_widget_fsel_selection_path_get(obj));

    e_object_del(E_OBJECT(inst->network.fsel_win));
    inst->network.fsel_win = NULL;
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

    Exalt_Configuration* conf = exalt_conf_new();

    exalt_conf_wireless_set(conf,1);
    if(inst->network.dhcp != 0)
    {
        exalt_conf_mode_set(conf,EXALT_STATIC);
        exalt_conf_ip_set(conf,e_widget_entry_text_get(inst->network.entry_ip));
        exalt_conf_netmask_set(conf,e_widget_entry_text_get(inst->network.entry_netmask));
        exalt_conf_gateway_set(conf,e_widget_entry_text_get(inst->network.entry_gateway));
    }
    else
        exalt_conf_mode_set(conf,EXALT_DHCP);
    exalt_conf_cmd_after_apply_set(conf,e_widget_entry_text_get(inst->network.entry_cmd));


    Exalt_Configuration_Network *n =  exalt_conf_network_new();
        //inst->network.network->n;
    exalt_conf_network_set(conf,n);

    exalt_conf_network_key_set(n,
            e_widget_entry_text_get(inst->network.entry_pwd));
    exalt_conf_network_ca_cert_set(n,
            e_widget_entry_text_get(inst->network.entry_ca_cert));
    exalt_conf_network_client_cert_set(n,
            e_widget_entry_text_get(inst->network.entry_client_cert));
    exalt_conf_network_private_key_set(n,
            e_widget_entry_text_get(inst->network.entry_private_key));

    exalt_conf_network_essid_set(n,
            exalt_wireless_network_essid_get(inst->network.network->n));
    exalt_conf_network_encryption_set(n,
            exalt_wireless_network_encryption_is(inst->network.network->n));
    exalt_conf_network_mode_set(n,
            exalt_wireless_network_mode_get(inst->network.network->n));
    exalt_conf_network_wep_hexa_set(n,inst->network.wep_key_hexa);

    if(exalt_wireless_network_ie_get(inst->network.network->n))
    {
        int choice = inst->network.ie_choice;
        int ie_choice = choice/100;
        int auth_choice = (choice-ie_choice*100)/10;
        int pairwise_choice = choice-ie_choice*100-auth_choice*10;
        Exalt_Wireless_Network_IE *ie = eina_list_nth(
                exalt_wireless_network_ie_get(inst->network.network->n),ie_choice);
        //printf("%d %d %d %d\n",choice,ie_choice,auth_choice,pairwise_choice);
        exalt_conf_network_wpa_set(n, 1);
        exalt_conf_network_wpa_type_set(n,
                exalt_wireless_network_ie_wpa_type_get(ie));
        exalt_conf_network_group_cypher_set(n,
                exalt_wireless_network_ie_group_cypher_get(ie));
        exalt_conf_network_pairwise_cypher_set(n,
                exalt_wireless_network_ie_pairwise_cypher_get(ie, pairwise_choice));
        exalt_conf_network_auth_suites_set(n,
                exalt_wireless_network_ie_auth_suites_get(ie, auth_choice));
        exalt_conf_network_eap_set(n,
                exalt_wireless_network_ie_eap_get(ie));
    }
    else if(exalt_conf_network_encryption_is(n))
        exalt_conf_network_wep_set(n, 1);

    if(exalt_conf->save_network)
        exalt_conf_network_save_when_apply_set(n, 1);

    exalt_conf_network_favoris_set(n, inst->network.favoris);

    exalt_conf_network_print(n);

    exalt_dbus_eth_conf_apply(inst->conn,inst->network.network->iface,conf);

    exalt_conf_free(&conf);
}


