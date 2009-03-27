/*
 * =====================================================================================
 *
 *       Filename:  iface_list.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  24/03/09 21:38:14 CET
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

static Elm_Genlist_Item_Class itc1;

/* Genlist callback functions */
char *gl_label_get(const void *data, Evas_Object *obj, const char *part)
{
    Iface_List_Elt * elt = (Iface_List_Elt*)data;
    if(elt->type==ITEM_IFACE)
    {
        char buf[1024];
        snprintf(buf,1024,"%s (%s)",elt->iface,elt->ip);
        return strdup(buf);
    }
    return NULL;
}

Evas_Object *gl_icon_get(const void *data, Evas_Object *obj, const char *part)
{
    Iface_List_Elt * elt = (Iface_List_Elt*)data;

    if (strcmp(part, "elm.swallow.icon")==0)
    {
        Evas_Object *icon;
        char *icon_file;

        icon = elm_icon_add(obj);
        switch(elt->iface_type)
        {
            case IFACE_WIRED:
                if(elt->is_link && elt->is_up)
                    icon_file =  ICONS_ETHERNET_ACTIVATE_SMALL;
                else
                    icon_file = ICONS_ETHERNET_NOT_ACTIVATE_SMALL;
                break;
            case IFACE_WIRELESS:
                if(elt->is_link && elt->is_up)
                    icon_file =  ICONS_WIRELESS_ACTIVATE_SMALL;
                else
                    icon_file = ICONS_WIRELESS_NOT_ACTIVATE_SMALL;
                break;
        }

        elm_icon_file_set(icon, icon_file, NULL);
        elm_icon_scale_set(icon, 0, 0);
        evas_object_show(icon);

        return icon;
    }
    else
        return NULL;
}

Evas_Bool gl_state_get(const void *data, Evas_Object *obj, const char *part)
{
    return 0;
}

void gl_del(const void *data, Evas_Object *obj)
{
}

static void gl_sel(void *data, Evas_Object *obj, void *event_info)
{
    printf("sel item data [%p] on genlist obj [%p], item pointer [%p]\n", data, obj, event_info);
}
/* End Genlist callback functions */


Evas_Object* iface_list_new()
{
    Evas_Object *gl = elm_genlist_add(win);
    evas_object_size_hint_align_set(gl, -1.0, -1.0);
    evas_object_size_hint_weight_set(gl, 1.0, 1.0);
    evas_object_show(gl);

    return gl;
}

void iface_list_add(Evas_Object *list, const char* iface, Iface_Type type)
{
    Iface_List_Elt *elt = calloc(1,sizeof(Iface_List_Elt));
    elt->type = ITEM_IFACE;
    elt->iface = strdup(iface);
    elt->iface_type = type;

    itc1.item_style     = "default";
    itc1.func.label_get = gl_label_get;
    itc1.func.icon_get  = gl_icon_get;
    itc1.func.state_get = gl_state_get;
    itc1.func.del       = gl_del;

    elt->item = elm_genlist_item_append(list, &itc1,
            (void *)elt/* item data */,
            NULL/* parent */,
            ELM_GENLIST_ITEM_NONE,
            gl_sel/* func */,
            (void *)elt/* func data */);

    exalt_dbus_eth_link_is(conn,iface);
    exalt_dbus_eth_up_is(conn,iface);
    exalt_dbus_eth_ip_get(conn,iface);
}

Elm_Genlist_Item* iface_list_get_elt_from_name(Evas_Object *list,char* iface)
{
    Elm_Genlist_Item *item;

    for(item = elm_genlist_first_item_get(list);
            item != NULL;
            item = elm_genlist_item_next_get(item))
    {
        Iface_List_Elt* elt = elm_genlist_item_data_get(item);
        if(elt->type == ITEM_IFACE && strcmp(elt->iface, iface)==0)
            return item;
    }
    return NULL;
}

void iface_list_response(Evas_Object *list, Exalt_DBus_Response* response)
{
    const char* iface;
    Elm_Genlist_Item* item;
    Iface_List_Elt *elt;

    switch(exalt_dbus_response_type_get(response))
    {
        case EXALT_DBUS_RESPONSE_IFACE_WIRED_LIST:
            {
                Eina_List *l2;
                Eina_List* l = exalt_dbus_response_list_get(response);
                EINA_LIST_FOREACH(l,l2,iface)
                    iface_list_add(list,iface,IFACE_WIRED);
            }
            break;
        case EXALT_DBUS_RESPONSE_IFACE_WIRELESS_LIST:
            {
                Eina_List *l2;
                Eina_List* l = exalt_dbus_response_list_get(response);
                EINA_LIST_FOREACH(l,l2,iface)
                    iface_list_add(list,iface,IFACE_WIRELESS);
            }
            break;
        case EXALT_DBUS_RESPONSE_IFACE_WIRELESS_IS:
            break;
        case EXALT_DBUS_RESPONSE_IFACE_IP_GET:
            item = iface_list_get_elt_from_name(list,
                    exalt_dbus_response_iface_get(response));
            if(!item)
                return ;
            elt =  elm_genlist_item_data_get(item);

            EXALT_FREE(elt->ip);
            elt->ip = strdup(exalt_dbus_response_address_get(response));
            elm_genlist_item_update(item);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_UP_IS:
            item = iface_list_get_elt_from_name(list,
                    exalt_dbus_response_iface_get(response));
            if(!item)
                return ;
            elt =  elm_genlist_item_data_get(item);

            elt->is_up = exalt_dbus_response_is_get(response);
            elm_genlist_item_update(item);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_LINK_IS:
            item = iface_list_get_elt_from_name(list,
                    exalt_dbus_response_iface_get(response));
            if(!item)
                return ;

            elt =  elm_genlist_item_data_get(item);

            elt->is_link = exalt_dbus_response_is_get(response);
            elm_genlist_item_update(item);
            break;
        default: ;
    }
}

void network_list_interval_get(Elm_Genlist_Item* list, const char* iface, int *id_first, int* id_last, Elm_Genlist_Item** first, Elm_Genlist_Item** last)
{
    int i=0;
    char buf[1024];

    *id_first = -1;
    *id_last = -1;

    *first = NULL;
    *last = NULL;

    Elm_Genlist_Item *item;

    for(item = elm_genlist_first_item_get(list);
            item != NULL;
            item = elm_genlist_item_next_get(item))
    {
        Iface_List_Elt* elt = elm_genlist_item_data_get(item);

        if(elt && elt->type == ITEM_IFACE && elt->iface
                && strcmp(elt->iface,iface)==0)
        {
            *id_first = i;
            *first = item;
            break;
        }
        i++;
    }

    i=*id_first+1;
    *id_last = *id_first;
    *last = *first;

    for(item = elm_genlist_item_next_get(item);
            item != NULL;
            item = elm_genlist_item_next_get(item))
    {
        Iface_List_Elt* elt = elm_genlist_item_data_get(item);

        if(elt && elt->type == ITEM_IFACE)
        {
            break;
        }
        else
        {
            *id_last = i;
            *last = item;
        }
        i++;
    }
}

