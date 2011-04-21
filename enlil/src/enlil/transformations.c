#include "enlil_private.h"
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>


struct Enlil_Trans_Job
{
    Enlil_Trans_Job_Type type;
    const char *file;
    const char *res_file;

    Enlil_Trans_History *history;

    Enlil_Trans_Done_Cb cb;
    void *data;
};

struct Enlil_Trans_History_Item
{
    const char *file;
    Enlil_Trans_Job_Type type;
};

struct Enlil_Trans_History
{
    Eina_List *history;
    Enlil_Trans_History_Item *current;

    const char *folder;
};

static Eina_List *l_jobs = NULL;
static Enlil_Trans_Job *job_current = NULL;
static Ecore_Exe *exe_job_current = NULL;
static int running = 0;

static void _enlil_trans_history_add(Enlil_Trans_History *h, const char *file, Enlil_Trans_Job_Type type);

static void _job_free(Enlil_Trans_Job *job);
static void _job_next();
static void _end_cb(void *data);
static Eina_Bool _exe_del_cb(void *data, int type, void *event);

static int count = 0;
static Ecore_Event_Handler *handler_exe_del = NULL;

int enlil_trans_init()
{
   if(count++>0) return count;
   handler_exe_del = ecore_event_handler_add(ECORE_EXE_EVENT_DEL, _exe_del_cb, NULL);

   return count;
}

int enlil_trans_shutdown()
{
   if(count-->1) return count;
   ecore_event_handler_del(handler_exe_del);
   handler_exe_del = NULL;
   return count;
}

Enlil_Trans_Job *enlil_trans_job_add(Enlil_Trans_History *h, const char *file, Enlil_Trans_Job_Type type, Enlil_Trans_Done_Cb cb, void *data)
{
    ASSERT_RETURN(h != NULL);
    ASSERT_RETURN(file!=NULL);

    Enlil_Trans_Job *job = calloc(1, sizeof(Enlil_Trans_Job));
    job->file =  eina_stringshare_add(file);
    job->type = type;
    job->cb = cb;
    job->data = data;
    job->history = h;

    l_jobs = eina_list_append(l_jobs, job);

    _job_next();

    return job;
}

void enlil_trans_job_del(Enlil_Trans_Job *job)
{
    ASSERT_RETURN_VOID(job != NULL);

    if(job == job_current)
    {
        job_current = NULL;
	ecore_exe_kill(exe_job_current);
	exe_job_current = NULL;
    }

    l_jobs = eina_list_remove(l_jobs, job);

    _job_free(job);
}

Enlil_Trans_History *enlil_trans_history_new(const char *file)
{
    char buf[PATH_MAX];
    int i = 0;

    ASSERT_RETURN(file!= NULL);

    Enlil_Trans_History *h = calloc(1, sizeof(Enlil_Trans_History));

    snprintf(buf, PATH_MAX, TEMP"/eenlil");
    if(!ecore_file_exists(buf))
        ecore_file_mkdir(buf);

    do
    {
        snprintf(buf, PATH_MAX, TEMP"/eenlil/%d", i);
        i++;
    } while(ecore_file_exists(buf));

    ecore_file_mkdir(buf);
    h->folder = eina_stringshare_add(buf);

    snprintf(buf, PATH_MAX, "%s/%d%s", h->folder,
            eina_list_count(h->history), strrchr(file,'.'));
    ecore_file_cp(file, buf);
    _enlil_trans_history_add(h, buf, Enlil_TRANS_ROTATE_180);

    return h;
}

void enlil_trans_history_free(Enlil_Trans_History *h)
{
    ASSERT_RETURN_VOID(h != NULL);

    enlil_trans_history_clear(h);
    ecore_file_recursive_rm(h->folder);
    EINA_STRINGSHARE_DEL(h->folder);
    FREE(h);
}

const Eina_List *enlil_trans_history_get(const Enlil_Trans_History *h)
{
    ASSERT_RETURN(h != NULL);
    return h->history;
}

const Enlil_Trans_History_Item *enlil_trans_history_current_get(const Enlil_Trans_History *h)
{
    ASSERT_RETURN(h != NULL);
    return h->current;
}


void enlil_trans_history_clear(Enlil_Trans_History *h)
{
    Enlil_Trans_History_Item *item;
    ASSERT_RETURN_VOID(h != NULL);

    EINA_LIST_FREE(h->history, item)
    {
        ecore_file_recursive_rm(item->file);
        EINA_STRINGSHARE_DEL(item->file);
        FREE(item);
    }
    h->current = NULL;
}

const char *enlil_trans_history_goto(Enlil_Trans_History *h, int pos)
{
    ASSERT_RETURN(h != NULL);
    ASSERT_RETURN(pos >= 0 && pos < (int)eina_list_count(h->history));

    h->current = eina_list_nth(h->history, pos);
    return h->current->file;
}

const char *enlil_trans_history_item_file_get(const Enlil_Trans_History_Item *item)
{
    ASSERT_RETURN(item != NULL);
    return item->file;
}

Enlil_Trans_Job_Type enlil_trans_history_item_type_get(const Enlil_Trans_History_Item *item)
{
    ASSERT_RETURN(item != NULL);
    return item->type;
}

static void _enlil_trans_history_add(Enlil_Trans_History *h, const char *file, Enlil_Trans_Job_Type type)
{
    Enlil_Trans_History_Item *item;
    Eina_List *l, *l2;

    ASSERT_RETURN_VOID(h != NULL);
    ASSERT_RETURN_VOID(file != NULL);

    LOG_DBG("Add transition in history");
    l = eina_list_data_find_list(h->history, h->current);
    l = eina_list_next(l);
    EINA_LIST_FOREACH(l, l2, item)
    {
        h->history = eina_list_remove(h->history, item);
        ecore_file_recursive_rm(item->file);
        EINA_STRINGSHARE_DEL(item->file);
        FREE(item);
    }

    item = calloc(1, sizeof(Enlil_Trans_History_Item));
    item->file = eina_stringshare_add(file);
    item->type = type;


    h->history = eina_list_append(h->history, item);
    h->current = item;
}

static void _job_free(Enlil_Trans_Job *job)
{
    ASSERT_RETURN_VOID(job != NULL);

    EINA_STRINGSHARE_DEL(job->file);
    EINA_STRINGSHARE_DEL(job->res_file);
    FREE(job);
}

static void _job_next()
{
    char buf[PATH_MAX], buf2[PATH_MAX];
    char *trans = NULL;

    if(running)
        return;

    if(!l_jobs)
        return ;

    running = 1;
    Enlil_Trans_Job *job = eina_list_nth(l_jobs,0);
    l_jobs = eina_list_remove(l_jobs, job);
    job_current = job;

    snprintf(buf, PATH_MAX, "%s/%d%s", job->history->folder,
            eina_list_count(job->history->history), strrchr(job->file,'.'));
    job->res_file = eina_stringshare_add(buf);


    switch(job->type)
      {
       case Enlil_TRANS_ROTATE_180:
	  trans = "Enlil_TRANS_ROTATE_180";
	  break;
       case Enlil_TRANS_ROTATE_90:
	  trans = "Enlil_TRANS_ROTATE_90";
	  break;
       case Enlil_TRANS_ROTATE_R90:
	  trans = "Enlil_TRANS_ROTATE_R90";
	  break;
       case Enlil_TRANS_FLIP_VERTICAL:
	  trans = "Enlil_TRANS_FLIP_VERTICAL";
	  break;
       case Enlil_TRANS_FLIP_HORIZONTAL:
	  trans = "Enlil_TRANS_FLIP_HORIZONTAL";
	  break;
       case Enlil_TRANS_BLUR:
	  trans = "Enlil_TRANS_BLUR";
	  break;
       case Enlil_TRANS_SHARPEN:
	  trans = "Enlil_TRANS_SHARPEN";
	  break;
       case Enlil_TRANS_GRAYSCALE:
	  trans = "Enlil_TRANS_GRAYSCALE";
	  break;
       case Enlil_TRANS_SEPIA:
	  trans = "Enlil_TRANS_SEPIA";
	  break;
      }

    snprintf(buf2, PATH_MAX, "enlil_transformations --fin \"%s\" --fout \"%s\" --trans \"%s\"", job->file, buf, trans);

    LOG_INFO("Execute %s", buf2);
    exe_job_current = ecore_exe_run(buf2, NULL);

    if(!exe_job_current)
      {
	 LOG_ERR("Can not run the command : %s", buf2);
	 return ;
      }
}




static void _end_cb(void *data)
{
    Enlil_Trans_Job *job = data;

    if(!job_current) //job deleted by the user
        goto end;

    if(job->history)
        _enlil_trans_history_add(job->history, job->res_file, job->type);

    if(job->cb)
        job->cb(job->data, job, job->res_file);

    _job_free(job);
end:
    running = 0;
    _job_next();
}


static Eina_Bool _exe_del_cb(__UNUSED__ void *data, __UNUSED__ int type, __UNUSED__ void *event)
{
   _end_cb(job_current);
   return EINA_TRUE;
}

