#include "enlil_private.h"

typedef struct Enlil_Thumb_Job Enlil_Thumb_Job;
typedef struct Enlil_Thumb Enlil_Thumb;

static void _thumb_connect_cb(void *data, Ethumb_Client *client, Eina_Bool success);
static void _thumb_die_cb(void *data, Ethumb_Client *client);
static void _thumb_done_cb(void *data, Ethumb_Client *client, int id, const char *file, const char *key, const char *thumb_path, const char *thumb_key, Eina_Bool success);
static void _job_next();
static void _job_free(Enlil_Thumb_Job *job);
static void _check_exists_thread_cb(void *data, Ecore_Thread *thread);
static void _check_exists_thread_done_cb(void *data, Ecore_Thread *thread);


struct Enlil_Thumb_Job
{
   Enlil_Thumb_Job_Type type;
   Enlil_Photo *photo;

   Enlil_Thumb_Done_Cb done_cb;
   Enlil_Thumb_Error_Cb error_cb;
   void *data;

   int try;
};

struct Enlil_Thumb
{
   Ethumb_Client *ethumb;
   int connected;
   int running;
   Eina_List *jobs;

   Enlil_Thumb_Job *current_job;
};

static Enlil_Thumb thumb;

static int count = 0;
int enlil_thumb_init()
{
   if(count>0) return ++count;

   thumb.running = EINA_FALSE;
   thumb.ethumb = ethumb_client_connect(_thumb_connect_cb, NULL, NULL);
   ethumb_client_on_server_die_callback_set(thumb.ethumb, _thumb_die_cb, NULL, NULL);
   return ++count;
}

int enlil_thumb_shutdown()
{
   if(count>1) return --count;

   enlil_thumb_clear();
   if(thumb.connected)
     ethumb_client_disconnect(thumb.ethumb);
   return --count;
}



const char* enlil_thumb_photo_get(Enlil_Photo *photo, Enlil_Thumb_Job_Type type,
      Enlil_Thumb_Done_Cb done_cb, Enlil_Thumb_Error_Cb error_cb, void *data)
{
   const char *path;
   Enlil_Thumb_Job *job;

   ASSERT_RETURN(photo!=NULL);

   switch(type)
     {
      case Enlil_THUMB_FDO_NORMAL:
	 if( (path = enlil_photo_thumb_fdo_normal_get(photo)) && ecore_file_exists(path))
	   return path;
	 break;
      case Enlil_THUMB_FDO_LARGE:
	 if( (path = enlil_photo_thumb_fdo_large_get(photo)) && ecore_file_exists(path))
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
	job = calloc(1, sizeof(Enlil_Thumb_Job));
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

void enlil_thumb_clear()
{
   Enlil_Thumb_Job *job;

   EINA_LIST_FREE(thumb.jobs, job)
     {
	_job_free(job); 
     }
   thumb.current_job = NULL;
}

void enlil_thumb_photo_clear(const Enlil_Photo *photo)
{
   Eina_List *l, *l_next;
   Enlil_Thumb_Job *job;

   EINA_LIST_FOREACH_SAFE(thumb.jobs, l, l_next, job)
     {
	if(job->photo == photo)
	  {
	     thumb.jobs = eina_list_remove(thumb.jobs, job);
	     _job_free(job);
	  }
     }
   if(thumb.current_job && thumb.current_job->photo == photo)
	 thumb.current_job = NULL;
}

static void _job_free(Enlil_Thumb_Job *job)
{
   ASSERT_RETURN_VOID(job != NULL);

   free(job);
}

static void _thumb_connect_cb(__UNUSED__ void *data, __UNUSED__ Ethumb_Client *client, Eina_Bool success)
{
   if(success)
     {
	thumb.connected = 1;
	_job_next(thumb);
     }
   else
     {
	enlil_thumb_shutdown();
	enlil_thumb_init();
	LOG_CRIT("CAN not connect to ethumb\n");
     }
}

static void _thumb_die_cb(__UNUSED__ void *data, __UNUSED__ Ethumb_Client *client)
{
   if(thumb.current_job)
     {
	if(thumb.current_job->error_cb)
	  thumb.current_job->error_cb(thumb.current_job->data, thumb.current_job->photo);

	_job_free(thumb.current_job);
	thumb.current_job = NULL;
     }

   thumb.running = 0;
   
   --count;

   if(thumb.connected)
     ethumb_client_disconnect(thumb.ethumb);

   enlil_thumb_init();
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

   Enlil_Thumb_Job *job = eina_list_nth(thumb.jobs,0);
   thumb.jobs = eina_list_remove(thumb.jobs, job);
   thumb.current_job = job;

   snprintf(buf, PATH_MAX, "%s/%s", enlil_photo_path_get(job->photo),enlil_photo_file_name_get(job->photo));
   ethumb_client_file_set(thumb.ethumb, buf, NULL);

   switch(job->type)
     {
      case Enlil_THUMB_FDO_LARGE:
	 ethumb_client_fdo_set(thumb.ethumb, ETHUMB_THUMB_LARGE);
	 break;
      case Enlil_THUMB_FDO_NORMAL:
	 ethumb_client_fdo_set(thumb.ethumb, ETHUMB_THUMB_NORMAL);
	 break;
     }

   thumb.running = 1;
   ecore_thread_run(_check_exists_thread_cb, _check_exists_thread_done_cb, NULL, job);
}

static void _check_exists_thread_cb(__UNUSED__ void *data, __UNUSED__ Ecore_Thread *thread)
{
   ethumb_client_thumb_exists(thumb.ethumb);
}


static void _check_exists_thread_done_cb(void *data, __UNUSED__ Ecore_Thread *thread)
{
   Enlil_Thumb_Job *job = data;
   const char *path = NULL;
   char buf[PATH_MAX];

   if(!thumb.current_job)
     {
	_job_free(job);
	thumb.running = 0;
	_job_next();
	return ;
     }

   if(ethumb_client_thumb_exists(thumb.ethumb))
     {
	ethumb_client_thumb_path_get(thumb.ethumb, &path, NULL);
	switch(job->type)
	  {
	   case Enlil_THUMB_FDO_LARGE:
	      enlil_photo_thumb_fdo_large_set(job->photo,path);
	      break;
	   case Enlil_THUMB_FDO_NORMAL:
	      enlil_photo_thumb_fdo_normal_set(job->photo, path);
	      break;
	  }
     }

   snprintf(buf, PATH_MAX, "%s/%s", enlil_photo_path_get(job->photo),enlil_photo_file_name_get(job->photo));

   if(path && ecore_file_exists(path))
	_thumb_done_cb(job, thumb.ethumb, -1, buf, NULL, path, NULL, EINA_TRUE);
   else
	ethumb_client_generate(thumb.ethumb, _thumb_done_cb, job, NULL);
}

static void _thumb_done_cb(void *data, __UNUSED__ Ethumb_Client *client, __UNUSED__ int id,
		__UNUSED__ const char *file, __UNUSED__ const char *key, const char *thumb_path,
		__UNUSED__ const char *thumb_key, Eina_Bool success)
{
   Enlil_Thumb_Job *job = (Enlil_Thumb_Job*) data;
  
   if(!thumb.current_job) goto end; 

   if(!success && job->try<3)
     {
	thumb.jobs = eina_list_prepend(thumb.jobs, job);
	job->try++;
	goto end_nofree;
     }
   else if(!success)
     {
	LOG_CRIT("Can't create the thumbnail of the photo %s/%s", enlil_photo_path_get(job->photo), enlil_photo_file_name_get(job->photo));
	if(job->error_cb)
	  job->error_cb(job->data, job->photo);
	goto end;
     }

   switch(job->type)
     {
      case Enlil_THUMB_FDO_LARGE:
	 enlil_photo_thumb_fdo_large_set(job->photo, thumb_path);
	 break;
      case Enlil_THUMB_FDO_NORMAL:
	 enlil_photo_thumb_fdo_normal_set(job->photo, thumb_path);
	 break;
     }

   if(job->done_cb)
     job->done_cb(job->data, job->photo, thumb_path);

   thumb.current_job = NULL;
end:
   _job_free(job);
end_nofree:
   thumb.running = 0;
   _job_next();
}

