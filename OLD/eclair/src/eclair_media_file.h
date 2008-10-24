#ifndef _ECLAIR_MEDIA_FILE_H_
#define _ECLAIR_MEDIA_FILE_H_

#include <Evas.h>
#include "eclair_types.h"

struct _Eclair_Media_File
{
   //Meta tags
   char *path;
   char *cover_path;
   char *artist;
   char *title;
   char *album;
   char *genre;
   char *comment;
   int length;
   int track;
   int year;
   int bitrate;
   int samplerate;

   Eina_List *shuffle_node;
   Eclair_Playlist_Container_Object *container_object;
   unsigned char selected : 1;
   unsigned char in_meta_tag_process : 1;
   unsigned char in_cover_process : 1;
   unsigned char delete_me : 1;
};

Eclair_Media_File *eclair_media_file_new();
void eclair_media_file_free(Eclair_Media_File *media_file);
void eclair_media_file_update(Eclair *eclair, Eclair_Media_File *media_file);
void eclair_media_file_set_field_string(char **field, const char *value);

#endif
