#ifndef __EVFS_H_
#define __EVFS_H_

#define _GNU_SOURCE
#include "evfs_macros.h"
#include "evfs_debug.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include <Ecore.h>
#include <Ecore_Ipc.h>
#include <Ecore_File.h>
#include <pthread.h>

#include <stdlib.h>
#include <string.h>


#define EVFS_IPC_TITLE "evfs_fs"
#define MAXPATHLEN 512
#define FALSE 0
#define TRUE 1
#define URI_SEP "#"
#define EVFS_META_DIR_NAME  ".e_meta"
#define COPY_BLOCKSIZE 4096
#define MAX_CLIENT 9999999

#define EVFS_FUNCTION_MONITOR_START "evfs_monitor_start"
#define EVFS_FUNCTION_MONITOR_STOP "evfs_monitor_stop"
#define EVFS_FUNCTION_DIRECTORY_LIST "evfs_directory_list"
#define EVFS_FUNCTION_FILE_COPY "evfs_file_copy"
#define EVFS_FUNCTION_FILE_MOVE "evfs_file_move"
#define EVFS_FUNCTION_FILE_STAT_GET "evfs_file_stat_get"


#include "evfs_plugin.h"


typedef enum
{
  EVFS_FS_OP_FORCE      = 1,
  EVFS_FS_OP_RECURSIVE  = 2
}
EfsdFsOps;



typedef struct evfs_server evfs_server;
struct evfs_server {
	Ecore_Hash* client_hash;
	Ecore_Hash* plugin_uri_hash;
	Ecore_Ipc_Server* ipc_server;
	unsigned long clientCounter;

	Ecore_List* incoming_command_list;

	int num_clients;
};

typedef struct evfs_command_client {
	evfs_client* client;
	evfs_command* command;
} evfs_command_client;











/*-----------*/

/*This structure needs more development*/
typedef struct evfs_auth_cache {
	char* path;
	char* username;
	char* password;
} evfs_auth_cache;




typedef struct evfs_file_monitor evfs_file_monitor;
struct evfs_file_monitor {
	evfs_client* client;
	char* monitor_path;

	Ecore_File_Monitor *em;
};


/*Event structures*/
typedef enum evfs_eventtype {
	EVFS_EV_REPLY = 0,
	EVFS_EV_FILE_MONITOR = 1,	
	EVFS_EV_NOTIFY_ID = 2,
	EVFS_EV_STAT = 3,
	EVFS_EV_DIR_LIST = 4,
	EVFS_EV_FILE_PROGRESS = 5,
	EVFS_EV_FILE_OPEN= 6,
	EVFS_EV_FILE_READ = 7
} evfs_eventtype;

typedef enum evfs_eventtype_sub {
	EVFS_EV_SUB_MONITOR_NOTIFY = 1
} evfs_eventtype_sub;

typedef enum evfs_eventpart {
	EVFS_EV_PART_TYPE = 1,
	EVFS_EV_PART_SUB_TYPE = 2,
	EVFS_EV_PART_FILE_MONITOR_TYPE = 3,
	EVFS_EV_PART_FILE_MONITOR_FILENAME = 4,
	EVFS_EV_PART_FILE_MONITOR_PLUGIN = 5,
	EVFS_EV_PART_DATA = 6,
	EVFS_EV_PART_STAT_SIZE = 7,
	EVFS_EV_PART_FILE_REFERENCE = 8,
	EVFS_EV_PART_FILE_REFERENCE_USERNAME = 9,
	EVFS_EV_PART_FILE_REFERENCE_PASSWORD = 10,

	EVFS_EV_PART_PROGRESS = 11,

	EVFS_COMMAND_EXTRA = 12,
        EVFS_COMMAND_TYPE = 13,
        EVFS_FILE_REFERENCE = 14,
	EVFS_FILE_REFERENCE_PASSWORD = 15,
	EVFS_FILE_REFERENCE_USERNAME = 16,
	EVFS_FILE_REFERENCE_FD = 17,
        EVFS_COMMAND_END = 18,

	
	EVFS_EV_PART_END = 1000
} evfs_eventpart;

typedef enum evfs_file_monitor_type {
	EVFS_FILE_EV_CREATE,
	EVFS_FILE_EV_CHANGE,
	EVFS_FILE_EV_REMOVE,
	EVFS_FILE_EV_REMOVE_DIRECTORY
} evfs_file_monitor_type;


/*-----------------------------------------------------------------*/
typedef struct evfs_stat {
	int st_uid;
	int st_gid;
	int st_size;
	int ist_atime;
	int ist_mtime;
	int ist_ctime;
	
} evfs_stat;

typedef struct evfs_event_id_notify evfs_event_id_notify;
struct evfs_event_id_notify {

	int id;
};

typedef struct evfs_event_file_monitor evfs_event_file_monitor;
struct evfs_event_file_monitor {
	
	evfs_file_monitor_type fileev_type;
	char* plugin;
	char* filename;
	int filename_len;
};

typedef struct evfs_event_stat evfs_event_stat;
struct evfs_event_stat {
	evfs_stat stat_obj;
};

typedef struct evfs_event_file_list evfs_event_file_list;
struct evfs_event_file_list {

	Ecore_List* list; /*A list of evfs_filereference*/	
};

typedef enum evfs_progress_type {
	EVFS_PROGRESS_TYPE_CONTINUE,
	EVFS_PROGRESS_TYPE_DONE
} evfs_progress_type;

typedef struct evfs_event_progress {
	char* file_from;
	char* file_to;
	double file_progress;
	evfs_progress_type type;
} evfs_event_progress;

typedef struct evfs_event_data {
	long size;
	char* bytes;
} evfs_event_data;

//Would be good if this could be a union -> but evfs_command changes size :( */
typedef struct evfs_event {
	evfs_eventtype type;
	evfs_command resp_command;

	evfs_event_file_list file_list;
	evfs_event_id_notify id_notify;
	evfs_event_file_monitor file_monitor;
	evfs_event_stat stat;	
	evfs_event_progress* progress;

	evfs_event_data data;
}
evfs_event;
/*---------------------------------------------------------------------*/

typedef struct evfs_connection evfs_connection;
struct evfs_connection {
	Ecore_Ipc_Server* server;
	unsigned long id;
	void (*callback_func)(evfs_event* data);
	evfs_event* prog_event;
};


void evfs_cleanup_client(evfs_client* client);
void evfs_disconnect(evfs_connection* connection);
evfs_connection* evfs_connect(void (*callback_func)(evfs_event*));
evfs_file_uri_path* evfs_parse_uri(char* uri);
void evfs_handle_command(evfs_client* client, evfs_command* command);
void evfs_handle_monitor_start_command(evfs_client* client, evfs_command* command);

unsigned long evfs_server_get_next_id(evfs_server* serve);

char* evfs_filereference_to_string(evfs_filereference* ref);



#include "evfs_commands.h"
#include "evfs_cleanup.h"
#include "evfs_io.h"
#include "evfs_new.h"
#include "evfs_event_helper.h"
#include "evfs_server_handle.h"
#include "evfs_common.h"
#include "evfs_misc.h"

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif




#endif
