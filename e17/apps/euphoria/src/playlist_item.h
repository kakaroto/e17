#ifndef __PLAYLIST_ITEM_H
#define __PLAYLIST_ITEM_H

/*
 * $Id$
 * vim:noexpandtab:sw=4:sts=4:ts=4
 */

#include <Evas.h>
#include <limits.h>

typedef struct {
	Evas *evas;
	Evas_Object *edje;
	Evas_Object *container;
	const char *theme;

	unsigned int id;
	x_hash_t *properties;
} PlayListItem;

PlayListItem *playlist_item_new(unsigned int id, Evas *evas,
                                Evas_Object *container,
                                const char *theme);
void playlist_item_free(PlayListItem *pli);

bool playlist_item_show(PlayListItem *pli);
void playlist_item_container_set(PlayListItem *pli,
                                 Evas_Object *container);
void playlist_item_properties_set(PlayListItem *pli, x_hash_t *p);

const char *playlist_item_artist_get(PlayListItem *pli);
const char *playlist_item_title_get(PlayListItem *pli);
const char *playlist_item_album_get(PlayListItem *pli);
unsigned int playlist_item_duration_get(PlayListItem *pli);
unsigned int playlist_item_samplerate_get(PlayListItem *pli);
unsigned int playlist_item_bitrate_get(PlayListItem *pli);

#endif
