#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "playlist.h"
#include "utils.h"

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
	free(pli);
}

/**
 * Creates a new PlayListItem object.
 *
 * @param file File to load.
 * @return The new PlayListItem object.
 */
PlayListItem *playlist_item_new(Evas_List *plugins, const char *file) {
	PlayListItem *pli;
	Evas_List *l;
	InputPlugin *ip;

	if (!(pli = malloc(sizeof(PlayListItem))))
		return NULL;
	
	memset(pli, 0, sizeof(PlayListItem));

	pthread_mutex_init(&pli->pos_mutex, NULL);

	/* find the plugin for this file */
	for (l = plugins; l; l = l->next) {
		ip = l->data;

		if (ip->open(file)) {
			pli->plugin = ip;
			break;
		}
	}

	if (!pli->plugin) {
		debug(DEBUG_LEVEL_WARNING, "No plugin found for %s!\n", file);

		playlist_item_free(pli);
		return NULL;
	}

	snprintf(pli->file, sizeof(pli->file), "%s", file);
	playlist_item_get_info(pli);

	return pli;
}

/**
 * Creates a new PlayList object.
 *
 * @param plugins
 * @return The newly created PlayList.
 */
PlayList *playlist_new(Evas_List *plugins) {
	PlayList *pl;

	if (!(pl = malloc(sizeof(PlayList))))
		return NULL;

	memset(pl, 0, sizeof(PlayList));

	pl->plugins = plugins;

	return pl;
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
		pl->items = evas_list_remove(pl->items, pl->items->data);
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
	if (!pl || !pli)
		return;
	
	pl->num--;
	pl->duration -= pli->duration;

	pl->items = evas_list_remove(pl->items, pli);
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
		pl->cur_item = evas_list_find_list(pl->items, pli);
}

int playlist_current_item_has_next(PlayList *pl) {
	return pl ? !!pl->cur_item->next : 0;
}

int playlist_current_item_has_prev(PlayList *pl) {
	return pl ? !!pl->cur_item->prev : 0;
}

/**
 * Moves the current item of a PlayList to the next item.
 *
 * @param pl
 * return 1 if the current item has been set to the beginning, else 0
 */
int playlist_current_item_next(PlayList *pl) {
	if (!pl)
		return 0;
	
	if (pl->cur_item->next) {
		pl->cur_item = pl->cur_item->next;
		return 0;
	} else { /* move to the beginning */
		pl->cur_item = pl->items;
		return 1;
	}
}

int playlist_current_item_prev(PlayList *pl) {
	if (!pl)
		return 0;

	if (playlist_current_item_has_prev(pl)) {
		pl->cur_item = pl->cur_item->prev;
		return 1;
	} else
		return 0;
}

/**
 * Appends a list with PlayListItems to a PlayList.
 *
 * @param pl
 * @param list
 */
static void playlist_append_list(PlayList *pl, Evas_List *list) {
	if (!pl || !list)
		return;
	
	if (!pl->items)
		pl->items = list;
	else {
		pl->items->last->next = list;
		list->prev = pl->items->last;
		pl->items->last = list->last;
	}
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
int playlist_load_file(PlayList *pl, const char *file, int append) {
	PlayListItem *pli;
	
	if (!pl || !(pli = playlist_item_new(pl->plugins, file)))
		return 0;

	if (!append)
		playlist_remove_all(pl);

	pl->items = evas_list_append(pl->items, pli);

	if (!append)
		pl->cur_item = pl->items;
	
	pl->num++;
	pl->duration += pli->duration;

	return 1;
}

static void finish_playlist(PlayList *pl, Evas_List *list, int append) {
	list = evas_list_reverse(list);
	
	if (append)
		playlist_append_list(pl, list);
	else {
		playlist_remove_all(pl);
		pl->items = list;
		pl->cur_item = pl->items;
	}
}

/**
 * Add a directory to a PlayList.
 *
 * @param pl
 * @param path Directory to load
 * @param append If 0, the old entries will be overwritten.
 * @return Boolean success or failure.
 */
int playlist_load_dir(PlayList *pl, const char *path, int append) {
	DIR *dir;
	struct dirent *entry;
	char buf[PATH_MAX + 1];

	if (!pl || !(dir = opendir(path)))
		return 0;

	while ((entry = readdir(dir))) {
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
			continue;
		
		snprintf(buf, sizeof(buf), "%s/%s", path, entry->d_name);
		playlist_load_any(pl, buf, 1);
	};

	closedir(dir);

	if (!pl->cur_item || !pl->cur_item->data)
		pl->cur_item = pl->items;

	return 1;
}

/**
 * Add a M3U file to a PlayList.
 *
 * @param pl
 * @param file
 * @param append If 0, the old entries will be overwritten.
 * @return Boolean success or failure.
 */
int playlist_load_m3u(PlayList *pl, const char *file, int append) {
	PlayListItem *pli = NULL;
	Evas_List *tmp = NULL;
	FILE *fp;
	char buf[PATH_MAX + 1], path[PATH_MAX + 1], dir[PATH_MAX + 1];
	char *ptr;

	if (!pl || !(fp = fopen(file, "r")))
		return 0;

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

		if ((pli = playlist_item_new(pl->plugins, ptr))) {
			tmp = evas_list_prepend(tmp, pli);
			pl->num++;
			pl->duration += pli->duration;
		}
	}

	fclose(fp);

	finish_playlist(pl, tmp, append);
	
	return 1;
}

/**
 * Add a M3U file, a media file or a directory to a PlayList.
 *
 * @param pl
 * @param path
 * @param append If 0, the old entries will be overwritten.
 * @return Boolean success or failure.
 */
int playlist_load_any(PlayList *pl, const char *path, int append) {
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
