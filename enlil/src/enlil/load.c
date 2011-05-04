#include "enlil_private.h"
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

/* FIXME: destroy the mutex */

#ifdef EFL_HAVE_POSIX_THREADS
# define ENLIL_MUTEX pthread_mutex_t
# define ENLIL_MUTEX_INIT(Load)   pthread_mutex_init(&(Load->mutex), NULL)
# define ENLIL_MUTEX_LOCK(Load)   pthread_mutex_lock(&(Load->mutex))
# define ENLIL_MUTEX_UNLOCK(Load) pthread_mutex_unlock(&(Load->mutex))
#else
# include <windows.h>
# define ENLIL_MUTEX HANDLE
# define ENLIL_MUTEX_INIT(Load)   Load->mutex = CreateMutex(NULL, FALSE, NULL)
# define ENLIL_MUTEX_LOCK(Load)   WaitForSingleObject(Load->mutex, INFINITE)
# define ENLIL_MUTEX_UNLOCK(Load) ReleaseMutex(Load->mutex)
#endif

static void _enlil_load(void         *data,
                        Ecore_Thread *thread);
static void _album_load(Enlil_Load  *load,
                        Enlil_Album *album);

static void _enlil_load_message_cb(void        *data,
                                   void        *buffer,
                                   unsigned int nbyte);
static void _enlil_load_end_cb(void         *data,
                               Ecore_Thread *thread);

typedef enum Enlil_Load_Message
{
   Enlil_LOAD_ERROR,
   Enlil_LOAD_ALBUM_DONE
}Enlil_Load_Message;

struct Enlil_Load_Configuration
{
   void                         *data;
   Enlil_Load_Conf_Error_Cb      error_cb;
   Enlil_Load_Conf_Album_Done_Cb album_done_cb;
   Enlil_Load_Conf_Done_Cb       done_cb;
};

struct enlil_load
{
   Enlil_Library           *library;
   Ecore_Thread            *thread;
   Enlil_Load_Configuration conf;
   int                      photos_count;

   int                      is_running;

   // mutex use to pause the thread and wake up
   ENLIL_MUTEX              mutex;

   struct
   {
      //thread send to the main loop
        Ecore_Pipe *thread_main;
   } pipe;

   struct
   {
      Enlil_Load_Message type;
      Load_Error         error;
      char              *msg;
      Enlil_Album       *album;
   } msg;

   double t0;
};

/**
 * @brief Create a new load struct
 * @param The photo manager associated
 * @param album_done_cb Callback called when a album is loaded
 * @param done_cb Callback called when the load is done
 * @param error_cb Callback called when an error occurs
 * @param user_data Data sent in the callbacks
 * @return Returns a new load struct
 */
Enlil_Load *
enlil_load_new(Enlil_Library                *library,
               Enlil_Load_Conf_Album_Done_Cb album_done_cb,
               Enlil_Load_Conf_Done_Cb       done_cb,
               Enlil_Load_Conf_Error_Cb      error_cb,
               void                         *user_data)
{
   ASSERT_RETURN(library != NULL);
   ASSERT_RETURN(album_done_cb != NULL);
   ASSERT_RETURN(done_cb != NULL);
   ASSERT_RETURN(error_cb != NULL);

   Enlil_Load *load = calloc(1, sizeof(Enlil_Load));
   ASSERT_RETURN(load != NULL);

   load->library = library;
   load->conf.album_done_cb = album_done_cb;
   load->conf.done_cb = done_cb;
   load->conf.error_cb = error_cb;
   load->conf.data = user_data;

   ENLIL_MUTEX_INIT(load);
   ENLIL_MUTEX_LOCK(load);

   load->pipe.thread_main = ecore_pipe_add(_enlil_load_message_cb, load);
   ASSERT_CUSTOM_RET(load->pipe.thread_main != NULL, enlil_load_free(&load); return NULL; );

   return load;
}

/**
 * @brief Free a load struct
 * @param load the load struct
 */
void
enlil_load_free(Enlil_Load **load)
{
   ASSERT_RETURN_VOID(load != NULL);
   Enlil_Load *_load = *load;
   ASSERT_RETURN_VOID(_load != NULL);

   if(_load->is_running)
     {
        LOG_ERR("You tried to free a load structure while the thread was running, this is really bad and can't be done\n");
        return;
     }

   if(_load->pipe.thread_main)
     ecore_pipe_del(_load->pipe.thread_main);

   FREE(_load);
}

/**
 * @brief Start to load the albums/photo in a thread
 * @param load the load struct
 */
void
enlil_load_run(Enlil_Load *load)
{
   ASSERT_RETURN_VOID(load != NULL);
   ASSERT_RETURN_VOID(load->is_running != 1);

   load->is_running = 1;
   load->t0 = ecore_time_get();
   load->thread = ecore_thread_run(_enlil_load, _enlil_load_end_cb, NULL, load);
   LOG_INFO("Loading start on the library : %s", enlil_library_path_get(load->library));
}

void
enlil_load_stop(Enlil_Load *load)
{
   ASSERT_RETURN_VOID(load != NULL);
   ASSERT_RETURN_VOID(load->is_running != 0);

   ecore_thread_cancel(load->thread);
   load->is_running = 0;
   LOG_INFO("Loading the library %s has been canceled.", enlil_library_path_get(load->library));
}

static void
_album_load(Enlil_Load  *load,
            Enlil_Album *album)
{
   Eina_List *l;
   Enlil_Photo *photo;

   //load the list of photos from the eet file
   EINA_LIST_FOREACH(enlil_album_photos_get(album), l, photo)
     enlil_photo_album_set(photo, album);

   load->photos_count += enlil_album_photos_count_get(album);

   load->msg.type = Enlil_LOAD_ALBUM_DONE;
   load->msg.album = album;
   ecore_pipe_write(load->pipe.thread_main, "a", 1);
   ENLIL_MUTEX_LOCK(load);
}

static void
_enlil_load(void         *data,
            Ecore_Thread *thread __UNUSED__)
{
   Enlil_Load *load = data;
   Enlil_Library *library, *library_list;
   const Eina_List *l;
   Enlil_Album *album_list;
   Enlil_Collection *collection_list;
   Enlil_Tag *tag_list;

   library = load->library;

   library_list = enlil_library_eet_tags_load(library);
   EINA_LIST_FOREACH(enlil_library_tags_get(library_list), l, tag_list)
     {
        Enlil_Tag *tag = enlil_tag_copy_new(tag_list);
        _enlil_library_tag_add_end(library, tag, EINA_TRUE);
     }
   enlil_library_free(&library_list);

   library_list = enlil_library_eet_collections_load(library);
   EINA_LIST_FOREACH(enlil_library_collections_get(library_list), l, collection_list)
     {
        Enlil_Collection *collection = enlil_collection_copy_new(collection_list);
        _enlil_library_collection_add_end(library, collection, EINA_TRUE);
     }
   enlil_library_free(&library_list);

   library_list = enlil_library_eet_albums_load(library);
   if(!library_list) return;
   EINA_LIST_FOREACH(enlil_library_albums_get(library_list), l, album_list)
     {
        Enlil_Album *album = enlil_library_eet_album_load(library, enlil_album_file_name_get(album_list));
        if(album)
          {
             _enlil_library_album_add_end(library, album);
             _album_load(load, album);
          }
     }
   enlil_library_free(&library_list);
}

static void
_enlil_load_message_cb(void        *data,
                       void        *buffer __UNUSED__,
                       unsigned int nbyte __UNUSED__)
{
   Enlil_Load *load = (Enlil_Load *)data;

   const char *msg = NULL;
   if(load->msg.msg)
     msg = eina_stringshare_add(load->msg.msg);
   Enlil_Album *album = load->msg.album;

   ENLIL_MUTEX_UNLOCK(load);

   switch(load->msg.type)
     {
      case Enlil_LOAD_ERROR:
        LOG_CRIT("%s", load->msg.msg);
        load->conf.error_cb(load->conf.data, load, load->msg.error, msg);
        break;

      case Enlil_LOAD_ALBUM_DONE:
        LOG_INFO("Loading album done: %s", enlil_album_name_get(load->msg.album));
        load->conf.album_done_cb(load->conf.data, load, load->library, album);
     }
   EINA_STRINGSHARE_DEL(msg);
}

static void
_enlil_load_end_cb(void         *data,
                   Ecore_Thread *thread __UNUSED__)
{
   double t;
   Enlil_Load *load = (Enlil_Load *)data;
   load->is_running = 0;

   Eina_List *l_enlil = enlil_library_albums_get(load->library);

   t = ecore_time_get();
   double _time = t - load->t0;
   LOG_DBG("(%f sec) Loading the library \"%s\" done. %d albums and %d photos.",
           _time, enlil_library_path_get(load->library), eina_list_count(l_enlil), load->photos_count);
   load->conf.done_cb(load->conf.data, load, eina_list_count(l_enlil), load->photos_count);
}

