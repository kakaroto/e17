/*
 * =====================================================================================
 *
 *       Filename:  popup.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  12/21/2008 05:39:07 PM UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "e_mod_main.h"

 void
popup_create(Instance* inst)
{
    Evas_Object *base, *ilist, *button, *o;
    Evas *evas;
    Evas_Coord mw, mh;

    inst->popup = e_gadcon_popup_new(inst->gcc, NULL /*popup_cb_resize*/ );
    evas = inst->popup->win->evas;

    edje_freeze();

    base = e_widget_table_add(evas, 0);

    o = edje_object_add(evas);
    e_theme_edje_object_set(o, "base/theme/modules/exalt",
            "e/modules/exalt/network");
    edje_object_size_min_get(o, &mw, &mh);
    if ((mw < 1) || (mh < 1)) edje_object_size_min_calc(o, &mw, &mh);
    if (mw < 20) mw = 20;
    if (mh < 20) mh = 20;
    evas_object_del(o);
    ilist = e_widget_ilist_add(evas, mw, mh, NULL);
    inst->popup_ilist_obj = ilist;

    e_widget_ilist_freeze(ilist);

    e_widget_ilist_go(ilist);
    e_widget_ilist_thaw(ilist);

    e_widget_min_size_set(ilist, 240, 320);
    e_widget_table_object_append(base, ilist,
            0, 0, 1, 1, 1, 1, 1, 1);

    button = e_widget_button_add(evas, D_("Settings"), NULL, popup_cb_setup,
            inst, NULL);
    e_widget_table_object_append(base, button,
            0, 1, 1, 1, 0, 0, 0, 0);

    edje_thaw();

    e_gadcon_popup_content_set(inst->popup, base);
}

void
popup_cb_setup(void *data, void *data2)
{
    Instance *inst;

    inst = data;
    popup_hide(inst);
}


 void
popup_iface_add(Instance* inst, char* iface, Iface_Type iface_type)
{
    Evas_Object *icon;
    Evas_Object *header_icon;
    Popup_Elt* elt;
    char buf[1024];

    if(!inst->popup_ilist_obj)
        return;

    switch(iface_type)
    {
        case IFACE_WIRED:
            snprintf(buf,1024,"%s (%s)",D_("Wired interface"),iface);
            break;
        case IFACE_WIRELESS:
            snprintf(buf,1024,"%s (%s)",D_("Wireless interface"),iface);
        break;
    }

    header_icon = edje_object_add(evas_object_evas_get(inst->popup_ilist_obj));
    e_theme_edje_object_set(icon, "base/theme/modules/exalt",
            "e/modules/exalt/network");
    e_widget_ilist_header_append(inst->popup_ilist_obj, header_icon, buf);

    elt = calloc(1,sizeof(Popup_Elt));
    elt->inst = inst;
    elt->iface = iface;
    elt->type = POPUP_IP;
    elt->iface_type = iface_type;
    elt->header_icon = header_icon;
    icon = edje_object_add(evas_object_evas_get(inst->popup_ilist_obj));
    e_widget_ilist_append(inst->popup_ilist_obj, icon, D_("No IP address"),
            popup_cb_ifnet_sel , elt, NULL);

    elt = calloc(1,sizeof(Popup_Elt));
    elt->inst = inst;
    elt->iface = iface;
    elt->type = POPUP_MANAGE;
    elt->iface_type = iface_type;
    elt->header_icon = header_icon;
    icon = edje_object_add(evas_object_evas_get(inst->popup_ilist_obj));
    e_widget_ilist_append(inst->popup_ilist_obj, icon, D_("Manage this interface"),
            popup_cb_ifnet_sel, elt, NULL);
    evas_object_show(icon);

    exalt_dbus_eth_ip_get(inst->conn,iface);
    exalt_dbus_eth_up_is(inst->conn,iface);
    exalt_dbus_eth_link_is(inst->conn,iface);
}

void
popup_cb_ifnet_sel(void *data)
{
    Popup_Elt *elt = data;
    Instance* inst = elt->inst;

    switch(elt->iface_type)
    {
        case IFACE_WIRED:
            if_wired_dialog_show(inst,elt->iface);
            break;
        case IFACE_WIRELESS:
                break;
    }
}

 void
popup_ip_update(Instance* inst, char* iface, char* ip)
{
    int i;
    char buf[1024];
    if(!inst->popup_ilist_obj || !iface)
        return;

    if(!ip)
        snprintf(buf,1024, D_("No IP address"));
    else
        snprintf(buf,1024,D_("IP address: %s"),ip);

    for(i=0;i<e_widget_ilist_count(inst->popup_ilist_obj);i++)
    {
        Popup_Elt* elt = e_widget_ilist_nth_data_get(inst->popup_ilist_obj,i);
        if(elt && elt->type == POPUP_IP && elt->iface
                && strcmp(elt->iface,iface)==0)
        {
            e_widget_ilist_nth_label_set(inst->popup_ilist_obj,i,buf);
            break;
        }
    }


    e_widget_ilist_go(inst->popup_ilist_obj);
    e_widget_ilist_thaw(inst->popup_ilist_obj);
}

void popup_up_update(Instance* inst, char* iface, int is_up)
{
    int i;
    for(i=0;i<e_widget_ilist_count(inst->popup_ilist_obj);i++)
    {
        Popup_Elt* elt = e_widget_ilist_nth_data_get(inst->popup_ilist_obj,i);
        if(elt && elt->type == POPUP_MANAGE && elt->iface
                && strcmp(elt->iface,iface)==0)
        {
            elt->is_up = is_up;
            break;
        }
    }
    popup_icon_update(inst,iface);
}

void popup_link_update(Instance* inst, char* iface, int is_link)
{
    int i;
    for(i=0;i<e_widget_ilist_count(inst->popup_ilist_obj);i++)
    {
        Popup_Elt* elt = e_widget_ilist_nth_data_get(inst->popup_ilist_obj,i);
        if(elt && elt->type == POPUP_MANAGE && elt->iface
                && strcmp(elt->iface,iface)==0)
        {
            elt->is_link = is_link;
            break;
        }
    }
    popup_icon_update(inst,iface);
}

void popup_icon_update(Instance* inst, char* iface)
{
    int i;
    char* group;
    char buf[1024];

    for(i=0;i<e_widget_ilist_count(inst->popup_ilist_obj);i++)
    {
        Popup_Elt* elt = e_widget_ilist_nth_data_get(inst->popup_ilist_obj,i);
        if(elt && elt->type == POPUP_MANAGE && elt->iface
                && strcmp(elt->iface,iface)==0)
        {
            switch(elt->iface_type)
            {
                case IFACE_WIRED:
                    if(elt->is_link && elt->is_up)
                        group ="modules/exalt/icons/ethernet";
                    else
                        group ="modules/exalt/icons/ethernet_not_activate_link";
                    break;
                case IFACE_WIRELESS:
                    if(elt->is_link && elt->is_up)
                        group ="modules/exalt/icons/wireless";
                    else
                        group ="modules/exalt/icons/wireless_not_activate_link";
                    break;
            }

            snprintf(buf,1024,"%s/e-module-exalt.edj",exalt_conf->module->dir);
            edje_object_file_set(elt->header_icon,buf,group);
            break;
        }
    }
}

void popup_update(Instance* inst, Exalt_DBus_Response* response)
{
    const char* iface;

    if(!inst->popup_ilist_obj)
        return;

    switch(exalt_dbus_response_type_get(response))
    {
        case EXALT_DBUS_RESPONSE_IFACE_WIRED_LIST:
            {
                Ecore_List* l = exalt_dbus_response_list_get(response);
                ecore_list_first_goto(l);
                while( (iface=ecore_list_next(l)) )
                    popup_iface_add(inst,strdup(iface),IFACE_WIRED);
            }
            break;
        case EXALT_DBUS_RESPONSE_IFACE_WIRELESS_LIST:
            {
                Ecore_List* l = exalt_dbus_response_list_get(response);
                ecore_list_first_goto(l);
                while( (iface=ecore_list_next(l)) )
                    popup_iface_add(inst,strdup(iface),IFACE_WIRELESS);
            }
            break;
        case EXALT_DBUS_RESPONSE_IFACE_IP_GET:
            popup_ip_update(inst,exalt_dbus_response_iface_get(response),
                    exalt_dbus_response_address_get(response));
            break;
        case EXALT_DBUS_RESPONSE_IFACE_UP_IS:
            popup_up_update(inst,exalt_dbus_response_iface_get(response),
                    exalt_dbus_response_is_get(response));
            break;
        case EXALT_DBUS_RESPONSE_IFACE_LINK_IS:
            popup_link_update(inst,exalt_dbus_response_iface_get(response),
                    exalt_dbus_response_is_get(response));
            break;
        default: ;
    }

    e_widget_ilist_go(inst->popup_ilist_obj);
    e_widget_ilist_thaw(inst->popup_ilist_obj);
}


void popup_show(Instance* inst)
{
    if(!inst->popup)
        popup_create(inst);
    e_gadcon_popup_show(inst->popup);
    exalt_dbus_eth_list_get(inst->conn);
    exalt_dbus_wireless_list_get(inst->conn);
}

void popup_hide(Instance *inst)
{
    if (inst->popup)
    {
        e_object_del(E_OBJECT(inst->popup));
        inst->popup = NULL;
        inst->popup_ilist_obj = NULL;
    }
}


