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

#include <evfs.h>
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

#define EVFS_PLUGIN_VFOLDER_GROUPS_ID "/Groups"
#define EVFS_PLUGIN_VFOLDER_QUERIES_ID "/Queries"
#define EVFS_PLUGIN_VFOLDER_URI "vfolder"

void evfs_dir_list(evfs_client * client, evfs_command * command,
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
evfs_dir_list(evfs_client * client, evfs_command * command,
              Ecore_List ** directory_list)
{
   Ecore_List *files = ecore_list_new();
   evfs_filereference* ref;
   char* path;

   path = command->file_command.files[0]->path;

   /*We should make this generic - perhaps a plugin system*/
   /*FIXME - but this will do for testing*/
   printf("Vfolder listing '%s'..\n", command->file_command.files[0]->path);
   
   if (!strcmp(path, "/")) {

	   /*Metadata groups ref*/
	   ref = NEW(evfs_filereference);
	   ref->plugin_uri = strdup(EVFS_PLUGIN_VFOLDER_URI);
	   ref->path = strdup(EVFS_PLUGIN_VFOLDER_GROUPS_ID);
	   ref->file_type = EVFS_FILE_DIRECTORY;
	   ecore_list_append(files, ref);

	   /*Metadata queries ref*/
	   ref = NEW(evfs_filereference);
	   ref->plugin_uri = strdup(EVFS_PLUGIN_VFOLDER_URI);
	   ref->path = strdup(EVFS_PLUGIN_VFOLDER_QUERIES_ID);
	   ref->file_type = EVFS_FILE_DIRECTORY;
	   ecore_list_append(files, ref);
   } else if (!strncmp(path, EVFS_PLUGIN_VFOLDER_GROUPS_ID, strlen(EVFS_PLUGIN_VFOLDER_GROUPS_ID))) {
	   if (!strcmp(path, EVFS_PLUGIN_VFOLDER_GROUPS_ID)) {
		   Evas_List* group_list;
		   Evas_List* iter;
		   char assemble[PATH_MAX];
		   evfs_metadata_group_header* g;
		   
		   /*Get group list, and return*/
		   group_list = evfs_metadata_groups_get();
		   
		   for (iter = group_list; iter; ) {
			   g = iter->data;
			   
			   snprintf(assemble, sizeof(assemble), "%s/%s", 
					   EVFS_PLUGIN_VFOLDER_GROUPS_ID, g->name);
			   
		   	   ref = NEW(evfs_filereference);
			   ref->plugin_uri = strdup(EVFS_PLUGIN_VFOLDER_URI);
			   ref->path = strdup(assemble);
			   if (g->visualhint) ref->attach = strdup(g->visualhint);
			   ref->file_type = EVFS_FILE_DIRECTORY;
			   ecore_list_append(files, ref);

			   iter = iter->next;
		   }
	   } else {
		   char* group_name;
		   char* item;
		   Ecore_List* list;
		   int i;
		   group_name = strstr(path + 1, "/") + 1;
		   evfs_file_uri_path* path;

		   printf("Group name: %s\n", group_name);

		   list = evfs_metadata_file_group_list(group_name);

		   ecore_list_goto_first(list);
		   while ( (item = ecore_list_remove_first(list))) {
			   path = evfs_parse_uri(item);
			   ecore_list_append(files, path->files[0]);

			   evfs_cleanup_file_uri_path(path);
		   }
		   ecore_list_destroy(list);
	   }
   }
		   
   *directory_list = evfs_file_list_sort(files);
}

int
evfs_client_disconnect(evfs_client * client)
{

   printf("Received disconnect for client at evfs_fs_vfolder.c for client %d\n",
          client->id);

}
