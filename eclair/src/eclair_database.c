#include "eclair_database.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "eclair.h"
#include "eclair_media_file.h"

//Init the eclair database (eventually create the db, then open it)
Evas_Bool eclair_database_init(Eclair_Database *database, Eclair *eclair)
{
   char *error_msg;
   int result;

   if (!database || !eclair)
      return 0;

   database->eclair = eclair;

   if (sqlite3_open(eclair->config.database_path, &database->db) != 0)
   {
      fprintf(stderr, "Database: Can't open the database \"%s\": %s\n", eclair->config.database_path, sqlite3_errmsg(database->db));
      return 0;
   }
   result = sqlite3_exec(database->db,
      "CREATE TABLE media_files("
      "path TEXT PRIMARY KEY,"
      "title TEXT,"
      "artist TEXT,"
      "album TEXT,"
      "genre TEXT,"
      "comment TEXT,"
      "track INTEGER,"
      "year INTEGER,"
      "length INTEGER,"
      "modification_time INTEGER)",
   NULL, NULL, &error_msg);
   if (result != SQLITE_OK)
   {
      fprintf(stderr, "Database: Unable to create \"media_files\" table: %s\n", error_msg);
      sqlite3_free(error_msg);
      return 0;
   }

   //TODO: alter table to automatically update when there is some changes?!

   return 1;
}

//Insert media file infos in the database
Evas_Bool eclair_database_insert_media_file(Eclair_Database *database, Eclair_Media_File *media_file)
{
   char *error_msg;
   int result;
   char *query;
   struct stat file_stat;
   time_t modification_time;

   if (!database || !media_file || !media_file->path)
      return 0;

   if (stat(media_file->path, &file_stat) == 0)
      modification_time = file_stat.st_mtime;
   else
      modification_time = 0;

   query = sqlite3_mprintf("REPLACE INTO media_files(path, title, artist, album, genre, comment, track, year, length, modification_time) "
      "VALUES('%q', '%q', '%q', '%q', '%q', '%q', %d, %d, %d, %d)",
      media_file->path,
      media_file->title ? media_file->title : "",
      media_file->artist ? media_file->artist : "",
      media_file->album ? media_file->album : "",
      media_file->genre ? media_file->genre : "",
      media_file->comment ? media_file->comment : "",
      media_file->track,
      media_file->year,
      media_file->length,
      modification_time);
   result = sqlite3_exec(database->db, query, NULL, NULL, &error_msg);
   if (result != SQLITE_OK)
   {
      fprintf(stderr, "Database: Unable to insert data into \"media_files\" table: %s\n", error_msg);
      sqlite3_free(error_msg);
      sqlite3_free(query);
      return 0;
   }
   sqlite3_free(query);
   
   return 1;
}

//Search a file from its path (media_file->path should not be NULL)
//And if the file is found, fill the meta infos if the media_file
//need_to_update will be set to 1 if the file in the database is older than the file on the disk
//Return 1 if the media file has been found in the database
Evas_Bool eclair_database_get_meta_infos(Eclair_Database *database, Eclair_Media_File *media_file, Evas_Bool *need_to_update)
{
   int result;
   char *error_msg;
   char *query;
   char **table_result;
   int nrows, ncols;
   struct stat file_stat;

   if (!database || !media_file || !media_file->path)
      return 0;

   query = sqlite3_mprintf("SELECT * FROM media_files WHERE path='%q'", media_file->path);
   result = sqlite3_get_table(database->db, query, &table_result, &nrows, &ncols, &error_msg);
   if (result != SQLITE_OK)
   {
      fprintf(stderr, "Database: Unable to select data from \"media_files\" table: %s\n", error_msg);
      sqlite3_free(error_msg);
      sqlite3_free(query);
      return 0;
   }
   sqlite3_free(query);

   if (nrows <= 0 || ncols < 10)
   {
      sqlite3_free_table(table_result);
      return 0;
   }

   eclair_media_file_set_field_string(&media_file->title, table_result[ncols + 1]);
   eclair_media_file_set_field_string(&media_file->artist, table_result[ncols + 2]);
   eclair_media_file_set_field_string(&media_file->album, table_result[ncols + 3]);
   eclair_media_file_set_field_string(&media_file->genre, table_result[ncols + 4]);
   eclair_media_file_set_field_string(&media_file->comment, table_result[ncols + 5]);
   media_file->track = atoi(table_result[ncols + 6]);
   media_file->year = atoi(table_result[ncols + 7]);
   media_file->length = atoi(table_result[ncols + 8]);

   if (need_to_update)
      *need_to_update = (stat(media_file->path, &file_stat) == 0 && atoi(table_result[ncols + 9]) != (int)file_stat.st_mtime);

   sqlite3_free_table(table_result);

   return 1;
}

//Search files from keywords
//Store the result in table_result
//Return 1 if succes. Result have to be freed by calling eclair_database_free_result()
Evas_Bool eclair_database_search(Eclair_Database *database, const char *keyword, char ***table_result, int *nrows, int *ncols)
{
   int result;
   char *error_msg;
   char *query;

   if (!database || !keyword || !table_result || !nrows || !ncols)
      return 0;

   query = sqlite3_mprintf("SELECT * FROM media_files WHERE artist LIKE '%%%q%%' or title LIKE '%%%q%%' or album LIKE '%%%q%%'", keyword, keyword, keyword);
   result = sqlite3_get_table(database->db, query, table_result, nrows, ncols, &error_msg);
   if (result != SQLITE_OK)
   {
      fprintf(stderr, "Database: Unable to select data from \"media_files\" table: %s\n", error_msg);
      sqlite3_free(error_msg);
      sqlite3_free(query);
      return 0;
   }
   sqlite3_free(query);

   return 1;
}

//Free the result returned by eclair_database_search
void eclair_database_free_result(char **table_result)
{
   sqlite3_free_table(table_result);
}

//Close the database
void eclair_database_shutdown(Eclair_Database *database)
{
   if (!database)
      return;

   sqlite3_close(database->db);
}
