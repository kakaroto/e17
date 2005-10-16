/*

Copyright (C) 2005 Alex Taylor

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

#include <evfs.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <Ecore_File.h>
#include <libsmbclient.h>

//static struct stat file_stat;
int smbc_remove_unused_server(SMBCCTX * context, SMBCSRV * srv);
static SMBCCTX *smb_context = NULL;

static void smb_evfs_dir_list(evfs_client* client, evfs_command* command);


int smb_evfs_file_stat(evfs_command* command, struct stat* file_stat);
int evfs_file_open(evfs_filereference* file);
int evfs_file_close(evfs_filereference* file);
int evfs_file_seek(evfs_filereference* file, long offset, int whence);
int evfs_file_read(evfs_filereference* file, char* bytes, long size);
int evfs_file_write(evfs_filereference* file, char* bytes, long size);
int evfs_file_create(evfs_filereference* file);



/*---------------------------------------------------------------------------*/
/*A temporary function until we build authentication into the protocol*/
void auth_fn(const char *server, const char *share,
	     char *workgroup, int wgmaxlen, char *username, int unmaxlen,
	     char *password, int pwmaxlen)
{
  char temp[128];

  fprintf(stdout, "Need password for //%s/%s\n", server, share);

  fprintf(stdout, "Enter workgroup: [%s] ", workgroup);
  fgets(temp, sizeof(temp), stdin);

  if (temp[strlen(temp) - 1] == 0x0a) /* A new line? */
    temp[strlen(temp) - 1] = 0x00;

  if (temp[0]) strncpy(workgroup, temp, wgmaxlen - 1);

  fprintf(stdout, "Enter username: [%s] ", username);
  fgets(temp, sizeof(temp), stdin);

  if (temp[strlen(temp) - 1] == 0x0a) /* A new line? */
    temp[strlen(temp) - 1] = 0x00;

  if (temp[0]) strncpy(username, temp, unmaxlen - 1);

  fprintf(stdout, "Enter password: [%s] ", password);
  fgets(temp, sizeof(temp), stdin);

  if (temp[strlen(temp) - 1] == 0x0a) /* A new line? */
    temp[strlen(temp) - 1] = 0x00;

  if (temp[0]) strncpy(password, temp, pwmaxlen - 1);

}
/*------------------------------------------------------------------------*/


evfs_plugin_functions* evfs_plugin_init() {
	int err;

	
	printf("Initialising the samba plugin..\n");
	evfs_plugin_functions* functions = malloc(sizeof(evfs_plugin_functions));
	functions->evfs_dir_list = &smb_evfs_dir_list;
	functions->evfs_file_open = &evfs_file_open;
	functions->evfs_file_close = &evfs_file_close;
	functions->evfs_file_seek = &evfs_file_seek;
	functions->evfs_file_read = &evfs_file_read;
	functions->evfs_file_write = &evfs_file_write;
	functions->evfs_file_create = &evfs_file_create;
	functions->evfs_file_stat = &smb_evfs_file_stat;
	printf("Samba stat func at '%p'\n", &smb_evfs_file_stat);

	//Initialize samba (temporarily borrowed from gnomevfs)
	smb_context = smbc_new_context ();
	if (smb_context != NULL) {
		smb_context->debug = 0;
		smb_context->callbacks.auth_fn 		    = auth_fn;
		/*smb_context->callbacks.add_cached_srv_fn    = add_cached_server;
		smb_context->callbacks.get_cached_srv_fn    = get_cached_server;
		smb_context->callbacks.remove_cached_srv_fn = remove_cached_server;
		smb_context->callbacks.purge_cached_fn      = purge_cached;*/

		if (!smbc_init_context (smb_context)) {
			printf("Error initializing samba context..\n");
			smbc_free_context (smb_context, FALSE);
			smb_context = NULL;
		}
	}


	
	return functions;

	
}

char* evfs_plugin_uri_get() {
	return "smb";
}

int smb_evfs_file_stat(evfs_command* command, struct stat* file_stat) {
	
	int err = 0;
	int fd = 0;
	char dir[128];
	//struct stat* file_stat = calloc(1,sizeof(struct stat));
	
	
	sprintf(dir,"smb:/%s", command->file_command.files[0]->path);
	printf("Getting stat on file '%s'\n", dir);

	err = smb_context->stat(smb_context, (const char*)dir, file_stat);
	printf("Returned error code: %d\n", err);
	printf("File size: %d\n", file_stat->st_size);
	
	printf("Returning to caller..\n");

	return 0;

}

static void smb_evfs_dir_list(evfs_client* client, evfs_command* command) {
	char dir_path[1024];

	int fd, dh1, dh2, dh3, dsize, dirc;
	int size;
	char dirbuf[8192];
	char* dirp;
	SMBCFILE *dir = NULL;
	struct smbc_dirent *entry = NULL;
	Ecore_List* files = ecore_list_new();

	//Reappend smb protocol header for libsmbclient..
	snprintf(dir_path,1024,"smb:/%s", command->file_command.files[0]->path);
	
	printf("evfs_fs_samba: Listing directory %s\n", dir_path);


	dir = smb_context->opendir(smb_context,dir_path);

	while (entry = smb_context->readdir(smb_context, dir) ) {
		
	     /*Make sure we don't use . or ..*/
	   if (strcmp(entry->name, ".") && strcmp(entry->name, "..")) { 
		evfs_filereference* reference = NEW(evfs_filereference);

		if (entry->smbc_type == SMBC_FILE) reference->file_type = EVFS_FILE_NORMAL;
		else if (entry->smbc_type == SMBC_DIR) reference->file_type = EVFS_FILE_DIRECTORY; 

		size = 
			  (sizeof(char) * strlen(command->file_command.files[0]->path)) + 
			  (sizeof(char) * strlen(entry->name )) + 
			  (sizeof(char) * 2 );
		reference->path = malloc(size);
		snprintf(reference->path, size, "%s/%s", command->file_command.files[0]->path, entry->name );
	
		printf("File '%s' is of type '%d'\n", reference->path, reference->file_type);
		
		
		ecore_list_append(files, reference);
	   }
		
	}
	smb_context->closedir(smb_context,dir);
	
	evfs_list_dir_event_create(client, command, files);

}


int evfs_file_open(evfs_filereference* file) {
	char dir_path[1024];
	snprintf(dir_path,1024,"smb:/%s", file->path);

	printf("Opening file '%s' in samba\n", dir_path);

	file->fd_p = smb_context->open(smb_context, dir_path, O_RDONLY, S_IRUSR);

	return 0;
}

int evfs_file_close(evfs_filereference* file) {
	printf ("SMB close: closing\n");

	smb_context->close_fn(smb_context, file->fd_p);

	return 0;
}

int evfs_file_write(evfs_filereference* file, char* bytes, long size) {
	ssize_t i;
	
	printf ("SMB write: %d bytes\n", size);

	i = smb_context->write(smb_context, file->fd_p, bytes, size);
	printf("Wrote %d bytes\n", i);

	return 0;
}

int evfs_file_seek(evfs_filereference* file, long pos, int whence) {
	printf ("Seeking file to %ld\n", pos);

	smb_context->lseek(smb_context, file->fd_p, pos, SEEK_SET);

	return 0;
}

int evfs_file_read(evfs_filereference* file, char* bytes, long size) {
	int bytes_read = 0;
	printf("Reading %ld bytes from file %s\n", file->path);
	
	bytes_read = smb_context->read(smb_context, file->fd_p, bytes, size);

	return 0;
}



int evfs_file_create(evfs_filereference* file) {
	char dir_path[1024];
	snprintf(dir_path,1024,"smb:/%s", file->path);

	printf ("SMB File create: %s\n", dir_path);

	file->fd_p = smb_context->open(smb_context, dir_path, O_CREAT | O_TRUNC | O_RDWR , S_IRUSR | S_IWUSR);
	return 0;
}
