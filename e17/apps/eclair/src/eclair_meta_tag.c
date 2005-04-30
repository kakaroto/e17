#include "eclair_meta_tag.h"
#include <tag_c.h>
#include <string.h>
#include <Evas.h>
#include "eclair.h"
#include "eclair_media_file.h"

static void *_eclair_meta_tag_thread(void *param);

//Initialize meta tag manager
void eclair_meta_tag_init(Eclair_Meta_Tag_Manager *meta_tag_manager, Eclair *eclair)
{
   if (!meta_tag_manager || !eclair)
      return;

   meta_tag_manager->meta_tag_files_to_scan = NULL;
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
   
   printf("Meta tag: Debug: Destroying meta tag thread\n");
   meta_tag_manager->meta_tag_delete_thread = 1;
   pthread_cond_broadcast(&meta_tag_manager->meta_tag_cond); 
   pthread_join(meta_tag_manager->meta_tag_thread, NULL); 
   printf("Meta tag: Debug: Meta tag thread destroyed\n");  
}

//Add a media file to the list of files to scan for meta tag
void eclair_meta_tag_add_file_to_scan(Eclair_Meta_Tag_Manager *meta_tag_manager, Eclair_Media_File *media_file)
{
   if (!meta_tag_manager || !media_file)
      return;
   
   meta_tag_manager->meta_tag_files_to_scan = evas_list_append(meta_tag_manager->meta_tag_files_to_scan, media_file);
   pthread_cond_broadcast(&meta_tag_manager->meta_tag_cond); 
}

//Read the meta tags of media_file and update eclair with the new data
void eclair_meta_tag_read(Eclair *eclair, Eclair_Media_File *media_file)
{
   TagLib_File *tag_file;
   TagLib_Tag *tag;
   const TagLib_AudioProperties *tag_audio_props;

   if (!eclair || !media_file)
      return;

   if (!media_file->path)
      return;
   if (!(tag_file = taglib_file_new(media_file->path)))
      return;

   if ((tag = taglib_file_tag(tag_file)))
   {
      media_file->artist = strdup(taglib_tag_artist(tag));
      media_file->title = strdup(taglib_tag_title(tag));
      media_file->album = strdup(taglib_tag_album(tag));
      media_file->genre = strdup(taglib_tag_genre(tag));
      media_file->comment = strdup(taglib_tag_comment(tag));
      media_file->year = taglib_tag_year(tag);
      media_file->track = taglib_tag_track(tag);
   }   
   if ((tag_audio_props = taglib_file_audioproperties(tag_file)))
      media_file->length = taglib_audioproperties_length(tag_audio_props);
   taglib_file_free(tag_file);
   taglib_tag_free_strings();

   eclair_media_file_update(eclair, media_file);
}

//Scan the files stored in the list of files to scan
static void *_eclair_meta_tag_thread(void *param)
{
   Eclair *eclair = (Eclair *)param;
   Eclair_Meta_Tag_Manager *meta_tag_manager;
   Evas_List *l, *next;
   Eclair_Media_File *current_file;

   if (!eclair)
      return NULL;

   meta_tag_manager = &eclair->meta_tag_manager;
   pthread_mutex_lock(&meta_tag_manager->meta_tag_mutex);
   for (;;)
   {
      pthread_cond_wait(&meta_tag_manager->meta_tag_cond, &meta_tag_manager->meta_tag_mutex);
      while (meta_tag_manager->meta_tag_files_to_scan || meta_tag_manager->meta_tag_delete_thread)
      {
         for (l = meta_tag_manager->meta_tag_files_to_scan; l; l = next)
         {
            next = l->next;
            current_file = (Eclair_Media_File *)l->data;
            meta_tag_manager->meta_tag_files_to_scan = evas_list_remove_list(meta_tag_manager->meta_tag_files_to_scan, l);
            eclair_meta_tag_read(eclair, current_file);
         }

         if (meta_tag_manager->meta_tag_delete_thread)
         {
            meta_tag_manager->meta_tag_files_to_scan = evas_list_free(meta_tag_manager->meta_tag_files_to_scan);
            meta_tag_manager->meta_tag_delete_thread = 0;
            return NULL;
         }
      }
   }
   return NULL;
}
