#include <e.h>
#include "e_mod_main.h"

struct _E_Config_Dialog_Data
{
  int           layout_mode;
  double	duration;
  double	spacing;
  int		grow;
  int		tight;
  int		show_iconic;
  
  int           desks_layout_mode;
  double	desks_duration;
  double	desks_spacing;
  int		desks_grow;
  int		desks_tight;
  int		desks_show_iconic;

  int		fade_popups;
  int		fade_desktop;
  int		fade_windows;

  double	pager_duration;
  int		pager_fade_popups;
  int		pager_fade_desktop;
  int		pager_fade_windows;
};


static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _fill_data(E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

E_Config_Dialog *
e_int_config_scale_module(E_Container *con, const char *params)
{
   E_Config_Dialog *cfd = NULL;
   E_Config_Dialog_View *v = NULL;
   char buf[4096];

   if (e_config_dialog_find("Scale", "appearance/comp-scale")) return NULL;

   v = E_NEW(E_Config_Dialog_View, 1);
   if (!v) return NULL;

   v->create_cfdata	   = _create_data;
   v->free_cfdata	   = _free_data;
   v->basic.create_widgets = _basic_create;
   v->basic.apply_cfdata   = _basic_apply;

   snprintf(buf, sizeof(buf), "%s/e-module-scale.edj", scale_conf->module->dir);

   cfd = e_config_dialog_new(con, D_("Scale Windows Module"), "Scale",
                             "appearance/comp-scale", buf, 0, v, NULL);

   e_dialog_resizable_set(cfd->dia, 0);
   scale_conf->cfd = cfd;
   return cfd;
}

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
   scale_conf->cfd = NULL;
   E_FREE(cfdata);
}

static void
_fill_data(E_Config_Dialog_Data *cfdata)
{
   cfdata->tight	      = scale_conf->tight;
   cfdata->grow		      = scale_conf->grow;
   cfdata->show_iconic        = scale_conf->show_iconic;
   cfdata->duration	      = scale_conf->scale_duration;
   cfdata->spacing	      = scale_conf->spacing;
   cfdata->desks_duration     = scale_conf->desks_duration;
   cfdata->desks_spacing      = scale_conf->desks_spacing;
   cfdata->fade_popups	      = scale_conf->fade_popups;
   cfdata->fade_desktop	      = scale_conf->fade_desktop;
   cfdata->fade_windows	      = scale_conf->fade_windows;
   cfdata->layout_mode	      = scale_conf->layout_mode;
   cfdata->desks_layout_mode  = scale_conf->desks_layout_mode;
   cfdata->desks_tight	      = scale_conf->desks_tight;
   cfdata->desks_grow	      = scale_conf->desks_grow;
   cfdata->desks_show_iconic  = scale_conf->desks_show_iconic;
   cfdata->pager_duration     = scale_conf->pager_duration;
   cfdata->pager_fade_popups  = scale_conf->pager_fade_popups;
   cfdata->pager_fade_desktop = scale_conf->pager_fade_desktop;
   cfdata->pager_fade_windows = scale_conf->pager_fade_windows;
}

static Evas_Object *
_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ow, *ot, *otb;
   E_Radio_Group *rg;
   
   otb = e_widget_toolbook_add(evas, 0, 0);

   ot = e_widget_table_add(evas, 0);

   o = e_widget_list_add(evas, 0, 0);

   of = e_widget_framelist_add(evas, D_("Current Desktop"), 0);
   ow = e_widget_label_add (evas, D_("Minimum space between windows"));
   e_widget_framelist_object_append (of, ow);
   ow = e_widget_slider_add (evas, 1, 0, D_("%1.0f"), 2.0, 64.0, 1.0, 0,
			     &(cfdata->spacing), NULL,100);
   e_widget_framelist_object_append (of, ow);

   ow = e_widget_label_add (evas, D_("Scale duration"));
   e_widget_framelist_object_append (of, ow);
   ow = e_widget_slider_add (evas, 1, 0, D_("%1.2f"), 0.01, 3.0, 0.01, 0,
			     &(cfdata->duration), NULL,100);
   e_widget_framelist_object_append (of, ow);

   rg = e_widget_radio_group_new(&cfdata->layout_mode);
   ow = e_widget_radio_add(evas, D_("Slotted Layout"), 1, rg);
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_radio_add(evas, D_("Natural Layout"), 0, rg);
   e_widget_framelist_object_append(of, ow);

   e_widget_framelist_content_align_set(of, 0.0, 0.0);
   ow = e_widget_check_add(evas, D_("Grow more!"),
			   &(cfdata->grow));
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_check_add(evas, D_("Keep it tight!"),
			   &(cfdata->tight));
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_check_add(evas, D_("Show iconified windows"),
			   &(cfdata->show_iconic));
   e_widget_framelist_object_append(of, ow);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   e_widget_table_object_append(ot, o, 0, 0, 1, 1, 1, 1, 0, 0);

   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, D_("Show All Desktops"), 0);
   ow = e_widget_label_add (evas, D_("Minimum space between windows"));
   e_widget_framelist_object_append (of, ow);
   ow = e_widget_slider_add (evas, 1, 0, D_("%1.0f"), 2.0, 64.0,
                             1.0, 0, &(cfdata->desks_spacing), NULL,100);
   e_widget_framelist_object_append (of, ow);

   ow = e_widget_label_add (evas, D_("Scale duration"));
   e_widget_framelist_object_append (of, ow);
   ow = e_widget_slider_add (evas, 1, 0, D_("%1.2f"), 0.01, 3.0, 0.01, 0,
			     &(cfdata->desks_duration), NULL,100);
   e_widget_framelist_object_append (of, ow);
   rg = e_widget_radio_group_new(&cfdata->desks_layout_mode);
   ow = e_widget_radio_add(evas, D_("Slotted Layout"), 1, rg);
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_radio_add(evas, D_("Natural Layout"), 0, rg);
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_check_add(evas, D_("Grow more!"),
			   &(cfdata->desks_grow));
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_check_add(evas, D_("Keep it tight!"),
			   &(cfdata->desks_tight));
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_check_add(evas, D_("Fade in windows"),
			   &(cfdata->fade_windows));
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_check_add(evas, D_("Show iconified windows"),
			   &(cfdata->desks_show_iconic));
   e_widget_framelist_object_append(of, ow);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   e_widget_table_object_append(ot, o, 1, 0, 1, 1, 1, 1, 0, 0);

   of = e_widget_framelist_add(evas, NULL, 1);
   ow = e_widget_check_add(evas, D_("Fade out shelves and popups"),
			   &(cfdata->fade_popups));
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_check_add(evas, D_("Darken desktop"),
			   &(cfdata->fade_desktop));
   e_widget_framelist_object_append(of, ow);
   e_widget_table_object_append(ot, of, 0, 1, 2, 1, 1, 1, 0, 0);

   e_widget_toolbook_page_append(otb, NULL, "Scale Windows", ot, 1, 1, 1, 1, 0, 0);

   ot = e_widget_table_add(evas, 0);
   of = e_widget_framelist_add(evas, D_("Pager Settings"), 0);

   ow = e_widget_label_add (evas, D_("Zoom duration"));
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_slider_add (evas, 1, 0, D_("%1.2f"), 0.01, 3.0, 0.01, 0,
			     &(cfdata->pager_duration), NULL,100);
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_check_add(evas, D_("Fade in windows"),
			   &(cfdata->pager_fade_windows));
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_check_add(evas, D_("Fade out shelves and popups"),
			   &(cfdata->pager_fade_popups));
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_check_add(evas, D_("Darken desktop"),
			   &(cfdata->fade_desktop));
   e_widget_framelist_object_append (of, ow);
   e_widget_table_object_append(ot, of, 0, 0, 1, 1, 1, 1, 1, 0);

   e_widget_toolbook_page_append(otb, NULL, "Pager", ot, 1, 1, 1, 1, 1, 1);

   e_widget_toolbook_page_show(otb, 0);

   return otb;
}

static int
_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   scale_conf->grow		  = cfdata->grow;
   scale_conf->tight		  = cfdata->tight;
   scale_conf->show_iconic	  = cfdata->show_iconic;
   scale_conf->scale_duration	  = cfdata->duration;
   scale_conf->spacing		  = cfdata->spacing;
   scale_conf->desks_duration	  = cfdata->desks_duration;
   scale_conf->desks_spacing	  = cfdata->desks_spacing;
   scale_conf->fade_popups	  = cfdata->fade_popups;
   scale_conf->fade_desktop	  = cfdata->fade_desktop;
   scale_conf->fade_windows	  = cfdata->fade_windows;
   scale_conf->fade_desktop	  = cfdata->fade_desktop;
   scale_conf->layout_mode	  = cfdata->layout_mode;
   scale_conf->desks_layout_mode  = cfdata->desks_layout_mode;
   scale_conf->desks_grow	  = cfdata->desks_grow;
   scale_conf->desks_tight	  = cfdata->desks_tight;
   scale_conf->desks_show_iconic  = cfdata->desks_show_iconic;
   scale_conf->pager_duration	  = cfdata->pager_duration;
   scale_conf->pager_fade_popups  = cfdata->pager_fade_popups;
   scale_conf->pager_fade_windows = cfdata->pager_fade_windows;
   scale_conf->pager_fade_desktop = cfdata->pager_fade_desktop;

   e_config_save_queue();
   return 1;
}
