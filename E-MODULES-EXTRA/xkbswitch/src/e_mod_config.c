#include "e_mod_main.h"
#include "e_mod_parse.h"

/* Local prototypes */

static void *_create_data(E_Config_Dialog *cfd);

static void _free_data(
    E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata
);

static Evas_Object *_basic_create(
    E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata
);

static int _basic_apply(
    E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata
);

static void _cb_add(void *data, void *data2 __UNUSED__);
static void _cb_del(void *data, void *data2 __UNUSED__);

static void _cb_up(void *data, void *data2 __UNUSED__);
static void _cb_dn(void *data, void *data2 __UNUSED__);

static void _dlg_add_cb_ok(void *data, E_Dialog *dlg);
static void _dlg_add_cb_cancel(void *data, E_Dialog *dlg);

static E_Dialog *_dlg_add_new(E_Config_Dialog_Data *cfdata);

static void _dlg_add_cb_del(void *obj);

static Eina_Bool _cb_dlg_fill_delay(void *data);

static void _cb_layout_select(void *data);
static void _cb_used_select  (void *data);

static Eina_Bool _cb_fill_delay(void *data);

/* Externals */

E_Config_Dialog *e_xkb_cfg_dialog(E_Container *con, const char *params) 
{
    E_Config_Dialog    *cfd = NULL;
    E_Config_Dialog_View *v = NULL;

    char buf[4096];

    if (e_config_dialog_find("XKB Switcher", "keyboard_and_mouse/xkbswitch"))
        return NULL;

    if (!(v = E_NEW(E_Config_Dialog_View, 1)))
        return NULL;

    v->create_cfdata        = _create_data;
    v->free_cfdata          = _free_data;
    v->basic.create_widgets = _basic_create;
    v->basic.apply_cfdata   = _basic_apply;

    snprintf(
        buf, sizeof(buf), "%s/e-module-xkbswitch.edj",
        e_xkb_cfg->module->dir
    );

    cfd = e_config_dialog_new(
        con, D_("XKB Switcher Module"), "XKB Switcher", 
        "keyboard_and_mouse/xkbswitch", buf, 0, v, NULL
    );

    e_dialog_resizable_set(cfd->dia, 1);
    e_xkb_cfg->cfd = cfd;

    return cfd;
}

/* Locals */

static void *_create_data(E_Config_Dialog *cfd)
{
    E_Config_Dialog_Data *cfdata  = NULL;
    Eina_List            *l       = NULL;
    E_XKB_Config_Layout  *cl      = NULL;
    E_XKB_Config_Layout  *nl      = NULL;
    E_XKB_Layout         *layout  = NULL;
    E_XKB_Model          *model   = NULL;
    E_XKB_Variant        *variant = NULL;

    parse_rules(); /* XXX: handle in case nothing was found? */

    cfdata = E_NEW(E_Config_Dialog_Data, 1);

    cfdata->cfg_layouts = NULL;
    EINA_LIST_FOREACH(e_xkb_cfg->used_layouts, l, cl)
    {
        nl          = E_NEW(E_XKB_Config_Layout, 1);
        nl->name    = eina_stringshare_add(cl->name);
        nl->model   = eina_stringshare_add(cl->model);
        nl->variant = eina_stringshare_add(cl->variant);

        cfdata->cfg_layouts = eina_list_append(cfdata->cfg_layouts, nl);
    }

    return cfdata;
}

static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
    E_XKB_Config_Layout *cl = NULL;

    e_xkb_cfg->cfd = NULL;

    EINA_LIST_FREE(cfdata->cfg_layouts, cl)
    {
        if (cl->name)    eina_stringshare_del(cl->name);
        if (cl->model)   eina_stringshare_del(cl->model);
        if (cl->variant) eina_stringshare_del(cl->variant);

        E_FREE(cl);
    }
    eina_stringshare_del(cfdata->default_model);

    E_FREE(cfdata);

    clear_rules();
}

static int _basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
    Eina_List           *l  = NULL;
    E_XKB_Config_Layout *cl = NULL;
    E_XKB_Config_Layout *nl = NULL;

    EINA_LIST_FREE(e_xkb_cfg->used_layouts, cl)
    {
        if (cl->name)    eina_stringshare_del(cl->name);
        if (cl->model)   eina_stringshare_del(cl->model);
        if (cl->variant) eina_stringshare_del(cl->variant);
    }

    e_xkb_cfg->used_layouts = NULL;

    EINA_LIST_FOREACH(cfdata->cfg_layouts, l, cl)
    {
        nl          = E_NEW(E_XKB_Config_Layout, 1);
        nl->name    = eina_stringshare_add(cl->name);
        nl->model   = eina_stringshare_add(cl->model);
        nl->variant = eina_stringshare_add(cl->variant);

        e_xkb_cfg->used_layouts =
            eina_list_append(e_xkb_cfg->used_layouts, cl);
    }

    if (e_xkb_cfg->default_model)
        eina_stringshare_del(e_xkb_cfg->default_model);

    e_xkb_cfg->default_model = eina_stringshare_add(cfdata->default_model);

    e_xkb_update_icon  ();
    e_xkb_update_layout();

    e_config_save_queue();
    return 1;
}

static Evas_Object *_basic_create(
    E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata
)
{
    Evas_Object *main    = NULL; /* The main toolbook */
    Evas_Object *layouts = NULL; /* The layout page */
    Evas_Object *models  = NULL; /* The model page */

    Evas_Object *configs = NULL; /* The list of configurations */
    Evas_Object *btn_tbl = NULL; /* Table of buttons */

    main = e_widget_toolbook_add(evas, 24, 24);

    layouts = e_widget_list_add(evas, 0, 0);
    e_widget_size_min_set(layouts, 220, 160);

    e_widget_toolbook_page_append(
        main, NULL, D_("Configurations"), layouts, 1, 1, 1, 1, 0.5, 0.0
    );

    models = e_widget_ilist_add(evas, 32, 32, &cfdata->default_model);
    e_widget_size_min_set(models, 220, 160);

    e_widget_toolbook_page_append(
        main, NULL, D_("Models"), models, 1, 1, 1, 1, 0.5, 0.0
    );

    cfdata->dmodel_list = models;

    configs = e_widget_ilist_add(evas, 32, 32, NULL);
    e_widget_size_min_set(configs, 220, 160);
    e_widget_ilist_go(configs);

    e_widget_list_object_append(layouts, configs, 1, 1, 0.5);

    cfdata->used_list = configs;

    btn_tbl = e_widget_table_add(evas, 1);

    cfdata->btn_up = e_widget_button_add(
        evas, D_("Up"), "go-up", _cb_up, cfdata, NULL
    );

    cfdata->btn_down = e_widget_button_add(
        evas, D_("Down"), "go-down", _cb_dn, cfdata, NULL
    );

    cfdata->btn_add = e_widget_button_add(
        evas, D_("Add"), "list-add", _cb_add, cfdata, NULL
    );

    cfdata->btn_del = e_widget_button_add(
        evas, D_("Remove"), "list-remove", _cb_del, cfdata, NULL
    );

    e_widget_disabled_set(cfdata->btn_up,   EINA_TRUE);
    e_widget_disabled_set(cfdata->btn_down, EINA_TRUE);
    e_widget_disabled_set(cfdata->btn_del,  EINA_TRUE);

    e_widget_table_object_append(
        btn_tbl, cfdata->btn_up, 0, 0, 1, 1, 1, 1, 1, 0
    );
    e_widget_table_object_append(
        btn_tbl, cfdata->btn_down, 1, 0, 1, 1, 1, 1, 1, 0
    );
    e_widget_table_object_append(
        btn_tbl, cfdata->btn_add, 0, 1, 1, 1, 1, 1, 1, 0
    );
    e_widget_table_object_append(
        btn_tbl, cfdata->btn_del, 1, 1, 1, 1, 1, 1, 1, 0
    );

    e_widget_list_object_append(layouts, btn_tbl, 1, 0, 1);

    e_widget_toolbook_page_show(main, 0);

    /* Expose evas */
    cfdata->evas = evas;

    if (cfdata->fill_delay)
        ecore_timer_del(cfdata->fill_delay);

    cfdata->fill_delay = ecore_timer_add(0.2, _cb_fill_delay, cfdata);

    return main;
}

static void _cb_add(void *data, void *data2 __UNUSED__)
{
    E_Config_Dialog_Data *cfdata = NULL;
    if (!(cfdata = data)) return;

    if (cfdata->dlg_add_new)
        e_win_raise(cfdata->dlg_add_new->win);
    else
        cfdata->dlg_add_new = _dlg_add_new(cfdata);
}

static void _cb_del(void *data, void *data2 __UNUSED__)
{
    E_Config_Dialog_Data *cfdata = NULL;
    int n = 0;

    if (!(cfdata = data)) return;

    if ((n = e_widget_ilist_selected_get(cfdata->used_list)) < 0)
        return;

    cfdata->cfg_layouts = eina_list_remove_list(
        cfdata->cfg_layouts, eina_list_nth_list(cfdata->cfg_layouts, n)
    );

    evas_event_freeze(cfdata->evas);
    edje_freeze      ();

    e_widget_ilist_freeze(cfdata->used_list);

    e_widget_ilist_remove_num(cfdata->used_list, n);

    e_widget_ilist_go  (cfdata->used_list);
    e_widget_ilist_thaw(cfdata->used_list);

    edje_thaw      ();
    evas_event_thaw(cfdata->evas);
}

static void _cb_up(void *data, void *data2 __UNUSED__)
{
    E_Config_Dialog_Data *cfdata = NULL;
    void                 *nddata = NULL;
    Evas_Object          *ic     = NULL;
    Eina_List            *l      = NULL;
    const char           *lbl    = NULL;
    int n = 0;

    if (!(cfdata = data)) return;

    if ((n = e_widget_ilist_selected_get(cfdata->used_list)) < 0)
        return;

    l = eina_list_nth_list(cfdata->cfg_layouts, n);

    nddata = eina_list_data_get(eina_list_prev(l));
    eina_list_data_set(eina_list_prev(l), eina_list_data_get(l));
    eina_list_data_set(l, nddata);

    evas_event_freeze(cfdata->evas);
    edje_freeze      ();

    e_widget_ilist_freeze(cfdata->used_list);

    ic = e_icon_add(cfdata->evas);
    e_icon_file_set(ic, e_icon_file_get(
        e_widget_ilist_nth_icon_get(cfdata->used_list, n)
    ));

    lbl = e_widget_ilist_nth_label_get(cfdata->used_list, n);

    e_widget_ilist_prepend_relative_full(
        cfdata->used_list, ic, NULL, lbl,
        _cb_used_select, cfdata, NULL, (n - 1)
    );

    e_widget_ilist_remove_num(cfdata->used_list, n);

    e_widget_ilist_go  (cfdata->used_list);
    e_widget_ilist_thaw(cfdata->used_list);

    edje_thaw      ();
    evas_event_thaw(cfdata->evas);

    e_widget_ilist_selected_set(cfdata->used_list, (n - 1));
}

static void _cb_dn(void *data, void *data2 __UNUSED__)
{
    E_Config_Dialog_Data *cfdata = NULL;
    void                 *nddata = NULL;
    Evas_Object          *ic     = NULL;
    Eina_List            *l      = NULL;
    const char           *lbl    = NULL;
    int n = 0;

    if (!(cfdata = data)) return;

    if ((n = e_widget_ilist_selected_get(cfdata->used_list)) < 0)
        return;

    l = eina_list_nth_list(cfdata->cfg_layouts, n);

    nddata = eina_list_data_get(eina_list_next(l));
    eina_list_data_set(eina_list_next(l), eina_list_data_get(l));
    eina_list_data_set(l, nddata);

    evas_event_freeze(cfdata->evas);
    edje_freeze      ();

    e_widget_ilist_freeze(cfdata->used_list);

    ic = e_icon_add(cfdata->evas);
    e_icon_file_set(ic, e_icon_file_get(
        e_widget_ilist_nth_icon_get(cfdata->used_list, n)
    ));

    lbl = e_widget_ilist_nth_label_get(cfdata->used_list, n);

    e_widget_ilist_append_relative_full(
        cfdata->used_list, ic, NULL, lbl, _cb_used_select, cfdata, NULL, n
    );

    e_widget_ilist_remove_num(cfdata->used_list, n);

    e_widget_ilist_go  (cfdata->used_list);
    e_widget_ilist_thaw(cfdata->used_list);

    edje_thaw      ();
    evas_event_thaw(cfdata->evas);

    e_widget_ilist_selected_set(cfdata->used_list, (n + 1));
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
        e_xkb_cfg->cfd->con, "E", "xkbswitch_config_add_dialog"
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
        main_tbox, NULL, D_("Available"), avb_lst, 1, 1, 1, 1, 0.5, 0.0
    );

    cfdata->layout_list = avb_lst;

    /* Models page */
    mdl_lst = e_widget_ilist_add(evas, 32, 32, NULL);
    e_widget_toolbook_page_append(
        main_tbox, NULL, D_("Model"), mdl_lst, 1, 1, 1, 1, 0.5, 0.0
    );

    cfdata->model_list = mdl_lst;

    /* Variants page */
    vrn_lst = e_widget_ilist_add(evas, 32, 32, NULL);
    e_widget_toolbook_page_append(
        main_tbox, NULL, D_("Variant"), vrn_lst, 1, 1, 1, 1, 0.5, 0.0
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
    E_Config_Dialog_Data *cfdata = dlg->data;
    E_XKB_Config_Layout  *cl     = NULL;
    Evas_Object          *ic     = NULL;
    const char           *name   = NULL;
    char buf[4096];

    const char *layout =
        e_widget_ilist_selected_value_get(cfdata->layout_list);

    const char *model =
        e_widget_ilist_selected_value_get(cfdata->model_list);

    const char *variant =
        e_widget_ilist_selected_value_get(cfdata->variant_list);

    cl          = E_NEW(E_XKB_Config_Layout, 1);
    cl->name    = eina_stringshare_add(layout);
    cl->model   = eina_stringshare_add(model);
    cl->variant = eina_stringshare_add(variant);

    cfdata->cfg_layouts = eina_list_append(cfdata->cfg_layouts, cl);

    evas_event_freeze(cfdata->evas);
    edje_freeze      ();

    e_widget_ilist_freeze(cfdata->used_list);

    ic = e_icon_add(cfdata->evas);

    name = cl->name;
    if (strchr(name, '/')) name = strchr(name, '/') + 1;
    snprintf(
        buf, sizeof(buf), "%s/flags/%s_flag.png",
        e_module_dir_get(e_xkb_cfg->module), name
    );

    if (!ecore_file_exists(buf)) snprintf(
        buf, sizeof(buf), "%s/flags/unknown_flag.png", 
        e_module_dir_get(e_xkb_cfg->module)
    );

    e_icon_file_set(ic, buf);

    snprintf(
        buf, sizeof(buf), "%s (%s, %s)", cl->name, cl->model, cl->variant
    );

    e_widget_ilist_append_full(
        cfdata->used_list, ic, NULL, buf, _cb_used_select, cfdata, NULL
    );

    e_widget_ilist_go  (cfdata->used_list);
    e_widget_ilist_thaw(cfdata->used_list);

    edje_thaw      ();
    evas_event_thaw(cfdata->evas);

    cfdata->dlg_add_new = NULL;
    e_object_unref(E_OBJECT(dlg));
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
            buf, sizeof(buf), "%s/flags/%s_flag.png",
            e_module_dir_get(e_xkb_cfg->module), name
        );

        if (!ecore_file_exists(buf)) snprintf(
            buf, sizeof(buf), "%s/flags/unknown_flag.png", 
            e_module_dir_get(e_xkb_cfg->module)
        );

        e_icon_file_set(ic, buf);

        snprintf(
            buf, sizeof(buf), "%s (%s)", layout->description, layout->name
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
        layouts, layout_sort_by_name_cb,
        e_widget_ilist_nth_value_get(cfdata->layout_list, n)
    ))) return;

    evas_event_freeze(cfdata->dlg_evas);
    edje_freeze      ();

    e_widget_ilist_freeze(cfdata->model_list);
    e_widget_ilist_clear (cfdata->model_list);

    EINA_LIST_FOREACH(models, l, model)
    {
        snprintf(buf, sizeof(buf), "%s (%s)", model->description, model->name);
        e_widget_ilist_append(
            cfdata->model_list, NULL, buf, NULL, cfdata, model->name
        );
    }

    e_widget_ilist_go    (cfdata->model_list);
    e_widget_ilist_thaw  (cfdata->model_list);
    e_widget_ilist_freeze(cfdata->variant_list);
    e_widget_ilist_clear (cfdata->variant_list);

    EINA_LIST_FOREACH(layout->variants, l, variant)
    {
        snprintf(
            buf, sizeof(buf),  "%s (%s)", variant->name, variant->description
        );

        e_widget_ilist_append(
            cfdata->variant_list, NULL, buf, NULL, cfdata, variant->name
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
    E_XKB_Model          *model  = NULL;
    int n = 0;

    char buf[4096];

    if (!(cfdata = data))
        return ECORE_CALLBACK_RENEW;

    evas_event_freeze(cfdata->evas);
    edje_freeze      ();

    e_widget_ilist_freeze(cfdata->used_list);
    e_widget_ilist_clear (cfdata->used_list);

    EINA_LIST_FOREACH(cfdata->cfg_layouts, l, cl)
    {
        const char *name = NULL;

        ic = e_icon_add(cfdata->evas);

        name = cl->name;
        if (strchr(name, '/')) name = strchr(name, '/') + 1;
        snprintf(
            buf, sizeof(buf), "%s/flags/%s_flag.png",
            e_module_dir_get(e_xkb_cfg->module), name
        );

        if (!ecore_file_exists(buf)) snprintf(
            buf, sizeof(buf), "%s/flags/unknown_flag.png", 
            e_module_dir_get(e_xkb_cfg->module)
        );

        e_icon_file_set(ic, buf);

        snprintf(
            buf, sizeof(buf), "%s (%s, %s)", cl->name, cl->model, cl->variant
        );

        e_widget_ilist_append_full(
            cfdata->used_list, ic, NULL, buf, _cb_used_select, cfdata, NULL
        );
    }

    e_widget_ilist_go  (cfdata->used_list);
    e_widget_ilist_thaw(cfdata->used_list);

    e_widget_ilist_freeze(cfdata->dmodel_list);
    e_widget_ilist_clear (cfdata->dmodel_list);

    EINA_LIST_FOREACH(models, l, model)
    {
        snprintf(buf, sizeof(buf), "%s (%s)", model->description, model->name);
        e_widget_ilist_append(
            cfdata->dmodel_list, NULL, buf, NULL, cfdata, model->name
        );
        if (model->name == e_xkb_cfg->default_model)
            e_widget_ilist_selected_set(cfdata->dmodel_list, n);

        ++n;
    }

    e_widget_ilist_go    (cfdata->dmodel_list);
    e_widget_ilist_thaw  (cfdata->dmodel_list);

    edje_thaw      ();
    evas_event_thaw(cfdata->evas);

    cfdata->fill_delay = NULL;
    return ECORE_CALLBACK_CANCEL;
}

static void
_cb_used_select(void *data)
{
    E_Config_Dialog_Data *cfdata  = NULL;
    int n, c;

    if (!(cfdata = data))
        return;

    if ((n = e_widget_ilist_selected_get(cfdata->used_list)) < 0)
        return;

    c = e_widget_ilist_count(cfdata->used_list);

    e_widget_disabled_set(cfdata->btn_del, EINA_FALSE);

    if (n == (c - 1))
    {
        e_widget_disabled_set(cfdata->btn_up,   EINA_FALSE);
        e_widget_disabled_set(cfdata->btn_down, EINA_TRUE );
    }
    else if (n == 0)
    {
        e_widget_disabled_set(cfdata->btn_up,   EINA_TRUE );
        e_widget_disabled_set(cfdata->btn_down, EINA_FALSE);
    }
    else
    {
        e_widget_disabled_set(cfdata->btn_up,   EINA_FALSE);
        e_widget_disabled_set(cfdata->btn_down, EINA_FALSE);
    }
}
