#ifndef __PLAYLIST_H
#define __PLAYLIST_H

#include <Evas.h>
#include <limits.h>
#include <pthread.h>
#include "plugin.h"

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

typedef void (*ItemAddCallback) (PlayListItem *pli, void *data);

typedef struct {
	int num; /* number of entries */
	int duration;
	Evas_List *items;
	Evas_List *cur_item;

	Evas_List *plugins; /* lists all available plugins */
} PlayList;

PlayList *playlist_new(Evas_List *plugins);
void playlist_free();

bool playlist_load_file(PlayList *pl, const char *file, bool append);
bool playlist_load_dir(PlayList *pl, const char *dir, bool append);
bool playlist_load_m3u(PlayList *pl, const char *file, bool append);
bool playlist_load_any(PlayList *pl, const char *path, bool append);

void playlist_remove_all(PlayList *pl);
void playlist_remove_item(PlayList *pl, PlayListItem *pli);

PlayListItem *playlist_current_item_get(PlayList *pl);
void playlist_current_item_set(PlayList *pl, PlayListItem *pli);

bool playlist_current_item_prev(PlayList *pl);
bool playlist_current_item_next(PlayList *pl);

bool playlist_current_item_has_prev(PlayList *pl);
bool playlist_current_item_has_next(PlayList *pl);

#endif
