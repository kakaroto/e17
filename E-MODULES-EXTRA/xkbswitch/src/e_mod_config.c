#include <e.h>

#include "e_mod_main.h"
#include "e_mod_parse.h"

/* Local prototypes */

static void *_create_data(E_Config_Dialog *cfd);

static void  _free_data  (
    E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata
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
    v->basic.create_widgets = dialog_create;
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
    E_XKB_Config_Layout  *nl      = NULL;
    E_XKB_Layout         *layout  = NULL;
    E_XKB_Model          *model   = NULL;
    E_XKB_Variant        *variant = NULL;

    cfdata = E_NEW(E_Config_Dialog_Data, 1);

    EINA_LIST_FOREACH(e_xkb_cfg_inst->used_layouts, l, cl)
    {
        nl          = E_NEW(E_XKB_Config_Layout, 1);
        nl->name    = eina_stringshare_add(cl->name);
        nl->model   = eina_stringshare_add(cl->model);
        nl->variant = eina_stringshare_add(cl->variant);

        cfdata->cfg_layouts = eina_list_append(cfdata->cfg_layouts, nl);
    }

    return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
    E_XKB_Config_Layout *cl = NULL;

    e_xkb_cfg_inst->cfd = NULL;

    EINA_LIST_FREE(cfdata->cfg_layouts, cl)
    {
        if (cl->name)    eina_stringshare_del(cl->name);
        if (cl->model)   eina_stringshare_del(cl->model);
        if (cl->variant) eina_stringshare_del(cl->variant);

        E_FREE(cl);
    }

    E_FREE(cfdata);
}

static int
_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
    /* Here, set stuff from cfdata back into config */
    Eina_List           *l  = NULL;
    E_XKB_Config_Layout *cl = NULL;
    E_XKB_Config_Layout *nl = NULL;

    EINA_LIST_FREE(e_xkb_cfg_inst->used_layouts, cl)
    {
        if (cl->name)    eina_stringshare_del(cl->name);
        if (cl->model)   eina_stringshare_del(cl->model);
        if (cl->variant) eina_stringshare_del(cl->variant);

        E_FREE(cl);
    }

    EINA_LIST_FOREACH(cfdata->cfg_layouts, l, cl)
    {
        nl          = E_NEW(E_XKB_Config_Layout, 1);
        nl->name    = eina_stringshare_add(cl->name);
        nl->model   = eina_stringshare_add(cl->model);
        nl->variant = eina_stringshare_add(cl->variant);

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
            const char *name;
           
            ll = eina_list_prev(l);

            layouts = eina_list_remove_list(layouts, l);
            layouts = eina_list_prepend_relative_list(layouts, layout, ll);

            e_widget_ilist_remove_num(cfdata->used_list, n);
            e_widget_ilist_go        (cfdata->used_list);

            icon = e_icon_add(cfdata->evas);

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
            const char *name;
            ll = eina_list_next(l);

            layouts = eina_list_remove_list(layouts, l);
            layouts = eina_list_append_relative_list(layouts, layout, ll);

            e_widget_ilist_remove_num(cfdata->used_list, n);
            e_widget_ilist_go        (cfdata->used_list);

            icon = e_icon_add(cfdata->evas);

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
        const char *name;
       
        ic = e_icon_add(cfdata->evas);

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
        const char *name;
       
        if (!layout->used)
            continue;

        ic = e_icon_add(cfdata->evas);

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
