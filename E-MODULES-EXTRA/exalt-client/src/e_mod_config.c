#include <e.h>
#include "e_mod_main.h"

struct _E_Config_Dialog_Data
{
    int mode;
    int notification;
    int save_network;
};

/* Local Function Prototypes */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _fill_data(E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

/* External Functions */
    EAPI E_Config_Dialog *
e_int_config_exalt_module(E_Container *con, const char *params __UNUSED__)
{
    E_Config_Dialog *cfd = NULL;
    E_Config_Dialog_View *v = NULL;
    char buf[4096];

    /* is this config dialog already visible ? */
    if (e_config_dialog_find("exalt", "_e_module_exalt_cfg_dlg")) return NULL;

    v = E_NEW(E_Config_Dialog_View, 1);
    if (!v) return NULL;

    v->create_cfdata = _create_data;
    v->free_cfdata = _free_data;
    v->basic.create_widgets = _basic_create;
    v->basic.apply_cfdata = _basic_apply;

    /* Icon in the theme */
    snprintf(buf, sizeof(buf), "%s/e-module-exalt.edj", exalt_conf->module->dir);

    /* create new config dialog */
    cfd = e_config_dialog_new(con, D_("Exalt Configuration"), "exalt",
            "_e_module_exalt_cfg_dlg", buf, 0, v, NULL);

    //e_dialog_resizable_set(cfd->dia, 1);
    exalt_conf->cfd = cfd;
    return cfd;
}

/* Local Functions */
    static void *
_create_data(E_Config_Dialog *cfd)
{
    E_Config_Dialog_Data *cfdata = NULL;

    cfdata = E_NEW(E_Config_Dialog_Data, 1);
    _fill_data(cfdata);
    return cfdata;
}

    static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
    exalt_conf->cfd = NULL;
    E_FREE(cfdata);
}

    static void
_fill_data(E_Config_Dialog_Data *cfdata)
{
    /* load a temp copy of the config variables */
    cfdata->mode = exalt_conf->mode;
    cfdata->notification = exalt_conf->notification;
    cfdata->save_network = exalt_conf->save_network;
}

    static Evas_Object *
_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
    Evas_Object *o = NULL, *of = NULL, *or = NULL, *notif = NULL;
    E_Radio_Group *rg;

    o = e_widget_list_add(evas, 0, 0);

    of = e_widget_framelist_add(evas, D_("General"), 0);
    e_widget_framelist_content_align_set(of, 0.0, 0.0);


    rg = e_widget_radio_group_new(&(cfdata->mode));
    or = e_widget_radio_add(evas, D_("Basic mode"), 0, rg);
    e_widget_framelist_object_append(of, or);
    or = e_widget_radio_add(evas, D_("Advanced mode"), 1, rg);
    e_widget_framelist_object_append(of, or);

    e_widget_list_object_append(o, of, 1, 1, 0.5);


    of = e_widget_framelist_add(evas, D_("Wireless network"), 0);
    e_widget_framelist_content_align_set(of, 0.0, 0.0);
    notif = e_widget_check_add(evas, D_("Save the configuration of each wireless network"), &(cfdata->save_network));
    e_widget_framelist_object_append(of, notif);

    e_widget_list_object_append(o, of, 1, 1, 0.5);

    of = e_widget_framelist_add(evas, D_("Notifications"), 0);
    e_widget_framelist_content_align_set(of, 0.0, 0.0);
    notif = e_widget_check_add(evas, D_("Enable the notifications"), &(cfdata->notification));
    e_widget_framelist_object_append(of, notif);

    e_widget_list_object_append(o, of, 1, 1, 0.5);

    int mw,mh;
    e_widget_min_size_get(o, &mw, &mh);
    e_dialog_content_set((E_Dialog*)cfd, o, mw, mh);

    return o;
}

    static int
_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
    exalt_conf->mode = cfdata->mode;
    exalt_conf->notification = cfdata->notification;
    exalt_conf->save_network = cfdata->save_network;
    e_config_save_queue();
    return 1;
}
