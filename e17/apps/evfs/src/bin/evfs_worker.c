
/*

Copyright (C) 2005-2007 Alexander Taylor <alex@logisticchaos.com>.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "evfs.h"

#include <Ecore_File.h>
#include <Efreet.h>
#include <Efreet_Mime.h>

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <dirent.h>
#include <sqlite3.h>
#include "evfs_metadata_db.h"

static evfs_client* worker_client;
static evfs_server* server;
static sqlite3* worker_db = NULL;
Ecore_Event_Handler* server_data;
Ecore_Ipc_Server* iserver;


int ecore_timer_enterer(__UNUSED__ void *data);
int incoming_command_cb(__UNUSED__ void *data);
int evfs_handle_command(evfs_client * client, evfs_command * command);
void evfs_load_plugins();


sqlite3* evfs_worker_db_get() 
{
	return worker_db;
}

int ecore_timer_enterer(__UNUSED__ void *data)
{
   incoming_command_cb(NULL);
   evfs_operation_queue_run();
	
   return 1;
}

int evfs_handle_command(evfs_client * client, evfs_command * command)
{
   int cleanup_command=1;

   /*printf("Handling data for client with ID: %d\n", client->id);*/
	
   switch (command->type)
     {
     case EVFS_CMD_STARTMON_FILE:
        /*printf("We received a monitor stop request */
        evfs_handle_monitor_start_command(client, command);

        break;
     case EVFS_CMD_STOPMON_FILE:
        /*printf("We received a monitor stop request\n"); */
        evfs_handle_monitor_stop_command(client, command);
        break;

     case EVFS_CMD_RENAME_FILE:
        evfs_handle_file_rename_command(client, command);
        break;

     case EVFS_CMD_MOVE_FILE:
        printf("Move file stub\n");
        break;
     case EVFS_CMD_REMOVE_FILE:
        evfs_handle_file_remove_command(client, command, command);
	cleanup_command=0;
        break;
     case EVFS_CMD_FILE_STAT:
        evfs_handle_file_stat_command(client, command);
        break;
     case EVFS_CMD_LIST_DIR: 
        evfs_handle_dir_list_command(client, command);
        break;
     case EVFS_CMD_FILE_OPEN:
        evfs_handle_file_open_command(client, command);
        break;
     case EVFS_CMD_FILE_READ:
        evfs_handle_file_read_command(client, command);
        break;
     case EVFS_CMD_FILE_COPY:
        evfs_handle_file_copy(client, command, command,0);
	cleanup_command = 0;
        break;
     case EVFS_CMD_FILE_MOVE:
        evfs_handle_file_copy(client, command, command,1);
	cleanup_command = 0;
        break;

     case EVFS_CMD_DIRECTORY_CREATE:
	evfs_handle_directory_create_command(client,command);
	break;
     case EVFS_CMD_PING:
        evfs_handle_ping_command(client, command);
        break;

     case EVFS_CMD_OPERATION_RESPONSE:
        evfs_handle_operation_command(client, command);
        break;

     case EVFS_CMD_METADATA_RETRIEVE:
	evfs_handle_metadata_command(client,command);
	break;

     case EVFS_CMD_METADATA_FILE_SET:
	printf("Key/value: %s -> %s\n", command->file_command->ref, command->file_command->ref2);
	evfs_handle_metadata_string_file_set_command(client,command, 
			command->file_command->ref, command->file_command->ref2);
	break;
     case EVFS_CMD_METADATA_FILE_GET:
	printf("Requested metadata retrieval.. key:%s\n", command->file_command->ref);
	evfs_handle_metadata_string_file_get_command(client,command, 
			command->file_command->ref);	
	break;

     case EVFS_CMD_METADATA_GROUPS_GET:
	printf("Request for metadata groups\n");
	evfs_handle_metadata_groups_request_command(client,command);
	break;

     case EVFS_CMD_METADATA_FILE_GROUP_ADD:
	evfs_handle_metadata_file_group_add(client,command);
	break;
     case EVFS_CMD_METADATA_FILE_GROUP_REMOVE:
        evfs_handle_metadata_file_group_remove(client,command);
	break;

     case EVFS_CMD_TRASH_RESTORE:
	evfs_handle_trash_restore_command(client,command);
	break;

     case EVFS_CMD_AUTH_RESPONSE:
	printf("Got auth response command..\n");
	evfs_handle_auth_respond_command(client,command);
	break;

     case EVFS_CMD_MIME_REQUEST:
	evfs_handle_mime_request(client,command);
	break;

     case EVFS_CMD_VFOLDER_CREATE:
	evfs_handle_vfolder_create(client,command);
	break;

     case EVFS_CMD_META_ALL_REQUEST:
     	evfs_handle_meta_all_request(client,command);
	break;

     default:
        printf("Warning - unhandled command %d\n", command->type);
        break;
     }

   return cleanup_command;
}


int incoming_command_cb(__UNUSED__ void *data)
{
   int clean =0;

   evfs_command_client *com_cli =
      ecore_list_first_remove(server->incoming_command_list);

   if (com_cli)
     {
        clean = evfs_handle_command(com_cli->client, com_cli->command);
        if (clean) evfs_cleanup_command(com_cli->command, EVFS_CLEANUP_FREE_COMMAND);
        free(com_cli);
     }

   return 1;
}

/*Handler for messages server->worker*/
int
ipc_server_data(void *data __UNUSED__, int type __UNUSED__, void *event)
{
	Ecore_Ipc_Event_Server_Data *e = (Ecore_Ipc_Event_Server_Data *) event;

	/*Ignore messages from master server*/
	//if (e->server == server->ipc_server) return 1;
	//
	if (e->major == EVFS_MESSAGE_KILL) {
	   printf("Our parent client has disconnected, suicide time\n");
	   ecore_main_loop_quit();
	} else {	
	   evfs_command* command;
	   /*printf("Got server data in fork!: PID: %d\n", getpid());*/

	   ecore_ipc_message *msg =
	      ecore_ipc_message_new(e->major, e->minor, e->ref, e->ref_to, e->response,
                            e->data, e->size);

	   /*True == command finished */
	   if ((command = evfs_process_incoming_command(evfs_server_get(), msg)))
	     {
	        evfs_command_client *com_cli = NEW(evfs_command_client);
	
	        com_cli->client = worker_client;
	        com_cli->command = command;
	        
		ecore_list_append(server->incoming_command_list, com_cli);
		/*printf("Finished processing command in fork\n");*/
	     }
	
	   free(msg);

	}

	return 1;
}


/* TODO these plugin loader functions should be 
 * consolidated for common functionality*/

evfs_plugin *
evfs_load_plugin_file(char *filename)
{
   evfs_plugin_file *plugin = NEW(evfs_plugin_file);

   char *(*evfs_plugin_uri_get) ();
   evfs_plugin_functions *(*evfs_plugin_init) ();

   printf("Loading plugin: %s\n", filename);
   EVFS_PLUGIN(plugin)->dl_ref = dlopen(filename, RTLD_LAZY);

   if (EVFS_PLUGIN(plugin)->dl_ref)
     {
        evfs_plugin_uri_get = dlsym(EVFS_PLUGIN(plugin)->dl_ref, "evfs_plugin_uri_get");
        if (evfs_plugin_uri_get)
          {
             plugin->uri = (*evfs_plugin_uri_get) ();
             printf("The plugin at '%s' handles '%s'\n", filename, plugin->uri);

             /*Execute the init function, if it's there.. */
             evfs_plugin_init = dlsym(EVFS_PLUGIN(plugin)->dl_ref, "evfs_plugin_init");
             if (evfs_plugin_init)
               {
                  plugin->functions = (*evfs_plugin_init) ();
               }
          }
        else
          {
             printf
                ("Error - plugin file does not contain uri identify function - %s\n",
                 filename);
             goto exit_error;
          }

     }
   else
     {
        printf("Error - plugin file invalid - %s\n", filename);
        goto exit_error;
     }

   return EVFS_PLUGIN(plugin);

 exit_error:
   free(plugin);
   return NULL;

}

evfs_plugin *
evfs_load_plugin_meta(char *filename)
{
   evfs_plugin_meta *plugin = NEW(evfs_plugin_meta);
   evfs_plugin_functions_meta *(*evfs_plugin_init) ();
   Ecore_List* (*evfs_plugin_meta_types_get)();
   char* type;
   Ecore_List* types;


   printf("Loading plugin: %s\n", filename);
   EVFS_PLUGIN(plugin)->dl_ref = dlopen(filename, RTLD_LAZY);

   if (EVFS_PLUGIN(plugin)->dl_ref)
     {
             /*Execute the init function, if it's there.. */
             evfs_plugin_init = dlsym(EVFS_PLUGIN(plugin)->dl_ref, "evfs_plugin_init");
             if (evfs_plugin_init)
               {
                  plugin->functions = (*evfs_plugin_init) ();

		  /*Load meta types*/
		  evfs_plugin_meta_types_get = dlsym(EVFS_PLUGIN(plugin)->dl_ref, "evfs_plugin_meta_types_get");
		  if (evfs_plugin_meta_types_get) {
			  
			  types = (*evfs_plugin_meta_types_get)();
			  /*Register..*/
			 while ( (type = ecore_list_first_remove(types))) {
				 ecore_hash_set(evfs_server_get()->plugin_meta_hash, type, plugin);
				 printf("  Registered meta plugin for '%s'...\n", type);
			 }
		  } else {
			  printf("Error - could not get type register function for meta plugin");
		  }
               } else {
	             printf
                ("Error - plugin file does not contain init function - %s\n",
                 filename);
       	      goto exit_error;
             }

     }
   else
     {
        printf("Error - plugin file invalid - %s\n", filename);
        goto exit_error;
     }

   return EVFS_PLUGIN(plugin);

 exit_error:
   free(plugin);
   return NULL;

}


evfs_plugin *
evfs_load_plugin_vfolder(char *filename)
{
   evfs_plugin_vfolder *plugin = NEW(evfs_plugin_vfolder);
   evfs_plugin_functions_vfolder *(*evfs_plugin_init) ();
   char* (*evfs_plugin_vfolder_root_get)();

   printf("Loading plugin: %s\n", filename);
   EVFS_PLUGIN(plugin)->dl_ref = dlopen(filename, RTLD_LAZY);

   if (EVFS_PLUGIN(plugin)->dl_ref)
     {
             /*Execute the init function, if it's there.. */
             evfs_plugin_init = dlsym(EVFS_PLUGIN(plugin)->dl_ref, "evfs_plugin_init");
             if (evfs_plugin_init)
               {
                  plugin->functions = (*evfs_plugin_init) ();

		  /*Load meta types*/
		  evfs_plugin_vfolder_root_get = dlsym(EVFS_PLUGIN(plugin)->dl_ref, "evfs_plugin_vfolder_root_get");
		  if (evfs_plugin_vfolder_root_get) {
			 char* root = (*evfs_plugin_vfolder_root_get)();
			 ecore_hash_set(evfs_server_get()->plugin_vfolder_hash, root, plugin);
		  } else {
			  printf("Error - could not get root register function for vfolder plugin");
		  }
               } else {
	             printf
                ("Error - plugin file does not contain init function - %s\n",
                 filename);
       	      goto exit_error;
             }

     }
   else
     {
        printf("Error - plugin file invalid - %s\n", filename);
        goto exit_error;
     }

   return EVFS_PLUGIN(plugin);

 exit_error:
   free(plugin);
   return NULL;

}


void evfs_load_plugins()
{
   struct dirent *de;
   DIR *dir;
   evfs_plugin *plugin;
   char plugin_path[1024];

   printf("Reading plugins from: %s\n", PACKAGE_PLUGIN_DIR "/plugins/file");
   dir = opendir(PACKAGE_PLUGIN_DIR "/plugins/file");
   if (dir)
     {
        while ((de = readdir(dir)))
          {
             if (!strncmp(de->d_name + strlen(de->d_name) - 3, ".so", 3))
               {
                  snprintf(plugin_path, 1024, "%s/%s",
                           PACKAGE_PLUGIN_DIR "/plugins/file", de->d_name);
                  if ((plugin = evfs_load_plugin_file(plugin_path)))
                    {
                       ecore_hash_set(server->plugin_uri_hash, EVFS_PLUGIN_FILE(plugin)->uri,
                                      plugin);
                    }
               }
          }
     }
   else
     {
        fprintf(stderr, "EVFS: Could not location plugin directory '%s'\n",
                PACKAGE_PLUGIN_DIR "/plugins/file");
        exit(1);
     }
   closedir(dir);

   /*Load meta plugins*/
   printf("Reading plugins from: %s\n", PACKAGE_PLUGIN_DIR "/plugins/meta");
   dir = opendir(PACKAGE_PLUGIN_DIR "/plugins/meta");
   if (dir)
     {
        while ((de = readdir(dir)))
          {
             if (!strncmp(de->d_name + strlen(de->d_name) - 3, ".so", 3))
               {
                  snprintf(plugin_path, 1024, "%s/%s",
                           PACKAGE_PLUGIN_DIR "/plugins/meta", de->d_name);
                  if ((plugin = evfs_load_plugin_meta(plugin_path)))
                    {
                    }
               }
          }
     }
   else
     {
        fprintf(stderr, "EVFS: Could not location plugin directory '%s'\n",
                PACKAGE_PLUGIN_DIR "/plugins/meta");
        exit(1);
     }
   closedir(dir);

   /*Load vfolder plugins*/
   printf("Reading plugins from: %s\n", PACKAGE_PLUGIN_DIR "/plugins/vfolder");
   dir = opendir(PACKAGE_PLUGIN_DIR "/plugins/vfolder");
   if (dir)
     {
        while ((de = readdir(dir)))
          {
             if (!strncmp(de->d_name + strlen(de->d_name) - 3, ".so", 3))
               {
                  snprintf(plugin_path, 1024, "%s/%s",
                           PACKAGE_PLUGIN_DIR "/plugins/vfolder", de->d_name);
                  if ((plugin = evfs_load_plugin_vfolder(plugin_path)))
                    {
                    }
               }
          }
     }
   else
     {
        fprintf(stderr, "EVFS: Could not location plugin directory '%s'\n",
                PACKAGE_PLUGIN_DIR "/plugins/vfolder");
        exit(1);
     }
   closedir(dir);
}


int
main(int argc, char **argv)
{
   char* search;

   /*Init the ipc server */
   if (ecore_ipc_init() < 1)
      return (1);

   ecore_file_init();
   if (!efreet_mime_init())
   {
       printf("Could not init efreet\n");
       return 1;
   }

   server = evfs_server_new();
   server->client_hash =
      ecore_hash_new(ecore_direct_hash, ecore_direct_compare);

   server->worker_hash =
      ecore_hash_new(ecore_direct_hash, ecore_direct_compare);

   server->plugin_uri_hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);
   server->plugin_meta_hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);
   server->plugin_vfolder_hash = ecore_hash_new(ecore_str_hash, ecore_str_compare); 

   server->incoming_command_list = ecore_list_new();

   evfs_object_server_is_set();

   evfs_load_plugins();
   evfs_io_initialise();
   evfs_vfolder_initialise();
   evfs_trash_initialise();
   evfs_operation_initialise();
   evfs_metadata_initialise_worker();
   worker_db = evfs_metadata_db_connect();

   /*Add a timer, to make sure our event loop keeps going.  Kinda hacky */
   server->tmr = ecore_timer_add(0.01, ecore_timer_enterer, NULL);

   worker_client = NEW(evfs_client);
   worker_client->server = server;

   if (argc > 0) {
	   if (strstr(argv[0] , "worker")) {
		   worker_client->id = atoi(argv[1]);
	   } else {		   
		   worker_client->id = atoi(argv[0]);
	   }
   }

   server_data = ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DATA, ipc_server_data,
                                NULL);
   worker_client->master = ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER, EVFS_WOR_TITLE, 0,
                                 NULL);

   /*We're going to be sending *quite* a lot of data*/
   ecore_ipc_server_data_size_max_set(worker_client->master,-1);

    printf("Connected to %p\n", worker_client->master);
   ecore_main_loop_begin();

   return 0;
}
