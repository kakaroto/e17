
/*

Copyright (C) 2005-2006 Alexander Taylor <alex@logisticchaos.com>.

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
#include <ctype.h>

#define EVFS_TIMER_INTERVAL 0.01

static evfs_client* client_worker_waiter = NULL;

/*An object used for testing, so we can GDB the worker*/
static Ecore_Ipc_Client* worker_client_waiter = NULL;

static evfs_server* server;
static Ecore_Event_Handler *client_add = NULL;
static Ecore_Event_Handler *client_del = NULL;
static Ecore_Event_Handler *client_data = NULL;

static Ecore_Event_Handler *worker_add = NULL;
static Ecore_Event_Handler *worker_del = NULL;
static Ecore_Event_Handler *worker_data = NULL;

int ipc_server_data(void *data __UNUSED__, int type __UNUSED__, void *event);
int ecore_timer_enterer(__UNUSED__ void *data);
void evfs_worker_initialise();
void evfs_load_plugins();


evfs_client *
evfs_client_get(Ecore_Ipc_Client * client)
{
   return ecore_hash_get(server->client_hash, client);
}

unsigned long
evfs_server_get_next_id(evfs_server * serve)
{
   serve->clientCounter++;
   printf("Allocated %ld\n", serve->clientCounter - 1);
   return serve->clientCounter - 1;
}

int evfs_server_worker_spawn(int id) 
{
   const char *server_exe = BINDIR "/evfsworker";
   char strid[20];

   snprintf(strid, 20, "%d", id);
   printf("Creating new worker, client ID: %d\n",id);

   if (!access(server_exe, X_OK | R_OK)) {
        setsid();
        if (fork() == 0) {
             execl(server_exe, strid, NULL);
        }
        return 1;
     } else {
        fprintf(stderr, "You don't have rights to execute the evfs worker\n");
        return 1;
     }

   return 0;
}


void evfs_worker_initialise()
{
   /*Load the plugins */ 
   evfs_load_plugins();
   evfs_operation_initialise();
}


/* Server -> Client IPC*/
int
ipc_client_add(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Ipc_Event_Client_Add *e;
   evfs_client *client;

   e = (Ecore_Ipc_Event_Client_Add *) event;
   /*printf("ERR: EVFS Client Connected!!!\n"); */

   /*Make sure we're not the worker server's event*/
   if (ecore_ipc_client_server_get(e->client) != server->ipc_server) return 1;

   ecore_ipc_client_data_size_max_set(e->client,-1);

   client = NEW(evfs_client);
   client->client = e->client;
   client->server = server;
   client->prog_command = NULL;
   client->id = evfs_server_get_next_id(server);
   ecore_hash_set(server->client_hash, client->client, client);
   server->num_clients++;


   printf("Creating new worker..\n");
   
   /*Save a reference to this client, so we can allocate the worker child to
    * it when it calls back*/
   
   if (worker_client_waiter) {	   
	   client->worker_client = worker_client_waiter;
	   ecore_hash_set(evfs_server_get()->worker_hash, worker_client_waiter, client);
	   worker_client_waiter = NULL;

	   evfs_event_client_id_notify(client);
   } else {
	   if (client_worker_waiter) {
		   printf("EVFS: Worker failed to connect for previous client - Abort\n");
	   } else {
		   client_worker_waiter = client;

		   /*Spawn a worker*/
		   evfs_server_worker_spawn(client->id);
	   }
   }
   
   /*ecore_ipc_server_send(client->master, EVFS_MESSAGE_CLIENTID,0,0,0,0,(void*)client,sizeof(int));*/

   /*Tell our child that we've connected*/
   return (1);
}

int
ipc_client_del(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Ipc_Event_Client_Del *e;
   Ecore_List *keys;
   evfs_client *client;
   evfs_plugin *plugin;
   char *key;

   e = (Ecore_Ipc_Event_Client_Del *) event;

   /*Make sure we're not the worker server's event*/
   if (ecore_ipc_client_server_get(e->client) != server->ipc_server) return 1;


   client = ecore_hash_get(server->client_hash, e->client);
   printf("Client %ld, Client Disconnected!!!\n", client->id);

   /*Notify the plugins that this client has disconnected */
   keys = ecore_hash_keys(server->plugin_uri_hash);
   ecore_list_first_goto(keys);
   while ((key = ecore_list_first_remove(keys)))
     {
        plugin = ecore_hash_get(server->plugin_uri_hash, key);
        (*EVFS_PLUGIN_FILE(plugin)->functions->evfs_client_disconnect) (client);
     }

   /*Kill the child pid*/
   if (client->worker_client) {
	printf("Sending client %p the kill signal\n", client->worker_client);
	ecore_ipc_client_send(client->worker_client, EVFS_MESSAGE_KILL,0,0,0,0,NULL,0);
   }

   ecore_list_destroy(keys);
   ecore_ipc_client_del(client->client);
   ecore_hash_remove(server->client_hash, client);
   evfs_cleanup_client(client);

   return (1);
}

int
ipc_client_data(void *data __UNUSED__, int type __UNUSED__, void *event)
{

   Ecore_Ipc_Event_Client_Data *e = (Ecore_Ipc_Event_Client_Data *) event;
   evfs_client *client;

   /*Make sure we're not the worker server's event*/
   if (ecore_ipc_client_server_get(e->client) != server->ipc_server) {
	   /*printf("CLIENT DATA: Not a message for us...server %p != client server %p: Type: %d\n", server->ipc_server,ecore_ipc_client_server_get(e->client),e->major );*/
	   return 1;
   }

   client = evfs_client_get(e->client);

   /*Onsend to client's worker, if any*/
   if (client->worker_client) {
	   /*printf("Onsending data to client %p..%d %d %d %d %d\n", client->worker_client,e->major,e->minor,e->ref,e->ref_to,e->response,e->data, e->size );*/
	   
	   ecore_ipc_client_send(client->worker_client,e->major,e->minor,e->ref,e->ref_to,e->response,e->data, e->size); 
   } else {
	   printf("No worker client to send to at ipc_client_data\n");
   }

   return 1;
}
/*-----------*/

/*Server -> Worker IPC*/
int
ipc_worker_add(void *data __UNUSED__, int type __UNUSED__, void *event)
{
	Ecore_Ipc_Event_Client_Data *e = (Ecore_Ipc_Event_Client_Data *) event;

	/*Make sure we're not the daemon server's event*/
	if (ecore_ipc_client_server_get(e->client) != server->worker_server) return 1;

	/*We're going to be sending *quite* a lot of data*/
	ecore_ipc_client_data_size_max_set(e->client,-1);

	printf("New worker client to server..%p\n", server->worker_server);
		
	if (client_worker_waiter) {
		printf("Client %p waiting for worker..\n", client_worker_waiter);
		
		client_worker_waiter->worker_client = e->client;
		ecore_hash_set(evfs_server_get()->worker_hash, e->client, client_worker_waiter);

		evfs_event_client_id_notify(client_worker_waiter);	
		client_worker_waiter = NULL;
	} else {
		printf("Added worker to holding queue..\n");
		worker_client_waiter = e->client;
	}

	return 1;
}

int
ipc_worker_del(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Ipc_Event_Client_Del *e;
   
   e = (Ecore_Ipc_Event_Client_Del *) event;

   /*Make sure we're not the daemon server's event*/
   if (ecore_ipc_client_server_get(e->client) != server->worker_server) return 1;

   printf("Worker disconnect..\n");

   return 1;
}

int
ipc_worker_data(void *data __UNUSED__, int type __UNUSED__, void *event)
{

   Ecore_Ipc_Event_Client_Data *e = (Ecore_Ipc_Event_Client_Data *) event;
   evfs_client *client;

   /*Make sure we're not the daemon server's event*/
   if (ecore_ipc_client_server_get(e->client) != server->worker_server) {
	   /*printf("Not a message for us...server %p != client server %p: Type: %d\n", server->worker_server,ecore_ipc_client_server_get(e->client),e->major );*/
	   return 1;
   }

   /*printf("Sending data to client.. %d %d %d %d %d, len:%d\n", e->major, e->minor, e->ref, e->ref_to, e->response, e->size);*/

   client = ecore_hash_get(evfs_server_get()->worker_hash, e->client);
   if (client) {
	   /*printf("Got worker message, sending to client: %d, %d, %d, %d\n",e->major,e->minor,e->ref,e->ref_to);*/
	   ecore_ipc_client_send(client->client, e->major,e->minor,client->id,e->ref_to,e->response,e->data,e->size);
   } else {
		   printf("Cannot find client at ipc_worker_data\n");
   }

   return 1;
}
/*------------------*/


int
evfs_handle_command(evfs_client * client, evfs_command * command)
{
   int cleanup_command=1;
	
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

     default:
        printf("Warning - unhandled command %d\n", command->type);
        break;
     }

   return cleanup_command;
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


void
evfs_load_plugins()
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
/*-------------------*/

int
incoming_command_cb(__UNUSED__ void *data)
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

int
ecore_timer_enterer(__UNUSED__ void *data)
{
   incoming_command_cb(NULL);
   evfs_operation_queue_run();
	
   return 1;
}

int
ecore_timer_enterer_server(__UNUSED__ void *data)
{
   return 1;
}

int
main(int argc, char **argv)
{
   /*Init the ipc server */
   if (ecore_ipc_init() < 1)
      return (1);

   ecore_file_init();

   server = evfs_server_new();
   server->client_hash =
      ecore_hash_new(ecore_direct_hash, ecore_direct_compare);

   server->worker_hash =
      ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
   
   server->plugin_uri_hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);
   server->plugin_meta_hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);
   server->plugin_vfolder_hash = ecore_hash_new(ecore_str_hash, ecore_str_compare); 
   
   server->clientCounter = 1000;
   server->incoming_command_list = ecore_list_new();

   //ecore_idle_enterer_add(incoming_command_cb, NULL);

   /*Identify that we are a server*/
   evfs_object_server_is_set();

   /*Add a timer, to make sure our event loop keeps going.  Kinda hacky */
   server->tmr = ecore_timer_add(1, ecore_timer_enterer_server, NULL);

   if ((server->ipc_server =
        ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER, EVFS_IPC_TITLE, 0,
                                 NULL)))
     {
        ecore_ipc_server_del(server->ipc_server);
        free(server);
        printf("ERR: Server already running...\n");
        return (1);
     }
   else
     {
        //printf ("ERR: Server created..\n");

        server->ipc_server =
           ecore_ipc_server_add(ECORE_IPC_LOCAL_USER, EVFS_IPC_TITLE, 0, NULL);

	/*We're going to be sending *quite* a lot of data*/
	ecore_ipc_server_data_size_max_set(server->ipc_server,-1);

        client_add = ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_ADD, ipc_client_add,
                                NULL);
        client_del = ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DEL, ipc_client_del,
                                NULL);
        client_data = ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DATA, ipc_client_data,
                                NULL);

	printf("Started IPC server %p for ipc_server\n", server->ipc_server);

	server->worker_server = 
	    ecore_ipc_server_add(ECORE_IPC_LOCAL_USER, EVFS_WOR_TITLE, 0, NULL);
	/*We're going to be sending *quite* a lot of data*/
	ecore_ipc_server_data_size_max_set(server->worker_server,-1);

        worker_add = ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_ADD, ipc_worker_add,
                                NULL);
        worker_del = ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DEL, ipc_worker_del,
                                NULL);
        worker_data = ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DATA, ipc_worker_data,
                                NULL);

		
     }

   evfs_io_initialise();
   evfs_vfolder_initialise();
   evfs_trash_initialise();

   /*We should rename this when running in core server..*/
   evfs_worker_initialise();
   
   if (argc >= 2 && !strcmp(argv[1], "-nometa"))
       evfs_metadata_initialise(0);
   else
       evfs_metadata_initialise(1);    	  


   ecore_main_loop_begin();

   return 0;
}
