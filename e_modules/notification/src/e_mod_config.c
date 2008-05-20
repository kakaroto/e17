#include "e_mod_main.h"

struct _E_Config_Dialog_Data 
{
   int direction;
   int show_low;
   int show_normal;
   int show_critical;
   int gap;
   struct 
     {
       int x;
       int y;
     } placement;
};

/* local function protos */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _fill_data(E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

EAPI E_Config_Dialog *
e_int_config_notification_module(E_Container *con, const char *params __UNUSED__) 
{
   E_Config_Dialog *cfd = NULL;
   E_Config_Dialog_View *v = NULL;
   char buf[4096];

   if (e_config_dialog_find("Notification", "_e_modules_notification_config_dialog")) return NULL;

   v = E_NEW(E_Config_Dialog_View, 1);
   if (!v) return NULL;

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.create_widgets = _basic_create;
   v->basic.apply_cfdata = _basic_apply;

   snprintf(buf, sizeof(buf), "%s/e-module-notification.edj", notification_mod->dir);
   cfd = e_config_dialog_new(con, D_("Notification Configuration"), "Notification", 
                             "_e_modules_notification_config_dialog", buf, 0, v, NULL);
   notification_cfg->cfd = cfd;
   return cfd;
}

/* local functions */
static void *
_create_data(E_Config_Dialog *cfd __UNUSED__) 
{
   E_Config_Dialog_Data *cfdata = NULL;

   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(cfdata);
   return cfdata;
}

static void 
_free_data(E_Config_Dialog *cfd __UNUSED__, E_Config_Dialog_Data *cfdata) 
{
   notification_cfg->cfd = NULL;
   E_FREE(cfdata);
}

static void 
_fill_data(E_Config_Dialog_Data *cfdata) 
{
   cfdata->show_low      = notification_cfg->show_low;
   cfdata->show_normal   = notification_cfg->show_normal;
   cfdata->show_critical = notification_cfg->show_critical;
   cfdata->direction     = notification_cfg->direction;
   cfdata->gap           = notification_cfg->gap;
   cfdata->placement.x   = notification_cfg->placement.x;
   cfdata->placement.y   = notification_cfg->placement.y;
}

static Evas_Object *
_basic_create(E_Config_Dialog *cfd __UNUSED__, Evas *evas, E_Config_Dialog_Data *cfdata) 
{
   Evas_Object *o = NULL, *of = NULL, *ow = NULL;
   E_Radio_Group *rg;
   E_Manager *man;

   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, D_("Urgency"), 0);
   ow = e_widget_label_add(evas, D_("Levels of urgency to popup : "));
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_check_add(evas, D_("low"), &(cfdata->show_low));
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_check_add(evas, D_("normal"), &(cfdata->show_normal));
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_check_add(evas, D_("critical"), &(cfdata->show_critical));
   e_widget_framelist_object_append(of, ow);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   man = e_manager_current_get();
   of = e_widget_framelist_add(evas, D_("Placement"), 0);
   ow = e_widget_slider_add(evas, 1, 0, D_("%2.0f x"), 0.0, man->w, 1.0, 0, 
                            NULL, &(cfdata->placement.x), 200);
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_slider_add(evas, 1, 0, D_("%2.0f y"), 0.0, man->h, 1.0, 0, 
                            NULL, &(cfdata->placement.y), 200);
   e_widget_framelist_object_append(of, ow);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, D_("Direction"), 0);
   ow = e_widget_label_add(evas, D_("Direction in which popups will stack themselves : "));
   e_widget_framelist_object_append(of, ow);
   rg = e_widget_radio_group_new(&(cfdata->direction));
   ow = e_widget_radio_add(evas, "up", DIRECTION_UP, rg);
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_radio_add(evas, "down", DIRECTION_DOWN, rg);
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_radio_add(evas, "left", DIRECTION_LEFT, rg);
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_radio_add(evas, "right", DIRECTION_RIGHT, rg);
   e_widget_framelist_object_append(of, ow);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, D_("Gap"), 0);
   ow = e_widget_label_add(evas, D_("Size of the gap between two popups : "));
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_slider_add(evas, 1, 0, D_("%2.0f pixels"), 0.0, 50, 1.0, 0, 
                            NULL, &(cfdata->gap), 200);
   e_widget_framelist_object_append(of, ow);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}

static int 
_basic_apply(E_Config_Dialog *cfd __UNUSED__, E_Config_Dialog_Data *cfdata) 
{
   notification_cfg->show_low      = cfdata->show_low;
   notification_cfg->show_normal   = cfdata->show_normal;
   notification_cfg->show_critical = cfdata->show_critical;
   notification_cfg->direction     = cfdata->direction;
   notification_cfg->gap           = cfdata->gap;
   notification_cfg->placement.x   = cfdata->placement.x;
   notification_cfg->placement.y   = cfdata->placement.y;

   e_modapi_save(notification_mod);
   return 1;
}
