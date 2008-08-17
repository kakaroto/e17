#include "eclair_media_file.h"
#include <Edje.h>
#include <Ecore_File.h>
#include "eclair.h"
#include "eclair_playlist.h"
#include "eclair_playlist_container.h"
#include "eclair_utils.h"

//Create a new media file
Eclair_Media_File *eclair_media_file_new()
{
   return calloc(1, sizeof(Eclair_Media_File));
}

//Free a media file
void eclair_media_file_free(Eclair_Media_File *media_file)
{
   if (!media_file)
      return;

   free(media_file->path);
   free(media_file->cover_path);
   free(media_file->artist);
   free(media_file->title);
   free(media_file->album);
   free(media_file->genre);
   free(media_file->comment);
   free(media_file);
}

//Update the media file with tag and cover infos
void eclair_media_file_update(Eclair *eclair, Eclair_Media_File *media_file)
{
   char length[10];
   char *artist_title_string;
   const char *filename;
   Eclair_Playlist_Container *playlist_container;
   Eclair_Playlist_Container_Object *container_object;
   Eclair_Color *color;

   if (!eclair)
      return;

   if (media_file)
   {
      //Update playlist entry
      if ((container_object = media_file->container_object) && eclair->playlist_container &&
         (playlist_container = evas_object_smart_data_get(eclair->playlist_container)))
      {   
         if ((artist_title_string = eclair_utils_mediafile_to_artist_title_string(media_file)))
         {
            edje_object_part_text_set(container_object->text, "playlist_entry_name", artist_title_string);
            free(artist_title_string);
         }
         else if (media_file->path && eclair_utils_uri_is_mrl(media_file->path))
            edje_object_part_text_set(container_object->text, "playlist_entry_name", media_file->path);
         else if (media_file->path && (filename = ecore_file_file_get(media_file->path)))
            edje_object_part_text_set(container_object->text, "playlist_entry_name", filename);
         else
            edje_object_part_text_set(container_object->text, "playlist_entry_name", "Media");
   
         if (media_file->length > 0)
         {
            eclair_utils_second_to_string(media_file->length, media_file->length, length);
            edje_object_part_text_set(container_object->text, "playlist_entry_length", length);
         }
         else
            edje_object_part_text_set(container_object->text, "playlist_entry_length", "");
   
         //Set the color of the playlist entry
         if (media_file->selected)
         {
            color = &playlist_container->selected_entry_bg_color;
            evas_object_color_set(container_object->rect, color->r, color->g, color->b, color->a);
            if (media_file == eclair_playlist_current_media_file(&eclair->playlist))
            {
               color = &playlist_container->current_entry_fg_color;
               evas_object_color_set(container_object->text, color->r, color->g, color->b, color->a);
            }
            else
            {
               color = &playlist_container->selected_entry_fg_color;
               evas_object_color_set(container_object->text, color->r, color->g, color->b, color->a);
            }
         }
         else if (media_file == eclair_playlist_current_media_file(&eclair->playlist))
         {
            color = &playlist_container->current_entry_bg_color;
            evas_object_color_set(container_object->rect, color->r, color->g, color->b, color->a);
            color = &playlist_container->current_entry_fg_color;
            evas_object_color_set(container_object->text, color->r, color->g, color->b, color->a);
         }
         else
         {
            color = &playlist_container->normal_entry_bg_color;
            evas_object_color_set(container_object->rect, color->r, color->g, color->b, color->a);
            color = &playlist_container->normal_entry_fg_color;
            evas_object_color_set(container_object->text, color->r, color->g, color->b, color->a);
         }
      }
   }

   //If the media file is the current, we also update gui infos
   if (media_file == eclair_playlist_current_media_file(&eclair->playlist))
      eclair_update_current_file_info(eclair, 0);
}

//Set the value of a meta tag field string of the media file
void eclair_media_file_set_field_string(char **field, const char *value)
{
   const char *c1;
   char *c2;

   if (!field)
      return;

   free(*field);
   if (!value)
      *field = NULL;
   else
   {
      //We remove the blanks before and after the meta tag value
      for (c1 = value; *c1 != 0 && *c1 <= 32; c1++);
      *field = strdup(c1);
      for (c2 = *field + strlen(*field); c2 >= *field && *c2 <= 32; c2--)
         *c2 = 0;
   }
}
