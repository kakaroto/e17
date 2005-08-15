#ifndef __EVFS_H_
#define __EVFS_H_

#define _GNU_SOURCE
#include "evfs_macros.h"
#include "evfs_debug.h"
#include "evfs_plugin.h"
#include <Ecore.h>
#include <Ecore_Ipc.h>
#include <pthread.h>


#define EVFS_IPC_TITLE "evfs_fs"
#define MAXPATHLEN 512
#define FALSE 0
#define TRUE 1
#define URI_SEP "#"
#define EVFS_META_DIR_NAME  ".e_meta"

#define EVFS_MONITOR_START "evfs_monitor_start"

typedef enum
{
  EVFS_FS_OP_FORCE      = 1,
  EVFS_FS_OP_RECURSIVE  = 2
}
EfsdFsOps;



typedef struct evfs_connection evfs_connection;
struct evfs_connection {
	Ecore_Ipc_Server* server;
};


typedef struct evfs_server evfs_server;
struct evfs_server {
	Ecore_Hash* client_hash;
	Ecore_Hash* plugin_uri_hash;
	Ecore_Ipc_Server* ipc_server;

	int num_clients;
};

typedef struct evfs_filereference evfs_filereference;
struct evfs_filereference {
	char* plugin_uri;
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
  EVFS_CMD_STARTMON_FILE = 1 
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
        void (*callback_func)(void* data);
};


void evfs_cleanup_client(evfs_client* client);
void evfs_disconnect(evfs_connection* connection);
evfs_connection* evfs_connect();
evfs_file_uri_path* evfs_parse_uri(char* uri);
void evfs_handle_command(evfs_client* client, evfs_command* command);
void evfs_handle_monitor_start_command(evfs_client* client, evfs_command* command);
evfs_plugin* evfs_get_plugin_for_uri(char* uri_base);



#include <evfs_commands.h>
#include <evfs_cleanup.h>
#include <evfs_io.h>
#include <evfs_new.h>

#endif
