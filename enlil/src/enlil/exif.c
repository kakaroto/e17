#include "enlil_private.h"
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <libexif/exif-data.h>

struct Enlil_Exif
{
   const char *tag;
   const char *value;
   const char *description;
};

struct Enlil_Exif_Job
{
   Enlil_Photo       *photo;

   Enlil_Exif_Done_Cb cb;
   void              *data;
};

typedef struct Enlil_Exif_Thread_Data
{
   const char *file;
   Eina_List  *l;
} Enlil_Exif_Thread_Data;

static Eina_List *l_jobs = NULL;

//do not use job_current in the thread
static Enlil_Exif_Job *job_current = NULL;

static int running = 0;

static void _job_free(Enlil_Exif_Job *job);
static void _job_next();
static void _exif_thread(void         *data,
                         Ecore_Thread *thread);
static void _end_cb(void         *data,
                    Ecore_Thread *thread);

Enlil_Exif *
enlil_exif_new()
{
   Enlil_Exif *exif = calloc(1, sizeof(Enlil_Exif));
   return exif;
}

Enlil_Exif *
enlil_exif_copy_new(Enlil_Exif *exif)
{
   ASSERT_RETURN(exif != NULL);

   Enlil_Exif *_exif = enlil_exif_new();

   enlil_exif_copy(exif, _exif);
   return _exif;
}

void
enlil_exif_copy(Enlil_Exif *exif_src,
                Enlil_Exif *exif_dest)
{
   ASSERT_RETURN_VOID(exif_src != NULL);
   ASSERT_RETURN_VOID(exif_dest != NULL);

   exif_dest->tag = eina_stringshare_add(exif_src->tag);
   exif_dest->value = eina_stringshare_add(exif_src->value);
   exif_dest->description = eina_stringshare_add(exif_src->description);
}

void
enlil_exif_free(Enlil_Exif **exif)
{
   ASSERT_RETURN_VOID(exif != NULL);
   Enlil_Exif *_exif = *exif;
   ASSERT_RETURN_VOID(_exif != NULL);

   if(_exif->tag)
     eina_stringshare_del(_exif->tag);

   if(_exif->value)
     eina_stringshare_del(_exif->value);

   if(_exif->description)
     eina_stringshare_del(_exif->description);
}

const char *
enlil_exif_tag_get(Enlil_Exif *exif)
{
   ASSERT_RETURN(exif != NULL);
   return exif->tag;
}

const char *
enlil_exif_value_get(Enlil_Exif *exif)
{
   ASSERT_RETURN(exif != NULL);
   return exif->value;
}

const char *
enlil_exif_description_get(Enlil_Exif *exif)
{
   ASSERT_RETURN(exif != NULL);
   return exif->description;
}

Eet_Data_Descriptor *
_enlil_exif_edd_new()
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Exif);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Exif, "tag", tag, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Exif, "value", value, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Exif, "description", description, EET_T_STRING);

   return edd;
}

Enlil_Exif_Job *
enlil_exif_job_append(Enlil_Photo       *photo,
                      Enlil_Exif_Done_Cb cb,
                      void              *data)
{
   ASSERT_RETURN(photo != NULL);
   Eina_List *l;
   Enlil_Exif_Job *job;

   EINA_LIST_FOREACH(l_jobs, l, job)
     if(job->photo == photo)
       break;

   if(!job)
     {
        job = calloc(1, sizeof(Enlil_Exif_Job));
        job->photo = photo;
        job->cb = cb;
        job->data = data;

        l_jobs = eina_list_append(l_jobs, job);
     }

   _job_next();

   return job;
}

Enlil_Exif_Job *
enlil_exif_job_prepend(Enlil_Photo       *photo,
                       Enlil_Exif_Done_Cb cb,
                       void              *data)
{
   ASSERT_RETURN(photo != NULL);
   Eina_List *l;
   Enlil_Exif_Job *job;

   EINA_LIST_FOREACH(l_jobs, l, job)
     if(job->photo == photo)
       break;

   if(!job)
     {
        job = calloc(1, sizeof(Enlil_Exif_Job));
        job->photo = photo;
        job->cb = cb;
        job->data = data;
     }
   else
     l_jobs = eina_list_remove(l_jobs, job);

   l_jobs = eina_list_prepend(l_jobs, job);

   _job_next();

   return job;
}

void
enlil_exif_job_del(Enlil_Exif_Job *job)
{
   ASSERT_RETURN_VOID(job != NULL);

   if(job == job_current)
     {
        job_current = NULL;
     }

   l_jobs = eina_list_remove(l_jobs, job);

   _job_free(job);
}

static void
_job_free(Enlil_Exif_Job *job)
{
   ASSERT_RETURN_VOID(job != NULL);

   FREE(job);
}

static void
_job_next()
{
   char buf[PATH_MAX];

   if(running)
     return;

   if(!l_jobs)
     return;

   running = 1;
   Enlil_Exif_Job *job = eina_list_data_get(l_jobs);
   l_jobs = eina_list_remove(l_jobs, job);
   job_current = job;

   snprintf(buf, PATH_MAX, "%s/%s", enlil_photo_path_get(job_current->photo),
            enlil_photo_file_name_get(job_current->photo));

   Enlil_Exif_Thread_Data *data = calloc(1, sizeof(Enlil_Exif_Thread_Data));
   data->file = eina_stringshare_add(buf);

   ecore_thread_run(_exif_thread, _end_cb, NULL, data);

   running = 1;
}

static void
_exif_content_foreach(ExifEntry *ee,
                      void      *data)
{
   Enlil_Exif_Thread_Data *t_data = data;

   char buf[PATH_MAX];
   exif_entry_get_value(ee, buf, PATH_MAX);

   Enlil_Exif *exif = calloc(1, sizeof(Enlil_Exif));
   exif->tag = eina_stringshare_add(exif_tag_get_name(ee->tag));
   exif->value = eina_stringshare_add(buf);
   exif->description = eina_stringshare_add(exif_tag_get_description(ee->tag));

   t_data->l = eina_list_append(t_data->l, exif);
}

static void
_exif_data_foreach(ExifContent *ec,
                   void        *data)
{
   exif_content_foreach_entry(ec, _exif_content_foreach, data);
}

static void
_exif_thread(void         *data,
             Ecore_Thread *thread __UNUSED__)
{
   Enlil_Exif_Thread_Data *t_data = data;
   const char *file = t_data->file;
   ExifData *ed;

   LOG_INFO("Load Exif data from the photo %s", file);
   ed = exif_data_new_from_file(file);
   if(!ed)
     goto end;

   exif_data_foreach_content(ed, _exif_data_foreach, t_data);

end:
   ;
}

static void
_end_cb(void         *data,
        Ecore_Thread *thread __UNUSED__)
{
   Enlil_Exif_Thread_Data *t_data = data;
   Enlil_Exif_Job *job = job_current;

   if(!job_current) //job deleted by the user
     {
        Enlil_Exif *exif;
        EINA_LIST_FREE(t_data->l, exif)
          enlil_exif_free(&exif);
        goto end;
     }

   if(job->cb)
     job->cb(job->data, job, t_data->l);

   _job_free(job);
end:
   eina_stringshare_del(t_data->file);
   free(t_data);
   running = 0;
   _job_next();
}

