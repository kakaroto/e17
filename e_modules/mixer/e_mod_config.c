#include <e.h>
#include "e_mod_main.h"

#ifdef HAVE_LIBASOUND
# include "alsa_mixer.h"
#endif

struct _E_Config_Dialog_Data
{
   Evas_Object *list;
   
   int card_id;
   int channel_id;
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
   cfdata->channel_id = ci->channel_id;
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
   Evas_Object   *o, *ob, *of;
   Evas_List     *cards, *c, *e;
   Config_Item   *ci;
   E_Radio_Group *rg;

   ci = cfd->data;

   o = e_widget_list_add(evas, 0, 1);	     
   of = e_widget_framelist_add(evas, _("Available Mixers"), 0);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   return 1;
}
