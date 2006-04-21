
/*

Copyright (C) 2005 Alexander Taylor <alex@logisticchaos.com>.

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

static evfs_server *server;

evfs_server* 
evfs_server_get()
{
	return server;
}

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

int
ipc_client_add(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Ipc_Event_Client_Add *e;
   evfs_client *client;

   e = (Ecore_Ipc_Event_Client_Add *) event;
   /*printf("ERR: EVFS Client Connected!!!\n"); */

   client = NEW(evfs_client);
   client->client = e->client;
   client->server = server;
   client->prog_command = NULL;
   client->id = evfs_server_get_next_id(server);
   ecore_hash_set(server->client_hash, client->client, client);

   server->num_clients++;

   evfs_event_client_id_notify(client);

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

   client = ecore_hash_get(server->client_hash, e->client);
   printf("Client %ld, Client Disconnected!!!\n", client->id);

   /*Notify the plugins that this client has disconnected */
   keys = ecore_hash_keys(server->plugin_uri_hash);
   ecore_list_goto_first(keys);
   while ((key = ecore_list_remove_first(keys)))
     {
        plugin = ecore_hash_get(server->plugin_uri_hash, key);
        (*EVFS_PLUGIN_FILE(plugin)->functions->evfs_client_disconnect) (client);
     }

   ecore_list_destroy(keys);

   ecore_hash_remove(server->client_hash, client);
   evfs_cleanup_client(client);

   return (1);
}

int
ipc_client_data(void *data __UNUSED__, int type __UNUSED__, void *event)
{

   Ecore_Ipc_Event_Client_Data *e = (Ecore_Ipc_Event_Client_Data *) event;
   evfs_client *client;

   ecore_ipc_message *msg =
      ecore_ipc_message_new(e->major, e->minor, e->ref, e->ref_to, e->response,
                            e->data, e->size);

   client = evfs_client_get(e->client);

   if (!client->prog_command)
     {
        client->prog_command = evfs_command_new();
     }

   /*True == command finished */
   if (evfs_process_incoming_command(server, client->prog_command, msg))
     {
        evfs_command_client *com_cli = NEW(evfs_command_client);

        com_cli->client = client;
        com_cli->command = client->prog_command;
        client->prog_command = NULL;
        ecore_list_append(server->incoming_command_list, com_cli);
     }

   free(msg);

   return 1;
}

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
        printf("File copy handler\n");
        evfs_handle_file_copy(client, command, command);
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
     default:
        printf("Warning - unhandled command %d\n", command->type);
        break;
     }

   return cleanup_command;
}

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
   evfs_plugin_meta *plugin = NEW(evfs_plugin_file);
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
			 while ( (type = ecore_list_remove_first(types))) {
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
                PACKAGE_PLUGIN_DIR "/plugins/file");
        exit(1);
     }
   closedir(dir);

}

int
ecore_timer_enterer(__UNUSED__ void *data)
{
   evfs_operation_queue_run();
	
   return 1;
}

int
incoming_command_cb(__UNUSED__ void *data)
{
   int clean =0;
	
   evfs_command_client *com_cli =
      ecore_list_remove_first(server->incoming_command_list);

   if (com_cli)
     {
        clean = evfs_handle_command(com_cli->client, com_cli->command);
        if (clean) evfs_cleanup_command(com_cli->command, EVFS_CLEANUP_FREE_COMMAND);
        free(com_cli);
     }

   return 1;
}

int
main(int argc, char **argv)
{
   /*Init the ipc server */
   if (ecore_ipc_init() < 1)
      return (1);

   ecore_file_init();

   server = NEW(evfs_server);
   server->client_hash =
      ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
   server->plugin_uri_hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);
   server->plugin_meta_hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);
   server->clientCounter = 0;
   server->incoming_command_list = ecore_list_new();

   ecore_idle_enterer_add(incoming_command_cb, NULL);

   /*Add a timer, to make sure our event loop keeps going.  Kinda hacky */
   ecore_timer_add(0.01, ecore_timer_enterer, NULL);

   /*Load the plugins */
   evfs_load_plugins();
   evfs_io_initialise();
   evfs_vfolder_initialise();
   evfs_operation_initialise();

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

        ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_ADD, ipc_client_add,
                                NULL);
        ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DEL, ipc_client_del,
                                NULL);
        ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DATA, ipc_client_data,
                                NULL);
     }

   ecore_main_loop_begin();

   return 0;
}
