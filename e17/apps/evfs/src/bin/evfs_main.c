
/*

Copyright (C) 2000, 2001 Alexander Taylor <alex@logisticchaos.com>.

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


static evfs_server* server;

evfs_client* evfs_client_get(Ecore_Ipc_Client* client) {
	return ecore_hash_get(server->client_hash, client);
}

evfs_plugin* evfs_get_plugin_for_uri(char* uri_base) {
	return ecore_hash_get(server->plugin_uri_hash, uri_base);
}

int
ipc_client_add(void *data, int type, void *event)
{
      Ecore_Ipc_Event_Client_Add *e;
      evfs_client* client;


      e = (Ecore_Ipc_Event_Client_Add *) event;
      printf("ERR: EVFS Client Connected!!!\n");

      client = NEW(evfs_client);
      client->client = e->client;
      client->prog_command = NULL;
      ecore_hash_set(server->client_hash, client->client, client);

      server->num_clients++;

      
      return (1);
}

int
ipc_client_del(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Client_Del *e;
   evfs_client* client;


   e = (Ecore_Ipc_Event_Client_Del *) event;
    printf("ERR: Client Disconnected!!!\n");

    client = ecore_hash_get(server->client_hash, e->client);
    evfs_cleanup_client(client);

    
   return (1);
}


int
ipc_client_data(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Client_Data *e = (Ecore_Ipc_Event_Client_Data*) event;
   evfs_client* client;
   
   ecore_ipc_message* msg = ecore_ipc_message_new(e->major,e->minor,e->ref,e->ref_to,e->response,e->data,e->size);


   client = evfs_client_get(e->client);
   
   if (!client->prog_command) {
	   client->prog_command = evfs_command_new();
   }

   /*True == command finished*/
   if (evfs_process_incoming_command(client->prog_command, msg)) {
	  printf("Finished getting command..processing..\n"); 
	  evfs_handle_command(client, client->prog_command);

	  printf("Nuking command..\n");
	  client->prog_command = NULL; /*TODO - CLEANUP MEMORY HERE*/
   }



   return 1;
}


void evfs_handle_command(evfs_client* client, evfs_command* command) {
	switch (command->type) {
		case EVFS_CMD_STARTMON_FILE:
			printf("We recieved a monitor request\n");
			evfs_handle_monitor_start_command(client, command);
			
			break;
	}
}

void evfs_handle_monitor_start_command(evfs_client* client, evfs_command* command) {
	/*First get the plugin responsible for this file*/

	void (*evfs_monitor_start)(evfs_client* client, evfs_command* command);

	if (command->file_command.num_files > 0) {
		evfs_plugin* plugin = evfs_get_plugin_for_uri(command->file_command.files[0]->plugin_uri);

		if (!plugin) {
			printf("No plugin able to monitor this uri type\n");
		} else {
			printf("Requesting a file monitor from this plugin for uri type '%s'\n", command->file_command.files[0]->plugin_uri );
			
			evfs_monitor_start = dlsym(plugin->dl_ref, EVFS_MONITOR_START);
			if (evfs_monitor_start) {
				(*evfs_monitor_start)(client,command);
			}
			
		}
	} else {
		printf("No files to monitor!\n");
	}
}


evfs_plugin* evfs_load_plugin(char* filename) {
	evfs_plugin* plugin = NEW(evfs_plugin);

	char* (*evfs_plugin_uri_get)();

	printf("Loading plugin: %s\n", filename);	
	plugin->dl_ref = dlopen(filename, RTLD_LAZY);

	if (plugin->dl_ref) {
		evfs_plugin_uri_get = dlsym(plugin->dl_ref, "evfs_plugin_uri_get");
		if (evfs_plugin_uri_get) {
			plugin->uri = (*evfs_plugin_uri_get)();			
			printf("The plugin at '%s' handles '%s'\n", filename, plugin->uri);
		} else {
			printf("Error - plugin file does not contain uri identify function - %s\n", filename);
			goto exit_error;
		}
			
	} else {
		printf("Error - plugin file invalid - %s\n", filename);
		goto exit_error;
	}


	

	return plugin;


	exit_error:
		free(plugin);
		return NULL;
	
	
}


void evfs_load_plugins() {
        struct dirent* de;
        struct stat st;
        DIR* dir;
	evfs_plugin* plugin;
	char plugin_path[1024];

	printf("Reading plugins from: %s\n", PACKAGE_PLUGIN_DIR "/plugins/file");
        dir = opendir(PACKAGE_PLUGIN_DIR "/plugins/file");
        while ( (de = readdir(dir)) ) {

		if (!strncmp(de->d_name + strlen(de->d_name) -3, ".so", 3)) {
			snprintf(plugin_path, 1024,"%s/%s", PACKAGE_PLUGIN_DIR "/plugins/file", de->d_name);
			if ( (plugin = evfs_load_plugin(plugin_path))) {
				ecore_hash_set(server->plugin_uri_hash, plugin->uri, plugin);
			}
		}
	}

}



int main(int argc, char** argv) {
	/*Init the ipc server*/
	if (ecore_ipc_init() < 1)
		return (1);

	ecore_file_init();

	
	server = NEW(evfs_server);
	server->client_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
	server->plugin_uri_hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);

	/*Load the plugins*/
	evfs_load_plugins();




	if ((server->ipc_server = ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER, EVFS_IPC_TITLE, 0, NULL))) {
	      ecore_ipc_server_del(server->ipc_server);
	      free(server);
	      printf ("ERR: Server already running...\n");
	      return (1);
	} else {
	      printf ("ERR: Server created..\n");

	      server->ipc_server = ecore_ipc_server_add(ECORE_IPC_LOCAL_USER, EVFS_IPC_TITLE, 0, NULL);

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
