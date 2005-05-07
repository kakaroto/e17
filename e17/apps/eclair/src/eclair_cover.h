#ifndef _ECLAIR_COVER_H_
#define _ECLAIR_COVER_H_

#include "eclair_private.h"

void eclair_cover_init(Eclair_Cover_Manager *cover_manager, Eclair *eclair);
void eclair_cover_shutdown(Eclair_Cover_Manager *cover_manager);
void eclair_cover_add_file_to_treat(Eclair_Cover_Manager *cover_manager, Eclair_Media_File *media_file);
char *eclair_cover_file_get(Eclair_Cover_Manager *cover_manager, const char *artist, const char *album, const char *file_path);
char *eclair_cover_file_get_from_local(Eclair_Cover_Manager *cover_manager, const char *artist, const char *album, const char *file_path);
char *eclair_cover_file_get_from_amazon(Eclair_Cover_Manager *cover_manager, const char *artist, const char *album);
void eclair_cover_current_set(Eclair_Cover_Manager *cover_manager, const char *uri);

#endif
