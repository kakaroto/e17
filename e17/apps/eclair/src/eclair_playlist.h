#ifndef _ECLAIR_PLAYLIST_H_
#define _ECLAIR_PLAYLIST_H_

#include "eclair_private.h"

void eclair_playlist_init(Eclair_Playlist *playlist, Eclair *eclair);
void eclair_playlist_empty(Eclair_Playlist *playlist);

Eclair_Media_File *eclair_playlist_current_media_file(Eclair_Playlist *playlist);
Eclair_Media_File *eclair_playlist_prev_media_file(Eclair_Playlist *playlist);
Eclair_Media_File *eclair_playlist_next_media_file(Eclair_Playlist *playlist);

Evas_Bool eclair_playlist_add_dir(Eclair_Playlist *playlist, char *dir);
Evas_Bool eclair_playlist_add_uri(Eclair_Playlist *playlist, char *uri);
void eclair_playlist_remove_media_file(Eclair_Playlist *playlist, Eclair_Media_File *media_file);
Evas_List *eclair_playlist_remove_media_file_list(Eclair_Playlist *playlist, Evas_List *list);

void eclair_playlist_current_set(Eclair_Playlist *playlist, Eclair_Media_File *media_file);
void eclair_playlist_current_set_list(Eclair_Playlist *playlist, Evas_List *list);
void eclair_playlist_prev_as_current(Eclair_Playlist *playlist);
void eclair_playlist_next_as_current(Eclair_Playlist *playlist);

#endif
