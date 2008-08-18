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
#include <Efreet.h>

#define EVFS_PLUGIN_VFOLDER_TRASH_ID "/Trash"

void evfs_vfolder_list(EvfsFilereference* ref, Ecore_List** list);
static char* evfs_home_dir;
static char evfs_trash_home[PATH_MAX];

evfs_plugin_functions_vfolder *
evfs_plugin_init()
{
   printf("Initialising the vfolder trash plugin..\n");
   evfs_plugin_functions_vfolder *functions = calloc(1, sizeof(evfs_plugin_functions_vfolder));
   functions->evfs_vfolder_list = &evfs_vfolder_list;

   efreet_desktop_init();

   evfs_home_dir = strdup(getenv("HOME"));
   snprintf(evfs_trash_home, PATH_MAX, "%s/.Trash/info", evfs_home_dir);

   return functions;
}

char*
evfs_plugin_vfolder_root_get() {
	return "Trash";
}


void evfs_vfolder_list(EvfsFilereference* ref, Ecore_List** list)
{
	Ecore_List* info_files;
	char* file;
	char parser[PATH_MAX];

	info_files = ecore_file_ls(evfs_trash_home);

	while ( (file = ecore_list_first_remove(info_files))) {
		EvfsFilereference* ref;
		char* pos;
		
		snprintf(parser, PATH_MAX, "%s/%s", evfs_trash_home, file);
		Efreet_Desktop* desk = efreet_desktop_get(parser);

		if (desk->path) {
			pos = strrchr(desk->path, '/');
			if (pos ) {
				char* file_dup = strdup(parser);
		
				/*First check if this is a complete uri path in .info*/
				snprintf(parser, PATH_MAX, "trash:///%s", pos+1);
				printf("Parsing %s --", parser);
				ref = evfs_parse_uri_single(parser);
				ref->attach = file_dup;
				printf ("   %s    %s\n", ref->plugin_uri, ref->path);
		
				if (ref) 
					ecore_list_append(*list, ref);
			}
		}

		free(file);
		
	}

	ecore_list_destroy(info_files);
}



