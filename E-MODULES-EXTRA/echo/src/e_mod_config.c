#include <e.h>
#include "e_mod_main.h"
#include "e_mod_system.h"

struct _E_Config_Dialog_Data 
{
   int card, channel, lock;
};

/* local function protos */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _fill_data(E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

EAPI E_Config_Dialog *
e_int_config_echo_module(E_Container *con, const char *params __UNUSED__) 
{
   E_Config_Dialog *cfd = NULL;
   E_Config_Dialog_View *v = NULL;
   char buf[4096];

   if (e_config_dialog_find("Echo", "_e_modules_echo_config_dialog")) return NULL;

   v = E_NEW(E_Config_Dialog_View, 1);
   if (!v) return NULL;

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.create_widgets = _basic_create;
   v->basic.apply_cfdata = _basic_apply;

   snprintf(buf, sizeof(buf), "%s/e-module-echo.edj", echo_mod->dir);
   cfd = e_config_dialog_new(con, D_("Echo Configuration"), "Echo", 
                             "_e_modules_echo_config_dialog", buf, 0, v, NULL);
   echo_cfg->cfd = cfd;
   return cfd;
}

/* local functions */
static void *
_create_data(E_Config_Dialog *cfd) 
{
   E_Config_Dialog_Data *cfdata = NULL;

   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(cfdata);
   return cfdata;
}

static void 
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   echo_cfg->cfd = NULL;
   E_FREE(cfdata);
}

static void 
_fill_data(E_Config_Dialog_Data *cfdata) 
{
   cfdata->lock = echo_cfg->lock_sliders;
   cfdata->card = echo_cfg->card.id;
   cfdata->channel = echo_cfg->channel.id;
}

static Evas_Object *
_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata) 
{
   Evas_Object *o = NULL, *of = NULL;
   Evas_Object *ow = NULL;
   Evas_List *l = NULL, *list;
   E_Radio_Group *rg;
   int i = 0;

   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, D_("General Settings"), 0);
   ow = e_widget_check_add(evas, D_("Lock Sliders"), &(cfdata->lock));
   e_widget_framelist_object_append(of, ow);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, D_("Sound Cards"), 0);
   rg = e_widget_radio_group_new(&(cfdata->card));
   list = echo_sys->get_cards();
   for (i = 0, l = list; l; l = l->next, i++) 
     {
        char *name = NULL;

	name = l->data;
        if (!name) continue;
        ow = e_widget_radio_add(evas, name, i, rg);
        e_widget_framelist_object_append(of, ow);
	evas_stringshare_del(name);
     }
   if (list) evas_list_free(list);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_frametable_add(evas, D_("Channels"), 0);
   rg = e_widget_radio_group_new(&(cfdata->channel));
   for (i = 0, l = echo_sys->get_channels(); l; l = l->next, i++) 
     {
        char *name = NULL;

        if (!(name = l->data)) continue;
        ow = e_widget_radio_add(evas, name, i, rg);
        if (!(i % 2))
          e_widget_frametable_object_append(of, ow, 0, i, 1, 1, 
                                            1, 0, 1, 0);
        else
          e_widget_frametable_object_append(of, ow, 1, (i - 1), 1, 1, 
                                            1, 0, 1, 0);
     }
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   if (l) evas_list_free(l);
   return o;
}

static int 
_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   Evas_List *l = NULL;
   char *name = NULL;
   int update = 0;

   echo_cfg->lock_sliders = cfdata->lock;
   echo_cfg->card.id = cfdata->card;
   if (echo_cfg->channel.id != cfdata->channel) 
     {
        update = 1;
        echo_cfg->channel.id = cfdata->channel;
        l = echo_sys->get_channels();
        name = evas_list_nth(l, cfdata->channel);
        if (echo_cfg->channel.name) evas_stringshare_del(echo_cfg->channel.name);
        if (name) echo_cfg->channel.name = evas_stringshare_add(name);
     }

   e_modapi_save(echo_mod);

   if (update) _echo_cb_config_updated();

   if (l) evas_list_free(l);
   return 1;
}
