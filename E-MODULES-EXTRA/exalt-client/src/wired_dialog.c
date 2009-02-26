/*
 * =====================================================================================
 *
 *       Filename:  wired_dialog.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  12/23/2008 10:15:31 AM UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "e_mod_main.h"

void if_wired_dialog_init(Instance* inst)
{
    inst->wired.iface = NULL;
}

void if_wired_dialog_create(Instance* inst)
{
    Evas* evas;
    Evas_Object *list, *flist, *o, *lbl;
    E_Radio_Group* rg;
    int mh,mw;
    char buf[4096];

    inst->wired.dialog = e_dialog_new(inst->gcc->gadcon->zone->container, "e", "exalt_wired_dialog");
    e_dialog_title_set(inst->wired.dialog, D_("Wired Connection Settings"));
    inst->wired.dialog->data = inst;
    inst->wired.dialog->win->data = inst;

    evas = e_win_evas_get(inst->wired.dialog->win);
    list = e_widget_list_add(evas, 0, 0);

    flist = e_widget_frametable_add(evas, D_("Wired interface"), 0);

    inst->wired.icon = edje_object_add(evas);
    snprintf(buf,1024,"%s/e-module-exalt.edj",exalt_conf->module->dir);
    edje_object_file_set(inst->wired.icon, buf,"modules/exalt/icons/wired");
    evas_object_show(inst->wired.icon);
    o = e_widget_image_add_from_object(evas,inst->wired.icon,40,40);
    e_widget_frametable_object_append(flist, o, 0, 0, 1, 1, 1, 0, 1, 0);

    inst->wired.btn_activate = e_widget_button_add(evas,D_("Activate"),NULL,if_wired_dialog_cb_activate,inst,NULL);
    e_widget_frametable_object_append(flist, inst->wired.btn_activate, 1, 0, 1, 1, 1, 0, 1, 0);

    inst->wired.btn_deactivate = e_widget_button_add(evas,D_("Deactivate"),NULL,if_wired_dialog_cb_deactivate,inst,NULL);
    e_widget_frametable_object_append(flist, inst->wired.btn_deactivate, 2, 0, 1, 1, 1, 0, 1, 0);

    rg = e_widget_radio_group_new(&(inst->wired.dhcp));
    o = e_widget_radio_add(evas, D_("DHCP (Dynamic host configuration protocol)"), 0, rg);
    evas_object_smart_callback_add(o, "changed", if_wired_dialog_cb_dhcp, inst);
    e_widget_frametable_object_append(flist, o, 0, 1, 3, 1, 1, 0, 1, 0);
    inst->wired.radio_dhcp = o;

    o = e_widget_radio_add(evas, D_("Static"), 1, rg);
    evas_object_smart_callback_add(o, "changed", if_wired_dialog_cb_dhcp, inst);
    e_widget_frametable_object_append(flist, o, 0, 2, 3, 1, 1, 0, 1, 0);
    inst->wired.radio_static = o;

    lbl = e_widget_label_add(evas,D_("IP address: "));
    e_widget_frametable_object_append(flist, lbl, 0, 3, 2, 1, 1, 0, 1, 0);
    inst->wired.entry_ip = e_widget_entry_add(evas,&(inst->wired.ip),if_wired_dialog_cb_entry,inst,NULL);
    e_widget_frametable_object_append(flist, inst->wired.entry_ip, 2, 3, 1, 1, 1, 0, 1, 0);

    lbl = e_widget_label_add(evas,D_("Netmask address: "));
    e_widget_frametable_object_append(flist, lbl, 0, 4, 2, 1, 1, 0, 1, 0);
    inst->wired.entry_netmask = e_widget_entry_add(evas,&(inst->wired.netmask),if_wired_dialog_cb_entry,inst,NULL);
    e_widget_frametable_object_append(flist, inst->wired.entry_netmask, 2, 4, 1, 1, 1, 0, 1, 0);

    lbl = e_widget_label_add(evas,D_("Gateway address: "));
    e_widget_frametable_object_append(flist, lbl, 0, 5, 2, 1, 1, 0, 1, 0);
    inst->wired.entry_gateway = e_widget_entry_add(evas,&(inst->wired.gateway),if_wired_dialog_cb_entry,inst,NULL);
    e_widget_frametable_object_append(flist, inst->wired.entry_gateway, 2, 5, 1, 1, 1, 0, 1, 0);

    lbl = e_widget_label_add(evas,D_("Command: "));
    e_widget_frametable_object_append(flist, lbl, 0, 6, 2, 1, 1, 0, 1, 0);
    inst->wired.entry_cmd = e_widget_entry_add(evas,&(inst->wired.cmd),if_wired_dialog_cb_entry,inst,NULL);
    e_widget_frametable_object_append(flist, inst->wired.entry_cmd, 2, 6, 1, 1, 1, 0, 1, 0);


    e_widget_list_object_append(list, flist, 1, 0, 0.5);

    e_widget_min_size_get(list, &mw, &mh);
    e_dialog_content_set(inst->wired.dialog, list, mw, mh);

    e_win_delete_callback_set(inst->wired.dialog->win, if_wired_dialog_cb_del);

    e_dialog_button_add(inst->wired.dialog, D_("OK"), NULL, if_wired_dialog_cb_ok, inst);
    e_dialog_button_add(inst->wired.dialog, D_("Apply"), NULL, if_wired_dialog_cb_apply, inst);
    e_dialog_button_add(inst->wired.dialog, D_("Cancel"), NULL, if_wired_dialog_cb_cancel, inst);
    e_dialog_button_focus_num(inst->wired.dialog, 1);
    e_win_centered_set(inst->wired.dialog->win, 1);
}

void if_wired_dialog_show(Instance* inst)
{
    if(!inst->wired.dialog)
        if_wired_dialog_create(inst);
    e_dialog_show(inst->wired.dialog);
}

void if_wired_dialog_set(Instance *inst, Popup_Elt* iface)
{
    if(inst->wired.iface)
    {
        inst->wired.iface->nb_use--;
        if(inst->wired.iface->nb_use<=0)
            popup_elt_free(inst->wired.iface);
    }

    inst->wired.iface = iface;
    iface->nb_use++;

    exalt_dbus_eth_ip_get(inst->conn,iface->iface);
    exalt_dbus_eth_gateway_get(inst->conn,iface->iface);
    exalt_dbus_eth_netmask_get(inst->conn,iface->iface);
    exalt_dbus_eth_command_get(inst->conn,iface->iface);
    exalt_dbus_eth_dhcp_is(inst->conn,iface->iface);
    exalt_dbus_eth_up_is(inst->conn,iface->iface);
    exalt_dbus_eth_link_is(inst->conn,iface->iface);
}

void if_wired_dialog_hide(Instance *inst)
{
    if(inst->wired.dialog)
    {
        e_object_del(E_OBJECT(inst->wired.dialog));
        inst->wired.dialog=NULL;
        if(inst->wired.iface)
        {
            inst->wired.iface->nb_use--;
            if(inst->wired.iface->nb_use<=0)
            {
                popup_elt_free(inst->wired.iface);
            }
            inst->wired.iface = NULL;
        }
    }
}

void if_wired_dialog_update(Instance* inst,Exalt_DBus_Response *response)
{
    char* string;
    int boolean;
    if(!inst->wired.dialog)
        return ;

    string = exalt_dbus_response_iface_get(response);
    if(!inst->wired.iface->iface || !string || !strcmp(inst->wired.iface->iface,string)==0)
        return;

    switch(exalt_dbus_response_type_get(response))
    {
        case EXALT_DBUS_RESPONSE_IFACE_IP_GET:
            string = exalt_dbus_response_address_get(response);
            e_widget_entry_text_set(inst->wired.entry_ip,string);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_NETMASK_GET:
            string = exalt_dbus_response_address_get(response);
            e_widget_entry_text_set(inst->wired.entry_netmask,string);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_GATEWAY_GET:
            string = exalt_dbus_response_address_get(response);
            e_widget_entry_text_set(inst->wired.entry_gateway,string);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_CMD_GET:
            string = exalt_dbus_response_string_get(response);
            e_widget_entry_text_set(inst->wired.entry_cmd,string);
            break;

        case EXALT_DBUS_RESPONSE_IFACE_DHCP_IS:
            boolean = exalt_dbus_response_is_get(response);
            if(boolean)
                e_widget_radio_toggle_set(inst->wired.radio_dhcp,1);
            else
                e_widget_radio_toggle_set(inst->wired.radio_static,1);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_UP_IS:
            boolean = exalt_dbus_response_is_get(response);
            e_widget_disabled_set(inst->wired.btn_activate,boolean);
            e_widget_disabled_set(inst->wired.btn_deactivate,!boolean);
            inst->wired.iface->is_up = boolean;
            if_wired_dialog_icon_update(inst);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_LINK_IS:
            boolean = exalt_dbus_response_is_get(response);
            inst->wired.iface->is_link = boolean;
            if_wired_dialog_icon_update(inst);
            break;

        default: break;
    }
    if_wired_disabled_update(inst);
}


void if_wired_dialog_icon_update(Instance *inst)
{
    char buf[1024];
    char* group;

    if(!inst->wired.dialog)
        return ;

    if(!inst->wired.iface->is_link)
        edje_object_signal_emit(inst->wired.icon,"notLink","exalt");
    else if(!inst->wired.iface->is_up)
        edje_object_signal_emit(inst->wired.icon,"notActivate","exalt");
    else
        edje_object_signal_emit(inst->wired.icon,"default","exalt");

}

void if_wired_disabled_update(Instance *inst)
{
    int disabled;
    int is_ip;

    if(!inst->wired.dialog)
        return;

    e_widget_disabled_set(inst->wired.entry_cmd,0);
    disabled = 0;
    if(!e_widget_disabled_get(inst->wired.btn_activate))
    {
        e_widget_disabled_set(inst->wired.radio_dhcp,1);
        e_widget_disabled_set(inst->wired.radio_static,1);

        e_widget_disabled_set(inst->wired.entry_ip,1);
        e_widget_disabled_set(inst->wired.entry_netmask,1);
        e_widget_disabled_set(inst->wired.entry_gateway,1);

        e_dialog_button_disable_num_set(inst->wired.dialog,0,1);
        e_dialog_button_disable_num_set(inst->wired.dialog,1,1);

        return ;
    }

    e_widget_disabled_set(inst->wired.radio_dhcp,0);
    e_widget_disabled_set(inst->wired.radio_static,0);

    if(inst->wired.dhcp == 0)
        disabled = 1;
    else
        disabled = 0;

    e_widget_disabled_set(inst->wired.entry_ip,disabled);
    e_widget_disabled_set(inst->wired.entry_netmask,disabled);
    e_widget_disabled_set(inst->wired.entry_gateway,disabled);

    e_dialog_button_disable_num_set(inst->wired.dialog,0,0);
    e_dialog_button_disable_num_set(inst->wired.dialog,1,0);
    if(inst->wired.dhcp != 0)
    {
        const char* string;
        string = e_widget_entry_text_get(inst->wired.entry_ip);
        is_ip = exalt_is_address(string);

        if(is_ip)
        {
            string = e_widget_entry_text_get(inst->wired.entry_netmask);
            is_ip = exalt_is_address(string);
        }

        if(is_ip)
        {
            string = e_widget_entry_text_get(inst->wired.entry_gateway);
            is_ip = exalt_is_address(string);
        }
        e_dialog_button_disable_num_set(inst->wired.dialog,0,!is_ip);
        e_dialog_button_disable_num_set(inst->wired.dialog,1,!is_ip);
    }
}

void if_wired_dialog_cb_dhcp(void *data, Evas_Object *obj, void *event_info)
{
   Instance *inst;

   inst = data;

   if_wired_disabled_update(inst);
}

void if_wired_dialog_cb_del(E_Win *win)
{
    Instance *inst;

    inst = win->data;
    if_wired_dialog_hide(inst);
}

void if_wired_dialog_cb_cancel(void *data, E_Dialog *dialog)
{
    Instance* inst = data;
    if_wired_dialog_hide(inst);
}

void if_wired_dialog_cb_ok(void *data, E_Dialog *dialog)
{
    Instance* inst = data;
    if_wired_dialog_cb_apply(data,dialog);
    if_wired_dialog_hide(inst);
}

void if_wired_dialog_cb_apply(void *data, E_Dialog *dialog)
{
    Instance* inst = data;

    Exalt_Connection* conn = exalt_conn_new();

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
}

void if_wired_dialog_cb_entry(void *data, void* data2)
{
    Instance* inst = data;
    if_wired_disabled_update(inst);
}

void if_wired_dialog_cb_activate(void *data, void*data2)
{
    Instance *inst = data;
    exalt_dbus_eth_up(inst->conn, inst->wired.iface->iface);
}

void if_wired_dialog_cb_deactivate(void *data, void*data2)
{
    Instance *inst = data;
    exalt_dbus_eth_down(inst->conn, inst->wired.iface->iface);
}
