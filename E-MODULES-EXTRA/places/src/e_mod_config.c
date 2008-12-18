#include <e.h>
#include "config.h"
#include "e_mod_main.h"

struct _E_Config_Dialog_Data 
{
   int auto_mount;
   int auto_open;
   char *fm;
};

/* Local Function Prototypes */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _fill_data(E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

/* External Functions */
EAPI E_Config_Dialog *
e_int_config_places_module(E_Container *con) 
{
   E_Config_Dialog *cfd = NULL;
   E_Config_Dialog_View *v = NULL;
   char buf[4096];

   /* is this config dialog already visible ? */
   if (e_config_dialog_find("Places", "_e_module_places_cfg_dlg")) return NULL;

   v = E_NEW(E_Config_Dialog_View, 1);
   if (!v) return NULL;

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.create_widgets = _basic_create;
   v->basic.apply_cfdata = _basic_apply;

   /* Icon in the theme */
   snprintf(buf, sizeof(buf), "%s/e-module-places.edj", places_conf->module->dir);

   /* create new config dialog */
   cfd = e_config_dialog_new(con, D_("Places Configuration"), "Places", 
                             "_e_module_places_cfg_dlg", buf, 0, v, NULL);

   e_dialog_resizable_set(cfd->dia, 1);
   places_conf->cfd = cfd;
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
   free(cfdata->fm);
   places_conf->cfd = NULL;
   E_FREE(cfdata);
}

static void 
_fill_data(E_Config_Dialog_Data *cfdata) 
{
   /* load a temp copy of the config variables */
   cfdata->auto_mount = places_conf->auto_mount;
   cfdata->auto_open = places_conf->auto_open;
   if (places_conf->fm)
     cfdata->fm = strdup(places_conf->fm);
   else
     cfdata->fm = strdup("");
}

static Evas_Object *
_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata) 
{
   Evas_Object *o = NULL, *of = NULL, *ow = NULL;

   o = e_widget_list_add(evas, 0, 0);

   of = e_widget_framelist_add(evas, D_("General"), 0);
   e_widget_framelist_content_align_set(of, 0.0, 0.0);

   ow = e_widget_check_add(evas, D_("Mount volumes on insert"),
                           &(cfdata->auto_mount));
   e_widget_framelist_object_append(of, ow);

   ow = e_widget_check_add(evas, D_("Open filemanager on insert"),
                           &(cfdata->auto_open));
   e_widget_framelist_object_append(of, ow);

   ow = e_widget_label_add
     (evas, D_("File manager (if blank uses Enlightenment)"));
   e_widget_framelist_object_append(of, ow);

   ow = e_widget_entry_add(evas, &(cfdata->fm), NULL, NULL, NULL);
   e_widget_framelist_object_append(of, ow);

   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}

static int 
_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   places_conf->auto_mount = cfdata->auto_mount;
   places_conf->auto_open = cfdata->auto_open;

   const char *fm = eina_stringshare_add(cfdata->fm);
   eina_stringshare_del(places_conf->fm);
   places_conf->fm = fm;

   e_config_save_queue();
   places_update_all_gadgets();
   return 1;
}
