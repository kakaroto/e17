#include "eclair.h"
#include <Ecore_File.h>
#include <string.h>
#include <stdlib.h>
#include "eclair_media_file.h"

//Need to be called for the random generation
void eclair_utils_init()
{
   srand((unsigned int)time(NULL));
}

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

//Remove uri special chars (e.g. "%20" -> ' ')
//The returned string has to be freed
char *eclair_utils_remove_uri_special_chars(const char *uri)
{
   int uri_length;
   char *clean_uri;
   int i, j;
   int hex_to_char;

   if (!uri)
      return NULL;

   uri_length = strlen(uri);
   clean_uri = malloc(strlen(uri) + 1);
   for (i = 0, j = 0; i < uri_length; i++, j++)
   {
      if ((uri[i] == '%') && (i < (uri_length - 2)))
      {
         if (sscanf(&uri[i + 1], "%2x", &hex_to_char) == 1)
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

//Add the uri special chars (e.g. ' ' -> "%20")
//The returned string has to be freed
char *eclair_utils_add_uri_special_chars(const char *uri)
{
   int uri_length;
   char *new_uri;
   int i, j;

   if (!uri)
      return NULL;

   uri_length = strlen(uri);
   new_uri = malloc(uri_length * 3 + 1);

   for (i = 0, j = 0; i < uri_length; i++, j++)
   {
      if (uri[i] == ' ' || uri[i] == ';'  || uri[i] == '&'  || uri[i] == '='  ||
         uri[i] == '<'  || uri[i] == '>'  || uri[i] == '?'  || uri[i] == '@'  ||
         uri[i] == '\"' || uri[i] == '['  || uri[i] == '#'  || uri[i] == '\\' ||
         uri[i] == '$'  || uri[i] == ']'  || uri[i] == '%'  || uri[i] == '^'  ||
         uri[i] == '\'' || uri[i] == '¨'  || uri[i] == '+'  || uri[i] == '{'  ||
         uri[i] == ','  || uri[i] == '|'  || uri[i] == '/'  || uri[i] == '}'  ||
         uri[i] == ':'  || uri[i] == '~')
      {
         new_uri[j] = '%';
         sprintf(&new_uri[j + 1], "%2x", uri[i]);
         j += 2;
      }
      else
         new_uri[j] = uri[i];
   }
   new_uri[j] = 0;

   return new_uri;
}

//Return the string "artist - title"
//NULL if the media file doesn't have tag
//This string has to be freed
char *eclair_utils_mediafile_to_artist_title_string(const Eclair_Media_File *media_file)
{
   char *string;

   if (!media_file || !media_file->title || strlen(media_file->title) <= 0)
      return NULL;

   if (!media_file->artist || strlen(media_file->artist) <= 0)
      string = strdup(media_file->title);
   else
   {
      string = malloc(strlen(media_file->artist) + strlen(media_file->title) + 4);
      sprintf(string, "%s - %s", media_file->artist, media_file->title);
   }

   return string;
}

//Return the extension of the file
//NULL if failed
char *eclair_utils_file_get_extension(char *file)
{
   char *ext;

   if (!file || !(ext = ecore_file_file_get(file)))
      return NULL;

   if ((ext = strrchr(ext, '.')))
   {
      ext++;
      return ext;
   }
   return NULL;
}

//Return the filename of the file without the extension
//The returned string has to be freed
//NULL if failed
char *eclair_utils_file_get_filename_without_ext(char *file)
{
   char *file_without_ext;
   char *ext_start;

   if (!file || !(file_without_ext = ecore_file_file_get(file)))
      return NULL;

   file_without_ext = strdup(file_without_ext);
   if ((ext_start = strrchr(file_without_ext, '.')))
      *ext_start = 0;

   return file_without_ext;
}

//TODO: Do we suppport all the mrl possible?
//Return 1 if the uri is a mrl (e.g. "dvd://", "http://www.domain.com/file.mp3")
Evas_Bool eclair_utils_uri_is_mrl(const char *uri)
{
   if (!uri)
      return 0;

   if (strstr(uri, "://"))
      return 1;
   if (strlen(uri) >= 6 && strncmp(uri, "cdda:/", 6) == 0)
      return 1;

   return 0;
}

//Search for a file in the dir root_dir with the name filename, regardless to the case
//Return NULL if no file found, otherwise, the path of the file
//Returned value has to be freed
char *eclair_utils_search_file(const char *filename, const char *root_dir)
{
   Ecore_List *files;
   char *file;
   char *result = NULL;

   if (!filename || !root_dir || !(files = ecore_file_ls(root_dir)))
      return NULL;

   ecore_list_first_goto(files);
   while ((file = ecore_list_next(files)))
   {
      if (strcasecmp(file, filename) == 0)
      {
         result = malloc(strlen(root_dir) + strlen(file) + 2);
         sprintf(result, "%s/%s", root_dir, file);
         break;
      }
   }
   ecore_list_destroy(files);
   return result;
}

//Return a random integer between min and max
int eclair_utils_get_random_int(int min, int max)
{
   return (int)((float)rand() / RAND_MAX * (max - min) + 0.5f) + min;
}

//Return a random float between min and max
int eclair_utils_get_random_float(float min, float max)
{
   return (float)rand() / RAND_MAX * (max - min) + min;
}
