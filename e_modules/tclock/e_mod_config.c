#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"

struct _E_Config_Dialog_Data
{
   unsigned int resolution;
   char *format;
   int userformat;
};

/* Protos */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void onCheckChange(void *data, Evas_Object *obj);

void
_config_tclock_module(E_Container *con, TClock_Face *f)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;

   v = E_NEW(E_Config_Dialog_View, 1);

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;

   cfd = e_config_dialog_new(con, D_("Tclock Configuration"), NULL, 0, v, f);
}

static void
_fill_data(TClock_Face *f, E_Config_Dialog_Data *cfdata)
{
   cfdata->resolution = f->conf->resolution;
   if (f->conf->format)
      cfdata->format = strdup(f->conf->format);
   cfdata->userformat = f->conf->userformat;
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;
   TClock_Face *f;

   f = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);

   _fill_data(f, cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   free(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob, *entry, *checked;
   E_Radio_Group *rg;

   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, D_("Resolution"), 0);
   rg = e_widget_radio_group_new(&(cfdata->resolution));
   ob = e_widget_radio_add(evas, D_("1 Minute"), RESOLUTION_MINUTE, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, D_("1 Second"), RESOLUTION_SECOND, rg);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_frametable_add(evas, D_("Time Format"), 0);
   checked = e_widget_check_add(evas, D_("User format String"), &(cfdata->userformat));
   if (cfdata->userformat)
      e_widget_check_checked_set(checked, 1);
   e_widget_frametable_object_append(of, checked, 0, 0, 1, 1, 1, 0, 1, 0);
   ob = e_widget_label_add(evas, D_("Consult strftime(3) for format syntax"));
   e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 1, 0, 1, 0);
   entry = e_widget_entry_add(evas, &(cfdata->format));
   e_widget_disabled_set(entry, !cfdata->userformat);
   e_widget_min_size_set(entry, 150, 1);
   e_widget_frametable_object_append(of, entry, 0, 1, 1, 1, 1, 0, 1, 0);
   ob = e_widget_label_add(evas, "");
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 1, 0, 1, 0);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   e_widget_on_change_hook_set(checked, onCheckChange, entry);

   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   TClock_Face *f;

   f = cfd->data;
   e_border_button_bindings_ungrab_all();
   if (cfdata->resolution != f->conf->resolution)
     {
        if (cfdata->resolution == RESOLUTION_MINUTE)
           f->tclock->conf->poll_time = 60.0;
        else
           f->tclock->conf->poll_time = 1.0;
        ecore_timer_interval_set(f->tclock->tclock_check_timer, f->tclock->conf->poll_time);
     }
   f->conf->resolution = cfdata->resolution;
   f->conf->userformat = cfdata->userformat;

   /* update the format string with the format string given by the user
    * when the userformat is true and the string(typed by the user) isn't empty */
   if (strlen(cfdata->format) > 0 && cfdata->userformat)
     {
        if (f->conf->format)
           evas_stringshare_del(f->conf->format);
        f->conf->format = (char *)evas_stringshare_add(cfdata->format);
     }

   e_config_save_queue();
   e_border_button_bindings_grab_all();
   return 1;
}

static void
onCheckChange(void *data, Evas_Object *obj)
{
   int checked = e_widget_check_checked_get(obj);

   e_widget_disabled_set(data, !checked);
}
