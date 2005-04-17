#ifndef _ECLAIR_UTILS_H_
#define _ECLAIR_UTILS_H_

#include "eclair_private.h"

void eclair_utils_second_to_string(double position, double length, char *string);
const char *eclair_utils_path_to_filename(const char *path);
char *eclair_utils_mediafile_to_artist_title_string(const Eclair_Playlist_Media_File *media_file);

#endif
