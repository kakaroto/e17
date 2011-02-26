#include "enlil_private.h"

/* FIXME: destroy the mutex */

#ifdef EFL_HAVE_POSIX_THREADS
# define ENLIL_MUTEX pthread_mutex_t
# define ENLIL_MUTEX_INIT(Mutex) pthread_mutex_init(&(Mutex), NULL)
# define ENLIL_MUTEX_LOCK(Mutex) pthread_mutex_lock(&(Mutex))
# define ENLIL_MUTEX_UNLOCK(Mutex) pthread_mutex_unlock(&(Mutex))
#else
# include <windows.h>
# define ENLIL_MUTEX HANDLE
# define ENLIL_MUTEX_INIT(Mutex) Mutex = CreateMutex(NULL, FALSE, NULL)
# define ENLIL_MUTEX_LOCK(Mutex) WaitForSingleObject(Mutex, INFINITE)
# define ENLIL_MUTEX_UNLOCK(Mutex) ReleaseMutex(Mutex)
#endif

//LRU list of Eet_File *
#define MAX_SIZE_LIST 3

typedef struct File_Table_Data File_Table_Data;
typedef struct EET_File_Version EET_File_Version;

static void _data_free(File_Table_Data *data);
File_Table_Data *_enlil_file_manager_find(const char *file);
static Eet_Data_Descriptor * _edd_version_new();

static Eina_List *file_list = NULL;
static ENLIL_MUTEX mutex;

static EET_File_Version current_version;
static Eet_Data_Descriptor *edd_version = NULL;


struct EET_File_Version
{
   double version;
};

struct File_Table_Data
{
   const char *path;
   Eet_File *f;
   //pthread_mutex_t mutex;
};

static int count;
int enlil_file_manager_init()
{
   if(count>0) return ++count;

   current_version.version = EET_FILE_VERSION;
   edd_version = _edd_version_new();

   ENLIL_MUTEX_INIT(mutex);

   return ++count;
}

int enlil_file_manager_shutdown()
{
   File_Table_Data *data;
   if(count>1) return --count;

   enlil_file_manager_flush();

   ENLIL_MUTEX_LOCK(mutex);
   EINA_LIST_FREE(file_list, data)
     {
	 _data_free(data);
     }
   ENLIL_MUTEX_UNLOCK(mutex);
   eet_data_descriptor_free(edd_version);
   edd_version = NULL;

   return --count;
}

File_Table_Data *_enlil_file_manager_find(const char *file)
{
   Eina_List *l;
   File_Table_Data *data;
   EINA_LIST_FOREACH(file_list, l, data)
      if(data->path == file)
	return data;
   return NULL;
}

void _enlil_file_manager_clean()
{
   if(eina_list_count(file_list)>MAX_SIZE_LIST - 1)
     {
	Eina_List *l, *l_next;
	File_Table_Data *data;
	EINA_LIST_FOREACH_SAFE(file_list, l, l_next, data)
	  {
	     file_list = eina_list_remove(file_list, data);
	     _data_free(data);

	     if(eina_list_count(file_list)<=MAX_SIZE_LIST - 1)
		  break;
	  }
     }
}

Eet_File *enlil_file_manager_open(const char *file)
{
   //LOG_DBG("Open file %s", file);
   ENLIL_MUTEX_LOCK(mutex);
   file = eina_stringshare_add(file);
   File_Table_Data *f_data = _enlil_file_manager_find(file);
   EET_File_Version *f_version;

   if(f_data)
     {
	file_list = eina_list_remove(file_list, f_data);
	file_list = eina_list_append(file_list, f_data);

	if(!f_data->f)
	  {
	     f_data->f = eet_open(file, EET_FILE_MODE_READ_WRITE);
	     if(!f_data->f)
	       {
		  f_data->f = eet_open(file, EET_FILE_MODE_WRITE);
		  eet_data_write(f_data->f, edd_version, "/version", &current_version, 0);
	       }
	  }
     }
   else
     {
	f_data = calloc(1, sizeof(File_Table_Data));
	f_data->path = eina_stringshare_add(file);

	_enlil_file_manager_clean();

	file_list = eina_list_append(file_list, f_data);

	f_data->f = eet_open(file, EET_FILE_MODE_READ_WRITE);
	if(!f_data->f)
	  {
	     f_data->f = eet_open(file, EET_FILE_MODE_WRITE);
	     eet_data_write(f_data->f, edd_version, "/version", &current_version, 0);
	  }
	else
	  {
	       f_version = eet_data_read(f_data->f, edd_version, "/version");
	       if(!f_version)
		  eet_data_write(f_data->f, edd_version, "/version", &current_version, 0);
	       else
		 FREE(f_version);
	  }
     }
   eina_stringshare_del(file);

   ASSERT(f_data->f != NULL);
   return f_data->f;
}

void enlil_file_manager_close(const char *file)
{
   //LOG_DBG("Close file %s", file);
   ENLIL_MUTEX_UNLOCK(mutex);
}

void enlil_file_manager_flush()
{
   Eina_List *l;
   File_Table_Data *data;
   ENLIL_MUTEX_LOCK(mutex);

   EINA_LIST_FOREACH(file_list, l, data)
     {
	if(data->f)
	  eet_close(data->f);
	data->f = NULL;
     }

   ENLIL_MUTEX_UNLOCK(mutex);
}

static void _data_free(File_Table_Data *data)
{
   if(data->f)
     eet_close(data->f);
   EINA_STRINGSHARE_DEL(data->path);
   FREE(data);
}

static Eet_Data_Descriptor * _edd_version_new()
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, EET_File_Version);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, EET_File_Version, "version", version, EET_T_DOUBLE);

   return edd;
}



