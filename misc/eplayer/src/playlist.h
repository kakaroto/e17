#ifndef __PLAYLIST_H
#define __PLAYLIST_H

#include <Evas.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PLAYLIST_ITEM_COMMENT_LEN 256

typedef struct {
	char file[PATH_MAX + 1];

	/* vorbis comments: */
	char artist[PLAYLIST_ITEM_COMMENT_LEN];
	char title[PLAYLIST_ITEM_COMMENT_LEN];
	char album[PLAYLIST_ITEM_COMMENT_LEN];
	double duration;
} PlayListItem;

typedef struct {
	int num; /* number of entries */
	Evas_List *items;
} PlayList;

PlayList *playlist_new();
void playlist_free();

int playlist_load_file(PlayList *pl, const char *file, int append);
int playlist_load_dir(PlayList *pl, const char *dir, int append);
int playlist_load_m3u(PlayList *pl, const char *file, int append);

void playlist_remove_all(PlayList *pl);

#ifdef __cplusplus
}
#endif

#endif
