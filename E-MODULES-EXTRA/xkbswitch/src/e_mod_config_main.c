#include <e.h>

#include "e_mod_main.h"
#include "e_mod_parse.h"

static void _cb_dummy(void *data __UNUSED__, void *data2 __UNUSED__) {}

static void _cb_add(void *data, void *data2 __UNUSED__);

static void _dlg_add_cb_ok(void *data, E_Dialog *dlg);
static void _dlg_add_cb_cancel(void *data, E_Dialog *dlg);

static E_Dialog *_dlg_add_new(E_Config_Dialog_Data *cfdata);

static void _dlg_add_cb_del(void *obj);

static Eina_Bool _cb_dlg_fill_delay(void *data);

static void _cb_layout_select(void *data);

static Eina_Bool _cb_fill_delay(void *data);

Evas_Object *
dialog_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
    /* These hold the contents */
    Evas_Object *main = NULL;
    Evas_Object *list = NULL;

    /* The configuration list */
    Evas_Object *configs = NULL;

    /* Button table */
    Evas_Object *btn_tbl = NULL;

    /* The buttons */
    Evas_Object *add_btn = NULL;
    Evas_Object *del_btn = NULL;
    Evas_Object * up_btn = NULL;
    Evas_Object * dn_btn = NULL;

    main = e_widget_list_add(evas, 0, 0);

    list = e_widget_framelist_add(evas, D_("Configurations"), 0);

    configs = e_widget_ilist_add(evas, 32, 32, NULL);
    e_widget_size_min_set(configs, 220, 160);
    e_widget_ilist_go(configs);

    e_widget_framelist_object_append(list, configs);

    cfdata->used_list = configs;

    e_widget_list_object_append(main, list, 1, 1, 0.5);

    btn_tbl = e_widget_table_add(evas, 1);

    up_btn = e_widget_button_add(
        evas, D_("Up"), "go-up", _cb_dummy, cfdata, NULL
    );

    dn_btn = e_widget_button_add(
        evas, D_("Down"), "go-down", _cb_dummy, cfdata, NULL
    );

    add_btn = e_widget_button_add(
        evas, D_("Add"), "list-add", _cb_add, cfdata, NULL
    );

    del_btn = e_widget_button_add(
        evas, D_("Remove"), "list-remove", _cb_dummy, cfdata, NULL
    );

    e_widget_table_object_append(btn_tbl,  up_btn, 0, 0, 1, 1, 1, 1, 1, 0);
    e_widget_table_object_append(btn_tbl,  dn_btn, 1, 0, 1, 1, 1, 1, 1, 0);
    e_widget_table_object_append(btn_tbl, add_btn, 0, 1, 1, 1, 1, 1, 1, 0);
    e_widget_table_object_append(btn_tbl, del_btn, 1, 1, 1, 1, 1, 1, 1, 0);

    e_widget_list_object_append(main, btn_tbl, 1, 0, 1);

    /* Expose evas */
    cfdata->evas = evas;

    if (cfdata->fill_delay)  ecore_timer_del(cfdata->fill_delay);
        cfdata->fill_delay = ecore_timer_add(0.2, _cb_fill_delay, cfdata);

    return main;
}

static void
_cb_add(void *data, void *data2 __UNUSED__)
{
   E_Config_Dialog_Data *cfdata = NULL;
   if (!(cfdata = data)) return;

   if (cfdata->dlg_add_new)
        e_win_raise(cfdata->dlg_add_new->win);
   else
        cfdata->dlg_add_new = _dlg_add_new(cfdata);
}

static E_Dialog *_dlg_add_new(E_Config_Dialog_Data *cfdata)
{
    E_Dialog *dlg = NULL;
    Evas *evas = NULL;
    Evas_Coord mw, mh;

    /* Main toolbook that holds all the pages */
    Evas_Object *main_tbox    = NULL;

    /* Available layouts page */
    Evas_Object *avb_lst      = NULL;

    /* Available models page */
    Evas_Object *mdl_lst      = NULL;

    /* Available variants page */
    Evas_Object *vrn_lst      = NULL;

    dlg = e_dialog_new(
        e_xkb_cfg_inst->cfd->con, "E", "xkbswitch_config_add_dialog"
    );
    if (!dlg) return NULL;
    dlg->data = cfdata;

    e_object_del_attach_func_set(E_OBJECT(dlg), _dlg_add_cb_del);
    e_win_centered_set(dlg->win, 1);

    evas = e_win_evas_get(dlg->win);
    e_dialog_title_set(dlg, D_("Add New Configuration"));

    /* Create widgets */

    /* The toolbook */
    main_tbox = e_widget_toolbook_add(evas, 24, 24);

    /* Available page */
    avb_lst = e_widget_ilist_add(evas, 32, 32, NULL);
    e_widget_size_min_set(avb_lst, 220, 160);
    e_widget_ilist_go(avb_lst);

    e_widget_toolbook_page_append(
        main_tbox,
        NULL,
        D_("Available"),
        avb_lst,
        1, 1, 1, 1,
        0.5, 0.0
    );

    cfdata->layout_list = avb_lst;

    /* Models page */
    mdl_lst = e_widget_ilist_add(evas, 32, 32, NULL);
    e_widget_toolbook_page_append(
        main_tbox,
        NULL,
        D_("Model"),
        mdl_lst,
        1, 1, 1, 1,
        0.5, 0.0
    );

    cfdata->model_list = mdl_lst;

    /* Variants page */
    vrn_lst = e_widget_ilist_add(evas, 32, 32, NULL);
    e_widget_toolbook_page_append(
        main_tbox,
        NULL,
        D_("Variant"),
        vrn_lst,
        1, 1, 1, 1,
        0.5, 0.0
    );

    cfdata->variant_list = vrn_lst;

    /* Set default page */
    e_widget_toolbook_page_show(main_tbox, 0);

    e_widget_size_min_get(main_tbox, &mw, &mh);
    e_dialog_content_set(dlg, main_tbox, mw, mh);

    cfdata->dlg_evas = evas;

    if (cfdata->dlg_fill_delay) 
        ecore_timer_del(cfdata->dlg_fill_delay);

    cfdata->dlg_fill_delay = ecore_timer_add(0.2, _cb_dlg_fill_delay, cfdata);

    e_dialog_button_add(dlg, D_("OK"), NULL, _dlg_add_cb_ok, cfdata);
    e_dialog_button_add(dlg, D_("Cancel"), NULL, _dlg_add_cb_cancel, cfdata);

    e_dialog_button_disable_num_set(dlg, 0, 1);
    e_dialog_button_disable_num_set(dlg, 1, 0);

    e_dialog_resizable_set(dlg, 1);
    e_dialog_show(dlg);

    return dlg;
}

static void
_dlg_add_cb_ok(void *data, E_Dialog *dlg)
{
}

static void
_dlg_add_cb_cancel(void *data, E_Dialog *dlg)
{
   E_Config_Dialog_Data *cfdata = dlg->data;

   cfdata->dlg_add_new = NULL;
   e_object_unref(E_OBJECT(dlg));
}

static void
_dlg_add_cb_del(void *obj)
{
   E_Dialog *dlg = obj;
   E_Config_Dialog_Data *cfdata = dlg->data;

   cfdata->dlg_add_new = NULL;
   e_object_unref(E_OBJECT(dlg));
}

static Eina_Bool
_cb_dlg_fill_delay(void *data)
{
    E_Config_Dialog_Data *cfdata = NULL;
    Eina_List            *l      = NULL;
    Evas_Object          *ic     = NULL;
    E_XKB_Layout         *layout = NULL;

    char buf[4096];

    if (!(cfdata = data))
        return ECORE_CALLBACK_RENEW;

    evas_event_freeze(cfdata->dlg_evas);
    edje_freeze      ();

    e_widget_ilist_freeze(cfdata->layout_list);
    e_widget_ilist_clear (cfdata->layout_list);

    EINA_LIST_FOREACH(layouts, l, layout)
    {
        const char *name;

        ic = e_icon_add(cfdata->dlg_evas);

        name = layout->name;
        if (strchr(name, '/')) name = strchr(name, '/') + 1;
        snprintf(
            buf, sizeof(buf),
            "%s/flags/%s_flag.png",
            e_module_dir_get(e_xkb_cfg_inst->module),
            name
        );

        if (!ecore_file_exists(buf)) snprintf(
            buf, sizeof(buf),
            "%s/flags/unknown_flag.png", 
            e_module_dir_get(e_xkb_cfg_inst->module)
        );

        e_icon_file_set(ic, buf);

        snprintf(
            buf, sizeof(buf),
            "%s (%s)",
            layout->description,
            layout->name
        );

        e_widget_ilist_append_full(
            cfdata->layout_list, ic, NULL, buf, _cb_layout_select,
            cfdata, layout->name
        );
    }

    e_widget_ilist_go  (cfdata->layout_list);
    e_widget_ilist_thaw(cfdata->layout_list);

    edje_thaw      ();
    evas_event_thaw(cfdata->dlg_evas);

    cfdata->dlg_fill_delay = NULL;
    return ECORE_CALLBACK_CANCEL;
}

static void
_cb_layout_select(void *data)
{
    E_Config_Dialog_Data *cfdata  = NULL;
    E_XKB_Variant        *variant = NULL;
    E_XKB_Layout         *layout  = NULL;
    E_XKB_Model          *model   = NULL;
    Eina_List            *l       = NULL;
    const char           *label   = NULL;
    int                   n       = 0;

    char buf[4096];

    if (!(cfdata = data))
        return;

    if ((n = e_widget_ilist_selected_get(cfdata->layout_list)) < 0)
        return;

    if (!(label = e_widget_ilist_nth_label_get(cfdata->layout_list, n)))
        return;

    if (!(layout = eina_list_search_unsorted(
        layouts, layout_sort_by_label_cb, label
    ))) return;

    evas_event_freeze(cfdata->dlg_evas);
    edje_freeze      ();

    e_widget_ilist_freeze(cfdata->model_list);
    e_widget_ilist_clear (cfdata->model_list);

    EINA_LIST_FOREACH(models, l, model)
    {
        e_widget_ilist_append(
            cfdata->model_list, NULL,
            model->description, NULL,
            cfdata, model->name
        );
    }

    e_widget_ilist_go    (cfdata->model_list);
    e_widget_ilist_thaw  (cfdata->model_list);
    e_widget_ilist_freeze(cfdata->variant_list);
    e_widget_ilist_clear (cfdata->variant_list);

    EINA_LIST_FOREACH(layout->variants, l, variant)
    {
        snprintf(
            buf, sizeof(buf),
            "%s (%s)",
            variant->name,
            variant->description
        );

        e_widget_ilist_append(
            cfdata->variant_list, NULL,
            buf, NULL,
            cfdata, variant->name
        );
    }

    e_widget_ilist_go  (cfdata->variant_list);
    e_widget_ilist_thaw(cfdata->variant_list);

    edje_thaw      ();
    evas_event_thaw(cfdata->dlg_evas);

    e_widget_ilist_selected_set(cfdata->model_list,   0);
    e_widget_ilist_selected_set(cfdata->variant_list, 0);

    e_dialog_button_disable_num_set(cfdata->dlg_add_new, 0, 0);
}

static Eina_Bool
_cb_fill_delay(void *data)
{
    E_Config_Dialog_Data *cfdata = NULL;
    Eina_List            *l      = NULL;
    Evas_Object          *ic     = NULL;
    E_XKB_Config_Layout  *cl     = NULL;

    char buf[4096];

    if (!(cfdata = data))
        return ECORE_CALLBACK_RENEW;

    evas_event_freeze(cfdata->evas);
    edje_freeze      ();

    e_widget_ilist_freeze(cfdata->used_list);
    e_widget_ilist_clear (cfdata->used_list);

    EINA_LIST_FOREACH(e_xkb_cfg_inst->used_layouts, l, cl)
    {
        const char *name = NULL;

        ic = e_icon_add(cfdata->evas);

        name = cl->name;
        if (strchr(name, '/')) name = strchr(name, '/') + 1;
        snprintf(
            buf, sizeof(buf),
            "%s/flags/%s_flag.png",
            e_module_dir_get(e_xkb_cfg_inst->module),
            name
        );

        if (!ecore_file_exists(buf)) snprintf(
            buf, sizeof(buf),
            "%s/flags/unknown_flag.png", 
            e_module_dir_get(e_xkb_cfg_inst->module)
        );

        e_icon_file_set(ic, buf);

        snprintf(
            buf, sizeof(buf), "%s (%s, %s)",
            cl->name, cl->model, cl->variant
        );

        e_widget_ilist_append_full(
            cfdata->used_list,
            ic, NULL, buf,
            /*_cb_layout_select*/ NULL,
            cfdata,
            NULL /* data */
        );
    }

    e_widget_ilist_go  (cfdata->used_list);
    e_widget_ilist_thaw(cfdata->used_list);

    edje_thaw      ();
    evas_event_thaw(cfdata->evas);

    cfdata->fill_delay = NULL;
    return ECORE_CALLBACK_CANCEL;
}
