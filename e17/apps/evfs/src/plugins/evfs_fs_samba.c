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

int smbc_remove_unused_server(SMBCCTX * context, SMBCSRV * srv);
static SMBCCTX *smb_context = NULL;

void smb_evfs_dir_list(evfs_client* client, evfs_command* command);
void smb_evfs_file_stat(evfs_client* client, evfs_command* command);



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
	evfs_plugin_functions* functions = calloc(1, sizeof(evfs_plugin_functions));
	functions->evfs_dir_list = &smb_evfs_dir_list;
	/*functions->evfs_file_remove= &evfs_file_remove;
	functions->evfs_monitor_start = &evfs_monitor_start;
	functions->evfs_monitor_stop = &evfs_monitor_stop;*/
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

void smb_evfs_file_stat(evfs_client* client, evfs_command* command) {
	int err = 0;
	int fd = 0;
	char dir[1024];
	struct stat* file_stat = calloc(1, sizeof(struct stat));
	SMBCFILE* file= NULL;
	
	snprintf(dir,1024,"smb:/%s", command->file_command.files[0]->path);
	printf("Getting stat on file '%s'\n", dir);

	
	file = smb_context->open(smb_context, dir, O_RDONLY, 0666);
	if (file) {
		smb_context->fstat(smb_context, file, file_stat);
		evfs_stat_event_create(client, command, file_stat);
		printf("File size: %d\n", file_stat->st_size);
		//smb_context->close(smb_context, file);
	} else {
		printf("Error opening file!\n");
	}
	
	printf("Returning to caller..\n");
}

void smb_evfs_dir_list(evfs_client* client, evfs_command* command) {
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
