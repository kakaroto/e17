#include <config.h>
#include <vorbis/vorbisfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "playlist.h"

static int is_dir (const char *dir) {
	struct stat st;

	if (stat(dir, &st))
		return 0;

	return (S_ISDIR(st.st_mode));
}

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
 * Searches a Vorbis comment for title, artist and album and stores
 * them in a PlayListItem.
 *
 * @param pli The PlayListItem to store the comments in
 * @param comment The Vorbis comment to search.
 */
void playlist_item_read_comments(PlayListItem *pli,
                                 vorbis_comment *comment) {
#define NUM_COMMENTS 3
	char *cmt, *key[NUM_COMMENTS] = {"artist=", "title=", "album="};
	char *dest[NUM_COMMENTS] = {pli->artist, pli->title, pli->album};
	int i, j, len[NUM_COMMENTS];

	for (i = 0; i < NUM_COMMENTS; i++)
		len[i] = strlen (key[i]);

	for (i = 0; i < comment->comments; i++) {
		cmt = comment->user_comments[i];

		for (j = 0; j < NUM_COMMENTS; j++) {
			if (!strncmp(cmt, key[j], len[j])) {
				snprintf(dest[j], PLAYLIST_ITEM_COMMENT_LEN,
				         "%s", &cmt[len[j]]);
			}
		}
	}
#undef NUM_COMMENTS
}

/**
 * Creates a new PlayListItem object.
 *
 * @param file File to load.
 * @return The new PlayListItem object.
 */
PlayListItem *playlist_item_new(const char *file) {
	PlayListItem *pli;
	FILE *fp;
	OggVorbis_File vf = {0};
	vorbis_info *info;

	if (!(fp = fopen(file, "rb")) || ov_open(fp, &vf, NULL, 0))
		return NULL;
	
	if (!(pli = malloc(sizeof(PlayListItem))))
		return NULL;

	memset(pli, 0, sizeof(PlayListItem));
	snprintf(pli->file, sizeof(pli->file), "%s", file);

	/* read the vorbis comments etc */
	playlist_item_read_comments(pli, ov_comment(&vf, -1));

	/* get bitrate and number of channels */
	info = ov_info(&vf, -1);
	pli->channels = info->channels;
	pli->rate = info->rate;
	
	pli->duration = ov_time_total(&vf, -1);

	ov_clear(&vf);	/* ov_clear closes the file, too */

	return pli;
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
 * Creates a new PlayList object.
 *
 * @return The newly created PlayList.
 */
PlayList *playlist_new() {
	PlayList *pl;

	if (!(pl = malloc(sizeof(PlayList))))
		return NULL;

	memset(pl, 0, sizeof(PlayList));

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
 */
int playlist_load_file(PlayList *pl, const char *file, int append) {
	PlayListItem *pli;
	
	if (!pl || !(pli = playlist_item_new(file)))
		return 0;

	if (!append)
		playlist_remove_all(pl);

	pl->items = evas_list_append(pl->items, pli);

	if (!append)
		pl->cur_item = pl->items;

	return 1;
}

/**
 * Add a directory to a PlayList.
 *
 * @param pl
 * @param path Directory to load
 * @param append If 0, the old entries will be overwritten.
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
	       || strcmp(entry->d_name, "..")));
	
	/* real entries: load directories recursively */
	while ((entry = readdir(dir))) {
		if (is_dir(entry->d_name))
			playlist_load_dir(pl, entry->d_name, 1);
		else if ((pli = playlist_item_new(entry->d_name)))
			tmp = evas_list_prepend(tmp, pli);
	}

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

	while (fgets (buf, sizeof (buf), fp)) {
		if (!(ptr = strstrip(buf)) || !*ptr || *ptr == '#')
			continue;
		else if (*ptr != '/') { /* if it's a relative path, prepend the directory */
			snprintf(path, sizeof(path), "%s/%s", dir, buf);
			ptr = path;
		}

		if ((pli = playlist_item_new(ptr))) {
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
 * Add a M3U file, an Ogg file or a directory to a PlayList.
 *
 * @param pl
 * @param path
 * @param append If 0, the old entries will be overwritten.
 */
int playlist_load_any(PlayList *pl, const char *path, int append) {
	char *ptr = NULL;
	
	if (is_dir(path))
		return playlist_load_dir(pl, path, append);

	/* FIXME we check for m3u or ogg using the suffix :/ */
	ptr = (char *) &path[strlen(path) - 3];

	if (!strcasecmp(ptr, "ogg"))
		return playlist_load_file(pl, path, append);
	else if (!strcasecmp(ptr, "m3u"))
		return playlist_load_m3u(pl, path, append);
	else
		return 0;
}
