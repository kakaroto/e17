#include <e.h>
#include "e_mod_main.h"

struct _E_Config_Dialog_Data
{
  int poll_time;
  char *hostname;
  char *port;
};

/* Protos */
static void *_create_data (E_Config_Dialog * cfd);
static void _free_data (E_Config_Dialog * cfd, E_Config_Dialog_Data * cfdata);
static Evas_Object *_basic_create_widgets (E_Config_Dialog * cfd, Evas * evas,
					   E_Config_Dialog_Data * cfdata);
static int _basic_apply_data (E_Config_Dialog * cfd,
			      E_Config_Dialog_Data * cfdata);

void
_config_mpdule_module (Config_Item * ci)
{
  E_Config_Dialog *cfd;
  E_Config_Dialog_View *v;
  E_Container *con;
  char buf[4096];

  v = E_NEW (E_Config_Dialog_View, 1);

  v->create_cfdata = _create_data;
  v->free_cfdata = _free_data;
  v->basic.apply_cfdata = _basic_apply_data;
  v->basic.create_widgets = _basic_create_widgets;

  snprintf (buf, sizeof (buf), "%s/e-module-mpdule.edj",
	    e_module_dir_get (mpdule_config->module));
  con = e_container_current_get (e_manager_current_get ());
  cfd =
    e_config_dialog_new (con, D_ ("MPDule Configuration"), "MPDule",
			 "_e_modules_mpdule_config_dialog", buf, 0, v, ci);
  mpdule_config->config_dialog = cfd;
}

static void
_fill_data (Config_Item * ci, E_Config_Dialog_Data * cfdata)
{
  char buf[128];

  cfdata->poll_time = ci->poll_time;
  cfdata->hostname = strdup (ci->hostname);
  snprintf (buf, sizeof (buf), "%d", ci->port);
  cfdata->port = strdup (buf);
}

static void *
_create_data (E_Config_Dialog * cfd)
{
  E_Config_Dialog_Data *cfdata;
  Config_Item *ci;

  ci = cfd->data;
  cfdata = E_NEW (E_Config_Dialog_Data, 1);

  _fill_data (ci, cfdata);
  return cfdata;
}

static void
_free_data (E_Config_Dialog * cfd, E_Config_Dialog_Data * cfdata)
{
  if (!mpdule_config)
    return;
  mpdule_config->config_dialog = NULL;
  free (cfdata->hostname);
  E_FREE (cfdata);
}

static Evas_Object *
_basic_create_widgets (E_Config_Dialog * cfd, Evas * evas,
		       E_Config_Dialog_Data * cfdata)
{
  Evas_Object *o, *of, *ob, *hostname_entry,
    *port_entry;

  o = e_widget_list_add (evas, 0, 0);

  of = e_widget_frametable_add (evas, D_ ("Configuration"), 1);
  ob = e_widget_label_add (evas, D_ ("Poll Time:"));
  e_widget_frametable_object_append (of, ob, 0, 0, 1, 1, 1, 0, 1, 0);
  ob =
    e_widget_slider_add (evas, 1, 0, D_ ("%1.0f seconds"), 1, 60, 1, 0, NULL,
			 &(cfdata->poll_time), 100);
  e_widget_frametable_object_append (of, ob, 0, 1, 1, 1, 1, 0, 1, 0);
  ob = e_widget_label_add (evas, D_ ("Hostname:"));
  e_widget_frametable_object_append (of, ob, 0, 2, 1, 1, 1, 0, 1, 0);
  hostname_entry =
    e_widget_entry_add (evas, &cfdata->hostname, NULL, NULL, NULL);
  e_widget_min_size_set (hostname_entry, 150, 1);
  e_widget_frametable_object_append (of, hostname_entry, 0, 3, 1, 1, 1, 0, 1,
				     0);
  ob = e_widget_label_add (evas, D_ ("Port:"));
  e_widget_frametable_object_append (of, ob, 0, 4, 1, 1, 1, 0, 1, 0);
  port_entry = e_widget_entry_add (evas, &cfdata->port, NULL, NULL, NULL);
  e_widget_min_size_set (hostname_entry, 150, 1);
  e_widget_frametable_object_append (of, port_entry, 0, 5, 1, 1, 1, 0, 1, 0);
  e_widget_list_object_append (o, of, 1, 1, 0.5);

  return o;
}

static int
_basic_apply_data (E_Config_Dialog * cfd, E_Config_Dialog_Data * cfdata)
{
  Config_Item *ci;

  ci = cfd->data;
  ci->poll_time = cfdata->poll_time;
  if (ci->hostname)
    evas_stringshare_del (ci->hostname);
  ci->hostname = evas_stringshare_add (cfdata->hostname);
  ci->port = atoi (cfdata->port);
  e_config_save_queue ();
  _mpdule_config_updated (ci);
  return 1;
}
