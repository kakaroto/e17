#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include "output_plugin.h"

OutputPlugin *output_plugin_new(const char *file) {
	OutputPlugin *op;
	int (*init)(OutputPlugin *op);
	char *error;

	if (!file || !*file)
		return NULL;

	if (!(op = malloc(sizeof(OutputPlugin))))
		return NULL;

	memset(op, 0, sizeof(OutputPlugin));

	if (!(op->handle = dlopen(file, RTLD_LAZY))) {
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
