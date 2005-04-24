#include "eclair_playlist.h"
#include "../config.h"
#include <Esmart/Esmart_Container.h>
#include <stdlib.h>
#include <string.h>
#include "eclair.h"
#include "eclair_utils.h"
#include "eclair_callbacks.h"

//Initialize the playlist
void eclair_playlist_init(Eclair *eclair, Eclair_Playlist *playlist)
{
   if (!playlist)
      return;   

   playlist->playlist = NULL;
   playlist->current = NULL;
   playlist->shuffle = 0;
   playlist->repeat = 0;
   playlist->eclair = eclair;
}

//Empty the playlist and destroy all the media_file
void eclair_playlist_empty(Eclair_Playlist *playlist)
{
   Evas_List *l;

   if (!playlist)
      return;

   for (l = playlist->playlist; l; )
      l = eclair_playlist_remove_media_file_list(playlist, l);

   evas_list_free(playlist->playlist);
   playlist->playlist = NULL;
   playlist->current = NULL;
}

//Free a media file
void eclair_playlist_media_file_free(Eclair_Playlist_Media_File *media_file)
{
   if (!media_file)
      return;

   free(media_file->path);
   free(media_file->artist);
   free(media_file->title);
   free(media_file->album);
   free(media_file->genre);
   free(media_file->comment);
}

//Update the entry of the media file in the playlist with tag infos
void eclair_playlist_media_file_entry_update(Eclair_Playlist_Media_File *media_file, Eclair *eclair)
{
   char length[10] = "";
   char *artist_title_string;
   const char *filename;

   if (!media_file)
      return;
   if (!media_file->playlist_entry)
      return;

   if (media_file->length >= 0)
   {
      eclair_utils_second_to_string(media_file->length, media_file->length, length);
      edje_object_part_text_set(media_file->playlist_entry, "playlist_entry_length", length);
   }
   else
      edje_object_part_text_set(media_file->playlist_entry, "playlist_entry_length", "");

   if ((artist_title_string = eclair_utils_mediafile_to_artist_title_string(media_file)))
   {
      edje_object_part_text_set(media_file->playlist_entry, "playlist_entry_name", artist_title_string);
      free(artist_title_string);
   }
   else if ((filename = eclair_utils_path_to_filename(media_file->path)))
      edje_object_part_text_set(media_file->playlist_entry, "playlist_entry_name", filename);
   else
      edje_object_part_text_set(media_file->playlist_entry, "playlist_entry_name", "Media");

   if (!eclair)
      return;

   if (media_file == evas_list_data(eclair->playlist.current))
      eclair_current_file_set(eclair, media_file);
}

//Return the active media file
Eclair_Playlist_Media_File *eclair_playlist_current_media_file(Eclair_Playlist *playlist)
{
   if (!playlist)
      return NULL;

   return evas_list_data(playlist->current);
}

//Return the media file just before the active media file
Eclair_Playlist_Media_File *eclair_playlist_prev_media_file(Eclair_Playlist *playlist)
{
   if (!playlist)
      return NULL;

   return (Eclair_Playlist_Media_File *)evas_list_data(evas_list_prev(playlist->current));
}

//Return the media file just after the active media file
Eclair_Playlist_Media_File *eclair_playlist_next_media_file(Eclair_Playlist *playlist)
{
   if (!playlist)
      return NULL;
   return (Eclair_Playlist_Media_File *)evas_list_data(evas_list_next(playlist->current));
}

//Add a new media file to the playlist
Eclair_Playlist_Media_File *eclair_playlist_add_media_file(Eclair_Playlist *playlist, char *path)
{
   Eclair_Playlist_Media_File *new_media_file;
   Evas_Coord min_height;
   Eclair *eclair;

   if (!playlist || !path)
      return NULL;
   new_media_file = (Eclair_Playlist_Media_File *)calloc(1, sizeof(Eclair_Playlist_Media_File));
   new_media_file->path = strdup(path);

   if ((eclair = playlist->eclair))
   {
      if (eclair->playlist_container)
      {
         new_media_file->playlist_entry = edje_object_add(evas_object_evas_get(eclair->playlist_container));
         edje_object_file_set(new_media_file->playlist_entry, PACKAGE_DATA_DIR "/themes/default.edj", "eclair_playlist_entry");
         evas_object_data_set(new_media_file->playlist_entry, "media_file", new_media_file);
         evas_object_data_set(new_media_file->playlist_entry, "media_filen", new_media_file->path);
         if (eclair->playlist_entry_height <= 0)
         {
            edje_object_size_min_get(new_media_file->playlist_entry, NULL, &min_height);
            eclair->playlist_entry_height = (int)min_height;
         }
         evas_object_resize(new_media_file->playlist_entry, 1, eclair->playlist_entry_height);
         edje_object_part_text_set(new_media_file->playlist_entry, "playlist_entry_name", eclair_utils_path_to_filename(path));
         edje_object_part_text_set(new_media_file->playlist_entry, "playlist_entry_length", "");
         edje_object_signal_callback_add(new_media_file->playlist_entry, "eclair_play_entry", "*", eclair_gui_play_entry_cb, eclair); 
         evas_object_show(new_media_file->playlist_entry);
         esmart_container_element_append(eclair->playlist_container, new_media_file->playlist_entry);
      }
      
      eclair->meta_tag_files_to_scan = evas_list_append(eclair->meta_tag_files_to_scan, new_media_file);
      pthread_cond_broadcast(&eclair->meta_tag_cond);
   }

   playlist->playlist = evas_list_append(playlist->playlist, new_media_file);
   if (!playlist->current)
      eclair_playlist_current_set_list(playlist, playlist->playlist);

   return (Eclair_Playlist_Media_File *)evas_list_data(playlist->playlist->last);   
}

//Remove the media file from the playlist
void eclair_playlist_remove_media_file(Eclair_Playlist *playlist, Eclair_Playlist_Media_File *media_file)
{
   if (!playlist || !media_file)
      return;

   eclair_playlist_remove_media_file_list(playlist, evas_list_find_list(playlist->playlist, media_file));
}

//Remove the media file pointed by the list from the playlist
//Return the next media file
Evas_List *eclair_playlist_remove_media_file_list(Eclair_Playlist *playlist, Evas_List *list)
{
   Eclair_Playlist_Media_File *remove_media_file;
   Evas_List *next;

   if (!playlist || !list)
      return NULL;

   if (playlist->current == list)
   {
      if (playlist->current->next)
         eclair_playlist_next_as_current(playlist);
      else
         eclair_playlist_prev_as_current(playlist);            
   }

   if ((remove_media_file = evas_list_data(list)))
   {
      if (remove_media_file->playlist_entry && playlist->eclair)
         esmart_container_element_destroy(playlist->eclair->playlist_container, remove_media_file->playlist_entry);
      eclair_playlist_media_file_free(remove_media_file);
   }

   next = list->next;
   playlist->playlist = evas_list_remove_list(playlist->playlist, list);

   return next;
}

//Set the media file as the active media file  
void eclair_playlist_current_set(Eclair_Playlist *playlist, Eclair_Playlist_Media_File *media_file)
{
   if (!playlist)
      return;

   eclair_playlist_current_set_list(playlist, evas_list_find_list(playlist->playlist, media_file));
}

//Set the media file pointed by the list as the active media file  
void eclair_playlist_current_set_list(Eclair_Playlist *playlist, Evas_List *list)
{
   Eclair_Playlist_Media_File *media_file;

   if (!playlist)
      return;

   if ((media_file = eclair_playlist_current_media_file(playlist)))
   {
      if (media_file->playlist_entry)
         edje_object_signal_emit(media_file->playlist_entry, "signal_unset_current", "eclair_bin");
   }

   if ((media_file = evas_list_data(list)))
   {
      if (media_file->playlist_entry)
      {
         edje_object_signal_emit(media_file->playlist_entry, "signal_set_current", "eclair_bin");
         if (playlist->eclair)
         {
            if (playlist->eclair->playlist_container)
               esmart_container_scroll_to(playlist->eclair->playlist_container, media_file->playlist_entry);
         }
      }
      if (playlist->eclair)
         eclair_current_file_set(playlist->eclair, media_file);
   }
   else
   {
      if (playlist->eclair)
         eclair_current_file_set(playlist->eclair, NULL);
   }
   
   playlist->current = list;
} 

//Set the media file which is just before the active media file as the active media file 
void eclair_playlist_prev_as_current(Eclair_Playlist *playlist)
{
   if (!playlist)
      return;

   if (!playlist->current)
      return;

   eclair_playlist_current_set_list(playlist,  playlist->current->prev);
}

//Set the media file which is just after the active media file as the active media file 
void eclair_playlist_next_as_current(Eclair_Playlist *playlist)
{
   if (!playlist)
      return;

   if (!playlist->current)
      return;

   eclair_playlist_current_set_list(playlist,  playlist->current->next);
}
