/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2,t0,(0
 */
#include <e.h>
#include "launcher.h"

/* Local Structures */
typedef struct _Instance Instance;
typedef struct _Dirwatcher_Conf Dirwatcher_Conf;
typedef struct _Conf Conf;

struct _Instance 
{
   Drawer_Source *source;

   Conf *conf;

   Eina_List *items;

   struct {
	E_Config_DD *dirwatcher;
	E_Config_DD *conf;
   } edd;

   Ecore_File_Monitor *monitor;

   const char *description;
   const char *parent_id;
};

struct _Dirwatcher_Conf
{
   Eina_List *items;
};

struct _Conf
{
   const char *id;

   const char *dir;
};

struct _E_Config_Dialog_Data 
{
   Instance *inst;

   const char *dir;
};

EAPI Drawer_Plugin_Api drawer_plugin_api = {DRAWER_PLUGIN_API_VERSION, "Directory Watcher"};

static void _dirwatcher_description_create(Instance *inst);
static void _dirwatcher_conf_new(Instance *inst, const char *id);
static void _dirwatcher_source_items_free(Instance *inst);
static Drawer_Source_Item * _dirwatcher_source_item_fill(Instance *inst, const char *file);
static void _dirwatcher_event_update_free(void *data __UNUSED__, void *event);

static void _dirwatcher_monitor_cb(void *data, Ecore_File_Monitor *em __UNUSED__, Ecore_File_Event event __UNUSED__, const char *path __UNUSED__);
static void _dirwatcher_conf_activation_cb(void *data1, void *data2 __UNUSED__);

static Dirwatcher_Conf *dirwatcher_conf = NULL;

EAPI void *
drawer_plugin_init(Drawer_Plugin *p, const char *id)
{
   Instance *inst = NULL;

   inst = E_NEW(Instance, 1);

   inst->source = DRAWER_SOURCE(p);

   /* Define EET Data Storage */
   inst->edd.conf = E_CONFIG_DD_NEW("Conf", Conf);
   #undef T
   #undef D
   #define T Conf
   #define D inst->edd.conf
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, dir, STR);

   inst->edd.dirwatcher = E_CONFIG_DD_NEW("Dirwatcher_Conf", Dirwatcher_Conf);
   #undef T
   #undef D
   #define T Dirwatcher_Conf
   #define D inst->edd.dirwatcher
   E_CONFIG_LIST(D, T, items, inst->edd.conf); /* the list */

   if (!dirwatcher_conf)
     dirwatcher_conf = e_config_domain_load("module.drawer.dirwatcher", inst->edd.dirwatcher);
   if (!dirwatcher_conf)
     {
	dirwatcher_conf = E_NEW(Dirwatcher_Conf, 1);
	e_config_save_queue();
     }


   inst->parent_id = eina_stringshare_add(id);
   _dirwatcher_conf_new(inst, id);

   _dirwatcher_description_create(inst);

   return inst;
}

EAPI int
drawer_plugin_shutdown(Drawer_Plugin *p)
{
   Instance *inst = NULL;
   
   inst = p->data;

   if (inst->monitor)
     ecore_file_monitor_del(inst->monitor);

   if (inst->description) eina_stringshare_del(inst->description);
   if (inst->parent_id) eina_stringshare_del(inst->parent_id);
   if (inst->conf->id) eina_stringshare_del(inst->conf->id);
   if (inst->conf->dir) eina_stringshare_del(inst->conf->dir);

   dirwatcher_conf->items = eina_list_remove(dirwatcher_conf->items, inst->conf);
   if (!eina_list_count(dirwatcher_conf->items))
     E_FREE(dirwatcher_conf);

   E_CONFIG_DD_FREE(inst->edd.dirwatcher);
   E_CONFIG_DD_FREE(inst->edd.conf);
   E_FREE(inst->conf);
   E_FREE(inst);

   return 1;
}

EAPI Eina_List *
drawer_source_list(Drawer_Source *s)
{
   Ecore_List *files;
   Instance *inst = NULL;
   char *file;

   if (!(inst = DRAWER_PLUGIN(s)->data)) return NULL;
   if (!(ecore_file_is_dir(inst->conf->dir))) return NULL;
   if (!inst->monitor)
     inst->monitor = ecore_file_monitor_add(inst->conf->dir,
					    _dirwatcher_monitor_cb, inst);

   _dirwatcher_source_items_free(inst);

   files = ecore_file_ls(inst->conf->dir);

   if (files)
     {
	while ((file = ecore_list_next(files)))
	  {
	     Drawer_Source_Item *si;

	     if (file[0] == '.') continue;
	     si = _dirwatcher_source_item_fill(inst, file);
	     if (si)
	       inst->items = eina_list_append(inst->items, si);
	  }
	ecore_list_destroy(files);
     }

   return inst->items;
}

EAPI void
drawer_source_activate(Drawer_Source *s, Drawer_Source_Item *si, E_Zone *zone)
{
}

EAPI Evas_Object *
drawer_plugin_config_get(Drawer_Plugin *p, Evas *evas)
{
   Evas_Object *button;

   button = e_widget_button_add(evas, D_("Directory Watcher settings"), NULL, _dirwatcher_conf_activation_cb, p, NULL);

   return button;
}

EAPI void
drawer_plugin_config_save(Drawer_Plugin *p)
{
   Instance *inst;

   inst = p->data;
   if (!inst->edd.dirwatcher) return;
   e_config_domain_save("module.drawer.dirwatcher", inst->edd.dirwatcher, dirwatcher_conf);
}

EAPI const char *
drawer_source_description_get(Drawer_Source *s)
{
   Instance *inst;

   inst = DRAWER_PLUGIN(s)->data;

   return inst->description;
}

static void
_dirwatcher_conf_new(Instance *inst, const char *id)
{
   Eina_List *l;
   Conf *ci;
   char buf[128];

   snprintf(buf, sizeof(buf), "%s.dirwatcher", id);
   EINA_LIST_FOREACH(dirwatcher_conf->items, l, ci)
     {
	if (!(ci = l->data)) continue;
	if ((ci->id) && (!strcmp(ci->id, buf))) break;
     }
   if (ci)
     inst->conf = ci;
   else
     {
	char buf2[4096];

	snprintf(buf2, sizeof(buf2), "%s/Desktop", e_user_homedir_get());

	inst->conf = E_NEW(Conf, 1);
	inst->conf->dir = eina_stringshare_add(buf2);
	inst->conf->id = eina_stringshare_add(buf);

	dirwatcher_conf->items = eina_list_append(dirwatcher_conf->items, inst->conf);

	e_config_save_queue();
     }
}

static void
_dirwatcher_description_create(Instance *inst)
{
   char buf[1024];
   char path[4096];
   const char *homedir;

   if (inst->description) eina_stringshare_del(inst->description);
   homedir = e_user_homedir_get();
   if (!(strncmp(inst->conf->dir, homedir, strlen(inst->conf->dir))))
     snprintf(buf, sizeof(buf), D_("Home"));
   else if (!(strncmp(inst->conf->dir, homedir, strlen(homedir))))
     {
	snprintf(path, sizeof(path), "%s", inst->conf->dir);
	snprintf(buf, sizeof(buf), "%s", path + strlen(homedir) + 1);
     }
   else
     snprintf(buf, sizeof(buf), "%s", inst->conf->dir);
   inst->description = eina_stringshare_add(buf);
}

static void
_dirwatcher_source_items_free(Instance *inst)
{
   while (inst->items)
     {
	Drawer_Source_Item *si = NULL;
	
	si = inst->items->data;
	inst->items = eina_list_remove_list(inst->items, inst->items);
	if (si->label) eina_stringshare_del(si->label);
	if (si->description) eina_stringshare_del(si->description);
	if (si->category) eina_stringshare_del(si->category);

	free(si);
     }
}

static Drawer_Source_Item *
_dirwatcher_source_item_fill(Instance *inst, const char *file)
{
   Drawer_Source_Item *si = NULL;
   char buf[4096];

   si = E_NEW(Drawer_Source_Item, 1);

   snprintf(buf, sizeof(buf), "%s/%s", inst->conf->dir, file);
   if ((e_util_glob_case_match(buf, "*.desktop")) ||
       (e_util_glob_case_match(buf, "*.directory")))
     {
	Efreet_Desktop *desktop;

	desktop = efreet_desktop_new(buf);
	if (!desktop) return NULL;
	si->label = eina_stringshare_add(desktop->name);
	efreet_desktop_free(desktop);
     }
   else
     si->label = eina_stringshare_add(file);

   si->file_path = eina_stringshare_add(buf);

   snprintf(buf, sizeof(buf), "%s (%s)", e_fm_mime_filename_get(si->file_path), e_util_size_string_get(ecore_file_size(si->file_path)));
   si->description = eina_stringshare_add(buf);

   return si;
}

static void
_dirwatcher_event_update_free(void *data __UNUSED__, void *event)
{
   Drawer_Event_Source_Update *ev;

   ev = event;
   if (ev->id) eina_stringshare_del(ev->id);
   free(ev);
}

static void
_dirwatcher_monitor_cb(void *data, Ecore_File_Monitor *em __UNUSED__, Ecore_File_Event event __UNUSED__, const char *path __UNUSED__)
{
   Instance *inst = NULL;
   Drawer_Event_Source_Update *ev;

   inst = data;

   ev = E_NEW(Drawer_Event_Source_Update, 1);
   ev->source = inst->source;
   ev->id = eina_stringshare_add(inst->parent_id);
   ecore_event_add(DRAWER_EVENT_SOURCE_UPDATE, ev, _dirwatcher_event_update_free, NULL);
}

static void
_dirwatcher_conf_activation_cb(void *data1, void *data2 __UNUSED__)
{
   Drawer_Plugin *p = NULL;
   Instance *inst = NULL;
   E_Config_Dialog_View *v = NULL;
   char buf[4096];

   p = data1;
   inst = p->data;
   /* is this config dialog already visible ? */
   if (e_config_dialog_find("Drawer_Dirwatcher", "_e_module_drawer_cfg_dlg"))
     return;

#if 0
   v = E_NEW(E_Config_Dialog_View, 1);
   if (!v) return;

   v->create_cfdata = _dirwatcher_cf_create_data;
   v->free_cfdata = _dirwatcher_cf_free_data;
   v->basic.create_widgets = _dirwatcher_cf_basic_create;
   v->basic.apply_cfdata = _dirwatcher_cf_basic_apply;

   /* Icon in the theme */
   snprintf(buf, sizeof(buf), "%s/e-module-drawer.edj", drawer_conf->module->dir);

   /* create new config dialog */
   _cfd = e_config_dialog_new(e_container_current_get(e_manager_current_get()),
	 D_("Drawer Plugin : Launcher"), "Drawer_Dirwatcher", 
	 "_e_module_drawer_cfg_dlg", buf, 0, v, inst);

   e_dialog_resizable_set(_cfd->dia, 1);
#endif
}

