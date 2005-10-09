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
#define MAX_CLIENT 9999999

#define EVFS_FUNCTION_MONITOR_START "evfs_monitor_start"
#define EVFS_FUNCTION_MONITOR_STOP "evfs_monitor_stop"
#define EVFS_FUNCTION_DIRECTORY_LIST "evfs_directory_list"
#define EVFS_FUNCTION_FILE_COPY "evfs_file_copy"
#define EVFS_FUNCTION_FILE_MOVE "evfs_file_move"
#define EVFS_FUNCTION_FILE_STAT_GET "evfs_file_stat_get"





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

	int num_clients;
};

typedef enum evfs_file_type evfs_file_type;
enum evfs_file_type {
	EVFS_FILE_NORMAL = 1,
	EVFS_FILE_DIRECTORY = 2
};


typedef struct evfs_filereference evfs_filereference;
struct evfs_filereference {
	char* plugin_uri;
	evfs_file_type file_type;
	char* path;
};

typedef struct evfs_file_uri_path evfs_file_uri_path;
struct evfs_file_uri_path {
	int num_files;
	evfs_filereference** files;
};

/*Command structures*/
typedef enum evfs_command_type
{
  EVFS_CMD_STARTMON_FILE = 1 ,
  EVFS_CMD_STOPMON_FILE = 2,
  EVFS_CMD_COPY_FILE = 3,
  EVFS_CMD_MOVE_FILE = 4,
  EVFS_CMD_RENAME_FILE = 5,
  EVFS_CMD_REMOVE_FILE=6,
  EVFS_CMD_LIST_DIR = 7,
  EVFS_CMD_FILE_STAT = 8
}
evfs_command_type;


typedef struct evfs_command_file {
	evfs_command_type type;
	int num_files;
	evfs_filereference** files;
}
evfs_command_file;

typedef union evfs_command {
	evfs_command_type type;
	evfs_command_file file_command;
}
evfs_command;

/*-----------*/


typedef struct evfs_client evfs_client;
struct evfs_client {
        Ecore_Ipc_Client* client;
        evfs_command* prog_command;
	unsigned long id;
};



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
} evfs_eventtype;

typedef enum evfs_eventtype_sub {
	EVFS_EV_SUB_MONITOR_NOTIFY = 1
} evfs_eventtype_sub;

typedef enum evfs_eventpart {
	EVFS_EV_PART_TYPE = 1,
	EVFS_EV_PART_SUB_TYPE = 2,
	EVFS_EV_PART_FILE_MONITOR_TYPE = 3,
	EVFS_EV_PART_FILE_MONITOR_FILENAME = 4,
	EVFS_EV_PART_DATA = 5,
	EVFS_EV_PART_STAT_SIZE = 6,
	EVFS_EV_PART_FILE_REFERENCE = 7,

        EVFS_COMMAND_TYPE = 8,
        EVFS_FILE_REFERENCE = 9,
        EVFS_COMMAND_END = 10,

	
	EVFS_EV_PART_END = 1000
} evfs_eventpart;

typedef enum evfs_file_monitor_type {
	EVFS_FILE_EV_CREATE,
	EVFS_FILE_EV_CHANGE,
	EVFS_FILE_EV_REMOVE
} evfs_file_monitor_type;


/*-----------------------------------------------------------------*/
typedef struct evfs_event_id_notify evfs_event_id_notify;
struct evfs_event_id_notify {

	int id;
};

typedef struct evfs_event_file_monitor evfs_event_file_monitor;
struct evfs_event_file_monitor {
	
	evfs_file_monitor_type fileev_type;
	char* filename;
	int filename_len;
};

typedef struct evfs_event_stat evfs_event_stat;
struct evfs_event_stat {

	
	struct stat stat_obj;
	
};

typedef struct evfs_event_file_list evfs_event_file_list;
struct evfs_event_file_list {

	Ecore_List* list; /*A list of evfs_filereference*/	
};

//Would be good if this could be a union -> but evfs_command changes size :( */
typedef struct evfs_event {
	evfs_eventtype type;
	evfs_command resp_command;
	 
	evfs_event_file_list file_list;
	evfs_event_id_notify id_notify;
	evfs_event_file_monitor file_monitor;
	evfs_event_stat stat;
	

	
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
evfs_connection* evfs_connect(void (*callback_func)(void*));
evfs_file_uri_path* evfs_parse_uri(char* uri);
void evfs_handle_command(evfs_client* client, evfs_command* command);
void evfs_handle_monitor_start_command(evfs_client* client, evfs_command* command);

unsigned long evfs_server_get_next_id(evfs_server* serve);



#include <evfs_commands.h>
#include <evfs_cleanup.h>
#include <evfs_io.h>
#include <evfs_new.h>
#include <evfs_event_helper.h>
#include <evfs_server_handle.h>
#include "evfs_plugin.h"

evfs_plugin* evfs_get_plugin_for_uri(char* uri_base);

#endif
