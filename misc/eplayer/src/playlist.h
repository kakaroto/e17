#ifndef __PLAYLIST_H
#define __PLAYLIST_H

#include <Evas.h>
#include <limits.h>
#include <pthread.h>
#include "plugin.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	char file[PATH_MAX + 1];

	char comment[COMMENT_ID_NUM][MAX_COMMENT_LEN];

	int duration;
	int channels; /* number of channels */
	long sample_rate; /* sample rate */

	int current_pos;
	pthread_mutex_t pos_mutex;

	InputPlugin *plugin; /* plugin that's used for this item */
} PlayListItem;

typedef struct {
	int num; /* number of entries */
	Evas_List *items;
	Evas_List *cur_item;

	Evas_List *plugins; /* lists all available plugins */
} PlayList;

PlayList *playlist_new(Evas_List *plugins);
void playlist_free();

int playlist_load_file(PlayList *pl, const char *file, int append);
int playlist_load_dir(PlayList *pl, const char *dir, int append);
int playlist_load_m3u(PlayList *pl, const char *file, int append);
int playlist_load_any(PlayList *pl, const char *path, int append);

void playlist_remove_all(PlayList *pl);

#ifdef __cplusplus
}
#endif

#endif
