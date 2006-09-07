#include <e.h>
#include "e_mod_main.h"
#include "alsa_mixer.h"

struct _E_Config_Dialog_Data
{

};

/* Protos */
static void        *_create_data          (E_Config_Dialog * cfd);
static void         _free_data            (E_Config_Dialog * cfd, 
					   E_Config_Dialog_Data * cfdata);
static Evas_Object *_basic_create_widgets (E_Config_Dialog * cfd, Evas * evas, 
					   E_Config_Dialog_Data * cfdata);
static int          _basic_apply_data     (E_Config_Dialog * cfd,
					   E_Config_Dialog_Data * cfdata);

void
_config_mixer_module(Config_Item *ci)
{
   E_Config_Dialog      *cfd;
   E_Config_Dialog_View *v;
   E_Container          *con;
   char                  buf[4096];

   v = E_NEW(E_Config_Dialog_View, 1);
   if (!v) return;
   
   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;

   snprintf(buf, sizeof(buf), "%s/module.eap",
	     e_module_dir_get(mixer_config->module));

   con = e_container_current_get(e_manager_current_get());
   cfd = e_config_dialog_new(con, _("Mixer Configuration"), "Mixer", 
			     "_mixer_config_dialog", buf, 0, v, ci);
   mixer_config->config_dialog = cfd;
}

static void
_fill_data(Config_Item *ci, E_Config_Dialog_Data *cfdata)
{

}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;
   Config_Item          *ci;

   ci = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(ci, cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   if (!mixer_config) return;
   mixer_config->config_dialog = NULL;
   E_FREE(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *ol, *ot, *ob, *icon;
   Evas_List   *cards, *c;

   cards = alsa_get_cards();
   
   o = e_widget_list_add(evas, 0, 0);
   ot = e_widget_table_add(evas, 0);
   
   ol = e_widget_ilist_add(evas, 48, 48, NULL);
   e_widget_min_size_set(ol, 160, 200);
   if (cards) 
     {
	for (c = cards; c; c = c->next) 
	  {
	     Alsa_Card *card;
	     
	     card = c->data;
	     if (!c) continue;
	     e_widget_ilist_append(ol, NULL, card->real, NULL, NULL, NULL);
	  }
     }
   e_widget_ilist_go(ol);
   e_widget_table_object_append(ot, ol, 0, 0, 1, 1, 1, 0, 1, 0);
   
   ob = e_widget_button_add(evas, _("Configure"), NULL, NULL, NULL, NULL);
   e_widget_disabled_set(ob, 1);
   e_widget_table_object_append(ot, ob, 0, 1, 1, 1, 1, 0, 1, 0);
   
   e_widget_list_object_append(o, ot, 1, 1, 0.5);
   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   return 1;
}
