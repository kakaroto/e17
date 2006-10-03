#include <e.h>
#include "e_mod_main.h"
#include "e_mod_types.h"

struct _E_Config_Dialog_Data
{
   Evas_Object *list;
   
   int card_id;
   int channel_id;
   int mode;
	
   char *app;
   int use_app;
};

/* Protos */
static void        *_create_data          (E_Config_Dialog * cfd);
static void         _free_data            (E_Config_Dialog * cfd, E_Config_Dialog_Data * cfdata);
static Evas_Object *_basic_create_widgets (E_Config_Dialog * cfd, Evas * evas, E_Config_Dialog_Data * cfdata);
static int          _basic_apply_data     (E_Config_Dialog * cfd, E_Config_Dialog_Data * cfdata);

static Mixer *mixer;

void
_config_mixer_module(void *data, Config_Item *ci)
{
   E_Config_Dialog      *cfd;
   E_Config_Dialog_View *v;
   E_Container          *con;
   char                  buf[4096];
   
   mixer = data;
   
   v = E_NEW(E_Config_Dialog_View, 1);
   if (!v) return;
   
   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;
   v->advanced.apply_cfdata = NULL;
   v->advanced.create_widgets = NULL;
   
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
   cfdata->channel_id = ci->channel_id;
   cfdata->mode = ci->mode;
   cfdata->use_app = ci->use_app;
   if (ci->app != NULL)
     cfdata->app = strdup(ci->app);
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
   Evas_Object   *o, *ob, *of, *ot;
   Evas_List     *cards, *chans;
   Config_Item   *ci;
   E_Radio_Group *cg, *mg, *dg;
   Mixer_Card    *card;

   ci = cfd->data;

   o = e_widget_list_add(evas, 0, 0);

   ot = e_widget_frametable_add(evas, _("Mixer Application"), 1);
   ob = e_widget_check_add(evas, _("Launch Mixer App"), &(cfdata->use_app));
   e_widget_frametable_object_append(ot, ob, 0, 0, 1, 1, 1, 0, 1, 0);
   ob = e_widget_entry_add(evas, &cfdata->app);
   e_widget_frametable_object_append(ot, ob, 0, 1, 1, 1, 1, 0, 1, 0);

   e_widget_list_object_append(o, ot, 1, 1, 0.5);
   
   if (!mixer->mix_sys->cards) 
     {
	if (mixer->mix_sys->get_cards)
	  mixer->mix_sys->cards = mixer->mix_sys->get_cards();
     }
   
   if ((mixer->mix_sys->cards) && (evas_list_count(mixer->mix_sys->cards) > 1))
     {
	of = e_widget_framelist_add(evas, _("Available Cards"), 0);
	cg = e_widget_radio_group_new(&cfdata->card_id);
	for (cards = mixer->mix_sys->cards; cards; cards = cards->next) 
	  {
	     card = cards->data;
	     if (!card) continue;
	     ob = e_widget_radio_add(evas, (char *)card->real, card->id, cg);
	     e_widget_framelist_object_append(of, ob);
	  }
	e_widget_list_object_append(o, of, 1, 1, 0.5);	
     }
     
   if ((mixer->mix_sys->get_card) && (ci->card_id != 0))
     {
	card = mixer->mix_sys->get_card(ci->card_id);
	if ((mixer->mix_sys->get_channels) && (card)) 
	  {
	     card->channels = mixer->mix_sys->get_channels(card);
	     if (card->channels) 
	       {
		  of = e_widget_framelist_add(evas, _("Available Mixers"), 0);
		  mg = e_widget_radio_group_new(&cfdata->channel_id);
		  for (chans = card->channels; chans; chans = chans->next) 
		    {
		       Mixer_Channel *chan;
		  
		       chan = chans->data;
		       if (!chan) continue;
		       ob = e_widget_radio_add(evas, (char *)chan->name, chan->id, mg);
		       e_widget_framelist_object_append(of, ob);
		    }
		  e_widget_list_object_append(o, of, 1, 1, 0.5);
	       }
	  }	
     }
   
   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Config_Item *ci;
   
   ci = cfd->data;
   
   ci->card_id = cfdata->card_id;
   ci->channel_id = cfdata->channel_id;
   ci->mode = cfdata->mode;
   ci->use_app = cfdata->use_app;
   if (ci->app) evas_stringshare_del(ci->app);
   if (cfdata->app != NULL)
     ci->app = evas_stringshare_add(cfdata->app);
   
   e_config_save_queue();
   return 1;
}
