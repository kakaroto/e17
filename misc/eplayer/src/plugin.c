#include <config.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "utils.h"
#include "plugin.h"

static char *find_plugin(const char *dir, const char *name) {
	Evas_List *files, *l;
	char *ret = NULL, tmp[128];

	if (!(files = dir_get_files(dir)))
		return NULL;

	for (l = files; l; l = l->next) {
		/* get the plugin name from the filename */
		sscanf((char *) l->data, "lib%127[^.].so", tmp);

		if (!strcasecmp(name, tmp)) {
			ret = strdup(l->data);
			break;
		}
	}

	while (files) {
		free(files->data);
		files = evas_list_remove(files, files->data);
	}

	return ret;
}

/**
 * Creates a new plugin.
 *
 * @param name Name of the plugin.
 * @param type Type of the plugin.
 * @return The newly created plugin.
 */
void *plugin_new(const char *name, PluginType type) {
	InputPlugin *p;
	int (*init)(void *p), size;
	char *error, path[PATH_MAX + 1];
	char *dir, *right_name;

	if (!name || !*name)
		return NULL;

	if (type == PLUGIN_TYPE_INPUT) {
		size = sizeof(InputPlugin);
		dir = PLUGIN_DIR "/input";
	} else {
		size = sizeof(OutputPlugin);
		dir = PLUGIN_DIR "/output";
	}

	if (!(p = malloc(size)))
		return NULL;

	memset(p, 0, size);

	snprintf(path, sizeof(path), "%s/lib%s.so", dir, name);
	
	if (!(p->handle = dlopen(path, RTLD_LAZY))) {
		/* couldn't load plugin, let's try with a
		 * case-insensitive search
		 */
		if (!(right_name = find_plugin(dir, name))) {
			plugin_free(p);
			return NULL;
		}

		snprintf(path, sizeof(path), "%s/%s", dir, right_name);
		free(right_name);

		if (!(p->handle = dlopen(path, RTLD_LAZY))) {
			plugin_free(p);
			return NULL;
		}
	}

	/* get the address of the init function */
	init = dlsym(p->handle, "plugin_init");

	if ((error = dlerror())) {
		plugin_free(p);
		return NULL;
	}

	p->debug = debug;

	/* now call the init function */
	if (!(*init)(p)) {
		plugin_free(p);
		return NULL;
	}

	return p;
}

/**
 * Frees a plugin, calling the shutdown method if available.
 *
 * @param plugin
 */
void plugin_free(void *plugin) {
	InputPlugin *p = plugin;
	
	if (!plugin)
		return;

	if (p->shutdown)
		p->shutdown();

	if (p->handle)
		dlclose(p->handle);

	free(p);
}
