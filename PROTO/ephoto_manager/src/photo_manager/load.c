// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#include "photo_manager_private.h"
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>


static void _pm_load(void *data);
static void _album_load(Photo_Manager_Load *load, PM_Album *album);

static void _pm_load_message_cb(void *data, void *buffer, unsigned int nbyte);
static void _pm_load_end_cb(void *data);

typedef enum PM_Load_Message
{
    PM_LOAD_ERROR,
    PM_LOAD_ALBUM_DONE
}PM_Load_Message;

struct PM_Load_Configuration
{
    void *data;
    PM_Load_Conf_Error_Cb error_cb;
    PM_Load_Conf_Album_Done_Cb album_done_cb;
    PM_Load_Conf_Done_Cb done_cb;
};

struct pm_load
{
    PM_Root *root;
    PM_Load_Configuration conf;
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
        PM_Load_Message type;
        Load_Error error;
        char *msg;
        PM_Album *album;
    }msg;
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
Photo_Manager_Load *pm_load_new(PM_Root *root,
        PM_Load_Conf_Album_Done_Cb album_done_cb,
        PM_Load_Conf_Done_Cb done_cb,
        PM_Load_Conf_Error_Cb error_cb,
        void *user_data)
{
    ASSERT_RETURN(root!=NULL);
    ASSERT_RETURN(album_done_cb!=NULL);
    ASSERT_RETURN(done_cb!=NULL);
    ASSERT_RETURN(error_cb!=NULL);

    Photo_Manager_Load *load = calloc(1, sizeof(Photo_Manager_Load));
    ASSERT_RETURN(load!=NULL);

    load->root = root;
    load->conf.album_done_cb = album_done_cb;
    load->conf.done_cb = done_cb;
    load->conf.error_cb = error_cb;
    load->conf.data = user_data;

    pthread_mutex_init(&(load->mutex), NULL);
    pthread_mutex_lock(&(load->mutex));

    load->pipe.thread_main = ecore_pipe_add(_pm_load_message_cb, load);
    ASSERT_CUSTOM_RET(load->pipe.thread_main != NULL, pm_load_free(&load); return NULL;);

    return load;
}

/**
 * @brief Free a load struct
 * @param load the load struct
 */
void pm_load_free(Photo_Manager_Load **load)
{
    ASSERT_RETURN_VOID(load != NULL);
    Photo_Manager_Load *_load = *load;
    ASSERT_RETURN_VOID(_load!=NULL);

    if(_load->pipe.thread_main)
        ecore_pipe_del(_load->pipe.thread_main);

    FREE(_load);
}

/**
 * @brief Start to load the albums/photo in a thread
 * @param load the load struct
 */
void pm_load_run(Photo_Manager_Load *load)
{
    ASSERT_RETURN_VOID(load!=NULL);
    ASSERT_RETURN_VOID(load->is_running != 1);

    load->is_running = 1;
    ecore_thread_run(_pm_load, _pm_load_end_cb, load);
    LOG_INFO("Loading start on pm : %s", pm_root_path_get(load->root));
}

static void _album_load(Photo_Manager_Load *load, PM_Album *album)
{
    Eina_List *l;
    PM_Photo *photo;

    //load the list of photos from the eet file
    EINA_LIST_FOREACH(pm_album_photos_get(album), l, photo)
        pm_photo_album_set(photo, album);

    load->photos_count+=pm_album_photos_count_get(album);

    load->msg.type = PM_LOAD_ALBUM_DONE;
    load->msg.album = album;
    ecore_pipe_write(load->pipe.thread_main, "a", 1);
    pthread_mutex_lock(&(load->mutex));
}

static void _pm_load(void *data)
{
    Photo_Manager_Load *load = data;
    PM_Root *root;

    root = load->root;

    PM_Root *root_list = pm_root_eet_albums_load(root);
    if(!root_list) return ;
    Eina_List *l;
    PM_Album *album_list;
    EINA_LIST_FOREACH(pm_root_albums_get(root_list), l, album_list)
    {
        PM_Album *album = pm_root_eet_album_load(root, pm_album_file_name_get(album_list));
        pm_root_album_add(root, album);

        _album_load(load, album);
    }
    pm_root_free(&root_list);
}

static void _pm_load_message_cb(void *data, void *buffer, unsigned int nbyte)
{
    Photo_Manager_Load *load = (Photo_Manager_Load*) data;

    switch(load->msg.type)
    {
        case PM_LOAD_ERROR:
            LOG_CRIT(load->msg.msg);
            load->conf.error_cb(load->conf.data, load, load->msg.error, load->msg.msg);
            break;
        case PM_LOAD_ALBUM_DONE:
            LOG_INFO("Loading album done: %s", pm_album_name_get(load->msg.album));
            load->conf.album_done_cb(load->conf.data, load, load->root, load->msg.album);
    }
    pthread_mutex_unlock(&(load->mutex));
}

static void _pm_load_end_cb(void *data)
{
    Photo_Manager_Load *load = (Photo_Manager_Load*) data;
    load->is_running = 0;

    Eina_List *l_pm = pm_root_albums_get(load->root);
    LOG_ERR("Loading the albums \"%s\" done. %d albums and %d photos.",
            pm_root_path_get(load->root), eina_list_count(l_pm), load->photos_count);
    load->conf.done_cb(load->conf.data, load, eina_list_count(l_pm), load->photos_count);
}

