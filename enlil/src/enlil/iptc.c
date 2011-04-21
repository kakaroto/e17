#include "enlil_private.h"
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <libiptcdata/iptc-data.h>
#include <libiptcdata/iptc-jpeg.h>

struct Enlil_IPTC
{
   double record;
   double tag;
   const char *name;
   const char *title;
   const char *format;
   const char *value;
};

struct Enlil_IPTC_Job
{
   Enlil_Photo *photo;

   Enlil_IPTC_Done_Cb cb;
   void *data;
};

typedef struct Enlil_IPTC_Thread_Data
{
   const char *file;
   Eina_List *l;
} Enlil_IPTC_Thread_Data;

static Eina_List *l_jobs = NULL;

//do not use job_current in the thread
static Enlil_IPTC_Job *job_current = NULL;

static int running = 0;

static void _job_free(Enlil_IPTC_Job *job);
static void _job_next();
static void _iptc_thread(void *data, Ecore_Thread *thread);
static void _end_cb(void *data, Ecore_Thread *thread);

Enlil_IPTC *enlil_iptc_new()
{
   Enlil_IPTC *iptc = calloc(1,sizeof(Enlil_IPTC));
   return iptc;
}

Enlil_IPTC *enlil_iptc_new_from_name(const char *name)
{
   IptcRecord record;
   IptcTag tag;
   Enlil_IPTC *iptc;

   if (iptc_tag_find_by_name (name, &record, &tag) < 0) {
	LOG_ERR("Invalid tag name : #%s#", name);
	return NULL;
   }

   iptc = calloc(1,sizeof(Enlil_IPTC));
   iptc->record = record;
   iptc->tag = tag;
   iptc->name = eina_stringshare_add(name);
   iptc->title = eina_stringshare_add(iptc_tag_get_title(record, tag));

   return iptc;
}


Enlil_IPTC *enlil_iptc_copy_new(Enlil_IPTC *iptc)
{
   ASSERT_RETURN(iptc!=NULL);

   Enlil_IPTC *_iptc = enlil_iptc_new();

   enlil_iptc_copy(iptc, _iptc);
   return _iptc;
}

void enlil_iptc_copy(Enlil_IPTC *iptc_src, Enlil_IPTC *iptc_dest)
{
   ASSERT_RETURN_VOID(iptc_src!=NULL);
   ASSERT_RETURN_VOID(iptc_dest!=NULL);

   iptc_dest->record = iptc_src->record;
   iptc_dest->tag = iptc_src->tag;
   iptc_dest->name = eina_stringshare_add(iptc_src->name);
   iptc_dest->title = eina_stringshare_add(iptc_src->title);
   iptc_dest->value = eina_stringshare_add(iptc_src->value);
}

void enlil_iptc_free(Enlil_IPTC **iptc)
{
   ASSERT_RETURN_VOID(iptc != NULL);
   Enlil_IPTC *_iptc = *iptc;
   ASSERT_RETURN_VOID(_iptc != NULL);

   if(_iptc->name)
     eina_stringshare_del(_iptc->name);

   if(_iptc->title)
     eina_stringshare_del(_iptc->title);

   if(_iptc->value)
     eina_stringshare_del(_iptc->value);

   FREE(_iptc);
}

#define FCT_NAME enlil_iptc
#define STRUCT_TYPE Enlil_IPTC

STRING_SET(value)

GET(record, double)
GET(tag, double)
GET(name, const char *)
GET(title, const char *)
GET(value, const char *)
GET(format, const char *)

#undef FCT_NAME
#undef STRUCT_TYPE

Eet_Data_Descriptor * _enlil_iptc_edd_new()
{
    Eet_Data_Descriptor *edd;
    Eet_Data_Descriptor_Class eddc;

    EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_IPTC);
    eddc.func.str_direct_alloc = NULL;
    eddc.func.str_direct_free = NULL;

    edd = eet_data_descriptor_file_new(&eddc);

    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_IPTC, "record", tag, EET_T_DOUBLE);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_IPTC, "tag", tag, EET_T_DOUBLE);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_IPTC, "name", name, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_IPTC, "title", title, EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_IPTC, "value", value, EET_T_STRING);

    return edd;
}

Enlil_IPTC_Job *enlil_iptc_job_append(Enlil_Photo *photo, Enlil_IPTC_Done_Cb cb, void *data)
{
    ASSERT_RETURN(photo!=NULL);
    Eina_List *l;
    Enlil_IPTC_Job *job;

    EINA_LIST_FOREACH(l_jobs, l, job)
       if(job->photo == photo)
	 break;

    if(!job)
      {
	 job = calloc(1, sizeof(Enlil_IPTC_Job));
	 job->photo = photo;
	 job->cb = cb;
	 job->data = data;

	 l_jobs = eina_list_append(l_jobs, job);
      }

    _job_next();

    return job;
}

Enlil_IPTC_Job *enlil_iptc_job_prepend(Enlil_Photo *photo, Enlil_IPTC_Done_Cb cb, void *data)
{
    ASSERT_RETURN(photo!=NULL);
    Eina_List *l;
    Enlil_IPTC_Job *job;

    EINA_LIST_FOREACH(l_jobs, l, job)
       if(job->photo == photo)
	 break;

    if(!job)
      {
	 job = calloc(1, sizeof(Enlil_IPTC_Job));
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

void enlil_iptc_job_del(Enlil_IPTC_Job *job)
{
    ASSERT_RETURN_VOID(job != NULL);

    if(job == job_current)
    {
        job_current = NULL;
    }

    l_jobs = eina_list_remove(l_jobs, job);

    _job_free(job);
}

static void _job_free(Enlil_IPTC_Job *job)
{
    ASSERT_RETURN_VOID(job != NULL);

    FREE(job);
}

static void _job_next()
{
   char buf[PATH_MAX];

    if(running)
        return;

    if(!l_jobs)
        return ;

    running = 1;
    Enlil_IPTC_Job *job = eina_list_data_get(l_jobs);
    l_jobs = eina_list_remove(l_jobs, job);
    job_current = job;

    snprintf(buf, PATH_MAX, "%s/%s", enlil_photo_path_get(job_current->photo),
	 enlil_photo_file_name_get(job_current->photo));

    Enlil_IPTC_Thread_Data *data = calloc(1, sizeof(Enlil_IPTC_Thread_Data));
    data->file = eina_stringshare_add(buf);

    ecore_thread_run(_iptc_thread, _end_cb, NULL, data);

    running = 1;
}

static void _iptc_thread(void *data, __UNUSED__ Ecore_Thread *thread)
{
   Enlil_IPTC_Thread_Data *t_data = data;
   const char *file = t_data->file;
   IptcData *d;
   int i;

   LOG_INFO("Load IPTC data from the photo %s", file);

   d = iptc_data_new_from_jpeg(file);
   if(!d)
     goto end;

   for (i=0; i < (int)d->count; i++)
     {
	IptcDataSet * e = d->datasets[i];
	Enlil_IPTC *iptc = calloc(1, sizeof(Enlil_IPTC));
	t_data->l = eina_list_append(t_data->l, iptc);
	char buf[256];

	iptc->record = e->record;
	iptc->tag = e->tag;
	iptc->name = eina_stringshare_add(iptc_tag_get_name(e->record, e->tag));
	iptc->title = eina_stringshare_add(iptc_tag_get_title(e->record, e->tag));
	iptc->format = eina_stringshare_add(iptc_format_get_name(iptc_dataset_get_format (e)));

	switch (iptc_dataset_get_format (e)) {
	   case IPTC_FORMAT_BYTE:
	   case IPTC_FORMAT_SHORT:
	   case IPTC_FORMAT_LONG:
	      snprintf(buf, 256, "%d", iptc_dataset_get_value (e));
	      break;
	   case IPTC_FORMAT_BINARY:
	      iptc_dataset_get_as_str (e, buf, sizeof(buf));
	      break;
	   default:
	      iptc_dataset_get_data (e, (unsigned char *)buf, sizeof(buf));
	      break;
	}
	iptc->value = eina_stringshare_add(buf);
     }

end:
   ;
}

static void _end_cb(void *data, __UNUSED__ Ecore_Thread *thread)
{
   Enlil_IPTC_Thread_Data *t_data = data;
   Enlil_IPTC_Job *job = job_current;

   if(!job_current) //job deleted by the user
     {
	Enlil_IPTC *iptc;
	EINA_LIST_FREE(t_data->l, iptc)
	   enlil_iptc_free(&iptc);
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

