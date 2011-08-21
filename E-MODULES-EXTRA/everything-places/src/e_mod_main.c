#include "e.h"
#include "e_fm_device.h"
#include "e_mod_main.h"
#include "evry_api.h"

#define ACT_MOUNT   0
#define ACT_UNMOUNT 1
/* #undef DBG
 * #define DBG(...) ERR(__VA_ARGS__) */

typedef struct _Plugin Plugin;

struct _Plugin
{
  Evry_Plugin base;
  const char *input;

  Eina_List *files;
};

static const Evry_API *evry = NULL;
static Evry_Module *evry_module = NULL;
static Eina_List *_plugins = NULL;
static const char _module_icon[] = "find";
static const char *_mime_dir;
static const char *_mime_mount;
static Evry_Action *act_mount;
static Evry_Action *act_umount;

/***************************************************************************/

static void
_item_free(Evry_Item *it)
{
   GET_FILE(file, it);

   IF_RELEASE(file->url);
   IF_RELEASE(file->path);
   IF_RELEASE(file->mime);

   E_FREE(file);
}

static Evry_Item *
_item_add(Plugin *p, const char *label, const char *path, const char *mime, const char *icon)
{
   Evry_Item_File *file;

   file = EVRY_ITEM_NEW(Evry_Item_File, p, label, NULL, _item_free);

   if (!path) path = "";
   file->path = eina_stringshare_add(path);
   file->mime = eina_stringshare_ref(mime);
   if (icon) EVRY_ITEM_ICON_SET(file, icon);
   EVRY_ITEM(file)->id = eina_stringshare_ref(file->path);
   EVRY_ITEM(file)->context = eina_stringshare_ref(file->mime);
   EVRY_ITEM(file)->browseable = EINA_TRUE;
   p->files = eina_list_append(p->files, file);

   return EVRY_ITEM(file);
}

/* taken from fileman module */
static void
_gtk_bookmarks_add(Plugin *p)
{
   char line[PATH_MAX];
   char buf[PATH_MAX];
   Efreet_Uri *uri;
   char *alias;
   FILE* fp;

   snprintf(buf, sizeof(buf), "%s/.gtk-bookmarks", e_user_homedir_get());
   fp = fopen(buf, "r");
   if (!fp) return;

   while(fgets(line, sizeof(line), fp))
     {
	if (strncmp(line, "file://", 7))
	  continue;
	
	alias = NULL;
	line[strlen(line) - 1] = '\0';
	alias = strchr(line, ' ');
	if (alias)
	  {
	     line[alias-line] =  '\0';
	     alias++;
	  }
	uri = efreet_uri_decode(line);
	if (!(uri) || !(uri->path))
	  {
	     if (uri) efreet_uri_free(uri);
	     continue;
	  }

	if (!ecore_file_exists(uri->path))
	  {
	     efreet_uri_free(uri);
	     continue;
	  }

	_item_add(p, ecore_file_file_get(uri->path), uri->path, _mime_dir, NULL);

	efreet_uri_free(uri);
     }
   fclose(fp);

}

static void
_volume_list_add(Plugin *p)
{
   const Eina_List *l;
   Evry_Item *it;
   E_Volume *vol;
   EINA_LIST_FOREACH(e_fm2_device_volume_list_get(), l, vol)
     {
	if (vol->mount_point && !strcmp(vol->mount_point, "/"))
	  continue;

	it = _item_add(p, vol->label, vol->mount_point, _mime_mount, vol->icon);
	if (!it) continue;

	it->browseable = vol->mounted;
	it->data = vol;
     }
}

static Evry_Plugin *
_begin(Evry_Plugin *plugin, const Evry_Item *item)
{
   Plugin *p;
   char path[PATH_MAX];

   EVRY_PLUGIN_INSTANCE(p, plugin);

   _item_add(p, N_("Home"), e_user_homedir_get(), _mime_dir, NULL);

   e_user_dir_concat_static(path, "backgrounds");
   _item_add(p, N_("Wallpaper"), path, _mime_dir, NULL);

   _gtk_bookmarks_add(p);

   _volume_list_add(p);

   return EVRY_PLUGIN(p);
}

static void
_finish(Evry_Plugin *plugin)
{
   Evry_Item_File *file;

   GET_PLUGIN(p, plugin);

   EVRY_PLUGIN_ITEMS_CLEAR(p);

   EINA_LIST_FREE(p->files, file)
     EVRY_ITEM_FREE(file);

   IF_RELEASE(p->input);

   E_FREE(p);
}


static int
_fetch(Evry_Plugin *plugin, const char *input)
{
   int len = (input ? strlen(input) : 0);

   GET_PLUGIN(p, plugin);

   EVRY_PLUGIN_ITEMS_CLEAR(p);

   if (len >= plugin->config->min_query)
     EVRY_PLUGIN_ITEMS_ADD(p, p->files, input, 0, 0);

   return EVRY_PLUGIN_HAS_ITEMS(p);
}

static int
_check_mount(Evry_Action *act __UNUSED__, const Evry_Item *it)
{
   GET_FILE(file, it);
   if (file->mime == _mime_mount)
     {
	E_Volume *vol = it->data;
	if (!vol) return 0;

	if (!vol->mounted && (EVRY_ITEM_DATA_INT_GET(act) == ACT_MOUNT))
	  return 1;

	if (vol->mounted && (EVRY_ITEM_DATA_INT_GET(act) == ACT_UNMOUNT))
	  return 1;
     }

   return 0;
}

static void
_cb_mount_ok(void *data)
{
   E_Volume *vol;

   GET_ITEM(it, data);

   vol = it->data;
   it->browseable = vol->mounted;
   evry->item_changed(it, 1, 0);

   EVRY_ITEM_FREE(it);
}

static void
_cb_mount_fail(void *data)
{
   GET_ITEM(it, data);
   EVRY_ITEM_FREE(it);
}

static int
_act_mount(Evry_Action *act)
{
   GET_ITEM(it, act->it1.item);

   E_Volume *vol = it->data;
   E_Fm2_Mount *mount;

   if (!vol)
     return 0;

   if (!vol->mounted && EVRY_ITEM_DATA_INT_GET(act) == ACT_MOUNT)
     {
	EVRY_ITEM_REF(it);
	mount = e_fm2_device_mount(vol, _cb_mount_ok, _cb_mount_fail,
			 _cb_mount_ok, _cb_mount_fail, it);
	/* XXX needs mount to be freed ? */
     }
   else if (vol->mounted && EVRY_ITEM_DATA_INT_GET(act) == ACT_UNMOUNT)
     {
   	/* if (!(mount = eina_list_data_get(vol->mounts))) */
	if (!(mount = e_fm2_device_mount_find(vol->mount_point)))
   	  {
   	     ERR("unmount: no mount point: %s", vol->mount_point);
   	     return 0;
   	  }

   	EVRY_ITEM_REF(it);
   	e_fm2_device_unmount(mount);
     }
   return 0;
}

static int
_plugins_init(const Evry_API *api)
{
   Evry_Plugin *p;

   evry = api;

   if (!evry->api_version_check(EVRY_API_VERSION))
     return EINA_FALSE;

   _mime_dir = eina_stringshare_add("inode/directory");
   _mime_mount = eina_stringshare_add("inode/mount-point");

   p = EVRY_PLUGIN_BASE("Places", "drive-harddisk", EVRY_TYPE_FILE,
			_begin, _finish, _fetch);
   _plugins = eina_list_append(_plugins, p);

   if (evry->plugin_register(p, EVRY_PLUGIN_SUBJECT, 3))
     {
	/* p->config->top_level = EINA_FALSE;
	 * p->config->min_query = 3; */
     }

   p = EVRY_PLUGIN_BASE("Places", "drive-harddisk", EVRY_TYPE_FILE,
		       _begin, _finish, _fetch);
   _plugins = eina_list_append(_plugins, p);

   if (evry->plugin_register(p, EVRY_PLUGIN_OBJECT, 1))
     {
	/* p->config->top_level = EINA_FALSE;
	 * p->config->min_query = 3; */
     }

   act_mount = EVRY_ACTION_NEW("Mount Drive", EVRY_TYPE_FILE, 0,
			       "drive-harddisk", _act_mount, _check_mount);
   EVRY_ITEM_DATA_INT_SET(act_mount, ACT_MOUNT);
   evry->action_register(act_mount, 1);

   act_umount = EVRY_ACTION_NEW("Unmount Drive", EVRY_TYPE_FILE, 0,
   				"hdd_unmount", _act_mount, _check_mount);
   act_mount->remember_context = EINA_TRUE;
   EVRY_ITEM_DATA_INT_SET(act_umount, ACT_UNMOUNT);
   evry->action_register(act_umount, 2);

   return EINA_TRUE;
}

static void
_plugins_shutdown(void)
{
   Plugin *p;

   eina_stringshare_del(_mime_dir);
   eina_stringshare_del(_mime_mount);

   EINA_LIST_FREE(_plugins, p)
     EVRY_PLUGIN_FREE(p);

   EVRY_ACTION_FREE(act_mount);
   EVRY_ACTION_FREE(act_umount);
}

/***************************************************************************/

EAPI E_Module_Api e_modapi =
  {
    E_MODULE_API_VERSION,
    PACKAGE
  };

EAPI void *
e_modapi_init(E_Module *m)
{
   char buf[4096];

   /* Location of message catalogs for localization */
   snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   EVRY_MODULE_NEW(evry_module, evry, _plugins_init, _plugins_shutdown);

   e_module_delayed_set(m, 1);

   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   EVRY_MODULE_FREE(evry_module);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   return 1;
}

/***************************************************************************/
