#include <config.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include "playlist.h"
#include "eplayer.h"

/**
 * Removes leading and trailing whitespace from a string.
 *
 * @param str String to strip
 * @return Stripped string
 */
static char *strstrip(char *str) {
	char *start, *ptr = str;
	
	/* step over leading whitespace */
	for (start = str; isspace(*start); start++);
	
	if (str != start) {
		while ((*ptr++ = *start++));
		*ptr = 0;
	}

	if (!*str)
		return str;

	/* remove trailing whitespace */
	ptr = &str[strlen(str) - 1];

	if (!isspace(*ptr))
		return str;
	
	while (isspace(*ptr) && ptr >= str)
		ptr--;

	ptr[1] = 0;

	return str;
}

/**
 * Fills a PlayListItem's comments/info fields.
 *
 * @param pli The PlayListItem to store the comments/info stuff in.
 */
void playlist_item_get_info(PlayListItem *pli) {
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
	if (pli)
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

	/* find the plugin for this file */
	for (l = plugins; l; l = l->next) {
		ip = l->data;

		if (ip->open(file)) {
			pli->plugin = ip;
			break;
		}
	}

	if (!pli->plugin) {
#ifdef DEBUG
		printf("No plugin found for %s!\n", file);
#endif

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

	return 1;
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
	PlayListItem *pli = NULL;
	Evas_List *tmp = NULL;
	DIR *dir;
	struct dirent *entry;

	if (!pl || !(dir = opendir(path)))
		return 0;

	/* ignore "." and ".." */
	while ((entry = readdir(dir))
	       && (!strcmp(entry->d_name, ".")
	       || !strcmp(entry->d_name, "..")));

	if (!entry)
		return 0;
	
	/* real entries: load directories recursively */
	do {
		if (is_dir(entry->d_name))
			playlist_load_dir(pl, entry->d_name, 1);
		else if ((pli = playlist_item_new(pl->plugins, entry->d_name))) {
			tmp = evas_list_prepend(tmp, pli);
			pl->num++;
		}
	} while ((entry = readdir(dir)));

	closedir(dir);

	tmp = evas_list_reverse(tmp);
	
	if (append)
		playlist_append_list(pl, tmp);
	else {
		playlist_remove_all(pl);
		pl->items = tmp;
		pl->cur_item = pl->items;
	}
	
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
	char buf[1024], path[PATH_MAX + 1], *dir, *ptr;

	if (!pl || !(fp = fopen(file, "r")))
		return 0;

	/* get the directory */
	dir = strdup(file);
	ptr = strrchr(dir, '/');
	*ptr = 0;

	while (fgets(buf, sizeof (buf), fp)) {
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
		}
	}

	fclose(fp);
	free(dir);

	tmp = evas_list_reverse(tmp);
	
	if (append)
		playlist_append_list(pl, tmp);
	else {
		playlist_remove_all(pl);
		pl->items = tmp;
		pl->cur_item = pl->items;
	}
	
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
	char *ptr = NULL;
	
	if (is_dir(path))
		return playlist_load_dir(pl, path, append);

	/* FIXME we check for m3u using the suffix :/ */
	ptr = (char *) &path[strlen(path) - 3];

	if (!strcasecmp(ptr, "m3u"))
		return playlist_load_m3u(pl, path, append);
	else
		return playlist_load_file(pl, path, append);
}
