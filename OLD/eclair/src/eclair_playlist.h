#ifndef _ECLAIR_PLAYLIST_H_
#define _ECLAIR_PLAYLIST_H_

#include <Evas.h>
#include <Ecore.h>
#include "eclair_types.h"

struct _Eclair_Playlist_Container_Object
{
   Evas_Object *rect;
   Evas_Object *text;
};

struct _Eclair_Playlist
{
   Evas_List *playlist;
   Evas_List *current;
   Evas_List *shuffle_list;
   Evas_List *removed_media_files;
   Evas_Bool shuffle;
   Evas_Bool repeat;
   Ecore_Timer *media_files_destructor_timer;

   Eclair *eclair;
};

void eclair_playlist_init(Eclair_Playlist *playlist, Eclair *eclair);
void eclair_playlist_shutdown(Eclair_Playlist *playlist);
Evas_Bool eclair_playlist_save(Eclair_Playlist *playlist, const char *path);

Eclair_Media_File *eclair_playlist_current_media_file(Eclair_Playlist *playlist);
Eclair_Media_File *eclair_playlist_prev_media_file(Eclair_Playlist *playlist);
Eclair_Media_File *eclair_playlist_next_media_file(Eclair_Playlist *playlist);

Evas_Bool eclair_playlist_add_dir(Eclair_Playlist *playlist, char *dir, Evas_Bool update_container, Evas_Bool autoplay);
Evas_Bool eclair_playlist_add_m3u(Eclair_Playlist *playlist, char *m3u_path, Evas_Bool update_container, Evas_Bool autoplay);
Evas_Bool eclair_playlist_add_uri(Eclair_Playlist *playlist, char *uri, Evas_Bool update_container, Evas_Bool autoplay);

void eclair_playlist_remove_media_file(Eclair_Playlist *playlist, Eclair_Media_File *media_file, Evas_Bool update_container);
Evas_List *eclair_playlist_remove_media_file_list(Eclair_Playlist *playlist, Evas_List *list, Evas_Bool update_container);
void eclair_playlist_remove_selected_media_files(Eclair_Playlist *playlist);
void eclair_playlist_remove_unselected_media_files(Eclair_Playlist *playlist);
void eclair_playlist_empty(Eclair_Playlist *playlist);

void eclair_playlist_set_repeat(Eclair_Playlist *playlist, Evas_Bool repeat);
void eclair_playlist_set_shuffle(Eclair_Playlist *playlist, Evas_Bool shuffle);
void eclair_playlist_reset_shuffle_list(Eclair_Playlist *playlist);

Evas_List *eclair_playlist_get_next_list(Eclair_Playlist *playlist);
Evas_List *eclair_playlist_get_prev_list(Eclair_Playlist *playlist);

void eclair_playlist_current_set(Eclair_Playlist *playlist, Eclair_Media_File *media_file);
void eclair_playlist_current_set_list(Eclair_Playlist *playlist, Evas_List *list);
void eclair_playlist_prev_as_current(Eclair_Playlist *playlist);
void eclair_playlist_next_as_current(Eclair_Playlist *playlist);

#endif
