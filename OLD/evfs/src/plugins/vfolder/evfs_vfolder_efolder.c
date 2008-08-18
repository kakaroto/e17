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
#include <sqlite3.h>
#include "evfs_metadata_db.h"

#define EVFS_PLUGIN_VFOLDER_EFOLDER_ID "/E Folders"

void evfs_vfolder_list(EvfsFilereference* ref, Ecore_List** list);

evfs_plugin_functions_vfolder *
evfs_plugin_init()
{
   printf("Initialising the vfolder efolder plugin..\n");
   evfs_plugin_functions_vfolder *functions = calloc(1, sizeof(evfs_plugin_functions_vfolder));
   functions->evfs_vfolder_list = &evfs_vfolder_list;

   return functions;
}

char*
evfs_plugin_vfolder_root_get() {
	return "E Folders";
}


void evfs_vfolder_list(EvfsFilereference* ref, Ecore_List** list)
{
	sqlite3* db;
	Ecore_List* retlist;
	char* path = ref->path;
	char* name;
	EvfsFilereference* new;
	char assemble[PATH_MAX];

	db = evfs_metadata_db_connect();
		
	if (!strcmp(path, EVFS_PLUGIN_VFOLDER_EFOLDER_ID)) {
		retlist = evfs_metadata_db_vfolder_search_list_get(db);
		ecore_list_first_goto(retlist);
		while ((name = ecore_list_first_remove(retlist))) {
	 	        snprintf(assemble, sizeof(assemble), "%s/%s", 
	     	           EVFS_PLUGIN_VFOLDER_EFOLDER_ID, name);
	
			new = NEW(EvfsFilereference);
	  	        new->plugin_uri = strdup(EVFS_PLUGIN_VFOLDER_URI);
	   	        new->path = strdup(assemble);
		        new->file_type = EVFS_FILE_DIRECTORY;

			ecore_list_append(*list, new);
		}
		ecore_list_destroy(retlist);
	} else {
		char* efolder_name;
		int id;
		efolder_name = strstr(path + 1, "/") + 1;

		/*Get the id for this efolder*/	
		if (efolder_name) {
			id = evfs_metadata_db_vfolder_search_id_get(db,efolder_name);

			if (id) {
				EvfsVfolderEntry* entry;
				Ecore_List* entries = 
					evfs_metadata_db_vfolder_search_entries_get(db,id);
				Ecore_List* files;

				files = evfs_metadata_db_vfolder_search_entries_execute(db,entries);
				ecore_list_first_goto(files);
				while ((name = ecore_list_first_remove(files))) {
					new = evfs_parse_uri_single(name);
					ecore_list_append(*list, new);
				}


			}
		}
	}

	evfs_metadata_db_close(db);
}
