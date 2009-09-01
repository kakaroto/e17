// Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>


#include "e_mod_main.h"

void dns_dialog_init(Instance* inst)
{
    inst->dns.dialog = NULL;
}

void dns_dialog_create(Instance* inst)
{
    Evas* evas;
    Evas_Object *list, *flist, *o, *lbl, *ilist;
    E_Radio_Group* rg;
    int mh,mw;
    char buf[4096];

    inst->dns.dialog = e_dialog_new(inst->gcc->gadcon->zone->container, "e", "exalt_dns_dialog");
    e_dialog_title_set(inst->dns.dialog, D_("DNS (Domain Name Server) Settings"));
    inst->dns.dialog->data = inst;

    evas = e_win_evas_get(inst->dns.dialog->win);

    flist = e_widget_frametable_add(evas, D_("DNS (Domain Name Server) configuration"), 0);

    //list
    ilist = e_widget_ilist_add(evas, 300, 100, NULL);
    inst->dns.list = ilist;

    e_widget_ilist_freeze(ilist);
    e_widget_ilist_go(ilist);
    e_widget_ilist_thaw(ilist);

    e_widget_size_min_set(ilist, 300, 100);
    e_widget_frametable_object_append(flist, ilist,0, 0, 5, 1, 1, 1, 1, 1);
    //

    //entry
    inst->dns.entry_ip = e_widget_entry_add(evas,&(inst->dns.ip), dns_dialog_cb_entry, inst,NULL);
    e_widget_frametable_object_append(flist, inst->dns.entry_ip, 0,1, 2, 1, 1, 1, 1, 1);
    //

    //buttons
        inst->dns.bt_add = e_widget_button_add(evas,D_("Add"),NULL, dns_dialog_cb_add,inst,NULL);
        e_widget_frametable_object_append(flist, inst->dns.bt_add ,2, 1, 1, 1, 1, 1, 1, 1);
        inst->dns.bt_replace = e_widget_button_add(evas,D_("Replace"),NULL, dns_dialog_cb_replace,inst,NULL);
        e_widget_frametable_object_append(flist, inst->dns.bt_replace,3, 1, 1, 1, 1, 1, 1, 1);
        inst->dns.bt_delete = e_widget_button_add(evas,D_("Delete"),NULL, dns_dialog_cb_delete,inst,NULL);
        e_widget_frametable_object_append(flist, inst->dns.bt_delete, 4, 1, 1, 1, 1, 1, 1, 1);
    //


    e_widget_size_min_get(flist, &mw, &mh);
    e_dialog_content_set(inst->dns.dialog, flist, mw, mh);

    e_win_delete_callback_set(inst->dns.dialog->win, dns_dialog_cb_del);

    e_dialog_button_add(inst->dns.dialog, D_("Close"), NULL, dns_dialog_cb_close, inst);

    e_dialog_button_focus_num(inst->dns.dialog, 1);
    e_win_centered_set(inst->dns.dialog->win, 1);
}

void dns_dialog_show(Instance* inst)
{
    if(!inst->dns.dialog)
        dns_dialog_create(inst);
    e_dialog_show(inst->dns.dialog);

    exalt_dbus_dns_list_get(inst->conn);


    e_widget_disabled_set(inst->dns.bt_delete,1);
    e_widget_entry_text_set(inst->dns.entry_ip,"");
}

void dns_dialog_hide(Instance *inst)
{
    if(inst->dns.dialog)
    {
        e_object_del(E_OBJECT(inst->dns.dialog));
        inst->dns.dialog=NULL;
    }
}

void dns_dialog_update(Instance* inst,Exalt_DBus_Response *response)
{
    char* string;
    int boolean;
    if(!inst->dns.dialog)
        return ;

    switch(exalt_dbus_response_type_get(response))
    {
        case EXALT_DBUS_RESPONSE_DNS_LIST_GET:
        {
            Eina_List *l_temp;
            char *ip;
            e_widget_ilist_clear(inst->dns.list);
            EINA_LIST_FOREACH(exalt_dbus_response_list_get(response), l_temp, ip)
            {
                e_widget_ilist_append(inst->dns.list,NULL, ip,dns_dialog_cb_list , inst, NULL);
                e_widget_disabled_set(inst->dns.bt_delete,1);
            }
        }
            break;

        default: break;
    }
    dns_disabled_update(inst);
}

void dns_dialog_cb_list(void *data)
{
    Instance* inst = data;

    const char* ip = e_widget_ilist_selected_label_get(inst->dns.list);
    e_widget_entry_text_set(inst->dns.entry_ip,ip);
    e_widget_disabled_set(inst->dns.bt_delete,0);
}

void dns_dialog_cb_entry(void *data, void* data2)
{
    Instance* inst = data;
    dns_disabled_update(inst);
}

void dns_disabled_update(Instance *inst)
{
    if(!inst->dns.dialog)
        return;

    const char* ip = e_widget_entry_text_get(inst->dns.entry_ip);

    e_widget_disabled_set(inst->dns.bt_add,1);
    e_widget_disabled_set(inst->dns.bt_replace,1);

    if(exalt_address_is(ip))
    {
        e_widget_disabled_set(inst->dns.bt_add,0);
        if( exalt_address_is(e_widget_ilist_selected_label_get(inst->dns.list) ) )
            e_widget_disabled_set(inst->dns.bt_replace,0);
    }
}

void dns_dialog_cb_add(void *data, void *data2)
{
    Instance* inst = data;

    exalt_dbus_dns_add(inst->conn, e_widget_entry_text_get(inst->dns.entry_ip));
}

void dns_dialog_cb_replace(void *data, void *data2)
{
    Instance* inst = data;

    const char* ip = e_widget_ilist_selected_label_get(inst->dns.list);
    const char* ip_new = e_widget_entry_text_get(inst->dns.entry_ip);
    exalt_dbus_dns_replace(inst->conn, ip, ip_new);
}

void dns_dialog_cb_delete(void *data, void *data2)
{
    Instance* inst = data;

    const char* ip = e_widget_ilist_selected_label_get(inst->dns.list);
    exalt_dbus_dns_delete(inst->conn, ip);
}

void dns_dialog_cb_close(void *data, E_Dialog *dialog)
{
    Instance* inst = data;
    dns_dialog_hide(inst);
}


void dns_dialog_cb_del(E_Win *win)
{
    E_Dialog *dialog;
    Instance *inst;

    dialog = win->data;
    inst = dialog->data;
    dns_dialog_hide(inst);
}
