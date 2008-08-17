/*
 * $Id$
 */

#include <config.h>
#include <Edje.h>
#include <Esmart/Esmart_Container.h>
#include <stdio.h>
#include <assert.h>
#include "callbacks.h"
#include "playlist_item.h"

/**
 * Fills a PlayListItem's comments/info fields.
 *
 * @param pli The PlayListItem to store the comments/info stuff in.
 */
static void playlist_item_get_info(PlayListItem *pli) {
	int i;

	pli->sample_rate = pli->plugin->get_sample_rate();
	pli->channels = pli->plugin->get_channels();
	pli->duration = pli->plugin->get_duration();
	pli->sample_rate = pli->plugin->get_sample_rate();
	pli->bitrate = pli->plugin->get_bitrate();

	for (i = 0; i < COMMENT_ID_NUM; i++)
		snprintf(pli->comment[i], MAX_COMMENT_LEN, "%s",
		         pli->plugin->get_comment(i));
}

/**
 * Frees a PlayListItem object.
 *
 * @param pli
 */
void playlist_item_free(PlayListItem *pli) {
	if (!pli)
		return;

	pthread_mutex_destroy(&pli->pos_mutex);

	if (pli->container)
		esmart_container_element_remove(pli->container, pli->edje);
	
	if (pli->edje)
		evas_object_del(pli->edje);

	free(pli);
}

static InputPlugin *find_plugin(const char *file, Evas_List *plugins) {
	InputPlugin *ip;
	Evas_List *l;

	assert(file);

	for (l = plugins; l; l = l->next) {
		ip = l->data;

		if (ip->open(file))
			return ip;
	}

	return NULL;
}

void playlist_item_container_set(PlayListItem *pli,
                                 Evas_Object *container) {
	assert(pli);

	pli->container = container;

	playlist_item_show(pli);
}

bool playlist_item_show(PlayListItem *pli) {
	char len[32];
	double w = 0, h = 0;
	void *udata;

	assert(pli);
	assert(pli->container);

	/* add the item to the container */
	if (!(pli->edje = edje_object_add(pli->evas)))
		return false;

	if (!edje_object_file_set(pli->edje, find_theme(pli->theme),
	                          "playlist_item"))
		return false;

	/* set parts text */
	snprintf(len, sizeof(len), "%i:%02i", pli->duration / 60,
	         pli->duration % 60);
	edje_object_part_text_set(pli->edje, "length", len);
	
	/* If there is no title, use the file name */
	if (strcmp(pli->comment[COMMENT_ID_TITLE], ""))
		edje_object_part_text_set(pli->edje, "title",
		                          pli->comment[COMMENT_ID_TITLE]);
	else
		edje_object_part_text_set(pli->edje, "title", pli->file);

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

	udata = evas_object_data_get(pli->container, "ePlayer");

	/* add playlist item callbacks */
	edje_object_signal_callback_add(pli->edje, "PLAYLIST_SCROLL_UP", "",
			                        (EdjeCb) cb_playlist_scroll_up, udata);
	edje_object_signal_callback_add(pli->edje, "PLAYLIST_SCROLL_DOWN", "",
			                        (EdjeCb) cb_playlist_scroll_down, udata);
	edje_object_signal_callback_add(pli->edje, "PLAYLIST_ITEM_PLAY", "",
			                        (EdjeCb) cb_playlist_item_play, udata);
	edje_object_signal_callback_add(pli->edje, "PLAYLIST_ITEM_SELECTED", "",
	                                (EdjeCb) cb_playlist_item_selected, udata);
	edje_object_signal_callback_add(pli->edje, "PLAYLIST_ITEM_REMOVE", "",
	                                (EdjeCb) cb_playlist_item_remove, udata);

	esmart_container_element_append(pli->container, pli->edje);

	return true;
}

/**
 * Creates a new PlayListItem object.
 *
 * @param file File to load.
 * @return The new PlayListItem object.
 */
PlayListItem *playlist_item_new(const char *file, Evas *evas,
                                Evas_List *plugins,
                                Evas_Object *container,
                                const char *theme) {
	PlayListItem *pli;

	assert(file);
	assert(evas);
	assert(plugins);
	assert(theme);

	if (!(pli = calloc(1, sizeof(PlayListItem))))
		return NULL;
	
	pthread_mutex_init(&pli->pos_mutex, NULL);

	/* find the plugin for this file */
	if (!(pli->plugin = find_plugin(file, plugins))) {
		debug(DEBUG_LEVEL_WARNING, "No plugin found for %s!\n", file);

		playlist_item_free(pli);
		return NULL;
	}

	snprintf(pli->file, sizeof(pli->file), "%s", file);
	playlist_item_get_info(pli);

	pli->evas = evas;
	pli->container = container;
	pli->theme = theme;

	if (container && !playlist_item_show(pli)) {
		playlist_item_free(pli);
		return NULL;
	}

	return pli;
}

