#include "eclair_media_file.h"
#include <Edje.h>
#include "eclair.h"
#include "eclair_playlist.h"
#include "eclair_cover.h"
#include "eclair_utils.h"

//Create a new media file
Eclair_Media_File *eclair_media_file_new()
{
   return (Eclair_Media_File *)calloc(1, sizeof(Eclair_Media_File));
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
   char length[10] = "";
   char *artist_title_string;
   const char *filename;

   if (!media_file)
      return;

   //Update playlist entry
   if (media_file->playlist_entry)
   {   
      if ((artist_title_string = eclair_utils_mediafile_to_artist_title_string(media_file)))
      {
         edje_object_part_text_set(media_file->playlist_entry, "playlist_entry_name", artist_title_string);
         free(artist_title_string);
      }
      else if ((filename = eclair_utils_path_to_filename(media_file->path)))
         edje_object_part_text_set(media_file->playlist_entry, "playlist_entry_name", filename);
      else
         edje_object_part_text_set(media_file->playlist_entry, "playlist_entry_name", "Media");

      if (media_file->length >= 0)
      {
         eclair_utils_second_to_string(media_file->length, media_file->length, length);
         edje_object_part_text_set(media_file->playlist_entry, "playlist_entry_length", length);
      }
      else
         edje_object_part_text_set(media_file->playlist_entry, "playlist_entry_length", "");
   }

   if (!eclair)
      return;

   //If the media file is the current, we also update gui infos
   if (media_file == eclair_playlist_current_media_file(&eclair->playlist))
      eclair_update_current_file_info(eclair, media_file);
}
