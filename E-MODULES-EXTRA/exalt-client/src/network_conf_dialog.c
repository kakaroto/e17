// Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>


#include "e_mod_main.h"

void network_conf_dialog_init(Instance* inst)
{
    inst->network_conf.dialog = NULL;
}

void network_conf_dialog_create(Instance* inst)
{
    Evas* evas;
    Evas_Object *list, *flist, *o, *lbl, *ilist;
    E_Radio_Group* rg;
    int mh,mw;
    char buf[4096];

    inst->network_conf.dialog = e_dialog_new(inst->gcc->gadcon->zone->container, "e", "exalt_network_conf_dialog");
    e_dialog_title_set(inst->network_conf.dialog, D_("Wireless network configuration"));
    inst->network_conf.dialog->data = inst;

    evas = e_win_evas_get(inst->network_conf.dialog->win);

    flist = e_widget_frametable_add(evas, D_("Wireless network configuration"), 0);

    //list
    ilist = e_widget_ilist_add(evas, 300, 150, NULL);
    inst->network_conf.list = ilist;

    e_widget_ilist_freeze(ilist);
    e_widget_ilist_go(ilist);
    e_widget_ilist_thaw(ilist);

    e_widget_size_min_set(ilist, 150, 150);
    e_widget_frametable_object_append(flist, ilist,0, 0, 1, 1, 1, 1, 1, 1);
    //

    //list favoris
    ilist = e_widget_ilist_add(evas, 300, 150, NULL);
    inst->network_conf.list_favoris = ilist;

    e_widget_ilist_freeze(ilist);
    e_widget_ilist_go(ilist);
    e_widget_ilist_thaw(ilist);

    e_widget_size_min_set(ilist, 150, 150);
    e_widget_frametable_object_append(flist, ilist,1, 0, 1, 1, 1, 1, 1, 1);
    //

    //buttons
    inst->network_conf.bt_add = e_widget_button_add(evas,D_("Add to favorite"),NULL, network_conf_dialog_cb_add,inst,NULL);
    e_widget_frametable_object_append(flist, inst->network_conf.bt_add, 0, 1, 1, 1, 1, 1, 1, 1);

    inst->network_conf.bt_remove = e_widget_button_add(evas,D_("Remove from favorite"),NULL, network_conf_dialog_cb_remove,inst,NULL);
    e_widget_frametable_object_append(flist, inst->network_conf.bt_remove, 1, 1, 1, 1, 1, 1, 1, 1);

    inst->network_conf.bt_delete = e_widget_button_add(evas,D_("Delete the network"),NULL, network_conf_dialog_cb_delete,inst,NULL);
    e_widget_frametable_object_append(flist, inst->network_conf.bt_delete, 0, 2, 2, 1, 1, 1, 1, 1);
    //

    e_widget_size_min_get(flist, &mw, &mh);
    e_dialog_content_set(inst->network_conf.dialog, flist, mw, mh);

    e_win_delete_callback_set(inst->network_conf.dialog->win, network_conf_dialog_cb_del);

    e_dialog_button_add(inst->network_conf.dialog, D_("Close"), NULL, network_conf_dialog_cb_close, inst);

    e_dialog_button_focus_num(inst->network_conf.dialog, 1);
    e_win_centered_set(inst->network_conf.dialog->win, 1);
}

void network_conf_dialog_show(Instance* inst)
{
    if(!inst->network_conf.dialog)
        network_conf_dialog_create(inst);
    e_dialog_show(inst->network_conf.dialog);

    exalt_dbus_network_list_get(inst->conn);

    //e_widget_disabled_set(inst->network_conf.bt_delete,1);
}

void network_conf_dialog_hide(Instance *inst)
{
    if(inst->network_conf.dialog)
    {
        e_object_del(E_OBJECT(inst->network_conf.dialog));
        inst->network_conf.dialog=NULL;
    }
}

void network_conf_dialog_update(Instance* inst,Exalt_DBus_Response *response)
{
    char* string;
    int boolean;
    Exalt_Configuration *c;
    Exalt_Configuration_Network *cn;

    if(!inst->network_conf.dialog)
        return ;

    switch(exalt_dbus_response_type_get(response))
    {
        case EXALT_DBUS_RESPONSE_NETWORK_LIST_GET:
            {
                Eina_List *l_temp;
                char *essid;
                e_widget_ilist_clear(inst->network_conf.list);
                e_widget_ilist_clear(inst->network_conf.list_favoris);
                EINA_LIST_FOREACH(exalt_dbus_response_list_get(response), l_temp, essid)
                    exalt_dbus_network_configuration_get(inst->conn, essid);
                e_widget_disabled_set(inst->network_conf.bt_delete,1);
                e_widget_disabled_set(inst->network_conf.bt_add,1);
                e_widget_disabled_set(inst->network_conf.bt_remove,1);
            }
            break;
        case EXALT_DBUS_RESPONSE_NETWORK_CONFIGURATION_GET:
            c = exalt_dbus_response_configuration_get(response);
            cn = exalt_conf_network_get(c);
            if(!cn) break;
            if(exalt_conf_network_favoris_is(cn))
                e_widget_ilist_append(inst->network_conf.list_favoris,NULL, exalt_conf_network_essid_get(cn),network_conf_dialog_cb_list_favoris , inst, NULL);
            else
                e_widget_ilist_append(inst->network_conf.list,NULL, exalt_conf_network_essid_get(cn),network_conf_dialog_cb_list , inst, NULL);
            break;
        default: break;
    }
}

void network_conf_dialog_cb_list(void *data)
{
    Instance* inst = data;

    e_widget_disabled_set(inst->network_conf.bt_delete,0);
    e_widget_disabled_set(inst->network_conf.bt_add,0);
}

void network_conf_dialog_cb_list_favoris(void *data)
{
    Instance* inst = data;

    e_widget_disabled_set(inst->network_conf.bt_remove,0);
}

void network_conf_dialog_cb_add(void *data, void *data2)
{
    Instance* inst = data;

    const char* essid = e_widget_ilist_selected_label_get(inst->network_conf.list);
    int select = e_widget_ilist_selected_get(inst->network_conf.list);
    e_widget_ilist_remove_num(inst->network_conf.list, select);
    e_widget_ilist_append(inst->network_conf.list_favoris,NULL, essid,network_conf_dialog_cb_list_favoris , inst, NULL);

    e_widget_disabled_set(inst->network_conf.bt_delete,1);
    e_widget_disabled_set(inst->network_conf.bt_add,1);


    exalt_dbus_network_favoris_set(inst->conn, essid, 1);
}

void network_conf_dialog_cb_remove(void *data, void *data2)
{
    Instance* inst = data;

    const char* essid = e_widget_ilist_selected_label_get(inst->network_conf.list_favoris);
    int select = e_widget_ilist_selected_get(inst->network_conf.list_favoris);
    e_widget_ilist_remove_num(inst->network_conf.list_favoris, select);
    e_widget_ilist_append(inst->network_conf.list,NULL, essid,network_conf_dialog_cb_list , inst, NULL);

    e_widget_disabled_set(inst->network_conf.bt_remove,1);

    exalt_dbus_network_favoris_set(inst->conn, essid, 0);
}


void network_conf_dialog_cb_delete(void *data, void *data2)
{
    Instance* inst = data;

    const char* essid = e_widget_ilist_selected_label_get(inst->network_conf.list);
    int select = e_widget_ilist_selected_get(inst->network_conf.list);
    e_widget_ilist_remove_num(inst->network_conf.list, select);

    exalt_dbus_network_delete(inst->conn, essid);
}

void network_conf_dialog_cb_close(void *data, E_Dialog *dialog)
{
    Instance* inst = data;
    network_conf_dialog_hide(inst);
}


void network_conf_dialog_cb_del(E_Win *win)
{
    E_Dialog *dialog;
    Instance *inst;

    dialog = win->data;
    inst = dialog->data;
    network_conf_dialog_hide(inst);
}
