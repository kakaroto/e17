#include "eclair_playlist.h"
#include "../config.h"
#include <string.h>
#include <stdio.h>
#include <Ecore_File.h>
#include <Edje.h>
#include "eclair.h"
#include "eclair_playlist_container.h"
#include "eclair_media_file.h"
#include "eclair_meta_tag.h"
#include "eclair_utils.h"
#include "eclair_config.h"

#define MAX_PATH_LEN 1024

static int eclair_playlist_media_files_destructor(void *data);

//Initialize the playlist
void eclair_playlist_init(Eclair_Playlist *playlist, Eclair *eclair)
{
   int shuffle, repeat;

   if (!playlist)
      return;

   playlist->playlist = NULL;
   playlist->current = NULL;
   playlist->shuffle_list = NULL;
   playlist->removed_media_files = NULL;
   playlist->shuffle = 0;
   playlist->repeat = 0;
   playlist->eclair = eclair;
   playlist->media_files_destructor_timer = ecore_timer_add(0.05, eclair_playlist_media_files_destructor, playlist);

   if (eclair)
   {
      if (eclair_config_get_prop_int(&eclair->config, "shuffle", "enabled", &shuffle) && shuffle)
         eclair_playlist_set_shuffle(playlist, 1);
      if (eclair_config_get_prop_int(&eclair->config, "repeat", "enabled", &repeat) && repeat)
         eclair_playlist_set_repeat(playlist, 1);
   }
}

//Shutdown the playlist
void eclair_playlist_shutdown(Eclair_Playlist *playlist)
{
   Eina_List *l;

   if (!playlist)
      return;

   ecore_timer_del(playlist->media_files_destructor_timer);
   for (l = playlist->playlist; l; l = l->next)
      eclair_media_file_free(l->data);
   for (l = playlist->removed_media_files; l; l = l->next)
      eclair_media_file_free(l->data);
   playlist->playlist = eina_list_free(playlist->playlist);
   playlist->shuffle_list = eina_list_free(playlist->shuffle_list);
   playlist->removed_media_files = eina_list_free(playlist->removed_media_files);
   playlist->current = NULL;

   if (playlist->eclair)
   {
      eclair_config_set_prop_int(&playlist->eclair->config, "shuffle", "enabled", playlist->shuffle);
      eclair_config_set_prop_int(&playlist->eclair->config, "repeat", "enabled", playlist->repeat);
      if (playlist->eclair->playlist_container)
         eclair_playlist_container_update(playlist->eclair->playlist_container);
   }
}

//Set the shuffle mode
void eclair_playlist_set_shuffle(Eclair_Playlist *playlist, Evas_Bool shuffle)
{
   if (!playlist)
      return;

   if (shuffle)
   {
      eclair_playlist_reset_shuffle_list(playlist);
      eclair_send_signal_to_all_windows(playlist->eclair, "signal_shuffle_enabled");
   }
   else
      eclair_send_signal_to_all_windows(playlist->eclair, "signal_shuffle_disabled");

   playlist->shuffle = shuffle;
}

//Set the repeat mode
void eclair_playlist_set_repeat(Eclair_Playlist *playlist, Evas_Bool repeat)
{
   if (!playlist)
      return;

   if (repeat)
      eclair_send_signal_to_all_windows(playlist->eclair, "signal_repeat_enabled");
   else
      eclair_send_signal_to_all_windows(playlist->eclair, "signal_repeat_disabled");
   
   playlist->repeat = repeat;
}

//Reset the shuffle list (should be called each time items are added or removed from the playlist)
void eclair_playlist_reset_shuffle_list(Eclair_Playlist *playlist)
{
   Eclair_Media_File *media_file;
   Eina_List *l;

   if (!playlist || !playlist->shuffle)
      return;
   
   for (l = playlist->playlist; l; l = l->next)
   {
      if ((media_file = l->data))
         media_file->shuffle_node = NULL;
   }
   playlist->shuffle_list = eina_list_free(playlist->shuffle_list);

   if (playlist->current && (media_file = playlist->current->data))
   {
      playlist->shuffle_list = eina_list_append(playlist->shuffle_list, media_file);
      media_file->shuffle_node = eina_list_last(playlist->shuffle_list);
   }
}

//Called every 50ms and destroy the removed media files
int eclair_playlist_media_files_destructor(void *data)
{
   Eclair_Playlist *playlist;
   Eina_List *l, *next;
   Eclair_Media_File *current_file;

   if (!(playlist = data))
      return 0;

   for (l = playlist->removed_media_files; l; l = next)
   {
      next = l->next;
      if (!(current_file = l->data))
         playlist->removed_media_files = eina_list_remove_list(playlist->removed_media_files, l);
      else if (!current_file->in_meta_tag_process && !current_file->in_cover_process)
      {
         eclair_media_file_free(current_file);
         playlist->removed_media_files = eina_list_remove_list(playlist->removed_media_files, l);
      }
   }

   return 1;
}

//Save the playlist
//0 if failed
Evas_Bool eclair_playlist_save(Eclair_Playlist *playlist, const char *path)
{
   Eina_List *l;
   FILE *playlist_file;
   Eclair_Media_File *media_file;

   if (!playlist || !path)
      return 0;
   
   if (!(playlist_file = fopen(path, "wt")))
      return 0;

   for (l = playlist->playlist; l; l = l->next)
   {
      if (!(media_file = l->data) || !media_file->path || strlen(media_file->path) <= 0)
         continue;
      fprintf(playlist_file, "%s\n", media_file->path);
   }
   fclose(playlist_file);
   return 1;
}

//Return the active media file
Eclair_Media_File *eclair_playlist_current_media_file(Eclair_Playlist *playlist)
{
   if (!playlist)
      return NULL;

   return eina_list_data_get(playlist->current);
}

//Add recursively a directory
Evas_Bool eclair_playlist_add_dir(Eclair_Playlist *playlist, char *dir, Evas_Bool update_container, Evas_Bool autoplay)
{
   Ecore_List *files;
   Ecore_List_Node *l;
   char *filename, *filepath;

   if (!playlist || !dir || !ecore_file_is_dir(dir))
      return 0;

   if ((files = ecore_file_ls(dir)))
   {
      for (l = files->first; l; l = l->next)
      {
         if (!(filename = l->data))
            continue;
         filepath = malloc(strlen(dir) + strlen(filename) + 2);
         sprintf(filepath, "%s/%s", dir, filename);
         eclair_playlist_add_uri(playlist, filepath, 0, autoplay);
         free(filepath);
      }
      ecore_list_destroy(files);
   }

   if (playlist->eclair && update_container)
      eclair_playlist_container_update(playlist->eclair->playlist_container);

   return 1;
}

//Add files stored in the m3u file
Evas_Bool eclair_playlist_add_m3u(Eclair_Playlist *playlist, char *m3u_path, Evas_Bool update_container, Evas_Bool autoplay)
{
   FILE *m3u_file;
   char line[MAX_PATH_LEN], *path, *c, *m3u_dir;

   if (!playlist || !m3u_path || !(m3u_file = fopen(m3u_path, "rt")))
      return 0;

   m3u_dir = ecore_file_dir_get(m3u_path);

   while (fgets(line, MAX_PATH_LEN, m3u_file))
   {
      if (line[0] == '#')
         continue;

      for (c = strpbrk(line, "\r\n"); c; c = strpbrk(c, "\r\n"))
         *c = 0;
   
      if (eclair_utils_uri_is_mrl(line) || line[0] == '/')
         eclair_playlist_add_uri(playlist, line, 0, autoplay);
      else if (m3u_dir)
      {
         path = malloc(strlen(m3u_dir) + strlen(line) + 2);
         sprintf(path, "%s/%s", m3u_dir, line);
         eclair_playlist_add_uri(playlist, path, 0, autoplay);
         free(path);
      }
   }

   free(m3u_dir);
   fclose(m3u_file);

   if (playlist->eclair && update_container)
      eclair_playlist_container_update(playlist->eclair->playlist_container);

   return 1;
}

//Add the media file located at the uri
Evas_Bool eclair_playlist_add_uri(Eclair_Playlist *playlist, char *uri, Evas_Bool update_container, Evas_Bool autoplay)
{
   Eclair_Media_File *new_media_file;
   Eclair *eclair;
   char *clean_uri, *new_path, *ext;

   if (!playlist || !uri || !(eclair = playlist->eclair))
      return 0;

   if (eclair_utils_uri_is_mrl(uri))
   {
      if (!(clean_uri = eclair_utils_remove_uri_special_chars(uri)))
         return 0;

      if (strlen(clean_uri) <= 7 || strncmp(clean_uri, "file://", 7) != 0)
         new_path = clean_uri;
      else
      {
         new_path = strdup(&clean_uri[7]);
         free(clean_uri);
      }
   }
   else
      new_path = strdup(uri);

   if (!eclair_utils_uri_is_mrl(new_path) && eclair_playlist_add_dir(playlist, new_path, 0, autoplay))
   {
      free(new_path);
      if (update_container)
         eclair_playlist_container_update(eclair->playlist_container);
   }
   else if (!eclair_utils_uri_is_mrl(new_path) && (ext = eclair_utils_file_get_extension(new_path)) && strcmp(ext, "m3u") == 0)
   {
      eclair_playlist_add_m3u(playlist, new_path, 0, autoplay);
      free(new_path);
      if (update_container)
         eclair_playlist_container_update(eclair->playlist_container);
   }
   else
   {   
      new_media_file = eclair_media_file_new();
      new_media_file->path = new_path;
      playlist->playlist = eina_list_append(playlist->playlist, new_media_file);
      if (!playlist->current)
      {
         eclair_playlist_current_set_list(playlist, playlist->playlist);
         if (autoplay)
            eclair_play_current(eclair);
      }
      if (!eclair_utils_uri_is_mrl(new_media_file->path))
         eclair_meta_tag_add_file_to_scan(&eclair->meta_tag_manager, new_media_file);
   }

   if (update_container)
      eclair_playlist_container_update(eclair->playlist_container);

   return 1;   
}

//Remove the media file from the playlist
void eclair_playlist_remove_media_file(Eclair_Playlist *playlist, Eclair_Media_File *media_file, Evas_Bool update_container)
{
   if (!playlist || !media_file)
      return;

   eclair_playlist_remove_media_file_list(playlist, eina_list_data_find_list(playlist->playlist, media_file), update_container);
}

//Remove the media file pointed by the list from the playlist
//Return the next media file
Eina_List *eclair_playlist_remove_media_file_list(Eclair_Playlist *playlist, Eina_List *list, Evas_Bool update_container)
{
   Eclair_Media_File *remove_media_file;
   Eina_List *next;

   if (!playlist || !list)
      return NULL;

   if (playlist->current == list)
      eclair_playlist_current_set_list(playlist, NULL);

   if ((remove_media_file = list->data))
   {
      remove_media_file->delete_me = 1;
      playlist->removed_media_files = eina_list_append(playlist->removed_media_files, remove_media_file);
      playlist->shuffle_list = eina_list_remove_list(playlist->shuffle_list, remove_media_file->shuffle_node);
   }

   next = list->next;
   playlist->playlist = eina_list_remove_list(playlist->playlist, list);

   if (update_container && playlist->eclair && playlist->eclair->playlist_container)
      eclair_playlist_container_update(playlist->eclair->playlist_container);

   return next;
}

//Remove the selected media files
void eclair_playlist_remove_selected_media_files(Eclair_Playlist *playlist)
{
   Eina_List *l;
   Eclair_Media_File *media_file;

   if (!playlist)
      return;

   for (l = playlist->playlist; l; )
   {
      if ((media_file = l->data) && media_file->selected)
         l = eclair_playlist_remove_media_file_list(playlist, l, 0);
      else
         l = l->next;
   }

   eclair_playlist_reset_shuffle_list(playlist);

   if (playlist->eclair && playlist->eclair->playlist_container)
      eclair_playlist_container_update(playlist->eclair->playlist_container);
}

//Remove the unselected media files
void eclair_playlist_remove_unselected_media_files(Eclair_Playlist *playlist)
{
   Eina_List *l;
   Eclair_Media_File *media_file;

   if (!playlist)
      return;

   for (l = playlist->playlist; l; )
   {
      if ((media_file = l->data) && !media_file->selected)
         l = eclair_playlist_remove_media_file_list(playlist, l, 0);
      else
         l = l->next;
   }

   eclair_playlist_reset_shuffle_list(playlist);

   if (playlist->eclair && playlist->eclair->playlist_container)
      eclair_playlist_container_update(playlist->eclair->playlist_container);
}

//Empty the playlist and destroy all the media_file
void eclair_playlist_empty(Eclair_Playlist *playlist)
{
   Eina_List *l;

   if (!playlist)
      return;

   for (l = playlist->playlist; l; l = eclair_playlist_remove_media_file_list(playlist, l, 0));
   playlist->playlist = eina_list_free(playlist->playlist);
   playlist->current = NULL;

   eclair_playlist_reset_shuffle_list(playlist);

   if (playlist->eclair && playlist->eclair->playlist_container)
      eclair_playlist_container_update(playlist->eclair->playlist_container);
}

//Set the media file as the active media file  
void eclair_playlist_current_set(Eclair_Playlist *playlist, Eclair_Media_File *media_file)
{
   if (!playlist)
      return;

   if (media_file)
      eclair_playlist_current_set_list(playlist, eina_list_data_find_list(playlist->playlist, media_file));
   else
      eclair_playlist_current_set_list(playlist, NULL);
}

//Set the media file stored in the list as the active media file  
void eclair_playlist_current_set_list(Eclair_Playlist *playlist, Eina_List *list)
{
   Eclair_Media_File *previous_media_file, *new_current_file;

   if (!playlist || !playlist->eclair)
      return;

   previous_media_file = eina_list_data_get(playlist->current);
   if (!playlist->current)
      eclair_playlist_reset_shuffle_list(playlist);
   playlist->current = list;
   eclair_media_file_update(playlist->eclair, previous_media_file);
   if (list)
   {
      eclair_playlist_container_scroll_to_list(playlist->eclair->playlist_container, list);
      eclair_media_file_update(playlist->eclair, list->data);
      if ((new_current_file = list->data) && !new_current_file->shuffle_node && playlist->shuffle)
      {
         playlist->shuffle_list = eina_list_append(playlist->shuffle_list, new_current_file);
         new_current_file->shuffle_node = eina_list_last(playlist->shuffle_list);
      }
   }
} 

//Return the next item to play, shuffled or not
//NULL if no next
Eina_List *eclair_playlist_get_next_list(Eclair_Playlist *playlist)
{
   Eclair_Media_File *current_file, *random_file;
   Eina_List *ramdom_first_list, *l;
   int k;

   if (!playlist || !playlist->current)
      return NULL;

   if (!playlist->shuffle)
   {
      if (playlist->repeat && !playlist->current->next)
         return playlist->playlist;
      else
         return playlist->current->next;
   }
   else if (!(current_file = playlist->current->data))
      return NULL;
   else if (current_file->shuffle_node && current_file->shuffle_node->next)
      return current_file->shuffle_node->next;
   else
   {
      k = eclair_utils_get_random_int(0, eina_list_count(playlist->playlist) - 1);
      ramdom_first_list = eina_list_nth_list(playlist->playlist, k);
      for (l = ramdom_first_list, random_file = NULL; l; )
      {
         if (!(random_file = l->data))
            continue;
         if (!random_file->shuffle_node)
            return l;
         if (l->next)
            l = l->next;
         else
            l = playlist->playlist;

         //No more item to play
         if (l == ramdom_first_list)
         {
            if (!playlist->repeat)
               return NULL;
            else
            {
               eclair_playlist_reset_shuffle_list(playlist);
               return eclair_playlist_get_next_list(playlist);
            }
         }
      }
      return NULL;
   }
}

//Return the previous item to play, shuffled or not
//NULL if no previous item
Eina_List *eclair_playlist_get_prev_list(Eclair_Playlist *playlist)
{
   Eclair_Media_File *current_file;

   if (!playlist || !playlist->current)
      return NULL;

   if (!playlist->shuffle)
   {
      if (playlist->repeat && !playlist->current->prev)
         return eina_list_last(playlist->playlist);
      else
         return playlist->current->prev;
   }
   else if ((current_file = playlist->current->data))
   {
      if (current_file->shuffle_node && current_file->shuffle_node->prev)
         return current_file->shuffle_node->prev;
      else
         return NULL;
   }
   else
      return NULL;
}

int count = 0;

//Set the media file which is just before the active media file as the active media file 
void eclair_playlist_prev_as_current(Eclair_Playlist *playlist)
{
   eclair_playlist_current_set_list(playlist,  eclair_playlist_get_prev_list(playlist));
}

//Set the media file which is just after the active media file as the active media file 
void eclair_playlist_next_as_current(Eclair_Playlist *playlist)
{
   eclair_playlist_current_set_list(playlist, eclair_playlist_get_next_list(playlist));
}
