#ifndef _ECLAIR_META_TAG_H_
#define _ECLAIR_META_TAG_H_

#include <Evas.h>
#include <pthread.h>
#include "eclair_types.h"

struct _Eclair_Meta_Tag_Manager
{
   Eclair_Add_File_State meta_tag_add_state;
   Evas_List *meta_tag_files_to_add;
   Evas_List *meta_tag_files_to_scan;

   Evas_Bool meta_tag_should_scan_files;
   Evas_Bool meta_tag_delete_thread;
   pthread_cond_t meta_tag_cond;
   pthread_mutex_t meta_tag_mutex;
   pthread_t meta_tag_thread;
};

void eclair_meta_tag_init(Eclair_Meta_Tag_Manager *meta_tag_manager, Eclair *eclair);
void eclair_meta_tag_shutdown(Eclair_Meta_Tag_Manager *meta_tag_manager);
void eclair_meta_tag_add_file_to_scan(Eclair_Meta_Tag_Manager *meta_tag_manager, Eclair_Media_File *media_file);
void eclair_meta_tag_read(Eclair *eclair, Eclair_Media_File *media_file);

#endif
