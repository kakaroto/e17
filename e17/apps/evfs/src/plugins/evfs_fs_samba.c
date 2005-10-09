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


void evfs_dir_list(evfs_client* client, evfs_command* command);



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

	
	//Initialize samba
	err = smbc_init(auth_fn,  2);
	

	functions->evfs_dir_list = &evfs_dir_list;
	/*functions->evfs_file_remove= &evfs_file_remove;
	functions->evfs_monitor_start = &evfs_monitor_start;
	functions->evfs_monitor_stop = &evfs_monitor_stop;
	functions->evfs_file_stat = &evfs_file_stat;*/
	return functions;

	
}

char* evfs_plugin_uri_get() {
	return "smb";
}

void evfs_dir_list(evfs_client* client, evfs_command* command) {
	char dir[1024];

	int fd, dh1, dh2, dh3, dsize, dirc;
	int size;
	char dirbuf[4096];
	char* dirp;
	Ecore_List* files = ecore_list_new();

	//Reappend smb protocol header for libsmbclient..
	snprintf(dir,1024,"smb:/%s", command->file_command.files[0]->path);
	
	printf("evfs_fs_samba: Listing directory %s\n", dir);


	dh1 = smbc_opendir(dir);
	dirp = (char*)dirbuf;
	dirc = smbc_getdents(dh1, (struct smbc_dirent *)dirp, sizeof(dirbuf));

	while (dirc > 0) {
		evfs_filereference* reference = NEW(evfs_filereference);

	      dsize = ((struct smbc_dirent *)dirp)->dirlen;
	      	fprintf(stdout, "Dir Ent, Type: %u, Name: %s, Comment: %s\n",
	      ((struct smbc_dirent *)dirp)->smbc_type, 
	      ((struct smbc_dirent *)dirp)->name, 
	      ((struct smbc_dirent *)dirp)->comment);

		if (((struct smbc_dirent *)dirp)->smbc_type == 7) reference->file_type = FILE_NORMAL;
		else if (((struct smbc_dirent *)dirp)->smbc_type == 7) reference->file_type = FILE_DIRECTORY; 

		size = 
			  (sizeof(char) * strlen(command->file_command.files[0]->path)) + 
			  (sizeof(char) * strlen(((struct smbc_dirent *)dirp)->name )) + 
			  (sizeof(char) * 2 );
		reference->path = malloc(size);
		snprintf(reference->path, size, "%s/%s", command->file_command.files[0]->path, ((struct smbc_dirent *)dirp)->name );
		
		
		
		ecore_list_append(files, reference);

	      dirp += dsize;
	      dirc -= dsize;

	}

	evfs_list_dir_event_create(client, command, files);

}
