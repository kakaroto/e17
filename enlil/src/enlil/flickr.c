// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
#include "enlil_private.h"

#include "../../config.h"

#ifdef HAVE_FLICKR
#include "flickcurl.h"
#endif

typedef enum Enlil_Flickr_Job_Type Enlil_Flickr_Job_Type;

enum Enlil_Flickr_Job_Type
{
    ENLIL_FLICKR_JOB_REINIT,
    ENLIL_FLICKR_JOB_GET_AUTHTOKEN,
    ENLIL_FLICKR_JOB_CMP_ALBUM_HEADER,
    ENLIL_FLICKR_JOB_CMP_ALBUMS_HEADER,
    ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_UPDATE_FLICKR,
    ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_UPDATE_LOCAL,
    ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR,
    ENLIL_FLICKR_JOB_CMP_ALBUM_PHOTOS,
    ENLIL_FLICKR_JOB_GET_PHOTO_SIZES
};

struct Enlil_Flickr_Job
{
    Enlil_Flickr_Job_Type type;

    Enlil_Album *album;
    Enlil_Root *root;
    const char *frob;
    const char *photo_id;

    struct
    {
       Eina_Bool error;
#ifdef HAVE_FLICKR
       flickcurl_photoset** photosets;
       flickcurl_photoset* photoset;
       flickcurl_photo** photos;
       flickcurl_size** photo_sizes;
#endif
       const char *photoset_id;
    } response;

    Enlil_Flickr_Album_New_Cb album_new_cb;
    Enlil_Flickr_Album_NotInFlickr_Cb album_notinflickr_cb;
    Enlil_Flickr_Album_NotUpToDate_Cb album_notuptodate_cb;
    Enlil_Flickr_Album_FlickrNotUpToDate_Cb album_flickrnotuptodate_cb;
    Enlil_Flickr_Album_UpToDate_Cb album_uptodate_cb;
    Enlil_Flickr_Album_Done_Cb album_done_cb;

    Enlil_Flickr_Photo_New_Cb photo_new_cb;
    Enlil_Flickr_Photo_NotInFlickr_Cb photo_notinflickr_cb;
    Enlil_Flickr_Photo_NotUpToDate_Cb photo_notuptodate_cb;
    Enlil_Flickr_Photo_FlickrNotUpToDate_Cb photo_flickrnotuptodate_cb;
    Enlil_Flickr_Photo_UpToDate_Cb photo_uptodate_cb;
    Enlil_Flickr_Photo_Done_Cb photo_done_cb;

    Enlil_Flickr_Error_Cb error_cb;
    Enlil_Flickr_Album_Error_Cb album_error_cb;

    Enlil_Flickr_Photo_Sizes_Cb photo_sizes_cb;

    void *data;
};

struct Enlil_Flickr_Photo_Size
{
   const char *label;
   int width;
   int height;
   const char *source;
   const char *url;
   const char* media;
   int order; //the bigger number is the bigger image
};

#ifdef HAVE_FLICKR
static flickcurl *fc = NULL;
#endif
static Eina_List *l_jobs;
static Enlil_Flickr_Job *job_current = NULL;
static Eina_Bool running = EINA_FALSE;

static void _job_next();
static void _flickr_thread(void *data);
static void _end_cb(void *data);

static Enlil_Flickr_Job *_enlil_flickr_job_get_authtoken_prepend(Enlil_Root *root, const char *code);
static int _connect(Enlil_Root *root, const char *code);
static int _disconnect();



const char *enlil_flickr_auth_url_get()
{
    return FLICKR_AUTH_URL;
}

Eina_Bool enlil_flickr_have()
{
#ifdef HAVE_FLICKR
   return EINA_TRUE;
#else
   return EINA_FALSE;
#endif
}

Enlil_Flickr_Job *enlil_flickr_job_reinit_prepend(Enlil_Root *root)
{
    Enlil_Flickr_Job *job;
    Eina_List *l;

    EINA_LIST_FOREACH(l_jobs, l, job)
        if(job->type == ENLIL_FLICKR_JOB_REINIT)
            break;

    if(!job)
    {
        job = calloc(1, sizeof(Enlil_Flickr_Job));
        job->root = root;
        job->type = ENLIL_FLICKR_JOB_REINIT;

        l_jobs = eina_list_prepend(l_jobs, job);
    }

    _job_next();

    return job;
}

Enlil_Flickr_Job *enlil_flickr_job_sync_album_header_update_flickr_append(Enlil_Album *album,
        Enlil_Flickr_Album_Done_Cb done_cb,
        void *data)
{
    Enlil_Flickr_Job *job;
    Eina_List *l;

    ASSERT_RETURN(album != NULL);

    EINA_LIST_FOREACH(l_jobs, l, job)
        if(job->album == album && job->type == ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_UPDATE_FLICKR)
            break;

    if(!job)
    {
        job = calloc(1, sizeof(Enlil_Flickr_Job));
        job->album = album;
        job->root = enlil_album_root_get(album);
        job->type = ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_UPDATE_FLICKR;

        job->album_done_cb = done_cb;
        job->data = data;

        l_jobs = eina_list_append(l_jobs, job);
    }

    _job_next();

    return job;
}

Enlil_Flickr_Job *enlil_flickr_job_sync_album_header_update_local_append(Enlil_Album *album,
        Enlil_Flickr_Album_Done_Cb done_cb,
        void *data)
{
    Enlil_Flickr_Job *job;
    Eina_List *l;

    ASSERT_RETURN(album != NULL);

    EINA_LIST_FOREACH(l_jobs, l, job)
        if(job->album == album && job->type == ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_UPDATE_LOCAL)
            break;

    if(!job)
    {
        job = calloc(1, sizeof(Enlil_Flickr_Job));
        job->album = album;
        job->root = enlil_album_root_get(album);
        job->type = ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_UPDATE_LOCAL;

        job->album_done_cb = done_cb;
        job->data = data;

        l_jobs = eina_list_append(l_jobs, job);
    }

    _job_next();

    return job;
}

Enlil_Flickr_Job *enlil_flickr_job_sync_album_header_create_flickr_append(Enlil_Album *album,
        Enlil_Flickr_Album_Done_Cb done_cb,
        void *data)
{
    Enlil_Flickr_Job *job;
    Eina_List *l;

    ASSERT_RETURN(album != NULL);

    EINA_LIST_FOREACH(l_jobs, l, job)
        if(job->album == album && job->type == ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR)
            break;

    if(!job)
    {
        job = calloc(1, sizeof(Enlil_Flickr_Job));
        job->album = album;
        job->root = enlil_album_root_get(album);
        job->type = ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR;

        job->album_done_cb = done_cb;
        job->data = data;

        l_jobs = eina_list_append(l_jobs, job);
    }

    _job_next();

    return job;
}

Enlil_Flickr_Job *enlil_flickr_job_sync_album_header_append(Enlil_Album *album,
        Enlil_Flickr_Album_New_Cb new_cb,
        Enlil_Flickr_Album_NotInFlickr_Cb notinflickr_cb,
        Enlil_Flickr_Album_NotUpToDate_Cb notuptodate_cb,
        Enlil_Flickr_Album_FlickrNotUpToDate_Cb flickrnotuptodate_cb,
	Enlil_Flickr_Album_UpToDate_Cb uptodate_cb,
        Enlil_Flickr_Error_Cb error_cb,
        void *data)
{
    Enlil_Flickr_Job *job;
    Eina_List *l;

    ASSERT_RETURN(album != NULL);

    EINA_LIST_FOREACH(l_jobs, l, job)
        if(job->type == ENLIL_FLICKR_JOB_CMP_ALBUM_HEADER && job->album == album)
            break;

    if(!job)
    {
        job = calloc(1, sizeof(Enlil_Flickr_Job));
        job->root = enlil_album_root_get(album);
        job->album = album;
        job->type = ENLIL_FLICKR_JOB_CMP_ALBUM_HEADER;
        job->album_new_cb = new_cb;
        job->album_notinflickr_cb = notinflickr_cb;
        job->album_notuptodate_cb = notuptodate_cb;
        job->album_flickrnotuptodate_cb = flickrnotuptodate_cb;
	job->album_uptodate_cb = uptodate_cb;
        job->error_cb = error_cb;
        job->data = data;

        l_jobs = eina_list_append(l_jobs, job);
    }

    _job_next();

    return job;
}

Enlil_Flickr_Job *enlil_flickr_job_sync_albums_append(Enlil_Root *root,
        Enlil_Flickr_Album_New_Cb new_cb,
        Enlil_Flickr_Album_NotInFlickr_Cb notinflickr_cb,
        Enlil_Flickr_Album_NotUpToDate_Cb notuptodate_cb,
        Enlil_Flickr_Album_FlickrNotUpToDate_Cb flickrnotuptodate_cb,
	Enlil_Flickr_Album_UpToDate_Cb uptodate_cb,
        Enlil_Flickr_Error_Cb error_cb,
        void *data)
{
    Enlil_Flickr_Job *job;
    Eina_List *l;

    ASSERT_RETURN(root != NULL);

    EINA_LIST_FOREACH(l_jobs, l, job)
        if(job->type == ENLIL_FLICKR_JOB_CMP_ALBUMS_HEADER)
            break;

    if(!job)
    {
        job = calloc(1, sizeof(Enlil_Flickr_Job));
        job->root = root;
        job->type = ENLIL_FLICKR_JOB_CMP_ALBUMS_HEADER;
        job->album_new_cb = new_cb;
        job->album_notinflickr_cb = notinflickr_cb;
        job->album_notuptodate_cb = notuptodate_cb;
        job->album_flickrnotuptodate_cb = flickrnotuptodate_cb;
	job->album_uptodate_cb = uptodate_cb;
        job->error_cb = error_cb;
        job->data = data;

        l_jobs = eina_list_append(l_jobs, job);
    }

    _job_next();

    return job;
}

Enlil_Flickr_Job *enlil_flickr_job_sync_album_photos_append(Enlil_Album *album,
        Enlil_Flickr_Photo_New_Cb new_cb,
        Enlil_Flickr_Photo_NotInFlickr_Cb notinflickr_cb,
        Enlil_Flickr_Photo_NotUpToDate_Cb notuptodate_cb,
        Enlil_Flickr_Photo_FlickrNotUpToDate_Cb flickrnotuptodate_cb,
	Enlil_Flickr_Photo_UpToDate_Cb uptodate_cb,
        Enlil_Flickr_Album_Error_Cb album_error_cb,
        void *data)
{
    Enlil_Flickr_Job *job;
    Eina_List *l;

    ASSERT_RETURN(album != NULL);

    EINA_LIST_FOREACH(l_jobs, l, job)
        if(job->type == ENLIL_FLICKR_JOB_CMP_ALBUM_PHOTOS && job->album == album)
            break;

    if(!job)
    {
        job = calloc(1, sizeof(Enlil_Flickr_Job));
        job->root = enlil_album_root_get(album);
        job->album = album;
        job->type = ENLIL_FLICKR_JOB_CMP_ALBUM_PHOTOS;
        job->photo_new_cb = new_cb;
        job->photo_notinflickr_cb = notinflickr_cb;
        job->photo_notuptodate_cb = notuptodate_cb;
        job->photo_flickrnotuptodate_cb = flickrnotuptodate_cb;
	job->photo_uptodate_cb = uptodate_cb;
        job->album_error_cb = album_error_cb;
        job->data = data;

        l_jobs = eina_list_append(l_jobs, job);
    }

    _job_next();

    return job;
}

Enlil_Flickr_Job *enlil_flickr_job_get_photo_sizes_append(const char *photo_id,
        Enlil_Flickr_Photo_Sizes_Cb cb,
        void *data)
{
    Enlil_Flickr_Job *job;
    Eina_List *l;

    ASSERT_RETURN(photo_id != NULL);

    EINA_LIST_FOREACH(l_jobs, l, job)
        if(job->type == ENLIL_FLICKR_JOB_GET_PHOTO_SIZES && job->photo_id == photo_id)
            break;

    if(!job)
    {
        job = calloc(1, sizeof(Enlil_Flickr_Job));
        job->photo_id = eina_stringshare_add(photo_id);
        job->type = ENLIL_FLICKR_JOB_GET_PHOTO_SIZES;
        job->photo_sizes_cb = cb;
        job->data = data;

        l_jobs = eina_list_append(l_jobs, job);
    }

    _job_next();

    return job;
}


static void _job_next()
{
    if(running)
        return;

    if(!l_jobs)
        return ;

#ifdef HAVE_FLICKR
    Enlil_Flickr_Job *job = eina_list_data_get(l_jobs);
    job_current = job;

    if(!fc)
    {
        Enlil_Root *root = job->root;

        if(!root)
        {
            LOG_CRIT("The job is not associated to a library !");
            return ;
        }
        else if(!enlil_root_flickr_account_get(root))
        {
            LOG_CRIT("The library is not associated to a flickr account !");
            return ;
        }

        if(!_connect(root, enlil_root_flickr_account_get(root)))
            return ;
    }

    l_jobs = eina_list_remove(l_jobs, job);
    running = 1;

    ecore_thread_run(_flickr_thread, _end_cb, NULL, NULL);
#else
    LOG_ERR("Flickr support is not built");
#endif
}

static void _flickr_thread(void *data)
{
    char *token;

    Enlil_Flickr_Job *job = job_current;

    if(!job) return ;

#ifdef HAVE_FLICKR
    switch(job_current->type)
    {
        case ENLIL_FLICKR_JOB_REINIT :
            _disconnect();
            break;
        case ENLIL_FLICKR_JOB_GET_AUTHTOKEN :
            token = flickcurl_auth_getFullToken(fc, job->frob);
            if(!token)
            {
                LOG_ERR("No Auth token associated to the frob");
                job->response.error = EINA_TRUE;
                break;
            }
            flickcurl_set_auth_token(fc, token);
            enlil_root_flickr_auth_token_set(job->root, token);
            break;
        case ENLIL_FLICKR_JOB_CMP_ALBUMS_HEADER :
            job->response.photosets = flickcurl_photosets_getList(fc, NULL);
            if(!job->response.photosets)
            {
                LOG_ERR("Can not retrieves the list of sets");
                job->response.error = EINA_TRUE;
                break;
            }
            break;
        case ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_UPDATE_FLICKR :
            if(flickcurl_photosets_editMeta(fc,
                        enlil_album_flickr_id_get(job->album),
                        enlil_album_name_get(job->album),
                        NULL))
            {
                job->response.error = EINA_TRUE;
                break;
            }
        case ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_UPDATE_LOCAL :
            job->response.photoset = flickcurl_photosets_getInfo(fc,
                    enlil_album_flickr_id_get(job->album));
            if(!job->response.photoset)
                job->response.error = EINA_TRUE;
            break;
        case ENLIL_FLICKR_JOB_CMP_ALBUM_HEADER :
            job->response.photoset = flickcurl_photosets_getInfo(fc,
                    enlil_album_flickr_id_get(job->album));
            if(!job->response.photoset)
                job->response.error = EINA_TRUE;
            break;
        case ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR :
            job->response.photoset_id = flickcurl_photosets_create(fc,
                    enlil_album_name_get(job->album),
                    NULL,
                    NULL,
                    NULL);
            if(!job->response.photoset_id)
                job->response.error = EINA_TRUE;
            break;
	case ENLIL_FLICKR_JOB_CMP_ALBUM_PHOTOS :
            job->response.photos = flickcurl_photosets_getPhotos(fc,
		  enlil_album_flickr_id_get(job->album), NULL, 0, 500, 0);
            if(!job->response.photos)
            {
                LOG_ERR("Can not retrieves the list of photos");
                job->response.error = EINA_TRUE;
                break;
            }
            break;
	case ENLIL_FLICKR_JOB_GET_PHOTO_SIZES :
	    job->response.photo_sizes = flickcurl_photos_getSizes(fc, job->photo_id);
            if(!job->response.photo_sizes)
            {
                LOG_ERR("Can not retrieves the list sizes");
                job->response.error = EINA_TRUE;
                break;
            }
            break;
    }
#else
    LOG_ERR("Flickr support is not built");
#endif
}

static void _end_cb(void *data)
{
    int i;
    char buf[PATH_MAX];
    Eina_List *l, *copy;
    Enlil_Album *album;
    Enlil_Photo *photo;
    Enlil_Flickr_Job *job = job_current;
    Eina_List *l_sizes = NULL;
    Enlil_Flickr_Photo_Size *size;

    running = 0;

    if(!job) goto end ;

#ifdef HAVE_FLICKR
    switch(job->type)
    {
        case ENLIL_FLICKR_JOB_CMP_ALBUMS_HEADER :
            if(job->response.error)
            {
                if(job->error_cb)
                    job->error_cb(job->data, job->root);
                break;
            }
            i = 0;

            copy = eina_list_clone(enlil_root_albums_get(job->root));

            while(job->response.photosets && job->response.photosets[i])
            {
                flickcurl_photoset *photoset = job->response.photosets[i];
                const char *id = eina_stringshare_add(photoset->id);

                EINA_LIST_FOREACH(copy, l, album)
                {
                    if(enlil_album_flickr_id_get(album) == id)
                    {
                        copy = eina_list_remove_list(copy, l);
                        break;
                    }
                }

                if(!album)
                {
                    //create the local album
                    LOG_INFO("The album '%s' exists in the Flickr account but not in the local library. Consequently we create it.", photoset->title);

                    Enlil_Album *album = enlil_album_new();
                    enlil_album_path_set(album, enlil_root_path_get(job->root));
                    enlil_album_name_set(album, photoset->title);
                    enlil_album_file_name_set(album, photoset->title);
                    _enlil_album_flickr_id_set(album, photoset->id);

                    snprintf(buf, PATH_MAX, "%s/%s", enlil_root_path_get(job->root), photoset->title);
                    if(ecore_file_exists(buf))
                    {
                        snprintf(buf, PATH_MAX, "%s_Flickr", photoset->title);
                        enlil_album_file_name_set(album, buf);

                        snprintf(buf, PATH_MAX, "%s/%s_Flickr", enlil_root_path_get(job->root),
                                photoset->title);
                    }
                    enlil_root_album_add(job->root, album);
                    enlil_root_eet_albums_save(job->root);
                    enlil_album_eet_header_save(album);

                    if(job->album_new_cb)
                        job->album_new_cb(job->data, job->root, album);

                    ecore_file_mkdir(buf);
                }
                else
                {
                    //check if both albums are different
                    if(!enlil_album_name_get(album) || !photoset->title
                            || strcmp(enlil_album_name_get(album), photoset->title) )
                    {
                        if(enlil_album_flickr_need_sync_get(album))
                        {
                            if(job->album_flickrnotuptodate_cb)
                                job->album_flickrnotuptodate_cb(job->data, job->root, album);
                        }
                        else
                        {
                            if(job->album_notuptodate_cb)
                                job->album_notuptodate_cb(job->data, job->root, album);
                        }
                    }
		    else if(job->album_uptodate_cb)
		      job->album_uptodate_cb(job->data, job->root, album);

                }

                EINA_STRINGSHARE_DEL(id);
                i++;
            }

            EINA_LIST_FREE(copy, album)
            {
                if(enlil_album_flickr_id_get(album))
                {
                    _enlil_album_flickr_id_set(album, NULL);
                    enlil_album_eet_header_save(album);
                }
                if(job->album_notinflickr_cb)
                    job->album_notinflickr_cb(job->data, job->root, album);
            }
            break;
        case ENLIL_FLICKR_JOB_CMP_ALBUM_HEADER :
            //check if both albums are different
            if(job->response.error)
            {
                if(job->error_cb)
                    job->error_cb(job->data, job->root);
                break;
            }

            album = job->album;
            if(!enlil_album_name_get(album) || !job->response.photoset->title
                    || strcmp(enlil_album_name_get(album), job->response.photoset->title) )
            {
                if(enlil_album_flickr_need_sync_get(album))
                {
                    if(job->album_flickrnotuptodate_cb)
                        job->album_flickrnotuptodate_cb(job->data, job->root, album);
                }
                else
                {
                    if(job->album_notuptodate_cb)
                        job->album_notuptodate_cb(job->data, job->root, album);
                }
            }
            break;
        case ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_UPDATE_FLICKR :
            if(!job->response.error)
            {
                enlil_album_flickr_need_sync_set(job->album, EINA_FALSE);
                enlil_album_eet_header_save(job->album);
            }
            if(job->album_done_cb)
                job->album_done_cb(job->data, job->root, job->album, job->response.error);
            break;
        case ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_UPDATE_LOCAL :
            if(!job->response.error)
            {
                enlil_album_flickr_need_sync_set(job->album, EINA_FALSE);
                enlil_album_name_set(job->album, job->response.photoset->title);
            }
            enlil_album_eet_header_save(job->album);
            if(job->album_done_cb)
                job->album_done_cb(job->data, job->root, job->album, job->response.error);
            break;
        case ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR :
            if(!job->response.error)
            {
                enlil_album_flickr_need_sync_set(job->album, EINA_FALSE);
                _enlil_album_flickr_id_set(job->album, job->response.photoset_id);
		enlil_album_eet_header_save(job->album);
            }
            if(job->album_done_cb)
                job->album_done_cb(job->data, job->root, job->album, job->response.error);
            break;
	case ENLIL_FLICKR_JOB_CMP_ALBUM_PHOTOS :
	    if(job->response.error)
            {
                if(job->error_cb)
                    job->album_error_cb(job->data, job->album);
                break;
            }
            i = 0;

            copy = eina_list_clone(enlil_album_photos_get(job->album));

            while(job->response.photos && job->response.photos[i])
            {
                flickcurl_photo *fc_photo = job->response.photos[i];
                const char *id = eina_stringshare_add(fc_photo->id);

                EINA_LIST_FOREACH(copy, l, photo)
                {
                    if(enlil_photo_flickr_id_get(photo) == id)
                    {
                        copy = eina_list_remove_list(copy, l);
                        break;
                    }
                }

                if(!photo)
                {
                    if(job->photo_new_cb)
		      {
			 const char *name = eina_stringshare_add(fc_photo->fields[PHOTO_FIELD_title].string);
			 const char *id = eina_stringshare_add(fc_photo->id);
			 job->photo_new_cb(job->data, job->album, name, id);
			 EINA_STRINGSHARE_DEL(name);
			 EINA_STRINGSHARE_DEL(id);
		      }
                }
                else if (job->type == ENLIL_FLICKR_JOB_CMP_ALBUM_PHOTOS)
                {
                    //check if both photos are different
                    if(!enlil_photo_name_get(photo) || !fc_photo->fields[PHOTO_FIELD_title].string
                            || strcmp(enlil_photo_name_get(photo), fc_photo->fields[PHOTO_FIELD_title].string) )
                    {
                        if(enlil_photo_flickr_need_sync_get(photo))
                        {
                            if(job->photo_flickrnotuptodate_cb)
                                job->photo_flickrnotuptodate_cb(job->data, job->album, photo);
                        }
                        else
                        {
                            if(job->photo_notuptodate_cb)
                                job->photo_notuptodate_cb(job->data, job->album, photo);
                        }
                    }
		    else if(job->photo_uptodate_cb)
		      job->photo_uptodate_cb(job->data, job->album, photo);
                }

                EINA_STRINGSHARE_DEL(id);
                i++;
            }

            EINA_LIST_FREE(copy, photo)
            {
                if(enlil_photo_flickr_id_get(photo))
                {
                    enlil_photo_flickr_id_set(photo, NULL);
                    enlil_photo_eet_save(photo);
                }
                if(job->photo_notinflickr_cb)
                    job->photo_notinflickr_cb(job->data, job->album, photo);
            }
            break;
	case ENLIL_FLICKR_JOB_GET_PHOTO_SIZES :
	    if(!job->response.error)
	      {
		i = 0;
		while(job->response.photo_sizes && job->response.photo_sizes[i])
		  {
			flickcurl_size *fc_size = job->response.photo_sizes[i];
			size = calloc(1, sizeof(Enlil_Flickr_Photo_Size));
			size->label = eina_stringshare_add(fc_size->label);
			size->width = fc_size->width;
			size->height = fc_size->height;
			size->source = eina_stringshare_add(fc_size->source);
			size->url = eina_stringshare_add(fc_size->url);
			size->media = eina_stringshare_add(fc_size->media);
			size->order = i+1;

			l_sizes = eina_list_append(l_sizes, size);

			i++;
		  }
		if(job->photo_sizes_cb)
			job->photo_sizes_cb(job->data, l_sizes, job->response.error);

		EINA_LIST_FREE(l_sizes, size)
		  {
		     EINA_STRINGSHARE_DEL(size->label);
		     EINA_STRINGSHARE_DEL(size->source);
		     EINA_STRINGSHARE_DEL(size->url);
		     EINA_STRINGSHARE_DEL(size->media);
		     FREE(size);
		  }

	      }
	    break;
        default : ;
    }

#else
    LOG_ERR("Flickr support is not built");
#endif
end:
    _job_next();
}


static Enlil_Flickr_Job *_enlil_flickr_job_get_authtoken_prepend(Enlil_Root *root, const char *frob)
{
    Enlil_Flickr_Job *job;
    Eina_List *l;

    EINA_LIST_FOREACH(l_jobs, l, job)
        if(job->type == ENLIL_FLICKR_JOB_GET_AUTHTOKEN)
            break;

    if(!job)
    {
        job = calloc(1, sizeof(Enlil_Flickr_Job));
        job->frob = eina_stringshare_add(frob);
        job->root = root;
        job->type = ENLIL_FLICKR_JOB_GET_AUTHTOKEN;

        l_jobs = eina_list_prepend(l_jobs, job);
    }

    _job_next();

    return job;
}


static int _connect(Enlil_Root *root, const char *frob)
{
    ASSERT_RETURN(frob != NULL);
    ASSERT_RETURN(root != NULL);

#ifdef HAVE_FLICKR
    flickcurl_init();
    fc = flickcurl_new();

    flickcurl_set_api_key(fc, FLICKR_KEY);
    flickcurl_set_shared_secret(fc, FLICKR_SECRET);
    if(!enlil_root_flickr_auth_token_get(root))
    {
        _enlil_flickr_job_get_authtoken_prepend(root, frob);
        return 0;
    }
    else
        flickcurl_set_auth_token(fc, enlil_root_flickr_auth_token_get(root));

    return 1;
#else
    LOG_ERR("Flickr support is not built");
#endif
}

static int _disconnect()
{
#ifdef HAVE_FLICKR
    if(fc)
    {
        flickcurl_finish();
        flickcurl_free(fc);
    }

    fc = NULL;

    return 1;
#else
    LOG_ERR("Flickr support is not built");
#endif
    return 0;
}



const char* enlil_flickr_size_label_get(Enlil_Flickr_Photo_Size *size)
{
   ASSERT_RETURN(size != NULL);
   return size->label;
}

const char* enlil_flickr_size_source_get(Enlil_Flickr_Photo_Size *size)
{
   ASSERT_RETURN(size != NULL);
   return size->source;
}


const char* enlil_flickr_size_url_get(Enlil_Flickr_Photo_Size *size)
{
   ASSERT_RETURN(size != NULL);
   return size->url;
}

const char* enlil_flickr_size_media_get(Enlil_Flickr_Photo_Size *size)
{
   ASSERT_RETURN(size != NULL);
   return size->media;
}

int enlil_flickr_size_width_get(Enlil_Flickr_Photo_Size *size)
{
   ASSERT_RETURN(size != NULL);
   return size->width;
}

int enlil_flickr_size_height_get(Enlil_Flickr_Photo_Size *size)
{
   ASSERT_RETURN(size != NULL);
   return size->height;
}

int enlil_flickr_size_order_get(Enlil_Flickr_Photo_Size *size)
{
   ASSERT_RETURN(size != NULL);
   return size->order;
}

