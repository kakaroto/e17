/*
 * $Id$
 * vim:noexpandtab:sw=4:sts=4:ts=4
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <assert.h>
#include "playlist.h"
#include "utils.h"

/**
 * Creates a new PlayList object.
 *
 * @param evas
 * @param theme
 * @param xmmms
 * @return The newly created PlayList.
 */
PlayList *playlist_new(Evas *evas, const char *theme,
                       xmmsc_connection_t *xmms) {
	PlayList *pl;

	if (!(pl = calloc(1, sizeof(PlayList))))
		return NULL;

	pl->evas = evas;
	pl->container = evas_object_name_find(evas, "PlayList");
	pl->theme = theme;
	pl->xmms = xmms;

	return pl;
}

void playlist_container_set(PlayList *pl, Evas_Object *container) {
	Evas_List *l;

	assert(pl);

	for (l = pl->items; l; l = l->next)
		playlist_item_container_set(l->data, container);
}

/**
 * Removes all items from a PlayList.
 *
 * @param pl
 */
void playlist_remove_all(PlayList *pl) {
	assert(pl);

	while (pl->items) {
		playlist_item_free((PlayListItem *) pl->items->data);
		pl->items = evas_list_remove(pl->items, pl->items->data);
	}

	pl->duration = 0;
	pl->current_item = NULL;
}

/**
 * Removes the item from a PlayList.
 *
 * @param pl
 * @param pli
 */
void playlist_item_remove(PlayList *pl, PlayListItem *pli) {
	assert(pl);
	assert(pli);

	pl->duration -= playlist_item_duration_get(pli);
	pl->items = evas_list_remove(pl->items, pli);
	playlist_item_free(pli);

	if (pl->current_item == pli)
		pl->current_item = NULL;
}

/**
 * Frees a PlayList object.
 *
 * @param pl The PlayList to free.
 */
void playlist_free(PlayList *pl) {
	if (!pl)
		return;

	playlist_remove_all(pl);
	free(pl);
}

PlayListItem *playlist_item_find_by_id(PlayList *pl, unsigned int id)
{
	PlayListItem *pli;
	Evas_List *l;

	assert(pl);

	for (l = pl->items; l; l = l->next) {
		pli = l->data;

		if (pli->id == id)
			return pli;
	}

	return NULL;
}

PlayListItem *playlist_item_add(PlayList *pl, unsigned int id)
{
	PlayListItem *pli;

	assert(pl);

	if (!(pli = playlist_item_new(id, pl->evas, pl->container,
	                              pl->theme)))
		return NULL;

	pl->items = evas_list_append(pl->items, pli);

	return pli;
}

PlayListItem *playlist_set_current(PlayList *pl, unsigned int id)
{
	PlayListItem *pli = NULL;

	assert(pl);

	if ((pli = playlist_item_find_by_id (pl, id)))
		pl->current_item = pli;
	else
		debug(DEBUG_LEVEL_INFO, "%s() failed for id %d\n",
		      __FUNCTION__, id);

	return pli;
}

void playlist_load_file(PlayList *pl, const char *uri, bool append) {
	assert(pl);
	assert(uri);

	xmmsc_playlist_add(pl->xmms, (char *) uri);
}
