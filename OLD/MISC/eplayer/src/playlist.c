/*
 * $Id$
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
 * @param plugins
 * @return The newly created PlayList.
 */
PlayList *playlist_new(Evas *evas, Eina_List *plugins,
                       const char *theme) {
	PlayList *pl;

	if (!(pl = calloc(1, sizeof(PlayList))))
		return NULL;

	pl->num = pl->duration = 0;
	pl->items = pl->cur_item = NULL;

	pl->evas = evas;
	pl->plugins = plugins;
	pl->container = evas_object_name_find(evas, "PlayList");
	pl->theme = theme;

	return pl;
}

void playlist_container_set(PlayList *pl, Evas_Object *container) {
	Eina_List *l;

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
	if (!pl)
		return;

	while (pl->items) {
		playlist_item_free((PlayListItem *) pl->items->data);
		pl->items = eina_list_remove(pl->items, pl->items->data);
	}

	pl->num = pl->duration = 0;
}

/**
 * Removes the item from a PlayList.
 *
 * @param pl
 * @param pli
 */
void playlist_remove_item(PlayList *pl, PlayListItem *pli) {
	assert(pl);
	assert(pli);

	pl->num--;
	pl->duration -= pli->duration;

	pl->items = eina_list_remove(pl->items, pli);
	playlist_item_free(pli);
}

PlayListItem *playlist_current_item_get(PlayList *pl) {
	return pl ? (pl->cur_item ? pl->cur_item->data : NULL) : NULL;
}

void playlist_current_item_set(PlayList *pl, PlayListItem *pli) {
	if (!pl)
		return;

	if (!pli) /* move to the beginning */
		pl->cur_item = pl->items;
	else
		pl->cur_item = eina_list_data_find_list(pl->items, pli);
}

bool playlist_current_item_has_next(PlayList *pl) {
	if (!pl || !pl->cur_item)
		return false;

	return !!pl->cur_item->next;
}

bool playlist_current_item_has_prev(PlayList *pl) {
	if (!pl || !pl->cur_item)
		return false;

	return !!pl->cur_item->prev;
}

/**
 * Moves the current item of a PlayList to the next item.
 *
 * @param pl
 * @return true if the current item has been set to the beginning, else false
 */
bool playlist_current_item_next(PlayList *pl) {
	if (!pl || !pl->cur_item)
		return false;
	
	if (pl->cur_item->next) {
		pl->cur_item = pl->cur_item->next;
		return false;
	} else { /* move to the beginning */
		pl->cur_item = pl->items;
		return true;
	}
}

bool playlist_current_item_prev(PlayList *pl) {
	if (!pl || !pl->cur_item)
		return false;

	if (playlist_current_item_has_prev(pl)) {
		pl->cur_item = pl->cur_item->prev;
		return true;
	} else
		return false;
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

/**
 * Add a single file to a PlayList.
 *
 * @param pl
 * @param file File to add
 * @param append If 0, the old entries will be overwritten.
 * @return Boolean success or failure.
 */
bool playlist_load_file(PlayList *pl, const char *file, bool append) {
	PlayListItem *pli;

	assert(pl);
	
	if (!(pli = playlist_item_new(file, pl->evas, pl->plugins, pl->container,
	                              pl->theme)))
		return false;

	if (!append)
		playlist_remove_all(pl);

	pl->items = eina_list_append(pl->items, pli);

	if (!append)
		pl->cur_item = pl->items;
	
	pl->num++;
	pl->duration += pli->duration;

	return true;
}

static void finish_playlist(PlayList *pl, Eina_List *list, bool append) {
	list = eina_list_reverse(list);
	
	if (append) {
		pl->items = eina_list_merge(pl->items, list);
	} else {
		playlist_remove_all(pl);
		pl->items = list;
		pl->cur_item = pl->items;
	}
	pl->num = eina_list_count(pl->items);
}

/**
 * Add a directory to a PlayList.
 *
 * @param pl
 * @param path Directory to load
 * @param append If 0, the old entries will be overwritten.
 * @return Boolean success or failure.
 */
bool playlist_load_dir(PlayList *pl, const char *path, bool append) {
	DIR *dir;
	struct dirent *entry;
	char buf[PATH_MAX + 1];

	if (!pl || !(dir = opendir(path)))
		return false;

	while ((entry = readdir(dir))) {
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
			continue;
		
		snprintf(buf, sizeof(buf), "%s/%s", path, entry->d_name);
		playlist_load_any(pl, buf, 1);
	};

	closedir(dir);

	if (!pl->cur_item || !pl->cur_item->data)
		pl->cur_item = pl->items;

	return true;
}

/**
 * Add a M3U file to a PlayList.
 *
 * @param pl
 * @param file
 * @param append If 0, the old entries will be overwritten.
 * @return Boolean success or failure.
 */
bool playlist_load_m3u(PlayList *pl, const char *file, bool append) {
	PlayListItem *pli = NULL;
	Eina_List *tmp = NULL;
	FILE *fp;
	char buf[PATH_MAX + 1], path[PATH_MAX + 1], dir[PATH_MAX + 1];
	char *ptr;

	if (!pl || !(fp = fopen(file, "r")))
		return false;

	if ((ptr = strrchr(file, '/'))) {
		snprintf(dir, sizeof(dir), "%s", file);
		dir[ptr - file] = 0;
	} else
		getcwd(dir, sizeof(dir));

	while (fgets(buf, sizeof(buf), fp)) {
		if (!(ptr = strstrip(buf)) || !*ptr || *ptr == '#')
			continue;
		else if (*ptr != '/') {
			/* if it's a relative path, prepend the directory */
			snprintf(path, sizeof(path), "%s/%s", dir, buf);
			ptr = path;
		}

		if ((pli = playlist_item_new(ptr, pl->evas, pl->plugins, pl->container,
		                             pl->theme))) {
			tmp = eina_list_prepend(tmp, pli);
			pl->num++;
			pl->duration += pli->duration;
		}
	}

	fclose(fp);

	finish_playlist(pl, tmp, append);
	
	return true;
}

/**
 * Add a M3U file, a media file or a directory to a PlayList.
 *
 * @param pl
 * @param path
 * @param append If false, the old entries will be overwritten.
 * @return Boolean success or failure.
 */
bool playlist_load_any(PlayList *pl, const char *path, bool append) {
	int len;
	
	if (is_dir(path))
		return playlist_load_dir(pl, path, append);

	/* FIXME we check for m3u using the suffix :/ */
	len = strlen(path) - 3;
	
	if (len >= 0 && !strcasecmp(&path[len], "m3u"))
		return playlist_load_m3u(pl, path, append);
	else
		return playlist_load_file(pl, path, append);
}
