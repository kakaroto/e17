/*
 * $Id$
 * vim:noexpandtab:sw=4:sts=4:ts=4
 */

#include <config.h>
#include <Edje.h>
#include <Esmart/container.h>
#include <stdio.h>
#include <assert.h>
#include "callbacks.h"
#include "playlist_item.h"
#include "utils.h"

const char *playlist_item_artist_get(PlayListItem *pli) {
	return x_hash_lookup(pli->properties, "artist");
}

const char *playlist_item_title_get(PlayListItem *pli) {
	return x_hash_lookup(pli->properties, "title");
}

const char *playlist_item_album_get(PlayListItem *pli) {
	return x_hash_lookup(pli->properties, "album");
}

unsigned int playlist_item_duration_get(PlayListItem *pli) {
	void *tmp;

	if ((tmp = x_hash_lookup(pli->properties, "duration")))
		return atoi(tmp) / 1000;
	else
		return 0;
}

/**
 * Frees a PlayListItem object.
 *
 * @param pli
 */
void playlist_item_free(PlayListItem *pli) {
	assert(pli);

	if (pli->container && pli->edje)
		e_container_element_destroy(pli->container, pli->edje);

	free(pli);
}

void playlist_item_container_set(PlayListItem *pli,
                                 Evas_Object *container) {
	assert(pli);

	if (pli->container == container)
		return;

	pli->container = container;

	playlist_item_show(pli);
}

static void set_parts_text(PlayListItem *pli) {
	char len[32];

	assert(pli);
	assert(pli->edje),

	snprintf(len, sizeof(len), "%i:%02i",
			 playlist_item_duration_get(pli) / 60,
	         playlist_item_duration_get(pli) % 60);
	edje_object_part_text_set(pli->edje, "length", len);

	edje_object_part_text_set(pli->edje, "title",
	                          playlist_item_title_get(pli));
}

void playlist_item_properties_set(PlayListItem *pli, x_hash_t *p) {
	assert(pli);

	if (pli->properties == p)
		return;

	pli->properties = p;

	if (pli->edje)
		set_parts_text(pli);
	else
		playlist_item_show(pli);
}

bool playlist_item_show(PlayListItem *pli) {
	double w = 0, h = 0;
	void *udata;

	assert(pli);

	if (!pli->container)
		return true;

	/* add the item to the container */
	if (!(pli->edje = edje_object_add(pli->evas)))
		return false;

	if (!edje_object_file_set(pli->edje, find_theme(pli->theme),
	                          "playlist_item"))
		return false;

	set_parts_text(pli);

	/* set parts dimensions */
	edje_object_size_min_get(pli->edje, &w, &h);
	evas_object_resize(pli->edje, w, h);

	evas_object_data_set(pli->edje, "PlayListItem", pli);

	/* store font size, we need it later for scrolling
	 * FIXME: we're assuming that the objects minimal height
	 * equals the text size
	 */
	evas_object_data_set(pli->container, "PlaylistFontSize",
	                     (void *) (int) h);

	udata = evas_object_data_get(pli->container, "Euphoria");

	/* add playlist item callbacks */
	edje_object_signal_callback_add(pli->edje, "PLAYLIST_SCROLL_UP", "",
			                        (EdjeCb) on_edje_playlist_scroll_up,
	                                udata);
	edje_object_signal_callback_add(pli->edje, "PLAYLIST_SCROLL_DOWN", "",
			                        (EdjeCb) on_edje_playlist_scroll_down,
	                                udata);
	edje_object_signal_callback_add(pli->edje, "PLAYLIST_ITEM_PLAY", "",
			                        (EdjeCb) on_edje_playlist_item_play,
	                                udata);
	edje_object_signal_callback_add(pli->edje, "PLAYLIST_ITEM_SELECTED", "",
	                                (EdjeCb) on_edje_playlist_item_selected,
	                                udata);
	edje_object_signal_callback_add(pli->edje, "PLAYLIST_ITEM_REMOVE", "",
	                                (EdjeCb) on_edje_playlist_item_remove,
	                                udata);

	e_container_element_append(pli->container, pli->edje);

	return true;
}

/**
 * Creates a new PlayListItem object.
 *
 * @return The new PlayListItem object.
 */
PlayListItem *playlist_item_new(unsigned int id, Evas *evas,
                                Evas_Object *container,
                                const char *theme) {
	PlayListItem *pli;

	assert(evas);
	assert(theme);

	if (!(pli = calloc(1, sizeof(PlayListItem))))
		return NULL;

	pli->id = id;
	pli->evas = evas;
	pli->container = container;
	pli->theme = theme;

	return pli;
}

