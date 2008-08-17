#ifndef _ECLAIR_COVER_H_
#define _ECLAIR_COVER_H_

#include <Evas.h>
#include <pthread.h>
#include "eclair_types.h"

struct _Eclair_Cover_Manager
{
   Eclair_Add_File_State cover_add_state;
   Evas_List *cover_files_to_add;
   Evas_List *cover_files_to_treat;
   Evas_List *not_in_amazon_db;

   Evas_Bool cover_should_treat_files;
   Evas_Bool cover_delete_thread;
   pthread_cond_t cover_cond;
   pthread_mutex_t cover_mutex;
   pthread_t cover_thread;

   Eclair *eclair;
};

void eclair_cover_init(Eclair_Cover_Manager *cover_manager, Eclair *eclair);
void eclair_cover_shutdown(Eclair_Cover_Manager *cover_manager);
void eclair_cover_add_file_to_treat(Eclair_Cover_Manager *cover_manager, Eclair_Media_File *media_file);
char *eclair_cover_file_get(Eclair_Cover_Manager *cover_manager, const char *artist, const char *album, char *file_path);
char *eclair_cover_file_get_from_local(Eclair_Cover_Manager *cover_manager, const char *artist, const char *album, char *file_path);
char *eclair_cover_file_get_from_amazon(Eclair_Cover_Manager *cover_manager, const char *artist, const char *album);
void eclair_cover_current_set(Eclair_Cover_Manager *cover_manager, const char *uri);

#endif
