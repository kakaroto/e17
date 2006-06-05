#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

struct _E_Config_Dialog_Data
{
   int shadow_enable;
   int shadow_size;
   double shadow_opacity;
   int shadow_vert_offset;
   int shadow_horz_offset;

   int fade_enable;
   int fade_opacity;
   double fade_frame_rate;
   double fade_in_speed;
   double fade_out_speed;
};

/* Protos */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _fill_data(Bling *b, E_Config_Dialog_Data *cfdata);


void
_config_bling_module(E_Container *con, Bling *b)
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

   cfd = e_config_dialog_new(con, D_("Bling Configuration"), NULL, 0, v, b);
   b->config_dialog = cfd;
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;
   Bling *b;
   
   b = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);

   _fill_data(b, cfdata);
   return cfdata;
}

static void
_fill_data(Bling *b, E_Config_Dialog_Data *cfdata)
{
   cfdata->shadow_enable = b->config->shadow_enable;
   cfdata->shadow_size = b->config->shadow_active_size;
   cfdata->shadow_opacity = b->config->shadow_opacity;
   cfdata->shadow_horz_offset = b->config->shadow_horz_offset;
   cfdata->shadow_vert_offset = b->config->shadow_vert_offset;

   cfdata->fade_enable = b->config->fx_fade_enable;
   cfdata->fade_opacity = b->config->fx_fade_opacity_enable;
   cfdata->fade_frame_rate = 1.0/b->config->fx_fade_delta;
   cfdata->fade_in_speed = b->config->fx_fade_in_speed * 100.0;
   cfdata->fade_out_speed = b->config->fx_fade_out_speed * 100.0;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Bling *b;

   b = cfd->data;
   b->config_dialog = NULL;
   free(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob;

   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, D_("Composite Settings"), 0);
   ob = e_widget_check_add(evas, D_("Enable Drop Shadows"), (&(cfdata->shadow_enable)));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, D_("Enable Windows In/Out"), (&(cfdata->fade_enable)));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, D_("Fade On Opacity Changes"), (&(cfdata->fade_opacity)));
   e_widget_framelist_object_append(of, ob);

   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Bling *b;

   b = cfd->data;

   b->config->shadow_enable = cfdata->shadow_enable;
   b->config->fx_fade_enable = cfdata->fade_enable;
   b->config->fx_fade_opacity_enable = cfdata->fade_opacity;

   /* Reload composite manager! */
   return 1;
}

static Evas_Object *
_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob, *ot;
   int i = 0;
   
   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, D_("Drop Shadow Settings"), 0);
   ob = e_widget_check_add(evas, D_("Enable Drop Shadows"), (&(cfdata->shadow_enable)));
   e_widget_framelist_object_append(of, ob);
   
   ot = e_widget_table_add(evas, 0);
   
   ob = e_widget_label_add(evas, D_("Shadow Size"));
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 0, 0, 1, 0);
   i++;
   ob = e_widget_slider_add(evas, 1, 0, _("%d pixels"), 1, 40, 1, 0, NULL, &(cfdata->shadow_size), 150);
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 1, 0, 1, 0);
   i++;

   ob = e_widget_label_add(evas, D_("Shadow Opacity"));
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 0, 0, 1, 0);
   i++;
   ob = e_widget_slider_add(evas, 1, 0, _("%1.00f\%"), 0, 1, 0.05, 0, &(cfdata->shadow_opacity), NULL, 150);
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 1, 0, 1, 0);
   i++;

   ob = e_widget_label_add(evas, D_("Horizontal Offset"));
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 0, 0, 1, 0);
   i++;
   ob = e_widget_slider_add(evas, 1, 0, _("%d pixels"), 1, 40, 1, 0, NULL, &(cfdata->shadow_horz_offset), 150);
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 1, 0, 1, 0);
   i++;

   ob = e_widget_label_add(evas, D_("Vertical Offset"));
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 0, 0, 1, 0);
   i++;
   ob = e_widget_slider_add(evas, 1, 0, _("%d pixels"), 1, 40, 1, 0, NULL, &(cfdata->shadow_vert_offset), 150);
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 1, 0, 1, 0);

   e_widget_framelist_object_append(of, ot);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, D_("Fade Settings"), 0);
   ob = e_widget_check_add(evas, D_("Fade Windows In/Out"), (&(cfdata->fade_enable)));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, D_("Fade On Opacity Changes"), (&(cfdata->fade_opacity)));
   e_widget_framelist_object_append(of, ob);
   
   ot = e_widget_table_add(evas, 0);
   i = 0;
   ob = e_widget_label_add(evas, D_("Fader Frame Rate"));
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 0, 0, 1, 0);
   i++;
   ob = e_widget_slider_add(evas, 1, 0, _("%1.00f\% fps"), 1.0, 60.0, 5.0, 0, &(cfdata->fade_frame_rate), NULL, 150);
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 1, 0, 1, 0);
   i++;

   ob = e_widget_label_add(evas, D_("Fade In Speed"));
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 0, 0, 1, 0);
   i++;
   ob = e_widget_slider_add(evas, 1, 0, _("%d"), 1.0, 100.0, 1, 0, &(cfdata->fade_in_speed), NULL, 150);
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 1, 0, 1, 0);
   i++;

   ob = e_widget_label_add(evas, D_("Fade Out Speed"));
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 0, 0, 1, 0);
   i++;
   ob = e_widget_slider_add(evas, 1, 0, _("%d"), 1.0, 100.0, 1, 0, &(cfdata->fade_out_speed), NULL, 150);
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 1, 0, 1, 0);

   e_widget_framelist_object_append(of, ot);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}

static int
_advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Bling *b;

   b = cfd->data;
   b->config->shadow_enable = cfdata->shadow_enable;
   b->config->shadow_active_size = cfdata->shadow_size;
   b->config->shadow_opacity = cfdata->shadow_opacity;
   b->config->shadow_horz_offset = cfdata->shadow_horz_offset;
   b->config->shadow_vert_offset = cfdata->shadow_vert_offset;

   b->config->fx_fade_enable = cfdata->fade_enable;
   b->config->fx_fade_opacity_enable = cfdata->fade_opacity;
   b->config->fx_fade_delta = 1.0/cfdata->fade_frame_rate;
   b->config->fx_fade_in_speed = cfdata->fade_in_speed/100.0;
   b->config->fx_fade_out_speed = cfdata->fade_out_speed/100.0;

   /* reload comp manager! */
   return 1;
}

