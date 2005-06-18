#ifndef _ECLAIR_MEDIA_FILE_H_
#define _ECLAIR_MEDIA_FILE_H_

#include "eclair_private.h"

Eclair_Media_File *eclair_media_file_new();
void eclair_media_file_free(Eclair_Media_File *media_file);
void eclair_media_file_update(Eclair *eclair, Eclair_Media_File *media_file);
void eclair_media_file_set_field_string(char **field, const char *value);

#endif
