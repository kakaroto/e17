#include <e.h>
#include "e_mod_main.h"

struct _E_Config_Dialog_Data
{
  char *player;
  int show_popup;
};

/* Protos */
static void *_create_data (E_Config_Dialog * cfd);
static void _free_data (E_Config_Dialog * cfd, E_Config_Dialog_Data * cfdata);
static Evas_Object *_basic_create_widgets (E_Config_Dialog * cfd, Evas * evas,
					   E_Config_Dialog_Data * cfdata);
static int _basic_apply_data (E_Config_Dialog * cfd,
			      E_Config_Dialog_Data * cfdata);

void
_config_empris_module (Config_Item * ci)
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

  snprintf (buf, sizeof (buf), "%s/e-module-empris.edj",
	    e_module_dir_get (empris_config->module));
  con = e_container_current_get (e_manager_current_get ());
  cfd =
    e_config_dialog_new (con, D_ ("Empris Configuration"), "Empris",
			 "_e_modules_empris_config_dialog", buf, 0, v, ci);
  empris_config->config_dialog = cfd;
}

static void
_fill_data (Config_Item * ci, E_Config_Dialog_Data * cfdata)
{
  char buf[128];

  cfdata->player = strdup (ci->player);
  cfdata->show_popup = ci->show_popup;
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
  if (!empris_config)
    return;
  empris_config->config_dialog = NULL;
  free (cfdata->player);
  E_FREE (cfdata);
}

static Evas_Object *
_basic_create_widgets (E_Config_Dialog * cfd, Evas * evas,
		       E_Config_Dialog_Data * cfdata)
{
  Evas_Object *o, *of, *ob, *player_entry;
  
  o = e_widget_list_add (evas, 0, 0);

  of = e_widget_frametable_add (evas, D_ ("Configuration"), 1);

  ob = e_widget_label_add (evas, D_ ("Player:"));
  e_widget_frametable_object_append (of, ob, 0, 0, 1, 1, 1, 0, 1, 0);

  player_entry = e_widget_entry_add (evas, &cfdata->player, NULL, NULL, NULL);
  e_widget_frametable_object_append (of, player_entry, 0, 1, 1, 1, 1, 0, 1, 0);
  e_widget_size_min_set (player_entry, 150, 1);

  ob = e_widget_check_add(evas, D_("Show Popup:"), &(cfdata->show_popup));
  e_widget_frametable_object_append(of, ob, 0, 4, 1, 1, 1, 0, 1, 0);

  e_widget_list_object_append (o, of, 1, 1, 0.5);
  return o;
}

static int
_basic_apply_data (E_Config_Dialog * cfd, E_Config_Dialog_Data * cfdata)
{
  Config_Item *ci;

  ci = cfd->data;
  if (ci->player)
    eina_stringshare_del (ci->player);
  ci->player = eina_stringshare_add (cfdata->player);
  ci->show_popup = cfdata->show_popup;
  e_config_save_queue ();
  _empris_config_updated (ci);
  return 1;
}
