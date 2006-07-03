#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

struct _E_Config_Dialog_Data
{
   int check_interval;
   int show_text;
   int show_graph;
};

/* Protos */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _fill_data(Cpu *c, E_Config_Dialog_Data *cfdata);

/* Config Calls */
void
_configure_cpu_module(E_Container *con, Cpu *c)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   char buf[4096];
   
   v = E_NEW(E_Config_Dialog_View, 1);

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;

   snprintf(buf, sizeof(buf), "%s/module.eap", e_module_dir_get(c->module));
   cfd = e_config_dialog_new(con, D_("Cpu Configuration"), buf, 0, v, c);
   c->cfd = cfd;
}

static void
_fill_data(Cpu *c, E_Config_Dialog_Data *cfdata)
{
   cfdata->check_interval = c->conf->check_interval;
   cfdata->show_text = c->conf->show_text;
   cfdata->show_graph = c->conf->show_graph;
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;
   Cpu *c;

   c = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);

   _fill_data(c, cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Cpu *c;

   c = cfd->data;
   c->cfd = NULL;
   free(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob;
   Cpu *c;

   c = cfd->data;

   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, D_("Cpu Settings"), 0);
   ob = e_widget_check_add(evas, D_("Show Text"), (&(cfdata->show_text)));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, D_("Show Graph"), (&(cfdata->show_graph)));
   e_widget_framelist_object_append(of, ob);

   ob = e_widget_label_add(evas, D_("Check Interval:"));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f seconds"), 1, 60, 1, 0, NULL, &(cfdata->check_interval), 150);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   char *tmp;
   Cpu *c;

   c = cfd->data;
   c->conf->check_interval = cfdata->check_interval;
   c->conf->show_text = cfdata->show_text;
   c->conf->show_graph = cfdata->show_graph;
   e_config_save_queue();
   if (c->face->monitor)
      ecore_timer_interval_set(c->face->monitor, (double)cfdata->check_interval);

   return 1;
}
