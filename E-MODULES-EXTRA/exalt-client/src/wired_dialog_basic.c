 /* Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
  * Description: The wired dialog used in the basic mode
  *              (Only allows to activate/deactivate an interface and run a DHCP)
  */

#include "e_mod_main.h"

void if_wired_dialog_basic_init(Instance* inst)
{
    inst->wired_basic.iface = NULL;
    inst->wired_basic.dialog = NULL;
}

void if_wired_dialog_basic_create(Instance* inst)
{
    Evas* evas;
    Evas_Object *list, *flist, *o, *lbl;
    E_Radio_Group* rg;
    int mh,mw;
    char buf[4096];

    inst->wired_basic.dialog = e_dialog_new(inst->gcc->gadcon->zone->container, "e", "exalt_wired_dialog_basic");
    e_dialog_title_set(inst->wired_basic.dialog, D_("Wired Connection Settings"));
    inst->wired_basic.dialog->data = inst;

    evas = e_win_evas_get(inst->wired_basic.dialog->win);
    list = e_widget_list_add(evas, 0, 0);

    flist = e_widget_frametable_add(evas, D_("Wired interface"), 0);

    inst->wired_basic.icon = edje_object_add(evas);
    snprintf(buf,1024,"%s/e-module-exalt.edj",exalt_conf->module->dir);
    edje_object_file_set(inst->wired_basic.icon, buf,"modules/exalt/icons/wired");
    evas_object_show(inst->wired_basic.icon);
    o = e_widget_image_add_from_object(evas,inst->wired_basic.icon,40,40);
    e_widget_frametable_object_append(flist, o, 0, 0, 1, 1, 1, 0, 1, 0);

    inst->wired_basic.btn = e_widget_button_add(evas,D_("Deactivate the interface"),NULL,if_wired_dialog_basic_cb_activate,inst,NULL);
    e_widget_frametable_object_append(flist, inst->wired_basic.btn, 1, 0, 1, 1, 1, 0, 1, 0);


    e_widget_list_object_append(list, flist, 1, 0, 0.5);

    e_widget_min_size_get(list, &mw, &mh);
    e_dialog_content_set(inst->wired_basic.dialog, list, mw, mh);

    e_win_delete_callback_set(inst->wired_basic.dialog->win, if_wired_dialog_basic_cb_del);

    e_dialog_button_add(inst->wired_basic.dialog, D_("Close"), NULL, if_wired_dialog_basic_cb_close, inst);
    e_dialog_button_focus_num(inst->wired_basic.dialog, 1);
    e_win_centered_set(inst->wired_basic.dialog->win, 1);
}

void if_wired_dialog_basic_show(Instance* inst)
{
    if(!inst->wired_basic.dialog)
        if_wired_dialog_basic_create(inst);
    e_dialog_show(inst->wired_basic.dialog);
}

void if_wired_dialog_basic_set(Instance *inst, Popup_Elt* iface)
{
    if(inst->wired_basic.iface)
    {
        inst->wired_basic.iface->nb_use--;
        if(inst->wired_basic.iface->nb_use<=0)
            popup_elt_free(inst->wired_basic.iface);
    }

    inst->wired_basic.iface = iface;
    iface->nb_use++;

    exalt_dbus_eth_up_is(inst->conn,iface->iface);
    exalt_dbus_eth_link_is(inst->conn,iface->iface);
}

void if_wired_dialog_basic_hide(Instance *inst)
{
    if(inst->wired_basic.dialog)
    {
        e_object_del(E_OBJECT(inst->wired_basic.dialog));
        inst->wired_basic.dialog=NULL;
        if(inst->wired_basic.iface)
        {
            inst->wired_basic.iface->nb_use--;
            if(inst->wired_basic.iface->nb_use<=0)
            {
                popup_elt_free(inst->wired_basic.iface);
            }
            inst->wired_basic.iface = NULL;
        }
    }
}

void if_wired_dialog_basic_update(Instance* inst,Exalt_DBus_Response *response)
{
    char* string;
    int boolean;
    if(!inst->wired_basic.dialog)
        return ;

    string = exalt_dbus_response_iface_get(response);
    if(!inst->wired_basic.iface->iface || !string || !strcmp(inst->wired_basic.iface->iface,string)==0)
        return;

    switch(exalt_dbus_response_type_get(response))
    {
        case EXALT_DBUS_RESPONSE_IFACE_UP_IS:
            boolean = exalt_dbus_response_is_get(response);
            if(boolean)
                e_widget_button_label_set(inst->wired_basic.btn, D_("Deactivate the interface"));
            else
                e_widget_button_label_set(inst->wired_basic.btn, D_("Activate the interface"));
            inst->wired_basic.iface->is_up = boolean;
            if_wired_dialog_basic_icon_update(inst);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_LINK_IS:
            boolean = exalt_dbus_response_is_get(response);
            inst->wired_basic.iface->is_link = boolean;
            if_wired_dialog_basic_icon_update(inst);
            break;
        default: break;
    }

    
}


void if_wired_dialog_basic_icon_update(Instance *inst)
{
    char buf[1024];
    char* group;

    if(!inst->wired_basic.dialog)
        return ;

    if(!inst->wired_basic.iface->is_link)
        edje_object_signal_emit(inst->wired_basic.icon,"notLink","exalt");
    else if(!inst->wired_basic.iface->is_up)
        edje_object_signal_emit(inst->wired_basic.icon,"notActivate","exalt");
    else
        edje_object_signal_emit(inst->wired_basic.icon,"default","exalt");
}

void if_wired_dialog_basic_cb_del(E_Win *win)
{
    E_Dialog *dialog;
    Instance *inst;

    dialog = win->data;
    inst = dialog->data;
    if_wired_dialog_basic_hide(inst);
}

void if_wired_dialog_basic_cb_close(void *data, E_Dialog *dialog)
{
    Instance* inst = data;
    if_wired_dialog_basic_hide(inst);
}

void if_wired_dialog_basic_cb_activate(void *data, void*data2)
{
    Instance *inst = data;

    if(inst->wired_basic.iface->is_up)
    {
        if_wired_dialog_basic_cb_deactivate(inst,NULL);
        return ;
    }

    //else apply a new configuration

    Exalt_Connection* conn = exalt_conn_new();

    exalt_conn_wireless_set(conn,0);
    exalt_conn_mode_set(conn,EXALT_DHCP);

    exalt_dbus_eth_conn_apply(inst->conn,inst->wired_basic.iface->iface,conn);
    exalt_conn_free(&conn);
}

void if_wired_dialog_basic_cb_deactivate(void *data, void*data2)
{
    Instance *inst = data;
    exalt_dbus_eth_down(inst->conn, inst->wired_basic.iface->iface);
}
