#include <e.h>
#include "e_mod_main.h"

struct _E_Config_Dialog_Data
{
   int show_label;
   int show_all;
};

/* Protos */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

void
_config_taskbar_module(Config_Item *ci)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   E_Container *con;

   v = E_NEW(E_Config_Dialog_View, 1);

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;

   con = e_container_current_get(e_manager_current_get());
   cfd = e_config_dialog_new(con, D_("Taskbar Configuration"), NULL, 0, v, ci);
   taskbar_config->config_dialog = evas_list_append(taskbar_config->config_dialog, cfd);
}

static void
_fill_data(Config_Item *ci, E_Config_Dialog_Data *cfdata)
{
   cfdata->show_label = ci->show_label;
   cfdata->show_all = ci->show_all;
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;
   Config_Item *ci;

   ci = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);

   _fill_data(ci, cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   if (!taskbar_config)
      return;
   taskbar_config->config_dialog = evas_list_remove(taskbar_config->config_dialog, cfd);
   free(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob;

   o = e_widget_list_add(evas, 0, 0);

   of = e_widget_framelist_add(evas, D_("Display"), 0);

   ob = e_widget_check_add(evas, D_("Show Labels"), &(cfdata->show_label));
   //if (cfdata->show_label)
   //  e_widget_check_checked_set(ob, 1);
   e_widget_framelist_object_append(of, ob);  

   ob = e_widget_check_add(evas, D_("Show windows from all desktops"), &(cfdata->show_all));
   //if (cfdata->show_all)
   //   e_widget_check_checked_set(ob, 1);
   e_widget_framelist_object_append(of, ob);

   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Config_Item *ci;

   ci = cfd->data;
   ci->show_label = cfdata->show_label;
   ci->show_all = cfdata->show_all;

   e_config_save_queue();

   _taskbar_config_updated(ci->id);
   return 1;
}

