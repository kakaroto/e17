#ifndef __OUTPUT_PLUGIN_H
#define __OUTPUT_PLUGIN_H

typedef struct {
	void *handle;

	/* playback */
	int (*configure)(int channels, int rate, int bits, int bigendian);
	int (*play)(unsigned char *data, int len);

	/* mixer */
	int (*volume_get)(int *left, int *right);
	int (*volume_set)(int left, int right);

	void (*shutdown)();
} OutputPlugin;

OutputPlugin *output_plugin_new(const char *name);
void output_plugin_free(OutputPlugin *op);

#endif
