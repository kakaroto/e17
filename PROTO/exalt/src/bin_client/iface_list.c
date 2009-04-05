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
#include "wired.h"

static Elm_Genlist_Item_Class itc1;

/* Genlist callback functions */
char *gl_label_get(const void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
    Iface_List_Elt * elt = (Iface_List_Elt*)data;
    if(elt->type==ITEM_IFACE)
    {
        char buf[1024];
        snprintf(buf,1024,"%s (%s)",elt->iface,elt->ip);
        return strdup(buf);
    }
    else
        return strdup(exalt_wireless_network_essid_get(elt->wn));
    return NULL;
}

Evas_Object *gl_icon_get(const void *data, Evas_Object *obj, const char *part)
{
    Iface_List_Elt * elt = (Iface_List_Elt*)data;

    if (strcmp(part, "elm.swallow.icon")==0)
    {
        Evas_Object *icon;
        icon = elm_icon_add(win);

        if(elt->type==ITEM_IFACE)
        {
            switch(elt->iface_type)
            {
                case IFACE_WIRED:
                    if(!elt->is_link)
                        elm_icon_file_set(icon, DEFAULT_THEME,
                                "exalt/icons/wired/notlink");
                    else if(!elt->is_up)
                        elm_icon_file_set(icon, DEFAULT_THEME,
                                "exalt/icons/wired/notactivate");
                    else
                        elm_icon_file_set(icon, DEFAULT_THEME,
                                "exalt/icons/wired");
                    break;
                case IFACE_WIRELESS:
                    if(!elt->is_link)
                        elm_icon_file_set(icon, DEFAULT_THEME,
                                "exalt/icons/wireless/notlink");
                    else if(!elt->is_up)
                        elm_icon_file_set(icon, DEFAULT_THEME,
                                "exalt/icons/wireless/notactivate");
                    else
                        elm_icon_file_set(icon, DEFAULT_THEME,
                                "exalt/icons/wireless");
                    break;
            }
        }
        else
        {
            if(exalt_wireless_network_encryption_is(elt->wn))
                elm_icon_file_set(icon, DEFAULT_THEME,
                        "exalt/icons/encryption");
            else
                elm_icon_file_set(icon, DEFAULT_THEME,
                        "exalt/icons/noencryption");

        }
        evas_object_size_hint_aspect_set(icon,
                EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
        elm_icon_scale_set(icon, 0, 0);
        evas_object_show(icon);

        return icon;
    }
    else
    {
        if(elt->type==ITEM_NETWORK)
        {
            Evas_Object *icon;

            icon = edje_object_add(evas_object_evas_get(obj));
            edje_object_file_set(icon, DEFAULT_THEME,
                    "exalt/icons/quality");
            //icon = elm_icon_add(obj);
            //elm_icon_file_set(icon, DEFAULT_THEME,
            //         "exalt/icons/quality");

            double pct =
                exalt_wireless_network_quality_get(elt->wn) / 100.0;

            edje_object_part_drag_value_set(icon,"drag",pct,0);

            //elm_icon_scale_set(icon, 0, 0);

            evas_object_show(icon);

            return icon;
        }
        else
            return NULL;
    }
}

Evas_Bool gl_state_get(const void *data __UNUSED__, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
    return 0;
}

void gl_del(const void *data __UNUSED__, Evas_Object *obj __UNUSED__)
{
}

static void gl_sel(void *data, Evas_Object *obj __UNUSED__,
        void *event_info __UNUSED__)
{
    Iface_List_Elt* elt = data;

    elm_genlist_item_selected_set(elt->item, 0);
    if(elt->type == ITEM_IFACE  && elt->iface_type == IFACE_WIRED)
    {
        elm_pager_content_promote(pager.pager, pager.p_wired);
        wired_set(pager.wired,elt);
    }
    else if(elt->type == ITEM_NETWORK)
    {
        elm_pager_content_promote(pager.pager, pager.p_wireless);
        wireless_set(pager.wireless, elt);
    }
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
    elt->nb_use++;

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

    if(type == IFACE_WIRELESS)
        exalt_dbus_wireless_scan(conn,elt->iface);
}

Elm_Genlist_Item* iface_list_get_elt_from_name(Evas_Object *list,const char* iface)
{
    Elm_Genlist_Item *item;

    for(item = elm_genlist_first_item_get(list);
            item != NULL;
            item = elm_genlist_item_next_get(item))
    {
        const Iface_List_Elt* elt = elm_genlist_item_data_get(item);
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
            elt =  (Iface_List_Elt*)elm_genlist_item_data_get(item);

            EXALT_FREE(elt->ip);
            const char* address = exalt_dbus_response_address_get(response);
            if(!address)
                address = D_("No address");
            elt->ip = strdup(address);
            elm_genlist_item_update(item);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_UP_IS:
            item = iface_list_get_elt_from_name(list,
                    exalt_dbus_response_iface_get(response));
            if(!item)
                return ;
            elt =  (Iface_List_Elt*)elm_genlist_item_data_get(item);

            elt->is_up = exalt_dbus_response_is_get(response);
            elm_genlist_item_update(item);
            break;
        case EXALT_DBUS_RESPONSE_IFACE_LINK_IS:
            item = iface_list_get_elt_from_name(list,
                    exalt_dbus_response_iface_get(response));
            if(!item)
                return ;

            elt =  (Iface_List_Elt*)elm_genlist_item_data_get(item);

            elt->is_link = exalt_dbus_response_is_get(response);
            elm_genlist_item_update(item);
            break;
        default: ;
    }
}

void iface_list_promote(Evas_Object *list)
{
    Elm_Genlist_Item *item;

    for(item = elm_genlist_first_item_get(list);
            item != elm_genlist_last_item_get(list);
            item = elm_genlist_item_next_get(item))
    {
        const Iface_List_Elt *elt = elm_genlist_item_data_get(item);
        if(elt->type == ITEM_IFACE
                && elt->iface_type == IFACE_WIRELESS)
        {
            exalt_dbus_wireless_scan(conn, elt->iface);
        }
    }
    elm_pager_content_promote(pager.pager, pager.p_list);
}

void network_list_interval_get(Evas_Object* list, const char* iface,
        Elm_Genlist_Item** first, Elm_Genlist_Item** last)
{
    int i=0;

    *first = NULL;
    *last = NULL;

    Elm_Genlist_Item *item;

    for(item = elm_genlist_first_item_get(list);
            item != NULL;
            item = elm_genlist_item_next_get(item))
    {
        Iface_List_Elt* elt = (Iface_List_Elt*) elm_genlist_item_data_get(item);

        if(elt && elt->type == ITEM_IFACE && elt->iface
                && strcmp(elt->iface,iface)==0)
        {
            *first = item;
            break;
        }
        i++;
    }

    *last = *first;

    for(item = elm_genlist_item_next_get(item);
            item != NULL;
            item = elm_genlist_item_next_get(item))
    {
        Iface_List_Elt* elt = (Iface_List_Elt*) elm_genlist_item_data_get(item);

        if(elt && elt->type == ITEM_IFACE)
        {
            break;
        }
        else
        {
            *last = item;
        }
        i++;
    }
}

void network_list_notify_scan(char* iface, Eina_List* networks, void* user_data )
{
    Evas_Object* list = user_data;
    Exalt_Wireless_Network* w;
    Eina_List *l;
    Elm_Genlist_Item *l_item;
    Elm_Genlist_Item* first, *last;
    Iface_List_Elt* elt;

    network_list_interval_get(list,iface,&first,&last);

    //init all networks as not found
    for(l_item = elm_genlist_first_item_get(list);
            l_item != NULL;
            l_item = elm_genlist_item_next_get(l_item))
    {
        elt = (Iface_List_Elt*)elm_genlist_item_data_get(l_item);
        elt->find = 0;
    }


    EINA_LIST_FOREACH(networks,l,w)
    {
        Elm_Genlist_Item* l2;
        Iface_List_Elt* elt_find=NULL;

        //search the network in the list
        const char* essid = exalt_wireless_network_essid_get(w);
        l2=first;
        do
        {

            Iface_List_Elt* elt;
            elt = (Iface_List_Elt*)elm_genlist_item_data_get(l2);
            if(elt->type==ITEM_NETWORK)
            {
                const char* essid_item =
                    exalt_wireless_network_essid_get(elt->wn);
                if(elt && essid_item && essid
                        && strcmp(essid_item,essid)==0)
                    elt_find = elt;
            }
        }while(l2!=last && !elt_find && (l2 = elm_genlist_item_next_get(l2)) );

        if(!elt_find)
        {
            //add a new network in the list
            Iface_List_Elt* elt;

            elt = calloc(1,sizeof(Iface_List_Elt));
            elt->iface = strdup(iface);
            elt->type = ITEM_NETWORK;
            elt->wn = w;
            elt->nb_use++;
            elt->find = 1;

            elt->item = elm_genlist_item_insert_after(list, &itc1,
                    elt, last,
                    ELM_GENLIST_ITEM_NONE,
                    gl_sel, elt);

            last = elt->item;
        }
        else
        {
            //update the network
            exalt_wireless_network_free(&(elt_find->wn));
            elt_find->wn = w;
            elt_find->find = 1;

            elm_genlist_item_update(l2);
        }
    }

    //remove old networks
    l_item=first;
    Elm_Genlist_Item* l_prev=NULL;
    int jump = 0;
    do
    {
        if(jump)
            jump = 0;

        Iface_List_Elt* elt;
        elt = (Iface_List_Elt*)elm_genlist_item_data_get(l_item);
        if(elt && elt->type == ITEM_NETWORK && !elt->find)
        {
            elm_genlist_item_del(l_item);
            l_item = l_prev;
            elt->nb_use--;
            if(elt->nb_use==0)
                iface_list_elt_free(elt);
        }
        l_prev = l_item;
    }while(l_item!=last &&
            (l_item = elm_genlist_item_next_get(l_item)) );

    elt = (Iface_List_Elt*)elm_genlist_item_data_get(first);
    if(elm_pager_content_top_get(pager.pager) == pager.p_list)
        elt->scan_timer  = ecore_timer_add(2,network_scan_timer_cb,elt);
}

int network_scan_timer_cb(void *data)
{
    Iface_List_Elt* elt = data;

    elt->scan_timer = NULL;

    exalt_dbus_wireless_scan(conn,elt->iface);

    return 0;
}

void iface_list_elt_free(Iface_List_Elt *elt)
{
    if(!elt)
        return;

    EXALT_FREE(elt->iface);
    EXALT_FREE(elt->ip);

    exalt_wireless_network_free(&(elt->wn));
    if(elt->scan_timer)
        ecore_timer_del(elt->scan_timer);

    EXALT_FREE(elt);
}
