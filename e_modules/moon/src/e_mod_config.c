/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

#include <e.h>
#include <e_config.h>
#include <e_config_dialog.h>
#include "e_mod_main.h"

#define POLICY_NO            0
#define POLICY_YES           1
#define POLICY_ON_MOUSE_OVER 2

struct _E_Config_Dialog_Data
{
   int show_phase_value; 
   int value_format;
   int show_border; 
   int show_cloud_anim; 
   double poll_time;
};

static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);

static E_Config_DD *conf_edd = NULL;
static E_Module *module = NULL;
Config *moon_config = NULL;

EAPI int
e_modapi_save(E_Module *m)
{  
   return e_config_domain_save("module.moon", conf_edd, moon_config);
}

void
moon_config_init(E_Module *m)
{ 
   module = m;
   conf_edd = E_CONFIG_DD_NEW("Moon_Config", Config);
   E_CONFIG_VAL(conf_edd, Config, show_phase_value, INT);
   E_CONFIG_VAL(conf_edd, Config, value_format, INT);
   E_CONFIG_VAL(conf_edd, Config, show_border, INT);
   E_CONFIG_VAL(conf_edd, Config, show_cloud_anim, INT);
   E_CONFIG_VAL(conf_edd, Config, poll_time, DOUBLE);
   moon_config = e_config_domain_load("module.moon", conf_edd);

   if (!moon_config)
     {
	moon_config = E_NEW(Config, 1);
	moon_config->show_phase_value = POLICY_NO;
	moon_config->value_format = VALUE_FMT_NUM;
	moon_config->show_border = POLICY_YES;
	moon_config->show_cloud_anim = POLICY_NO;
	moon_config->poll_time = 60.0;
     }
   E_CONFIG_LIMIT(moon_config->show_phase_value, POLICY_NO, POLICY_ON_MOUSE_OVER);
   E_CONFIG_LIMIT(moon_config->value_format, VALUE_FMT_STR, VALUE_FMT_NUM);
   E_CONFIG_LIMIT(moon_config->show_border, POLICY_NO, POLICY_YES);
   E_CONFIG_LIMIT(moon_config->show_cloud_anim, POLICY_NO, POLICY_YES);
   E_CONFIG_LIMIT(moon_config->poll_time, 0.05, 86400.0);
}

void
moon_config_shutdown()
{
   if (moon_config->config_dialog)
     {
	e_object_del(E_OBJECT(moon_config->config_dialog));
	moon_config->config_dialog = NULL;
     }
   free(moon_config);
   moon_config = NULL;

   E_CONFIG_DD_FREE(conf_edd);
   module = NULL;
}

void 
moon_config_dialog_show(Evas_Object *o)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   E_Container *con;
   char buf[4096];

   v = E_NEW(E_Config_Dialog_View, 1);
   if (v)
     {
	v->create_cfdata           = _create_data;
	v->free_cfdata             = _free_data;
	v->basic.apply_cfdata      = _basic_apply_data;
	v->basic.create_widgets    = _basic_create_widgets;
	v->advanced.apply_cfdata   = NULL;
	v->advanced.create_widgets = NULL; 
	
	con = e_container_current_get(e_manager_current_get());
	snprintf(buf, sizeof(buf), "%s/e-module-moon.edj", e_module_dir_get(module));
	cfd = e_config_dialog_new(con, D_("Moon Configuration"), "Moon", "_e_modules_moon_config_dialog", buf, 0, v, o);
	moon_config->config_dialog = cfd;
     }
}

/**--CREATE--**/
static void
_fill_data(E_Config_Dialog_Data *cfdata)
{
   cfdata->show_phase_value = moon_config->show_phase_value;
   cfdata->value_format = moon_config->value_format;
   cfdata->show_border = moon_config->show_border;
   cfdata->show_cloud_anim = moon_config->show_cloud_anim;
   cfdata->poll_time = moon_config->poll_time;
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;
        
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(cfdata);
   return cfdata;
}

static void 
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   /* Free the cfdata */
   moon_config->config_dialog = NULL;
   free(cfdata);
}

static int 
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   moon_config->show_phase_value = cfdata->show_phase_value;
   moon_config->value_format = cfdata->value_format;
   moon_config->show_border = cfdata->show_border;
   moon_config->show_cloud_anim = cfdata->show_cloud_anim;
   moon_config->poll_time = cfdata->poll_time;

   e_config_save_queue();
   moon_reset(cfd->data);
   return 1; /* Apply was OK */
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *ob, *of, *o;
   E_Radio_Group *rg;

   o = e_widget_list_add(evas, 0, 0);

   of = e_widget_framelist_add(evas, D_("Display Settings"), 0);
   ob = e_widget_check_add(evas, D_("Animated Clouds"), &(cfdata->show_cloud_anim));
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, D_("Moon Phase Value"), 0); 
   rg = e_widget_radio_group_new(&(cfdata->show_phase_value));
   ob = e_widget_radio_add(evas, D_("Show always"), POLICY_YES, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, D_("Show on mouse over"), POLICY_ON_MOUSE_OVER, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, D_("Never show"), POLICY_NO, rg);
   e_widget_framelist_object_append(of, ob);

   rg = e_widget_radio_group_new(&(cfdata->value_format));
   ob = e_widget_radio_add(evas, D_("Description"), VALUE_FMT_STR, rg);
   e_widget_disabled_set(ob, (!cfdata->show_phase_value));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, D_("Numerical"), VALUE_FMT_NUM, rg);
   e_widget_disabled_set(ob, (!cfdata->show_phase_value));
   e_widget_framelist_object_append(of, ob);

   e_widget_list_object_append(o, of, 1, 1, 0.5);

/*
   of = e_widget_framelist_add(evas, D_("Moon Phase Calculation Interval"), 0);
   ob = e_widget_slider_add(evas, 1, 0, D_("%1.2f seconds"), 0.05, 120.00, 0.05, 0, &(cfdata->poll_time), NULL, 200);
   e_widget_framelist_object_append(of, ob);

   e_widget_list_object_append(o, of, 1, 1, 0.5);
*/

   return o;
}

