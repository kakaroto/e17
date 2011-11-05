#include <e.h>

#include "e_mod_main.h"
#include "e_mod_parse.h"

/* automatically typedef'd by E */
struct _E_Config_Dialog_Data 
{
    Evas *evas;
    Evas_Object *layout_list, *used_list, *model_list, *variant_list;
    Evas_Object *btn_add, *btn_del, *btn_up, *btn_down;
    Ecore_Timer *fill_delay;
};

/* Local prototypes */

static void *_create_data(E_Config_Dialog *cfd);

static void  _free_data  (
    E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata
);

static Evas_Object *_basic_create(
    E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata
);

static int _basic_apply(
    E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata
);

static void _cb_add (void *data, void *data2 __UNUSED__);
static void _cb_del (void *data, void *data2 __UNUSED__);
static void _cb_up  (void *data, void *data2 __UNUSED__);
static void _cb_down(void *data, void *data2 __UNUSED__);

static Eina_Bool _cb_fill_delay        (void *data);
static void      _cb_layout_select     (void *data);
static void      _cb_layout_used_select(void *data);
static void      _cb_model_select      (void *data);
static void      _cb_variant_select    (void *data);

static void _fill_used_list(E_Config_Dialog_Data *cfdata);

/* Externals */

E_Config_Dialog *
e_xkb_cfg_dialog(E_Container *con, const char *params) 
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

    /* Icon in the theme */
    snprintf(
        buf, sizeof(buf),
        "%s/e-module-xkbswitch.edj",
        e_xkb_cfg_inst->module->dir
    );

    /* create our config dialog */
    cfd = e_config_dialog_new(
        con,
        D_("XKB Switcher Module"),
        "XKB Switcher", 
        "keyboard_and_mouse/xkbswitch",
        buf, 0, v, NULL
    );

    e_dialog_resizable_set(cfd->dia, 1);
    e_xkb_cfg_inst->cfd = cfd;
    return cfd;
}

/* Locals */

static void *
_create_data(E_Config_Dialog *cfd)
{
    E_Config_Dialog_Data *cfdata  = NULL;
    Eina_List            *l       = NULL;
    E_XKB_Config_Layout  *cl      = NULL;
    E_XKB_Layout         *layout  = NULL;
    E_XKB_Model          *model   = NULL;
    E_XKB_Variant        *variant = NULL;

    cfdata = E_NEW(E_Config_Dialog_Data, 1);

    EINA_LIST_FOREACH(e_xkb_cfg_inst->used_layouts, l, cl)
    {
        Eina_List *node = eina_list_search_unsorted_list(
            layouts, layout_sort_by_name_cb, cl->name
        );
        if (!node) continue;

        layout = eina_list_data_get(node);

        /* this is important for load ordering */
        layouts = eina_list_append(
            eina_list_remove_list(layouts, node), layout
        );

        model = eina_list_search_unsorted(
            models, model_sort_by_name_cb, cl->model
        );
        variant = eina_list_search_unsorted(
            layout->variants, variant_sort_by_name_cb, cl->variant
        );

        layout->used = EINA_TRUE;

        if (model  ) layout->model   = model;
        if (variant) layout->variant = variant;
    }

    return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
    e_xkb_cfg_inst->cfd = NULL;
    E_FREE(cfdata);
}

static Evas_Object *
_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
    /* Main toolbook that holds all the pages */
    Evas_Object *main_tbox    = NULL;

    /* Available layouts page */
    Evas_Object *avb_box      = NULL;
    Evas_Object *avb_lst      = NULL;
    Evas_Object *avb_act_hbox = NULL;
    Evas_Object *avb_add      = NULL;
    Evas_Object *avb_del      = NULL;

    /* Used layouts page */
    Evas_Object *add_box      = NULL;
    Evas_Object *add_lst      = NULL;
    Evas_Object *add_act_hbox = NULL;
    Evas_Object *add_up       = NULL;
    Evas_Object *add_down     = NULL;

    /* Available models page */
    Evas_Object *mdl_lst      = NULL;

    /* Available variants page */
    Evas_Object *vrn_lst      = NULL;

    /* Create widgets */

    /* The toolbook */
    main_tbox = e_widget_toolbook_add(evas, 24, 24);

    /* Available page */
    avb_box = e_widget_list_add(evas, 0, 0);

    avb_lst = e_widget_ilist_add(evas, 32, 32, NULL);
    e_widget_size_min_set(avb_lst, 220, 160);
    e_widget_ilist_go(avb_lst);
    e_widget_list_object_append(avb_box, avb_lst, 1, 1, 0.5);

    avb_act_hbox = e_widget_list_add(evas, 0, 1);

    avb_add = e_widget_button_add(
        evas, D_("Add"), "list-add", 
        _cb_add, cfdata, NULL
    );
    e_widget_disabled_set(avb_add, EINA_TRUE);
    e_widget_list_object_append(avb_act_hbox, avb_add, 1, 1, 0.5);

    avb_del = e_widget_button_add(
        evas, D_("Remove"), "list-remove", 
        _cb_del, cfdata, NULL
    );
    e_widget_disabled_set(avb_del, EINA_TRUE);
    e_widget_list_object_append(avb_act_hbox, avb_del, 1, 1, 0.5);

    e_widget_list_object_append(avb_box, avb_act_hbox, 1, 0, -1);

    e_widget_toolbook_page_append(
        main_tbox,
        NULL,
        D_("Available"),
        avb_box,
        1, 1, 1, 1,
        0.5, 0.0
    );

    /* Used page */
    add_box = e_widget_list_add(evas, 0, 0);

    add_lst = e_widget_ilist_add(evas, 32, 32, NULL);
    e_widget_size_min_set(add_lst, 220, 160);
    e_widget_ilist_go(add_lst);
    e_widget_list_object_append(add_box, add_lst, 1, 1, 0.5);

    add_act_hbox = e_widget_list_add(evas, 0, 1);

    add_up = e_widget_button_add(
        evas, D_("Up"), "go-up", 
        _cb_up, cfdata, NULL
    );
    e_widget_disabled_set(add_up, EINA_TRUE);
    e_widget_list_object_append(add_act_hbox, add_up, 1, 1, 0.5);

    add_down = e_widget_button_add(
        evas, D_("Down"), "go-down", 
        _cb_down, cfdata, NULL
    );
    e_widget_disabled_set(add_down, EINA_TRUE);
    e_widget_list_object_append(add_act_hbox, add_down, 1, 1, 0.5);

    e_widget_list_object_append(add_box, add_act_hbox, 1, 0, -1);
    
    e_widget_toolbook_page_append(
        main_tbox,
        NULL,
        D_("Used"),
        add_box,
        1, 1, 1, 1,
        0.5, 0.0
    );

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

    /* Set default page */
    e_widget_toolbook_page_show(main_tbox, 0);

    /* Expose ilists outside */
    cfdata->layout_list  = avb_lst;
    cfdata->used_list    = add_lst;
    cfdata->model_list   = mdl_lst;
    cfdata->variant_list = vrn_lst;
    /* Expose buttons outside */
    cfdata->btn_add  = avb_add;
    cfdata->btn_del  = avb_del;
    cfdata->btn_up   = add_up;
    cfdata->btn_down = add_down;
    /* Expose evas */
    cfdata->evas = evas;

    if (cfdata->fill_delay)  ecore_timer_del(cfdata->fill_delay);
        cfdata->fill_delay = ecore_timer_add(0.2, _cb_fill_delay, cfdata);

    return main_tbox;
}

static int
_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
    /* Here, set stuff from cfdata back into config */
    Eina_List           *l      = NULL;
    E_XKB_Layout        *layout = NULL;
    E_XKB_Config_Layout *cl     = NULL;

    /* the old stuff has to be freed */
    while (e_xkb_cfg_inst->used_layouts)
    {
        cl = e_xkb_cfg_inst->used_layouts->data;

        e_xkb_cfg_inst->used_layouts =
            eina_list_remove_list(e_xkb_cfg_inst->used_layouts,
                                  e_xkb_cfg_inst->used_layouts);

        if (cl->name)    eina_stringshare_del(cl->name);
        if (cl->model)   eina_stringshare_del(cl->model);
        if (cl->variant) eina_stringshare_del(cl->variant);

        E_FREE(cl);
    }

    EINA_LIST_FOREACH(layouts, l, layout)
    {
        if (!layout->used) continue;

        cl          = E_NEW(E_XKB_Config_Layout, 1);
        cl->name    = eina_stringshare_add(layout->name);
        cl->model   = eina_stringshare_add(layout->model
            ? layout->model->name : ""
        );
        cl->variant = eina_stringshare_add(layout->variant
            ? layout->variant->name : ""
        );

        e_xkb_cfg_inst->used_layouts =
            eina_list_append(e_xkb_cfg_inst->used_layouts, cl);
    }

    e_config_save_queue();
    return 1;
}

static void
_cb_add(void *data, void *data2 __UNUSED__)
{
    E_Config_Dialog_Data *cfdata = NULL;
    E_XKB_Layout         *layout = NULL;
    Evas_Object          *end    = NULL;
    Eina_List            *l      = NULL;
    const char           *label  = NULL;
    int                   n      = 0;

    if (!(cfdata = data))
        return;

    if ((n = e_widget_ilist_selected_get(cfdata->layout_list)) < 0)
        return;

    if (!(label = e_widget_ilist_nth_label_get(cfdata->layout_list, n)))
        return;

    if (!(layout = eina_list_search_unsorted(
        layouts, layout_sort_by_label_cb, label
    ))) return;

    if ((end = e_widget_ilist_nth_end_get(cfdata->layout_list, n)))
        edje_object_signal_emit(end, "e,state,checked", "e");

    layout->used = EINA_TRUE;

    e_widget_ilist_unselect(cfdata->layout_list);
    e_widget_disabled_set  (cfdata->btn_add, EINA_TRUE);
    e_widget_disabled_set  (cfdata->btn_del, EINA_TRUE);

    _fill_used_list(cfdata);
}

static void
_cb_del(void *data, void *data2 __UNUSED__)
{
    E_Config_Dialog_Data *cfdata = NULL;
    E_XKB_Layout         *layout = NULL;
    Evas_Object          *end    = NULL;
    Eina_List            *l      = NULL;
    const char           *label  = NULL;
    int                   n      = 0;

    if (!(cfdata = data))
        return;

    if ((n = e_widget_ilist_selected_get(cfdata->layout_list)) < 0)
        return;

    if (!(label = e_widget_ilist_nth_label_get(cfdata->layout_list, n)))
        return;

    if (!(layout = eina_list_search_unsorted(
        layouts, layout_sort_by_label_cb, label
    ))) return;

    if ((end = e_widget_ilist_nth_end_get(cfdata->layout_list, n)))
        edje_object_signal_emit(end, "e,state,unchecked", "e");

    layout->used = EINA_FALSE;

    e_widget_ilist_unselect(cfdata->layout_list);
    e_widget_disabled_set  (cfdata->btn_add, EINA_TRUE);
    e_widget_disabled_set  (cfdata->btn_del, EINA_TRUE);

    _fill_used_list(cfdata);
}

static void
_cb_up(void *data, void *data2 __UNUSED__)
{
    E_Config_Dialog_Data *cfdata = NULL;
    Eina_List            *l      = NULL;
    Eina_List            *ll     = NULL;
    E_XKB_Layout         *layout = NULL;
    Evas_Object          *icon   = NULL;
    const char           *label  = NULL;
    int                   n      = 0;

    char buf[4096];

    if (!(cfdata = data))
        return;

    evas_event_freeze    (cfdata->evas);
    edje_freeze          ();

    e_widget_ilist_freeze(cfdata->used_list);

    if ((n = e_widget_ilist_selected_get(cfdata->used_list)) < 0)
        return;

    if (!(label = e_widget_ilist_nth_label_get(cfdata->used_list, n)))
        return;

    if ((l = eina_list_search_unsorted_list(
        layouts, layout_sort_by_label_cb, label
    )))
    {
        layout = eina_list_data_get(l);

        if (eina_list_prev(l))
        {
            ll = eina_list_prev(l);

            layouts = eina_list_remove_list(layouts, l);
            layouts = eina_list_prepend_relative_list(layouts, layout, ll);

            e_widget_ilist_remove_num(cfdata->used_list, n);
            e_widget_ilist_go        (cfdata->used_list);

            icon = e_icon_add(cfdata->evas);

            snprintf(
                buf, sizeof(buf),
                "%s/flags/%s_flag.png",
                e_module_dir_get(e_xkb_cfg_inst->module),
                layout->name
            );

            if (!ecore_file_exists(buf)) snprintf(
                buf, sizeof(buf),
                "%s/flags/unknown_flag.png",
                e_module_dir_get(e_xkb_cfg_inst->module)
            );

            e_icon_file_set(icon, buf);

            snprintf(
                buf, sizeof(buf),
                "%s (%s)",
                layout->description,
                layout->name
            );

            e_widget_ilist_prepend_relative(
                cfdata->used_list, icon, buf,
                _cb_layout_used_select,
                cfdata, layout->name, (n - 1)
            );
            e_widget_ilist_selected_set(cfdata->used_list, (n - 1));
        }
    }

    e_widget_ilist_go  (cfdata->used_list);
    e_widget_ilist_thaw(cfdata->used_list);

    edje_thaw      ();
    evas_event_thaw(cfdata->evas);
}

static void
_cb_down(void *data, void *data2 __UNUSED__)
{
    E_Config_Dialog_Data *cfdata = NULL;
    Eina_List            *l      = NULL;
    Eina_List            *ll     = NULL;
    E_XKB_Layout         *layout = NULL;
    Evas_Object          *icon   = NULL;
    const char           *label  = NULL;
    int                   n      = 0;

    char buf[4096];

    if (!(cfdata = data))
        return;

    evas_event_freeze    (cfdata->evas);
    edje_freeze          ();

    e_widget_ilist_freeze(cfdata->used_list);

    if ((n = e_widget_ilist_selected_get(cfdata->used_list)) < 0)
        return;

    if (!(label = e_widget_ilist_nth_label_get(cfdata->used_list, n)))
        return;

    if ((l = eina_list_search_unsorted_list(
        layouts, layout_sort_by_label_cb, label
    )))
    {
        layout = eina_list_data_get(l);

        if (eina_list_next(l))
        {
            ll = eina_list_next(l);

            layouts = eina_list_remove_list(layouts, l);
            layouts = eina_list_append_relative_list(layouts, layout, ll);

            e_widget_ilist_remove_num(cfdata->used_list, n);
            e_widget_ilist_go        (cfdata->used_list);

            icon = e_icon_add(cfdata->evas);

            snprintf(
                buf, sizeof(buf),
                "%s/flags/%s_flag.png",
                e_module_dir_get(e_xkb_cfg_inst->module),
                layout->name
            );

            if (!ecore_file_exists(buf)) snprintf(
                buf, sizeof(buf),
                "%s/flags/unknown_flag.png",
                e_module_dir_get(e_xkb_cfg_inst->module)
            );

            e_icon_file_set(icon, buf);

            snprintf(
                buf, sizeof(buf),
                "%s (%s)",
                layout->description,
                layout->name
            );

            e_widget_ilist_append_relative(
                cfdata->used_list, icon, buf,
                _cb_layout_used_select,
                cfdata, layout->name, n
            );
            e_widget_ilist_selected_set(cfdata->used_list, (n + 1));
        }
    }

    e_widget_ilist_go  (cfdata->used_list);
    e_widget_ilist_thaw(cfdata->used_list);

    edje_thaw      ();
    evas_event_thaw(cfdata->evas);
}

static Eina_Bool
_cb_fill_delay(void *data)
{
    E_Config_Dialog_Data *cfdata = NULL;
    Eina_List            *l      = NULL;
    Evas_Object          *ic     = NULL;
    Evas_Object          *end    = NULL;
    E_XKB_Layout         *layout = NULL;

    char buf[4096];

    if (!(cfdata = data))
        return ECORE_CALLBACK_RENEW;

    evas_event_freeze(cfdata->evas);
    edje_freeze      ();

    e_widget_ilist_freeze(cfdata->layout_list);
    e_widget_ilist_clear (cfdata->layout_list);

    EINA_LIST_FOREACH(layouts, l, layout)
    {
        ic = e_icon_add(cfdata->evas);

        snprintf(
            buf, sizeof(buf),
            "%s/flags/%s_flag.png",
            e_module_dir_get(e_xkb_cfg_inst->module),
            layout->name
        );

        if (!ecore_file_exists(buf)) snprintf(
            buf, sizeof(buf),
            "%s/flags/unknown_flag.png", 
            e_module_dir_get(e_xkb_cfg_inst->module)
        );

        e_icon_file_set(ic, buf);

        end = edje_object_add(cfdata->evas);
        if (!e_theme_edje_object_set(
            end, "base/theme/widgets",
            "e/widgets/ilist/toggle_end"
        ))
        {
            evas_object_del(end);
            end = NULL;
        }

        if (layout->used) if (end)
            edje_object_signal_emit(end, "e,state,checked", "e");
        else if (end)
            edje_object_signal_emit(end, "e,state,unchecked", "e");

        snprintf(
            buf, sizeof(buf),
            "%s (%s)",
            layout->description,
            layout->name
        );

        e_widget_ilist_append_full(
            cfdata->layout_list,
            ic, end, buf,
            _cb_layout_select,
            cfdata,
            layout->name
        );
    }

    e_widget_ilist_go  (cfdata->layout_list);
    e_widget_ilist_thaw(cfdata->layout_list);

    edje_thaw      ();
    evas_event_thaw(cfdata->evas);

    /* fill the list of used keyboards here */
    _fill_used_list(cfdata);

    cfdata->fill_delay = NULL;
    return ECORE_CALLBACK_CANCEL;
}

static void
_fill_used_list(E_Config_Dialog_Data *cfdata)
{
    Eina_List    *l      = NULL;
    Evas_Object  *ic     = NULL;
    E_XKB_Layout *layout = NULL;

    char buf[4096];

    evas_event_freeze(cfdata->evas);
    edje_freeze      ();

    e_widget_ilist_freeze(cfdata->used_list);
    e_widget_ilist_clear (cfdata->used_list);

    EINA_LIST_FOREACH(layouts, l, layout)
    {
        if (!layout->used)
            continue;

        ic = e_icon_add(cfdata->evas);

        snprintf(
            buf, sizeof(buf),
            "%s/flags/%s_flag.png",
            e_module_dir_get(e_xkb_cfg_inst->module),
            layout->name
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

        e_widget_ilist_append(
            cfdata->used_list,
            ic, buf,
            _cb_layout_used_select,
            cfdata,
            layout->name
        );
    }

    e_widget_ilist_go  (cfdata->used_list);
    e_widget_ilist_thaw(cfdata->used_list);

    edje_thaw      ();
    evas_event_thaw(cfdata->evas);
}

static void
_cb_layout_select(void *data)
{
    E_Config_Dialog_Data *cfdata = NULL;
    E_XKB_Layout         *layout = NULL;
    const char           *label  = NULL;
    int                   n      = 0;

    if (!(cfdata = data))
        return;

    if ((n = e_widget_ilist_selected_get(cfdata->layout_list)) < 0)
        return;

    if (!(label = e_widget_ilist_nth_label_get(cfdata->layout_list, n)))
        return;

    if (!(layout = eina_list_search_unsorted(
        layouts, layout_sort_by_label_cb, label
    ))) return;

    e_widget_disabled_set(cfdata->btn_add,  layout->used);
    e_widget_disabled_set(cfdata->btn_del, !layout->used);
}

static void
_cb_layout_used_select(void *data)
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

    if ((n = e_widget_ilist_selected_get(cfdata->used_list)) < 0)
        return;

    if (!(label = e_widget_ilist_nth_label_get(cfdata->used_list, n)))
        return;

    if (!(layout = eina_list_search_unsorted(
        layouts, layout_sort_by_label_cb, label
    ))) return;

    evas_event_freeze(cfdata->evas);
    edje_freeze      ();

    e_widget_ilist_freeze(cfdata->model_list);
    e_widget_ilist_clear (cfdata->model_list);

    EINA_LIST_FOREACH(models, l, model)
    {
        e_widget_ilist_append(
            cfdata->model_list, NULL,
            model->description, _cb_model_select,
            cfdata, model->name
        );

        if (model == layout->model)
            e_widget_ilist_selected_set(
                cfdata->model_list,
                (e_widget_ilist_count(cfdata->model_list) - 1)
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
            buf, _cb_variant_select,
            cfdata, variant->name
        );

        if (variant == layout->variant)
            e_widget_ilist_selected_set(
                cfdata->variant_list,
                (e_widget_ilist_count(cfdata->variant_list) - 1)
            );
    }

    e_widget_ilist_go  (cfdata->variant_list);
    e_widget_ilist_thaw(cfdata->variant_list);

    edje_thaw      ();
    evas_event_thaw(cfdata->evas);

    n = e_widget_ilist_selected_get(cfdata->used_list);

    e_widget_disabled_set(cfdata->btn_up, (n <= 0));
    e_widget_disabled_set(
        cfdata->btn_down,
        (n == (e_widget_ilist_count(cfdata->used_list) - 1))
    );
}

static void
_cb_model_select(void *data)
{
    E_Config_Dialog_Data *cfdata = NULL;
    E_XKB_Layout         *layout = NULL;
    E_XKB_Model          *model  = NULL;
    const char           *label  = NULL;
    int                   n      = 0;

    if (!(cfdata = data))
        return;

    if ((n = e_widget_ilist_selected_get(cfdata->used_list)) < 0)
        return;

    if (!(label = e_widget_ilist_nth_label_get(cfdata->used_list, n)))
        return;

    if (!(layout = eina_list_search_unsorted(
        layouts, layout_sort_by_label_cb, label
    ))) return;

    if ((n = e_widget_ilist_selected_get(cfdata->model_list)) < 0)
        return;

    if (!(label = e_widget_ilist_nth_label_get(cfdata->model_list, n)))
        return;

    if (!(model = eina_list_search_unsorted(
        models, model_sort_by_label_cb, label
    ))) return;

    if (model) layout->model = model;
}

static void
_cb_variant_select(void *data)
{
    E_Config_Dialog_Data *cfdata  = NULL;
    E_XKB_Variant        *variant = NULL;
    E_XKB_Layout         *layout  = NULL;
    const char           *label   = NULL;
    int                   n       = 0;

    if (!(cfdata = data))
        return;

    if ((n = e_widget_ilist_selected_get(cfdata->used_list)) < 0)
        return;

    if (!(label = e_widget_ilist_nth_label_get(cfdata->used_list, n)))
        return;

    if (!(layout = eina_list_search_unsorted(
        layouts, layout_sort_by_label_cb, label
    ))) return;

    if ((n = e_widget_ilist_selected_get(cfdata->variant_list)) < 0)
        return;

    if (!(label = e_widget_ilist_nth_label_get(cfdata->variant_list, n)))
        return;

    if (!(variant = eina_list_search_unsorted(
        layout->variants, variant_sort_by_label_cb, label
    ))) return;

    if (variant) layout->variant = variant;
}
