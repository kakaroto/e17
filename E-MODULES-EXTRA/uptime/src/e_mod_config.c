#include <e.h>
#include "e_mod_main.h"

#include <stdlib.h>
#include <stdint.h>
#include <err.h>
#include <sys/types.h>
#include <sys/time.h>

struct _E_Config_Dialog_Data
{
  double check_interval;
  double update_interval;
};

/* Protos */
static void *_create_data (E_Config_Dialog * cfd);
static void _free_data (E_Config_Dialog * cfd, E_Config_Dialog_Data * cfdata);
static Evas_Object *_basic_create_widgets (E_Config_Dialog * cfd, Evas * evas,
					   E_Config_Dialog_Data * cfdata);
static int _basic_apply_data (E_Config_Dialog * cfd,
			      E_Config_Dialog_Data * cfdata);
static void _fill_data (Config_Item * ci, E_Config_Dialog_Data * cfdata);

/* Config Calls */
void
_config_ut_module (Config_Item * ci)
{
  E_Config_Dialog *cfd;
  E_Config_Dialog_View *v;
  E_Container *con;
  char buf[4096];

  if (!ut_config)
    return;

  v = E_NEW (E_Config_Dialog_View, 1);
  if (v == NULL)
    {
      warn ("malloc()");
      return;
    }

  v->create_cfdata = _create_data;
  v->free_cfdata = _free_data;
  v->basic.apply_cfdata = _basic_apply_data;
  v->basic.create_widgets = _basic_create_widgets;

  snprintf (buf, sizeof (buf), "%s/e-module-uptime.edj",
	    e_module_dir_get (ut_config->module));
  con = e_container_current_get (e_manager_current_get ());
  cfd = e_config_dialog_new (con, D_ ("Uptime Configuration"), "UpTime", "_e_modules_uptime_config_dialog", buf, 0, v, ci);
  ut_config->config_dialog = cfd;
}

static void
_fill_data (Config_Item * ci, E_Config_Dialog_Data * cfdata)
{
  if (!cfdata)
    return;
  cfdata->check_interval = ci->check_interval;
  cfdata->update_interval = ci->update_interval;
}

static void *
_create_data (E_Config_Dialog * cfd)
{
  E_Config_Dialog_Data *cfdata;
  Config_Item *ci;

  if (!cfd)
    return NULL;

  ci = cfd->data;
  cfdata = E_NEW (E_Config_Dialog_Data, 1);
  if (cfdata == NULL)
    {
      warn ("malloc()");
      return NULL;
    }

  _fill_data (ci, cfdata);

  return cfdata;
}

static void
_free_data (E_Config_Dialog * cfd, E_Config_Dialog_Data * cfdata)
{
  if (ut_config == NULL)
    return;
  ut_config->config_dialog = NULL;

  free (cfdata);
  cfdata = NULL;
}

static Evas_Object *
_basic_create_widgets (E_Config_Dialog * cfd, Evas * evas,
		       E_Config_Dialog_Data * cfdata)
{
  Evas_Object *o, *of, *ob;

  o = e_widget_list_add (evas, 0, 0);
  of = e_widget_framelist_add (evas, D_ ("Uptime Settings"), 0);

  ob = e_widget_label_add (evas, D_ ("Update Interval:"));
  e_widget_framelist_object_append (of, ob);

  ob = e_widget_slider_add (evas, 1, 0, D_ ("%1.0f seconds"), 1.0, 60.0,
			    1.0, 0, &(cfdata->update_interval), NULL, 100);
  e_widget_framelist_object_append (of, ob);

  ob = e_widget_label_add (evas, D_ ("Check Interval:"));
  e_widget_framelist_object_append (of, ob);

  ob = e_widget_slider_add (evas, 1, 0, D_ ("%1.0f seconds"), 1.0, 60.0,
			    1.0, 0, &(cfdata->check_interval), NULL, 100);
  e_widget_framelist_object_append (of, ob);

  e_widget_list_object_append (o, of, 1, 1, 0.5);

  return o;
}

static int
_basic_apply_data (E_Config_Dialog * cfd, E_Config_Dialog_Data * cfdata)
{
  Config_Item *ci;

  if (!cfd)
    return 0;
  ci = cfd->data;
  ci->check_interval = cfdata->check_interval;
  ci->update_interval = cfdata->update_interval;
  e_config_save_queue ();
  _ut_config_updated (ci);

  return 1;
}
