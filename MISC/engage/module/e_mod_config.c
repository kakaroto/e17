#include <e.h>
#include "e_mod_main.h"
#include "config.h"

struct _E_Config_Dialog_Data
{
   int    click_focus;

   int    enabled;
   double iconsize;

   int    zoom, zoom_stretch;
   double zoom_factor, zoom_duration;

   int    tray;
};


/* Protos */
static void     *_create_data(E_Config_Dialog *cfd);
static void     _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *data);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *data);
static int      _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *data);
static Evas_Object *_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *data);
static int      _advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *data);

void _engage_module_config(E_Container *con, Engage_Bar *eb)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;

   if (eb->cfd)
     return;

   v = E_NEW(E_Config_Dialog_View, 1);
   if (v)
      {
	 char buf[4096];

         v->create_cfdata = _create_data;
         v->free_cfdata = _free_data;
         v->basic.apply_cfdata = _basic_apply_data;
         v->basic.create_widgets = _basic_create_widgets;
         v->advanced.apply_cfdata   = _advanced_apply_data;
         v->advanced.create_widgets = _advanced_create_widgets;

	 snprintf(buf, sizeof(buf), "%s/module.eap", e_module_dir_get(eb->engage->module));
         cfd = e_config_dialog_new(con, "Engage Configuration", "Engage", "_e_modules_engage_config_dialog", buf, 0, v, eb);
         eb->cfd = cfd;
      }
}

static void
_fill_data(Engage_Bar *eb, E_Config_Dialog_Data *cfdata)
{
   cfdata->click_focus = eb->engage->conf->click_focus;

   cfdata->enabled = eb->conf->enabled;
   cfdata->iconsize = (double) eb->conf->iconsize;

   cfdata->zoom = eb->conf->zoom;
   cfdata->zoom_factor = eb->conf->zoom_factor;
   cfdata->zoom_duration = eb->conf->zoom_duration;
   cfdata->zoom_stretch = eb->conf->zoom_stretch;

   cfdata->tray = eb->conf->tray;
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;
   Engage_Bar *eb;

   eb = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);

   _fill_data(eb, cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Engage_Bar *eb;

   free(cfdata);

   eb = cfd->data;
   eb->cfd = NULL;
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *data)
{
   Evas_Object *o, *of, *ob;
   E_Config_Dialog_Data *cfdata;
   Engage_Bar *eb;

   eb = cfd->data;
   cfdata = data;

   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, "Global Options", 0);
   ob = e_widget_check_add(evas, "Focus Applications on Icon Click", &(cfdata->click_focus));
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, "Bar Options", 0);
   ob = e_widget_check_add(evas, "Enable Bar", &(cfdata->enabled));
   e_widget_framelist_object_append(of, ob);
/* FIXME - hook in a resize method, as engage only accepts gadman controlled
 * resize at the moment
   ob = e_widget_label_add(evas, "Icon Size");
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_slider_add(evas, 1, 0, "%1.0f Pixels", 2.0, 400.0, 1.0, 0, &(cfdata->iconsize), NULL, 150);
   e_widget_framelist_object_append(of, ob);*/
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, "Zoom Options", 0);
   ob = e_widget_check_add(evas, "Enable Zooming", &(cfdata->zoom));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, "Stretch Bar", &(cfdata->zoom_stretch));
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, "System Tray", 0);
   ob = e_widget_check_add(evas, "Enable System Tray", &(cfdata->tray));
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Engage_Bar *eb;

   eb = cfd->data;
   eb->engage->conf->click_focus = cfdata->click_focus;

   eb->conf->enabled = cfdata->enabled;
   eb->conf->iconsize = (int) cfdata->iconsize;

   eb->conf->zoom = cfdata->zoom;
   eb->conf->zoom_stretch = cfdata->zoom_stretch;

   eb->conf->tray = cfdata->tray;
   _engage_cb_config_updated(eb);
   return 1;
}

static Evas_Object *
_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob;
   Engage_Bar *eb;

   eb = cfd->data;

   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, "Global Options", 0);
   ob = e_widget_check_add(evas, "Focus Applications on Icon Click", &(cfdata->click_focus));
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, "Bar Options", 0);
   ob = e_widget_check_add(evas, "Enable Bar", &(cfdata->enabled));
   e_widget_framelist_object_append(of, ob);
/* FIXME - hook in a resize method, as engage only accepts gadman controlled
 * resize at the moment
   ob = e_widget_label_add(evas, "Icon Size");
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_slider_add(evas, 1, 0, "%1.0f Pixels", 2.0, 400.0, 1.0, 0, &(cfdata->iconsize), NULL, 150);
   e_widget_framelist_object_append(of, ob);*/
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, "Zoom Options", 0);
   ob = e_widget_check_add(evas, "Enable Zooming", &(cfdata->zoom));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_label_add(evas, "Zoom Factor");
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_slider_add(evas, 1, 0, "%1.1f*", 1.0, 4.0, 0.1, 0, &(cfdata->zoom_factor), NULL, 150);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_label_add(evas, "Zoom Duration");
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_slider_add(evas, 1, 0, "%1.2f secs", 0.1, 0.5, 0.01, 0, &(cfdata->zoom_duration), NULL, 150);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, "Stretch Bar", &(cfdata->zoom_stretch));
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, "System Tray", 0);
   ob = e_widget_check_add(evas, "Enable System Tray", &(cfdata->tray));
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}

static int
_advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Engage_Bar *eb;

   eb = cfd->data;
   eb->engage->conf->click_focus = cfdata->click_focus;

   eb->conf->enabled = cfdata->enabled;
   eb->conf->iconsize = (int) cfdata->iconsize;

   eb->conf->zoom = cfdata->zoom;
   eb->conf->zoom_factor = cfdata->zoom_factor;
   eb->conf->zoom_duration = cfdata->zoom_duration;
   eb->conf->zoom_stretch = cfdata->zoom_stretch;

   eb->conf->tray = cfdata->tray;
   _engage_cb_config_updated(eb);
   return 1;
}
