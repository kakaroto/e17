#ifndef __PLAYLIST_ITEM_H
#define __PLAYLIST_ITEM_H

/*
 * $Id$
 */

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
	int bitrate;

	int current_pos;
	pthread_mutex_t pos_mutex;

	InputPlugin *plugin; /* plugin that's used for this item */

	Evas *evas;
	Evas_Object *edje;
	Evas_Object *container;
	const char *theme;
} PlayListItem;

PlayListItem *playlist_item_new(const char *file, Evas *evas,
                                Eina_List *plugins,
                                Evas_Object *container,
                                const char *theme);
void playlist_item_free(PlayListItem *pli);

bool playlist_item_show(PlayListItem *pli);
void playlist_item_container_set(PlayListItem *pli,
                                 Evas_Object *container);

#endif
