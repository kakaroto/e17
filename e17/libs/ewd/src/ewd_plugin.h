#ifndef __EWD_PLUGINS_H__
#define __EWD_PLUGINS_H__

typedef struct _ewd_plugin_group Ewd_Plugin_Group;
struct _ewd_plugin_group
{
	int id;
	char *name;
	Ewd_List *paths;
	Ewd_List *loaded_plugins;
};

typedef struct _ewd_plugin Ewd_Plugin;
struct _ewd_plugin
{
	int group;
	char *name;
	void *handle;
};

/*
 * Create a new plugin group
 */
int ewd_plugin_group_new(char *group_name);

/*
 * Destroy a previous plugin group
 */
void ewd_plugin_group_del(int group_id);

/*
 * Add a directory to be searched for plugins
 */
void ewd_plugin_path_add(int group_id, char *path);

/*
 * Remove a directory to be searched for plugins
 */
void ewd_plugin_path_del(int group_id, char *path);

/*
 * Load the specified plugin
 */
Ewd_Plugin *ewd_plugin_load(int group_id, char *plugin);

/*
 * Unload the specified plugin
 */
void ewd_plugin_unload(Ewd_Plugin * plugin);

/*
 * Lookup the specified symbol for the plugin
 */
void *ewd_plugin_call(Ewd_Plugin * plugin, char *symbol_name);

Ewd_List *ewd_plugin_get_available(int group_id);

#endif
