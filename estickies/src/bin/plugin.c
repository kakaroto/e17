#include "stickies.h"

static Ecore_Path_Group *_e_plugin_path = NULL;
static Ecore_Hash *_e_plugin_loaded;

static int _e_plugin_unload_from_ptr(Ecore_Plugin *plugin);
int _e_plugin_unload(const char *plugin_name);

/* initialize the plugin subsystem */
int _e_plugin_init()
{
   char *home;
   char path[PATH_MAX];
      
   home = getenv("HOME");
   if (!home)
     return 0;
   
   _e_plugin_path = ecore_path_group_new();
   
   snprintf(path, sizeof(path), "%s/.e/estickies/plugins", home);
   ecore_file_mkpath(path);
   ecore_path_group_add(_e_plugin_path, path);
   
   snprintf(path, sizeof(path), PACKAGE_DATA_DIR"/plugins");
   ecore_path_group_add(_e_plugin_path, path);

   _e_plugin_loaded = ecore_hash_new(ecore_str_hash, ecore_str_compare);
   
   return 1;
}

/* shutdown the plugin subsystem */
int _e_plugin_shutdown()
{
   Ecore_List *keys;
   char *key = NULL;
   
   keys = ecore_hash_keys(_e_plugin_loaded);
   ecore_list_first(keys);
   key = ecore_list_current(keys);
   while (key)
     {
	key = ecore_list_next(keys);
	_e_plugin_unload(key);
     }
   return 1;
}

/* load a plugin and run its on_load() function if available */
int _e_plugin_load(const char *plugin_name)
{
   Ecore_Plugin *plugin = NULL;
   int *(*on_load)(void);
   
   if (!plugin_name)
     return 0;
   
   if (ecore_hash_get(_e_plugin_loaded, plugin_name))
     return 0;
   
   if((plugin = ecore_plugin_load(_e_plugin_path, plugin_name, NULL)) == NULL)
     return 0;

   ecore_hash_set(_e_plugin_loaded, plugin_name, plugin);
   
   if ((on_load = ecore_plugin_symbol_get(plugin, "plugin_on_load")))
     {
	on_load();
     }
   
   return 1;
}

/* unload a plugin and call its on_unload() function */
int _e_plugin_unload(const char *plugin_name)
{
   Ecore_Plugin *plugin = NULL;
   int *(*on_unload)(void);
   
   if (!plugin_name || !(plugin = ecore_hash_get(_e_plugin_loaded, plugin_name)))
     return 0;
   
   if ((on_unload = ecore_plugin_symbol_get(plugin, "plugin_on_unload")))
     {
	on_unload();
     }
   
   ecore_plugin_unload(plugin);
   
   return 1;
}

/* subsystem internal functions */

/* unload a plugin from the Ecore_Plugin pointer */
static int _e_plugin_unload_from_ptr(Ecore_Plugin *plugin)
{
   int *(*on_unload)(void);

   if ((on_unload = ecore_plugin_symbol_get(plugin, "plugin_on_unload")))
     {
	on_unload();
     }
   
   ecore_plugin_unload(plugin);
   
   return 1;
}
