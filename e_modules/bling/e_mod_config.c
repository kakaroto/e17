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
   double fade_in_step;
   double fade_out_step;

   int inactive_transparent;
   double inactive_opacity;
};

/* Protos */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _fill_data(Bling *b, E_Config_Dialog_Data *cfdata);


EAPI E_Config_Dialog *
e_int_config_bling_module(E_Container *con, const char *params __UNUSED__)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   Bling *b;
   char buf[4096];

   b = bling_mod->data;
   if (e_config_dialog_find("E", "_e_modules_bling_config_dialog")) return NULL;
   v = E_NEW(E_Config_Dialog_View, 1);

   
   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;
   v->advanced.apply_cfdata = _advanced_apply_data;
   v->advanced.create_widgets = _advanced_create_widgets;

   snprintf(buf, sizeof(buf), "%s/module.edj", e_module_dir_get(b->module));
   cfd = e_config_dialog_new(con, "Bling Configuration",
                             "Bling", "_e_modules_bling_config_dialog", buf, 0, v, b);

   b->config_dialog = cfd;
   return cfd;
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
   cfdata->fade_in_step = b->config->fx_fade_in_step * 100.0;
   cfdata->fade_out_step = b->config->fx_fade_out_step * 100.0;

   cfdata->inactive_opacity = b->config->trans_inactive_value;
   cfdata->inactive_transparent = (cfdata->inactive_opacity == 100) ? 0 : 1;
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
   of = e_widget_framelist_add(evas, "Composite Settings", 0);
   ob = e_widget_check_add(evas, "Enable Drop Shadows", (&(cfdata->shadow_enable)));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, "Enable Windows In/Out", (&(cfdata->fade_enable)));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, "Inactive Window Opaque", (&(cfdata->inactive_transparent)));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, "Fade On Opacity Changes", (&(cfdata->fade_opacity)));
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

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
   b->config->trans_inactive_value = cfdata->inactive_transparent ? 75 : 100;

   e_config_save_queue();
   bling_composite_restart(b);
   
   return 1;
}

static Evas_Object *
_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob, *ot;
   int i = 0;
   
   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, "Drop Shadow Settings", 0);
   ob = e_widget_check_add(evas, "Enable Drop Shadows", (&(cfdata->shadow_enable)));
   e_widget_framelist_object_append(of, ob);
   
   ot = e_widget_table_add(evas, 0);
   
   ob = e_widget_label_add(evas, "Shadow Size");
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 0, 0, 0, 0);
   ob = e_widget_slider_add(evas, 1, 0, "%.0f pixels", 1, 40, 1, 0, NULL, &(cfdata->shadow_size), 150);
   e_widget_slider_value_double_set(ob, cfdata->shadow_size);
   e_widget_table_object_append(ot, ob, 1, i, 1, 1, 0, 0, 1, 0);
   i++;

   ob = e_widget_label_add(evas, "Shadow Opacity");
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 0, 0, 0, 0);
   ob = e_widget_slider_add(evas, 1, 0, "%1.2f\%", 0, 1, 0.05, 0, &(cfdata->shadow_opacity), NULL, 150);
   e_widget_table_object_append(ot, ob, 1, i, 1, 1, 0, 0, 1, 0);
   i++;

   ob = e_widget_label_add(evas, "Horizontal Offset");
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 0, 0, 0, 0);
   ob = e_widget_slider_add(evas, 1, 0, "%.0f pixels", 1, 40, 1, 0, NULL, &(cfdata->shadow_horz_offset), 150);
   e_widget_table_object_append(ot, ob, 1, i, 1, 1, 0, 0, 1, 0);
   i++;

   ob = e_widget_label_add(evas, "Vertical Offset");
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 0, 0, 0, 0);
   ob = e_widget_slider_add(evas, 1, 0, "%.0f pixels", 1, 40, 1, 0, NULL, &(cfdata->shadow_vert_offset), 150);
   e_widget_table_object_append(ot, ob, 1, i, 1, 1, 0, 0, 1, 0);

   e_widget_framelist_object_append(of, ot);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, "Fade Settings", 0);
   ob = e_widget_check_add(evas, "Fade Windows In/Out", (&(cfdata->fade_enable)));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, "Fade On Opacity Changes", (&(cfdata->fade_opacity)));
   e_widget_framelist_object_append(of, ob);
   
   ot = e_widget_table_add(evas, 0);
   ob = e_widget_label_add(evas, "Fade In Speed");
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 0, 0, 0, 0);
   ob = e_widget_slider_add(evas, 1, 0, "%.0f", 1.0, 100.0, 1, 0, &(cfdata->fade_in_step), NULL, 150);
   e_widget_table_object_append(ot, ob, 1, i, 1, 1, 0, 0, 1, 0);
   i++;

   ob = e_widget_label_add(evas, "Fade Out Speed");
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 0, 0, 0, 0);
   ob = e_widget_slider_add(evas, 1, 0, "%.0f", 1.0, 100.0, 1, 0, &(cfdata->fade_out_step), NULL, 150);
   e_widget_table_object_append(ot, ob, 1, i, 1, 1, 0, 0, 1, 0);
   i++;

   ob = e_widget_label_add(evas, "Window Opacity");
   e_widget_table_object_append(ot, ob, 0, i, 1, 1, 0, 0, 0, 0);
   ob = e_widget_slider_add(evas, 1, 0, "%.0f", 0.0, 100.0, 1, 0, &(cfdata->inactive_opacity), NULL, 150);
   e_widget_table_object_append(ot, ob, 1, i, 1, 1, 0, 0, 1, 0);

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
   b->config->fx_fade_in_step = cfdata->fade_in_step/100.0;
   b->config->fx_fade_out_step = cfdata->fade_out_step/100.0;

   b->config->trans_inactive_value = cfdata->inactive_opacity;
   
   e_config_save_queue();
   bling_composite_restart(b);
   
   return 1;
}

