#ifndef __PLAYLIST_H
#define __PLAYLIST_H

/*
 * $Id$
 */

#include <Evas.h>
#include <xmms/xmmsclient.h>
#include "playlist_item.h"

typedef struct {
	Evas *evas;
	Evas_Object *container;
	const char *theme;
	xmmsc_connection_t *xmms;

	PlayListItem *current_item;

	unsigned int duration;
	Evas_List *items;
} PlayList;

PlayList *playlist_new(Evas *evas, const char *theme,
                       xmmsc_connection_t *xmms);
void playlist_free(PlayList *pl);

void playlist_container_set(PlayList *pl, Evas_Object *container);

bool playlist_load_file(PlayList *pl, const char *file, bool append);
bool playlist_load_dir(PlayList *pl, const char *dir, bool append);
bool playlist_load_any(PlayList *pl, const char *path, bool append);

void playlist_remove_all(PlayList *pl);

PlayListItem *playlist_item_add(PlayList *pl, unsigned int id);
PlayListItem *playlist_item_find_by_id(PlayList *pl, unsigned int id);
void playlist_item_remove(PlayList *pl, PlayListItem *pli);

PlayListItem *playlist_set_current(PlayList *pl, unsigned int id);

#endif
