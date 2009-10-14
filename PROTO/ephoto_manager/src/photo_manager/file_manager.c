// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#include "photo_manager_private.h"

//LRU list of Eet_File *
#define MAX_SIZE_LIST 3

typedef struct File_Table_Data File_Table_Data;

static void _data_free(File_Table_Data *data);
File_Table_Data *_pm_file_manager_find(const char *file);

static Eina_List *file_list = NULL;
static pthread_mutex_t mutex;

struct File_Table_Data
{
   const char *path;
   Eet_File *f;
   pthread_mutex_t mutex;
};

static int count;
int pm_file_manager_init()
{
   if(count>0) return ++count;

   pthread_mutex_init(&mutex, NULL);

   return ++count;
}

int pm_file_manager_shutdown()
{
   File_Table_Data *data;
   if(count>1) return --count;

   pm_file_manager_flush();

   pthread_mutex_lock(&mutex);
   EINA_LIST_FREE(file_list, data)
     {
	 _data_free(data);
     }
   pthread_mutex_unlock(&mutex);

   return --count;
}

File_Table_Data *_pm_file_manager_find(const char *file)
{
   Eina_List *l;
   File_Table_Data *data;
   EINA_LIST_FOREACH(file_list, l, data)
      if(data->path == file)
	return data;
   return NULL;
}

void _pm_file_manager_clean()
{
   pthread_mutex_lock(&mutex);
   if(eina_list_count(file_list)>MAX_SIZE_LIST)
     {

	Eina_List *l, *l_next;
	File_Table_Data *data;
	EINA_LIST_FOREACH_SAFE(file_list, l, l_next, data)
	  {
	     if(pthread_mutex_trylock(&(data->mutex)) != EBUSY)
	       {
		  file_list = eina_list_remove(file_list, data);

		  _data_free(data);
	       }
	     else
	       break;

	     if(eina_list_count(file_list)<=MAX_SIZE_LIST)
		  break;
	  }
     }
   pthread_mutex_unlock(&mutex);
}

Eet_File *pm_file_manager_open(const char *file)
{
   //LOG_DBG("Open file %s", file);
   pthread_mutex_lock(&mutex);
   file = eina_stringshare_add(file);
   File_Table_Data *f_data = _pm_file_manager_find(file);

   if(f_data)
     {	
	file_list = eina_list_remove(file_list, f_data);
	file_list = eina_list_append(file_list, f_data);
	pthread_mutex_unlock(&mutex);

	pthread_mutex_lock(&(f_data->mutex));
	if(!f_data->f)
	  {
	     f_data->f = eet_open(file, EET_FILE_MODE_READ_WRITE);
	     if(!f_data->f)
	       {
		  f_data->f = eet_open(file, EET_FILE_MODE_WRITE);
	       }
	  }
     }
   else
     {
	f_data = calloc(1, sizeof(File_Table_Data));
	pthread_mutex_init(&(f_data->mutex), NULL);

	file_list = eina_list_append(file_list, f_data);

	pthread_mutex_unlock(&mutex);
	_pm_file_manager_clean();

	pthread_mutex_lock(&(f_data->mutex));

	f_data->path = eina_stringshare_add(file);

	f_data->f = eet_open(file, EET_FILE_MODE_READ_WRITE);
	if(!f_data->f)
	  f_data->f = eet_open(file, EET_FILE_MODE_WRITE);
     }
   eina_stringshare_del(file);

   ASSERT(f_data->f != NULL);
   return f_data->f;
}

void pm_file_manager_close(const char *file)
{
   //LOG_DBG("Close file %s", file);
   pthread_mutex_lock(&mutex);
   file = eina_stringshare_add(file);
   File_Table_Data *f_data = _pm_file_manager_find(file);
   pthread_mutex_unlock(&mutex);

   if(f_data)
     {
	pthread_mutex_unlock(&(f_data->mutex));
     }
   else
     LOG_ERR("The file is unknown ! %s", file);

   EINA_STRINGSHARE_DEL(file);
}

void pm_file_manager_flush()
{
   Eina_List *l;
   File_Table_Data *data;
   pthread_mutex_lock(&mutex);

   EINA_LIST_FOREACH(file_list, l, data)
     {
	if(pthread_mutex_trylock(&(data->mutex)) != EBUSY)
	  {
	     if(data->f)
	       eet_close(data->f);
	     data->f = NULL;
	     pthread_mutex_unlock(&(data->mutex));
	  }
     }

   pthread_mutex_unlock(&mutex);
}

static void _data_free(File_Table_Data *data)
{
   if(data->f)
     eet_close(data->f);
   EINA_STRINGSHARE_DEL(data->path);
   FREE(data);
}

