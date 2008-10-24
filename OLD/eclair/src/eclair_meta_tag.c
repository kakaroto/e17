#include "eclair_meta_tag.h"
#include <tag_c.h>
#include <string.h>
#include <unistd.h>
#include "eclair.h"
#include "eclair_cover.h"
#include "eclair_media_file.h"
#include "eclair_database.h"

static void *_eclair_meta_tag_thread(void *param);

//Initialize meta tag manager
void eclair_meta_tag_init(Eclair_Meta_Tag_Manager *meta_tag_manager, Eclair *eclair)
{
   if (!meta_tag_manager || !eclair)
      return;

   meta_tag_manager->meta_tag_add_state = ECLAIR_IDLE;
   meta_tag_manager->meta_tag_files_to_add = NULL;
   meta_tag_manager->meta_tag_files_to_scan = NULL;
   meta_tag_manager->meta_tag_should_scan_files = 0;
   meta_tag_manager->meta_tag_delete_thread = 0;
   pthread_cond_init(&meta_tag_manager->meta_tag_cond, NULL);
   pthread_mutex_init(&meta_tag_manager->meta_tag_mutex, NULL);
   pthread_create(&meta_tag_manager->meta_tag_thread, NULL, _eclair_meta_tag_thread, eclair);
}

//Shutdown meta tag manager
void eclair_meta_tag_shutdown(Eclair_Meta_Tag_Manager *meta_tag_manager)
{
   if (!meta_tag_manager)
      return;
   
   fprintf(stderr, "Meta tag: Debug: Destroying meta tag thread\n");
   meta_tag_manager->meta_tag_delete_thread = 1;
   pthread_cond_broadcast(&meta_tag_manager->meta_tag_cond); 
   pthread_join(meta_tag_manager->meta_tag_thread, NULL); 
   fprintf(stderr, "Meta tag: Debug: Meta tag thread destroyed\n");  
}

//Add a media file to the list of files to scan for meta tag
void eclair_meta_tag_add_file_to_scan(Eclair_Meta_Tag_Manager *meta_tag_manager, Eclair_Media_File *media_file)
{
   if (!meta_tag_manager || !media_file || meta_tag_manager->meta_tag_delete_thread)
      return;

   media_file->in_meta_tag_process = 1;
   while (meta_tag_manager->meta_tag_add_state != ECLAIR_IDLE)
      usleep(10000);
   meta_tag_manager->meta_tag_add_state = ECLAIR_ADDING_FILE_TO_ADD;
   meta_tag_manager->meta_tag_files_to_add = eina_list_append(meta_tag_manager->meta_tag_files_to_add, media_file);
   meta_tag_manager->meta_tag_add_state = ECLAIR_IDLE;
   meta_tag_manager->meta_tag_should_scan_files = 1;
   pthread_cond_broadcast(&meta_tag_manager->meta_tag_cond);
}

//Read the meta tags of media_file and update eclair with the new data
void eclair_meta_tag_read(Eclair *eclair, Eclair_Media_File *media_file)
{
   TagLib_File *tag_file;
   TagLib_Tag *tag;
   const TagLib_AudioProperties *tag_audio_props;
   Evas_Bool need_to_update = 0;

   if (!eclair || !media_file || !media_file->path)
      return;

   
   if (!eclair_database_get_meta_infos(&eclair->database, media_file, &need_to_update) || need_to_update)
   {
      if (!(tag_file = taglib_file_new(media_file->path)))
         return;
   
      if ((tag = taglib_file_tag(tag_file)))
      {
         eclair_media_file_set_field_string(&media_file->artist, taglib_tag_artist(tag));
         eclair_media_file_set_field_string(&media_file->title, taglib_tag_title(tag));
         eclair_media_file_set_field_string(&media_file->album, taglib_tag_album(tag));
         eclair_media_file_set_field_string(&media_file->genre, taglib_tag_genre(tag));
         eclair_media_file_set_field_string(&media_file->comment, taglib_tag_comment(tag));
         media_file->year = taglib_tag_year(tag);
         media_file->track = taglib_tag_track(tag);
      }   
      if ((tag_audio_props = taglib_file_audioproperties(tag_file)))
      {
         media_file->length = taglib_audioproperties_length(tag_audio_props);
         media_file->bitrate = taglib_audioproperties_bitrate(tag_audio_props);
         media_file->samplerate = taglib_audioproperties_samplerate(tag_audio_props);
      }
      taglib_tag_free_strings();
      taglib_file_free(tag_file);
   
      //Insert the new tag infos in the database
      eclair_database_insert_media_file(&eclair->database, media_file);
   }

   //Try to load the cover
   if (!media_file->cover_path && !(media_file->cover_path = eclair_cover_file_get_from_local(&eclair->cover_manager, media_file->artist, media_file->album, media_file->path)))
      eclair_cover_add_file_to_treat(&eclair->cover_manager, media_file);
}

//Scan the files stored in the list of files to scan
static void *_eclair_meta_tag_thread(void *param)
{
   Eclair *eclair = param;
   Eclair_Meta_Tag_Manager *meta_tag_manager;
   Eina_List *l, *next;
   Eclair_Media_File *current_file;

   if (!eclair)
      return NULL;

   next = NULL;
   meta_tag_manager = &eclair->meta_tag_manager;
   pthread_mutex_lock(&meta_tag_manager->meta_tag_mutex);
   for (;;)
   {
      if (meta_tag_manager->meta_tag_should_scan_files || meta_tag_manager->meta_tag_delete_thread)
         meta_tag_manager->meta_tag_should_scan_files = 0;
      else
         pthread_cond_wait(&meta_tag_manager->meta_tag_cond, &meta_tag_manager->meta_tag_mutex);
      while (meta_tag_manager->meta_tag_files_to_scan || meta_tag_manager->meta_tag_files_to_add || meta_tag_manager->meta_tag_delete_thread)
      {
         if (meta_tag_manager->meta_tag_delete_thread)
         {
            meta_tag_manager->meta_tag_files_to_scan = eina_list_free(meta_tag_manager->meta_tag_files_to_scan);
            meta_tag_manager->meta_tag_files_to_add = eina_list_free(meta_tag_manager->meta_tag_files_to_add);
            meta_tag_manager->meta_tag_delete_thread = 0;
            return NULL;
         }
         //Add the new files to the list of files to treat
         if (meta_tag_manager->meta_tag_files_to_add)
         {
            while (meta_tag_manager->meta_tag_add_state != ECLAIR_IDLE)
               usleep(10000);
            meta_tag_manager->meta_tag_add_state = ECLAIR_ADDING_FILE_TO_TREAT;
            for (l = meta_tag_manager->meta_tag_files_to_add; l; l = l->next)
               meta_tag_manager->meta_tag_files_to_scan = eina_list_append(meta_tag_manager->meta_tag_files_to_scan, l->data);
            meta_tag_manager->meta_tag_files_to_add = eina_list_free(meta_tag_manager->meta_tag_files_to_add);
            meta_tag_manager->meta_tag_add_state = ECLAIR_IDLE; 
         }
         //Treat the files in the list
         for (l = meta_tag_manager->meta_tag_files_to_scan; l || meta_tag_manager->meta_tag_delete_thread; l = next)
         {
            if (meta_tag_manager->meta_tag_delete_thread || meta_tag_manager->meta_tag_files_to_add)
               break;
            next = l->next;
            current_file = l->data;
            meta_tag_manager->meta_tag_files_to_scan = eina_list_remove_list(meta_tag_manager->meta_tag_files_to_scan, l);
            if (current_file)
            {
               if (current_file->delete_me)
               {
                  current_file->in_meta_tag_process = 0;
                  continue;
               }
               eclair_meta_tag_read(eclair, current_file);
               eclair_media_file_update(eclair, current_file);
               current_file->in_meta_tag_process = 0;
            }
         }
      }
   }
   return NULL;
}
