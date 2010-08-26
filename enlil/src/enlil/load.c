#include "enlil_private.h"
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>


static void _enlil_load(void *data);
static void _album_load(Enlil_Load *load, Enlil_Album *album);

static void _enlil_load_message_cb(void *data, void *buffer, unsigned int nbyte);
static void _enlil_load_end_cb(void *data);

typedef enum Enlil_Load_Message
{
    Enlil_LOAD_ERROR,
    Enlil_LOAD_ALBUM_DONE
}Enlil_Load_Message;

struct Enlil_Load_Configuration
{
    void *data;
    Enlil_Load_Conf_Error_Cb error_cb;
    Enlil_Load_Conf_Album_Done_Cb album_done_cb;
    Enlil_Load_Conf_Done_Cb done_cb;
};

struct enlil_load
{
    Enlil_Root *root;
    Enlil_Load_Configuration conf;
    int photos_count;

    int is_running;

    // mutex use to pause the thread and wake up
    pthread_mutex_t mutex;

    struct {
        //thread send to the main loop
        Ecore_Pipe *thread_main;
    }pipe;

    struct
    {
        Enlil_Load_Message type;
        Load_Error error;
        char *msg;
        Enlil_Album *album;
    }msg;

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
Enlil_Load *enlil_load_new(Enlil_Root *root,
        Enlil_Load_Conf_Album_Done_Cb album_done_cb,
        Enlil_Load_Conf_Done_Cb done_cb,
        Enlil_Load_Conf_Error_Cb error_cb,
        void *user_data)
{
    ASSERT_RETURN(root!=NULL);
    ASSERT_RETURN(album_done_cb!=NULL);
    ASSERT_RETURN(done_cb!=NULL);
    ASSERT_RETURN(error_cb!=NULL);

    Enlil_Load *load = calloc(1, sizeof(Enlil_Load));
    ASSERT_RETURN(load!=NULL);

    load->root = root;
    load->conf.album_done_cb = album_done_cb;
    load->conf.done_cb = done_cb;
    load->conf.error_cb = error_cb;
    load->conf.data = user_data;

    pthread_mutex_init(&(load->mutex), NULL);
    pthread_mutex_lock(&(load->mutex));

    load->pipe.thread_main = ecore_pipe_add(_enlil_load_message_cb, load);
    ASSERT_CUSTOM_RET(load->pipe.thread_main != NULL, enlil_load_free(&load); return NULL;);

    return load;
}

/**
 * @brief Free a load struct
 * @param load the load struct
 */
void enlil_load_free(Enlil_Load **load)
{
    ASSERT_RETURN_VOID(load != NULL);
    Enlil_Load *_load = *load;
    ASSERT_RETURN_VOID(_load!=NULL);

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
void enlil_load_run(Enlil_Load *load)
{
    ASSERT_RETURN_VOID(load!=NULL);
    ASSERT_RETURN_VOID(load->is_running != 1);

    load->is_running = 1;
    load->t0 = ecore_time_get();
    ecore_thread_run(_enlil_load, _enlil_load_end_cb, NULL, load);
    LOG_INFO("Loading start on the library : %s", enlil_root_path_get(load->root));
}

static void _album_load(Enlil_Load *load, Enlil_Album *album)
{
    Eina_List *l;
    Enlil_Photo *photo;

    //load the list of photos from the eet file
    EINA_LIST_FOREACH(enlil_album_photos_get(album), l, photo)
        enlil_photo_album_set(photo, album);

    load->photos_count+=enlil_album_photos_count_get(album);

    load->msg.type = Enlil_LOAD_ALBUM_DONE;
    load->msg.album = album;
    ecore_pipe_write(load->pipe.thread_main, "a", 1);
    pthread_mutex_lock(&(load->mutex));
}

static void _enlil_load(void *data)
{
    Enlil_Load *load = data;
    Enlil_Root *root;

    root = load->root;

    Enlil_Root *root_list = enlil_root_eet_albums_load(root);
    if(!root_list) return ;
    Eina_List *l;
    Enlil_Album *album_list;
    EINA_LIST_FOREACH(enlil_root_albums_get(root_list), l, album_list)
      {
	 Enlil_Album *album = enlil_root_eet_album_load(root, enlil_album_file_name_get(album_list));
	 if(album)
	   {
	      _enlil_root_album_add_end(root, album);
	      _album_load(load, album);
	   }
    }
    enlil_root_free(&root_list);
}

static void _enlil_load_message_cb(void *data, void *buffer, unsigned int nbyte)
{
   Enlil_Load *load = (Enlil_Load*) data;

   const char *msg = NULL;
   if(load->msg.msg)
     msg = eina_stringshare_add(load->msg.msg);
   Enlil_Album *album = load->msg.album;

   pthread_mutex_unlock(&(load->mutex));

   switch(load->msg.type)
     {
      case Enlil_LOAD_ERROR:
	 LOG_CRIT(load->msg.msg);
	 load->conf.error_cb(load->conf.data, load, load->msg.error, msg);
	 break;
      case Enlil_LOAD_ALBUM_DONE:
	 LOG_INFO("Loading album done: %s", enlil_album_name_get(load->msg.album));
	 load->conf.album_done_cb(load->conf.data, load, load->root, album);
     }
   EINA_STRINGSHARE_DEL(msg);
}

static void _enlil_load_end_cb(void *data)
{
   double t;
   Enlil_Load *load = (Enlil_Load*) data;
   load->is_running = 0;

    Eina_List *l_enlil = enlil_root_albums_get(load->root);

    t = ecore_time_get();
    double time = t - load->t0;
    LOG_ERR("(%f sec) Loading the library \"%s\" done. %d albums and %d photos.",
            time, enlil_root_path_get(load->root), eina_list_count(l_enlil), load->photos_count);
    load->conf.done_cb(load->conf.data, load, eina_list_count(l_enlil), load->photos_count);
}

