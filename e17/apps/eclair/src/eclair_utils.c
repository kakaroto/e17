#include "eclair_utils.h"
#include <stdio.h>
#include <string.h>

//Convert the progress rate in seconds to a string depending to the length of the media
void eclair_utils_second_to_string(double position, double length, char *string)
{
   char tmp[4];

   if (length >= 3600)
   {
      sprintf(tmp, "%d:", (int)position / 3600);
      strcat(string, tmp);
      sprintf(tmp, "%.2d:", ((int)position / 60) % 60);
      strcat(string, tmp);
      sprintf(tmp, "%.2d", (int)position % 60);
      strcat(string, tmp);
   }
   else
   {
      if (length >= 600)
         sprintf(tmp, "%.2d:", ((int)position / 60) % 60);
      else
         sprintf(tmp, "%d:", ((int)position / 60) % 60);
      strcat(string, tmp);
      sprintf(tmp, "%.2d", (int)position % 60);
      strcat(string, tmp);
   }
}

//Get filename from the path
const char *eclair_utils_path_to_filename(const char *path)
{
   char *filename;

   if (!path)
      return NULL;

   if ((filename = rindex(path, '/')))
      return &(filename[1]);
   else
      return path;
}

//Return the string "artist - title"
//NULL if the media file doesn't have tag
//This string needs to be freed
char *eclair_utils_mediafile_to_artist_title_string(const Eclair_Playlist_Media_File *media_file)
{
   char *string;

   if (!media_file)
      return NULL;
   if (!media_file->title)
      return NULL;
   if (strlen(media_file->title) <= 0)
      return NULL;

   if (!media_file->artist)
      string = strdup(media_file->title);
   else
   {
      if (strlen(media_file->artist) <= 0)
         string = strdup(media_file->title);
      else
      {
         string = (char *)malloc(strlen(media_file->artist) + strlen(media_file->title) + 4);
         strcpy(string, media_file->artist);
         strcat(string, " - ");
         strcat(string, media_file->title); 
      }
   }

   return string;
}
