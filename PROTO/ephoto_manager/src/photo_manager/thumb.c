// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#include "photo_manager_private.h"

static void _thumb_connect_cb(void *data, Ethumb_Client *client, Eina_Bool success);
static void _thumb_fdo_normal_done_cb(void *data, Ethumb_Client *client, int id, const char *file, const char *key, const char *thumb_path, const char *thumb_key, Eina_Bool success);
static void _thumb_fdo_large_done_cb(void *data, Ethumb_Client *client, int id, const char *file, const char *key, const char *thumb_path, const char *thumb_key, Eina_Bool success);
static void _job_next(Photo_Manager_Thumb *thumb);

typedef struct PM_Thumb_Job PM_Thumb_Job;
typedef enum PM_Thumb_Job_Type PM_Thumb_Job_Type;


enum PM_Thumb_Job_Type
{
   PM_THUMB_FDO_NORMAL,
   PM_THUMB_FDO_LARGE
};

struct PM_Thumb_Job
{
   PM_Thumb_Job_Type type;
   Photo_Manager_Thumb *thumb;
   PM_Photo *photo;

   int try;
};

struct Photo_Manager_Thumb
{
   Ethumb_Client *ethumb;
   int connected;
   int running;
   Eina_List *jobs;

   struct {
	PM_Thumb_Fdo_Normal_Cb fdo_normal_cb;
	PM_Thumb_Fdo_Large_Cb fdo_large_cb;
	void *data;
   }conf;
};


Photo_Manager_Thumb *pm_thumb_new(PM_Thumb_Fdo_Normal_Cb fdo_normal_cb, PM_Thumb_Fdo_Large_Cb fdo_large_cb, void *data)
{
   ASSERT_RETURN(fdo_normal_cb!=NULL);
   ASSERT_RETURN(fdo_large_cb!=NULL);

   Photo_Manager_Thumb *thumb = calloc(1, sizeof(Photo_Manager_Thumb));
   ASSERT_RETURN(thumb!=NULL);

   thumb->conf.fdo_normal_cb = fdo_normal_cb;
   thumb->conf.fdo_large_cb = fdo_large_cb;
   thumb->conf.data = data;

   thumb->ethumb = ethumb_client_connect(_thumb_connect_cb, thumb, NULL);
   return thumb;
}

const char* pm_thumb_photo_fdo_normal_get(Photo_Manager_Thumb *thumb, PM_Photo *photo)
{
   const char *path;
   PM_Thumb_Job *job;

   ASSERT_RETURN(thumb!=NULL);
   ASSERT_RETURN(photo!=NULL);

   if( (path = pm_photo_thumb_fdo_normal_get(photo)) && ecore_file_exists(path))
     return path;

   job = calloc(1, sizeof(PM_Thumb_Job));
   job->thumb = thumb;
   job->photo = photo;
   job->type = PM_THUMB_FDO_NORMAL;

   thumb->jobs = eina_list_prepend(thumb->jobs, job);

   _job_next(thumb);
   return NULL;
}

const char* pm_thumb_photo_fdo_large_get(Photo_Manager_Thumb *thumb, PM_Photo *photo)
{
   const char *path;
   PM_Thumb_Job *job;

   ASSERT_RETURN(thumb!=NULL);
   ASSERT_RETURN(photo!=NULL);

   if( (path = pm_photo_thumb_fdo_large_get(photo)) && ecore_file_exists(path))
     return path;

   job = calloc(1, sizeof(PM_Thumb_Job));
   job->thumb = thumb;
   job->photo = photo;
   job->type = PM_THUMB_FDO_LARGE;

   thumb->jobs = eina_list_prepend(thumb->jobs, job);

   _job_next(thumb);
   return NULL;
}

void pm_thumb_free(Photo_Manager_Thumb **thumb)
{
   ASSERT_RETURN_VOID(thumb!=NULL);
   Photo_Manager_Thumb *_thumb = *thumb;
   ASSERT_RETURN_VOID(_thumb!=NULL);

   if(_thumb->connected)
     ethumb_client_disconnect(_thumb->ethumb);
   FREE(_thumb);
}

static void _thumb_connect_cb(void *data, Ethumb_Client *client, Eina_Bool success)
{
   Photo_Manager_Thumb *thumb = (Photo_Manager_Thumb*) data;

   if(success)
     {
	thumb->connected = 1;
	_job_next(thumb);
     }
   else
     printf("CAN not connect to thumb\n");
}

static void _job_next(Photo_Manager_Thumb *thumb)
{
   char buf[PATH_MAX];

   ASSERT_RETURN_VOID(thumb!=NULL);

   if(thumb->running)
     return;


   if(!thumb->jobs || !thumb->connected)
     {
	thumb->running = 0;
	return ;
     }

   PM_Thumb_Job *job = eina_list_nth(thumb->jobs,0);
   thumb->jobs = eina_list_remove(thumb->jobs, job);

   snprintf(buf, PATH_MAX, "%s/%s", pm_photo_path_get(job->photo),pm_photo_file_name_get(job->photo));
   ethumb_client_file_set(thumb->ethumb, buf, NULL);
   switch(job->type)
     {
      case PM_THUMB_FDO_LARGE:
	 ethumb_client_fdo_set(thumb->ethumb, ETHUMB_THUMB_LARGE);
	 ethumb_client_generate(job->thumb->ethumb, _thumb_fdo_large_done_cb, job, NULL);
	 break;
      case PM_THUMB_FDO_NORMAL:
	 ethumb_client_fdo_set(job->thumb->ethumb, ETHUMB_THUMB_NORMAL);
	 ethumb_client_generate(job->thumb->ethumb, _thumb_fdo_normal_done_cb, job, NULL);
	 break;
     }
   thumb->running = 1;
}

static void _thumb_fdo_normal_done_cb(void *data, Ethumb_Client *client, int id, const char *file, const char *key, const char *thumb_path, const char *thumb_key, Eina_Bool success)
{
   PM_Thumb_Job *job = (PM_Thumb_Job*) data;
   Photo_Manager_Thumb *thumb = job->thumb;
   if(!success && job->try<3)
     {
	job->thumb->jobs = eina_list_prepend(job->thumb->jobs, job);
	job->try++;
	goto end;
     }

   pm_photo_thumb_fdo_normal_set(job->photo, thumb_path);

   job->thumb->conf.fdo_normal_cb(
	 job->thumb->conf.data, job->thumb, job->photo, thumb_path);

   FREE(job);
end:
   thumb->running = 0;
   _job_next(thumb);
}

static void _thumb_fdo_large_done_cb(void *data, Ethumb_Client *client, int id, const char *file, const char *key, const char *thumb_path, const char *thumb_key, Eina_Bool success)
{
   PM_Thumb_Job *job = (PM_Thumb_Job*) data;
   Photo_Manager_Thumb *thumb = job->thumb;

   if(!success && job->try < 3)
     {
	job->thumb->jobs = eina_list_prepend(job->thumb->jobs, job);
	job->try++;
	goto end;
     }

   pm_photo_thumb_fdo_large_set(job->photo, thumb_path);

   job->thumb->conf.fdo_large_cb(
	 job->thumb->conf.data, job->thumb, job->photo, thumb_path);

   FREE(job);
end:
   thumb->running = 0;
   _job_next(thumb);
}
