#include "enlil_private.h"


static void _job_free(Enlil_Download_Job *job);
static void _job_next();
static void _done_cb(void *data, const char *file, int status);
static int _progress_cb(void *data, const char *file, long int dltotal, long int dlnow, long int ultotal, long int ulnow);

struct Enlil_Download_Job
{
    Enlil_Photo *photo;
    const char *source;

    long int last_dlnow;

    Enlil_Download_Start_Cb start_cb;
    Enlil_Download_Progress_Cb progress_cb;
    Enlil_Download_Done_Cb done_cb;
    void *data;

    Ecore_File_Download_Job *job;
};


static Eina_List *jobs = NULL;
static Enlil_Download_Job *current_job;
static Eina_Bool running = EINA_FALSE;

static void _job_free(Enlil_Download_Job *job)
{
    ASSERT_RETURN_VOID(job != NULL);

    EINA_STRINGSHARE_DEL(job->source);
    free(job);
}


void enlil_download_add(Enlil_Photo *photo, const char *source,
        Enlil_Download_Start_Cb start_cb,
        Enlil_Download_Progress_Cb progress_cb,
        Enlil_Download_Done_Cb done_cb,
        void *data)
{
    Enlil_Download_Job *job;

    ASSERT_RETURN_VOID(photo!=NULL);
    ASSERT_RETURN_VOID(source != NULL);

    //maybe the job is already in the list
    Eina_List *l;
    EINA_LIST_FOREACH(jobs, l, job)
    {
        if(job->photo == photo && job->source == source)
            break;
    }

    if(job)
        return ;

    job = calloc(1, sizeof(Enlil_Download_Job));
    job->photo = photo;
    job->source = eina_stringshare_add(source);
    job->done_cb = done_cb;
    job->start_cb = start_cb;
    job->progress_cb = progress_cb;
    job->data = data;
    jobs = eina_list_append(jobs, job);

    _job_next();
}


static void _job_next()
{
    char buf[PATH_MAX];
    int i = 0;

    if(running)
        return;

    if(!jobs)
        return ;

    current_job = eina_list_data_get(jobs);
    jobs = eina_list_remove(jobs, current_job);

    running = EINA_TRUE;

    do {
        snprintf(buf, PATH_MAX, TEMP"enlil_downloads/%d", i);
        i++;
    } while (ecore_file_exists(buf));

    if(current_job->start_cb)
        current_job->start_cb(current_job->data, current_job->photo);

    if(!ecore_file_exists(TEMP"enlil_downloads"))
        ecore_file_mkdir(TEMP"enlil_downloads");

    LOG_INFO("Start download photo %s in %s", current_job->source, buf);
    ecore_file_download(current_job->source,
            buf,
            _done_cb,
            _progress_cb,
            current_job,
            &(current_job->job));
}


static void _done_cb(void *data, const char *file, int status)
{
    char buf[PATH_MAX];
    int i = 0;

    if(current_job != data) //job deleted
        return ;

    snprintf(buf, PATH_MAX, "%s/%s", enlil_photo_path_get(current_job->photo),
            enlil_photo_file_name_get(current_job->photo));

    while(ecore_file_exists(buf))
    {
        snprintf(buf, PATH_MAX, "%d_%s", i, enlil_photo_file_name_get(current_job->photo));

        enlil_photo_file_name_set(current_job->photo, buf);

        snprintf(buf, PATH_MAX, "%s/%d_%s", enlil_photo_path_get(current_job->photo),
            i, enlil_photo_file_name_get(current_job->photo));
    }

    enlil_album_photo_add(enlil_photo_album_get(current_job->photo), current_job->photo);
    enlil_album_eet_photos_list_save(enlil_photo_album_get(current_job->photo));
    enlil_photo_eet_save(current_job->photo);

    ecore_file_mv(file, buf);

    if(current_job->done_cb)
        current_job->done_cb(current_job->data, current_job->photo, status);

    if(status)
        LOG_ERR("Download failed (%d)", status);

    _job_free(current_job);
    current_job = NULL;
    running = EINA_FALSE;
    _job_next();
}

static int _progress_cb(void *data, const char *file, long int dltotal, long int dlnow, long int ultotal, long int ulnow)
{
    if(current_job != data) //job deleted
        return 1;

    if(current_job->last_dlnow == dlnow)
        return 0;

    current_job->last_dlnow = dlnow;

    if(current_job->progress_cb)
        return current_job->progress_cb(current_job->data, current_job->photo, dltotal, dlnow);

    return 0;
}





