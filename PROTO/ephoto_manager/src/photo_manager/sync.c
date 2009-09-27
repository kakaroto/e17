// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#include "photo_manager_private.h"
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

int LOG_DOMAIN;

typedef struct PM_Sync_Configuration PM_Sync_Configuration;

typedef enum PM_Sync_Message
{
    PM_SYNC_ERROR,
    PM_SYNC_ALBUM_NEW,
    PM_SYNC_ALBUM_UPDATE,
    PM_SYNC_ALBUM_DISAPPEAR,
    PM_SYNC_PHOTO_NEW,
    PM_SYNC_PHOTO_UPDATE,
    PM_SYNC_PHOTO_DISAPPEAR,
    PM_SYNC_DONE
}PM_Sync_Message;

typedef enum PM_Sync_Job_Type
{
    PM_SYNC_JOB_ALL,
    PM_SYNC_JOB_ALBUM,
    PM_SYNC_JOB_PHOTO
} PM_Sync_Job_Type;

typedef struct PM_Sync_Job
{
    PM_Sync_Job_Type type;
    const char *folder;
    const char* file;
} PM_Sync_Job;

struct PM_Sync_Configuration
{
    void *data;
    PM_Sync_Error_Cb error_cb;
    PM_Sync_Album_New_Cb album_new_cb;
    PM_Sync_Album_Update_Cb album_update_cb;
    PM_Sync_Album_Disappear_Cb album_disappear_cb;
    PM_Sync_Photo_New_Cb photo_new_cb;
    PM_Sync_Photo_Update_Cb photo_update_cb;
    PM_Sync_Photo_Disappear_Cb photo_disappear_cb;
    PM_Sync_Done_Cb done_cb;
};

struct pm_sync
{
    const char* path;
    PM_Sync_Configuration sync;

    Eina_List *jobs;
    int is_running;

    // mutex use to pause the thread and wake up
    pthread_mutex_t mutex;

    struct {
        //thread send to the main loop
        Ecore_Pipe *thread_main;
    }pipe;

    //message sent by the thread
    struct
    {
        PM_Sync_Message type;
        Sync_Error error;
        char *msg;
        PM_Root *root;
        PM_Album *album;
        PM_Photo *photo;
    }msg;

    //extra data used by pm_sync_album_folder() and pm_sync_photo_file()
    struct
    {
        const char *file;
        const char *folder;
    } extra;
};



static void _pm_sync_message_cb(void *data, void *buffer, unsigned int nbyte);
static void _pm_sync_end_cb(void *data);

static void _pm_sync_album_folder_thread(void *data);
static void _pm_sync_photo_file_thread(void *data);

static void _pm_sync_run(Photo_Manager_Sync *sync);
static void _pm_sync_album_folder_run(Photo_Manager_Sync *sync, const char *file);
static void _pm_sync_photo_file_run(Photo_Manager_Sync *sync, const char *folder, const char *file);

//method used to synchronise a photo
static void  _pm_sync_photo_file_start(Photo_Manager_Sync *sync, const char *folder, const char *file);

//method used to synchronise an album folder
static void _pm_sync_album_folder_start(Photo_Manager_Sync *sync, const char *file);
static void _pm_sync_album_album_sync(Photo_Manager_Sync *sync, PM_Album *album);
static int _pm_sync_album_photo_file(Photo_Manager_Sync *sync, PM_Album *album, const char *file);

//methods used to do a global synchronisation
static void _pm_sync_all_start(void *data);
static int _pm_sync_all_album_folder(Photo_Manager_Sync *sync, PM_Root *pm_list, const char *file);
static void _pm_sync_all_album_sync(Photo_Manager_Sync *sync, PM_Album *album);
static int _pm_sync_all_photo_file(Photo_Manager_Sync *sync, PM_Album *album, const char *file);

static void _pm_sync_next_job_process(Photo_Manager_Sync *sync);
static void _pm_sync_job_free(PM_Sync_Job **job);

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
Photo_Manager_Sync *pm_sync_new(const char *path,
        PM_Sync_Album_New_Cb album_new_cb,
        PM_Sync_Album_Update_Cb album_update_cb,
        PM_Sync_Album_Disappear_Cb album_disappear_cb,
        PM_Sync_Photo_New_Cb photo_new_cb,
        PM_Sync_Photo_Update_Cb photo_update_cb,
        PM_Sync_Photo_Disappear_Cb photo_disappear_cb,
        PM_Sync_Done_Cb done_cb,
        PM_Sync_Error_Cb error_cb,
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
    ASSERT_RETURN(error_cb!=NULL);

    Photo_Manager_Sync *sync = calloc(1, sizeof(Photo_Manager_Sync));
    ASSERT_RETURN(sync!=NULL);

    sync->path = eina_stringshare_add(path);
    sync->sync.album_new_cb = album_new_cb;
    sync->sync.album_update_cb = album_update_cb;
    sync->sync.album_disappear_cb = album_disappear_cb;
    sync->sync.photo_new_cb = photo_new_cb;
    sync->sync.photo_update_cb = photo_update_cb;
    sync->sync.photo_disappear_cb = photo_disappear_cb;
    sync->sync.done_cb = done_cb;
    sync->sync.error_cb = error_cb;
    sync->sync.data = user_data;

    pthread_mutex_init(&(sync->mutex), NULL);
    pthread_mutex_lock(&(sync->mutex));

    sync->pipe.thread_main = ecore_pipe_add(_pm_sync_message_cb, sync);
    ASSERT_CUSTOM_RET(sync->pipe.thread_main != NULL, pm_sync_free(&sync); return NULL;);

    return sync;
}

/**
 * @brief Free a synchronisation struct
 * @param sync the sync struct
 */
void pm_sync_free(Photo_Manager_Sync **sync)
{
    ASSERT_RETURN_VOID(sync != NULL);
    Photo_Manager_Sync *_sync = *sync;
    ASSERT_RETURN_VOID(_sync!=NULL);
    ASSERT_RETURN_VOID(!_sync->is_running);

    eina_stringshare_del(_sync->path);

    if(_sync->pipe.thread_main)
        ecore_pipe_del(_sync->pipe.thread_main);

    FREE(_sync);
}

/**
 * @brief Add a new job : Synchronise all the albums and photos
 * @param sync the sync struct
 */
void pm_sync_job_all_add(Photo_Manager_Sync *sync)
{
    PM_Sync_Job *job;

    ASSERT_RETURN_VOID(sync != NULL);

    job = calloc(1, sizeof(PM_Sync_Job));
    job->type = PM_SYNC_JOB_ALL;
    sync->jobs = eina_list_append(sync->jobs, job);

    if(!sync->is_running)
        _pm_sync_next_job_process(sync);
}

/**
 * @brief Add a new job : Synchronise a album and its photos
 * @brief sync the sync struct
 * @brief folder the album folder name
 */
void pm_sync_job_album_folder_add(Photo_Manager_Sync *sync, const char *folder)
{
    PM_Sync_Job *job;

    ASSERT_RETURN_VOID(sync != NULL);
    ASSERT_RETURN_VOID(folder != NULL);

    job = calloc(1, sizeof(PM_Sync_Job));
    job->type = PM_SYNC_JOB_ALBUM;
    job->folder = eina_stringshare_add(folder);
    sync->jobs = eina_list_append(sync->jobs, job);

    if(!sync->is_running)
        _pm_sync_next_job_process(sync);
}

/**
 * @brief Add a new job : Synchronise a photo
 * @param sync the sync struct
 * @param folder the album folder name
 * @brief file the photo file name
 */
void pm_sync_job_photo_file_add(Photo_Manager_Sync *sync, const char *folder, const char *file)
{
    PM_Sync_Job *job;

    ASSERT_RETURN_VOID(sync != NULL);
    ASSERT_RETURN_VOID(file != NULL);

    job = calloc(1, sizeof(PM_Sync_Job));
    job->type = PM_SYNC_JOB_PHOTO;
    job->folder = eina_stringshare_add(folder);
    job->file = eina_stringshare_add(file);
    sync->jobs = eina_list_append(sync->jobs, job);

    if(!sync->is_running)
        _pm_sync_next_job_process(sync);
}

static void _pm_sync_job_free(PM_Sync_Job **job)
{
    ASSERT_RETURN_VOID(job != NULL);
    PM_Sync_Job *_job = * job;
    ASSERT_RETURN_VOID(_job!=NULL);
    switch(_job->type)
    {
        case PM_SYNC_JOB_ALBUM:
            eina_stringshare_del(_job->folder);
            break;
        case PM_SYNC_JOB_PHOTO:
            eina_stringshare_del(_job->file);
            eina_stringshare_del(_job->folder);
            break;
        default: ;
    }
    FREE(_job);
}

static void _pm_sync_next_job_process(Photo_Manager_Sync *sync)
{
    PM_Sync_Job *job;

    ASSERT_RETURN_VOID(sync != NULL);

    if(!sync->jobs)
    {
        sync->is_running = 0;
        return ;
    }

    sync->is_running = 1;

    job = eina_list_data_get(sync->jobs);
    switch(job->type)
    {
        case PM_SYNC_JOB_ALL:
            _pm_sync_run(sync);
            break;
        case PM_SYNC_JOB_ALBUM:
            _pm_sync_album_folder_run(sync, job->folder);
            break;
        case PM_SYNC_JOB_PHOTO:
            _pm_sync_photo_file_run(sync, job->folder, job->file);
    }
}

static void _pm_sync_run(Photo_Manager_Sync *sync)
{
    ASSERT_RETURN_VOID(sync!=NULL);

    ecore_thread_run(_pm_sync_all_start, _pm_sync_end_cb, sync);
    LOG_INFO("Synchronisation start on albums : %s", sync->path);
}

static void _pm_sync_album_folder_run(Photo_Manager_Sync *sync, const char *folder)
{
    ASSERT_RETURN_VOID(sync!=NULL);
    ASSERT_RETURN_VOID(folder!=NULL);

    sync->extra.folder = folder;
    ecore_thread_run(_pm_sync_album_folder_thread, _pm_sync_end_cb, sync);
    LOG_INFO("Synchronisation start on album : %s", folder);
}

static void _pm_sync_album_folder_thread(void *data)
{
    Photo_Manager_Sync *sync = (Photo_Manager_Sync *) data;
    _pm_sync_album_folder_start(sync, sync->extra.folder);

    sync->msg.type = PM_SYNC_DONE;
    ecore_pipe_write(sync->pipe.thread_main, "a", 1);
}

static void _pm_sync_photo_file_run(Photo_Manager_Sync *sync, const char *folder, const char *file)
{
    ASSERT_RETURN_VOID(sync!=NULL);
    ASSERT_RETURN_VOID(file!=NULL);

    sync->extra.file = file;
    sync->extra.folder = folder;
    ecore_thread_run(_pm_sync_photo_file_thread, _pm_sync_end_cb, sync);
    LOG_INFO("Synchronisation start on photo : %s", file);
}

static void _pm_sync_photo_file_thread(void *data)
{
    Photo_Manager_Sync *sync = (Photo_Manager_Sync *) data;
    _pm_sync_photo_file_start(sync, sync->extra.folder, sync->extra.file);

    sync->msg.type = PM_SYNC_DONE;
    ecore_pipe_write(sync->pipe.thread_main, "a", 1);
}

/**
 * @brief Synchronise a specific photo file
 * @param sync the sync struct
 * @param album the album which contains the file
 * @param file the photo file name
 */
static void _pm_sync_photo_file_start(Photo_Manager_Sync *sync, const char *folder, const char *file)
{
    char buf_path[PATH_MAX], buf_file[PATH_MAX], buf_key[PATH_MAX], buf[PATH_MAX],
         buf_eet[PATH_MAX];
    time_t time;
    PM_Photo *photo;
    int save = 0;
    int ret;
    int not_delete_photo = 0;
    int new_photo = 0;
    int file_exist;
    PM_Album *album;

    ASSERT_RETURN_VOID(folder != NULL);
    ASSERT_RETURN_VOID(file != NULL);

    album = pm_album_new();
    pm_album_path_set(album, sync->path);
    pm_album_file_name_set(album, folder);
    pm_album_name_set(album, folder);

    snprintf(buf_path,PATH_MAX,"%s/%s",pm_album_path_get(album),pm_album_file_name_get(album));
    snprintf(buf_eet,PATH_MAX,"%s/%s/"EET_FILE,pm_album_path_get(album),pm_album_file_name_get(album));
    snprintf(buf_file, PATH_MAX, "%s/%s", buf_path, file);
    snprintf(buf_key, PATH_MAX, "/photo %s", file);

    //load the photo from the eet file
    photo = pm_photo_eet_load(buf_eet, buf_key);
    //test if the file exits
    file_exist = ecore_file_exists(buf_file);
    if(file_exist)
        FILE_LASTCHANGE_TIME_GET(buf_file, time);

    if(file_exist && !photo)
    {
        //new photo
        new_photo = 1;
        photo = pm_photo_new();
        pm_photo_name_set(photo, file);
        pm_photo_file_name_set(photo, file);
        pm_photo_time_set(photo, time);
        pm_photo_path_set(photo, buf_path);

        pm_album_photo_add(album, photo);
        sync->msg.type = PM_SYNC_PHOTO_NEW;
        sync->msg.album = album;
        sync->msg.photo = photo;
        ecore_pipe_write(sync->pipe.thread_main, "a", 1);
        pthread_mutex_lock(&(sync->mutex));

        //update the list of album
        pm_album_eet_photos_list_save(album);

        save = 1;
        not_delete_photo = 1;
    }
    else if(file_exist && photo && time > pm_photo_time_get(photo))
    {
        //update
        pm_photo_time_set(photo, time);

        sync->msg.type = PM_SYNC_PHOTO_UPDATE;
        sync->msg.album = album;
        sync->msg.photo = photo;
        ecore_pipe_write(sync->pipe.thread_main, "a", 1);
        pthread_mutex_lock(&(sync->mutex));

        save = 1;
    }
    else if(!file_exist && photo)
    {
        //photo disappear
        sync->msg.type = PM_SYNC_PHOTO_DISAPPEAR;
        sync->msg.album = album;
        sync->msg.photo = photo;
        ecore_pipe_write(sync->pipe.thread_main, "a", 1);
        pthread_mutex_lock(&(sync->mutex));

        PM_Photo *photo_list = pm_album_photo_search_file_name(album, file);
        if(photo_list)
        {
            pm_album_photo_remove(album, photo_list);
            pm_album_eet_photos_list_save(album);
            pm_photo_free(&photo_list);
        }

        pm_photo_eet_remove(buf_eet, pm_photo_file_name_get(photo));
    }
    else
        goto done;

    if(save)
    {
        ret = pm_photo_eet_save(photo);
        if(!ret)
        {
            snprintf(buf, 1024, "Failed to save the photo \"%s\" in the file %s/%s/"EET_FILE, pm_photo_name_get(photo), sync->path, pm_album_file_name_get(album));

            sync->msg.type = PM_SYNC_ERROR;
            sync->msg.error = Sync_Error_Eet_Save_Failed;
            sync->msg.msg = buf;
            ecore_pipe_write(sync->pipe.thread_main, "a", 1);
            pthread_mutex_lock(&(sync->mutex));
        }
    }

done:
    if(!not_delete_photo && photo)
        pm_photo_free(&photo);
    pm_album_free(&album);
}






static int _pm_sync_album_photo_file(Photo_Manager_Sync *sync, PM_Album *album, const char *file)
{
    char buf_path[PATH_MAX], buf_file[PATH_MAX], buf[PATH_MAX], buf2[PATH_MAX];
    time_t time;
    PM_Photo *photo;
    int save = 0;
    int ret;
    int delete_photo = 0;
    int save_photo_list = 0;

    ASSERT_RETURN(album != NULL);
    ASSERT_RETURN(file != NULL);

    snprintf(buf_path,PATH_MAX,"%s/%s",pm_album_path_get(album),pm_album_file_name_get(album));
    snprintf(buf_file, PATH_MAX, "%s/%s", buf_path, file);
    snprintf(buf,PATH_MAX,"%s/"EET_FILE,buf_path);
    snprintf(buf2, PATH_MAX, "/photo %s", file);

    photo = pm_photo_eet_load(buf, buf2);

    if(!photo)
    {
        //new photo
        save_photo_list = 1;
        photo = pm_photo_new();
        pm_photo_name_set(photo, file);
        pm_photo_file_name_set(photo, file);

        FILE_LASTCHANGE_TIME_GET(buf_file, time);
        pm_photo_time_set(photo, time);
        pm_photo_path_set(photo, buf_path);

        pm_album_photo_add(album, photo);
        sync->msg.type = PM_SYNC_PHOTO_NEW;
        sync->msg.album = album;
        sync->msg.photo = photo;
        ecore_pipe_write(sync->pipe.thread_main, "a", 1);
        pthread_mutex_lock(&(sync->mutex));

        save = 1;
    }
    else
    {
        delete_photo = 1;
        FILE_LASTCHANGE_TIME_GET(buf_file, time);
        if(time > pm_photo_time_get(photo))
        {
            pm_photo_time_set(photo, time);

            sync->msg.type = PM_SYNC_PHOTO_UPDATE;
            sync->msg.album = album;
            sync->msg.photo = photo;
            ecore_pipe_write(sync->pipe.thread_main, "a", 1);
            pthread_mutex_lock(&(sync->mutex));
            save = 1;
        }
    }

    if(save)
    {
        ret = pm_photo_eet_save(photo);
        if(!ret)
        {
            snprintf(buf, 1024, "Failed to save the photo \"%s\" in the file %s/%s/"EET_FILE, pm_photo_name_get(photo), sync->path, pm_album_file_name_get(album));

            sync->msg.type = PM_SYNC_ERROR;
            sync->msg.error = Sync_Error_Eet_Save_Failed;
            sync->msg.msg = buf;
            ecore_pipe_write(sync->pipe.thread_main, "a", 1);
            pthread_mutex_lock(&(sync->mutex));
        }
    }
    if(delete_photo)
        pm_photo_free(&photo);

    return save_photo_list;
}

static void _pm_sync_album_album_sync(Photo_Manager_Sync *sync, PM_Album *album)
{
    char buf_path[PATH_MAX], buf_eet[PATH_MAX];
    Eina_List *l_files, *_l_files = NULL;
    Eina_List *l, *l2, *l_next;
    char *file;
    PM_Photo *photo;
    int save_album_list = 0;

    snprintf(buf_path,PATH_MAX,"%s/%s",pm_album_path_get(album),pm_album_file_name_get(album));
    snprintf(buf_eet,PATH_MAX,"%s/"EET_FILE,buf_path);
    if(!ecore_file_exists(buf_path))
    {
        return ;
    }

    l_files = ecore_file_ls(buf_path);
    //clean l_files, keep only the photos
    EINA_LIST_FREE(l_files, file)
    {
        if(pm_photo_is(file))
            _l_files = eina_list_append(_l_files, eina_stringshare_add(file));
        FREE(file);
    }
    l_files = _l_files;

    //crosschecking
    pm_album_photos_get(album);
    EINA_LIST_FOREACH(l_files, l, file)
    {
        if(_pm_sync_album_photo_file(sync, album, file))
            save_album_list = 1;
    }

    EINA_LIST_FOREACH_SAFE(pm_album_photos_get(album), l, l_next, photo)
    {
        int find = 0;
        EINA_LIST_FOREACH(l_files, l2, file)
        {
            if(pm_photo_file_name_get(photo) == file)
            {
                find = 1;
                break;
            }
        }
        if(!find)
        {
            pm_album_photo_remove(album, photo);
            pm_photo_eet_remove(buf_eet, pm_photo_file_name_get(photo));

            sync->msg.type = PM_SYNC_PHOTO_DISAPPEAR;
            sync->msg.album = album;
            sync->msg.photo = photo;
            ecore_pipe_write(sync->pipe.thread_main, "a", 1);
            pthread_mutex_lock(&(sync->mutex));

            pm_photo_free(&photo);
            save_album_list = 1;
        }
    }

    if(save_album_list)
        pm_album_eet_photos_list_save(album);

    EINA_LIST_FREE(l_files, file)
        eina_stringshare_del(file);
}


/**
 * @brief Synchronise a specific album folder
 * @param sync the sync struct
 * @param folder the album folder name
 */
static void _pm_sync_album_folder_start(Photo_Manager_Sync *sync, const char *folder)
{
    char buf_album[PATH_MAX], buf[PATH_MAX];
    int ret;
    int folder_exist;
    PM_Album *album = NULL;
    int save = 0;
    time_t time;

    PM_Root *root = pm_root_new(NULL,NULL,NULL,NULL,NULL,NULL,NULL);
    pm_root_path_set(root, sync->path);

    snprintf(buf_album, PATH_MAX, "%s/%s", pm_root_path_get(root), folder);
    //load the album from the eet file
    album = pm_root_eet_album_load(root, folder);
    //test if the folder exists
    folder_exist = ecore_file_exists(buf_album);
    //

    if(folder_exist)
        FILE_LASTCHANGE_TIME_GET(buf_album, time);

    if(!album && folder_exist)
    {
        //create the new album
        album = pm_album_new();
        pm_album_path_set(album, pm_root_path_get(root));
        pm_album_file_name_set(album, folder);
        pm_album_name_set(album, folder);
        pm_album_time_set(album, time);

        //send notif new album
        sync->msg.type = PM_SYNC_ALBUM_NEW;
        sync->msg.root = root;
        sync->msg.album = album;
        ecore_pipe_write(sync->pipe.thread_main, "a", 1);
        pthread_mutex_lock(&(sync->mutex));

        save = 1;

        //update the list of album
        PM_Root *root_list = pm_root_eet_albums_load(root);
        if(!root_list) root_list = pm_root_new(NULL,NULL,NULL,NULL,NULL,NULL,NULL);
        pm_root_path_set(root_list, sync->path);
        PM_Album *album_list = pm_album_new();
        pm_album_path_set(album_list, pm_album_path_get(album));
        pm_album_file_name_set(album_list, pm_album_file_name_get(album));
        pm_root_album_add(root_list, album_list);
        pm_root_eet_albums_save(root_list);
        pm_root_free(&root_list);
    }
    else if(album && folder_exist && time > pm_album_time_get(album))
    {
        pm_album_time_set(album, time);
        //send notif update album
        sync->msg.type = PM_SYNC_ALBUM_UPDATE;
        sync->msg.root = root;
        sync->msg.album = album;
        ecore_pipe_write(sync->pipe.thread_main, "a", 1);
        pthread_mutex_lock(&(sync->mutex));

        save = 1;
    }
    else if(album && !folder_exist)
    {
        //the album is referenced in the eet file but the folder does not exists

        //send notif
        sync->msg.type = PM_SYNC_ALBUM_DISAPPEAR;
        sync->msg.root = root;
        sync->msg.album = album;
        ecore_pipe_write(sync->pipe.thread_main, "a", 1);
        pthread_mutex_lock(&(sync->mutex));

        //update the list of album
        PM_Root *root_list = pm_root_eet_albums_load(root);
        if(root_list)
        {
            PM_Album *album_list = pm_root_album_search_file_name(root_list, folder);
            if(album_list)
            {
                pm_root_path_set(root_list, sync->path);
                pm_root_album_remove(root_list, album_list);
                pm_root_eet_albums_save(root_list);
                pm_album_free(&album_list);
            }
            pm_root_free(&root_list);
        }

        pm_root_eet_album_remove(root, folder);
    }
    else
        goto done ;

    if(save)
    {
        ret = pm_album_eet_global_header_save(album);
        if(!ret)
        {
            snprintf(buf, 1024, "Failed to save the album \"%s\" in the file %s/"EET_FILE, pm_album_name_get(album), sync->path);
            sync->msg.type = PM_SYNC_ERROR;
            sync->msg.error = Sync_Error_Eet_Save_Failed;
            sync->msg.msg = buf;
            ecore_pipe_write(sync->pipe.thread_main, "a", 1);
            pthread_mutex_lock(&(sync->mutex));
        }
    }

    _pm_sync_album_album_sync(sync, album);

    if(album)
        pm_album_free(&album);
done:
    pm_root_free(&root);
}






static int _pm_sync_all_photo_file(Photo_Manager_Sync *sync, PM_Album *album, const char *file)
{
    char buf_path[PATH_MAX], buf_file[PATH_MAX], buf[PATH_MAX], buf2[PATH_MAX];
    time_t time;
    PM_Photo *photo;
    int save = 0;
    int ret;
    int delete_photo = 0;
    int save_photo_list = 0;

    ASSERT_RETURN(album != NULL);
    ASSERT_RETURN(file != NULL);

    snprintf(buf_path,PATH_MAX,"%s/%s",pm_album_path_get(album),pm_album_file_name_get(album));
    snprintf(buf_file, PATH_MAX, "%s/%s", buf_path, file);
    snprintf(buf,PATH_MAX,"%s/"EET_FILE,buf_path);
    snprintf(buf2, PATH_MAX, "/photo %s", file);

    photo = pm_photo_eet_load(buf, buf2);

    if(!photo)
    {
        //new photo
        save_photo_list = 1;
        photo = pm_photo_new();
        pm_photo_name_set(photo, file);
        pm_photo_file_name_set(photo, file);

        FILE_LASTCHANGE_TIME_GET(buf_file, time);
        pm_photo_time_set(photo, time);
        pm_photo_path_set(photo, buf_path);

        pm_album_photo_add(album, photo);
        sync->msg.type = PM_SYNC_PHOTO_NEW;
        sync->msg.album = album;
        sync->msg.photo = photo;
        ecore_pipe_write(sync->pipe.thread_main, "a", 1);
        pthread_mutex_lock(&(sync->mutex));

        save = 1;
    }
    else
    {
        delete_photo = 1;
        FILE_LASTCHANGE_TIME_GET(buf_file, time);
        if(time > pm_photo_time_get(photo))
        {
            pm_photo_time_set(photo, time);

            sync->msg.type = PM_SYNC_PHOTO_UPDATE;
            sync->msg.album = album;
            sync->msg.photo = photo;
            ecore_pipe_write(sync->pipe.thread_main, "a", 1);
            pthread_mutex_lock(&(sync->mutex));
            save = 1;
        }
    }

    if(save)
    {
        ret = pm_photo_eet_save(photo);
        if(!ret)
        {
            snprintf(buf, 1024, "Failed to save the photo \"%s\" in the file %s/%s/"EET_FILE, pm_photo_name_get(photo), sync->path, pm_album_file_name_get(album));

            sync->msg.type = PM_SYNC_ERROR;
            sync->msg.error = Sync_Error_Eet_Save_Failed;
            sync->msg.msg = buf;
            ecore_pipe_write(sync->pipe.thread_main, "a", 1);
            pthread_mutex_lock(&(sync->mutex));
        }
    }
    if(delete_photo)
        pm_photo_free(&photo);

    return save_photo_list;
}

static void _pm_sync_all_album_sync(Photo_Manager_Sync *sync, PM_Album *album)
{
    char buf_path[PATH_MAX], buf_eet[PATH_MAX];
    Eina_List *l_files, *_l_files = NULL;
    Eina_List *l, *l2, *l_next;
    char *file;
    PM_Photo *photo;
    int save_album_list = 0;


    snprintf(buf_path,PATH_MAX,"%s/%s",pm_album_path_get(album),pm_album_file_name_get(album));
    snprintf(buf_eet,PATH_MAX,"%s/"EET_FILE,buf_path);
    if(!ecore_file_exists(buf_path))
    {
        return ;
    }

    l_files = ecore_file_ls(buf_path);
    //clean l_files, keep only the photos
    EINA_LIST_FREE(l_files, file)
    {
        if(pm_photo_is(file))
            _l_files = eina_list_append(_l_files, eina_stringshare_add(file));
        FREE(file);
    }
    l_files = _l_files;

    //crosschecking
    pm_album_photos_get(album);
    EINA_LIST_FOREACH(l_files, l, file)
    {
        if(_pm_sync_all_photo_file(sync, album, file))
            save_album_list = 1;
    }

    EINA_LIST_FOREACH_SAFE(pm_album_photos_get(album), l, l_next, photo)
    {
        int find = 0;
        EINA_LIST_FOREACH(l_files, l2, file)
        {
            if(pm_photo_file_name_get(photo) == file)
            {
                find = 1;
                break;
            }
        }
        if(!find)
        {
            pm_album_photo_remove(album, photo);
            pm_photo_eet_remove(buf_eet, pm_photo_file_name_get(photo));

            sync->msg.type = PM_SYNC_PHOTO_DISAPPEAR;
            sync->msg.album = album;
            sync->msg.photo = photo;
            ecore_pipe_write(sync->pipe.thread_main, "a", 1);
            pthread_mutex_lock(&(sync->mutex));

            pm_photo_free(&photo);
            save_album_list = 1;
        }
    }

    if(save_album_list)
        pm_album_eet_photos_list_save(album);

    EINA_LIST_FREE(l_files, file)
        eina_stringshare_del(file);
}

static int _pm_sync_all_album_folder(Photo_Manager_Sync *sync, PM_Root *root_list, const char *file)
{
    char buf[PATH_MAX];
    int ret;
    int save_album_list = 0;

    PM_Root *root = pm_root_new(NULL,NULL,NULL,NULL,NULL,NULL,NULL);
    pm_root_path_set(root, sync->path);

    //
    PM_Album *album= pm_album_new();
    pm_album_path_set(album, sync->path);
    pm_album_file_name_set(album, file);
    pm_album_name_set(album, file);

    time_t time;
    snprintf(buf, PATH_MAX, "%s/%s", pm_album_path_get(album), pm_album_file_name_get(album));
    FILE_LASTCHANGE_TIME_GET(buf, time);

    pm_album_time_set(album, time);
    //

    //compare to the eet file
    PM_Album *_album = pm_root_eet_album_load(root, pm_album_file_name_get(album));
    int save = 0;
    if(!_album)
    {
        save_album_list = 1;
        //send notif new album
        sync->msg.type = PM_SYNC_ALBUM_NEW;
        sync->msg.root = root;
        sync->msg.album = album;
        ecore_pipe_write(sync->pipe.thread_main, "a", 1);
        pthread_mutex_lock(&(sync->mutex));

        save = 1;
        PM_Album *album_list = pm_album_new();
        pm_album_path_set(album_list, pm_album_path_get(album));
        pm_album_file_name_set(album_list, pm_album_file_name_get(album));
        pm_root_album_add(root_list, album_list);
    }

    //compare the dates
    if(!save && pm_album_time_get(album) > pm_album_time_get(_album))
    {
        //send notif update album
        sync->msg.type = PM_SYNC_ALBUM_UPDATE;
        sync->msg.root = root;
        sync->msg.album = album;
        ecore_pipe_write(sync->pipe.thread_main, "a", 1);
        pthread_mutex_lock(&(sync->mutex));

        save = 1;
    }

    if(save)
    {
        ret = pm_album_eet_global_header_save(album);
        if(!ret)
        {
            snprintf(buf, 1024, "Failed to save the album \"%s\" in the file %s/"EET_FILE, pm_album_name_get(album), sync->path);
            sync->msg.type = PM_SYNC_ERROR;
            sync->msg.error = Sync_Error_Eet_Save_Failed;
            sync->msg.msg = buf;
            ecore_pipe_write(sync->pipe.thread_main, "a", 1);
            pthread_mutex_lock(&(sync->mutex));
        }
    }

    if(_album) pm_album_free(&_album);

    _pm_sync_all_album_sync(sync, album);
    pm_root_free(&root);
    pm_album_free(&album);

    return save_album_list;
}

/**
 * @brief Synchronise a photo manager folder
 * @param data a sync struct
 */
static void _pm_sync_all_start(void *data)
{
    Photo_Manager_Sync *sync = data;
    PM_Root *root;
    char buf[PATH_MAX];
    Eina_List *l_files;
    Eina_List *l;
    char *file;
    int save_album_list = 0;

    root = pm_root_new(NULL,NULL,NULL,NULL,NULL,NULL, NULL);
    pm_root_path_set(root, sync->path);

    l_files = ecore_file_ls(pm_root_path_get(root));

    PM_Root *root_list = pm_root_eet_albums_load(root);
    if(!root_list) root_list = pm_root_new(NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    pm_root_path_set(root_list, sync->path);

    //search new/updated album
    EINA_LIST_FOREACH(l_files, l, file)
    {
        snprintf(buf, PATH_MAX, "%s/%s", pm_root_path_get(root),file);
        if(ecore_file_is_dir(buf))
            if(_pm_sync_all_album_folder(sync, root_list, file))
                save_album_list = 1;
    }

    char *s;
    EINA_LIST_FREE(l_files, s)
        FREE(s);

    //search deleted pm
    PM_Album *album_list;
    EINA_LIST_FOREACH(pm_root_albums_get(root_list), l, album_list)
    {
        snprintf(buf,PATH_MAX,"%s/%s", pm_root_path_get(root_list), pm_album_file_name_get(album_list));
        if(!ecore_file_exists(buf))
        {
            sync->msg.type = PM_SYNC_ALBUM_DISAPPEAR;
            sync->msg.root = root;
            sync->msg.album = album_list;
            ecore_pipe_write(sync->pipe.thread_main, "a", 1);
            pthread_mutex_lock(&(sync->mutex));

            pm_root_album_remove(root_list, album_list);
            pm_root_eet_album_remove(root_list, pm_album_file_name_get(album_list));
            pm_album_free(&album_list);
            save_album_list = 1;
        }
    }
    if(save_album_list)
        pm_root_eet_albums_save(root_list);

    pm_root_free(&root_list);

    pm_root_free(&root);

    sync->msg.type = PM_SYNC_DONE;
    ecore_pipe_write(sync->pipe.thread_main, "a", 1);
}

/**
 * @brief Handle which is trigger when the thread send a message (write in the pipe).
 * This method process the message and call the corresponded callback.
 * @data a sync struct (which contains the message)
 * @buffer the message send in the pipe (not used)
 * @nbyte the size of @message (not used)
 */
static void _pm_sync_message_cb(void *data, void *buffer, unsigned int nbyte)
{
    Photo_Manager_Sync *sync = (Photo_Manager_Sync*) data;

    if(sync->msg.type == PM_SYNC_DONE)
    {
        LOG_INFO("All files are synchronised");
        //The thread is not in pause.
        sync->sync.done_cb(sync->sync.data, sync);
        return ;
    }

    switch(sync->msg.type)
    {
        case PM_SYNC_ERROR:
            LOG_CRIT(sync->msg.msg);
            sync->sync.error_cb(sync->sync.data, sync, sync->msg.error, sync->msg.msg);
            break;
        case PM_SYNC_ALBUM_NEW:
            LOG_INFO("New album : %s", pm_album_file_name_get(sync->msg.album));
            sync->sync.album_new_cb(sync->sync.data, sync, sync->msg.root, sync->msg.album);
            break;
        case PM_SYNC_ALBUM_UPDATE:
            LOG_INFO("Update album : %s", pm_album_file_name_get(sync->msg.album));
            sync->sync.album_update_cb(sync->sync.data, sync, sync->msg.root, sync->msg.album);
            break;
        case PM_SYNC_ALBUM_DISAPPEAR:
            LOG_INFO("The album is referenced in the eet file but the folder does not exists: %s", pm_album_file_name_get(sync->msg.album));
            sync->sync.album_disappear_cb(sync->sync.data, sync, sync->msg.root, sync->msg.album);
            break;
        case PM_SYNC_PHOTO_NEW:
            LOG_INFO("New photo \"%s\" in the album : %s", pm_photo_file_name_get(sync->msg.photo), pm_album_file_name_get(sync->msg.album));
            sync->sync.photo_new_cb(sync->sync.data, sync, sync->msg.album, sync->msg.photo);
            break;
        case PM_SYNC_PHOTO_UPDATE:
            LOG_INFO("Update photo \"%s\" of the album : %s", pm_photo_name_get(sync->msg.photo),pm_album_file_name_get(sync->msg.album));
            sync->sync.photo_update_cb(sync->sync.data, sync, sync->msg.album, sync->msg.photo);
            break;
        case PM_SYNC_PHOTO_DISAPPEAR:
            LOG_INFO("The photo \"%s\" of the album %s is referenced in the eet file but the file does not exists", pm_photo_name_get(sync->msg.photo), pm_album_file_name_get(sync->msg.album));
            sync->sync.photo_disappear_cb(sync->sync.data, sync, sync->msg.album, sync->msg.photo);
            break;
        case PM_SYNC_DONE: ;
    }
    pthread_mutex_unlock(&(sync->mutex));
}

/**
 * @brief Method called when a synchronisation is done. The method
 * start a new job if the list is not empty
 * @param data a sync struct
 */
static void _pm_sync_end_cb(void *data)
{
    Photo_Manager_Sync *sync = (Photo_Manager_Sync*) data;

    sync->is_running = 0;
    PM_Sync_Job *job = eina_list_data_get(sync->jobs);
    sync->jobs = eina_list_remove(sync->jobs, job);
    _pm_sync_job_free(&job);
    _pm_sync_next_job_process(sync);
}

