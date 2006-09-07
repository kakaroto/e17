#include <e.h>
#include "e_mod_main.h"
#include "alsa_mixer.h"

struct _E_Config_Dialog_Data
{
   Evas_Object *list;
   
   int card_id;
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
   cfdata->card_id = ci->card_id;
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
   Evas_Object *o, *ol, *ot, *ob, *of, *icon;
   Evas_List   *cards, *c;
   Config_Item *ci;
   int          i = 0;

   ci = cfd->data;
     
   cards = alsa_get_cards();
   
   o = e_widget_list_add(evas, 0, 1);
   of = e_widget_framelist_add(evas, _("Sound Cards"), 0);
   ol = e_widget_ilist_add(evas, 48, 48, NULL);
   cfdata->list = ol;
   e_widget_min_size_set(ol, 160, 200);
   if (cards) 
     {
	for (c = cards; c; c = c->next) 
	  {
	     Alsa_Card *card;
	     
	     card = c->data;
	     if (!card) continue;
	     e_widget_ilist_append(ol, NULL, card->real, NULL, NULL, NULL);
	     if (!ci) continue;
	     if (ci->card_id == card->id)
	       e_widget_ilist_selected_set(ol, i);
	     i++;
	  }
     }
   e_widget_ilist_go(ol);
   e_widget_framelist_object_append(of, ol);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   
   ot = e_widget_table_add(evas, 0);
   ob = e_widget_button_add(evas, _("Configure"), NULL, NULL, NULL, NULL);
   e_widget_disabled_set(ob, 1);
   e_widget_table_object_append(ot, ob, 0, 0, 1, 1, 1, 1, 1, 0);
   
   e_widget_list_object_append(o, ot, 1, 1, 0.0);
   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Evas_List   *cards, *c;
   Alsa_Card   *card;
   Config_Item *ci;

   ci = cfd->data;
   if (!ci) return 0;
   
   cards = alsa_get_cards();
   if (cards) 
     {
	card = evas_list_nth(cards, e_widget_ilist_selected_get(cfdata->list));
	if (!card) return 0;
	ci->card_id = card->id;
	e_config_save_queue();
     }
   
   return 1;
}
