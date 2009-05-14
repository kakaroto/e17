// Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>


#include "e_mod_main.h"

void if_wireless_dialog_init(Instance* inst)
{
    inst->wireless.iface = NULL;
    inst->wireless.dialog = NULL;
}

void if_wireless_dialog_create(Instance* inst)
{
    Evas* evas;
    Evas_Object *list, *flist, *o, *lbl;
    E_Radio_Group* rg;
    int mh,mw;
    char buf[4096];

    inst->wireless.dialog = e_dialog_new(inst->gcc->gadcon->zone->container, "e", "exalt_wireless_dialog");
    e_dialog_title_set(inst->wireless.dialog, D_("Wireless Interface Settings"));
    inst->wireless.dialog->data = inst;

    evas = e_win_evas_get(inst->wireless.dialog->win);
    list = e_widget_list_add(evas, 0, 0);

    flist = e_widget_frametable_add(evas, D_("wireless interface"), 0);

    inst->wireless.icon = edje_object_add(evas);
    snprintf(buf,1024,"%s/e-module-exalt.edj",exalt_conf->module->dir);
    edje_object_file_set(inst->wireless.icon, buf,"modules/exalt/icons/wireless");
    evas_object_show(inst->wireless.icon);
    o = e_widget_image_add_from_object(evas,inst->wireless.icon,40,40);
    e_widget_frametable_object_append(flist, o, 0, 0, 1, 1, 1, 0, 1, 0);

    inst->wireless.btn_activate = e_widget_button_add(evas,D_("Activate"),NULL,if_wireless_dialog_cb_activate,inst,NULL);
    e_widget_frametable_object_append(flist, inst->wireless.btn_activate, 1, 0, 1, 1, 1, 0, 1, 0);

    inst->wireless.btn_deactivate = e_widget_button_add(evas,D_("Deactivate"),NULL,if_wireless_dialog_cb_deactivate,inst,NULL);
    e_widget_frametable_object_append(flist, inst->wireless.btn_deactivate, 2, 0, 1, 1, 1, 0, 1, 0);

    e_widget_list_object_append(list, flist, 1, 0, 0.5);

    e_widget_min_size_get(list, &mw, &mh);
    e_dialog_content_set(inst->wireless.dialog, list, mw, mh);

    e_win_delete_callback_set(inst->wireless.dialog->win, if_wireless_dialog_cb_del);

    e_dialog_button_add(inst->wireless.dialog, D_("OK"), NULL, if_wireless_dialog_cb_ok, inst);

    e_dialog_button_focus_num(inst->wireless.dialog, 1);
    e_win_centered_set(inst->wireless.dialog->win, 1);
}

void if_wireless_dialog_show(Instance* inst)
{
    if(!inst->wireless.dialog)
        if_wireless_dialog_create(inst);
    e_dialog_show(inst->wireless.dialog);
}

void if_wireless_dialog_set(Instance *inst, Popup_Elt* iface)
{
    if(inst->wireless.iface)
    {
        inst->wireless.iface->nb_use--;
        if(inst->wireless.iface->nb_use<=0)
            popup_elt_free(inst->wireless.iface);
    }

    inst->wireless.iface = iface;
    iface->nb_use++;

    exalt_dbus_eth_up_is(inst->conn,iface->iface);
    exalt_dbus_eth_link_is(inst->conn,iface->iface);
}

void if_wireless_dialog_hide(Instance *inst)
{
    if(inst->wireless.dialog)
    {
        e_object_del(E_OBJECT(inst->wireless.dialog));
        inst->wireless.dialog=NULL;
        if(inst->wireless.iface)
        {
            inst->wireless.iface->nb_use--;
            if(inst->wireless.iface->nb_use<=0)
            {
                popup_elt_free(inst->wireless.iface);
            }
            inst->wireless.iface = NULL;
        }
    }
}

void if_wireless_dialog_update(Instance* inst,Exalt_DBus_Response *response)
{
    char* string;
    int boolean;
    if(!inst->wireless.dialog)
        return ;

    string = exalt_dbus_response_iface_get(response);
    if(!inst->wireless.iface->iface || !string || !strcmp(inst->wireless.iface->iface,string)==0)
        return;

    switch(exalt_dbus_response_type_get(response))
    {
        case EXALT_DBUS_RESPONSE_IFACE_UP_IS:
            boolean = exalt_dbus_response_is_get(response);
            e_widget_disabled_set(inst->wireless.btn_activate,boolean);
            e_widget_disabled_set(inst->wireless.btn_deactivate,!boolean);
            inst->wireless.iface->is_up = boolean;
            if_wireless_dialog_icon_update(inst);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_LINK_IS:
            boolean = exalt_dbus_response_is_get(response);
            inst->wireless.iface->is_link = boolean;
            if_wireless_dialog_icon_update(inst);
            break;

        default: break;
    }
    if_wireless_disabled_update(inst);
}


void if_wireless_dialog_icon_update(Instance *inst)
{
    char buf[1024];
    char* group;

    if(!inst->wireless.dialog)
        return ;

    if(!inst->wireless.iface->is_link)
        edje_object_signal_emit(inst->wireless.icon,"notLink","exalt");
    else if(!inst->wireless.iface->is_up)
        edje_object_signal_emit(inst->wireless.icon,"notActivate","exalt");
    else
        edje_object_signal_emit(inst->wireless.icon,"default","exalt");

}

void if_wireless_disabled_update(Instance *inst)
{
    int disabled;
    int is_ip;

    if(!inst->wireless.dialog)
        return;

    //nothing to do
}


void if_wireless_dialog_cb_ok(void *data, E_Dialog *dialog)
{
    Instance* inst = data;
    if_wireless_dialog_hide(inst);
}


void if_wireless_dialog_cb_del(E_Win *win)
{
    E_Dialog *dialog;
    Instance *inst;

    dialog = win->data;
    inst = dialog->data;
    if_wireless_dialog_hide(inst);
}

void if_wireless_dialog_cb_activate(void *data, void*data2)
{
    Instance *inst = data;
    exalt_dbus_eth_up(inst->conn, inst->wireless.iface->iface);
}

void if_wireless_dialog_cb_deactivate(void *data, void*data2)
{
    Instance *inst = data;
    exalt_dbus_eth_down(inst->conn, inst->wireless.iface->iface);
}
