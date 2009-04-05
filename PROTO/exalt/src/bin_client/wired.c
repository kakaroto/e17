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

#include "wired.h"

Wired* wired_new(Evas_Object *win)
{
    Evas_Object *lbl, *table, *scroll;

    Wired *wired = calloc(1,sizeof(Wired));

    wired->frame = elm_frame_add(win);
    elm_frame_label_set(wired->frame,D_("Wired Connection Settings"));
    evas_object_show(wired->frame);

    table = elm_table_add(win);
    evas_object_size_hint_weight_set(table, 1.0, 1.0);
    elm_table_homogenous_set(table, 1);
    evas_object_show(table);

    wired->icon = elm_icon_add(win);
    elm_icon_file_set(wired->icon, DEFAULT_THEME,
            "exalt/icons/wired");
    evas_object_show(wired->icon);
    elm_icon_scale_set(wired->icon, 1, 1);
    evas_object_size_hint_align_set(wired->icon, -1.0, -1.0);
    evas_object_size_hint_weight_set(wired->icon, 1.0, 1.0);
    elm_table_pack(table,wired->icon,0, 0, 1, 1);


    wired->btn_deactivate = elm_button_add(win);
    elm_button_label_set(wired->btn_deactivate, D_("Deactivate"));
    evas_object_size_hint_weight_set(wired->btn_deactivate, 1.0, 1.0);
    evas_object_show(wired->btn_deactivate);
    evas_object_smart_callback_add(wired->btn_deactivate,
            "clicked", wired_cb_deactivate, wired);
    elm_table_pack(table, wired->btn_deactivate, 1, 0, 1, 1);

    wired->btn_activate = elm_button_add(win);
    elm_button_label_set(wired->btn_activate, D_("Activate"));
    evas_object_size_hint_weight_set(wired->btn_activate, 1.0, 1.0);
    evas_object_show(wired->btn_activate);
    evas_object_smart_callback_add(wired->btn_activate,
            "clicked", wired_cb_activate, wired);
    elm_table_pack(table, wired->btn_activate, 2, 0, 1, 1);



    wired->radio_dhcp = elm_radio_add(win);
    elm_radio_state_value_set(wired->radio_dhcp, 0);
    evas_object_size_hint_weight_set(wired->radio_dhcp, 1.0, 0.0);
    evas_object_size_hint_align_set(wired->radio_dhcp, -1.0, -1.0);
    elm_radio_label_set(wired->radio_dhcp, D_("DHCP (Dynamic Hots Configuration Protocol)"));
    evas_object_smart_callback_add(wired->radio_dhcp,
            "changed", wired_cb_dhcp, wired);
    evas_object_show(wired->radio_dhcp);
    elm_table_pack(table, wired->radio_dhcp, 0, 1, 3, 1);

    wired->radio_static = elm_radio_add(win);
    elm_radio_state_value_set(wired->radio_static, 1);
    evas_object_size_hint_weight_set(wired->radio_static, 1.0, 0.0);
    evas_object_size_hint_align_set(wired->radio_static, -1.0, -1.0);
    elm_radio_label_set(wired->radio_static, D_("Static"));
    evas_object_smart_callback_add(wired->radio_static,
            "changed", wired_cb_dhcp, wired);
    evas_object_show(wired->radio_static);
    elm_radio_group_add(wired->radio_static, wired->radio_dhcp);
    elm_table_pack(table, wired->radio_static, 0, 2, 3, 1);


    //
    lbl = elm_label_add(win);
    elm_label_label_set(lbl, D_("IP address: "));
    evas_object_show(lbl);
    elm_table_pack(table, lbl, 0, 3, 1, 1);

    scroll = elm_scroller_add(win);
    elm_scroller_content_min_limit(scroll, 0, 1);
    evas_object_size_hint_weight_set(scroll, 1.0, 0.0);
    evas_object_size_hint_align_set(scroll, -1.0, 0.5);
    evas_object_show(scroll);
    elm_table_pack(table, scroll, 1, 3, 2, 1);

    wired->entry_ip = elm_entry_add(win);
    elm_entry_single_line_set(wired->entry_ip, 1);
    evas_object_size_hint_weight_set(wired->entry_ip, 1.0, 1.0);
    evas_object_size_hint_align_set(wired->entry_ip, -1.0, -1.0);
    elm_entry_line_wrap_set(wired->entry_ip, 0);
    evas_object_show(wired->entry_ip);
    evas_object_smart_callback_add(wired->entry_ip,
            "changed", wired_cb_entry, wired);
    elm_scroller_content_set(scroll, wired->entry_ip);
    //


    //
    lbl = elm_label_add(win);
    elm_label_label_set(lbl, D_("Netmask address: "));
    evas_object_show(lbl);
    elm_table_pack(table, lbl, 0, 4, 1, 1);

    scroll = elm_scroller_add(win);
    elm_scroller_content_min_limit(scroll, 0, 1);
    evas_object_size_hint_weight_set(scroll, 1.0, 0.0);
    evas_object_size_hint_align_set(scroll, -1.0, 0.5);
    evas_object_show(scroll);
    elm_table_pack(table, scroll, 1, 4, 2, 1);

    wired->entry_netmask = elm_entry_add(win);
    evas_object_size_hint_weight_set(wired->entry_netmask, 1.0, 1.0);
    evas_object_size_hint_align_set(wired->entry_netmask, -1.0, -1.0);
    elm_entry_line_wrap_set(wired->entry_netmask, 0);
    evas_object_show(wired->entry_netmask);
    evas_object_smart_callback_add(wired->entry_netmask,
            "changed", wired_cb_entry, wired);
    elm_scroller_content_set(scroll, wired->entry_netmask);
    //

    //
    lbl = elm_label_add(win);
    elm_label_label_set(lbl, D_("Gateway address: "));
    evas_object_show(lbl);
    elm_table_pack(table, lbl, 0, 5, 1, 1);

    scroll = elm_scroller_add(win);
    elm_scroller_content_min_limit(scroll, 0, 1);
    evas_object_size_hint_weight_set(scroll, 1.0, 0.0);
    evas_object_size_hint_align_set(scroll, -1.0, 0.5);
    evas_object_show(scroll);
    elm_table_pack(table, scroll, 1, 5, 2, 1);

    wired->entry_gateway = elm_entry_add(win);
    evas_object_size_hint_weight_set(wired->entry_gateway, 1.0, 1.0);
    evas_object_size_hint_align_set(wired->entry_gateway, -1.0, -1.0);
    elm_entry_line_wrap_set(wired->entry_gateway, 0);
    evas_object_show(wired->entry_gateway);
    evas_object_smart_callback_add(wired->entry_gateway,
            "changed", wired_cb_entry, wired);
    elm_scroller_content_set(scroll, wired->entry_gateway);
    //

    //
    lbl = elm_label_add(win);
    elm_label_label_set(lbl, D_("Command: "));
    evas_object_show(lbl);
    elm_table_pack(table, lbl, 0, 6, 1, 1);

    scroll = elm_scroller_add(win);
    elm_scroller_content_min_limit(scroll, 0, 1);
    evas_object_size_hint_weight_set(scroll, 1.0, 0.0);
    evas_object_size_hint_align_set(scroll, -1.0, 0.5);
    evas_object_show(scroll);
    elm_table_pack(table, scroll, 1, 6, 2, 1);

    wired->entry_cmd = elm_entry_add(win);
    evas_object_size_hint_weight_set(wired->entry_cmd, 1.0, 1.0);
    evas_object_size_hint_align_set(wired->entry_cmd, -1.0, -1.0);
    elm_entry_line_wrap_set(wired->entry_cmd, 0);
    evas_object_show(wired->entry_cmd);
    evas_object_smart_callback_add(wired->entry_cmd,
            "changed", wired_cb_entry, wired);
    elm_scroller_content_set(scroll, wired->entry_cmd);
    //


    wired->btn_back = elm_button_add(win);
    elm_button_label_set(wired->btn_back, D_("Back"));
    evas_object_size_hint_weight_set(wired->btn_back, 1.0, 1.0);
    evas_object_show(wired->btn_back);
    evas_object_smart_callback_add(wired->btn_back,
            "clicked", wired_cb_back, wired);
    elm_table_pack(table, wired->btn_back, 0, 7, 1, 1);

    wired->btn_close = elm_button_add(win);
    elm_button_label_set(wired->btn_close, D_("Close"));
    evas_object_size_hint_weight_set(wired->btn_close, 1.0, 1.0);
    evas_object_show(wired->btn_close);
    evas_object_smart_callback_add(wired->btn_close,
            "clicked", wired_cb_close, wired);
    elm_table_pack(table, wired->btn_close, 1, 7, 1, 1);

    wired->btn_apply = elm_button_add(win);
    elm_button_label_set(wired->btn_apply, D_("Apply"));
    evas_object_size_hint_weight_set(wired->btn_apply, 1.0, 1.0);
    evas_object_show(wired->btn_apply);
    evas_object_smart_callback_add(wired->btn_apply,
            "clicked", wired_cb_apply, wired);
    elm_table_pack(table, wired->btn_apply, 2, 7, 1, 1);

    elm_frame_content_set(wired->frame,table);
    return wired;
}

void wired_set(Wired *wired, Iface_List_Elt* iface)
{
    if(wired->iface)
    {
        wired->iface->nb_use--;
        if(wired->iface->nb_use<=0)
            iface_list_elt_free(wired->iface);
    }

    wired->iface = iface;
    iface->nb_use++;

    exalt_dbus_eth_ip_get(conn,iface->iface);
    exalt_dbus_eth_gateway_get(conn,iface->iface);
    exalt_dbus_eth_netmask_get(conn,iface->iface);
    exalt_dbus_eth_command_get(conn,iface->iface);
    exalt_dbus_eth_dhcp_is(conn,iface->iface);
    exalt_dbus_eth_up_is(conn,iface->iface);
    exalt_dbus_eth_link_is(conn,iface->iface);
}

void wired_response(Wired *wired,Exalt_DBus_Response *response)
{
    char* string;
    int boolean;

    string = exalt_dbus_response_iface_get(response);

    if(!wired->iface)
        return ;
    if(!wired->iface->iface || !string
            || !strcmp(wired->iface->iface,string)==0)
        return;

    switch(exalt_dbus_response_type_get(response))
    {
        case EXALT_DBUS_RESPONSE_IFACE_IP_GET:
            string = exalt_dbus_response_address_get(response);
            elm_entry_entry_set(wired->entry_ip,string);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_NETMASK_GET:
            string = exalt_dbus_response_address_get(response);
            elm_entry_entry_set(wired->entry_netmask,string);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_GATEWAY_GET:
            string = exalt_dbus_response_address_get(response);
            elm_entry_entry_set(wired->entry_gateway,string);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_CMD_GET:
            string = exalt_dbus_response_string_get(response);
            elm_entry_entry_set(wired->entry_cmd,string);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_DHCP_IS:
            boolean = exalt_dbus_response_is_get(response);
            if(boolean)
                elm_radio_state_value_set(wired->radio_dhcp,0);
            else
                elm_radio_state_value_set(wired->radio_static,1);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_UP_IS:
            boolean = exalt_dbus_response_is_get(response);
            //TODO
            //e_widget_disabled_set(inst->wired.btn_activate,boolean);
            //e_widget_disabled_set(inst->wired.btn_deactivate,!boolean);
            wired->iface->is_up = boolean;
            wired_icon_update(wired);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_LINK_IS:
            boolean = exalt_dbus_response_is_get(response);
            wired->iface->is_link = boolean;
            wired_icon_update(wired);
            break;
        default: break;
    }
    wired_disabled_update(wired);
}




void wired_icon_update(Wired *wired)
{
    if(!wired->iface)
        return ;

    if(!wired->iface->is_link)
        elm_icon_file_set(wired->icon, DEFAULT_THEME,
                "exalt/icons/wired/notlink");
    else if(!wired->iface->is_up)
        elm_icon_file_set(wired->icon, DEFAULT_THEME,
                "exalt/icons/wired/notactivate");
    else
        elm_icon_file_set(wired->icon, DEFAULT_THEME,
                "exalt/icons/wired");

    evas_object_show(wired->icon);
    elm_icon_scale_set(wired->icon, 1, 1);
    evas_object_size_hint_align_set(wired->icon, -1.0, -1.0);
    evas_object_size_hint_weight_set(wired->icon, 1.0, 1.0);
}


void wired_disabled_update(Wired *wired)
{
    //int disabled;
    //int is_ip;

    if(!wired->iface)
        return;

    //currently we can disabled a widget in elementary
    //TODO
    /*e_widget_disabled_set(inst->wired.entry_cmd,0);
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
    */
}

void wired_cb_entry(void *data __UNUSED__, Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__)
{
    Wired *wired = (Wired*)data;
    wired_disabled_update(wired);
}

void wired_cb_dhcp(void *data __UNUSED__,
        Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__)
{
    Wired *wired = (Wired*)data;
    wired_disabled_update(wired);
}

void wired_cb_static(void *data __UNUSED__,
        Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__)
{
    Wired *wired = (Wired*)data;
    wired_disabled_update(wired);
}


// CALLBACKS BUTTONS

void wired_cb_back(void *data __UNUSED__, Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__)
{
    iface_list_promote(pager.iface_list);
}

void wired_cb_close(void *data __UNUSED__, Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__)
{
    ecore_main_loop_quit();
}

void wired_cb_apply(void *data __UNUSED__, Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__)
{
    Wired *wired = data;

    Exalt_Connection* c = exalt_conn_new();

    exalt_conn_wireless_set(c,0);
    if(elm_radio_value_get(wired->radio_dhcp) != 0)
    {
        exalt_conn_mode_set(c,EXALT_STATIC);
        exalt_conn_ip_set(c,
                elm_entry_entry_get(wired->entry_ip));
        exalt_conn_netmask_set(c,
                elm_entry_entry_get(wired->entry_netmask));
        exalt_conn_gateway_set(c,
                elm_entry_entry_get(wired->entry_gateway));
    }
    else
        exalt_conn_mode_set(c,EXALT_DHCP);
    exalt_conn_cmd_after_apply_set(c,
            elm_entry_entry_get(wired->entry_cmd));

    exalt_dbus_eth_conn_apply(conn,wired->iface->iface,c);
    exalt_conn_free(&c);
}

void wired_cb_activate(void *data,
        Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__)
{
    Wired *wired = (Wired*)data;
    exalt_dbus_eth_up(conn, wired->iface->iface);
}

void wired_cb_deactivate(void *data,
        Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__)
{
    Wired *wired = (Wired*)data;
    exalt_dbus_eth_down(conn, wired->iface->iface);
}

