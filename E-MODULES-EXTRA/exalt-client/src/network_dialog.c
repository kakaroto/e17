/*
 * =====================================================================================
 *
 *       Filename:  wireless_dialog.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  18/02/09 19:19:52 CET
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "e_mod_main.h"

void if_network_dialog_init(Instance* inst)
{
    inst->network.network = NULL;
}

void if_network_dialog_create(Instance* inst)
{
    Evas* evas;
    Evas_Object *list, *flist, *o, *lbl;
    E_Radio_Group* rg;
    int mh,mw;
    char buf[4096];

    inst->network.dialog = e_dialog_new(inst->gcc->gadcon->zone->container, "e", "exalt_wireless_dialog");
    e_dialog_title_set(inst->network.dialog, D_("Wireless Connection Settings"));
    inst->network.dialog->data = inst;

    evas = e_win_evas_get(inst->network.dialog->win);
    list = e_widget_list_add(evas, 0, 0);

    flist = e_widget_frametable_add(evas, D_("Wireless network information"), 0);

    lbl = e_widget_label_add(evas,D_("Essid: "));
    e_widget_frametable_object_append(flist, lbl, 0, 0, 1, 1, 1, 0, 1, 0);
    inst->network.lbl_essid = e_widget_label_add(evas,"hehe");
    e_widget_frametable_object_append(flist, inst->network.lbl_essid, 1, 0, 1, 1, 1, 0, 1, 0);

    e_widget_list_object_append(list, flist, 1, 0, 0.5);

    e_widget_min_size_get(list, &mw, &mh);
    e_dialog_content_set(inst->network.dialog, list, mw, mh);

    e_win_delete_callback_set(inst->network.dialog->win, if_network_dialog_cb_del);

    e_dialog_button_focus_num(inst->network.dialog, 1);
    e_win_centered_set(inst->network.dialog->win, 1);
}

void if_network_dialog_show(Instance* inst)
{
    if(!inst->network.dialog)
        if_network_dialog_create(inst);
    e_dialog_show(inst->network.dialog);
}

void if_network_dialog_set(Instance *inst, Popup_Elt* network)
{
    if(inst->network.network)
    {
        inst->network.network->nb_use--;
        if(inst->network.network->nb_use<=0)
            popup_elt_free(inst->network.network);
    }

    inst->network.network = network;
    network->nb_use++;

    e_widget_label_text_set(inst->network.lbl_essid,exalt_dbus_wireless_network_essid_get(network->w));
}

void if_network_dialog_hide(Instance *inst)
{
    if(inst->network.dialog)
    {
        e_object_del(E_OBJECT(inst->network.dialog));
        inst->network.dialog=NULL;
        if(inst->network.network)
        {
            inst->network.network->nb_use--;
            if(inst->network.network->nb_use<=0)
            {
                popup_elt_free(inst->network.network);
            }
            inst->network.network = NULL;
        }
    }
}

void if_network_dialog_cb_del(E_Win *win)
{
    E_Dialog *dialog;
    Instance *inst;

    dialog = win->data;
    inst = dialog->data;
    if_network_dialog_hide(inst);
}

