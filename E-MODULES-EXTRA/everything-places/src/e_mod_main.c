/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

#include "e.h"
#include "e_mod_main.h"
#include "evry_api.h"

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
static Evry_Plugin *_plug = NULL;
static const char _module_icon[] = "find";
static const char *_mime_dir;

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

/* taken from fileman module */
static void
_parse_gtk_bookmarks(Plugin *p)
{
   Evry_Item_File *file;
   char line[PATH_MAX];
   char buf[PATH_MAX];
   E_Menu_Item *mi;
   Efreet_Uri *uri;
   char *alias;
   FILE* fp;

   snprintf(buf, sizeof(buf), "%s/.gtk-bookmarks", e_user_homedir_get());
   fp = fopen(buf, "r");
   if (!fp) return;

   while(fgets(line, sizeof(line), fp))
     {
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

	file = EVRY_ITEM_NEW(Evry_Item_File, p,
			     ecore_file_file_get(uri->path),
			     NULL, _item_free);
	printf("%s\n", uri->path);

	file->path = eina_stringshare_add(uri->path);
	file->mime = eina_stringshare_ref(_mime_dir);
	EVRY_ITEM(file)->id = eina_stringshare_ref(file->path);
	EVRY_ITEM(file)->context = eina_stringshare_ref(file->mime);
	EVRY_ITEM(file)->browseable = EINA_TRUE;
	p->files = eina_list_append(p->files, file);

	efreet_uri_free(uri);
     }
   fclose(fp);

}

static Evry_Plugin *
_begin(Evry_Plugin *plugin, const Evry_Item *item)
{
   Evry_Item_File *file;
   char path[PATH_MAX];

   GET_PLUGIN(p, plugin);

   e_user_dir_concat_static(path, "backgrounds");
   file = EVRY_ITEM_NEW(Evry_Item_File, p, N_("Wallpaper"), NULL, _item_free);
   file->path = eina_stringshare_add(path);
   file->mime = eina_stringshare_ref(_mime_dir);
   EVRY_ITEM(file)->browseable = EINA_TRUE;
   EVRY_ITEM(file)->id = eina_stringshare_ref(file->path);
   EVRY_ITEM(file)->context = eina_stringshare_ref(file->mime);
   p->files = eina_list_append(p->files, file);

   _parse_gtk_bookmarks(p);

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
}


static int
_fetch(Evry_Plugin *plugin, const char *input)
{
   int len = (input ? strlen(input) : 0);

   GET_PLUGIN(p, plugin);

   EVRY_PLUGIN_ITEMS_CLEAR(p);

   if (len >= plugin->config->min_query)
     EVRY_PLUGIN_ITEMS_ADD(p, p->files, input, 0, 0);

   return !!(p->base.items);
}

static int
_plugins_init(const Evry_API *api)
{
   Evry_Plugin *p;

   if (evry_module->active)
     return EINA_TRUE;

   evry = api;

   if (!evry->api_version_check(EVRY_API_VERSION))
     return EINA_FALSE;

   _mime_dir = eina_stringshare_add("inode/directory");

   _plug = EVRY_PLUGIN_NEW(Plugin, N_("Places"), NULL, EVRY_TYPE_FILE,
			   _begin, _finish, _fetch, NULL);

   if (evry->plugin_register(_plug, EVRY_PLUGIN_SUBJECT, 3))
     {
	/* p->config->top_level = EINA_FALSE;
	 * p->config->min_query = 3; */
     }

   return EINA_TRUE;
}

static void
_plugins_shutdown(void)
{
   Plugin *p;

   if (!evry_module->active) return;

   eina_stringshare_del(_mime_dir);

   EVRY_PLUGIN_FREE(_plug);

   evry_module->active = EINA_FALSE;
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

   evry_module = E_NEW(Evry_Module, 1);
   evry_module->init     = &_plugins_init;
   evry_module->shutdown = &_plugins_shutdown;
   EVRY_MODULE_REGISTER(evry_module);

   if ((evry = e_datastore_get("everything_loaded")))
     evry_module->active = _plugins_init(evry);

   e_module_delayed_set(m, 1);

   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   _plugins_shutdown();

   EVRY_MODULE_UNREGISTER(evry_module);
   E_FREE(evry_module);

   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   return 1;
}

/***************************************************************************/
