#include <e.h>
#include "e_mod_main.h"
#include "e_mod_configure.h"
#include "e_mod_config.h"

struct _E_Config_Dialog_Data 
{
   char *device;
   double poll_time;
   
   Ecore_List *devs;
   int num;
};

static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _fill_data(Config_Item *ci, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

EAPI void 
_configure_net_module(void *data) 
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   E_Container *con;
   Config_Item *ci;
   char buf[PATH_MAX];

   if (e_config_dialog_find("Net", "_netmod_config_dialog")) return;
   
   ci = data;
   v = E_NEW(E_Config_Dialog_View, 1);
   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _apply_data;
   v->basic.create_widgets = _basic_create;
   
   snprintf(buf, sizeof(buf), "%s/net.edj", e_module_dir_get(cfg->mod));
   con = e_container_current_get(e_manager_current_get());
   cfd = e_config_dialog_new(con, _("Net Module Configuration"), "Net", 
			     "_netmod_config_dialog", buf, 0, v, ci);
   cfg->cfd = cfd;
}

static void *
_create_data(E_Config_Dialog *cfd) 
{
   E_Config_Dialog_Data *cfdata;
   Config_Item *ci;
   
   ci = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(ci, cfdata);
   return cfdata;
}

static void 
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   if (cfdata->devs) ecore_list_destroy(cfdata->devs);
   E_FREE(cfdata->device);
   E_FREE(cfdata);
   if (!cfg) return;
   cfg->cfd = NULL;
}

static void 
_fill_data(Config_Item *ci, E_Config_Dialog_Data *cfdata) 
{
   char *tmp;
   int i = 0;
   
   cfdata->poll_time = ci->poll_time;
   if (ci->device) 
     cfdata->device = strdup(ci->device);
   else
     cfdata->device = NULL;
   
   cfdata->devs = _config_devices_get();
   if (!cfdata->devs) return;
   while ((tmp = ecore_list_next(cfdata->devs)) != NULL)
     {
	if (!cfdata->device) continue;
	if (!strcmp(cfdata->device, tmp)) 
	  {
	     cfdata->num = i;
	     break;
	  }
	i++;
     }
}

static Evas_Object *
_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata) 
{
   Evas_Object *o, *ob, *of;
   E_Radio_Group *rg;
   char *tmp;
   int i = 0;
   
   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, _("General Settings"), 0);
   ob = e_widget_label_add(evas, _("Check Interval"));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f seconds"), 1, 60, 1, 0, 
			    &(cfdata->poll_time), NULL, 150);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   if (!cfdata->devs) return o;
   
   of = e_widget_framelist_add(evas, _("Device Settings"), 0);
   rg = e_widget_radio_group_new(&(cfdata->num));
   ecore_list_goto_first(cfdata->devs);
   while ((tmp = ecore_list_next(cfdata->devs)) != NULL) 
     {
	ob = e_widget_radio_add(evas, tmp, i, rg);
	e_widget_framelist_object_append(of, ob);
	i++;
     }
   
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   return o;
}

static int 
_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   char *tmp;
   Config_Item *ci;
   
   ci = cfd->data;
   tmp = ecore_list_goto_index(cfdata->devs, cfdata->num);
   if (tmp != NULL) 
     {
	evas_stringshare_del(ci->device);
	ci->device = evas_stringshare_add(tmp);
     }
   ci->poll_time = cfdata->poll_time;
   
   e_config_save_queue();
   _config_updated(ci->id);
   return 1;
}
