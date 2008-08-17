        /*
         * 
         * Copyright (C) 2006 Alex Taylor <alex@logisticchaos.com>.
         * 
         * Permission is hereby granted, free of charge, to any person obtaining a copy
         * of this software and associated documentation files (the "Software"), to
         * deal in the Software without restriction, including without limitation the
         * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
         * sell copies of the Software, and to permit persons to whom the Software is
         * furnished to do so, subject to the following conditions:
         * 
         * The above copyright notice and this permission notice shall be included in
         * all copies of the Software and its documentation and acknowledgment shall be
         * given in the documentation and software packages that this Software was
         * used.
         * 
         * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
         * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
         * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
         * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
         * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
         * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
         * 
         */
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "evfs.h"
#include "evfs_plugin.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <Ecore_File.h>


//#define EVFS_PLUGIN_VFOLDER_QUERIES_ID "/Queries"
#define MAX_GROUP_LENGTH 255

void evfs_dir_list(evfs_client * client, evfs_command* command,
              Ecore_List ** directory_list);

int
evfs_client_disconnect(evfs_client * client);


evfs_plugin_functions *
evfs_plugin_init()
{
   printf("Initialising the vfolder plugin..\n");
   evfs_plugin_functions *functions = calloc(1, sizeof(evfs_plugin_functions));

   functions->evfs_dir_list = &evfs_dir_list;
   functions->evfs_client_disconnect = &evfs_client_disconnect;
   
   return functions;
}

char *
evfs_plugin_uri_get()
{
   return EVFS_PLUGIN_VFOLDER_URI;
}


void
evfs_dir_list(evfs_client * client, evfs_command* command,
              Ecore_List ** directory_list)
{
   Ecore_List *files = ecore_list_new();
   EvfsFilereference* ref;
   char* path;

   EvfsFilereference* file = evfs_command_first_file_get(command);
   path = file->path;

   /*We should make this generic - perhaps a plugin system*/
   /*FIXME - but this will do for testing*/
   printf("Vfolder listing '%s'..\n", file->path);
   
   if (!strcmp(path, "/")) {
	   Ecore_List* keys = ecore_hash_keys(evfs_server_get()->plugin_vfolder_hash);
	   char* key;
	   char path[MAX_GROUP_LENGTH];

	   ecore_list_first_goto(keys);
	   while ((key = ecore_list_next(keys))) {
		   snprintf(path,sizeof(path),"/%s",key); 

		   ref = NEW(EvfsFilereference);
		   ref->plugin_uri = strdup(EVFS_PLUGIN_VFOLDER_URI);
		   ref->path = strdup(path);
		   ref->file_type = EVFS_FILE_DIRECTORY;
		   ecore_list_append(files, ref);
	   }
	   ecore_list_destroy(keys);

   } else {
	   /*Find the plugin that handles this, */
	   if (path[0] == '/') {
		   char vfolder_type[MAX_GROUP_LENGTH];
		   char* pos=strchr(&path[1], '/');
		   if (!pos) {
			   strncpy(vfolder_type, &path[1], MAX_GROUP_LENGTH);
		   } else {
			   strncpy(vfolder_type, &path[1], pos-path-1);
			   vfolder_type[pos-path-1] = '\0';
		   }
		   printf("Looking for vfolder plugin for: %s\n", vfolder_type);

		   evfs_plugin* plugin = ecore_hash_get(evfs_server_get()->plugin_vfolder_hash, vfolder_type);

		   if (EVFS_PLUGIN_VFOLDER(plugin)->functions->evfs_vfolder_list) 
			(*EVFS_PLUGIN_VFOLDER(plugin)->functions->evfs_vfolder_list)(file, &files);
	   }
	   
   }
		   
   *directory_list = evfs_file_list_sort(files);
}

int
evfs_client_disconnect(evfs_client * client)
{

   /*printf("Received disconnect for client at evfs_fs_vfolder.c for client %lu\n",
          client->id);*/
   return 1;
}
