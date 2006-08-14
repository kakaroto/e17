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

/*All these homedir variables need to be consolidated - TODO*/
static char evfs_fs_trash_info[PATH_MAX];
static char evfs_fs_trash_files[PATH_MAX];
static evfs_plugin* posix_plugin;

/*Main file wrappers */
int evfs_file_remove(char *src);
int evfs_file_rename(evfs_client * client, evfs_command * command);

int evfs_client_disconnect(evfs_client * client);
int evfs_file_open(evfs_client * client, evfs_filereference * file);
int evfs_file_close(evfs_filereference * file);
int evfs_file_stat(evfs_command * command, struct stat *file_stat, int);
int evfs_file_lstat(evfs_command * command, struct stat *file_stat, int);
int evfs_file_seek(evfs_filereference * file, long offset, int whence);
int evfs_file_read(evfs_client * client, evfs_filereference * file,
                   char *bytes, long size);
int evfs_file_write(evfs_filereference * file, char *bytes, long size);
int evfs_file_create(evfs_filereference * file);
int evfs_file_mkdir(evfs_filereference * file);
void evfs_dir_list(evfs_client * client, evfs_command * command,
                   Ecore_List ** directory_list);

evfs_plugin_functions *
evfs_plugin_init()
{
   printf("Initialising the file plugin..\n");

   snprintf(evfs_fs_trash_info, sizeof(evfs_fs_trash_info), "%s/.Trash/info", getenv("HOME"));
   snprintf(evfs_fs_trash_files, sizeof(evfs_fs_trash_files), "%s/.Trash/files", getenv("HOME"));
   posix_plugin = evfs_get_plugin_for_uri(evfs_server_get(), "file");
   
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
   printf("Received disconnect for client at evfs_fs_posix.c for client %d\n",
          client->id);
}

int
evfs_file_stat(evfs_command * command, struct stat *file_stat, int file_number)
{
	/*FIXME - if we're asking for the root - it's a directory*/
	if (!strcmp(command->file_command.files[file_number]->path, "/")) {
		file_stat->st_mode |= S_IFDIR;
		return EVFS_SUCCESS;
	}
	
	return EVFS_ERROR;
}

int
evfs_file_open(evfs_client * client, evfs_filereference * file)
{
	printf("evfs_fs_trash.c open - STUB\n");
	return -1;	
}

int
evfs_file_close(evfs_filereference * file)
{
	if (file->plugin) 
		return (*EVFS_PLUGIN_FILE(posix_plugin)->functions->evfs_file_close) (file);
	else
		printf("Trash file not opened with trash plugin\n");	

	return 0;
}

int
evfs_file_seek(evfs_filereference * file, long offset, int whence)
{
	printf("evfs_fs_trash.c seek - STUB\n");
	return -1;	
}

int
evfs_file_read(evfs_client * client, evfs_filereference * file, char *bytes,
               long size)
{
	printf("evfs_fs_trash.c read - STUB\n");	
	return -1;
}

int
evfs_file_write(evfs_filereference * file, char *bytes, long size)
{
	if (file->plugin) {
		return (*EVFS_PLUGIN_FILE(posix_plugin)->functions->evfs_file_write) (file, bytes, size);
	} else
		printf("Trash file not opened with trash plugin\n");
}

int
evfs_file_create(evfs_filereference * file)
{	
	evfs_filereference* file_trash = evfs_filereference_clone(file);
	int fd;
	int size;
	
	free(file_trash->path);
	size= strlen(evfs_fs_trash_files) + strlen(file->path) +2;
	file_trash->path = calloc(size, 1);
	snprintf(file_trash->path, size, "%s/%s", evfs_fs_trash_files, file->path);
	free(file_trash->plugin_uri);
	file_trash->plugin_uri = strdup("file");
	file_trash->plugin = posix_plugin;

	printf("Creating trash file: %s\n", file_trash->path);

	(*EVFS_PLUGIN_FILE(file_trash->plugin)->functions->evfs_file_create) (file_trash);
	file->fd = file_trash->fd;	

	printf("Trash file fd: %d\n", file->fd);
	
	evfs_cleanup_filereference(file_trash);
	
	return file->fd;
}

int
evfs_file_mkdir(evfs_filereference * file)
{
	printf("evfs_fs_trash.c mkdir - STUB\n");
	return -1;
}
