#include "e.h"
#include "e_mod_main.h"

struct _E_Config_Dialog_Data
{
  int   show_label;
  int	desk_policy;
  int   icon_label;
  int   skip_dialogs;
  int   skip_always_below_windows;
  int   iconify_focused;
  int   ibox_style;
  int   always_group;
  struct
  {
    Evas_Object *o_desk_show_all;
    Evas_Object *o_desk_show_active;
  }
    gui;
};

/* Protos */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

void
_config_itask_module(Config_Item *ci)
{
   E_Config_Dialog_View *v;
   char buf[4096];

   v = E_NEW(E_Config_Dialog_View, 1);

   /* Dialog Methods */
   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;
   v->advanced.apply_cfdata = NULL;
   v->advanced.create_widgets = NULL;

   /* Create The Dialog */
   snprintf(buf, sizeof(buf), "%s/module.eap", e_module_dir_get(itask_config->module));
   ci->cfd = e_config_dialog_new(e_container_current_get(e_manager_current_get()),
				 D_("ITask Configuration"),
				 "E", "_e_mod_itask_config_dialog",
				 buf, 0, v, ci);
}

static void
_fill_data(Config_Item *ci, E_Config_Dialog_Data *cfdata)
{
   printf("_fill_data");
   cfdata->skip_dialogs = ci->skip_dialogs;
   cfdata->skip_always_below_windows = ci->skip_always_below_windows;
   cfdata->show_label = ci->show_label;
   cfdata->desk_policy = ci->show_desk;
   cfdata->icon_label = ci->icon_label;
   cfdata->iconify_focused = ci->iconify_focused;
   cfdata->ibox_style = ci->ibox_style;
   cfdata->always_group = ci->always_group;
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
   Config_Item *ci = cfd->data;
   ci->cfd = NULL;

   free(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   E_Radio_Group *rg;
   Evas_Object *o, *of, *ob;

   o = e_widget_list_add(evas, 0, 0);

   of = e_widget_framelist_add(evas, D_("General Settings"), 0);
   ob = e_widget_check_add(evas, D_("Good ol' IBox style (instead of taskbar style)"), &(cfdata->ibox_style));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, D_("Iconify Window on Click if Window has Focus"), &(cfdata->iconify_focused));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, D_("Do not show dialog windows in Itask"), &(cfdata->skip_dialogs));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, D_("Do not show windows that are always below in Itask"), &(cfdata->skip_always_below_windows));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, D_("Always group Windows from the same Class"), &(cfdata->always_group));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, D_("Show Icon Label"), &(cfdata->show_label));
   e_widget_framelist_object_append(of, ob);

   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, D_("Zone"), 0);

   rg = e_widget_radio_group_new((int *)&(cfdata->desk_policy));

   ob = e_widget_radio_add(evas, D_("Show windows from current zone"), 0, rg);
   e_widget_framelist_object_append(of, ob);

   ob = e_widget_radio_add(evas, D_("Show windows from current desktop only"), 1, rg);
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
   ci->icon_label = cfdata->icon_label;
   ci->show_desk = cfdata->desk_policy;
   ci->skip_dialogs = cfdata->skip_dialogs;
   ci->skip_always_below_windows = cfdata->skip_always_below_windows;
   ci->iconify_focused = cfdata->iconify_focused;
   ci->ibox_style = cfdata->ibox_style;
   ci->always_group = cfdata->always_group;

   itask_reload(ci->it);

   e_config_save_queue();
   return 1;
}
