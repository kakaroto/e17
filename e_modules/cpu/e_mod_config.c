/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2(0
 */
#include <e.h>
#include "e_mod_main.h"

struct _E_Config_Dialog_Data
{
   double poll_time;
   int show_popup;
   double max_points;
};

/* Protos */
static void *_create_data(E_Config_Dialog * cfd);
static void _free_data(E_Config_Dialog * cfd, E_Config_Dialog_Data * cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog * cfd, Evas * evas,
					  E_Config_Dialog_Data * cfdata);
static int _basic_apply_data(E_Config_Dialog * cfd,
			     E_Config_Dialog_Data * cfdata);
static void _fill_data(Config_Item * ci, E_Config_Dialog_Data * cfdata);

static int max_points;

/* Config Calls */
void
_config_cpu_module(Config_Item * ci, int max)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   E_Container *con;
   char buf[4096];

   v = E_NEW(E_Config_Dialog_View, 1);

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;

   max_points = max;
   snprintf(buf, sizeof(buf), "%s/module.edj",
	    e_module_dir_get(cpu_conf->module));
   con = e_container_current_get(e_manager_current_get());
   cfd = e_config_dialog_new(con,
			     _("CPU Configuration"),
			     "Cpu",
			     "_e_modules_cpu_config_dialog",
			     buf, 0, v, ci);
   cpu_conf->config_dialog = cfd;
}

static void
_fill_data(Config_Item * ci, E_Config_Dialog_Data * cfdata)
{
   cfdata->poll_time  = ci->poll_time;
   cfdata->show_popup = ci->show_popup;
   cfdata->max_points = (double) ci->max_points;
}

static void *
_create_data(E_Config_Dialog * cfd)
{
   E_Config_Dialog_Data *cfdata;
   Config_Item *ci;

   ci = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);

   _fill_data(ci, cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog * cfd, E_Config_Dialog_Data * cfdata)
{
   if (!cpu_conf)
     return;
   cpu_conf->config_dialog = NULL;
   free(cfdata);
   cfdata = NULL;
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog * cfd, Evas * evas,
		      E_Config_Dialog_Data * cfdata)
{
   Evas_Object *o, *of, *ob;

   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, _("General Settings"), 0);
   ob = e_widget_check_add(evas, _("Show Popup On Mouse Over"),
			   &(cfdata->show_popup));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_label_add(evas, _("Check Interval:"));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f seconds"), 1.0, 60.0, 1.0, 0,
			    &(cfdata->poll_time), NULL, 150);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_label_add(evas, _("Maximum chart points:"));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f points"), 1.0, (double) max_points, 1.0, 0,
			    &(cfdata->max_points), NULL, 150);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}

static int
_basic_apply_data(E_Config_Dialog * cfd, E_Config_Dialog_Data * cfdata)
{
   Config_Item *ci;

   ci = cfd->data;
   ci->poll_time  = cfdata->poll_time;
   ci->show_popup = cfdata->show_popup;
   ci->max_points = (int) cfdata->max_points;
   e_config_save_queue();
   _cpu_config_updated(ci->id);

   return 1;
}
