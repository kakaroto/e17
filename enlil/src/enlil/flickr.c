#include "enlil_private.h"
#include "flickr.h"

#include "../../config.h"

#ifdef HAVE_FLICKR
#include "flickcurl.h"
#include <time.h>
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
   ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR_PHOTO_UPLOAD,
   ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR_ALBUM_CREATE,
   ENLIL_FLICKR_JOB_CMP_ALBUM_PHOTOS,
   ENLIL_FLICKR_JOB_CMP_PHOTO,
   ENLIL_FLICKR_JOB_SYNC_PHOTO_UPDATE_FLICKR,
   ENLIL_FLICKR_JOB_SYNC_PHOTO_UPLOAD_FLICKR,
   ENLIL_FLICKR_JOB_SYNC_PHOTO_UPLOAD_FLICKR_ADD_IN_SET,
   ENLIL_FLICKR_JOB_GET_PHOTO_SIZES,
   ENLIL_FLICKR_JOB_SET_PHOTO_TIMES_FLICKR_FS
};

struct Enlil_Flickr_Job
{
   Enlil_Flickr_Job_Type type;

   Enlil_Album *album;
   Enlil_Photo *photo;
   Enlil_Library *library;
   const char *frob;
   const char *photo_id;

   struct
     {
	Eina_Bool error;
#ifdef HAVE_FLICKR
	flickcurl_photoset** photosets;
	flickcurl_photoset* photoset;
	flickcurl_photo** photos;
	flickcurl_photo* photo;
	flickcurl_size** photo_sizes;
	flickcurl_upload_status *upload_status;
	flickcurl_upload_params upload_params;
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
   Enlil_Flickr_Photo_Known_Cb photo_known_cb;

   Enlil_Flickr_Photo_NotInFlickr_Cb photo_notinflickr_cb;
   Enlil_Flickr_Photo_NotUpToDate_Cb photo_notuptodate_cb;
   Enlil_Flickr_Photo_FlickrNotUpToDate_Cb photo_flickrnotuptodate_cb;
   Enlil_Flickr_Photo_UpToDate_Cb photo_uptodate_cb;

   Enlil_Flickr_Photo_Upload_Start_Cb upload_start_cb;
   Enlil_Flickr_Photo_Upload_Progress_Cb upload_progress_cb;
   Enlil_Flickr_Photo_Upload_Done_Cb upload_done_cb;

   Enlil_Flickr_Photo_Done_Cb photo_done_cb;

   Enlil_Flickr_Error_Cb error_cb;
   Enlil_Flickr_Album_Error_Cb album_error_cb;
   Enlil_Flickr_Photo_Error_Cb photo_error_cb;

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

static Enlil_Flickr_Job_Start_Cb _job_start_cb = NULL;
static Enlil_Flickr_Job_Done_Cb _job_done_cb = NULL;
static void *_job_start_data = NULL;
static void *_job_done_data = NULL;

static void _job_next();
static void _job_free(Enlil_Flickr_Job *job);
static void _flickr_thread(void *data, Ecore_Thread *thread);
static void _end_cb(void *data, Ecore_Thread *thread);
static const char *_enlil_flickr_job_type_tostring(Enlil_Flickr_Job_Type type);

static Enlil_Flickr_Job *_enlil_flickr_job_get_authtoken_prepend(Enlil_Library *library, const char *code);
static int _connect(Enlil_Library *library, const char *code);
static int _disconnect();
static Eina_Bool _idler_upload_cb(void *data);


static const char *_enlil_flickr_job_type_tostring(Enlil_Flickr_Job_Type type)
{
   switch(type)
     {
      case ENLIL_FLICKR_JOB_REINIT:
	 return "ENLIL_FLICKR_JOB_REINIT";
	 break;
      case ENLIL_FLICKR_JOB_GET_AUTHTOKEN:
	 return "ENLIL_FLICKR_JOB_GET_AUTHTOKEN";
	 break;
      case ENLIL_FLICKR_JOB_CMP_ALBUM_HEADER:
	 return "ENLIL_FLICKR_JOB_CMP_ALBUM_HEADER";
	 break;
      case ENLIL_FLICKR_JOB_CMP_ALBUMS_HEADER:
	 return "ENLIL_FLICKR_JOB_CMP_ALBUMS_HEADER";
	 break;
      case ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_UPDATE_FLICKR:
	 return "ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_UPDATE_FLICKR";
	 break;
      case ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_UPDATE_LOCAL:
	 return "ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_UPDATE_LOCAL";
	 break;
      case ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR_PHOTO_UPLOAD:
	 return "ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR_PHOTO_UPLOAD";
	 break;
      case ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR_ALBUM_CREATE:
	 return "ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR_ALBUM_CREATE";
	 break;
      case ENLIL_FLICKR_JOB_CMP_ALBUM_PHOTOS:
	 return "ENLIL_FLICKR_JOB_CMP_ALBUM_PHOTOS";
	 break;
      case ENLIL_FLICKR_JOB_CMP_PHOTO:
	 return "ENLIL_FLICKR_JOB_CMP_PHOTO";
	 break;
      case ENLIL_FLICKR_JOB_SYNC_PHOTO_UPDATE_FLICKR:
	 return "ENLIL_FLICKR_JOB_SYNC_PHOTO_UPDATE_FLICKR";
	 break;
      case ENLIL_FLICKR_JOB_SYNC_PHOTO_UPLOAD_FLICKR:
	 return "ENLIL_FLICKR_JOB_SYNC_PHOTO_UPLOAD_FLICKR";
	 break;
      case ENLIL_FLICKR_JOB_SYNC_PHOTO_UPLOAD_FLICKR_ADD_IN_SET:
	 return "ENLIL_FLICKR_JOB_SYNC_PHOTO_UPLOAD_FLICKR_ADD_IN_SET";
	 break;
      case ENLIL_FLICKR_JOB_GET_PHOTO_SIZES:
	 return "ENLIL_FLICKR_JOB_GET_PHOTO_SIZES";
	 break;
      case ENLIL_FLICKR_JOB_SET_PHOTO_TIMES_FLICKR_FS:
	 return "ENLIL_FLICKR_JOB_SET_PHOTO_TIMES_FLICKR_FS";
	 break;
     }
   return "unknown";
}

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


void enlil_flickr_job_start_cb_set(Enlil_Flickr_Job_Start_Cb start_cb, void *data)
{
   _job_start_cb = start_cb;
   _job_start_data = data;
}
void enlil_flickr_job_done_cb_set(Enlil_Flickr_Job_Done_Cb done_cb, void *data)
{
   _job_done_cb = done_cb;
   _job_done_data = data;
}

void enlil_flickr_job_del(Enlil_Flickr_Job *job)
{
    ASSERT_RETURN_VOID(job != NULL);

    LOG_INFO("Delete Flickr's job : %s", _enlil_flickr_job_type_tostring(job->type));

    if(job == job_current)
    {
        job_current = NULL;
    }

    l_jobs = eina_list_remove(l_jobs, job);

    _job_free(job);
}

Enlil_Flickr_Job *enlil_flickr_job_reinit_prepend(Enlil_Library *library)
{
   Enlil_Flickr_Job *job;
   Eina_List *l;

   EINA_LIST_FOREACH(l_jobs, l, job)
      if(job->type == ENLIL_FLICKR_JOB_REINIT)
	break;

   if(!job)
     {
	job = calloc(1, sizeof(Enlil_Flickr_Job));
	job->library = library;
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
	job->library = enlil_album_library_get(album);
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
	job->library = enlil_album_library_get(album);
	job->type = ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_UPDATE_LOCAL;

	job->album_done_cb = done_cb;
	job->data = data;

	l_jobs = eina_list_append(l_jobs, job);
     }

   _job_next();

   return job;
}

Enlil_Flickr_Job *enlil_flickr_job_sync_album_header_create_flickr_append(Enlil_Album *album,
      Enlil_Flickr_Photo_Upload_Start_Cb start_cb,
      Enlil_Flickr_Photo_Upload_Progress_Cb progress_cb,
      Enlil_Flickr_Photo_Upload_Done_Cb done_cb,
      Enlil_Flickr_Photo_Error_Cb error_cb,
      void *data)
{
   Enlil_Flickr_Job *job;
   Eina_List *l;
   Enlil_Photo *photo;

   ASSERT_RETURN(album != NULL);

   EINA_LIST_FOREACH(l_jobs, l, job)
      if(job->album == album && (
	       job->type == ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR_PHOTO_UPLOAD
	       || job->type == ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR_ALBUM_CREATE))
	break;

   //1. upload a photo from the album into Flickr
   //2. create the album
   //Flickr requires to have a photo in a album in order to create it
   //An album can not be empty

   if(!job)
     {
	//SELECT if a photo is already in Flickr
	//else select the first and upload it
	EINA_LIST_FOREACH(enlil_album_photos_get(album), l, photo)
	  {
		if(enlil_photo_netsync_id_get(photo))
		  break;
	  }

	job = calloc(1, sizeof(Enlil_Flickr_Job));
	job->album = album;
	job->library = enlil_album_library_get(album);

	if(!photo)
	  {
		job->type = ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR_PHOTO_UPLOAD;
		job->photo = eina_list_data_get(enlil_album_photos_get(album));
	  }
	else
	  {
		job->type = ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR_ALBUM_CREATE;
		job->photo = photo;
	  }

	job->upload_start_cb = start_cb;
	job->upload_progress_cb = progress_cb;
	job->upload_done_cb = done_cb;
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
	job->library = enlil_album_library_get(album);
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

Enlil_Flickr_Job *enlil_flickr_job_sync_albums_append(Enlil_Library *library,
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

   ASSERT_RETURN(library != NULL);

   EINA_LIST_FOREACH(l_jobs, l, job)
      if(job->type == ENLIL_FLICKR_JOB_CMP_ALBUMS_HEADER)
	break;

   if(!job)
     {
	job = calloc(1, sizeof(Enlil_Flickr_Job));
	job->library = library;
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
      Enlil_Flickr_Photo_Known_Cb known_cb,
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
	job->library = enlil_album_library_get(album);
	job->album = album;
	job->type = ENLIL_FLICKR_JOB_CMP_ALBUM_PHOTOS;
	job->photo_new_cb = new_cb;
	job->photo_notinflickr_cb = notinflickr_cb;
	job->photo_known_cb = known_cb;
	job->album_error_cb = album_error_cb;
	job->data = data;

	l_jobs = eina_list_append(l_jobs, job);
     }

   _job_next();

   return job;
}

Enlil_Flickr_Job *enlil_flickr_job_cmp_photo_append(
      Enlil_Photo *photo,
      Enlil_Flickr_Photo_FlickrNotUpToDate_Cb flickrnotuptodate_cb,
      Enlil_Flickr_Photo_NotUpToDate_Cb notuptodate_cb,
      Enlil_Flickr_Photo_UpToDate_Cb uptodate_cb,
      Enlil_Flickr_Photo_Error_Cb error_cb,
      void *data)
{
   Enlil_Flickr_Job *job;
   Eina_List *l;

   ASSERT_RETURN(photo != NULL);
   ASSERT_RETURN(enlil_photo_netsync_id_get(photo) != NULL);

   EINA_LIST_FOREACH(l_jobs, l, job)
      if(job->type == ENLIL_FLICKR_JOB_CMP_PHOTO && job->photo == photo)
	break;

   if(!job)
     {
	job = calloc(1, sizeof(Enlil_Flickr_Job));
	job->photo = photo;
	job->type = ENLIL_FLICKR_JOB_CMP_PHOTO;
	job->photo_flickrnotuptodate_cb = flickrnotuptodate_cb;
	job->photo_notuptodate_cb = notuptodate_cb;
	job->photo_uptodate_cb = uptodate_cb;
	job->photo_error_cb = error_cb;

	job->data = data;

	l_jobs = eina_list_append(l_jobs, job);
     }

   _job_next();

   return job;
}

Enlil_Flickr_Job *enlil_flickr_job_set_photo_times_flickr_fs_prepend(
      Enlil_Photo *photo,
      Enlil_Flickr_Photo_Error_Cb error_cb,
      void *data)
{
   Enlil_Flickr_Job *job;
   Eina_List *l;

   ASSERT_RETURN(photo != NULL);
   ASSERT_RETURN(enlil_photo_netsync_id_get(photo) != NULL);

   EINA_LIST_FOREACH(l_jobs, l, job)
      if(job->type == ENLIL_FLICKR_JOB_SET_PHOTO_TIMES_FLICKR_FS && job->photo == photo)
	break;

   if(!job)
     {
	job = calloc(1, sizeof(Enlil_Flickr_Job));
	job->photo = photo;
	job->type = ENLIL_FLICKR_JOB_SET_PHOTO_TIMES_FLICKR_FS;
	job->photo_error_cb = error_cb;

	job->data = data;

	l_jobs = eina_list_prepend(l_jobs, job);
     }

   _job_next();

   return job;
}

Enlil_Flickr_Job *enlil_flickr_job_sync_photo_update_flickr_append(
      Enlil_Photo *photo,
      Enlil_Flickr_Photo_Upload_Start_Cb start_cb,
      Enlil_Flickr_Photo_Upload_Progress_Cb progress_cb,
      Enlil_Flickr_Photo_Upload_Done_Cb done_cb,
      Enlil_Flickr_Photo_Error_Cb error_cb,
      void *data)
{
   Enlil_Flickr_Job *job;
   Eina_List *l;

   ASSERT_RETURN(photo != NULL);

   EINA_LIST_FOREACH(l_jobs, l, job)
      if(job->type == ENLIL_FLICKR_JOB_SYNC_PHOTO_UPDATE_FLICKR && job->photo == photo)
	break;

   if(!job)
     {
	job = calloc(1, sizeof(Enlil_Flickr_Job));
	job->photo = photo;
	job->type = ENLIL_FLICKR_JOB_SYNC_PHOTO_UPDATE_FLICKR;
	job->photo_error_cb = error_cb;
	job->upload_start_cb = start_cb;
	job->upload_progress_cb = progress_cb;
	job->upload_done_cb = done_cb;

	job->data = data;

	l_jobs = eina_list_append(l_jobs, job);
     }

   _job_next();

   return job;
}

Enlil_Flickr_Job *enlil_flickr_job_sync_photo_upload_flickr_prepend(
      Enlil_Photo *photo,
      Enlil_Flickr_Photo_Upload_Start_Cb start_cb,
      Enlil_Flickr_Photo_Upload_Progress_Cb progress_cb,
      Enlil_Flickr_Photo_Upload_Done_Cb done_cb,
      Enlil_Flickr_Photo_Error_Cb error_cb,
      void *data)
{
   Enlil_Flickr_Job *job;
   Eina_List *l;

   ASSERT_RETURN(photo != NULL);

   EINA_LIST_FOREACH(l_jobs, l, job)
      if(job->type == ENLIL_FLICKR_JOB_SYNC_PHOTO_UPLOAD_FLICKR && job->photo == photo)
	break;

   if(!job)
     {
	job = calloc(1, sizeof(Enlil_Flickr_Job));
	job->photo = photo;
	job->type = ENLIL_FLICKR_JOB_SYNC_PHOTO_UPLOAD_FLICKR;
	job->photo_error_cb = error_cb;
	job->upload_start_cb = start_cb;
	job->upload_progress_cb = progress_cb;
	job->upload_done_cb = done_cb;

	job->data = data;

	l_jobs = eina_list_prepend(l_jobs, job);
     }

   _job_next();

   return job;
}

Enlil_Flickr_Job *enlil_flickr_job_get_photo_sizes_prepend(const char *photo_id,
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

	l_jobs = eina_list_prepend(l_jobs, job);
     }

   _job_next();

   return job;
}


static void _job_free(Enlil_Flickr_Job *job)
{
   ASSERT_RETURN_VOID(job != NULL);


#ifdef HAVE_FLICKR
   EINA_STRINGSHARE_DEL(job->photo_id);
   EINA_STRINGSHARE_DEL(job->response.upload_params.title);
   EINA_STRINGSHARE_DEL(job->response.upload_params.description);
   EINA_STRINGSHARE_DEL(job->response.upload_params.tags);
   EINA_STRINGSHARE_DEL(job->response.upload_params.photo_file);

   FREE(job);
#endif
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
	Enlil_Library *library = job->library;

	if(!library)
	  {
	     LOG_CRIT("The job is not associated to a library !");
	     return ;
	  }
	else if(!enlil_library_flickr_account_get(library))
	  {
	     LOG_CRIT("The library is not associated to a flickr account !");
	     return ;
	  }

	if(!_connect(library, enlil_library_flickr_account_get(library)))
	  return ;
     }

   running = 1;

   if( (job->type == ENLIL_FLICKR_JOB_SYNC_PHOTO_UPDATE_FLICKR
	 || job->type == ENLIL_FLICKR_JOB_SYNC_PHOTO_UPLOAD_FLICKR
	 || job->type == ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR_PHOTO_UPLOAD)
	 && job->upload_start_cb)
     job->upload_start_cb(job->data, job->photo);

   ecore_thread_run(_flickr_thread, _end_cb, NULL, NULL);

   if(_job_start_cb)
     _job_start_cb(_job_start_data, job, job->album, job->photo);
#else
   LOG_ERR("Flickr support is not built");
#endif
}

static void _flickr_thread(void *data, Ecore_Thread *thread)
{
   char *token;
   char buf[PATH_MAX];

   Enlil_Flickr_Job *job = job_current;

   if(!job) return ;

#ifdef HAVE_FLICKR
   switch(job_current->type)
     {
      case ENLIL_FLICKR_JOB_REINIT :
	 LOG_INFO("Reinit");
	 _disconnect();
	 break;
      case ENLIL_FLICKR_JOB_GET_AUTHTOKEN :
	 LOG_INFO("Get authtoken");
	 token = flickcurl_auth_getFullToken(fc, job->frob);
	 if(!token)
	   {
	      LOG_ERR("No Auth token associated to the frob");
	      job->response.error = EINA_TRUE;
	      break;
	   }
	 flickcurl_set_auth_token(fc, token);
	 enlil_library_flickr_auth_token_set(job->library, token);
	 break;
      case ENLIL_FLICKR_JOB_CMP_ALBUMS_HEADER :
	 LOG_INFO("Get albums header");
	 job->response.photosets = flickcurl_photosets_getList(fc, NULL);
	 if(!job->response.photosets)
	   {
	      LOG_ERR("Can not retrieves the list of sets");
	      job->response.error = EINA_TRUE;
	      break;
	   }
	 break;
      case ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_UPDATE_FLICKR :
	 LOG_INFO("Update flickr album header : %s", enlil_album_name_get(job->album));
	 if(flickcurl_photosets_editMeta(fc,
		  enlil_album_netsync_id_get(job->album),
		  enlil_album_name_get(job->album),
		  NULL))
	   {
	      job->response.error = EINA_TRUE;
	      break;
	   }
      case ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_UPDATE_LOCAL :
	 LOG_INFO("Update local album header : %s", enlil_album_name_get(job->album));
	 job->response.photoset = flickcurl_photosets_getInfo(fc,
	       enlil_album_netsync_id_get(job->album));
	 if(!job->response.photoset)
	   job->response.error = EINA_TRUE;
	 break;
      case ENLIL_FLICKR_JOB_CMP_ALBUM_HEADER :
	 LOG_INFO("Compare album header : %s", enlil_album_name_get(job->album));
	 job->response.photoset = flickcurl_photosets_getInfo(fc,
	       enlil_album_netsync_id_get(job->album));
	 break;
      case ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR_ALBUM_CREATE:
	 LOG_INFO("Create album on flickr : %s", enlil_album_name_get(job->album));
	 //The album musst have one photo already in Flickr
	 job->response.photoset_id = flickcurl_photosets_create(fc,
	       enlil_album_name_get(job->album),
	       NULL,
	       enlil_photo_netsync_id_get(job->photo),
	       NULL);
	 if(!job->response.photoset_id)
	   job->response.error = EINA_TRUE;
	 break;
      case ENLIL_FLICKR_JOB_CMP_ALBUM_PHOTOS :
	 LOG_INFO("Compare photos of the album : %s", enlil_album_name_get(job->album));
	 job->response.photos = flickcurl_photosets_getPhotos(fc,
	       enlil_album_netsync_id_get(job->album), NULL, 0, 500, 0);
	 if(!job->response.photos)
	   {
	      LOG_ERR("Can not retrieves the list of photos");
	      job->response.error = EINA_TRUE;
	      break;
	   }
	 break;
      case ENLIL_FLICKR_JOB_CMP_PHOTO :
	 LOG_INFO("Compare the photo %s", enlil_photo_name_get(job->photo));
	 goto ENLIL_FLICKR_JOB_CMP_PHOTO_NEXT;
      case ENLIL_FLICKR_JOB_SET_PHOTO_TIMES_FLICKR_FS:
	 LOG_INFO("Set photos times (flickr last change and local last change) : %s",
	       enlil_photo_name_get(job->photo));
ENLIL_FLICKR_JOB_CMP_PHOTO_NEXT:
	 job->response.photo = flickcurl_photos_getInfo(fc,
	       enlil_photo_netsync_id_get(job->photo));
	 if(!job->response.photo)
	   {
	      LOG_ERR("Can not retrieve the list photo");
	      job->response.error = EINA_TRUE;
	      break;
	   }
	 break;
      case ENLIL_FLICKR_JOB_SYNC_PHOTO_UPDATE_FLICKR:
	 snprintf(buf, sizeof(buf), "%s/%s", enlil_photo_path_get(job->photo),
	       enlil_photo_file_name_get(job->photo));
	 LOG_INFO("Update the flickr photo : %s", enlil_photo_name_get(job->photo));
	 job->response.upload_status = flickcurl_photos_replace(fc, buf,
	       enlil_photo_netsync_id_get(job->photo), 1);
	 if(!job->response.upload_status)
	   {
	      LOG_ERR("Can not upload the photo");
	      job->response.error = EINA_TRUE;
	      break;
	   }

	 break;

      case ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR_PHOTO_UPLOAD:
      case ENLIL_FLICKR_JOB_SYNC_PHOTO_UPLOAD_FLICKR:
	 snprintf(buf, sizeof(buf), "%s/%s", enlil_photo_path_get(job->photo),
	       enlil_photo_file_name_get(job->photo));
	 LOG_INFO("Upload new photo to flickr : %s", enlil_photo_name_get(job->photo));
	 job->response.upload_params.photo_file = eina_stringshare_add(buf);
	 if(enlil_photo_name_get(job->photo))
	   job->response.upload_params.title = eina_stringshare_add(enlil_photo_name_get(job->photo));
	 else
	   job->response.upload_params.title = NULL;
	 if(enlil_photo_description_get(job->photo))
	   job->response.upload_params.description = eina_stringshare_add(enlil_photo_description_get(job->photo));
	 else
	   job->response.upload_params.description = NULL;

	 job->response.upload_params.is_public = EINA_TRUE;
	 job->response.upload_params.is_friend = EINA_FALSE;
	 job->response.upload_params.is_family = EINA_FALSE;
	 job->response.upload_params.safety_level = 1;
	 job->response.upload_params.content_type = 1;


	 job->response.upload_status = flickcurl_photos_upload_params(fc, &(job->response.upload_params));
	 if(!job->response.upload_status)
	   {
	      LOG_ERR("Can not upload the photo");
	      job->response.error = EINA_TRUE;
	      break;
	   }

	 break;
      case ENLIL_FLICKR_JOB_SYNC_PHOTO_UPLOAD_FLICKR_ADD_IN_SET:
	 flickcurl_photosets_addPhoto(fc, enlil_album_netsync_id_get(enlil_photo_album_get(job->photo))
	       , enlil_photo_netsync_id_get(job->photo));
	    break;
      case ENLIL_FLICKR_JOB_GET_PHOTO_SIZES :
	 LOG_INFO("Retrieve the list of sizes of the photo : %s", job->photo_id);
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

static void _end_cb(void *data, Ecore_Thread *thread)
{
   int i;
   char buf[PATH_MAX];
   Eina_List *l, *copy;
   Enlil_Album *album;
   Enlil_Photo *photo;
   Enlil_Flickr_Job *job = job_current;
   Eina_List *l_sizes = NULL;
   Enlil_Flickr_Photo_Size *size;
   Ecore_Idler *idler;

   struct tm tm;
   time_t flickr_time;
   running = 0;

   if(!job) goto end ;

   l_jobs = eina_list_remove(l_jobs, job);
#ifdef HAVE_FLICKR
   LOG_INFO("Flickr job done");

   if(_job_done_cb
	 && job->type !=  ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR_PHOTO_UPLOAD
	 && job->type != ENLIL_FLICKR_JOB_SYNC_PHOTO_UPDATE_FLICKR
	 && job->type != ENLIL_FLICKR_JOB_SYNC_PHOTO_UPLOAD_FLICKR )
     _job_done_cb(_job_done_data, job, job->album, job->photo);

   switch(job->type)
     {
      case ENLIL_FLICKR_JOB_CMP_ALBUMS_HEADER :
	 if(job->response.error)
	   {
	      if(job->error_cb)
		job->error_cb(job->data, job->library);
	      break;
	   }
	 i = 0;

	 copy = eina_list_clone(enlil_library_albums_get(job->library));

	 while(job->response.photosets && job->response.photosets[i])
	   {
	      flickcurl_photoset *photoset = job->response.photosets[i];
	      const char *id = eina_stringshare_add(photoset->id);

	      EINA_LIST_FOREACH(copy, l, album)
		{
		   if(enlil_album_netsync_id_get(album) == id)
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
		   enlil_album_path_set(album, enlil_library_path_get(job->library));
		   enlil_album_name_set(album, photoset->title);
		   enlil_album_file_name_set(album, photoset->title);
		   //_enlil_album_netsync_id_set(album, photoset->id);

		   snprintf(buf, PATH_MAX, "%s/%s", enlil_library_path_get(job->library), photoset->title);
		   if(ecore_file_exists(buf))
		     {
			snprintf(buf, PATH_MAX, "%s_Flickr", photoset->title);
			enlil_album_file_name_set(album, buf);
			enlil_album_name_set(album, buf);

			snprintf(buf, PATH_MAX, "%s/%s", enlil_library_path_get(job->library),
			      enlil_album_file_name_get(album));
		     }
		   enlil_library_album_add(job->library, album);
		   enlil_library_eet_albums_save(job->library);
		   enlil_album_eet_header_save(album);

		   ecore_file_mkdir(buf);

		   if(job->album_new_cb)
		     job->album_new_cb(job->data, job->library, album);
		}
	      else
		{
		   //check if both albums are different
		   if(!enlil_album_name_get(album) || !photoset->title
			 || strcmp(enlil_album_name_get(album), photoset->title) )
		     {
			/*if(enlil_album_netsync_need_sync_get(album))
			  {
			     if(job->album_flickrnotuptodate_cb)
			       job->album_flickrnotuptodate_cb(job->data, job->library, album);
			  }
			else
			  {
			     if(job->album_notuptodate_cb)
			       job->album_notuptodate_cb(job->data, job->library, album);
			  }*/
		     }
		   else if(job->album_uptodate_cb)
		     job->album_uptodate_cb(job->data, job->library, album);

		}

	      EINA_STRINGSHARE_DEL(id);
	      i++;
	   }

	 EINA_LIST_FREE(copy, album)
	   {
	      if(enlil_album_netsync_id_get(album))
		{
		   //_enlil_album_netsync_id_set(album, NULL);
		   enlil_album_eet_header_save(album);
		}
	      if(job->album_notinflickr_cb)
		job->album_notinflickr_cb(job->data, job->library, album);
	   }
	 break;
      case ENLIL_FLICKR_JOB_CMP_ALBUM_HEADER :
	 //check if both albums are different
	 if(job->response.error)
	   {
	      if(job->error_cb)
		job->error_cb(job->data, job->library);
	      break;
	   }

	 album = job->album;
	 if(!job->response.photoset)
	   {
	      if(job->album_notinflickr_cb)
		job->album_notinflickr_cb(job->data, job->library, album);
	      break;
	   }

	 if(!enlil_album_name_get(album) || !job->response.photoset->title
	       || strcmp(enlil_album_name_get(album), job->response.photoset->title) )
	   {
	      /*if(enlil_album_netsync_need_sync_get(album))
		{
		   if(job->album_flickrnotuptodate_cb)
		     job->album_flickrnotuptodate_cb(job->data, job->library, album);
		}
	      else
		{
		   if(job->album_notuptodate_cb)
		     job->album_notuptodate_cb(job->data, job->library, album);
		}*/
	   }
	 break;
      case ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_UPDATE_FLICKR :
	 if(!job->response.error)
	   {
	      //enlil_album_netsync_need_sync_set(job->album, EINA_FALSE);
	      enlil_album_eet_header_save(job->album);
	   }
	 if(job->album_done_cb)
	   job->album_done_cb(job->data, job->library, job->album, job->response.error);
	 break;
      case ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_UPDATE_LOCAL :
	 if(!job->response.error)
	   {
	      //enlil_album_netsync_need_sync_set(job->album, EINA_FALSE);
	      enlil_album_name_set(job->album, job->response.photoset->title);
	   }
	 enlil_album_eet_header_save(job->album);
	 if(job->album_done_cb)
	   job->album_done_cb(job->data, job->library, job->album, job->response.error);
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
		   /*if(enlil_photo_netsync_id_get(photo) == id)
		     {
			copy = eina_list_remove_list(copy, l);
			break;
		     }*/
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
	      else
		{
		   if(job->photo_known_cb)
		     job->photo_known_cb(job->data, job->album, photo);
		}

	      EINA_STRINGSHARE_DEL(id);
	      i++;
	   }

	 EINA_LIST_FREE(copy, photo)
	   {
	      if(enlil_photo_netsync_id_get(photo))
		{
		   //enlil_photo_netsync_id_set(photo, NULL);
		   enlil_photo_eet_save(photo);
		}
	      if(job->photo_notinflickr_cb)
		job->photo_notinflickr_cb(job->data, job->album, photo);
	   }
	 break;
      case ENLIL_FLICKR_JOB_CMP_PHOTO :
	 if(job->response.error)
	   {
	      if(job->error_cb)
		job->photo_error_cb(job->data, job->photo);
	      break;
	   }
	 //strptime(job->response.photo->fields[PHOTO_FIELD_dates_lastupdate].string,
	  //     "%Y-%m-%dT%H:%M:%SZ", &tm);
	 //flickr_time = mktime(&tm);

	 /*if(flickr_time > _enlil_photo_netsync_last_change_get(job->photo)
	       && flickr_time > enlil_photo_time_get(job->photo))
	   {
	      //flickr more update
	      if(job->photo_notuptodate_cb)
		job->photo_notuptodate_cb(job->data, job->photo);
	   }
	 else if(enlil_photo_time_get(job->photo) > _enlil_photo_netsync_fs_time_get(job->photo))
	   {
	      //local more update
	      if(job->photo_flickrnotuptodate_cb)
		job->photo_flickrnotuptodate_cb(job->data, job->photo);
	   }
	 else
	   {
	      //no update
	      if(job->photo_uptodate_cb)
		job->photo_uptodate_cb(job->data, job->photo);
	   }*/
	 break;
      case ENLIL_FLICKR_JOB_SET_PHOTO_TIMES_FLICKR_FS :
	 if(job->response.error)
	   {
	      if(job->error_cb)
		job->photo_error_cb(job->data, job->photo);
	      break;
	   }
	 //strptime(job->response.photo->fields[PHOTO_FIELD_dates_lastupdate].string,
	 //      "%Y-%m-%dT%H:%M:%SZ", &tm);
	 //flickr_time = mktime(&tm);
	 //_enlil_photo_netsync_last_change_set(job->photo, flickr_time);
	 //_enlil_photo_netsync_fs_time_calc(job->photo);
	 enlil_photo_eet_save(job->photo);
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
      case ENLIL_FLICKR_JOB_SYNC_PHOTO_UPDATE_FLICKR:
      case ENLIL_FLICKR_JOB_SYNC_PHOTO_UPLOAD_FLICKR:
      case ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR_PHOTO_UPLOAD:
	 //in this case we upload a photo in an asynchronous way
	 //the upload is not done when we are here, we use a Ecore_Idler to check the advancement
	 if(job->response.error)
	   {
	      if(job->error_cb)
		job->photo_error_cb(job->data, job->photo);
	      break;
	   }
	 running = 1;
	 idler = ecore_idler_add(_idler_upload_cb, NULL);
	 return ;
	 break;
      case ENLIL_FLICKR_JOB_SYNC_PHOTO_UPLOAD_FLICKR_ADD_IN_SET:
	  if(job->response.error)
	   {
	      if(job->error_cb)
		job->photo_error_cb(job->data, job->photo);
	      break;
	   }

	  if(job->upload_done_cb)
	    job->upload_done_cb(job->data, job->photo, EINA_TRUE);

	 break;
      case ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR_ALBUM_CREATE:
	 if(job->response.error)
	   {
	      if(job->error_cb)
		job->upload_done_cb(job->data, job->photo, EINA_FALSE);
	      break;
	   }

	 //enlil_album_netsync_need_sync_set(job->album, EINA_FALSE);
	 //_enlil_album_netsync_id_set(job->album, job->response.photoset_id);
	 enlil_album_eet_header_save(job->album);
	 if(job->upload_done_cb)
	   job->upload_done_cb(job->data, job->photo, EINA_TRUE);

      default : ;
     }

   _job_free(job);

#else
   LOG_ERR("Flickr support is not built");
#endif
end:
   _job_next();
}


static Enlil_Flickr_Job *_enlil_flickr_job_get_authtoken_prepend(Enlil_Library *library, const char *frob)
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
	job->library = library;
	job->type = ENLIL_FLICKR_JOB_GET_AUTHTOKEN;

	l_jobs = eina_list_prepend(l_jobs, job);
     }

   _job_next();

   return job;
}


static int _connect(Enlil_Library *library, const char *frob)
{
   ASSERT_RETURN(frob != NULL);
   ASSERT_RETURN(library != NULL);

#ifdef HAVE_FLICKR
   flickcurl_init();
   fc = flickcurl_new();

   flickcurl_set_api_key(fc, FLICKR_KEY);
   flickcurl_set_shared_secret(fc, FLICKR_SECRET);
   if(!enlil_library_flickr_auth_token_get(library))
     {
	_enlil_flickr_job_get_authtoken_prepend(library, frob);
	return 0;
     }
   else
     flickcurl_set_auth_token(fc, enlil_library_flickr_auth_token_get(library));

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


static Eina_Bool _idler_upload_cb(void *data)
{
#ifdef HAVE_FLICKR
   Enlil_Flickr_Job *job = job_current;
   char **t;

   if(!job)
     goto upload_error;

   l_jobs = eina_list_remove(l_jobs, job);


   if(job->type == ENLIL_FLICKR_JOB_SYNC_PHOTO_UPLOAD_FLICKR
	 || job->type == ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR_PHOTO_UPLOAD)
     {
	//enlil_photo_netsync_id_set(job->photo, job->response.upload_status->photoid);
	enlil_photo_eet_save(job->photo);

	//create the album
	if(job->type == ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR_PHOTO_UPLOAD)
	  job->type =  ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR_ALBUM_CREATE;
	else
	  //add the photo in the album/set
	  //we keep using the current job but we change it
	  job->type = ENLIL_FLICKR_JOB_SYNC_PHOTO_UPLOAD_FLICKR_ADD_IN_SET;
	l_jobs = eina_list_prepend(l_jobs, job);
     }
   else
     {
	t = calloc(2, sizeof(char *));
	t[0] = job->response.upload_status->ticketid;
	t[1] = NULL;

	flickcurl_ticket** tickets = flickcurl_photos_upload_checkTickets(fc, (const char **)t);
	FREE(t);
	if(!tickets || !tickets[0])
	  goto upload_done;

	int done = tickets[0]->complete;
	flickcurl_free_tickets(tickets);

	if(done)
	  goto upload_done;

	if(job->upload_progress_cb)
	  job->upload_progress_cb(job->data, job->photo, -1, -1);

	return EINA_TRUE;
     }

upload_done:

   if(_job_done_cb)
     _job_done_cb(_job_done_data, job, job->album, job->photo);

   if(job->type != ENLIL_FLICKR_JOB_SYNC_PHOTO_UPLOAD_FLICKR_ADD_IN_SET
	 && job->upload_done_cb)
     job->upload_done_cb(job->data, job->photo, EINA_TRUE);

   flickcurl_free_upload_status(job->response.upload_status);

   enlil_flickr_job_set_photo_times_flickr_fs_prepend(job->photo, NULL, NULL);

   if(job->type != ENLIL_FLICKR_JOB_SYNC_PHOTO_UPLOAD_FLICKR_ADD_IN_SET
	 && job->type != ENLIL_FLICKR_JOB_SYNC_ALBUM_HEADER_CREATE_FLICKR_ALBUM_CREATE)
	_job_free(job);
upload_error:
   running = 0;
   _job_next();
   return EINA_FALSE;

#else
   LOG_ERR("Flickr support is not built !");
   return EINA_FALSE;
#endif
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

