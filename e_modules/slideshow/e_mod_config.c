#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

typedef struct _Cfg_File_Data Cfg_File_Data;

struct _E_Config_Dialog_Data
{
   int disable_timer;
   double cycle_time;
#ifdef WANT_OSIRIS
   char *theme;
#endif
};

struct _Cfg_File_Data
{
   E_Config_Dialog *cfd;
   char *file;
};

/* Protos */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas,
                                          E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd,
                             E_Config_Dialog_Data *cfdata);

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
   cfdata->cycle_time = s->conf->cycle_time;
   cfdata->disable_timer = s->conf->disable_timer;
#ifdef WANT_OSIRIS
   cfdata->theme = s->conf->theme;
#endif
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
   /* Free the cfdata */
   free(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas,
                      E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *ob, *of;

   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, D_("Cycle Time"), 0);
   ob = e_widget_check_add(evas, D_("Disable Timer"), &(cfdata->disable_timer));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_slider_add(evas, 1, 0, D_("%3.0f seconds"), 5.0, 600.0, 1.0, 0,
                            &(cfdata->cycle_time), NULL, 200);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

#ifdef WANT_OSIRIS
   of = e_widget_framelist_add(evas, D_("Theme"), 0);
   ot = e_widget_table_add(evas, 1);
   ob = e_widget_label_add(evas, D_("Sub-directory to use for backgrounds"));
   e_widget_table_object_append(ot, ob, 0, 0, 1, 1, 0, 0, 1, 0);
   ob = e_widget_label_add(evas, D_("Leave blank for none"));
   e_widget_table_object_append(ot, ob, 0, 1, 1, 1, 0, 0, 1, 0);
   ob = e_widget_entry_add(evas, &cfdata->theme);
   e_widget_table_object_append(ot, ob, 0, 2, 1, 1, 1, 0, 1, 0);
   e_widget_framelist_object_append(of, ot);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
#endif
   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Slide *s;

   s = cfd->data;
   /* Actually take our cfdata settings and apply them in real life */
   e_border_button_bindings_ungrab_all();
   s->conf->cycle_time = cfdata->cycle_time;
   s->conf->disable_timer = cfdata->disable_timer;
#ifdef WANT_OSIRIS
   if (cfdata->theme != NULL)
     s->conf->theme = (char *)evas_stringshare_add(cfdata->theme);
   else
     s->conf->theme = (char *)evas_stringshare_add("");
#endif
   e_config_save_queue();
   e_border_button_bindings_grab_all();

   _slide_cb_config_updated(s);
   return 1;                    /* Apply was OK */
}
