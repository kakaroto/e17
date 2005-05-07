#include "eclair_utils.h"
#include <string.h>

//Convert the progress rate in seconds to a string depending to the length of the media
void eclair_utils_second_to_string(double position, double length, char *string)
{
   if (length >= 3600)
      sprintf(string, "%d:%.2d:%.2d", (int)position / 3600, ((int)position / 60) % 60, (int)position % 60);
   else
   {
      if (length >= 600)
         sprintf(string, "%.2d:%.2d", ((int)position / 60) % 60, (int)position % 60);
      else
         sprintf(string, "%d:%.2d", ((int)position / 60) % 60, (int)position % 60);
   }
}

//Get filename from the path
const char *eclair_utils_path_to_filename(const char *path)
{
   char *filename;

   if (!path)
      return NULL;

   //TODO: check if it's a local file
   if ((filename = rindex(path, '/')))
      return &(filename[1]);
   else
      return path;
}

//Remove uri special chars (e.g. "%20" -> ' ')
//The returned string has to be freed
char *eclair_utils_remove_uri_special_chars(const char *uri)
{
   int uri_length;
   char *clean_uri;
   int i, j;
   char hex_code[3];
   int hex_to_char;

   if (!uri)
      return NULL;

   uri_length = strlen(uri);
   clean_uri = (char *)malloc(strlen(uri) + 1);
   for (i = 0, j = 0; i < uri_length; i++, j++)
   {
      if ((uri[i] == '%') && (i < (uri_length - 2)))
      {
         strncpy(hex_code, &uri[i + 1], 2);
         hex_code[2] = 0;
         if (sscanf(hex_code, "%x", &hex_to_char) == 1)
         {
            clean_uri[j] = hex_to_char;
            i += 2;
         }
         else
            clean_uri[j] = uri[i];
      }
      else
         clean_uri[j] = uri[i];
   }
   clean_uri[j] = 0;

   return clean_uri;
}

//Return the string "artist - title"
//NULL if the media file doesn't have tag
//This string has to be freed
char *eclair_utils_mediafile_to_artist_title_string(const Eclair_Media_File *media_file)
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
         sprintf(string, "%s - %s", media_file->artist, media_file->title);
      }
   }

   return string;
}
