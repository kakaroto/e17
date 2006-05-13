#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

struct _E_Config_Dialog_Data
{
   int disable_timer;
   double cycle_time;
   char *dir;
};

/* Protos */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

/* Config Calls */
void
_config_slideshow_module(E_Container *con, Slide *s)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;

   v = E_NEW(E_Config_Dialog_View, 1);

   /* methods */
   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;

   /* create config diaolg */
   cfd = e_config_dialog_new(con, D_("Slideshow Configuration"), NULL, 0, v, s);
   s->config_dialog = cfd;
}

static void
_fill_data(Slide *s, E_Config_Dialog_Data *cfdata)
{
   char buf[PATH_MAX];
   
   cfdata->cycle_time = s->conf->cycle_time;
   cfdata->disable_timer = s->conf->disable_timer;
   if (s->conf->dir)
     cfdata->dir = s->conf->dir;
   else 
     {
	snprintf(buf, sizeof(buf), "%s/.e/e/backgrounds", e_user_homedir_get());
	cfdata->dir = (char *)evas_stringshare_add(buf);
     }
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;
   Slide *s;

   s = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(s, cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Slide *s;

   s = cfd->data;
   s->config_dialog = NULL;
   free(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *ob, *of, *ot;

   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, D_("Cycle Time"), 0);
   ob = e_widget_check_add(evas, D_("Disable Timer"), &(cfdata->disable_timer));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_slider_add(evas, 1, 0, D_("%3.0f seconds"), 5.0, 600.0, 1.0, 0, &(cfdata->cycle_time), NULL, 200);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, D_("Directory"), 0);
   ot = e_widget_table_add(evas, 1);
   ob = e_widget_label_add(evas, D_("Sub-directory to use for backgrounds"));
   e_widget_table_object_append(ot, ob, 0, 0, 1, 1, 0, 0, 1, 0);
   ob = e_widget_entry_add(evas, &cfdata->dir);
   e_widget_table_object_append(ot, ob, 0, 1, 1, 1, 1, 0, 1, 0);
   e_widget_framelist_object_append(of, ot);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Slide *s;
   char buf[PATH_MAX];

   s = cfd->data;
   e_border_button_bindings_ungrab_all();
   s->conf->cycle_time = cfdata->cycle_time;
   s->conf->disable_timer = cfdata->disable_timer;
   if (cfdata->dir != NULL)
     s->conf->dir = (char *)evas_stringshare_add(cfdata->dir);
   else 
     {
	snprintf(buf, sizeof(buf), "%s/.e/e/backgrounds", e_user_homedir_get());
	s->conf->dir = (char *)evas_stringshare_add(buf);
     }
   
   e_config_save_queue();
   e_border_button_bindings_grab_all();

   _slide_cb_config_updated(s);
   return 1;
}
