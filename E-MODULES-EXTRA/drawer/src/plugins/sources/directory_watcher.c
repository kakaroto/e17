/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2,t0,(0,W4
 */
#include "directory_watcher.h"

/* Local Structures */
typedef struct _Instance Instance;
typedef struct _Conf Conf;
typedef struct _Dirwatcher_Priv Dirwatcher_Priv;

typedef enum
{
   SORT_NAME,
   SORT_ATIME,
   SORT_MTIME,
   SORT_CTIME,
   SORT_SIZE,
} Sort_Type;

struct _Instance 
{
   Drawer_Source *source;

   Conf *conf;

   Eina_List *items;

   struct {
	E_Config_DD *conf;
   } edd;

   Ecore_File_Monitor *monitor;

   const char *description;
};

struct _Conf
{
   const char *id;

   const char *dir;
   const char *fm;

   Sort_Type sort_type;

   Eina_Bool sort_dir;
};

struct _Dirwatcher_Priv
{
   Eina_Bool dir : 1;
   Eina_Bool link : 1;
   Eina_Bool mount : 1;

   const char *mime;

   Instance *inst;
};

struct _E_Config_Dialog_Data 
{
   Instance *inst;

   char *dir;
   char *fm;

   int sort_dir;
   int sort_type;
};

EAPI Drawer_Plugin_Api drawer_plugin_api = {DRAWER_PLUGIN_API_VERSION, "Directory Watcher"};

static void _dirwatcher_directory_activate(Instance *inst, E_Zone *zone, const char *path);
static void _dirwatcher_description_create(Instance *inst);
static void _dirwatcher_source_items_free(Instance *inst);
static Drawer_Source_Item * _dirwatcher_source_item_fill(Instance *inst, const char *file);
static void _dirwatcher_event_update_free(void *data __UNUSED__, void *event);

static void _dirwatcher_monitor_cb(void *data, Ecore_File_Monitor *em __UNUSED__, Ecore_File_Event event __UNUSED__, const char *path);
static void _dirwatcher_conf_activation_cb(void *data1, void *data2 __UNUSED__);

static void * _dirwatcher_cf_create_data(E_Config_Dialog *cfd);
static void _dirwatcher_cf_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _dirwatcher_cf_fill_data(E_Config_Dialog_Data *cfdata);
static Evas_Object * _dirwatcher_cf_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _dirwatcher_cf_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static int _dirwatcher_cb_sort(const void *data1, const void *data2);
static int _dirwatcher_cb_sort_dir(const Drawer_Source_Item *si1, const Drawer_Source_Item *si2);

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
   E_CONFIG_VAL(D, T, dir, STR);
   E_CONFIG_VAL(D, T, fm, STR);
   E_CONFIG_VAL(D, T, sort_type, INT);
   E_CONFIG_VAL(D, T, sort_dir, INT);

   snprintf(buf, sizeof(buf), "module.drawer/%s.dirwatcher", id);
   inst->conf = e_config_domain_load(buf, inst->edd.conf);
   if (!inst->conf)
     {
	char buf2[4096];

	snprintf(buf2, sizeof(buf2), "%s/Desktop", e_user_homedir_get());

	inst->conf = E_NEW(Conf, 1);
	inst->conf->sort_dir = EINA_TRUE;
	inst->conf->dir = eina_stringshare_add(buf2);
	inst->conf->fm = eina_stringshare_add("");
	inst->conf->id = eina_stringshare_add(id);

	e_config_save_queue();
     }

   inst->monitor = ecore_file_monitor_add(inst->conf->dir,
					  _dirwatcher_monitor_cb, inst);
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

   eina_stringshare_del(inst->description);
   eina_stringshare_del(inst->conf->id);
   eina_stringshare_del(inst->conf->dir);
   eina_stringshare_del(inst->conf->fm);

   E_CONFIG_DD_FREE(inst->edd.conf);
   E_FREE(inst->conf);
   E_FREE(inst);

   return 1;
}

EAPI Eina_List *
drawer_source_list(Drawer_Source *s, Evas *evas __UNUSED__)
{
   Eina_List *files;
   Instance *inst = NULL;
   char *file;

   if (!(inst = DRAWER_PLUGIN(s)->data)) return NULL;
   if (!(ecore_file_is_dir(inst->conf->dir))) return NULL;

   _dirwatcher_source_items_free(inst);

   files = ecore_file_ls(inst->conf->dir);
   EINA_LIST_FREE(files, file)
	  {
	     Drawer_Source_Item *si;

	if (file[0] == '.') goto end;
	     si = _dirwatcher_source_item_fill(inst, file);
	     if (si)
	       inst->items = eina_list_append(inst->items, si);

     end:
	free(file);
     }

   inst->items = eina_list_sort(inst->items,
	      eina_list_count(inst->items), _dirwatcher_cb_sort);

   return inst->items;
}

EAPI void
drawer_source_activate(Drawer_Source *s, Drawer_Source_Item *si, E_Zone *zone)
{
   Dirwatcher_Priv *p = NULL;

   p = si->priv;
   if (p->dir)
     return _dirwatcher_directory_activate(p->inst, zone, si->file_path);
   if (si->file_path)
     {
	if ((e_util_glob_case_match(si->file_path, "*.desktop")) ||
	    (e_util_glob_case_match(si->file_path, "*.directory")))
	  {
	     Efreet_Desktop *desktop;

	     desktop = efreet_desktop_new(si->file_path);
	     if (!desktop) return;

	     e_exec(e_util_zone_current_get(e_manager_current_get()),
		    desktop, NULL, NULL, NULL);
	     if (p->mime)
	       e_exehist_mime_desktop_add(p->mime, desktop);

	     efreet_desktop_free(desktop);
	  }

	/* XXX: open the file with the default application */
	return;
     }
}

EAPI void
drawer_source_trigger(Drawer_Source *s, E_Zone *zone)
{
   Instance *inst = DRAWER_PLUGIN(s)->data;

   _dirwatcher_directory_activate(inst, zone, inst->conf->dir);
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
   char buf[128];

   inst = p->data;
   snprintf(buf, sizeof(buf), "module.drawer/%s.dirwatcher", inst->conf->id);
   e_config_domain_save(buf, inst->edd.conf, inst->conf);
}

EAPI const char *
drawer_source_description_get(Drawer_Source *s)
{
   Instance *inst;

   inst = DRAWER_PLUGIN(s)->data;

   return inst->description;
}

static void
_dirwatcher_directory_activate(Instance *inst, E_Zone *zone, const char *path)
{
   char exec[PATH_MAX];

   if (inst->conf->fm && (inst->conf->fm[0] != '\0'))
     {
	snprintf(exec, PATH_MAX, "%s \"%s\"", inst->conf->fm, path);
	e_exec(zone, NULL, exec, NULL, NULL);
     }
   else
     {
	E_Action *act = NULL;

	act = e_action_find("fileman");
	if (act)
	  {
	     if (act && act->func.go)
	       act->func.go(E_OBJECT(e_manager_current_get()), path);
	  }
     }
}

static void
_dirwatcher_description_create(Instance *inst)
{
   char buf[1024];
   char path[4096];
   const char *homedir;

   eina_stringshare_del(inst->description);
   homedir = e_user_homedir_get();
   if (!(strncmp(inst->conf->dir, homedir, 4096)))
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
	eina_stringshare_del(si->label);
	eina_stringshare_del(si->description);
	eina_stringshare_del(si->category);

	E_FREE(si->priv);
	E_FREE(si);
     }
}

static Drawer_Source_Item *
_dirwatcher_source_item_fill(Instance *inst, const char *file)
{
   Drawer_Source_Item *si = NULL;
   Dirwatcher_Priv *p = NULL;
   char buf[4096];
   const char *mime;

   si = E_NEW(Drawer_Source_Item, 1);
   p = E_NEW(Dirwatcher_Priv, 1);

   si->priv = p;

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

   mime = e_fm_mime_filename_get(si->file_path);
   if (mime)
     {
	snprintf(buf, sizeof(buf), "%s (%s)", mime,
		 e_util_size_string_get(ecore_file_size(si->file_path)));
	p->mime = mime;
     }
   else if (ecore_file_is_dir(si->file_path))
     {
	snprintf(buf, sizeof(buf), D_("Directory (%s)"),
		 e_util_size_string_get(ecore_file_size(si->file_path)));
	p->dir = EINA_TRUE;
     }
   else
     snprintf(buf, sizeof(buf), "%s (%s)", basename((char *) si->file_path),
	      e_util_size_string_get(ecore_file_size(si->file_path)));
   si->description = eina_stringshare_add(buf);

   p->inst = inst;

   return si;
}

static void
_dirwatcher_event_update_free(void *data __UNUSED__, void *event)
{
   Drawer_Event_Source_Update *ev;

   ev = event;
   eina_stringshare_del(ev->id);
   free(ev);
}

static void
_dirwatcher_monitor_cb(void *data, Ecore_File_Monitor *em __UNUSED__, Ecore_File_Event event __UNUSED__, const char *path)
{
   Instance *inst = NULL;
   Drawer_Event_Source_Update *ev;
   char *base;

   inst = data;
   base = basename((char *) path);
   if (base[0] == '.') return;

   ev = E_NEW(Drawer_Event_Source_Update, 1);
   ev->source = inst->source;
   ev->id = eina_stringshare_add(inst->conf->id);
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
	 D_("Drawer Plugin : Directory Watcher"), "Drawer_Dirwatcher", 
	 "_e_module_drawer_cfg_dlg", buf, 0, v, inst);

   e_dialog_resizable_set(_cfd->dia, 1);
}

static void *
_dirwatcher_cf_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata = NULL;

   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   cfdata->inst = cfd->data;
   _dirwatcher_cf_fill_data(cfdata);
   return cfdata;
}

static void 
_dirwatcher_cf_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   if (cfdata->dir) E_FREE(cfdata->dir);
   if (cfdata->fm) E_FREE(cfdata->fm);

   _cfd = NULL;
   E_FREE(cfdata);
}

static void 
_dirwatcher_cf_fill_data(E_Config_Dialog_Data *cfdata)
{
   cfdata->dir = strdup(cfdata->inst->conf->dir);
   cfdata->fm = strdup(cfdata->inst->conf->fm);
   cfdata->sort_dir = cfdata->inst->conf->sort_dir;
}

static Evas_Object *
_dirwatcher_cf_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob;
   E_Radio_Group *gr;

   o = e_widget_list_add(evas, 0, 0);

   of = e_widget_framelist_add(evas, D_("Watch path"), 1);
   ob = e_widget_entry_add(evas, &cfdata->dir, NULL, NULL, NULL);
   e_widget_framelist_object_append(of, ob);  

   e_widget_list_object_append(o, of, 1, 0, 0.5);

   of = e_widget_framelist_add(evas, D_("Custom file manager"), 1);
   ob = e_widget_entry_add(evas, &cfdata->fm, NULL, NULL, NULL);
   e_widget_framelist_object_append(of, ob);

   e_widget_list_object_append(o, of, 1, 0, 0.5);

   of = e_widget_framelist_add(evas, D_("Sort Options"), 0);
   ob = e_widget_check_add(evas, D_("Sort directories first"), &cfdata->sort_dir);
   e_widget_framelist_object_append(of, ob);

   gr = e_widget_radio_group_new(&cfdata->sort_type);
   ob = e_widget_radio_add(evas, D_("Sort by name"), SORT_NAME, gr);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, D_("Sort by access time"), SORT_ATIME, gr);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, D_("Sort by modification time"), SORT_MTIME, gr);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, D_("Sort by change time"), SORT_CTIME, gr);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, D_("Sort by size"), SORT_SIZE, gr);
   e_widget_framelist_object_append(of, ob);

   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}

static int 
_dirwatcher_cf_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Instance *inst = NULL;
   Drawer_Event_Source_Update *ev;
   char *path;

   inst = cfdata->inst;
   eina_stringshare_del(cfdata->inst->conf->dir);
   eina_stringshare_del(cfdata->inst->conf->fm);
   cfdata->inst->conf->sort_dir = cfdata->sort_dir;
   cfdata->inst->conf->sort_type = cfdata->sort_type;

   path = ecore_file_realpath(cfdata->dir);
   cfdata->inst->conf->dir = eina_stringshare_add(path);
   cfdata->inst->conf->fm = eina_stringshare_add(cfdata->fm);
   E_FREE(path);

   if (inst->monitor)
     ecore_file_monitor_del(inst->monitor);
   inst->monitor = ecore_file_monitor_add(inst->conf->dir,
					  _dirwatcher_monitor_cb, inst);

   _dirwatcher_description_create(inst);

   ev = E_NEW(Drawer_Event_Source_Update, 1);
   ev->source = inst->source;
   ev->id = eina_stringshare_add(inst->conf->id);
   ecore_event_add(DRAWER_EVENT_SOURCE_UPDATE, ev, _dirwatcher_event_update_free, NULL);

   e_config_save_queue();
   return 1;
}

static int
_dirwatcher_cb_sort_dir(const Drawer_Source_Item *si1, const Drawer_Source_Item *si2)
{
   int d1, d2;

   d1 = ecore_file_is_dir(si1->file_path);
   d2 = ecore_file_is_dir(si2->file_path);

   if (d1 && d2)
     return strcmp(si1->file_path, si2->file_path);

   if (d1)
     return -1;
   if (d2)
     return 1;
   return 0;
}

static int
_dirwatcher_cb_sort(const void *data1, const void *data2)
{
   const Drawer_Source_Item *si1 = NULL, *si2 = NULL;
   Instance *inst;
   const char *name1;
   const char *name2;
   struct stat st1, st2;
   long long size1, size2;

   si1 = data1;
   si2 = data2;
   inst = ((Dirwatcher_Priv *) si1->priv)->inst;
   switch (inst->conf->sort_type)
     {
      case SORT_NAME:
	 if (inst->conf->sort_dir)
	   {
	      int ret = _dirwatcher_cb_sort_dir(si1, si2);
	      if (ret) return ret;
	   }

	 name1 = ecore_file_file_get(si1->file_path);
	 name2 = ecore_file_file_get(si2->file_path);
	 return strcmp(name1, name2);
      case SORT_MTIME:
	 if (inst->conf->sort_dir)
	   {
	      int ret = _dirwatcher_cb_sort_dir(si1, si2);
	      if (ret) return ret;
	   }

	 if (stat(si1->file_path, &st1) < 0) return 0;
	 if (stat(si2->file_path, &st2) < 0) return 0;
	 return st1.st_mtime - st2.st_mtime;
      case SORT_CTIME:
	 if (inst->conf->sort_dir)
	   {
	      int ret = _dirwatcher_cb_sort_dir(si1, si2);
	      if (ret) return ret;
	   }

	 if (stat(si1->file_path, &st1) < 0) return 0;
	 if (stat(si2->file_path, &st2) < 0) return 0;
	 return st1.st_ctime - st2.st_ctime;
      case SORT_ATIME:
	 if (inst->conf->sort_dir)
	   {
	      int ret = _dirwatcher_cb_sort_dir(si1, si2);
	      if (ret) return ret;
	   }

	 if (stat(si1->file_path, &st1) < 0) return 0;
	 if (stat(si2->file_path, &st2) < 0) return 0;
	 return st1.st_atime - st2.st_atime;
      case SORT_SIZE:
	 if (inst->conf->sort_dir)
	   {
	      int ret = _dirwatcher_cb_sort_dir(si1, si2);
	      if (ret) return ret;
	   }

	 size1 = ecore_file_size(si1->file_path);
	 size2 = ecore_file_size(si2->file_path);
	 return size1 - size2;
     }

   return 0;
}

