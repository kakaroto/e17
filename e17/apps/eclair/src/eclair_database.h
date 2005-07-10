#ifndef _ECLAIR_DATABASE_H_
#define _ECLAIR_DATABASE_H_

#include <sqlite3.h>
#include <Evas.h>
#include "eclair_types.h"

struct _Eclair_Database
{
   sqlite3 *db;
   Eclair *eclair;
};

Evas_Bool eclair_database_init(Eclair_Database *database, Eclair *eclair);
void eclair_database_shutdown(Eclair_Database *database);
Evas_Bool eclair_database_insert_media_file(Eclair_Database *database, Eclair_Media_File *media_file);
Evas_Bool eclair_database_get_meta_infos(Eclair_Database *database, Eclair_Media_File *media_file, Evas_Bool *need_to_update);
Evas_Bool eclair_database_search(Eclair_Database *database, const char *keyword, char ***table_result, int *nrows, int *ncols);
void eclair_database_free_result(char **table_result);

#endif
