#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

struct _E_Config_Dialog_Data 
{
   double poll_time;
   int display;
   int degrees;
   char *code;
};

static void        *_create_data            (E_Config_Dialog *cfd);
static void         _free_data              (E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets   (E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int          _basic_apply_data       (E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void         _fill_data              (Weather_Face *wf, E_Config_Dialog_Data *cfdata);

void
_configure_weather_module(Weather_Face *wf) 
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;

   v = E_NEW(E_Config_Dialog_View, 1);

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;
   v->override_auto_apply = 1;
   cfd = e_config_dialog_new(wf->con, _("Weather Configuration"), NULL, 0, v, wf);
   wf->weather->cfd = cfd;
}

static void
_fill_data(Weather_Face *wf, E_Config_Dialog_Data *cfdata)
{
   cfdata->poll_time = (wf->conf->poll_time / 60.0);
   cfdata->display = wf->conf->display;
   cfdata->degrees = wf->conf->degrees;
   cfdata->code = (char *)evas_stringshare_add(wf->conf->code);
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;
   Weather_Face *wf;

   wf = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(wf, cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Weather_Face *wf;

   wf = cfd->data;
   wf->weather->cfd = NULL;
   E_FREE(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob, *ot;
   E_Radio_Group *rg, *dg;
   
   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, _("Display Settings"), 0);   
   rg = e_widget_radio_group_new(&(cfdata->display));
   ob = e_widget_radio_add(evas, _("Simple"), SIMPLE_DISPLAY, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Detailed"), DETAILED_DISPLAY, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_label_add(evas, _("Poll Time"));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_slider_add(evas, 1, 0, _("%2.0f minutes"), 15.0, 60.0, 1.0, 0, &(cfdata->poll_time), NULL, 40);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, _("Degree Settings"), 0);      
   dg = e_widget_radio_group_new(&(cfdata->degrees));
   ob = e_widget_radio_add(evas, _("Celcius"), DEGREES_C, dg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Farenheit"), DEGREES_F, dg);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   
   of = e_widget_frametable_add(evas, _("ICAO Code"), 0);
   ob = e_widget_label_add(evas, _("ICAO code"));
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 0, 1, 0);   
   ob = e_widget_entry_add(evas, (&(cfdata->code)));
   e_widget_min_size_set(ob, 100, 1);
   e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 1, 0, 1, 0);
   ob = e_widget_label_add(evas, _("Get your ICAO code at:"));
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 1, 0, 1, 0);
   ob = e_widget_label_add(evas, _("http://www.weather.gov/tg/siteloc.shtml"));
   e_widget_frametable_object_append(of, ob, 0, 2, 2, 1, 1, 0, 1, 0);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   
   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Weather_Face *wf;
   int i, len;
   char tmp[1024];
   
   if (!cfdata->code)
     return 0;
   len = strlen(cfdata->code);
   if (len < 4 || len > 4)
     return 0;
      
   wf = cfd->data;
   wf->conf->display = cfdata->display;
   wf->conf->degrees = cfdata->degrees;
   wf->conf->poll_time = (cfdata->poll_time * 60.0);
   if (wf->conf->code)
     evas_stringshare_del(wf->conf->code);
   wf->conf->code = evas_stringshare_add((char *)toupper(cfdata->code));
       
   e_config_save_queue();
   if (wf->check_timer)
     ecore_timer_interval_set(wf->check_timer, (double)(wf->conf->poll_time));
   
   _weather_display_set(wf, 1);
   return 1;
}
