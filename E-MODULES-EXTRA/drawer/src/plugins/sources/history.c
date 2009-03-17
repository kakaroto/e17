/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2,t0,(0
 */
#include "history.h"

/* Local Structures */
typedef enum _History_Sort_Type
{
   HISTORY_SORT_EXE,
   HISTORY_SORT_DATE,
   HISTORY_SORT_POPULARITY
} History_Sort_Type;

typedef struct _Instance Instance;
typedef struct _Conf Conf;

struct _Instance 
{
   Drawer_Source *source;

   Eina_List *items, *handlers;

   Conf *conf;

   E_Menu *menu;

   struct {
	E_Config_DD *conf;
   } edd;

   const char *description;
};

struct _Conf
{
   const char *id;
   History_Sort_Type sort_type;
   Eina_Bool	show_info;
};

struct _E_Config_Dialog_Data 
{
   Instance *inst;

   Evas_Object *ilist;
   E_Confirm_Dialog *dialog_delete;

   int sort_type;
   int show_info;
};

static void _history_description_create(Instance *inst);

static Drawer_Source_Item *_history_source_item_fill(Instance *inst, Efreet_Desktop *desktop, const char *file);
static void _history_source_items_free(Instance *inst);
static void _history_event_update_free(void *data __UNUSED__, void *event);
static int  _history_efreet_desktop_list_change_cb(void *data, int ev_type __UNUSED__, void *event __UNUSED__);
static void _history_cb_menu_post(void *data, E_Menu *menu);
static void _history_cb_menu_item_properties(void *data, E_Menu *m, E_Menu_Item *mi);
static void _history_cb_menu_item_remove(void *data, E_Menu *m, E_Menu_Item *mi);
static void _history_conf_activation_cb(void *data1, void *data2 __UNUSED__);

static void *_history_cf_create_data(E_Config_Dialog *cfd);
static void _history_cf_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _history_cf_fill_data(E_Config_Dialog_Data *cfdata);
static Evas_Object *_history_cf_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _history_cf_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

EAPI Drawer_Plugin_Api drawer_plugin_api = {DRAWER_PLUGIN_API_VERSION, "History"};

static E_Config_Dialog *_cfd = NULL;

EAPI void *
drawer_plugin_init(Drawer_Plugin *p, const char *id)
{
   Instance *inst = NULL;
   char buf[128];

   inst = E_NEW(Instance, 1);

   inst->source = DRAWER_SOURCE(p);

   /* Define EET Data Storage */
   inst->edd.conf = E_CONFIG_DD_NEW("Conf", Conf);
   #undef T
   #undef D
   #define T Conf
   #define D inst->edd.conf
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, sort_type, INT);
   E_CONFIG_VAL(D, T, show_info, INT);

   snprintf(buf, sizeof(buf), "module.drawer/%s.history", id);
   inst->conf = e_config_domain_load(buf, inst->edd.conf);
   if (!inst->conf)
     {
	inst->conf = E_NEW(Conf, 1);
	inst->conf->id = eina_stringshare_add(id);
	inst->conf->sort_type = HISTORY_SORT_POPULARITY;

	e_config_save_queue();
     }

   inst->handlers = eina_list_append(inst->handlers,
				     ecore_event_handler_add(EFREET_EVENT_DESKTOP_LIST_CHANGE,
							     _history_efreet_desktop_list_change_cb, inst));
   inst->handlers = eina_list_append(inst->handlers,
				     ecore_event_handler_add(E_EVENT_EXEHIST_UPDATE,
							     _history_efreet_desktop_list_change_cb, inst));
   _history_description_create(inst);

   return inst;
}

EAPI int
drawer_plugin_shutdown(Drawer_Plugin *p)
{
   Instance *inst = NULL;
   
   inst = p->data;

   _history_source_items_free(inst);

   eina_stringshare_del(inst->description);
   eina_stringshare_del(inst->conf->id);

   E_CONFIG_DD_FREE(inst->edd.conf);
   E_FREE_LIST(inst->handlers, ecore_event_handler_del);
   E_FREE(inst->conf);
   E_FREE(inst);

   return 1;
}

EAPI Evas_Object *
drawer_plugin_config_get(Drawer_Plugin *p, Evas *evas)
{
   Evas_Object *button;

   button = e_widget_button_add(evas, D_("History settings"), NULL, _history_conf_activation_cb, p, NULL);

   return button;
}

EAPI void
drawer_plugin_config_save(Drawer_Plugin *p)
{
   Instance *inst;
   char buf[128];

   inst = p->data;
   snprintf(buf, sizeof(buf), "module.drawer/%s.history", inst->conf->id);
   e_config_domain_save(buf, inst->edd.conf, inst->conf);
}

EAPI Eina_List *
drawer_source_list(Drawer_Source *s, Evas *evas __UNUSED__)
{
   Instance *inst = NULL;
   Eina_List *hist = NULL, *l;
   const char *file;
   char buf[4096];

   if (!(inst = DRAWER_PLUGIN(s)->data)) return NULL;

   _history_source_items_free(inst);

   switch(inst->conf->sort_type)
     {
      case HISTORY_SORT_EXE:
	 hist = e_exehist_sorted_list_get(E_EXEHIST_SORT_BY_EXE, 0);
	 break;
      case HISTORY_SORT_DATE:
	 hist = e_exehist_sorted_list_get(E_EXEHIST_SORT_BY_DATE, 0);
	 break;
      case HISTORY_SORT_POPULARITY:
	 hist = e_exehist_sorted_list_get(E_EXEHIST_SORT_BY_POPULARITY, 0);
	 break;
     }

   if (!hist) return NULL;
   EINA_LIST_FOREACH(hist, l, file)
     {
	Drawer_Source_Item *si = NULL;
	Efreet_Desktop *desktop = efreet_util_desktop_exec_find(file);

	/* Instead of desktops, work with executables directly */

	si = _history_source_item_fill(inst, desktop, file);
	inst->items = eina_list_append(inst->items, si);
     }

   return inst->items;
}

EAPI void
drawer_source_activate(Drawer_Source *s, Drawer_Source_Item *si, E_Zone *zone)
{
   Instance *inst = NULL;

   if (si->desktop)
     {
	if (si->desktop->type == EFREET_DESKTOP_TYPE_APPLICATION)
	  e_exec(zone, si->desktop, NULL, NULL, "drawer");
	else if (si->desktop->type == EFREET_DESKTOP_TYPE_LINK)
	  {
	     if (!strncasecmp(si->desktop->url, "file:", 5))
	       {
		  E_Action *act;

		  act = e_action_find("fileman");
		  if (act) act->func.go(NULL, si->desktop->url + 5);
	       }
	  }
     }
   else if (si->file_path)
     e_exec(zone, NULL, si->file_path, NULL, "drawer");

   inst = DRAWER_PLUGIN(s)->data;
}

EAPI void
drawer_source_context(Drawer_Source *s, Drawer_Source_Item *si, E_Zone *zone, Drawer_Event_View_Context *ev)
{
   Instance *inst = NULL;
   E_Menu_Item *mi = NULL;

   inst = DRAWER_PLUGIN(s)->data;

   inst->menu = e_menu_new();
   e_menu_post_deactivate_callback_set(inst->menu, _history_cb_menu_post, inst);

   mi = e_menu_item_new(inst->menu);
   e_menu_item_label_set(mi, D_("Change Item Properties"));
   e_util_menu_item_theme_icon_set(mi, "widget/config");
   e_menu_item_callback_set(mi, _history_cb_menu_item_properties, si);

   mi = e_menu_item_new(inst->menu);
   e_menu_item_label_set(mi, D_("Remove Item"));
   e_util_menu_item_theme_icon_set(mi, "widget/del");
   e_menu_item_callback_set(mi, _history_cb_menu_item_remove, si);

   e_menu_activate(inst->menu, zone, ev->x, ev->y, 1, 1, E_MENU_POP_DIRECTION_AUTO);
}

EAPI const char *
drawer_source_description_get(Drawer_Source *s)
{
   Instance *inst;

   inst = DRAWER_PLUGIN(s)->data;

   return inst->description;
}

static void
_history_description_create(Instance *inst)
{
   char buf[1024];

   eina_stringshare_del(inst->description);
   switch(inst->conf->sort_type)
     {
      case HISTORY_SORT_EXE:
	 inst->description = eina_stringshare_add("Programs in history");
	 break;
      case HISTORY_SORT_DATE:
	 inst->description = eina_stringshare_add("Recently used programs");
	 break;
      case HISTORY_SORT_POPULARITY:
	 inst->description = eina_stringshare_add("Most used programs");
	 break;
     }
}

static Drawer_Source_Item *
_history_source_item_fill(Instance *inst, Efreet_Desktop *desktop, const char *file)
{
   Drawer_Source_Item *si = NULL;
   Eina_List *l;
   int found = 0;
   char buf[5];

   si = E_NEW(Drawer_Source_Item, 1);

   if (desktop)
     {
	si->desktop = desktop;
	si->label = eina_stringshare_add(desktop->name);
	si->description = eina_stringshare_add(desktop->comment);
	if (inst->conf->show_info && inst->conf->sort_type == HISTORY_SORT_POPULARITY)
	  {
	     snprintf(buf, sizeof(buf), "%d", e_exehist_popularity_get(desktop->exec));
	     si->info = eina_stringshare_add(buf);
	  }
     }
   else
     {
	si->file_path = eina_stringshare_add(file);
	si->label = eina_stringshare_add(file);
     }

   si->priv = eina_stringshare_add(file);

   return si;
}

static void
_history_source_items_free(Instance *inst)
{
   while (inst->items)
     {
	Drawer_Source_Item *si = NULL;
	
	si = inst->items->data;
	inst->items = eina_list_remove_list(inst->items, inst->items);
	efreet_desktop_free(si->desktop);
	eina_stringshare_del(si->file_path);
	eina_stringshare_del(si->label);
	eina_stringshare_del(si->description);
	eina_stringshare_del(si->category);
	eina_stringshare_del(si->priv);
	eina_stringshare_del(si->info);

	free(si);
     }
}

static void
_history_event_update_free(void *data __UNUSED__, void *event)
{
   Drawer_Event_Source_Update *ev;

   ev = event;
   eina_stringshare_del(ev->id);
   free(ev);
}

static int
_history_efreet_desktop_list_change_cb(void *data, int ev_type __UNUSED__, void *event __UNUSED__)
{
   Instance *inst = data;
   Drawer_Event_Source_Update *ev;

   inst = data;
   ev = E_NEW(Drawer_Event_Source_Update, 1);
   ev->source = inst->source;
   ev->id = eina_stringshare_add(inst->conf->id);
   ecore_event_add(DRAWER_EVENT_SOURCE_UPDATE, ev, _history_event_update_free, NULL);
   
   return 1;
}

static void 
_history_cb_menu_post(void *data, E_Menu *menu)
{
   Instance *inst = NULL;

   if (!(inst = data)) return;
   if (!inst->menu) return;
   e_object_del(E_OBJECT(inst->menu));
   inst->menu = NULL;
}

static void
_history_cb_menu_item_properties(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Drawer_Source_Item *si = data;

   e_desktop_edit(e_container_current_get(e_manager_current_get()), si->desktop);
}

static void
_history_cb_menu_item_remove(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Drawer_Source_Item *si = data;
   
   e_exehist_del((const char *) si->priv);
}

static void
_history_conf_activation_cb(void *data1, void *data2 __UNUSED__)
{
   Drawer_Plugin *p = NULL;
   Instance *inst = NULL;
   E_Config_Dialog_View *v = NULL;
   char buf[4096];

   p = data1;
   inst = p->data;
   /* is this config dialog already visible ? */
   if (e_config_dialog_find("Drawer_History", "_e_module_drawer_cfg_dlg"))
     return;

   v = E_NEW(E_Config_Dialog_View, 1);
   if (!v) return;

   v->create_cfdata = _history_cf_create_data;
   v->free_cfdata = _history_cf_free_data;
   v->basic.create_widgets = _history_cf_basic_create;
   v->basic.apply_cfdata = _history_cf_basic_apply;

   /* Icon in the theme */
   snprintf(buf, sizeof(buf), "%s/e-module-drawer.edj", drawer_conf->module->dir);

   /* create new config dialog */
   _cfd = e_config_dialog_new(e_container_current_get(e_manager_current_get()),
	 D_("Drawer Plugin : History"), "Drawer_History", 
	 "_e_module_drawer_cfg_dlg", buf, 0, v, inst);

   e_dialog_resizable_set(_cfd->dia, 1);
}

/* Local Functions */
static void *
_history_cf_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata = NULL;

   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   cfdata->inst = cfd->data;
   _history_cf_fill_data(cfdata);
   return cfdata;
}

static void 
_history_cf_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   _cfd = NULL;
   E_FREE(cfdata);
}

static void 
_history_cf_fill_data(E_Config_Dialog_Data *cfdata)
{
   cfdata->sort_type = cfdata->inst->conf->sort_type;
   cfdata->show_info = cfdata->inst->conf->show_info;
}

static Evas_Object *
_history_cf_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob;
   E_Radio_Group *rg;

   o = e_widget_list_add(evas, 0, 0);

   rg = e_widget_radio_group_new(&(cfdata->sort_type));
   of = e_widget_framelist_add(evas, D_("Sorting options"), 0);
   ob = e_widget_radio_add(evas, D_("Sort applications by executable"), HISTORY_SORT_EXE, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, D_("Sort applications by date"), HISTORY_SORT_DATE, rg);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, D_("Sort applications by popularity"), HISTORY_SORT_POPULARITY, rg);
   e_widget_framelist_object_append(of, ob);

   ob = e_widget_check_add(evas, D_("Show popularity"), &(cfdata->show_info));
   e_widget_framelist_object_append(of, ob);

   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}

static int 
_history_cf_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Instance *inst = NULL;
   Drawer_Event_Source_Update *ev;

   inst = cfdata->inst;
   cfdata->inst->conf->sort_type = cfdata->sort_type;
   cfdata->inst->conf->show_info = cfdata->show_info;

   _history_description_create(inst);

   ev = E_NEW(Drawer_Event_Source_Update, 1);
   ev->source = inst->source;
   ev->id = eina_stringshare_add(inst->conf->id);
   ecore_event_add(DRAWER_EVENT_SOURCE_UPDATE, ev, _history_event_update_free, NULL);

   e_config_save_queue();
   return 1;
}
