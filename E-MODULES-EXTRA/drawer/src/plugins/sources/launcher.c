/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include <e.h>
#include "launcher.h"

/* Local Structures */
typedef struct _Instance Instance;
typedef struct _Launcher_Conf Launcher_Conf;
typedef struct _Conf Conf;
typedef struct _Conf_Rating Conf_Rating;

struct _Instance 
{
   Drawer_Source *source;

   E_Order *apps;
   Eina_List *items;

   Conf *conf;
};

struct _Launcher_Conf
{
   Eina_List *items;
};

struct _Conf
{
   const char *id;

   const char *dir;

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
};

#define CONF_RATING(obj) ((Conf_Rating *) obj)

static void _launcher_conf_new(Instance *inst);
static void _launcher_sources_rating_discount(Instance *inst, int min);

static int _launcher_cb_sort_rating(const void *data1, const void *data2);
static void _launcher_cb_app_change(void *data, E_Order *eo __UNUSED__);
static Drawer_Source_Item *_launcher_source_item_fill(Instance *inst, Efreet_Desktop *desktop);
static void _launcher_source_item_free(Instance *inst);
static void _launcher_event_update_free(void *data __UNUSED__, void *event);
static void _launcher_conf_activation_cb(void *data1, void *data2 __UNUSED__);

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

static Launcher_Conf *launcher_conf = NULL;
static E_Config_Dialog *_cfd = NULL;
static int uuid = 0;
static E_Config_DD *_launcher_conf_edd = NULL;
static E_Config_DD *_conf_edd = NULL;
static E_Config_DD *_conf_rel_edd = NULL;

EAPI void *
drawer_plugin_init(Drawer_Plugin *p)
{
   Instance *inst = NULL;

   inst = E_NEW(Instance, 1);

   inst->source = DRAWER_SOURCE(p);

   /* Define EET Data Storage */
   _conf_rel_edd = E_CONFIG_DD_NEW("Conf_Rating", Conf_Rating);
   #undef T
   #undef D
   #define T Conf_Rating 
   #define D _conf_rel_edd
   E_CONFIG_VAL(D, T, label, STR);
   E_CONFIG_VAL(D, T, rating, INT);

   _conf_edd = E_CONFIG_DD_NEW("Conf", Conf);
   #undef T
   #undef D
   #define T Conf
   #define D _conf_edd
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, dir, STR);
   E_CONFIG_LIST(D, T, ratings, _conf_rel_edd); /* the list */

   _launcher_conf_edd = E_CONFIG_DD_NEW("Launcher_Conf", Launcher_Conf);
   #undef T
   #undef D
   #define T Launcher_Conf
   #define D _launcher_conf_edd
   E_CONFIG_LIST(D, T, items, _conf_edd); /* the list */

   if (!launcher_conf)
     launcher_conf = e_config_domain_load("module.drawer.launcher", _launcher_conf_edd);
   if (!launcher_conf)
     {
	launcher_conf = E_NEW(Launcher_Conf, 1);
	e_config_save_queue();
     }

   _launcher_conf_new(inst);

   return inst;
}

EAPI int
drawer_plugin_shutdown(Drawer_Plugin *p)
{
   Instance *inst = NULL;
   
   inst = p->data;

   _launcher_source_item_free(inst);

   if (inst->conf->id) eina_stringshare_del(inst->conf->id);
   if (inst->conf->dir) eina_stringshare_del(inst->conf->dir);

   while (inst->conf->ratings)
     {
	Conf_Rating *r = NULL;

	r = inst->conf->ratings->data;

	inst->conf->ratings = eina_list_remove_list(
	      inst->conf->ratings, inst->conf->ratings);

	if (r->label) eina_stringshare_del(r->label);

	E_FREE(r);
     }
   E_FREE(inst->conf);
   E_FREE(inst);

   E_CONFIG_DD_FREE(_launcher_conf_edd);
   E_CONFIG_DD_FREE(_conf_edd);
   E_CONFIG_DD_FREE(_conf_rel_edd);

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
   Instance *inst = NULL;
   
   inst = p->data;

   e_config_domain_save("module.drawer.launcher", _launcher_conf_edd, launcher_conf);
}

EAPI Eina_List *
drawer_source_list(Drawer_Source *s)
{
   Instance *inst = NULL;
   char buf[4096];
   const char *homedir;
   int min = 0;

   if (!(inst = DRAWER_PLUGIN(s)->data)) return NULL;

   _launcher_source_item_free(inst);

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

/*    XXX: needs to be switched off via conf */
   if (min > 10)
     _launcher_sources_rating_discount(inst, min);
   inst->items = eina_list_sort(inst->items, eina_list_count(inst->items),
                                _launcher_cb_sort_rating);
   return inst->items;
}

EAPI void
drawer_source_activate(Drawer_Source *s, Drawer_Source_Item *si, E_Zone *zone)
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

   /* XXX: needs to be switched off via conf */
   CONF_RATING(si->priv)->rating++;
   _launcher_cb_app_change(DRAWER_PLUGIN(s)->data, NULL);
}

static void
_launcher_conf_new(Instance *inst)
{
   Eina_List *l;
   Conf *ci;
   char buf[128];

   snprintf(buf, sizeof(buf), "drawer.launcher.%d", ++uuid);
   EINA_LIST_FOREACH(launcher_conf->items, l, ci)
     {
	if (!(ci = l->data)) continue;
	if ((ci->id) && (!strcmp(ci->id, buf))) break;
     }
   if (ci)
     inst->conf = ci;
   else
     {
	inst->conf = E_NEW(Conf, 1);
	inst->conf->dir = eina_stringshare_add("default");

	inst->conf->id = eina_stringshare_add(buf);

	launcher_conf->items = eina_list_append(launcher_conf->items, inst->conf);

	e_config_save_queue();
     }
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

   return CONF_RATING(si1->priv)->rating - CONF_RATING(si2->priv)->rating;
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
   ecore_event_add(DRAWER_EVENT_SOURCE_UPDATE, ev, _launcher_event_update_free, NULL);
}

static Drawer_Source_Item *
_launcher_source_item_fill(Instance *inst, Efreet_Desktop *desktop)
{
   Drawer_Source_Item *si = NULL;
   Conf_Rating *r;
   Eina_List *l;
   int found = 0;
   char *path;

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
_launcher_source_item_free(Instance *inst)
{
   while (inst->items)
     {
	Drawer_Source_Item *si = NULL;
	
	si = inst->items->data;
	inst->items = eina_list_remove_list(inst->items, inst->items);
	if (si->label) eina_stringshare_del(si->label);
	if (si->description) eina_stringshare_del(si->description);

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
   free(event);
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

   return o;
}

static int 
_launcher_cf_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Instance *inst = NULL;
   Drawer_Event_Source_Update *ev;

   inst = cfdata->inst;
   if (cfdata->inst->conf->dir)
     eina_stringshare_del(cfdata->inst->conf->dir);

   cfdata->inst->conf->dir = eina_stringshare_add(cfdata->dir);

   ev = E_NEW(Drawer_Event_Source_Update, 1);
   ev->source = inst->source;
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
   Ecore_List *dirs;
   const char *home;
   char buf[4096], *file;
   int selnum = -1;

   e_widget_ilist_clear(cfdata->ilist);
   
   home = e_user_homedir_get();
   snprintf(buf, sizeof(buf), "%s/.e/e/applications/bar", home);
   dirs = ecore_file_ls(buf);
   
   if (dirs)
     {
	int i = 0;
	
	while ((file = ecore_list_next(dirs)))
	  {
	     if (file[0] == '.') continue;
	     snprintf(buf, sizeof(buf), "%s/.e/e/applications/bar/%s", home, file);
	     if (ecore_file_is_dir(buf))
	       {
		  e_widget_ilist_append(cfdata->ilist, NULL, file, NULL, NULL, file);
		  if ((cfdata->dir) && (!strcmp(cfdata->dir, file)))
		    selnum = i;
		  i++;
	       }
	  }
	ecore_list_destroy(dirs);
     }
   e_widget_ilist_go(cfdata->ilist);
   if (selnum >= 0)
     e_widget_ilist_selected_set(cfdata->ilist, selnum);   
}

