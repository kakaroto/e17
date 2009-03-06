/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2,t0,(0
 */
#include "launcher.h"

/* Local Structures */
typedef struct _Instance Instance;
typedef struct _Conf Conf;
typedef struct _Conf_Rating Conf_Rating;
typedef struct _Launcher_Menu_Data Launcher_Menu_Data;

struct _Instance 
{
   Drawer_Source *source;

   E_Order *apps;
   Eina_List *items;

   E_Menu *menu;
   Launcher_Menu_Data *m_data;

   Conf *conf;

   struct {
	E_Config_DD *conf;
	E_Config_DD *conf_rel;
   } edd;

   const char *description;
};

struct _Conf
{
   const char *id;

   const char *dir;

   int sort_rel;

   Eina_List *ratings;
};

struct _Conf_Rating
{
   const char *label;

   int rating;
};

struct _E_Config_Dialog_Data 
{
   Instance *inst;

   Evas_Object *ilist;
   E_Confirm_Dialog *dialog_delete;

   char *dir;
   int sort_rel;
};

struct _Launcher_Menu_Data
{
   Instance *inst;

   Drawer_Source_Item *si;
};

#define CONF_RATING(obj) ((Conf_Rating *) obj)

static void _launcher_description_create(Instance *inst);
static void _launcher_sources_rating_discount(Instance *inst, int min);

static int _launcher_cb_sort_rating(const void *data1, const void *data2);
static void _launcher_cb_app_change(void *data, E_Order *eo __UNUSED__);
static Drawer_Source_Item *_launcher_source_item_fill(Instance *inst, Efreet_Desktop *desktop);
static void _launcher_source_item_free(Instance *inst, Drawer_Source_Item *si);
static void _launcher_source_items_free(Instance *inst);
static void _launcher_event_update_free(void *data __UNUSED__, void *event);
static void _launcher_conf_activation_cb(void *data1, void *data2 __UNUSED__);
static void _launcher_cb_menu_post(void *data, E_Menu *menu);
static void _launcher_cb_menu_item_properties(void *data, E_Menu *m, E_Menu_Item *mi);
static void _launcher_cb_menu_item_remove(void *data, E_Menu *m, E_Menu_Item *mi);

static void *_launcher_cf_create_data(E_Config_Dialog *cfd);
static void _launcher_cf_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _launcher_cf_fill_data(E_Config_Dialog_Data *cfdata);
static Evas_Object *_launcher_cf_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _launcher_cf_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

static void _cb_add(void *data, void *data2);
static void _cb_del(void *data, void *data2);
static void _cb_config(void *data, void *data2);
static void _cb_entry_ok(char *text, void *data);
static void _cb_confirm_dialog_yes(void *data);
static void _cb_confirm_dialog_destroy(void *data);
static void _launcher_cf_load_ilist(E_Config_Dialog_Data *cfdata);

EAPI Drawer_Plugin_Api drawer_plugin_api = {DRAWER_PLUGIN_API_VERSION, "Launcher"};

static E_Config_Dialog *_cfd = NULL;

EAPI void *
drawer_plugin_init(Drawer_Plugin *p, const char *id)
{
   Instance *inst = NULL;
   char buf[128];

   inst = E_NEW(Instance, 1);

   inst->source = DRAWER_SOURCE(p);

   /* Define EET Data Storage */
   inst->edd.conf_rel = E_CONFIG_DD_NEW("Conf_Rating", Conf_Rating);
   #undef T
   #undef D
   #define T Conf_Rating 
   #define D inst->edd.conf_rel
   E_CONFIG_VAL(D, T, label, STR);
   E_CONFIG_VAL(D, T, rating, INT);

   inst->edd.conf = E_CONFIG_DD_NEW("Conf", Conf);
   #undef T
   #undef D
   #define T Conf
   #define D inst->edd.conf
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, dir, STR);
   E_CONFIG_VAL(D, T, sort_rel, INT);
   E_CONFIG_LIST(D, T, ratings, inst->edd.conf_rel); /* the list */

   snprintf(buf, sizeof(buf), "module.drawer/%s.launcher", id);
   inst->conf = e_config_domain_load(buf, inst->edd.conf);
   if (!inst->conf)
     {
	inst->conf = E_NEW(Conf, 1);
	inst->conf->dir = eina_stringshare_add("default");
	inst->conf->id = eina_stringshare_add(id);

	e_config_save_queue();
     }

   _launcher_description_create(inst);

   return inst;
}

EAPI int
drawer_plugin_shutdown(Drawer_Plugin *p)
{
   Instance *inst = NULL;
   
   inst = p->data;

   _launcher_source_items_free(inst);

   eina_stringshare_del(inst->description);
   eina_stringshare_del(inst->conf->id);
   eina_stringshare_del(inst->conf->dir);

   while (inst->conf->ratings)
     {
	Conf_Rating *r = NULL;

	r = inst->conf->ratings->data;

	inst->conf->ratings = eina_list_remove_list(
	      inst->conf->ratings, inst->conf->ratings);

	eina_stringshare_del(r->label);

	E_FREE(r);
     }

   if (inst->menu) 
     {
        e_menu_post_deactivate_callback_set(inst->menu, NULL, NULL);
        e_object_del(E_OBJECT(inst->menu));
        inst->menu = NULL;
     }

   if (inst->m_data)
     E_FREE(inst->m_data);

   E_CONFIG_DD_FREE(inst->edd.conf);
   E_CONFIG_DD_FREE(inst->edd.conf_rel);
   E_FREE(inst->conf);
   E_FREE(inst);

   return 1;
}

EAPI Evas_Object *
drawer_plugin_config_get(Drawer_Plugin *p, Evas *evas)
{
   Evas_Object *button;

   button = e_widget_button_add(evas, D_("Launcher settings"), NULL, _launcher_conf_activation_cb, p, NULL);

   return button;
}

EAPI void
drawer_plugin_config_save(Drawer_Plugin *p)
{
   Instance *inst;
   char buf[128];

   inst = p->data;
   snprintf(buf, sizeof(buf), "module.drawer/%s.launcher", inst->conf->id);
   e_config_domain_save(buf, inst->edd.conf, inst->conf);
}

EAPI Eina_List *
drawer_source_list(Drawer_Source *s, Evas *evas __UNUSED__)
{
   Instance *inst = NULL;
   char buf[4096];
   const char *homedir;
   int min = 0;

   if (!(inst = DRAWER_PLUGIN(s)->data)) return NULL;

   _launcher_source_items_free(inst);

   homedir = e_user_homedir_get();
   snprintf(buf, sizeof(buf), "%s/.e/e/applications/bar/%s/.order", homedir, inst->conf->dir);

   inst->apps = e_order_new(buf);
   e_order_update_callback_set(inst->apps, _launcher_cb_app_change, inst);

   if (inst->apps)
     {
	Efreet_Desktop *desktop = NULL;
	Eina_List *l = NULL;

	for (l = inst->apps->desktops; l; l = l->next)
	  {
	     Drawer_Source_Item *si;

	     desktop = l->data;
	     si = _launcher_source_item_fill(inst, desktop);
	     inst->items = eina_list_append(inst->items, si);

	     min = MIN(CONF_RATING(si->priv)->rating, min);
	  }
     }

   if (min > 20000)
     _launcher_sources_rating_discount(inst, min);
   if (inst->conf->sort_rel)
	inst->items = eina_list_sort(inst->items,
	      eina_list_count(inst->items), _launcher_cb_sort_rating);
   return inst->items;
}

EAPI void
drawer_source_activate(Drawer_Source *s, Drawer_Source_Item *si, E_Zone *zone)
{
   Instance *inst = NULL;

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

   inst = DRAWER_PLUGIN(s)->data;
   CONF_RATING(si->priv)->rating++;
   if (inst->conf->sort_rel)
     _launcher_cb_app_change(inst, NULL);
}

EAPI void
drawer_source_context(Drawer_Source *s, Drawer_Source_Item *si, E_Zone *zone, Drawer_Event_View_Context *ev)
{
   Instance *inst = NULL;
   E_Menu_Item *mi = NULL;
   Launcher_Menu_Data *m_data = E_NEW(Launcher_Menu_Data, 1);

   inst = DRAWER_PLUGIN(s)->data;
   inst->m_data = m_data;
   m_data->inst = inst;
   m_data->si = si;

   inst->menu = e_menu_new();
   e_menu_post_deactivate_callback_set(inst->menu, _launcher_cb_menu_post, inst);

   mi = e_menu_item_new(inst->menu);
   e_menu_item_label_set(mi, D_("Change Item Properties"));
   e_util_menu_item_theme_icon_set(mi, "widget/config");
   e_menu_item_callback_set(mi, _launcher_cb_menu_item_properties, si);

   mi = e_menu_item_new(inst->menu);
   e_menu_item_label_set(mi, D_("Remove Item"));
   e_util_menu_item_theme_icon_set(mi, "widget/del");
   e_menu_item_callback_set(mi, _launcher_cb_menu_item_remove, m_data);

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
_launcher_description_create(Instance *inst)
{
   char buf[1024];

   eina_stringshare_del(inst->description);
   snprintf(buf, sizeof(buf), D_("%s source"), inst->conf->dir);
   inst->description = eina_stringshare_add(buf);
}

static void
_launcher_sources_rating_discount(Instance *inst, int min)
{
   Drawer_Source_Item *si;
   Eina_List *l;

   EINA_LIST_FOREACH(inst->items, l, si)
      CONF_RATING(si->priv)->rating -= min;
}

static int
_launcher_cb_sort_rating(const void *data1, const void *data2)
{
   const Drawer_Source_Item *si1 = NULL, *si2 = NULL;

   si1 = data1;
   si2 = data2;
   if(!si1) return(1);
   if(!si2) return(-1);

   /* reverse the list if equal */
   if (CONF_RATING(si2->priv)->rating == CONF_RATING(si1->priv)->rating)
     return -1;
   return CONF_RATING(si2->priv)->rating - CONF_RATING(si1->priv)->rating;
}

static void
_launcher_cb_app_change(void *data, E_Order *eo __UNUSED__)
{
   Instance *inst = NULL;
   Drawer_Event_Source_Update *ev;

   inst = data;
   if (!inst->apps) return;
   ev = E_NEW(Drawer_Event_Source_Update, 1);
   ev->source = inst->source;
   ev->id = eina_stringshare_add(inst->conf->id);
   ecore_event_add(DRAWER_EVENT_SOURCE_UPDATE, ev, _launcher_event_update_free, NULL);
}

static Drawer_Source_Item *
_launcher_source_item_fill(Instance *inst, Efreet_Desktop *desktop)
{
   Drawer_Source_Item *si = NULL;
   Conf_Rating *r;
   Eina_List *l;
   int found = 0;

   si = E_NEW(Drawer_Source_Item, 1);

   si->desktop = desktop;
   si->label = eina_stringshare_add(desktop->name);
   si->description = eina_stringshare_add(desktop->comment);

   EINA_LIST_FOREACH(inst->conf->ratings, l, r)
     {
	if (!(strcmp(si->label, r->label)))
	  {
	     si->priv = r;
	     found = 1;
	     break;
	  }
     }

   if (!found)
     {
	r = E_NEW(Conf_Rating, 1);
	r->label = eina_stringshare_add(si->label);
	r->rating = 0;
	si->priv = r;

	inst->conf->ratings = eina_list_append(inst->conf->ratings, r);
     }

   return si;
}

static void
_launcher_source_item_free(Instance *inst, Drawer_Source_Item *si)
{
   e_order_remove(inst->apps, si->desktop);

   inst->items = eina_list_remove(inst->items, si);
   eina_stringshare_del(si->label);
   eina_stringshare_del(si->description);
   eina_stringshare_del(si->category);
   free(si);
}

static void
_launcher_source_items_free(Instance *inst)
{
   while (inst->items)
     {
	Drawer_Source_Item *si = NULL;
	
	si = inst->items->data;
	inst->items = eina_list_remove_list(inst->items, inst->items);
	eina_stringshare_del(si->label);
	eina_stringshare_del(si->description);
	eina_stringshare_del(si->category);

	free(si);
     }

   if (inst->apps)
     {
	e_order_update_callback_set(inst->apps, NULL, NULL);
       	e_object_del(E_OBJECT(inst->apps));
     }
}

static void
_launcher_event_update_free(void *data __UNUSED__, void *event)
{
   Drawer_Event_Source_Update *ev;

   ev = event;
   eina_stringshare_del(ev->id);
   free(ev);
}

static void
_launcher_conf_activation_cb(void *data1, void *data2 __UNUSED__)
{
   Drawer_Plugin *p = NULL;
   Instance *inst = NULL;
   E_Config_Dialog_View *v = NULL;
   char buf[4096];

   p = data1;
   inst = p->data;
   /* is this config dialog already visible ? */
   if (e_config_dialog_find("Drawer_Launcher", "_e_module_drawer_cfg_dlg"))
     return;

   v = E_NEW(E_Config_Dialog_View, 1);
   if (!v) return;

   v->create_cfdata = _launcher_cf_create_data;
   v->free_cfdata = _launcher_cf_free_data;
   v->basic.create_widgets = _launcher_cf_basic_create;
   v->basic.apply_cfdata = _launcher_cf_basic_apply;

   /* Icon in the theme */
   snprintf(buf, sizeof(buf), "%s/e-module-drawer.edj", drawer_conf->module->dir);

   /* create new config dialog */
   _cfd = e_config_dialog_new(e_container_current_get(e_manager_current_get()),
	 D_("Drawer Plugin : Launcher"), "Drawer_Launcher", 
	 "_e_module_drawer_cfg_dlg", buf, 0, v, inst);

   e_dialog_resizable_set(_cfd->dia, 1);
}

static void 
_launcher_cb_menu_post(void *data, E_Menu *menu)
{
   Instance *inst = NULL;

   if (!(inst = data)) return;
   if (inst->m_data)
     E_FREE(inst->m_data);
   if (!inst->menu) return;
   e_object_del(E_OBJECT(inst->menu));
   inst->menu = NULL;
}

static void
_launcher_cb_menu_item_properties(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Drawer_Source_Item *si = data;

   e_desktop_edit(e_container_current_get(e_manager_current_get()), si->desktop);
}

static void
_launcher_cb_menu_item_remove(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Launcher_Menu_Data *m_data = data;
   
   _launcher_source_item_free(m_data->inst, m_data->si);
}

/* Local Functions */
static void *
_launcher_cf_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata = NULL;

   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   cfdata->inst = cfd->data;
   _launcher_cf_fill_data(cfdata);
   return cfdata;
}

static void 
_launcher_cf_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   if (cfdata->dir) E_FREE(cfdata->dir);

   _cfd = NULL;
   E_FREE(cfdata);
}

static void 
_launcher_cf_fill_data(E_Config_Dialog_Data *cfdata)
{
   cfdata->dir = strdup(cfdata->inst->conf->dir);
   cfdata->sort_rel = cfdata->inst->conf->sort_rel;
}

static Evas_Object *
_launcher_cf_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ol, *ob, *ot;

   o = e_widget_list_add(evas, 0, 0);

   of = e_widget_frametable_add(evas, D_("Selected Bar Source"), 0);
   ol = e_widget_ilist_add(evas, 32, 32, &(cfdata->dir));
   cfdata->ilist = ol;
   _launcher_cf_load_ilist(cfdata);
   e_widget_min_size_set(ol, 140, 140);
   e_widget_frametable_object_append(of, ol, 0, 0, 1, 2, 1, 1, 1, 0);
   
   ot = e_widget_table_add(evas, 0);
   ob = e_widget_button_add(evas, D_("Add"), "widget/add", _cb_add, cfdata, NULL);
   e_widget_table_object_append(ot, ob, 0, 0, 1, 1, 1, 1, 1, 0);
   ob = e_widget_button_add(evas, D_("Delete"), "widget/del", _cb_del, cfdata, NULL);
   e_widget_table_object_append(ot, ob, 0, 1, 1, 1, 1, 1, 1, 0);
   ob = e_widget_button_add(evas, D_("Configure"), "widget/config", _cb_config, cfdata, NULL);
   e_widget_table_object_append(ot, ob, 0, 2, 1, 1, 1, 1, 1, 0);   

   if (!e_configure_registry_exists("applications/ibar_applications")) 
     e_widget_disabled_set(ob, 1);
   
   e_widget_frametable_object_append(of, ot, 1, 0, 1, 1, 1, 1, 1, 0);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_framelist_add(evas, D_("Sorting options"), 0);
   ob = e_widget_check_add(evas, D_("Sort applications by usage"), &(cfdata->sort_rel));
   e_widget_framelist_object_append(of, ob);  

   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}

static int 
_launcher_cf_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Instance *inst = NULL;
   Drawer_Event_Source_Update *ev;

   inst = cfdata->inst;
   eina_stringshare_del(cfdata->inst->conf->dir);

   cfdata->inst->conf->dir = eina_stringshare_add(cfdata->dir);
   cfdata->inst->conf->sort_rel = cfdata->sort_rel;

   _launcher_description_create(inst);

   ev = E_NEW(Drawer_Event_Source_Update, 1);
   ev->source = inst->source;
   ev->id = eina_stringshare_add(inst->conf->id);
   ecore_event_add(DRAWER_EVENT_SOURCE_UPDATE, ev, _launcher_event_update_free, NULL);

   e_config_save_queue();
   return 1;
}

static void
_cb_add(void *data, void *data2)
{
   E_Config_Dialog_Data *cfdata;
   
   cfdata = data;
   e_entry_dialog_show(D_("Create new Launcher source"), "enlightenment/e",
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
   if (cfdata->dialog_delete)
     return;

   snprintf(buf, sizeof(buf), D_("You requested to delete \"%s\".<br><br>"
				"Are you sure you want to delete this bar source?"),
	    cfdata->dir);
   
   dialog = e_confirm_dialog_show(D_("Are you sure you want to delete this bar source?"),
                                  "enlightenment/exit", buf, NULL, NULL, 
                                  _cb_confirm_dialog_yes, NULL, cfdata, NULL, _cb_confirm_dialog_destroy, cfdata);
   cfdata->dialog_delete = dialog;
}

static void
_cb_config(void *data, void *data2)
{
   char path[PATH_MAX];
   E_Config_Dialog_Data *cfdata;
   
   cfdata = data;
   snprintf(path, sizeof(path), "%s/.e/e/applications/bar/%s/.order", 
	    e_user_homedir_get(), cfdata->dir);
   e_configure_registry_call("internal/ibar_other",
			     e_container_current_get(e_manager_current_get()),
			     path);
}

static void
_cb_entry_ok(char *text, void *data)
{
   char buf[4096];
   char tmp[4096];
   FILE *f;
   
   snprintf(buf, sizeof(buf), "%s/.e/e/applications/bar/%s", 
	    e_user_homedir_get(), text);

   if (!ecore_file_exists(buf)) 
     {
	ecore_file_mkdir(buf);
	snprintf(buf, sizeof(buf), "%s/.e/e/applications/bar/%s/.order", 
		 e_user_homedir_get(), text);
	f = fopen(buf, "w");
	if (f) 
	  {
	     /* Populate this .order file with some defaults */
	     snprintf(tmp, sizeof(tmp), "xterm.desktop\n" "sylpheed.desktop\n" 
		      "firefox.desktop\n" "openoffice.desktop\n" "xchat.desktop\n"
		      "gimp.desktop\n" "xmms.desktop\n");
	     fwrite(tmp, sizeof(char), strlen(tmp), f);
	     fclose(f);
	  }
     }
   
   _launcher_cf_load_ilist(data);
}

static void
_cb_confirm_dialog_yes(void *data)
{
   E_Config_Dialog_Data *cfdata;
   char buf[4096];
   
   cfdata = data;
   snprintf(buf, sizeof(buf), "%s/.e/e/applications/bar/%s", e_user_homedir_get(), cfdata->dir);
   if (ecore_file_is_dir(buf))
     ecore_file_recursive_rm(buf);
   
   _launcher_cf_load_ilist(cfdata);
}

static void
_cb_confirm_dialog_destroy(void *data) 
{
   E_Config_Dialog_Data *cfdata;
   
   cfdata = data;
   cfdata->dialog_delete = NULL;
}

static void 
_launcher_cf_load_ilist(E_Config_Dialog_Data *cfdata)
{
   Eina_List *dirs;
   const char *home;
   char buf[4096], *file;
   int selnum = -1;
   int i = 0;

   e_widget_ilist_clear(cfdata->ilist);
   
   home = e_user_homedir_get();
   snprintf(buf, sizeof(buf), "%s/.e/e/applications/bar", home);
   dirs = ecore_file_ls(buf);
   EINA_LIST_FREE(dirs, file)
     {
      if (file[0] == '.') goto end;
	     snprintf(buf, sizeof(buf), "%s/.e/e/applications/bar/%s", home, file);
	     if (ecore_file_is_dir(buf))
	       {
		  e_widget_ilist_append(cfdata->ilist, NULL, file, NULL, NULL, file);
		  if ((cfdata->dir) && (!strcmp(cfdata->dir, file)))
		    selnum = i;
		  i++;
	       }

   end:
      free(file);
     }
   e_widget_ilist_go(cfdata->ilist);
   if (selnum >= 0)
     e_widget_ilist_selected_set(cfdata->ilist, selnum);   
}
