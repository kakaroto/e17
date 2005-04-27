#ifndef _ECLAIR_COVER_H_
#define _ECLAIR_COVER_H_

#include "eclair_private.h"

void eclair_cover_init();
void eclair_cover_shutdown();
char *eclair_cover_file_get(Eclair *eclair, const char *artist, const char *album);
char *eclair_cover_file_get_from_local(Eclair *eclair, const char *artist, const char *album);
char *eclair_cover_file_get_from_amazon(Eclair *eclair, const char *artist, const char *album);

#endif
