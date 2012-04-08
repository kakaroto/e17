#include <e.h>
#include "e_mod_main.h"

#define MAX_CMD_LINE_LENGTH 4096

struct _E_Config_Dialog_Data
{
   double refresh_interval;
   char *cmd;
};

static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _fill_data(Config_Item *ci, E_Config_Dialog_Data *cfdata);

void
_config_eektool_module(Config_Item *ci)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   E_Container *con;

   v = E_NEW(E_Config_Dialog_View, 1);

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply;
   v->basic.create_widgets = _basic_create;
   v->basic_only = 1;
   v->override_auto_apply = 1;

   con = e_container_current_get(e_manager_current_get());
   cfd = e_config_dialog_new(con, D_("Eektool Settings"), "Eektool",
                             "_e_modules_eektool_config_dialog", NULL, 0, v, ci);
   eektool_config->config_dialog = cfd;
}

static void
_fill_data(Config_Item *ci, E_Config_Dialog_Data *cfdata)
{
   cfdata->refresh_interval = ci->refresh_interval;
   snprintf(cfdata->cmd, (MAX_CMD_LINE_LENGTH - 1), "%s", ci->cmd);
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;
   Config_Item *ci;

   ci = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   cfdata->cmd = calloc(MAX_CMD_LINE_LENGTH, sizeof(char));
   _fill_data(ci, cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd __UNUSED__, E_Config_Dialog_Data *cfdata)
{
   if (!eektool_config) return;
   eektool_config->config_dialog = NULL;
   free(cfdata->cmd);
   cfdata->cmd = NULL;
   free(cfdata);
   cfdata = NULL;
}

static Evas_Object *
_basic_create(E_Config_Dialog *cfd __UNUSED__, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *ob;

   o = e_widget_list_add(evas, 0, 0);
   ob = e_widget_label_add(evas, D_("Command"));
   e_widget_list_object_append(o, ob, 1, 1, 0.5);

   ob = e_widget_entry_add(evas, &cfdata->cmd, NULL, NULL, NULL);
   //e_widget_size_min_set(ob, 100, 1);
   e_widget_list_object_append(o, ob, 1, 1, 0.5);

   ob = e_widget_slider_add(evas, 1, 0, D_("%4.0f seconds"), 1.0, 3600.0, 1.0, 0, &(cfdata->refresh_interval), NULL, 40);
   e_widget_list_object_append(o, ob, 1, 1, 0.5);

   return o;
}

static int
_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Config_Item *ci;

   ci = cfd->data;
   ci->refresh_interval = cfdata->refresh_interval;
   eina_stringshare_replace(&ci->cmd, cfdata->cmd);

   e_config_save_queue();
   _eektool_config_updated(ci);
   return 1;
}
