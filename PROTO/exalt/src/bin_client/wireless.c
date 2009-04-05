/*
 * =====================================================================================
 *
 *       Filename:  wireless.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  04/05/2009 12:52:08 PM CEST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "wireless.h"


Wireless* wireless_new(Evas_Object *win)
{
    Evas_Object *lbl, *table, *scroll;

    Wireless *wireless = calloc(1,sizeof(Wireless));

    wireless->frame = elm_frame_add(win);
    elm_frame_label_set(wireless->frame,D_("Wireless Connection Settings"));
    evas_object_show(wireless->frame);

    table = elm_table_add(win);
    evas_object_size_hint_weight_set(table, 0.0, 1.0);
    elm_table_homogenous_set(table, 1);
    evas_object_show(table);


    //
    lbl = elm_label_add(win);
    elm_label_label_set(lbl, D_("Essid: "));
    evas_object_show(lbl);
    elm_table_pack(table, lbl, 0, 0, 1, 1);

    lbl = elm_label_add(win);
    elm_label_label_set(lbl, D_("It's gonna be legendary"));
    evas_object_show(lbl);
    elm_table_pack(table, lbl, 1, 0, 2, 1);
    wireless->lbl_essid = lbl;
    //

    //
    lbl = elm_label_add(win);
    elm_label_label_set(lbl, D_("Security: "));
    evas_object_show(lbl);
    elm_table_pack(table, lbl, 0, 1, 1, 1);

    lbl = elm_label_add(win);
    elm_label_label_set(lbl, D_("It's gonna be legendary"));
    evas_object_show(lbl);
    elm_table_pack(table, lbl, 1, 1, 2, 1);
    wireless->lbl_security = lbl;
    //


    //
    wireless->hoversel_wpa  = elm_hoversel_add(win);
    elm_hoversel_hover_parent_set(wireless->hoversel_wpa, win);
    evas_object_size_hint_weight_set(wireless->hoversel_wpa, 1.0, 1.0);
    evas_object_show(wireless->hoversel_wpa);
    elm_table_pack(table, wireless->hoversel_wpa, 0, 2, 3, 1);
    //


    //
    wireless->radio_plain = elm_radio_add(win);
    elm_radio_state_value_set(wireless->radio_plain, 0);
    evas_object_size_hint_weight_set(wireless->radio_plain, 1.0, 0.0);
    evas_object_size_hint_align_set(wireless->radio_plain, -1.0, -1.0);
    elm_radio_label_set(wireless->radio_plain, D_("Plain text key"));
    evas_object_smart_callback_add(wireless->radio_plain,
            "changed", wireless_cb_plain, wireless);
    evas_object_show(wireless->radio_plain);
    elm_table_pack(table, wireless->radio_plain, 0, 4, 1, 1);
    //

    //
    wireless->radio_hexa = elm_radio_add(win);
    elm_radio_state_value_set(wireless->radio_hexa, 1);
    evas_object_size_hint_weight_set(wireless->radio_hexa, 1.0, 0.0);
    evas_object_size_hint_align_set(wireless->radio_hexa, -1.0, -1.0);
    elm_radio_label_set(wireless->radio_hexa, D_("Hexadecimal key"));
    evas_object_smart_callback_add(wireless->radio_hexa,
            "changed", wireless_cb_hexa, wireless);
    evas_object_show(wireless->radio_hexa);
    elm_radio_group_add(wireless->radio_hexa, wireless->radio_plain);
    elm_table_pack(table, wireless->radio_hexa, 1, 4, 2, 1);
    //

    //
    lbl = elm_label_add(win);
    elm_label_label_set(lbl, D_("Password: "));
    evas_object_show(lbl);
    elm_table_pack(table, lbl, 0, 5, 1, 1);

    scroll = elm_scroller_add(win);
    elm_scroller_content_min_limit(scroll, 0, 1);
    evas_object_size_hint_weight_set(scroll, 1.0, 0.0);
    evas_object_size_hint_align_set(scroll, -1.0, 0.5);
    evas_object_show(scroll);
    elm_table_pack(table, scroll, 1, 5, 2, 1);

    wireless->entry_password = elm_entry_add(win);
    evas_object_size_hint_weight_set(wireless->entry_password, 1.0, 1.0);
    evas_object_size_hint_align_set(wireless->entry_password, -1.0, -1.0);
    elm_entry_line_wrap_set(wireless->entry_password, 0);
    evas_object_show(wireless->entry_password);
    evas_object_smart_callback_add(wireless->entry_password,
            "changed", wireless_cb_entry, wireless);
    elm_scroller_content_set(scroll, wireless->entry_password);
    //



    //
    wireless->btn_back = elm_button_add(win);
    elm_button_label_set(wireless->btn_back, D_("Back"));
    evas_object_size_hint_weight_set(wireless->btn_back, 1.0, 1.0);
    evas_object_show(wireless->btn_back);
    evas_object_smart_callback_add(wireless->btn_back,
            "clicked", wireless_cb_back, wireless);
    elm_table_pack(table, wireless->btn_back, 0, 7, 1, 1);

    wireless->btn_close = elm_button_add(win);
    elm_button_label_set(wireless->btn_close, D_("Close"));
    evas_object_size_hint_weight_set(wireless->btn_close, 1.0, 1.0);
    evas_object_show(wireless->btn_close);
    evas_object_smart_callback_add(wireless->btn_close,
            "clicked", wireless_cb_close, wireless);
    elm_table_pack(table, wireless->btn_close, 1, 7, 1, 1);

    wireless->btn_apply = elm_button_add(win);
    elm_button_label_set(wireless->btn_apply, D_("Apply"));
    evas_object_size_hint_weight_set(wireless->btn_apply, 1.0, 1.0);
    evas_object_show(wireless->btn_apply);
    evas_object_smart_callback_add(wireless->btn_apply,
            "clicked", wireless_cb_apply, wireless);
    elm_table_pack(table, wireless->btn_apply, 2, 7, 1, 1);
    //

    elm_frame_content_set(wireless->frame,table);
    return wireless;
}

void wireless_set(Wireless *wireless, Iface_List_Elt* network)
{
    wireless_hoversel_wpa_clean(wireless);

    if(wireless->network)
    {
        wireless->network->nb_use--;
        if(wireless->network->nb_use<=0)
            iface_list_elt_free(wireless->network);
    }

    wireless->network = network;
    network->nb_use++;

    elm_label_label_set(wireless->lbl_essid,
            exalt_wireless_network_essid_get(network->wn));

    char* security;
    Eina_List *l_ie;
    evas_object_hide(wireless->hoversel_wpa);
    evas_object_hide(wireless->entry_password);
    evas_object_hide(wireless->radio_plain);
    evas_object_hide(wireless->radio_hexa);

    if(!exalt_wireless_network_encryption_is(network->wn))
        security = D_("No protection");
    else if( (l_ie = exalt_wireless_network_ie_get(network->wn)) == NULL)
    {
        //WEP
        security = D_("WEP");
        evas_object_show(wireless->entry_password);
        evas_object_show(wireless->radio_plain);
        evas_object_show(wireless->radio_hexa);
    }
    else
    {
        Eina_List* l;
        Exalt_Wireless_Network_IE* ie;
        //WPA
        security = D_("WPA");
        evas_object_show(wireless->hoversel_wpa);
        evas_object_show(wireless->entry_password);

        int is_eap = 0;
        int ie_number = 0;
        EINA_LIST_FOREACH(l_ie,l,ie)
        {
            int i,j;
            const char* wpa =
                exalt_wireless_network_name_from_wpa_type(
                        exalt_wireless_network_ie_wpa_type_get(ie));

            for(i=0;i<exalt_wireless_network_ie_auth_suites_number_get(ie);i++)
            {
                const char* auth =
                    exalt_wireless_network_name_from_auth_suites(
                            exalt_wireless_network_ie_auth_suites_get(ie,i));

                if(exalt_wireless_network_ie_auth_suites_get(ie,i)
                        == AUTH_SUITES_EAP)
                    is_eap = 1;
                for(j=0;j<exalt_wireless_network_ie_pairwise_cypher_number_get(ie);j++)
                {
                    char buf[1024];
                    char* pairwise =
                        exalt_wireless_network_name_from_cypher_name(
                                exalt_wireless_network_ie_pairwise_cypher_get(ie,j));

                    snprintf(buf,1024,"%s-%s-%s",wpa,auth,pairwise);
                    Hoversel_Data *data =
                        calloc(1,sizeof(Hoversel_Data));
                    data->choice = ie_number*100+i*10+j;
                    data->wireless = wireless;
                    data->desc = strdup(buf);
                    Elm_Hoversel_Item* item =
                        elm_hoversel_item_add(wireless->hoversel_wpa,
                                buf, NULL, ELM_ICON_NONE,
                                wireless_hoversel_wpa_select, data);
                    wireless->l_item_hoversel = eina_list_append(
                            wireless->l_item_hoversel, item);
                    if(ie_number == 0)
                    {
                        elm_hoversel_label_set(wireless->hoversel_wpa,
                                buf);
                        wireless->l_data_hoversel =
                            eina_list_append(wireless->l_data_hoversel,
                                    data);
                        wireless->data_hoversel_current = data;
                    }
                }
            }
            ie_number++;
        }
    }
    elm_label_label_set(wireless->lbl_security,security);

}

void wireless_hoversel_wpa_clean(Wireless *wireless)
{
    Hoversel_Data *data;
    Elm_Hoversel_Item* item;

    EINA_LIST_FREE(wireless->l_data_hoversel, data)
    {
        EXALT_FREE(data->desc);
        EXALT_FREE(data);
    }

    EINA_LIST_FREE(wireless->l_item_hoversel, item)
        elm_hoversel_item_del(item);
}

void wireless_hoversel_wpa_select(void *data,
        Evas_Object *obj __UNUSED__,
        void* event_info __UNUSED__)
{
    Hoversel_Data *hd = data;
    hd->wireless->data_hoversel_current = hd;
    elm_hoversel_label_set(hd->wireless->hoversel_wpa, hd->desc);
}


void wireless_disabled_update(Wireless *wireless)
{
    ;
}


void wireless_cb_entry(void *data __UNUSED__,
        Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__)
{
    Wireless *wireless = (Wireless*)data;
    wireless_disabled_update(wireless);
}

void wireless_cb_plain(void *data __UNUSED__,
        Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__)
{
    Wireless *wireless = (Wireless*)data;
    wireless_disabled_update(wireless);
}

void wireless_cb_hexa(void *data __UNUSED__,
        Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__)
{
    Wireless *wireless = (Wireless*)data;
    wireless_disabled_update(wireless);
}



void wireless_cb_back(void *data __UNUSED__,
        Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__)
{
    iface_list_promote(pager.iface_list);
}

void wireless_cb_close(void *data __UNUSED__,
        Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__)
{
    ecore_main_loop_quit();
}

void wireless_cb_apply(void *data __UNUSED__,
        Evas_Object* obj __UNUSED__,
        void *event_info __UNUSED__)
{
    Wireless *wireless = data;

    Exalt_Connection* c = exalt_conn_new();

    exalt_conn_wireless_set(c,1);
    exalt_conn_mode_set(c,EXALT_DHCP);

    Exalt_Wireless_Network *n = wireless->network->wn;
    exalt_conn_network_set(c,n);

    //exalt_conn_cmd_after_apply_set(c,
    //        elm_entry_entry_get(wireless->entry_cmd));

    if( elm_radio_value_get(wireless->radio_plain) == 0)
        exalt_conn_wep_key_hexa_set(c,0);
    else
        exalt_conn_wep_key_hexa_set(c,1);

    if(exalt_wireless_network_ie_get(n))
    {
        int choice = wireless->data_hoversel_current->choice;
        int ie_choice = choice/100;
        int auth_choice = (choice-ie_choice*100)/10;
        int pairwise_choice = choice-ie_choice*100-auth_choice*10;
        //printf("CHOICE %d %d %d %d\n",choice,ie_choice,auth_choice,pairwise_choice);
        exalt_wireless_network_ie_choice_set(n,ie_choice);
        Exalt_Wireless_Network_IE *ie = eina_list_nth(
                exalt_wireless_network_ie_get(n),ie_choice);
        exalt_wireless_network_ie_auth_choice_set(ie,auth_choice);
        exalt_wireless_network_ie_pairwise_choice_set(ie,pairwise_choice);
    }

    exalt_conn_key_set(c,
            elm_entry_entry_get(wireless->entry_password));

    exalt_dbus_eth_conn_apply(conn,wireless->network->iface,c);
    exalt_conn_free(&c);
}


