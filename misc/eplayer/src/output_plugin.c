#include <config.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "output_plugin.h"

OutputPlugin *output_plugin_new(const char *name) {
	OutputPlugin *op;
	int (*init)(OutputPlugin *op);
	char *error, path[PATH_MAX + 1];

	if (!name || !*name)
		return NULL;

	if (!(op = malloc(sizeof(OutputPlugin))))
		return NULL;

	memset(op, 0, sizeof(OutputPlugin));

	snprintf(path, sizeof(path), PLUGIN_DIR "/output/lib%s.so", name);

	if (!(op->handle = dlopen(path, RTLD_LAZY))) {
		output_plugin_free(op);
		return NULL;
	}

	/* get the address of the init function */
	init = dlsym(op->handle, "output_plugin_init");

	if ((error = dlerror())) {
		output_plugin_free(op);
		return NULL;
	}

	/* now call the init function */
	if (!(*init)(op)) {
		output_plugin_free(op);
		return NULL;
	}

	return op;
}

void output_plugin_free(OutputPlugin *op) {
	if (!op)
		return;

	if (op->handle)
		dlclose(op->handle);

	free(op);
}
