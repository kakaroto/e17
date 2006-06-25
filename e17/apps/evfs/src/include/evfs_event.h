#ifndef _EVFS_EVENT_H_
#define _EVFS_EVENT_H_

/*Event structures*/
typedef enum evfs_eventtype
{
   EVFS_EV_REPLY = 0,
   EVFS_EV_FILE_MONITOR = 1,
   EVFS_EV_NOTIFY_ID = 2,
   EVFS_EV_STAT = 3,
   EVFS_EV_DIR_LIST = 4,
   EVFS_EV_FILE_PROGRESS = 5,
   EVFS_EV_FILE_OPEN = 6,
   EVFS_EV_FILE_READ = 7,
   EVFS_EV_PONG = 8,
   EVFS_EV_OPERATION = 9,
   EVFS_EV_METADATA = 10,
   EVFS_EV_METADATA_FILE_GET = 11,

   EVFS_EV_ERROR = 100,
   EVFS_EV_NOT_SUPPORTED = 101
} evfs_eventtype;

typedef enum evfs_eventtype_sub
{
   EVFS_EV_SUB_MONITOR_NOTIFY = 1
} evfs_eventtype_sub;

typedef enum evfs_eventpart
{
   EVFS_EV_PART_TYPE = 1,
   EVFS_EV_PART_SUB_TYPE = 2,
   EVFS_EV_PART_FILE_MONITOR_TYPE = 3,
   EVFS_EV_PART_FILE_MONITOR_FILENAME = 4,
   EVFS_EV_PART_FILE_MONITOR_PLUGIN = 5,
   EVFS_EV_PART_DATA = 6,
   EVFS_EV_PART_STAT_SIZE = 7,
   EVFS_EV_PART_FILE_REFERENCE = 8,
   EVFS_EV_PART_METALIST = 9,

   EVFS_EV_PART_PROGRESS = 11,

   EVFS_COMMAND_EXTRA = 12,
   EVFS_COMMAND_TYPE = 13,
   EVFS_FILE_REFERENCE = 14,

   EVFS_EV_PART_OPERATION = 18,
   EVFS_EV_PART_FILE_MONITOR = 19,
   
   EVFS_COMMAND_END = 20,
   EVFS_COMMAND_PART_OPERATION = 21,
   EVFS_COMMAND_CLIENTID = 22,
   EVFS_COMMAND_PART_FILECOMMAND_REF1 = 23,
   EVFS_COMMAND_PART_FILECOMMAND_REF2 = 24,

   EVFS_EV_PART_END = 1000
} evfs_eventpart;

typedef enum evfs_file_monitor_type
{
   EVFS_FILE_EV_CREATE,
   EVFS_FILE_EV_CHANGE,
   EVFS_FILE_EV_REMOVE,
   EVFS_FILE_EV_REMOVE_DIRECTORY
} evfs_file_monitor_type;

/*-----------------------------------------------------------------*/
typedef struct evfs_stat
{
   int st_mode;
   int st_uid;
   int st_gid;
   uint64 st_size;
   int ist_atime;
   int ist_mtime;
   int ist_ctime;

} evfs_stat;

typedef struct evfs_meta_obj
{
	char* key;
	char* value;
} evfs_meta_obj;

typedef struct evfs_event_id_notify evfs_event_id_notify;
struct evfs_event_id_notify
{

   int id;
};

typedef struct evfs_event_file_monitor evfs_event_file_monitor;
struct evfs_event_file_monitor
{

   evfs_file_monitor_type fileev_type;
   char *plugin;
   char *filename;
   int filename_len;
   int filetype;
};

typedef struct evfs_event_stat evfs_event_stat;
struct evfs_event_stat
{
   evfs_stat stat_obj;
};

typedef struct evfs_event_file_list evfs_event_file_list;
struct evfs_event_file_list
{

   Ecore_List *list;            /*A list of evfs_filereference */
};

typedef enum evfs_progress_type
{
   EVFS_PROGRESS_TYPE_CONTINUE,
   EVFS_PROGRESS_TYPE_DONE
} evfs_progress_type;

typedef struct evfs_event_progress
{
   char *file_from;
   char *file_to;
   double file_progress;
   evfs_progress_type type;
} evfs_event_progress;

typedef struct evfs_event_data
{
   long size;
   char *bytes;
} evfs_event_data;

typedef struct evfs_event_meta 
{
   Evas_List* meta_list;
   Ecore_Hash* meta_hash;
} evfs_event_meta;

/*typedef struct evfs_event_operation {
	evfs_operation* op;
} evfs_event_operation;*/

//Would be good if this could be a union -> but evfs_command changes size :( */
typedef struct evfs_event
{
   evfs_eventtype type;
   evfs_command resp_command;

   evfs_event_file_list file_list;
   evfs_event_id_notify id_notify;
   evfs_event_file_monitor file_monitor;
   evfs_event_stat stat;
   evfs_event_progress *progress;
   struct evfs_operation *op;
  
   /*This is just a hack until we separate out 
    * the different evfs_event types into a struct
    * hierarchy */
   evfs_event_meta* meta;

   evfs_event_data data;
}
evfs_event;

/*---------------------------------------------------------------------*/

#endif
