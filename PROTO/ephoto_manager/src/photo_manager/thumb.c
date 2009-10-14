// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#include "photo_manager_private.h"

typedef struct PM_Thumb_Job PM_Thumb_Job;
typedef struct Photo_Manager_Thumb Photo_Manager_Thumb;

static void _thumb_connect_cb(void *data, Ethumb_Client *client, Eina_Bool success);
static void _thumb_done_cb(void *data, Ethumb_Client *client, int id, const char *file, const char *key, const char *thumb_path, const char *thumb_key, Eina_Bool success);
static void _job_next();
static void _job_free(PM_Thumb_Job *job);

struct PM_Thumb_Job
{
   PM_Thumb_Job_Type type;
   PM_Photo *photo;

   PM_Thumb_Done_Cb done_cb;
   PM_Thumb_Error_Cb error_cb;
   void *data;

   int try;
};

struct Photo_Manager_Thumb
{
   Ethumb_Client *ethumb;
   int connected;
   int running;
   Eina_List *jobs;

   PM_Thumb_Job *current_job;
};

static Photo_Manager_Thumb thumb;

static int count = 0;
int pm_thumb_init()
{
   if(count>0) return ++count;

   thumb.ethumb = ethumb_client_connect(_thumb_connect_cb, NULL, NULL);
   return ++count;
}

int pm_thumb_shutdown()
{
   if(count>1) return --count;

   pm_thumb_clear();
   if(thumb.connected)
     ethumb_client_disconnect(thumb.ethumb);
   return --count;
}



const char* pm_thumb_photo_get(PM_Photo *photo, PM_Thumb_Job_Type type,
      PM_Thumb_Done_Cb done_cb, PM_Thumb_Error_Cb error_cb, void *data)
{
   const char *path;
   PM_Thumb_Job *job;

   ASSERT_RETURN(photo!=NULL);

   switch(type)
     {
      case PM_THUMB_FDO_NORMAL:
	 if( (path = pm_photo_thumb_fdo_normal_get(photo)) && ecore_file_exists(path))
	   return path;
	 break;
      case PM_THUMB_FDO_LARGE:
	 if( (path = pm_photo_thumb_fdo_large_get(photo)) && ecore_file_exists(path))
	   return path;
	 break;
     }
 
   //maybe the job is already in the list
   Eina_List *l;
   Eina_Bool find = EINA_FALSE;
   EINA_LIST_FOREACH(thumb.jobs, l, job)
     {
	if(job->photo == photo && job->type == type)
	  {
	     find = EINA_TRUE;
	     break;
	  }
     }

   if(find)
     {
	thumb.jobs = eina_list_remove(thumb.jobs, job);
     }
   else
     {
	job = calloc(1, sizeof(PM_Thumb_Job));
	job->photo = photo;
	job->type = type;
	job->done_cb = done_cb;
	job->error_cb = error_cb;
	job->data = data;
     }
   thumb.jobs = eina_list_prepend(thumb.jobs, job);

   _job_next();
   return NULL;
}

void pm_thumb_clear()
{
   PM_Thumb_Job *job;

   EINA_LIST_FREE(thumb.jobs, job)
     {
	_job_free(job); 
     }
   if(thumb.current_job)
     _job_free(thumb.current_job);
   thumb.current_job = NULL;
}

static void _job_free(PM_Thumb_Job *job)
{
   ASSERT_RETURN_VOID(job != NULL);

   free(job);
}

static void _thumb_connect_cb(void *data, Ethumb_Client *client, Eina_Bool success)
{
   if(success)
     {
	thumb.connected = 1;
	_job_next(thumb);
     }
   else
     LOG_CRIT("CAN not connect to ethumb\n");
}

static void _job_next()
{
   char buf[PATH_MAX];

   if(thumb.running)
     return;


   if(!thumb.jobs || !thumb.connected)
     {
	thumb.running = 0;
	return ;
     }

   PM_Thumb_Job *job = eina_list_nth(thumb.jobs,0);
   thumb.jobs = eina_list_remove(thumb.jobs, job);
   thumb.current_job = job;

   snprintf(buf, PATH_MAX, "%s/%s", pm_photo_path_get(job->photo),pm_photo_file_name_get(job->photo));
   ethumb_client_file_set(thumb.ethumb, buf, NULL);
   switch(job->type)
     {
      case PM_THUMB_FDO_LARGE:
	 ethumb_client_fdo_set(thumb.ethumb, ETHUMB_THUMB_LARGE);
	 ethumb_client_generate(thumb.ethumb, _thumb_done_cb, job, NULL);
	 break;
      case PM_THUMB_FDO_NORMAL:
	 ethumb_client_fdo_set(thumb.ethumb, ETHUMB_THUMB_NORMAL);
	 ethumb_client_generate(thumb.ethumb, _thumb_done_cb, job, NULL);
	 break;
     }
   thumb.running = 1;
}

static void _thumb_done_cb(void *data, Ethumb_Client *client, int id, const char *file, const char *key, const char *thumb_path, const char *thumb_key, Eina_Bool success)
{
   PM_Thumb_Job *job = (PM_Thumb_Job*) data;
  
   if(!thumb.current_job) return;

   if(!success && job->try<3)
     {
	thumb.jobs = eina_list_prepend(thumb.jobs, job);
	job->try++;
	goto end;
     }
   else if(!success)
     {
	LOG_CRIT("Can't create the thumbnail of the photo %s/%s", pm_photo_path_get(job->photo), pm_photo_file_name_get(job->photo));
	if(job->error_cb)
	  job->error_cb(job->data, job->photo);
     }

   switch(job->type)
     {
      case PM_THUMB_FDO_LARGE:
	 pm_photo_thumb_fdo_large_set(job->photo, thumb_path);
      case PM_THUMB_FDO_NORMAL:
	 pm_photo_thumb_fdo_normal_set(job->photo, thumb_path);
	 break;
     }

   if(job->done_cb)
     job->done_cb(job->data, job->photo, thumb_path);

   thumb.current_job = NULL;
   FREE(job);
end:
   thumb.running = 0;
   _job_next();
}

