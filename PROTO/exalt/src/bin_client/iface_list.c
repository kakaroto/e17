/*
 * =====================================================================================
 *
 *       Filename:  iface_list.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  11/02/2009 05:39:07 PM UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "iface_list.h"
#include "config.h"

void iface_list_create()
{
    iface_list.list = elm_list_add(win);
    evas_object_show(iface_list.list);
    evas_object_size_hint_weight_set(iface_list.list, 1.0, 1.0);
    evas_object_size_hint_align_set(iface_list.list, -1.0, -1.0);

    elm_table_pack(table, iface_list.list, 1, 1, 6, 10);
}

void iface_list_add(char* iface, Iface_Type iface_type)
{
    Evas_Object *icon;
    Iface_Elt* elt;
    char buf[PATH_MAX];

    switch(iface_type)
    {
        case IFACE_WIRED:
            snprintf(buf,sizeof(buf),"%s (%s)",D_("Wired interface"),iface);
            break;
        case IFACE_WIRELESS:
            snprintf(buf,sizeof(buf),"%s (%s)",D_("Wireless interface"),iface);
            break;
    }

    icon = elm_icon_add(win);
    elm_icon_scale_set(icon, 0,0);
    elm_icon_file_set(icon, ICONS_ETHERNET_ACTIVATE_SMALL ,NULL);

    evas_object_show(icon);

    elt = calloc(1,sizeof(Iface_Elt));
    elt->iface = iface;
    elt->type = iface_type;
    elt->icon = icon;
    elt->item = elm_list_item_append(iface_list.list, buf,icon,NULL,NULL, elt);
    evas_object_smart_callback_add(iface_list.list, "selected", iface_list_sel, elt);

    elm_list_go(iface_list.list);

    exalt_dbus_eth_up_is(conn,iface);
    exalt_dbus_eth_link_is(conn,iface);
}


void iface_list_sel(void *data, Evas_Object* o, void* event_info)
{
    Iface_Elt *elt = data;

    if(elt->type == IFACE_WIRED)
    {
        hover_wired_iface_set(elt->iface);
        evas_object_show(hover_wired.hover);
    }
    elm_list_item_selected_set(elt->item,0);
}

void iface_list_icon_update(char* iface, Exalt_DBus_Response* response)
{
    const Eina_List *list = elm_list_items_get(iface_list.list);
    const Eina_List* l;
    char* icon_path;

    int res= exalt_dbus_response_is_get(response);

    Elm_List_Item* item;
    EINA_LIST_FOREACH(list,l,item)
    {
        const Iface_Elt* elt = elm_list_item_data_get(item);
        if(elt && elt->iface
                && strcmp(elt->iface,iface)==0)
        {
            if(elt->type == IFACE_WIRED)
            {
                if(res)
                    icon_path = ICONS_ETHERNET_ACTIVATE_SMALL;
                else
                    icon_path = ICONS_ETHERNET_NOT_ACTIVATE_SMALL;
            }
            else // IFACE_WIRELESS
            {
                if(res)
                    icon_path = ICONS_WIRELESS_ACTIVATE_SMALL;
                else
                    icon_path = ICONS_WIRELESS_NOT_ACTIVATE_SMALL;
            }
            elm_icon_file_set(elt->icon, icon_path ,NULL);
            break;
        }
    }
}

void iface_list_response(Exalt_DBus_Response* response)
{
    const char* iface;

    switch(exalt_dbus_response_type_get(response))
    {
        case EXALT_DBUS_RESPONSE_IFACE_WIRED_LIST:
            {
                Ecore_List* l = exalt_dbus_response_list_get(response);
                ecore_list_first_goto(l);
                while( (iface=ecore_list_next(l)) )
                    iface_list_add(strdup(iface),IFACE_WIRED);
            }
            break;
        case EXALT_DBUS_RESPONSE_IFACE_WIRELESS_LIST:
            {
                Ecore_List* l = exalt_dbus_response_list_get(response);
                ecore_list_first_goto(l);
                while( (iface=ecore_list_next(l)) )
                    iface_list_add(strdup(iface),IFACE_WIRELESS);
            }
            break;
        case EXALT_DBUS_RESPONSE_IFACE_LINK_IS:
        case EXALT_DBUS_RESPONSE_IFACE_UP_IS:
            iface_list_icon_update(exalt_dbus_response_iface_get(response),
                    response);
            break;
        default: ;
    }
}

Iface_Type iface_list_iface_type_get(char* iface)
{
    const Eina_List *list = elm_list_items_get(iface_list.list);
    const Eina_List* l;

    Elm_List_Item* item;
    EINA_LIST_FOREACH(list,l,item)
    {
        const Iface_Elt* elt = elm_list_item_data_get(item);
        if(elt && elt->iface
                && strcmp(elt->iface,iface)==0)
            return elt->type;
    }
    return IFACE_UNKNOWN;
}


