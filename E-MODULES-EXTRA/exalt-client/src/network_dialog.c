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
    list = e_widget_list_add(evas, 0, 0);
    inst->network.list = list;

    flist = e_widget_frametable_add(evas, D_("Wireless network information"), 0);

    lbl = e_widget_label_add(evas,D_("Essid: "));
    e_widget_frametable_object_append(flist, lbl, 0, 0, 1, 1, 1, 0, 1, 0);
    inst->network.lbl_essid = e_widget_label_add(evas,"hehe");
    e_widget_frametable_object_append(flist, inst->network.lbl_essid, 1, 0, 1, 1, 1, 0, 1, 0);

    lbl = e_widget_label_add(evas,D_("Address: "));
    e_widget_frametable_object_append(flist, lbl, 0, 1, 1, 1, 1, 0, 1, 0);
    inst->network.lbl_address = e_widget_label_add(evas,"hehe");
    e_widget_frametable_object_append(flist, inst->network.lbl_address, 1, 1, 1, 1, 1, 0, 1, 0);

    lbl = e_widget_label_add(evas,D_("Quality: "));
    e_widget_frametable_object_append(flist, lbl, 0, 2, 1, 1, 1, 0, 1, 0);
    inst->network.lbl_quality = e_widget_label_add(evas,"hehe");
    e_widget_frametable_object_append(flist, inst->network.lbl_quality, 1, 2, 1, 1, 1, 0, 1, 0);

    lbl = e_widget_label_add(evas,D_("Authentification: "));
    e_widget_frametable_object_append(flist, lbl, 0, 3, 1, 1, 1, 0, 1, 0);
    inst->network.lbl_auth = e_widget_label_add(evas,"hehe");
    e_widget_frametable_object_append(flist, inst->network.lbl_auth, 1, 3, 1, 1, 1, 0, 1, 0);



    e_widget_list_object_append(list, flist, 1, 0, 0.5);

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

    printf("##%s\n",network->iface);
    if(exalt_dbus_wireless_network_encryption_is(network->n))
    {
        Eina_List* l_ie;
        if( (l_ie=exalt_dbus_wireless_network_ie_get(network->n))==NULL)
        {
            Evas_Object* fr = if_network_dialog_wep_new(inst,network->n);
            e_widget_list_object_append(inst->network.list, fr, 1, 0, 0.5);
        }
        else
        {
            Eina_List* l;
            Exalt_Wireless_Network_IE *ie;
            EINA_LIST_FOREACH(l_ie,l,ie)
            {
                Evas_Object *fr;
                switch(exalt_wireless_network_ie_wpa_type_get(ie))
                {
                    case WPA_TYPE_WPA:
                        fr = if_network_dialog_wpa_new(inst,ie,D_("WPA information"));
                        break;
                    case WPA_TYPE_WPA2:
                        fr = if_network_dialog_wpa_new(inst,ie,D_("WPA2 information"));
                    default: //unknow ....
                        break;
                }
                e_widget_list_object_append(inst->network.list, fr, 1, 0, 0.5);
            }
        }
    }

    int mw,mh;
    e_widget_min_size_get(inst->network.list, &mw, &mh);
    e_dialog_content_set(inst->network.dialog, inst->network.list, mw, mh);

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
    INFO_ADD(D_("Group Cypher: "),s,3);

    buf[0]='\0';
    for(i=0;i<exalt_wireless_network_ie_pairwise_cypher_number_get(ie);i++)
    {
        if(i!=0)
            strcat(buf,", ");
        Exalt_Wireless_Network_Cypher_Name name;
        name = exalt_wireless_network_ie_pairwise_cypher_get(ie,i);
        strcat(buf,exalt_wireless_network_name_from_cypher_name(name));
    }
    INFO_ADD(D_("Pairwise Cyphers: "),buf,4);

    buf[0]='\0';
    for(i=0;i<exalt_wireless_network_ie_auth_suites_number_get(ie);i++)
    {
        if(i!=0)
            strcat(buf,", ");
        Exalt_Wireless_Network_Auth_Suites name;
        name = exalt_wireless_network_ie_auth_suites_get(ie,i);
        strcat(buf,exalt_wireless_network_name_from_auth_suites(name));
    }
    INFO_ADD(D_("Authentifications Suites: "),buf,5);

    i = exalt_wireless_network_ie_preauth_supported_is(ie);
    if(i) s=D_("Yes");
    else s= D_("No");
    INFO_ADD(D_("Pre-authentification: "),s,2);

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

