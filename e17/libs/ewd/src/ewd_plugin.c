#include <Ewd.h>

static Ewd_List *loaded_plugins = NULL;

/**
 * ewd_plugin_load - load the specified plugin from the specified path group
 * @group_id: the path group to search for the plugin to load
 * @plugin_name: the name of the plugin to load
 *
 * Returns a pointer to the newly loaded plugin on success, NULL on failure.
 */
Ewd_Plugin *
ewd_plugin_load(int group_id, char *plugin_name)
{
	char *path;
	char temp[PATH_MAX];

	Ewd_Plugin *plugin;
	void *handle = NULL;

	CHECK_PARAM_POINTER_RETURN("plugin_name", plugin_name, NULL);

	snprintf(temp, PATH_MAX, "%s.so", plugin_name);
	path = ewd_path_group_find(group_id, temp);
	if (!path)
		return NULL;

	handle = dlopen(path, RTLD_LAZY);
	if (!handle)
		return NULL;

	/*
	 * Allocate the new plugin and initialize it's fields
	 */
	plugin = malloc(sizeof(Ewd_Plugin));
	memset(plugin, 0, sizeof(Ewd_Plugin));

	plugin->group = group_id;
	plugin->name = strdup(plugin_name);
	plugin->handle = handle;

	/*
	 * Now add it to the list of the groups loaded plugins
	 */
	if (!loaded_plugins)
		loaded_plugins = ewd_list_new();

	ewd_list_append(loaded_plugins, plugin);

	return plugin;
}

/**
 * ewd_plugin_unload - unload the specified plugin
 * @plugin: the plugin to unload from memory
 *
 * Returns no value.
 */
void
ewd_plugin_unload(Ewd_Plugin * plugin)
{
	CHECK_PARAM_POINTER("plugin", plugin);

	if (!plugin->handle)
		return;

	if (ewd_list_goto(loaded_plugins, plugin))
		ewd_list_remove(loaded_plugins);

	dlclose(plugin->handle);

	FREE(plugin->name);
	FREE(plugin);
}

/*
 * Lookup the specified symbol for the plugin
 */
void *
ewd_plugin_call(Ewd_Plugin * plugin, char *symbol_name)
{
	void *ret;

	CHECK_PARAM_POINTER_RETURN("plugin", plugin, NULL);
	CHECK_PARAM_POINTER_RETURN("symbol_name", symbol_name, NULL);

	if (!plugin->handle)
		return NULL;

	ret = dlsym(plugin->handle, symbol_name);

	return ret;
}
