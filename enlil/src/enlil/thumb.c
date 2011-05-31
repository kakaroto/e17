#include "enlil_private.h"

typedef struct Enlil_Thumb_Job Enlil_Thumb_Job;
typedef struct Enlil_Thumb     Enlil_Thumb;

static void _thumb_connect_cb(void          *data,
                              Ethumb_Client *client,
                              Eina_Bool      success);
static void _thumb_die_cb(void          *data,
                          Ethumb_Client *client);
static void _thumb_done_cb(void          *data,
                           Ethumb_Client *client,
                           int            id,
                           const char    *file,
                           const char    *key,
                           const char    *thumb_path,
                           const char    *thumb_key,
                           Eina_Bool      success);

struct Enlil_Thumb_Job
{
   Enlil_Thumb_Done_Cb  done_cb;
   Enlil_Thumb_Error_Cb error_cb;
   void                *data;

   Ethumb_Exists       *thread;
   Enlil_Photo         *photo;

   Enlil_Thumb_Job_Type type;
   long                 id;
};

struct Enlil_Thumb
{
   Ethumb_Client *ethumb;

   int            connected;
   int            running;

   Eina_List     *jobs;
};

static Enlil_Thumb thumb;

static int count = 0;
int
enlil_thumb_init(void)
{
   if(count > 0) return ++count;

   thumb.running = EINA_FALSE;
   thumb.ethumb = ethumb_client_connect(_thumb_connect_cb, NULL, NULL);
   ethumb_client_on_server_die_callback_set(thumb.ethumb, _thumb_die_cb, NULL, NULL);

   return ++count;
}

int
enlil_thumb_shutdown(void)
{
   if(count > 1) return --count;

   enlil_thumb_clear();
   if(thumb.connected)
     ethumb_client_disconnect(thumb.ethumb);
   return --count;
}

void
enlil_exists_cb(Ethumb_Client *client,
                Ethumb_Exists *request,
                Eina_Bool      exists,
                void          *data)
{
   Enlil_Thumb_Job *job = data;

   if (ethumb_client_thumb_exists_check(request))
     return;

   if (exists)
     {
        const char *path;

        ethumb_client_thumb_path_get(client, &path, NULL);
        switch (job->type)
          {
           case Enlil_THUMB_FDO_LARGE:
             enlil_photo_thumb_fdo_large_set(job->photo, path);
             break;

           case Enlil_THUMB_FDO_NORMAL:
             enlil_photo_thumb_fdo_normal_set(job->photo, path);
             break;
          }

        if (job->done_cb)
          job->done_cb(job->data, job->photo, path);

        thumb.jobs = eina_list_remove(thumb.jobs, job);
        free(job);
     }
   else
     {
        char buf[PATH_MAX];

        snprintf(buf, PATH_MAX, "%s/%s",
                 enlil_photo_path_get(job->photo),
                 enlil_photo_file_name_get(job->photo));

        job->thread = NULL;
        job->id = ethumb_client_generate(thumb.ethumb, _thumb_done_cb, job, NULL);
     }
}

const char *
enlil_thumb_photo_get(Enlil_Photo         *photo,
                      Enlil_Thumb_Job_Type type,
                      Enlil_Thumb_Done_Cb  done_cb,
                      Enlil_Thumb_Error_Cb error_cb,
                      void                *data)
{
   const char *path;
   Enlil_Thumb_Job *job;
   char buf[PATH_MAX];

   ASSERT_RETURN(photo != NULL);

   switch (type)
     {
      case Enlil_THUMB_FDO_NORMAL:
        if ((path = enlil_photo_thumb_fdo_normal_get(photo))
            && ecore_file_exists(path))
          return path;
        break;

      case Enlil_THUMB_FDO_LARGE:
        if ((path = enlil_photo_thumb_fdo_large_get(photo))
            && ecore_file_exists(path))
          return path;
        break;
     }

   snprintf(buf, PATH_MAX, "%s/%s",
            enlil_photo_path_get(photo),
            enlil_photo_file_name_get(photo));
   ethumb_client_file_set(thumb.ethumb, buf, NULL);

   switch (type)
     {
      case Enlil_THUMB_FDO_LARGE:
        ethumb_client_fdo_set(thumb.ethumb, ETHUMB_THUMB_LARGE);
        break;

      case Enlil_THUMB_FDO_NORMAL:
        ethumb_client_fdo_set(thumb.ethumb, ETHUMB_THUMB_NORMAL);
        break;
     }

   job = calloc(1, sizeof (Enlil_Thumb_Job));
   if (!job) return NULL;

   job->photo = photo;
   job->type = type;
   job->done_cb = done_cb;
   job->error_cb = error_cb;
   job->data = data;
   job->thread = ethumb_client_thumb_exists(thumb.ethumb,
                                            enlil_exists_cb,
                                            job);

   thumb.jobs = eina_list_prepend(thumb.jobs, job);

   return NULL;
}

void
enlil_thumb_clear(void)
{
   Enlil_Thumb_Job *job;

   EINA_LIST_FREE(thumb.jobs, job)
     {
        if (job->thread)
          ethumb_client_thumb_exists_cancel(job->thread, enlil_exists_cb, job);
        else
          ethumb_client_generate_cancel(thumb.ethumb, job->id, NULL, NULL, NULL);
     }
}

void
enlil_thumb_photo_clear(const Enlil_Photo *photo)
{
   Eina_List *l, *l_next;
   Enlil_Thumb_Job *job;

   EINA_LIST_FOREACH_SAFE(thumb.jobs, l, l_next, job)
     {
        if(job->photo == photo)
          {
             thumb.jobs = eina_list_remove(thumb.jobs, job);

             if (job->thread)
               ethumb_client_thumb_exists_cancel(job->thread, enlil_exists_cb, job);
             else
               ethumb_client_generate_cancel(thumb.ethumb, job->id, NULL, NULL, NULL);
          }
     }
}

static void
_thumb_connect_cb(void          *data __UNUSED__,
                  Ethumb_Client *client __UNUSED__,
                  Eina_Bool      success)
{
   if(success)
     {
        thumb.connected = 1;
     }
   else
     {
        enlil_thumb_shutdown();
        enlil_thumb_init();
        LOG_CRIT("CAN not connect to ethumb\n");
     }
}

static void
_thumb_die_cb(void          *data __UNUSED__,
              Ethumb_Client *client __UNUSED__)
{
   if(thumb.connected)
     ethumb_client_disconnect(thumb.ethumb);

   thumb.running = EINA_FALSE;
   thumb.ethumb = ethumb_client_connect(_thumb_connect_cb, NULL, NULL);
   ethumb_client_on_server_die_callback_set(thumb.ethumb, _thumb_die_cb, NULL, NULL);
}

static void
_thumb_done_cb(void          *data,
               Ethumb_Client *client __UNUSED__,
               int            id __UNUSED__,
               const char    *file __UNUSED__,
               const char    *key __UNUSED__,
               const char    *thumb_path,
               const char    *thumb_key __UNUSED__,
               Eina_Bool      success)
{
   Enlil_Thumb_Job *job = data;

   thumb.jobs = eina_list_remove(thumb.jobs, job);

   if (!success)
     {
        LOG_CRIT("Can't create the thumbnail of the photo %s/%s",
                 enlil_photo_path_get(job->photo),
                 enlil_photo_file_name_get(job->photo));
        if (job->error_cb)
          job->error_cb(job->data, job->photo);
        goto end;
     }

   switch (job->type)
     {
      case Enlil_THUMB_FDO_LARGE:
        enlil_photo_thumb_fdo_large_set(job->photo, thumb_path);
        break;

      case Enlil_THUMB_FDO_NORMAL:
        enlil_photo_thumb_fdo_normal_set(job->photo, thumb_path);
        break;
     }

   if (job->done_cb)
     job->done_cb(job->data, job->photo, thumb_path);

end:
   free(job);
}

