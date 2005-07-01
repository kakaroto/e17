#ifndef _ECLAIR_DATABASE_H_
#define _ECLAIR_DATABASE_H_

#include "eclair_private.h"

Evas_Bool eclair_database_init(Eclair_Database *database, Eclair *eclair);
void eclair_database_shutdown(Eclair_Database *database);
Evas_Bool eclair_database_insert_media_file(Eclair_Database *database, Eclair_Media_File *media_file);
Evas_Bool eclair_database_search(Eclair_Database *database, Eclair_Media_File *media_file, Evas_Bool *need_to_update);
Evas_Bool eclair_database_search2(Eclair_Database *database, const char *keyword);

#endif
