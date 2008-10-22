/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include <e.h>
#include "e_mod_main.h"

struct _E_Config_Dialog_Data
{
   char *dir;
   int   hide_window;
   int   show_label;
   int	 zone_policy;
   int	 desk_policy;
   int   icon_label;
   struct
     {
	Evas_Object *ilist;
	Evas_Object *o_desk_show_all;
	Evas_Object *o_desk_show_active;
	E_Confirm_Dialog *dialog_delete;
     } gui;
};

/* Protos */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _cb_add(void *data, void *data2);
static void _cb_del(void *data, void *data2);
static void _cb_config(void *data, void *data2);
static void _cb_entry_ok(char *text, void *data);
static void _cb_confirm_dialog_yes(void *data);
static void _cb_confirm_dialog_destroy(void *data);
static void _load_ilist(E_Config_Dialog_Data *cfdata);
static void _cb_zone_policy_change(void *data, Evas_Object *obj);

void 
_config_iiirk_module(Config_Item *ci)
{
   E_Config_Dialog *cfd;
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
   snprintf(buf, sizeof(buf), "%s/e-module-iiirk.edj", e_module_dir_get(iiirk_config->module));
   cfd = e_config_dialog_new(e_container_current_get(e_manager_current_get()),
			     D_("Iiirk Configuration"),
			     "E", "_e_mod_iiirk_config_dialog",
			     buf, 0, v, ci);
   iiirk_config->config_dialog = eina_list_append(iiirk_config->config_dialog, cfd);
}

static void 
_fill_data(Config_Item *ci, E_Config_Dialog_Data *cfdata)
{
   if (ci->dir)
     cfdata->dir = strdup(ci->dir);
   else
     cfdata->dir = strdup("");
   cfdata->hide_window = ci->hide_window;
   cfdata->show_label = ci->show_label;
   cfdata->zone_policy = ci->show_zone;
   cfdata->desk_policy = ci->show_desk;
   cfdata->icon_label = ci->icon_label;
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
   if (cfdata->dir) free(cfdata->dir);
   if (cfdata->gui.dialog_delete) e_object_del(E_OBJECT(cfdata->gui.dialog_delete));
   iiirk_config->config_dialog = eina_list_remove(iiirk_config->config_dialog, cfd);
   E_FREE(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   E_Radio_Group *rg;
   Evas_Object *o, *of, *ol, *ob, *ot;

   Eina_List *l, *l2;
   int zone_count;

   o = e_widget_list_add(evas, 0, 0);

   of = e_widget_frametable_add(evas, D_("Selected Iiirk Source"), 0);
   ol = e_widget_ilist_add(evas, 16, 16, &(cfdata->dir));
   cfdata->gui.ilist = ol;
   _load_ilist(cfdata);
   e_widget_min_size_set(ol, 140, 30);
   e_widget_frametable_object_append(of, ol, 0, 0, 1, 2, 1, 1, 1, 0);
   
   ot = e_widget_table_add(evas, 0);
   ob = e_widget_button_add(evas, D_("Add"), "widget/add", _cb_add, cfdata, NULL);
   e_widget_table_object_append(ot, ob, 0, 0, 1, 1, 1, 1, 1, 0);
   ob = e_widget_button_add(evas, D_("Delete"), "widget/del", _cb_del, cfdata, NULL);
   e_widget_table_object_append(ot, ob, 0, 1, 1, 1, 1, 1, 1, 0);
   ob = e_widget_button_add(evas, D_("Configure"), "widget/config", _cb_config, cfdata, NULL);
   e_widget_table_object_append(ot, ob, 0, 2, 1, 1, 1, 1, 1, 0);   

   if (!e_configure_registry_exists("applications/iiirk_applications")) 
     e_widget_disabled_set(ob, 1);
   
   e_widget_frametable_object_append(of, ot, 1, 0, 1, 1, 1, 1, 1, 0);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, D_("General Settings"), 0);
   ob = e_widget_check_add(evas, D_("Hide window from any list when iconified"), &(cfdata->hide_window));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, D_("Show Icon Label"), &(cfdata->show_label));
   e_widget_framelist_object_append(of, ob);
   rg = e_widget_radio_group_new(&(cfdata->icon_label));
   ob = e_widget_radio_add(evas, D_("Display Name"), 0, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, D_("Display Title"), 1, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, D_("Display Class"), 2, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, D_("Display Icon Name"), 3, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, D_("Display Border Caption"), 4, rg);
   e_widget_framelist_object_append(of, ob);
   
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, D_("Screen"), 0);

   zone_count = 0;
   for (l = e_manager_list(); l; l = l->next)
     {
	E_Manager *man;
	man = l->data;

	for (l2 = man->containers; l2; l2 = l2->next)
	  {
	     E_Container *con;

	     con = l2->data;
	     zone_count += eina_list_count(con->zones);
	  }
     }

   if (zone_count <= 1) cfdata->zone_policy = 1;

   rg = e_widget_radio_group_new((int *)&(cfdata->zone_policy));
   ob = e_widget_radio_add(evas, D_("Show windows from all screens"), 0, rg);
   e_widget_on_change_hook_set(ob, _cb_zone_policy_change, cfdata);
   e_widget_framelist_object_append(of, ob);
   if (zone_count <= 1) e_widget_disabled_set(ob, 1);

   ob = e_widget_radio_add(evas, D_("Show windows from current screen"), 1, rg);
   e_widget_on_change_hook_set(ob, _cb_zone_policy_change, cfdata);
   e_widget_framelist_object_append(of, ob);

   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, D_("Desktop"), 0);

   rg = e_widget_radio_group_new((int *)&(cfdata->desk_policy));
   ob = e_widget_radio_add(evas, D_("Show windows from all desktops"), 0, rg);
   e_widget_framelist_object_append(of, ob);
   if (cfdata->zone_policy == 0) e_widget_disabled_set(ob, 1);
   cfdata->gui.o_desk_show_all = ob;

   ob = e_widget_radio_add(evas, D_("Show windows from active desktop"), 1, rg);
   e_widget_framelist_object_append(of, ob);
   if (cfdata->zone_policy == 0) e_widget_disabled_set(ob, 1);
   cfdata->gui.o_desk_show_active = ob;

   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Config_Item *ci;
   
   ci = cfd->data;
   if (ci->dir) evas_stringshare_del(ci->dir);
   ci->dir = NULL;
   if (cfdata->dir) ci->dir = evas_stringshare_add(cfdata->dir);
   ci->hide_window = cfdata->hide_window;
   ci->show_label = cfdata->show_label;
   ci->icon_label = cfdata->icon_label;
   ci->show_zone = cfdata->zone_policy;
   ci->show_desk = cfdata->desk_policy;

   _iiirk_config_update(ci);
   e_config_save_queue();
   return 1;
}


/****** callbacks **********/

static void 
_cb_add(void *data, void *data2) 
{
   E_Config_Dialog_Data *cfdata;
   
   cfdata = data;
   e_entry_dialog_show(D_("Create new IIirk source"), "enlightenment/e",
		       D_("Enter a name for this new source:"), "", NULL, NULL,
		       _cb_entry_ok, NULL, cfdata);
}

static void 
_cb_del(void *data, void *data2) 
{
   char buf[4096];
   E_Config_Dialog_Data *cfdata;
   E_Confirm_Dialog *dialog;

   cfdata = data;   
   if (cfdata->gui.dialog_delete)
     return;

   snprintf(buf, sizeof(buf), D_("You requested to delete \"%s\".<br><br>"
				"Are you sure you want to delete this iiirk source?"),
	    cfdata->dir);
   
   dialog = e_confirm_dialog_show(D_("Are you sure you want to delete this iiirk source?"),
                                  "enlightenment/exit", buf, NULL, NULL, 
                                  _cb_confirm_dialog_yes, NULL, cfdata, NULL, _cb_confirm_dialog_destroy, cfdata);
   cfdata->gui.dialog_delete = dialog;
}

static void 
_cb_config(void *data, void *data2) 
{
   char path[PATH_MAX];
   E_Config_Dialog_Data *cfdata;
   
   cfdata = data;
   snprintf(path, sizeof(path), "%s/.e/e/applications/iiirk/%s/.order", 
	    e_user_homedir_get(), cfdata->dir);
   e_configure_registry_call("internal/iiirk_other",
			     e_container_current_get(e_manager_current_get()),
			     path);
}

static void
_cb_entry_ok(char *text, void *data) 
{
   iiirk_create_default_profile(text);
   _load_ilist(data);
}

static void
_cb_confirm_dialog_yes(void *data) 
{
   E_Config_Dialog_Data *cfdata;
   char buf[4096];
   
   cfdata = data;
   snprintf(buf, sizeof(buf), "%s/.e/e/applications/iiirk/%s", e_user_homedir_get(), cfdata->dir);
   if (ecore_file_is_dir(buf))
     ecore_file_recursive_rm(buf);
   
   _load_ilist(cfdata);
}

static void
_cb_confirm_dialog_destroy(void *data) 
{
   E_Config_Dialog_Data *cfdata;
   
   cfdata = data;
   cfdata->gui.dialog_delete = NULL;
}

static void 
_load_ilist(E_Config_Dialog_Data *cfdata) 
{
   Ecore_List *dirs;
   const char *home;
   char buf[4096], *file;
   int selnum = -1;

   e_widget_ilist_clear(cfdata->gui.ilist);
   
   home = e_user_homedir_get();
   snprintf(buf, sizeof(buf), "%s/.e/e/applications/iiirk", home);
   dirs = ecore_file_ls(buf);
   
   if (dirs)
     {
	int i;
	
	i = 0;
	while ((file = ecore_list_next(dirs)))
	  {
	     if (file[0] == '.') continue;
	     snprintf(buf, sizeof(buf), "%s/.e/e/applications/iiirk/%s", home, file);
	     if (ecore_file_is_dir(buf))
	       {
		  e_widget_ilist_append(cfdata->gui.ilist, NULL, file, NULL, NULL, file);
		  if ((cfdata->dir) && (!strcmp(cfdata->dir, file)))
		    selnum = i;
		  i++;
	       }
	  }
	ecore_list_destroy(dirs);
     }
   e_widget_ilist_go(cfdata->gui.ilist);
   if (selnum >= 0)
     e_widget_ilist_selected_set(cfdata->gui.ilist, selnum);   
}

static void
_cb_zone_policy_change(void *data, Evas_Object *obj)
{
   E_Config_Dialog_Data *cfdata;

   cfdata = data;

   if (cfdata->zone_policy == 0)
     {
	e_widget_disabled_set(cfdata->gui.o_desk_show_all, 1);
	e_widget_disabled_set(cfdata->gui.o_desk_show_active, 1);
     }
   else
     {
	e_widget_disabled_set(cfdata->gui.o_desk_show_all, 0);
	e_widget_disabled_set(cfdata->gui.o_desk_show_active, 0);
     }
}
