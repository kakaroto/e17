/*
 * =====================================================================================
 *
 *       Filename:  hover_wired.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  12/02/09 18:22:09 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */


#include "hover_wired.h"
#include "config.h"

void hover_wired_create()
{
    Evas_Object* fr,*bx,*table,*lbl, * bt;

    hover_wired.iface = NULL;

    hover_wired.hover = elm_hover_add(win);
    elm_hover_style_set(hover_wired.hover,"popout");
    elm_hover_parent_set(hover_wired.hover, pnl_right.background);
    elm_hover_target_set(hover_wired.hover, pnl_right.btn_wired);


    hover_wired.icon = elm_icon_add(win);
    elm_icon_scale_set(hover_wired.icon, 0,0);
    elm_icon_file_set(hover_wired.icon, ICONS_ETHERNET_ACTIVATE_SMALL ,NULL);


    hover_wired.btn_title = elm_button_add(win);
    elm_button_icon_set(hover_wired.btn_title, hover_wired.icon);
    elm_hover_content_set(hover_wired.hover, "middle", hover_wired.btn_title);

    table = elm_table_add(win);
    elm_win_resize_object_add(win, table);
    evas_object_size_hint_weight_set(table, 1.0, 1.0);
    evas_object_show(table);

    fr = elm_frame_add(win);
    elm_frame_label_set(fr, D_("Informations."));

    lbl = elm_label_add(win);
    elm_table_pack(table, lbl, 0, 0, 1, 1);
    elm_label_label_set(lbl,"IP address: ");
    evas_object_show(lbl);

    lbl = elm_label_add(win);
    elm_table_pack(table, lbl, 0, 1, 1, 1);
    elm_label_label_set(lbl,"Netmask: ");
    evas_object_show(lbl);

    lbl = elm_label_add(win);
    elm_table_pack(table, lbl, 0, 2, 1, 1);
    elm_label_label_set(lbl,"Gateway: ");
    evas_object_show(lbl);



    hover_wired.lbl_ip = elm_label_add(win);
    elm_table_pack(table, hover_wired.lbl_ip, 1, 0, 1, 1);
    elm_label_label_set(hover_wired.lbl_ip,"test");
    evas_object_show(hover_wired.lbl_ip);

    hover_wired.lbl_netmask = elm_label_add(win);
    elm_table_pack(table, hover_wired.lbl_netmask, 1, 1, 1, 1);
    elm_label_label_set(hover_wired.lbl_netmask,"test");
    evas_object_show(hover_wired.lbl_netmask);

    hover_wired.lbl_gateway = elm_label_add(win);
    elm_table_pack(table, hover_wired.lbl_gateway, 1, 2, 1, 1);
    elm_label_label_set(hover_wired.lbl_gateway,"test");
    evas_object_show(hover_wired.lbl_gateway);

    elm_frame_content_set(fr, table);

    elm_hover_content_set(hover_wired.hover, "top", fr);

    bt = elm_button_add(win);
    elm_button_label_set(bt, "Fast Configuration");
    elm_hover_content_set(hover_wired.hover, "right", bt);


    bt = elm_button_add(win);
    elm_button_label_set(bt, "Configuration");
    elm_hover_content_set(hover_wired.hover, "bottom", bt);

}

void hover_wired_icon_update(char* iface, Exalt_DBus_Response* response)
{
    char* icon_path;

    int res= exalt_dbus_response_is_get(response);

    if(res)
        icon_path = ICONS_ETHERNET_ACTIVATE_SMALL;
    else
        icon_path = ICONS_ETHERNET_NOT_ACTIVATE_SMALL;
    elm_icon_file_set(hover_wired.icon, icon_path ,NULL);
}

void hover_wired_iface_set(char* iface)
{
    char buf[1024];
    if(!iface)
        return ;

    EXALT_FREE(hover_wired.iface);
    hover_wired.iface = strdup(iface);

    snprintf(buf,1024,D_("Wired interface (%s)"),iface);
    elm_button_label_set(hover_wired.btn_title,buf);

    exalt_dbus_eth_ip_get(conn,iface);
    exalt_dbus_eth_netmask_get(conn, iface);
    exalt_dbus_eth_gateway_get(conn, iface);
}

void hover_wired_response(Exalt_DBus_Response* response)
{
    char* iface = exalt_dbus_response_iface_get(response);
    if(iface_list_iface_type_get(iface)!=IFACE_WIRED)
        return ;
    if(!iface || !hover_wired.iface || strcmp(iface,hover_wired.iface)!=0)
        return 0;

    switch(exalt_dbus_response_type_get(response))
    {
        case EXALT_DBUS_RESPONSE_IFACE_IP_GET:
            elm_label_label_set(hover_wired.lbl_ip,
                    exalt_dbus_response_address_get(response));
            break;
        case EXALT_DBUS_RESPONSE_IFACE_NETMASK_GET:
            elm_label_label_set(hover_wired.lbl_netmask,
                    exalt_dbus_response_address_get(response));
            break;
        case EXALT_DBUS_RESPONSE_IFACE_GATEWAY_GET:
            elm_label_label_set(hover_wired.lbl_gateway,
                    exalt_dbus_response_address_get(response));
            break;
        case EXALT_DBUS_RESPONSE_IFACE_LINK_IS:
        case EXALT_DBUS_RESPONSE_IFACE_UP_IS:
            hover_wired_icon_update(iface, response);
        default: ;
    }
}
