#include <Ewd.h>

static Ewd_List *group_list = NULL;


Ewd_Plugin_Group *__ewd_plugin_group_find(char *name);
Ewd_Plugin_Group *__ewd_plugin_group_find_id(int id);

/**
 * ewd_plugin_group_new - create a new plugin group
 * @group_name: the name of the new group
 *
 * Returns 0 on error, the integer id of the new group on success.
 */
int
ewd_plugin_group_new(char *group_name)
{
	Ewd_Plugin_Group *group;

	CHECK_PARAM_POINTER_RETURN("group_name", group_name, -1);

	if (!group_list)
	  {
		  group_list = ewd_list_new();
	  }
	else
	  {
		  group = __ewd_plugin_group_find(group_name);
		  if (group)
			  return -1;
	  }

	group = (Ewd_Plugin_Group *) malloc(sizeof(Ewd_Plugin_Group));
	memset(group, 0, sizeof(Ewd_Plugin_Group));

	group->name = strdup(group_name);

	ewd_list_append(group_list, group);

	group->id = ewd_list_nodes(group_list);

	return group->id;
}

/**
 * ewd_plugin_group_del - destroy a previously created plugin group
 * @group_id: the unique identifier for the group
 *
 * Returns no value.
 */
void
ewd_plugin_group_del(int group_id)
{
	Ewd_Plugin_Group *group;

	group = __ewd_plugin_group_find_id(group_id);

	if (!group)
		return;

	ewd_list_for_each(group->loaded_plugins,
			  EWD_FOR_EACH(ewd_plugin_unload));
	ewd_list_destroy(group->loaded_plugins);

	ewd_list_for_each(group->paths,
			  EWD_FOR_EACH(free));
	ewd_list_destroy(group->paths);

	free(group->name);
	free(group);
}

/**
 * ewd_plugin_path_add - add a directory to be searched for plugins
 * @group_id: the unique identifier for the group to add the path
 * @path: the new path to be added to the group
 *
 * Returns no value.
 */
void
ewd_plugin_path_add(int group_id, char *path)
{
	Ewd_Plugin_Group *group;

	CHECK_PARAM_POINTER("path", path);

	group = __ewd_plugin_group_find_id(group_id);

	if (!group)
		return;

	if (!group->paths)
		group->paths = ewd_list_new();

	ewd_list_append(group->paths, strdup(path));
}

/*
 * Remove a directory to be searched for plugins
 */
void
ewd_plugin_path_del(int group_id, char *path)
{
	char *found;
	Ewd_Plugin_Group *group;

	CHECK_PARAM_POINTER("path", path);

	group = __ewd_plugin_group_find_id(group_id);

	if (!group || !group->paths)
		return;

	/*
	 * Find the path in the list of available paths
	 */
	ewd_list_goto_first(group->paths);

	while ((found = ewd_list_current(group->paths))
	       && strcmp(found, path))
		ewd_list_next(group->paths);

	/*
	 * If the path is found, remove and free it
	 */
	if (found)
	  {
		  ewd_list_remove(group->paths);
		  free(found);
	  }
}

/**
 * ewd_plugin_load - load the specified plugin from the specified group
 * @group_id: the group to search for the plugin to load
 * @plugin: the name of the plugin to load
 *
 * Returns a pointer to the newly loaded plugin on success, NULL on failure.
 */
Ewd_Plugin *
ewd_plugin_load(int group_id, char *plugin)
{
	Ewd_Plugin_Group *group;
	Ewd_Plugin *plugin2;
	void *handle = NULL;

	CHECK_PARAM_POINTER_RETURN("plugin", plugin, NULL);

	group = __ewd_plugin_group_find_id(group_id);
	
	if (!group || !group->paths)
		return NULL;

	ewd_list_goto_first(group->paths);

	  {
		char *p;
		char path[1024];
		struct stat st;

		ewd_list_goto_first(group->paths);

		while ((p = ewd_list_next(group->paths)) != NULL)
		  {
			snprintf(path, 1024, "%s/%s.so", p, plugin);

			stat(path, &st);

			if (S_ISREG(st.st_mode))
			  {
				handle = dlopen(path, RTLD_LAZY);
				break;
			  }
		  }
	  }

	plugin2 = malloc(sizeof(Ewd_Plugin));
	memset(plugin2, 0, sizeof(Ewd_Plugin));

	plugin2->group = group_id;
	plugin2->name = strdup(plugin);
	plugin2->handle = handle;

	if (!group->loaded_plugins)
		group->loaded_plugins = ewd_list_new();

	ewd_list_append(group->loaded_plugins, plugin2);

	return plugin2;
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
	Ewd_Plugin_Group *group;

	CHECK_PARAM_POINTER("plugin", plugin);

	if (!plugin->handle)
		return;

	group = __ewd_plugin_group_find_id(plugin->group);

	if (ewd_list_goto(group->loaded_plugins, plugin))
		ewd_list_remove(group->loaded_plugins);

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

Ewd_List *
ewd_plugin_get_available(int group_id)
{
	Ewd_List *avail = NULL;
	Ewd_Plugin_Group *group;
	char *path;

	group = __ewd_plugin_group_find_id(group_id);

	if (!group || !group->paths || ewd_list_is_empty(group->paths))
		return NULL;

	ewd_list_goto_first(group->paths);

	while ((path = ewd_list_next(group->paths)) != NULL)
	  {
		DIR *dir;
		struct stat st;
		struct dirent *d;

		stat(path, &st);

		if (!S_ISDIR(st.st_mode))
			continue;

		dir = opendir(path);

		if (!dir)
			continue;

		while ((d = readdir(dir)) != NULL)
		  {
			char ppath[1024];
			char *ext;
			char n[1024];
			int l;

			if (!strncmp(d->d_name, ".", 1))
				continue;

			ext = strrchr(d->d_name, '.');

			if (!ext || strncmp(ext, ".so", 3))
				continue;

			snprintf(ppath, 1024, "%s/%s", path, d->d_name);

			stat(ppath, &st);

			if (!S_ISREG(st.st_mode))
				continue;

			l = strlen(d->d_name);

			snprintf(n, l - 2, "%s", d->d_name);

			if (!avail)
				avail = ewd_list_new();

			ewd_list_append(avail, strdup(n));
		  }
	  }

	return avail;
}

/*
 * Find the specified group name
 */
Ewd_Plugin_Group *
__ewd_plugin_group_find(char *name)
{
	Ewd_Plugin_Group *group;

	CHECK_PARAM_POINTER_RETURN("name", name, NULL);

	ewd_list_goto_first(group_list);

	while ((group = ewd_list_next(group_list)) != NULL)
		if (!strcmp(group->name, name))
			return group;

	return NULL;
}

/*
 * Find the specified group id
 */
Ewd_Plugin_Group *
__ewd_plugin_group_find_id(int id)
{
	Ewd_Plugin_Group *group;

	ewd_list_goto_first(group_list);

	while ((group = ewd_list_next(group_list)) != NULL)
		if (group->id == id)
			return group;

	return NULL;
}
