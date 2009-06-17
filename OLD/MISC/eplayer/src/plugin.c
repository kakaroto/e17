/*
 * $Id$
 */

#include <config.h>
#include <stdlib.h>
#include <ltdl.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "utils.h"
#include "plugin.h"

/**
 * Creates a new plugin.
 *
 * @param name Name of the plugin.
 * @param type Type of the plugin.
 * @return The newly created plugin.
 */
void *plugin_new(const char *file, PluginType type) {
	InputPlugin *p;
	int (*init)(void *p), size;

	if (!file || !*file)
		return NULL;

	if (type == PLUGIN_TYPE_INPUT)
		size = sizeof(InputPlugin);
	else
		size = sizeof(OutputPlugin);

	if (!(p = malloc(size)))
		return NULL;

	memset(p, 0, size);

	if (!(p->handle = lt_dlopenext(file))) {
		fprintf(stderr, "Cannot load plugin '%s': %s\n",
		        file, lt_dlerror());
		plugin_free(p);
		return NULL;
	}

	/* get the address of the init function */
	if (!(init = lt_dlsym(p->handle, "plugin_init"))) {
		fprintf(stderr, "Cannot load plugin '%s':"
		        "cannot find init method!\n", file);
		plugin_free(p);
		return NULL;
	}

	p->debug = debug;

	/* now call the init function */
	if (!(*init)(p)) {
		fprintf(stderr, "Cannot load plugin '%s':"
		        "init method failed!\n", file);
		plugin_free(p);
		return NULL;
	}

	if (!p->name) {
		fprintf(stderr, "Cannot load plugin '%s':"
		        "plugin name not set!\n", file);
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

	if (p->name)
		free(p->name);

	if (p->handle)
		lt_dlclose(p->handle);

	free(p);
}
