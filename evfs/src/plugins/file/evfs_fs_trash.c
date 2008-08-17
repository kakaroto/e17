        /*
         * 
         * Copyright (C) 2006 by Alex Taylor
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

/* TODO:
 *
 * * Cloning a filereference, and passing it off to evfs_fs_posix, for each op, is
 *   terribly inefficient.  Make a caching engine to avoid this 
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

/*All these homedir variables need to be consolidated - TODO*/
static char* next_trash_file;
static char* next_trash_path;
static Ecore_Hash* trash_dir_mapping;
evfs_plugin* posix_plugin;

/*Main file wrappers */
int evfs_file_remove(char *src);
int evfs_file_rename(evfs_client * client, evfs_command * command);

int evfs_client_disconnect(evfs_client * client);
int evfs_file_open(evfs_client * client, EvfsFilereference * file);
int evfs_file_close(EvfsFilereference * file);
int evfs_file_stat(evfs_command * command, struct stat *file_stat, int);
int evfs_file_lstat(evfs_command * command, struct stat *file_stat, int);
int evfs_file_seek(EvfsFilereference * file, long offset, int whence);
int evfs_file_read(evfs_client * client, EvfsFilereference * file,
                   char *bytes, long size);
int evfs_file_write(EvfsFilereference * file, char *bytes, long size);

void evfs_file_notify_create(EvfsFilereference* file);
int evfs_file_create(EvfsFilereference * file);
int evfs_file_mkdir(EvfsFilereference * file);
void evfs_dir_list(evfs_client * client, evfs_command * command,
                   Ecore_List ** directory_list);


EvfsFilereference* evfs_fs_trash_proxy_create(EvfsFilereference* ref, char* newpath)
{
	int size;
	EvfsFilereference* newfile = EvfsFilereference_clone(ref);

	/*Make a proxy file, and send this to the posix plugin create*/
	free(newfile->path);
	
	size= strlen(evfs_trash_files_dir_get()) + strlen(newpath) +2;
	newfile->path = calloc(size, 1);
	snprintf(newfile->path, size, "%s/%s", evfs_trash_files_dir_get(), newpath);
	free(newfile->plugin_uri);

	newfile->plugin_uri = strdup("file");
	newfile->plugin = posix_plugin;

	return newfile;
}

EvfsFilereference* evfs_fs_trash_proxy_create_absolute(EvfsFilereference* ref, char* newdir, char* newsuffix)
{
	int size;
	EvfsFilereference* newfile = EvfsFilereference_clone(ref);

	/*Make a proxy file, and send this to the posix plugin create*/
	free(newfile->path);
	
	size= strlen(newdir) + 1 + strlen(newsuffix) + 1;
	newfile->path = calloc(size, 1);
	snprintf(newfile->path, size, "%s/%s", newdir, newsuffix);
	free(newfile->plugin_uri);

	newfile->plugin_uri = strdup("file");
	newfile->plugin = posix_plugin;

	return newfile;
}

/*Internal functions*/
char* evfs_fs_trash_filename_get(EvfsFilereference* ref)
{
	time_t res;
	int fulllen;
	char* newname;
	int i;

	fulllen = strlen(ref->path) + 1 + 11 /*time() length*/;
	newname = calloc(fulllen, 1);

	

	res = time(NULL);
	snprintf(newname, fulllen, "%s.%ld", ref->path, res);
	for (i=0;i<strlen(newname);i++)
		if (newname[i] == '/') newname[i] = '_';

	return newname;
}

void evfs_fs_trash_infofile_create(EvfsFilereference* ref, char* newname, char* path)
{
	int origlen;
	FILE* file;
	char* fullpath;

	/* 1 for '/', 1 for \0 */
	origlen = strlen(evfs_trash_info_dir_get()) + 1 + strlen(newname)+10+1;
	fullpath = malloc(origlen);
	snprintf(fullpath, origlen, "%s/%s%s", evfs_trash_info_dir_get(), newname, ".trashinfo");

	printf("Create info file: '%s'\n", fullpath);

	/*Should we use the evfs wrappers here? Faster not to..*/
	if ((file = fopen(fullpath, "w+"))){
		fprintf(file, "[Trash Info]\n");
		fprintf(file, "Path=%s\n", path);
		fprintf(file, "DeletionDate=20040831T22:32:08\n");
		
		fclose(file);
	} else {
		printf("Could not open trash info file\n");
	}

	free(fullpath);
}

evfs_plugin_functions *
evfs_plugin_init()
{
   printf("Initialising the file plugin..\n");

   /*FIXME - this assumes the trash plugin is loaded after posix - not always true*/
   posix_plugin = evfs_get_plugin_for_uri(evfs_server_get(), "file");

   trash_dir_mapping = ecore_hash_new(ecore_str_hash, ecore_str_compare);
   ecore_hash_free_key_cb_set(trash_dir_mapping, free);

   next_trash_file = NULL;
   
   evfs_plugin_functions *functions = calloc(1, sizeof(evfs_plugin_functions));
   functions->evfs_client_disconnect = &evfs_client_disconnect;

   /*functions->evfs_file_remove = &evfs_file_remove;
   functions->evfs_monitor_start = &evfs_monitor_start;
   functions->evfs_monitor_stop = &evfs_monitor_stop;*/
   functions->evfs_file_stat = &evfs_file_stat;
   functions->evfs_file_lstat = &evfs_file_stat;

   functions->evfs_file_open = &evfs_file_open;
   functions->evfs_file_close = &evfs_file_close;
   /*functions->evfs_dir_list = &evfs_dir_list;*/

   functions->evfs_file_seek = &evfs_file_seek;
   functions->evfs_file_read = &evfs_file_read;
   functions->evfs_file_write = &evfs_file_write;
   
   functions->evfs_file_create = &evfs_file_create;
   functions->evfs_file_notify_create = &evfs_file_notify_create;

   functions->evfs_file_mkdir = &evfs_file_mkdir;
   /*functions->evfs_file_rename = &evfs_file_rename;*/
   return functions;

}

char *
evfs_plugin_uri_get()
{
   return "trash";
}

int
evfs_client_disconnect(evfs_client * client)
{
   /*printf("Received disconnect for client at evfs_fs_posix.c for client %lu\n",
          client->id);*/
   return 0;
}

int
evfs_file_stat(evfs_command * command, struct stat *file_stat, int file_number)
{
	EvfsFilereference* ref = evfs_command_nth_file_get(command,file_number);

	printf("Performing stat on: '%s'\n", ref->path);
	
	/*FIXME - if we're asking for the root - it's a directory*/
	if (!strcmp(ref->path, "/")) {
		memset(file_stat, sizeof(struct stat), 0);
		file_stat->st_mode = S_IFDIR;
		return EVFS_SUCCESS;
	} else {
		if (ref->attach) {
			EvfsFilereference* proxy;
			char* pos;
			char* slashpos;
			
			pos = strstr(ref->attach, ".trashinfo");
			slashpos = strrchr(ref->attach, '/');
			if (pos && slashpos) { 
				char* newfile = NULL;
				evfs_command* command_proxy;
				int res;
				int newsize;
				
				newsize= strlen(slashpos) - strlen(".trashinfo");
				newfile = malloc(newsize);
				strncpy(newfile, slashpos+1 , newsize);
				newfile[newsize-1] = '\0';
				proxy = evfs_fs_trash_proxy_create(ref, newfile); 
				free(newfile);
				
				command_proxy = evfs_file_command_single_build(proxy);

				res = (*EVFS_PLUGIN_FILE(proxy->plugin)->functions->evfs_file_stat) 
					(command_proxy, file_stat, 0);
				
				evfs_cleanup_command(command_proxy,EVFS_CLEANUP_FREE_COMMAND);

				return res;
			} else {
				printf("Attach data did not contain trashinfo\n");
			}	
		}
	}
	
	return EVFS_ERROR;
}

int
evfs_file_open(evfs_client * client, EvfsFilereference * file)
{
	printf("evfs_fs_trash.c open - STUB\n");
	return 0;	
}

int
evfs_file_close(EvfsFilereference * file)
{
	if (file->plugin) 
		return (*EVFS_PLUGIN_FILE(posix_plugin)->functions->evfs_file_close) (file);
	else
		printf("Trash file not opened with trash plugin\n");	

	return 0;
}

int
evfs_file_seek(EvfsFilereference * file, long offset, int whence)
{
	printf("evfs_fs_trash.c seek - STUB\n");
	return -1;	
}

int
evfs_file_read(evfs_client * client, EvfsFilereference * file, char *bytes,
               long size)
{
	printf("evfs_fs_trash.c read - STUB\n");	
	return 0;
}

int
evfs_file_write(EvfsFilereference * file, char *bytes, long size)
{
	if (file->plugin) {
		return (*EVFS_PLUGIN_FILE(posix_plugin)->functions->evfs_file_write) (file, bytes, size);
	} else {
		printf("Trash file not opened with trash plugin\n");
		return -1;
	}
}

void evfs_file_notify_create(EvfsFilereference* ref)
{
	if (next_trash_file) {
		free(next_trash_file);
		next_trash_file = NULL;
	}
	if (next_trash_path) {
		free(next_trash_path);
		next_trash_path = NULL;

	}
	next_trash_file = evfs_fs_trash_filename_get(ref);
	next_trash_path = EvfsFilereference_to_string(ref);
	printf("Next trash path is : %s\n", next_trash_path);
}

int
evfs_file_create(EvfsFilereference * file)
{	
	EvfsFilereference* file_trash;
	char* pos;
	char* rewrite_parent;
	char* parent_dir;

	printf("File->path at trash create: '%s'\n", file->path);

	/*Check if this file lives in a directory other than '/'*/
	if ( (pos = strchr(file->path+1, '/'))) {
		/*If it does, see if we have a mapping for this dir*/
		parent_dir = calloc(pos-file->path+1,1 );
		strncpy(parent_dir, file->path,pos-file->path);
		parent_dir[pos-file->path] = '\0';

		printf("Rewritten dir: %s\n", parent_dir);
		rewrite_parent = ecore_hash_get(trash_dir_mapping, parent_dir);
		printf("Parent dir: %s\n", rewrite_parent);
		printf("Suffix: '%s'\n", pos+1);

		file_trash = evfs_fs_trash_proxy_create_absolute(file, rewrite_parent, pos+1);

		free(parent_dir);
	} else {
		printf("Destination path: %s\n", file->path);
		file_trash = evfs_fs_trash_proxy_create(file, next_trash_file);
	
		/*printf("Creating new file: %s\n", file_trash->path);*/

		/*Create the infofile*/
		evfs_fs_trash_infofile_create(file,next_trash_file,next_trash_path);
	}

	/*Dispatch to posix*/
	(*EVFS_PLUGIN_FILE(file_trash->plugin)->functions->evfs_file_create) (file_trash);
	file->fd = file_trash->fd;	

	evfs_cleanup_filereference(file_trash);

	free(next_trash_file);
	free(next_trash_path);

	next_trash_file = NULL;
	next_trash_path = NULL;
	
	return file->fd;
}

int
evfs_file_mkdir(EvfsFilereference * file)
{
	char* pos;
	char* rewrite_parent;
	char* parent_dir;
	EvfsFilereference* par;

	/*Check if this file lives in a directory other than '/'*/
	if ( (pos = strchr(file->path+1, '/'))) {
		/*If it does, see if we have a mapping for this dir*/
		parent_dir = calloc(pos-file->path+1,1 );
		strncpy(parent_dir, file->path,pos-file->path);
		parent_dir[pos-file->path] = '\0';

		printf("Rewritten dir: %s\n", parent_dir);
		rewrite_parent = ecore_hash_get(trash_dir_mapping, parent_dir);
		printf("Parent dir: %s\n", rewrite_parent);
		printf("Suffix: '%s'\n", pos+1);

		par=  	evfs_fs_trash_proxy_create_absolute(file, rewrite_parent, pos+1);

		free(parent_dir);
	} else {
		par = evfs_fs_trash_proxy_create(file, next_trash_file);
		evfs_fs_trash_infofile_create(file,next_trash_file,next_trash_path);
	}

	/*Dispatch to posix*/
	(*EVFS_PLUGIN_FILE(par->plugin)->functions->evfs_file_mkdir) (par);
	
	printf("Trash mkdir: '%s' '%s' '%s'\n", file->path, next_trash_path, next_trash_file);

	ecore_hash_set(trash_dir_mapping, strdup(file->path), strdup(par->path));

	evfs_cleanup_filereference(par);

	return 1;
}
