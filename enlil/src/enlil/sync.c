#include "enlil_private.h"
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

/* FIXME: destroy the mutex */

#ifdef EFL_HAVE_POSIX_THREADS
# define ENLIL_MUTEX pthread_mutex_t
# define ENLIL_MUTEX_INIT(Sync) pthread_mutex_init(&(Sync->mutex), NULL)
# define ENLIL_MUTEX_LOCK(Sync) pthread_mutex_lock(&(Sync->mutex))
# define ENLIL_MUTEX_UNLOCK(Sync) pthread_mutex_unlock(&(Sync->mutex))
#else
# include <windows.h>
# define ENLIL_MUTEX HANDLE
# define ENLIL_MUTEX_INIT(Sync) Sync->mutex = CreateMutex(NULL, FALSE, NULL)
# define ENLIL_MUTEX_LOCK(Sync) WaitForSingleObject(Sync->mutex, INFINITE)
# define ENLIL_MUTEX_UNLOCK(Sync) ReleaseMutex(Sync->mutex)
#endif

int LOG_DOMAIN;

typedef struct Enlil_Sync_Configuration Enlil_Sync_Configuration;

typedef enum Enlil_Sync_Message
{
   Enlil_SYNC_ERROR,
   Enlil_SYNC_ALBUM_NEW,
   Enlil_SYNC_ALBUM_UPDATE,
   Enlil_SYNC_ALBUM_DISAPPEAR,
   Enlil_SYNC_PHOTO_NEW,
   Enlil_SYNC_PHOTO_UPDATE,
   Enlil_SYNC_PHOTO_DISAPPEAR,
   Enlil_SYNC_DONE
}Enlil_Sync_Message;

typedef enum Enlil_Sync_Job_Type
{
   Enlil_SYNC_JOB_ALL,
   Enlil_SYNC_JOB_ALBUM,
   Enlil_SYNC_JOB_PHOTO
} Enlil_Sync_Job_Type;

typedef struct Enlil_Sync_Job
{
   Enlil_Sync_Job_Type type;
   const char *folder;
   const char* file;
} Enlil_Sync_Job;

struct Enlil_Sync_Configuration
{
   void *data;
   Enlil_Sync_Error_Cb error_cb;
   Enlil_Sync_Album_New_Cb album_new_cb;
   Enlil_Sync_Album_Update_Cb album_update_cb;
   Enlil_Sync_Album_Disappear_Cb album_disappear_cb;
   Enlil_Sync_Photo_New_Cb photo_new_cb;
   Enlil_Sync_Photo_Update_Cb photo_update_cb;
   Enlil_Sync_Photo_Disappear_Cb photo_disappear_cb;
   Enlil_Sync_Done_Cb done_cb;
   Enlil_Sync_Start_Cb start_cb;
};

struct enlil_sync
{
   const char* path;
   Enlil_Sync_Configuration sync;

   Eina_List *jobs;
   Enlil_Sync_Job *current_job;

   int is_running;

   // mutex used to pause the thread and wake up
   ENLIL_MUTEX mutex;

   struct {
	//thread send to the main loop
	Ecore_Pipe *thread_main;
   }pipe;

   //message sent by the thread
   struct
     {
	Enlil_Sync_Message type;
	Sync_Error error;
	char *msg;
	Enlil_Library *library;
	Enlil_Album *album;
	Enlil_Photo *photo;
     }msg;

   //extra data used by enlil_sync_album_folder() and enlil_sync_photo_file()
   struct
     {
	const char *file;
	const char *folder;
     } extra;

   double t0;
};



static void _enlil_sync_message_cb(void *data, void *buffer, unsigned int nbyte);
static void _enlil_sync_end_cb(void *data, Ecore_Thread *thread);

static void _enlil_sync_album_folder_thread(void *data, Ecore_Thread *thread);
static void _enlil_sync_photo_file_thread(void *data, Ecore_Thread *thread);

static void _enlil_sync_run(Enlil_Sync *_sync);
static void _enlil_sync_album_folder_run(Enlil_Sync *_sync, const char *file);
static void _enlil_sync_photo_file_run(Enlil_Sync *_sync, const char *folder, const char *file);

//method used to synchronise a photo
static void  _enlil_sync_photo_file_start(Enlil_Sync *_sync, const char *folder, const char *file);

//method used to synchronise an album folder
static void _enlil_sync_album_folder_start(Enlil_Sync *_sync, const char *file);

//methods used to do a global synchronisation
static void _enlil_sync_all_start(void *data, Ecore_Thread *thread);
static void _enlil_sync_all_album_sync(Enlil_Sync *_sync, Enlil_Album *album);
static void _enlil_sync_all_photo_new(Enlil_Sync *_sync, Enlil_Album *album, const char *file);
static int _enlil_sync_all_photo_update(Enlil_Sync *_sync, Enlil_Album *album, Enlil_Photo *photo);
static void _enlil_sync_all_album_new(Enlil_Sync *_sync, Enlil_Library *library_list, const char *file);
static int _enlil_sync_all_album_update(Enlil_Sync *_sync, Enlil_Library *library_list, Enlil_Album *_album);

static void _enlil_sync_next_job_process(Enlil_Sync *_sync);
static void _enlil_sync_job_free(Enlil_Sync_Job **job);
static Enlil_Sync_Job *_enlil_sync_job_equivalent_search(Enlil_Sync *_sync, Enlil_Sync_Job *job);

static int file_album_comp_cb(const void *d1, const void *d2);
static int file_photo_comp_cb(const void *d1, const void *d2);

/**
 * @brief Create a new Synchronisation struct
 * @param path The path of the photo manager folder (which contains the list of albums)
 * @param album_new_cb Callback called when a new album is found
 * @param album_update_cb Callback called when an album has been updated
 * @param album_disappear_cb Callback called when an album has disappear
 *              (defined in the eet file but the folder does not exists)
 * @param photo_new_cb Callback called when a new photo is found
 * @param photo_update_cb Callback called when a photo has been updated
 * @param photo_disappear_cb Callback called when a photo has disappear
 *              (defined in the eet file but the file does not exists)
 * @param done_cb Callback called when a synchronisation is done
 * @param error_cb Callback called when an error occurs
 * @param user_data Data sent in the callbacks
 * @return Returns the sync struct
 */
Enlil_Sync *enlil_sync_new(const char *path,
      Enlil_Sync_Album_New_Cb album_new_cb,
      Enlil_Sync_Album_Update_Cb album_update_cb,
      Enlil_Sync_Album_Disappear_Cb album_disappear_cb,
      Enlil_Sync_Photo_New_Cb photo_new_cb,
      Enlil_Sync_Photo_Update_Cb photo_update_cb,
      Enlil_Sync_Photo_Disappear_Cb photo_disappear_cb,
      Enlil_Sync_Done_Cb done_cb,
      Enlil_Sync_Start_Cb start_cb,
      Enlil_Sync_Error_Cb error_cb,
      void *user_data)
{
   ASSERT_RETURN(path!=NULL);
   ASSERT_RETURN(album_new_cb!=NULL);
   ASSERT_RETURN(album_update_cb!=NULL);
   ASSERT_RETURN(album_disappear_cb!=NULL);
   ASSERT_RETURN(photo_new_cb!=NULL);
   ASSERT_RETURN(photo_update_cb!=NULL);
   ASSERT_RETURN(photo_disappear_cb!=NULL);
   ASSERT_RETURN(done_cb!=NULL);
   ASSERT_RETURN(start_cb!=NULL);
   ASSERT_RETURN(error_cb!=NULL);

   Enlil_Sync *_sync = calloc(1, sizeof(Enlil_Sync));
   ASSERT_RETURN(_sync!=NULL);

   _sync->path = eina_stringshare_add(path);
   _sync->sync.album_new_cb = album_new_cb;
   _sync->sync.album_update_cb = album_update_cb;
   _sync->sync.album_disappear_cb = album_disappear_cb;
   _sync->sync.photo_new_cb = photo_new_cb;
   _sync->sync.photo_update_cb = photo_update_cb;
   _sync->sync.photo_disappear_cb = photo_disappear_cb;
   _sync->sync.done_cb = done_cb;
   _sync->sync.start_cb = start_cb;
   _sync->sync.error_cb = error_cb;
   _sync->sync.data = user_data;

   ENLIL_MUTEX_INIT(_sync);
   ENLIL_MUTEX_LOCK(_sync);

   _sync->pipe.thread_main = ecore_pipe_add(_enlil_sync_message_cb, _sync);
   ASSERT_CUSTOM_RET(_sync->pipe.thread_main != NULL, enlil_sync_free(&_sync); return NULL;);

   return _sync;
}

/**
 * @brief Free a synchronisation struct
 * @param sync the sync struct
 */
void enlil_sync_free(Enlil_Sync **_sync)
{
   ASSERT_RETURN_VOID(_sync != NULL);
   Enlil_Sync *__sync = *_sync;
   ASSERT_RETURN_VOID(__sync!=NULL);

   if(__sync->is_running)
     {
	LOG_ERR("You tried to free a sync structure while the thread was running, this is really bad and can't be done\n");
	return;
     }

   eina_stringshare_del(__sync->path);

   if(__sync->pipe.thread_main)
     ecore_pipe_del(__sync->pipe.thread_main);

   FREE(__sync);
}

int enlil_sync_jobs_count_get(Enlil_Sync *_sync)
{
   ASSERT_RETURN(_sync != NULL);

   return eina_list_count(_sync->jobs);
}

/**
 * @brief Add a new job : Synchronise all the albums and photos
 * @param sync the sync struct
 */
void enlil_sync_job_all_add(Enlil_Sync *_sync)
{
   Enlil_Sync_Job *job, *_job;

   ASSERT_RETURN_VOID(_sync != NULL);

   job = calloc(1, sizeof(Enlil_Sync_Job));
   job->type = Enlil_SYNC_JOB_ALL;

   _job = _enlil_sync_job_equivalent_search(_sync, job);
   if(!_job)
     _sync->jobs = eina_list_append(_sync->jobs, job);
   else
     {
	_enlil_sync_job_free(&job);
	return ;
     }

   if(!_sync->is_running)
     _enlil_sync_next_job_process(_sync);
}

/**
 * @brief Add a new job : Synchronise a album and its photos
 * @brief sync the sync struct
 * @brief folder the album folder name
 */
void enlil_sync_job_album_folder_add(Enlil_Sync *_sync, const char *folder)
{
   Enlil_Sync_Job *job, *_job;

   ASSERT_RETURN_VOID(_sync != NULL);
   ASSERT_RETURN_VOID(folder != NULL);

   job = calloc(1, sizeof(Enlil_Sync_Job));
   job->type = Enlil_SYNC_JOB_ALBUM;
   job->folder = eina_stringshare_add(folder);

   _job = _enlil_sync_job_equivalent_search(_sync, job);
   if(!_job)
     _sync->jobs = eina_list_append(_sync->jobs, job);
   else
     {
	_enlil_sync_job_free(&job);
	return ;
     }

   if(!_sync->is_running)
     _enlil_sync_next_job_process(_sync);
}

/**
 * @brief Add a new job : Synchronise a photo
 * @param sync the sync struct
 * @param folder the album folder name
 * @brief file the photo file name
 */
void enlil_sync_job_photo_file_add(Enlil_Sync *_sync, const char *folder, const char *file)
{
   Enlil_Sync_Job *job, *_job;

   ASSERT_RETURN_VOID(_sync != NULL);
   ASSERT_RETURN_VOID(file != NULL);

   job = calloc(1, sizeof(Enlil_Sync_Job));
   job->type = Enlil_SYNC_JOB_PHOTO;
   job->folder = eina_stringshare_add(folder);
   job->file = eina_stringshare_add(file);

   _job = _enlil_sync_job_equivalent_search(_sync, job);
   if(!_job)
     _sync->jobs = eina_list_append(_sync->jobs, job);
   else
     {
	_enlil_sync_job_free(&job);
	return ;
     }

   if(!_sync->is_running)
     _enlil_sync_next_job_process(_sync);
}

static Enlil_Sync_Job *_enlil_sync_job_equivalent_search(Enlil_Sync *_sync, Enlil_Sync_Job *job)
{
   Eina_List *l;
   Enlil_Sync_Job *_job = NULL;

   ASSERT_RETURN(_sync != NULL);
   ASSERT_RETURN(job != NULL);

   EINA_LIST_FOREACH(_sync->jobs, l, _job)
     {
	if(_job->type == job->type
	      && _job->folder == job->folder
	      && _job->file == job->file )
	  break;
     }

   return _job;
}

static void _enlil_sync_job_free(Enlil_Sync_Job **job)
{
   ASSERT_RETURN_VOID(job != NULL);
   Enlil_Sync_Job *_job = * job;
   ASSERT_RETURN_VOID(_job!=NULL);
   switch(_job->type)
     {
      case Enlil_SYNC_JOB_ALBUM:
	 eina_stringshare_del(_job->folder);
	 break;
      case Enlil_SYNC_JOB_PHOTO:
	 eina_stringshare_del(_job->file);
	 eina_stringshare_del(_job->folder);
	 break;
      default: ;
     }
   FREE(_job);
}

static void _enlil_sync_next_job_process(Enlil_Sync *_sync)
{
   Enlil_Sync_Job *job;

   ASSERT_RETURN_VOID(_sync != NULL);

   if(!_sync->jobs)
     {
	_sync->is_running = 0;
	return ;
     }

   _sync->is_running = 1;

   job = eina_list_data_get(_sync->jobs);
   _sync->jobs = eina_list_remove(_sync->jobs, job);
   _sync->current_job = job;
   switch(job->type)
     {
      case Enlil_SYNC_JOB_ALL:
	 _enlil_sync_run(_sync);
	 break;
      case Enlil_SYNC_JOB_ALBUM:
	 _enlil_sync_album_folder_run(_sync, job->folder);
	 break;
      case Enlil_SYNC_JOB_PHOTO:
	 _enlil_sync_photo_file_run(_sync, job->folder, job->file);
     }
   _sync->sync.start_cb(_sync->sync.data, _sync);
}

static void _enlil_sync_run(Enlil_Sync *_sync)
{
   ASSERT_RETURN_VOID(_sync!=NULL);

   ecore_thread_run(_enlil_sync_all_start, _enlil_sync_end_cb, NULL, _sync);
   LOG_INFO("Synchronisation start on the library : %s", _sync->path);
   _sync->t0 = ecore_time_get();
}

static void _enlil_sync_album_folder_run(Enlil_Sync *_sync, const char *folder)
{
   ASSERT_RETURN_VOID(_sync!=NULL);
   ASSERT_RETURN_VOID(folder!=NULL);

   _sync->extra.folder = folder;
   ecore_thread_run(_enlil_sync_album_folder_thread, _enlil_sync_end_cb, NULL, _sync);
   LOG_INFO("Synchronisation start on album : %s", folder);
   _sync->t0 = ecore_time_get();
}

static void _enlil_sync_album_folder_thread(void *data, __UNUSED__ Ecore_Thread *thread)
{
   Enlil_Sync *_sync = (Enlil_Sync *) data;
   _enlil_sync_album_folder_start(_sync, _sync->extra.folder);

   _sync->msg.type = Enlil_SYNC_DONE;
   ecore_pipe_write(_sync->pipe.thread_main, "a", 1);
}

static void _enlil_sync_photo_file_run(Enlil_Sync *_sync, const char *folder, const char *file)
{
   ASSERT_RETURN_VOID(_sync!=NULL);
   ASSERT_RETURN_VOID(file!=NULL);

   _sync->extra.file = file;
   _sync->extra.folder = folder;
   ecore_thread_run(_enlil_sync_photo_file_thread, _enlil_sync_end_cb, NULL, _sync);
   LOG_INFO("Synchronisation start on photo : %s", file);
   _sync->t0 = ecore_time_get();
}

static void _enlil_sync_photo_file_thread(void *data, __UNUSED__ Ecore_Thread *thread)
{
   Enlil_Sync *_sync = (Enlil_Sync *) data;
   _enlil_sync_photo_file_start(_sync, _sync->extra.folder, _sync->extra.file);

   _sync->msg.type = Enlil_SYNC_DONE;
   ecore_pipe_write(_sync->pipe.thread_main, "a", 1);
}

/**
 * @brief Synchronise a specific photo file
 * @param sync the sync struct
 * @param album the album which contains the file
 * @param file the photo file name
 */
static void _enlil_sync_photo_file_start(Enlil_Sync *_sync, const char *folder, const char *file)
{
   char buf_path[PATH_MAX], buf_file[PATH_MAX], buf_key[PATH_MAX], buf[PATH_MAX],
	buf_eet[PATH_MAX];
   time_t _time;
   int size;
   Enlil_Photo *photo = NULL, *photo_list;
   int ret;
   int not_delete_photo = 0;
   int new_photo = 0;
   int file_exist;
   Enlil_Album *album;
   Enlil_Library *library;

#define SAVE() \
   do { \
	ret = enlil_photo_eet_save(photo); \
	if(!ret) \
	  { \
	     snprintf(buf, 1024, "Failed to save the photo \"%s\" in the file %s/%s/"EET_FILE, enlil_photo_name_get(photo), _sync->path, enlil_album_file_name_get(album)); \
	     _sync->msg.type = Enlil_SYNC_ERROR; \
	     _sync->msg.error = Sync_Error_Eet_Save_Failed; \
	     _sync->msg.msg = buf; \
	     ecore_pipe_write(_sync->pipe.thread_main, "a", 1); \
	     ENLIL_MUTEX_LOCK(_sync); \
	  } \
   }while(0)

   ASSERT_RETURN_VOID(folder != NULL);
   ASSERT_RETURN_VOID(file != NULL);

   library = enlil_library_new(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
   enlil_library_path_set(library, _sync->path);

   album = enlil_library_eet_album_load(library, folder);

   snprintf(buf_path,PATH_MAX,"%s/%s",enlil_album_path_get(album),enlil_album_file_name_get(album));
   snprintf(buf_eet,PATH_MAX,"%s/%s/"EET_FILE,enlil_album_path_get(album),enlil_album_file_name_get(album));
   snprintf(buf_file, PATH_MAX, "%s/%s", buf_path, file);
   snprintf(buf_key, PATH_MAX, "/photo %s", file);


   photo_list = enlil_album_photo_search_file_name(album, file);

   //load the photo from the eet file
   if(photo_list)
     photo = enlil_photo_eet_load(buf_eet, buf_key);
   //test if the file exits
   file_exist = ecore_file_exists(buf_file);
   if(file_exist)
     {
	FILE_INFO_GET(buf_file, _time, size);
     }

   if(file_exist && !photo)
     {
	//new photo
	new_photo = 1;
	photo = enlil_photo_new();
	char *name =  ecore_file_strip_ext(file);
	enlil_photo_name_set(photo, name);
	FREE(name);
	enlil_photo_file_name_set(photo, file);
	enlil_photo__time_set(photo, _time);
	enlil_photo_size_set(photo, size);
	enlil_photo_path_set(photo, buf_path);

	if(enlil_photo_is(buf_file))
	  enlil_photo_type_set(photo, ENLIL_PHOTO_TYPE_PHOTO);
	else if(enlil_video_is(buf_file))
	  enlil_photo_type_set(photo, ENLIL_PHOTO_TYPE_VIDEO);
	else if(enlil_gpx_is(buf_file))
	  enlil_photo_type_set(photo, ENLIL_PHOTO_TYPE_GPX);

	SAVE();

	_sync->msg.type = Enlil_SYNC_PHOTO_NEW;
	_sync->msg.album = album;
	_sync->msg.photo = photo;
	ecore_pipe_write(_sync->pipe.thread_main, "a", 1);
	ENLIL_MUTEX_LOCK(_sync);

	//update the list of album
	if(photo_list)
	  {
	     enlil_album_photo_remove(album, photo_list);
	     enlil_photo_free(&photo_list);
	  }
	enlil_album_photo_add(album, photo);
	enlil_album_eet_photos_list_save(album);

	not_delete_photo = 1;
     }
   else if(file_exist && photo && _time > enlil_photo__time_get(photo))
     {
	//update
	enlil_photo__time_set(photo, _time);
	enlil_photo_size_set(photo, size);

	SAVE();

	_sync->msg.type = Enlil_SYNC_PHOTO_UPDATE;
	_sync->msg.album = album;
	_sync->msg.photo = photo;
	ecore_pipe_write(_sync->pipe.thread_main, "a", 1);
	ENLIL_MUTEX_LOCK(_sync);
     }
   else if(!file_exist && photo)
     {
	//photo disappear
	_sync->msg.type = Enlil_SYNC_PHOTO_DISAPPEAR;
	_sync->msg.album = album;
	_sync->msg.photo = photo;
	ecore_pipe_write(_sync->pipe.thread_main, "a", 1);
	ENLIL_MUTEX_LOCK(_sync);

	Enlil_Photo *photo_list_file = enlil_album_photo_search_file_name(album, file);
	if(photo_list_file)
	  {
	     enlil_album_photo_remove(album, photo_list_file);
	     enlil_album_eet_photos_list_save(album);
	     enlil_photo_free(&photo_list_file);
	  }

	enlil_photo_eet_remove(buf_eet, enlil_photo_file_name_get(photo));
     }

   if(!not_delete_photo && photo)
     enlil_photo_free(&photo);
   enlil_album_free(&album);
   enlil_library_free(&library);
#undef SAVE
}




/**
 * @brief Synchronise a specific album folder
 * @param sync the sync struct
 * @param folder the album folder name
 */
static void _enlil_sync_album_folder_start(Enlil_Sync *_sync, const char *folder)
{
   char buf_album[PATH_MAX], buf[PATH_MAX], buf2[PATH_MAX];
   int ret;
   int folder_exist;
   Enlil_Album *album = NULL, *album_list;
   time_t _time;
   int size;

#define SAVE() \
   do { \
	ret = enlil_album_eet_header_save(album); \
	if(!ret) \
	  { \
	     snprintf(buf, 1024, "Failed to save the album \"%s\" in the file %s/"EET_FILE, enlil_album_name_get(album), _sync->path); \
	     _sync->msg.type = Enlil_SYNC_ERROR; \
	     _sync->msg.error = Sync_Error_Eet_Save_Failed; \
	     _sync->msg.msg = buf; \
	     ecore_pipe_write(_sync->pipe.thread_main, "a", 1); \
	     ENLIL_MUTEX_LOCK(_sync); \
	  } \
   }while(0);

   Enlil_Library *library = enlil_library_new(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
   enlil_library_path_set(library, _sync->path);

   Enlil_Library *library_list = enlil_library_eet_albums_load(library);
   if(!library_list)
     library_list = enlil_library_new(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
   enlil_library_path_set(library_list, _sync->path);

   album_list = enlil_library_album_search_file_name(library_list, folder);

   snprintf(buf_album, PATH_MAX, "%s/%s", enlil_library_path_get(library), folder);
   //load the album from the eet file
   if(album_list)
     album = enlil_library_eet_album_load(library, folder);

   //test if the folder exists
   folder_exist = ecore_file_exists(buf_album);
   //

   if(folder_exist)
     FILE_INFO_GET(buf_album, _time, size);

   if(!album && folder_exist)
     {
	//if an eet file is in the album we delete it
	snprintf(buf2, PATH_MAX, "%s/"EET_FILE, buf_album);
	remove(buf2);

	//create the new album
	album = enlil_album_new();
	enlil_album_path_set(album, enlil_library_path_get(library));
	enlil_album_file_name_set(album, folder);
	enlil_album_name_set(album, folder);
	enlil_album__time_set(album, _time);

	SAVE();

	//send notif new album

	_sync->msg.type = Enlil_SYNC_ALBUM_NEW;
	_sync->msg.library = library;
	_sync->msg.album = album;
	ecore_pipe_write(_sync->pipe.thread_main, "a", 1);
	ENLIL_MUTEX_LOCK(_sync);


	//update the list of album
	if(album_list)
	  {
	    enlil_library_album_remove(library_list, album_list);
	    enlil_album_free(&album_list);
	  }

	Enlil_Album *album_list2 = enlil_album_new();
	enlil_album_path_set(album_list2, enlil_album_path_get(album));
	enlil_album_file_name_set(album_list2, enlil_album_file_name_get(album));
	enlil_album_name_set(album_list2, enlil_album_name_get(album));
	enlil_library_album_add(library_list, album_list2);
	enlil_library_eet_albums_save(library_list);
	enlil_library_eet_collections_save(library_list);
	enlil_library_eet_tags_save(library_list);
     }
   /*else if(album && folder_exist && time > enlil_album__time_get(album))
     {
	enlil_album__time_set(album, time);
	SAVE();

	//send notif update album
	sync->msg.type = Enlil_SYNC_ALBUM_UPDATE;
	sync->msg.library = library;
	sync->msg.album = album;
	ecore_pipe_write(sync->pipe.thread_main, "a", 1);
	ENLIL_MUTEX_LOCK(sync);
     }
   */
   else if(album_list && !folder_exist)
     {
	//the album is referenced in the eet file but the folder does not exists

	if(album)
	  enlil_album_free(&album);

	//send notif
	_sync->msg.type = Enlil_SYNC_ALBUM_DISAPPEAR;
	_sync->msg.library = library;
	_sync->msg.album = album_list;
	ecore_pipe_write(_sync->pipe.thread_main, "a", 1);
	ENLIL_MUTEX_LOCK(_sync);

	//update the list of album
	enlil_library_album_remove(library_list, album_list);
	enlil_library_eet_albums_save(library_list);
	enlil_library_eet_collections_save(library_list);
	enlil_library_eet_tags_save(library_list);
     }

   if(album)
     _enlil_sync_all_album_sync(_sync, album);

   if(album)
     enlil_album_free(&album);
   if(library_list)
     enlil_library_free(&library_list);
   enlil_library_free(&library);
#undef SAVE
}






static void _enlil_sync_all_photo_new(Enlil_Sync *_sync, Enlil_Album *album, const char *file)
{
   char buf_path[PATH_MAX], buf_file[PATH_MAX], buf[PATH_MAX], buf2[PATH_MAX];
   time_t _time;
   int size;
   Enlil_Photo *photo;
   int ret;

#define SAVE() \
   do { \
	ret = enlil_photo_eet_save(photo); \
	if(!ret) \
	  { \
	     snprintf(buf, 1024, "Failed to save the photo \"%s\" in the file %s/%s/"EET_FILE, enlil_photo_name_get(photo), _sync->path, enlil_album_file_name_get(album)); \
	     _sync->msg.type = Enlil_SYNC_ERROR; \
	     _sync->msg.error = Sync_Error_Eet_Save_Failed; \
	     _sync->msg.msg = buf; \
	     ecore_pipe_write(_sync->pipe.thread_main, "a", 1); \
	     ENLIL_MUTEX_LOCK(_sync); \
	  } \
   }while (0);

   ASSERT_RETURN_VOID(album != NULL);
   ASSERT_RETURN_VOID(file != NULL);

   snprintf(buf_path,PATH_MAX,"%s/%s",enlil_album_path_get(album),enlil_album_file_name_get(album));
   snprintf(buf_file, PATH_MAX, "%s/%s", buf_path, file);
   snprintf(buf,PATH_MAX,"%s/"EET_FILE,buf_path);
   snprintf(buf2, PATH_MAX, "/photo %s", file);

   //new photo
   photo = enlil_photo_new();
   char *name =  ecore_file_strip_ext(file);
   enlil_photo_name_set(photo, name);
   FREE(name);
   enlil_photo_file_name_set(photo, file);

   if(enlil_photo_is(buf_file))
     enlil_photo_type_set(photo, ENLIL_PHOTO_TYPE_PHOTO);
	else if(enlil_video_is(buf_file))
	  enlil_photo_type_set(photo, ENLIL_PHOTO_TYPE_VIDEO);
	else if(enlil_gpx_is(buf_file))
	  enlil_photo_type_set(photo, ENLIL_PHOTO_TYPE_GPX);

   FILE_INFO_GET(buf_file, _time, size);
   enlil_photo__time_set(photo, _time);
   enlil_photo_size_set(photo, size);
   enlil_photo_path_set(photo, buf_path);

   SAVE();

   enlil_album_photo_add(album, photo);
   _sync->msg.type = Enlil_SYNC_PHOTO_NEW;
   _sync->msg.album = album;
   _sync->msg.photo = photo;
   ecore_pipe_write(_sync->pipe.thread_main, "a", 1);
   ENLIL_MUTEX_LOCK(_sync);

#undef SAVE
}

static int _enlil_sync_all_photo_update(Enlil_Sync *_sync, Enlil_Album *album, Enlil_Photo *_photo)
{
   char buf_path[PATH_MAX], buf_file[PATH_MAX], buf[PATH_MAX], buf2[PATH_MAX];
   time_t _time;
   int size;
   Enlil_Photo *photo;
   int ret;

#define SAVE() \
   do { \
	ret = enlil_photo_eet_save(photo); \
	if(!ret) \
	  { \
	     snprintf(buf, 1024, "Failed to save the photo \"%s\" in the file %s/%s/"EET_FILE, enlil_photo_name_get(photo), _sync->path, enlil_album_file_name_get(album)); \
	     _sync->msg.type = Enlil_SYNC_ERROR; \
	     _sync->msg.error = Sync_Error_Eet_Save_Failed; \
	     _sync->msg.msg = buf; \
	     ecore_pipe_write(_sync->pipe.thread_main, "a", 1); \
	     ENLIL_MUTEX_LOCK(_sync); \
	  } \
   }while (0);

   ASSERT_RETURN(album != NULL);
   ASSERT_RETURN(_photo != NULL);

   snprintf(buf_path,PATH_MAX,"%s/%s",enlil_album_path_get(album),enlil_album_file_name_get(album));
   snprintf(buf_file, PATH_MAX, "%s/%s", buf_path, enlil_photo_file_name_get(_photo));
   snprintf(buf,PATH_MAX,"%s/"EET_FILE,buf_path);
   snprintf(buf2, PATH_MAX, "/photo %s", enlil_photo_file_name_get(_photo));

   photo = enlil_photo_eet_load(buf, buf2);

   if(!photo)
     {
	enlil_album_photo_remove(album, _photo);
	_enlil_sync_all_photo_new(_sync, album, enlil_photo_file_name_get(_photo));
	enlil_photo_free(&_photo);
	return 1;
     }
   else
     {
	FILE_INFO_GET(buf_file, _time, size);
	if(_time > enlil_photo__time_get(photo))
	  {
	     enlil_photo__time_set(photo, _time);
	     enlil_photo_size_set(photo, size);

	     SAVE();

	     _sync->msg.type = Enlil_SYNC_PHOTO_UPDATE;
	     _sync->msg.album = album;
	     _sync->msg.photo = photo;
	     ecore_pipe_write(_sync->pipe.thread_main, "a", 1);
	     ENLIL_MUTEX_LOCK(_sync);
	  }
     }

   enlil_photo_free(&photo);

   return 0;
#undef SAVE
}

static void _enlil_sync_all_album_sync(Enlil_Sync *_sync, Enlil_Album *album)
{
   char buf_path[PATH_MAX], buf_eet[PATH_MAX], buf[PATH_MAX];
   Eina_List *l_files;
   Eina_List *l;
   char *file;
   Enlil_Photo *photo;
   int save_list = 0;
   Eina_List *l_files_new, *l_files_common, *l_files_disapear;
   Enlil_Photo_Sort sort;

   snprintf(buf_path,PATH_MAX,"%s/%s",enlil_album_path_get(album),enlil_album_file_name_get(album));
   snprintf(buf_eet,PATH_MAX,"%s/"EET_FILE,buf_path);
   if(!ecore_file_exists(buf_path))
     return ;

   l_files = ecore_file_ls(buf_path);

   enlil_album_photos_get(album);
   sort = enlil_album_photos_sort_get(album);
   l_files_common = eina_list_right_sorted_diff(enlil_album_photos_get(album),
	   l_files, &l_files_disapear, &l_files_new, file_photo_comp_cb);

   //new photos
   if(l_files_new)
     save_list = 1;
   EINA_LIST_FOREACH(l_files_new, l, file)
     {
	snprintf(buf, PATH_MAX, "%s/%s", buf_path, file);
	if(enlil_photo_is(buf) || enlil_video_is(buf) || enlil_gpx_is(buf))
	  _enlil_sync_all_photo_new(_sync, album, file);
     }

   //known photos
   EINA_LIST_FOREACH(l_files_common, l, photo)
     {
	if( _enlil_sync_all_photo_update(_sync, album, photo) )
	  save_list = 1;
     }

   //deleted photos
   if(l_files_disapear)
     save_list = 1;
   EINA_LIST_FOREACH(l_files_disapear, l, photo)
     {
	snprintf(buf, PATH_MAX, "%s/%s", buf_path, enlil_photo_file_name_get(photo));
	if(!ecore_file_exists(buf))
	  {
	     enlil_album_photo_remove(album, photo);
	     enlil_photo_eet_remove(buf_eet, enlil_photo_file_name_get(photo));

	     _sync->msg.type = Enlil_SYNC_PHOTO_DISAPPEAR;
	     _sync->msg.album = album;
	     _sync->msg.photo = photo;
	     ecore_pipe_write(_sync->pipe.thread_main, "a", 1);
	     ENLIL_MUTEX_LOCK(_sync);

	     enlil_photo_free(&photo);
	  }
     }

   if(save_list)
     enlil_album_eet_photos_list_save(album);

   if(l_files_new) eina_list_free(l_files_new);
   if(l_files_disapear) eina_list_free(l_files_disapear);
   if(l_files_common) eina_list_free(l_files_common);

   EINA_LIST_FREE(l_files, file)
      free(file);
}

static void _enlil_sync_all_album_new(Enlil_Sync *_sync, Enlil_Library *library_list, const char *file)
{
   char buf[PATH_MAX], buf2[PATH_MAX];
   int ret;
   Enlil_Album *album;

#define SAVE() \
   do { \
	ret = enlil_album_eet_header_save(album); \
	if(!ret) \
	  { \
	     snprintf(buf, 1024, "Failed to save the album \"%s\" in the file %s/"EET_FILE, enlil_album_name_get(album), _sync->path); \
	     _sync->msg.type = Enlil_SYNC_ERROR; \
	     _sync->msg.error = Sync_Error_Eet_Save_Failed; \
	     _sync->msg.msg = buf; \
	     ecore_pipe_write(_sync->pipe.thread_main, "a", 1); \
	     ENLIL_MUTEX_LOCK(_sync); \
	  } \
   } while(0);

   Enlil_Library *library = enlil_library_new(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
   enlil_library_path_set(library, _sync->path);

   //
   time_t _time;
   int size;
   snprintf(buf, PATH_MAX, "%s/%s", _sync->path, file);
   FILE_INFO_GET(buf, _time, size);
   //

   //if an eet file is in the album we delete it
   snprintf(buf2, PATH_MAX, "%s/"EET_FILE, buf);
   remove(buf2);

   album = enlil_album_new();
   enlil_album_path_set(album, _sync->path);
   enlil_album_file_name_set(album, file);
   enlil_album_name_set(album, file);
   enlil_album__time_set(album, _time);

   SAVE();

   //send notif new album
   _sync->msg.type = Enlil_SYNC_ALBUM_NEW;
   _sync->msg.library = library;
   _sync->msg.album = album;
   ecore_pipe_write(_sync->pipe.thread_main, "a", 1);
   ENLIL_MUTEX_LOCK(_sync);

   Enlil_Album *album_list = enlil_album_new();
   enlil_album_path_set(album_list, enlil_album_path_get(album));
   enlil_album_file_name_set(album_list, enlil_album_file_name_get(album));
   enlil_album_name_set(album_list, enlil_album_name_get(album));
   enlil_library_album_add(library_list, album_list);

   _enlil_sync_all_album_sync(_sync, album);
   enlil_library_free(&library);
   enlil_album_free(&album);

#undef SAVE
}

static int _enlil_sync_all_album_update(Enlil_Sync *_sync, Enlil_Library *library_list, Enlil_Album *_album)
{
   char buf[PATH_MAX];

   Enlil_Library *library = enlil_library_new(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
   enlil_library_path_set(library, _sync->path);

   //
   time_t _time;
   int size;
   snprintf(buf, PATH_MAX, "%s/%s", _sync->path, enlil_album_file_name_get(_album));
   FILE_INFO_GET(buf, _time, size);
   //

   //compare to the eet file
   Enlil_Album *album = enlil_library_eet_album_load(library, enlil_album_file_name_get(_album));
   if(!album)
     {
	enlil_library_album_remove(library_list, _album);
	_enlil_sync_all_album_new(_sync, library_list, enlil_album_file_name_get(_album));
	enlil_album_free(&_album);
	enlil_library_free(&library);
	return 1;
     }

   //compare the dates
   /*if(!save && enlil_album__time_get(album) > time)
     {
     SAVE();

   //send notif update album
   sync->msg.type = Enlil_SYNC_ALBUM_UPDATE;
   sync->msg.library = library;
   sync->msg.album = album;
   ecore_pipe_write(sync->pipe.thread_main, "a", 1);
   ENLIL_MUTEX_LOCK(sync);
   } */

   _enlil_sync_all_album_sync(_sync, album);
   enlil_library_free(&library);
   enlil_album_free(&album);

   return 0;
}

/**
 * @brief Synchronise a photo manager folder
 * @param data a sync struct
 */
static void _enlil_sync_all_start(void *data, __UNUSED__ Ecore_Thread *thread)
{
   Enlil_Sync *_sync = data;
   Enlil_Library *library;
   char buf[PATH_MAX];
   Eina_List *l_files;
   Eina_List *l;
   Eina_List *l_files_new;
   Eina_List *l_files_disapear;
   Eina_List *l_files_common;
   char *file;
   int save_album_list = 0;
   Enlil_Album *album;

   library = enlil_library_new(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, NULL);
   enlil_library_path_set(library, _sync->path);

   l_files = ecore_file_ls(enlil_library_path_get(library));

   Enlil_Library *library_list = enlil_library_eet_albums_load(library);
   if(!library_list) library_list = enlil_library_new(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, NULL, NULL, NULL, NULL, NULL, NULL);
   enlil_library_path_set(library_list, _sync->path);

   l_files_common = eina_list_left_sorted_diff(enlil_library_albums_get(library_list),
	 l_files, &l_files_disapear, &l_files_new, file_album_comp_cb);

   //new album
   if(l_files_new)
     save_album_list = 1;
   EINA_LIST_FOREACH(l_files_new, l, file)
     {
	snprintf(buf, PATH_MAX, "%s/%s", enlil_library_path_get(library),file);
	if(ecore_file_is_dir(buf))
	  _enlil_sync_all_album_new(_sync, library_list, file);
     }

   //known albums
   EINA_LIST_FOREACH(l_files_common, l, album)
     {
	if( _enlil_sync_all_album_update(_sync, library_list, album) )
	  save_album_list = 1;
     }

   //deleted albums
   if(l_files_disapear)
     save_album_list = 1;
   EINA_LIST_FOREACH(l_files_disapear, l, album)
     {
	snprintf(buf,PATH_MAX,"%s/%s", enlil_library_path_get(library_list), enlil_album_file_name_get(album));
	if(!ecore_file_exists(buf))
	  {
	     _sync->msg.type = Enlil_SYNC_ALBUM_DISAPPEAR;
	     _sync->msg.library = library;
	     _sync->msg.album = album;
	     ecore_pipe_write(_sync->pipe.thread_main, "a", 1);
	     ENLIL_MUTEX_LOCK(_sync);

	     enlil_library_album_remove(library_list, album);
	     enlil_library_eet_album_remove(library_list, enlil_album_file_name_get(album));
	     enlil_album_free(&album);
	  }
     }
   if(save_album_list)
     {
	enlil_library_eet_albums_save(library_list);
	enlil_library_eet_collections_save(library_list);
	enlil_library_eet_tags_save(library_list);
     }


   EINA_LIST_FREE(l_files, file)
      FREE(file);

   if(l_files_new) eina_list_free(l_files_new);
   if(l_files_disapear) eina_list_free(l_files_disapear);
   if(l_files_common) eina_list_free(l_files_common);

   enlil_library_free(&library_list);

   enlil_library_free(&library);

   _sync->msg.type = Enlil_SYNC_DONE;
   ecore_pipe_write(_sync->pipe.thread_main, "a", 1);
}

/**
 * @brief Handle which is trigger when the thread send a message (write in the pipe).
 * This method process the message and call the corresponded callback.
 * @data a sync struct (which contains the message)
 * @buffer the message send in the pipe (not used)
 * @nbyte the size of @message (not used)
 */
static void _enlil_sync_message_cb(void *data, __UNUSED__ void *buffer, __UNUSED__ unsigned int nbyte)
{
   Enlil_Sync *_sync = (Enlil_Sync*) data;

   if(_sync->msg.type == Enlil_SYNC_DONE)
     {
	LOG_INFO("All files are synchronised");
	//The thread is not in pause.
	_sync->sync.done_cb(_sync->sync.data, _sync);
	return ;
     }

   switch(_sync->msg.type)
     {
      case Enlil_SYNC_ERROR:
	 LOG_CRIT("%s", _sync->msg.msg);
	 _sync->sync.error_cb(_sync->sync.data, _sync, _sync->msg.error, _sync->msg.msg);
	 break;
      case Enlil_SYNC_ALBUM_NEW:
	 LOG_INFO("New album : %s", enlil_album_file_name_get(_sync->msg.album));
	 _sync->sync.album_new_cb(_sync->sync.data, _sync, _sync->msg.library, _sync->msg.album);
	 break;
      case Enlil_SYNC_ALBUM_UPDATE:
	 LOG_INFO("Update album : %s", enlil_album_file_name_get(_sync->msg.album));
	 _sync->sync.album_update_cb(_sync->sync.data, _sync, _sync->msg.library, _sync->msg.album);
	 break;
      case Enlil_SYNC_ALBUM_DISAPPEAR:
	 LOG_INFO("The album is referenced in the eet file but the folder does not exists: %s", enlil_album_file_name_get(_sync->msg.album));
	 _sync->sync.album_disappear_cb(_sync->sync.data, _sync, _sync->msg.library, _sync->msg.album);
	 break;
      case Enlil_SYNC_PHOTO_NEW:
	 LOG_INFO("New photo \"%s\" in the album : %s", enlil_photo_file_name_get(_sync->msg.photo), enlil_album_file_name_get(_sync->msg.album));
	 _sync->sync.photo_new_cb(_sync->sync.data, _sync, _sync->msg.album, _sync->msg.photo);
	 break;
      case Enlil_SYNC_PHOTO_UPDATE:
	 LOG_INFO("Update photo \"%s\" of the album : %s", enlil_photo_name_get(_sync->msg.photo),enlil_album_file_name_get(_sync->msg.album));
	 _sync->sync.photo_update_cb(_sync->sync.data, _sync, _sync->msg.album, _sync->msg.photo);
	 break;
      case Enlil_SYNC_PHOTO_DISAPPEAR:
	 LOG_INFO("The photo \"%s\" of the album %s is referenced in the eet file but the file does not exists", enlil_photo_name_get(_sync->msg.photo), enlil_album_file_name_get(_sync->msg.album));
	 _sync->sync.photo_disappear_cb(_sync->sync.data, _sync, _sync->msg.album, _sync->msg.photo);
	 break;
      case Enlil_SYNC_DONE: ;
     }
   ENLIL_MUTEX_UNLOCK(_sync);
}

/**
 * @brief Method called when a synchronisation is done. The method
 * start a new job if the list is not empty
 * @param data a sync struct
 */
static void _enlil_sync_end_cb(void *data, __UNUSED__ Ecore_Thread *thread)
{
   double t;

   Enlil_Sync *_sync = (Enlil_Sync*) data;

   t = ecore_time_get();
   double _time = t- _sync->t0;

   LOG_DBG("(%f sec) Synchronizing done.", _time);

   _sync->is_running = 0;
   _enlil_sync_job_free(&(_sync->current_job));
   _enlil_sync_next_job_process(_sync);
}

static int file_album_comp_cb(const void *d1, const void *d2)
{
   const Enlil_Album *album = d1;
   const char *s = d2;

   return strcmp(enlil_album_file_name_get(album), s);
}

static int file_photo_comp_cb(const void *d1, const void *d2)
{
   const Enlil_Photo *photo = d1;
   const char *s = d2;

   return strcmp(enlil_photo_file_name_get(photo), s);
}

