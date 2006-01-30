#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

typedef struct _Cfg_File_Data Cfg_File_Data;

struct _E_Config_Dialog_Data
{
   int palette;
   int r, g, b;
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
static Evas_Object *_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas,
                                             E_Config_Dialog_Data *cfdata);
static int _advanced_apply_data(E_Config_Dialog *cfd,
                                E_Config_Dialog_Data *cfdata);

void
_config_flame_module(E_Container *con, Flame *fl)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;

   v = E_NEW(E_Config_Dialog_View, 1);

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;
   v->advanced.apply_cfdata = _advanced_apply_data;
   v->advanced.create_widgets = _advanced_create_widgets;

   cfd = e_config_dialog_new(con, _("Flame Configuration"), NULL, 0, v, fl);
   fl->config_dialog = cfd;
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;

   cfdata = E_NEW(E_Config_Dialog_Data, 1);

   return cfdata;
}

static void
_fill_data(Flame *f, E_Config_Dialog_Data *cfdata)
{
   cfdata->palette = f->conf->palette_type;
   cfdata->r = f->conf->r;
   cfdata->g = f->conf->g;
   cfdata->b = f->conf->b;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Flame *f;

   f = cfd->data;
   f->config_dialog = NULL;
   free(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas,
                      E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob;
   E_Radio_Group *rg;
   Flame *f;

   f = cfd->data;
   _fill_data(f, cfdata);

   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, _("Flame Color"), 0);
   rg = e_widget_radio_group_new(&(cfdata->palette));
   ob = e_widget_radio_add(evas, _("Gold"), GOLD_PALETTE, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Fire"), FIRE_PALETTE, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Plasma"), PLASMA_PALETTE, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Matrix"), MATRIX_PALETTE, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Ice"), ICE_PALETTE, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Custom"), CUSTOM_PALETTE, rg);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Flame *f;

   f = cfd->data;
   e_border_button_bindings_ungrab_all();
   switch (cfdata->palette)
     {
     case 1:
        f->conf->palette_type = GOLD_PALETTE;   // Gold
        break;
     case 2:
        f->conf->palette_type = FIRE_PALETTE;   //Fire
        f->conf->r = 0;
        f->conf->g = 80;
        f->conf->b = 160;
        break;
     case 3:
        f->conf->palette_type = PLASMA_PALETTE; //Plasma
        f->conf->r = 0;
        f->conf->g = 0;
        f->conf->b = 80;
        break;
     case 4:
        f->conf->palette_type = MATRIX_PALETTE; //Matrix
        f->conf->r = 80;
        f->conf->g = 0;
        f->conf->b = 160;
        break;
     case 5:
        f->conf->palette_type = ICE_PALETTE;    //Ice
        f->conf->r = 160;
        f->conf->g = 40;
        f->conf->b = 0;
        break;
     case 6:
        f->conf->palette_type = CUSTOM_PALETTE; //Custom
        break;
     default:
        break;
     }
   e_config_save_queue();
   e_border_button_bindings_grab_all();

   _flame_cb_config_updated(f);
   return 1;
}

static Evas_Object *
_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas,
                         E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob;
   Flame *f;

   f = cfd->data;
   _fill_data(f, cfdata);

   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, _("Flame Color"), 0);

   ob = e_widget_label_add(evas, _("Red Amount"));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 300, 0, -10, 0, NULL,
                            &(cfdata->r), 200);
   e_widget_framelist_object_append(of, ob);

   ob = e_widget_label_add(evas, _("Blue Amount"));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 0, 300, 10, 0, NULL,
                            &(cfdata->b), 200);
   e_widget_framelist_object_append(of, ob);

   ob = e_widget_label_add(evas, _("Green Amount"));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 0, 300, 10, 0, NULL,
                            &(cfdata->g), 200);
   e_widget_framelist_object_append(of, ob);

   e_widget_list_object_append(o, of, 1, 1, 0.5);
   return o;
}

static int
_advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Flame *f;

   f = cfd->data;
   e_border_button_bindings_ungrab_all();

   f->conf->palette_type = CUSTOM_PALETTE;      //Custom
   f->conf->r = cfdata->r;
   f->conf->g = cfdata->g;
   f->conf->b = cfdata->b;

   e_config_save_queue();
   e_border_button_bindings_grab_all();

   _flame_cb_config_updated(f);
   return 1;
}
