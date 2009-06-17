#ifndef __PLUGIN_H
#define __PLUGIN_H

/*
 * $Id$
 */

#include "utils.h"

#define MAX_COMMENT_LEN 256

typedef enum {
	PLUGIN_TYPE_INPUT,
	PLUGIN_TYPE_OUTPUT,
	PLUGIN_TYPE_NUM
} PluginType;

typedef enum {
	COMMENT_ID_ARTIST,
	COMMENT_ID_TITLE,
	COMMENT_ID_ALBUM,
	COMMENT_ID_NUM
} CommentID;

typedef struct {
	void *handle;
	char *name;

	void (*debug)(DebugLevel level, const char *fmt, ...);

	void (*shutdown)();

	int (*open)(const char *file);
	void (*close)();

	char *(*get_comment)(CommentID id);
	int (*get_duration)();
	int (*get_channels)();
	int (*get_sample_rate)();
	int (*get_bitrate)();
	
	int (*read)(unsigned char **buf);

	int (*get_current_pos)();
	void (*set_current_pos)(int pos);
} InputPlugin;

typedef struct {
	void *handle;
	char *name;
	
	void (*debug)(DebugLevel level, const char *fmt, ...);

	void (*shutdown)();

	/* playback */
	int (*configure)(int channels, int rate, int bits);
	int (*play)(unsigned char *data, int len);

	/* mixer */
	int (*volume_get)(int *left, int *right);
	int (*volume_set)(int left, int right);
} OutputPlugin;

void *plugin_new(const char *file, PluginType type);
void plugin_free(void *p);

#endif
