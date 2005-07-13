#ifndef _ECLAIR_UTILS_H_
#define _ECLAIR_UTILS_H_

#include <Evas.h>
#include "eclair_types.h"

void eclair_utils_init();
void eclair_utils_second_to_string(double position, double length, char *string);
char *eclair_utils_mediafile_to_artist_title_string(const Eclair_Media_File *media_file);
char *eclair_utils_add_uri_special_chars(const char *uri);
char *eclair_utils_remove_uri_special_chars(const char *uri);
char *eclair_utils_file_get_extension(char *file);
char *eclair_utils_file_get_filename_without_ext(char *file);
Evas_Bool eclair_utils_uri_is_mrl(const char *uri);
char *eclair_utils_search_file(const char *filename, const char *root_dir);
int eclair_utils_get_random_int(int min, int max);
int eclair_utils_get_random_float(float min, float max);

#endif
