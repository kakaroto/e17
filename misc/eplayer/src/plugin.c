#include <config.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "plugin.h"

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
	char *dir;

	if (!name || !*name)
		return NULL;

	if (type == PLUGIN_TYPE_INPUT) {
		size = sizeof(InputPlugin);
		dir = "input";
	} else {
		size = sizeof(OutputPlugin);
		dir = "output";
	}

	if (!(p = malloc(size)))
		return NULL;

	memset(p, 0, size);

	snprintf(path, sizeof(path), PLUGIN_DIR "/%s/lib%s.so", dir, name);

	if (!(p->handle = dlopen(path, RTLD_LAZY))) {
		plugin_free(p);
		return NULL;
	}

	/* get the address of the init function */
	init = dlsym(p->handle, "plugin_init");

	if ((error = dlerror())) {
		plugin_free(p);
		return NULL;
	}

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
