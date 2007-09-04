/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
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
#include <limits.h>

int smbc_remove_unused_server(SMBCCTX * context, SMBCSRV * srv);
static int smbLOCK = 0;
static SMBCCTX *smb_context = NULL;
Ecore_List *auth_cache;
Ecore_List* auth_command_list;
int smb_next_fd;
Ecore_Hash *smb_fd_hash;

evfs_auth_cache *
evfs_auth_cache_get(Ecore_List * cache, char *path);

static void smb_evfs_dir_list(evfs_client * client, evfs_command* command,
                              Ecore_List ** directory_list);
int smb_evfs_file_stat(evfs_command * command, struct stat *file_stat, int);
int evfs_file_open(evfs_client * client, EvfsFilereference * file);
int evfs_file_close(EvfsFilereference * file);
int evfs_file_seek(EvfsFilereference * file, long offset, int whence);
int evfs_file_read(evfs_client * client, EvfsFilereference * file,
                   char *bytes, long size);
int evfs_file_write(EvfsFilereference * file, char *bytes, long size);
int evfs_file_create(EvfsFilereference * file);
int smb_evfs_file_rename(evfs_client* client, evfs_command* command);
int smb_evfs_file_mkdir(EvfsFilereference * file);
int evfs_file_remove(char *file);
void
evfs_auth_push(evfs_command* command);

int
evfs_client_disconnect(evfs_client * client)
{
   /*printf("Received disconnect for client at evfs_fs_samba.c for client %d\n",
          client->id);*/
}

typedef struct {
	evfs_command* command;
	evfs_client* client;
} smb_auth_struct;
evfs_auth_cache* last_auth = NULL;

smb_auth_struct* evfs_smb_auth_top_get()
{
	smb_auth_struct* as = NULL;
	
	if (ecore_list_count(auth_command_list) >0) {		 
		ecore_list_first_goto(auth_command_list);
		as = ecore_list_current(auth_command_list);
		printf("Request for top auth item (count %d): com: %p, cli: %p\n",ecore_list_count(auth_command_list), as->command, as->client);
		return ecore_list_current(auth_command_list);
	}

	return NULL;
}

void evfs_smb_auth_push(evfs_command* command, evfs_client* client)
{
	smb_auth_struct* as = NEW(smb_auth_struct);
	as->command = command;
	as->client = client;
	ecore_list_prepend(auth_command_list, as);

	printf("Pushed comm: %p, cli: %p to stack\n", command, client);
}

void evfs_smb_auth_pop(evfs_command* command)
{
	smb_auth_struct* it;
	ecore_list_first_goto(auth_command_list);

	while ( (it=ecore_list_next(auth_command_list))) {
		if (it->command = command) {
			printf("Popped comm: %p, cli: %p from stack\n", it->command, it->client);	
			
			ecore_list_first_remove(auth_command_list);
			free(it);			
			return;
		}
	}
	
	printf("EVFS_SMB: error: Could not find command in auth list\n");
}

evfs_auth_cache*
evfs_auth_structure_add(char *username, char *password,
                        char *path)
{
   evfs_auth_cache *obj = NULL;

  
   /*Populate last auth structure*/
   if (last_auth->username) free(last_auth->username);
   if (last_auth->password) free(last_auth->password);
   last_auth->username = strdup(username);
   last_auth->password = strdup(password);
     
   if ((obj = evfs_auth_cache_get(auth_cache,path)) == NULL) {
	   obj = NEW(evfs_auth_cache);

	   obj->username = strdup(username);
	   obj->password = strdup(password);
	   obj->path = strdup(path);
	
	   printf("Added %s:***** to '%s' for auth\n", username, path);

	   ecore_list_append(auth_cache, obj);
   } else {
	   free(obj->username);
	   free(obj->password);
	   obj->username = strdup(username);
	   obj->password = strdup(password);
	   obj->attempts = 0;

	   printf("Updated auth for '%s': %s:*****\n",path,username);
   }

   return obj;
}

evfs_auth_cache *
evfs_auth_cache_get(Ecore_List * cache, char *path)
{
   evfs_auth_cache *obj;

   printf("Looking for match for '%s' in auth cache\n", path);

   ecore_list_first_goto(cache);
   while ((obj = ecore_list_next(cache)))
     {
        if (!strncmp(obj->path, path, strlen(path)))
          {
             printf("Found match for path in cache: user '%s'\n",
                    obj->username);
             return obj;
          }
     }

   return NULL;
}

int
smb_fd_get_next(void *ptr)
{
   smb_next_fd++;
   ecore_hash_set(smb_fd_hash, (int *)smb_next_fd, ptr);

   printf("Wrote ptr %p for fd %d\n", ptr, smb_next_fd);

   return smb_next_fd;
}

void *
smb_fd_get_for_int(int fd)
{
   return ecore_hash_get(smb_fd_hash, (int *)fd);
}

void
evfs_smb_populate_fd(EvfsFilereference * file)
{
   if (file->fd_p == NULL)
     {
        file->fd_p = smb_fd_get_for_int(file->fd);
     }
}

/*---------------------------------------------------------------------------*/
/*A temporary function until we build authentication into the protocol*/
void
auth_fn(const char *server, const char *share,
        char *workgroup, int wgmaxlen, char *username, int unmaxlen,
        char *password, int pwmaxlen)
{
   char temp[128];
   char path[PATH_MAX];
   evfs_auth_cache *obj;

   fprintf(stdout, "Need password for //%s/%s\n", server, share);
   snprintf(path, 512, "/%s/%s", server, share);

   /*Look for a cached auth for this */
   obj = evfs_auth_cache_get(auth_cache, path);
   if (obj)
     {
        if (obj->attempts == 0) {
		strncpy(username, obj->username, unmaxlen);
	        strncpy(password, obj->password, pwmaxlen);
		obj->attempts++;
	} else {
	     /*Don't auth for ADMIN shares*/
	     if (!strstr(share, "$")) {
		     printf("Sending auth request to client...\n");
		     smb_auth_struct* as = evfs_smb_auth_top_get();
		     if (as) {
			     evfs_auth_failure_event_create(as->client,as->command);
		     } else {
			     printf("No command to request auth for in stack.\n");
		     }
	     }
	}
        return;
     } else {
	     /*No object found, try with guest*/
	     obj = evfs_auth_structure_add("guest", "guest", path);
	     strncpy(username, obj->username, unmaxlen);
	     strncpy(password, obj->password, pwmaxlen);
     }

}

/*------------------------------------------------------------------------*/

evfs_plugin_functions *
evfs_plugin_init()
{
   int err;

   printf("Initialising the samba plugin..\n");
   evfs_plugin_functions *functions = malloc(sizeof(evfs_plugin_functions));

   functions->evfs_dir_list = &smb_evfs_dir_list;
   functions->evfs_client_disconnect = &evfs_client_disconnect;
   functions->evfs_file_open = &evfs_file_open;
   functions->evfs_file_close = &evfs_file_close;
   functions->evfs_file_seek = &evfs_file_seek;
   functions->evfs_file_read = &evfs_file_read;
   functions->evfs_file_write = &evfs_file_write;
   functions->evfs_file_create = &evfs_file_create;
   functions->evfs_file_stat = &smb_evfs_file_stat;
   functions->evfs_file_lstat = &smb_evfs_file_stat;    /*Windows file systems have no concept
                                                         * of 'lstat' */
   functions->evfs_file_rename = &smb_evfs_file_rename;

   functions->evfs_file_mkdir = &smb_evfs_file_mkdir;
   functions->evfs_file_remove = &evfs_file_remove;
   functions->evfs_auth_push = &evfs_auth_push;
   
   printf("Samba stat func at '%p'\n", &smb_evfs_file_stat);

   auth_cache = ecore_list_new();
   smb_fd_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
   smb_next_fd = 0;

   auth_command_list = ecore_list_new();
   last_auth = NEW(evfs_auth_cache);

   smb_context = smbc_new_context();
   smbc_option_set(smb_context, "debug_stderr", (void *) 1);
   if (smb_context != NULL)
     {
        smb_context->debug = 0;
        smb_context->callbacks.auth_fn = auth_fn;

        if (!smbc_init_context(smb_context))
          {
             printf("Error initializing samba context..\n");
             smbc_free_context(smb_context, FALSE);
             smb_context = NULL;
          }
     }

   return functions;
}

char *
evfs_plugin_uri_get()
{
   return "smb";
}

void
evfs_auth_push(evfs_command* command)
{
	evfs_auth_structure_add(evfs_command_first_file_get(command)->username, 
			evfs_command_first_file_get(command)->password,
			evfs_command_first_file_get(command)->path);
}

int 
smb_evfs_file_rename(evfs_client* client, evfs_command* command)
{
	int err;

	evfs_smb_auth_push(command, client);

	/*TODO: Check that these files are on same filesystem.
	 * This should really be a per-plugin function called from the top level*/
	err = smb_context->rename(smb_context, evfs_command_first_file_get(command)->path, 
			smb_context,
			evfs_command_second_file_get(command)->path);

	evfs_smb_auth_pop(command);
	return err;
}

int
smb_evfs_file_stat(evfs_command * command, struct stat *file_stat, int number)
{

   int err = 0;
   int fd = 0;
   char dir[PATH_MAX];
   static struct stat smb_stat;

   //struct stat* file_stat = calloc(1,sizeof(struct stat));
   //
   /*Does this command have an attached authentication object? */
   if (evfs_command_nth_file_get(command,number)->username)
     {
        printf("We have a username, adding to hash..\n");
        evfs_auth_structure_add( evfs_command_nth_file_get(command,number)->username,
                                evfs_command_nth_file_get(command,number)->password,
                                evfs_command_nth_file_get(command,number)->path);
     }

   sprintf(dir, "smb:/%s", evfs_command_nth_file_get(command,number)->path);
   //printf("Getting stat on file '%s'\n", dir);

   err = smb_context->stat(smb_context, (const char *)dir, &smb_stat);
   //printf("Returned error code: %d\n", err);
   //printf("File size: %ld\n", file_stat->st_size);

   //printf("Returning to caller..\n");

   /*Ugly as shit - but if libsmbclient is compiled
    * with a different mem packing regime, then
    * this is the only way we can safely avoid blowing
    * the stack (i.e we can't use memcpy) */
   file_stat->st_dev = smb_stat.st_dev;
   file_stat->st_ino = smb_stat.st_ino;
   file_stat->st_mode = smb_stat.st_mode;
   file_stat->st_nlink = smb_stat.st_nlink;
   file_stat->st_uid = smb_stat.st_uid;
   file_stat->st_gid = smb_stat.st_gid;
   file_stat->st_rdev = smb_stat.st_rdev;
   file_stat->st_size = smb_stat.st_size;
   file_stat->st_blksize = smb_stat.st_blksize;
   file_stat->st_blocks = smb_stat.st_blocks;
   file_stat->st_atime = smb_stat.st_atime;
   file_stat->st_mtime = smb_stat.st_mtime;
   file_stat->st_ctime = smb_stat.st_ctime;

   if (!err)
      return EVFS_SUCCESS;
   else
      return EVFS_ERROR;
}

static void
smb_evfs_dir_list(evfs_client * client, evfs_command * command,
                  /*Returns.. */
                  Ecore_List ** directory_list)
{

   char dir_path[PATH_MAX];
   int size;
   SMBCFILE *dir = NULL;
   struct smbc_dirent *entry = NULL;
   EvfsFilereference* file = evfs_command_first_file_get(command);
   
   Ecore_List *files = ecore_list_new();

   evfs_smb_auth_push(command,client);

   /*Does this command have an attached authentication object? */
   if (file->username)
     {
        printf("We have a username, adding to hash..\n");
        evfs_auth_structure_add( file->username,
                                file->password,
                                file->path);
     }

   //Reappend smb protocol header for libsmbclient..
   snprintf(dir_path, 1024, "smb:/%s", file->path);

   printf("evfs_fs_samba: Listing directory %s\n", dir_path);

   if ((dir = smb_context->opendir(smb_context, dir_path)) >= 0)
     {

        while (entry = smb_context->readdir(smb_context, dir))
          {

             /*Make sure we don't use . or .. */
             if (strcmp(entry->name, ".") && strcmp(entry->name, ".."))
               {
                  EvfsFilereference *reference = NEW(EvfsFilereference);

                  if (entry->smbc_type == SMBC_FILE)
                     reference->file_type = EVFS_FILE_NORMAL;
                  else if (entry->smbc_type == SMBC_DIR)
                     reference->file_type = EVFS_FILE_DIRECTORY;
                  else if (entry->smbc_type == SMBC_WORKGROUP)
                     reference->file_type = EVFS_FILE_DIRECTORY;
                  else if (entry->smbc_type == SMBC_SERVER)
                     reference->file_type = EVFS_FILE_DIRECTORY;
                  else if (entry->smbc_type == SMBC_FILE_SHARE)
                     reference->file_type = EVFS_FILE_DIRECTORY;
                  else if (entry->smbc_type == SMBC_PRINTER_SHARE)
                     reference->file_type = EVFS_FILE_SMB_PRINTER_SHARE;
                  else if (entry->smbc_type == SMBC_COMMS_SHARE)
                     reference->file_type = EVFS_FILE_SMB_COMMS_SHARE;
                  else if (entry->smbc_type == SMBC_IPC_SHARE)
                     reference->file_type = EVFS_FILE_SMB_IPC_SHARE;
                  else if (entry->smbc_type == SMBC_LINK)
                     reference->file_type = EVFS_FILE_LINK;

		  /*If the entry is a server, rewrite to remove the workgroup,
		   * if any*/
                  if (entry->smbc_type != SMBC_SERVER) {
			  size =
	                     (sizeof(char) *
	                      strlen(file->path)) +
	                     (sizeof(char) * strlen(entry->name)) + (sizeof(char) * 2);
	                  reference->path = malloc(size);

	                  snprintf(reference->path, size, "%s/%s",
	                           file->path, entry->name);
		  } else {
			  size = (sizeof(char) + (sizeof(char) * strlen(entry->name)) + (sizeof(char) * 2));
			   reference->path = malloc(size);

	                  snprintf(reference->path, size, "/%s",
	                           entry->name);					  
		  }

                  printf("File '%s' is of type '%d'\n", reference->path, reference->file_type);

                  reference->plugin_uri = strdup("smb");
                  ecore_list_append(files, reference);

                  ecore_main_loop_iterate();
               }
          }
        smb_context->closedir(smb_context, dir);

        /*Set the return pointer.. */
        *directory_list = evfs_file_list_sort(files);
     }
   else
     {
        printf("Could not open [%s] (%d:%s)\n", dir_path, errno,
               strerror(errno));
     }

    evfs_smb_auth_pop(command);
}

int
evfs_file_open(evfs_client * client, EvfsFilereference * file)
{
   char dir_path[1024];

   snprintf(dir_path, 1024, "smb:/%s", file->path);

   /*Does this command have an attached authentication object? */
   if (file->username)
     {
        printf("We have a username, adding to hash..\n");
        evfs_auth_structure_add(file->username, file->password,
                                file->path);
     }

   printf("Opening file '%s' in samba\n", dir_path);

   file->fd_p = smb_context->open(smb_context, dir_path, O_RDONLY, S_IRUSR);
   file->fd = smb_fd_get_next(file->fd_p);

   if (file->fd_p)
      return file->fd;
   else
      return -1;
}

int
evfs_file_remove(char *file)
{
   char file_smb[PATH_MAX];
   static struct stat file_stat;

   snprintf(file_smb, PATH_MAX, "smb:/%s", file);

   int rr = smb_context->stat(smb_context, (const char *)file_smb, &file_stat);

   if (!rr)
     {
        /*File stat successful */
        if (S_ISDIR(file_stat.st_mode))
          {
             printf("Rmdiring '%s'\n", file_smb);
             return smb_context->rmdir(smb_context, file_smb);
          }
        else
          {
             printf("Unlinking '%s'\n", file_smb);
             return smb_context->unlink(smb_context, file_smb);
          }
     }
   else
     {
        printf("Could not stat '%s'\n", file_smb);
	return 1;
     }

}

int
evfs_file_close(EvfsFilereference * file)
{
   printf("SMB close: closing\n");

#ifdef HAVE_SAMBA_OLD_CLOSE
   smb_context->close(smb_context, file->fd_p);
#else
   smb_context->close_fn(smb_context, file->fd_p);
#endif

   return 0;
}

int
evfs_file_write(EvfsFilereference * file, char *bytes, long size)
{
   ssize_t i;

   /*printf ("SMB write: %d bytes\n", size); */

   i = smb_context->write(smb_context, file->fd_p, bytes, size);
   /*printf("Wrote %d bytes\n", i); */

   return i;
}

int
evfs_file_seek(EvfsFilereference * file, long pos, int whence)
{
   //printf ("Seeking file to %ld\n", pos);

   smb_context->lseek(smb_context, file->fd_p, pos, SEEK_SET);

   return 0;
}

int
evfs_file_read(evfs_client * client, EvfsFilereference * file, char *bytes,
               long size)
{
   int bytes_read = 0;

   //printf("Reading %ld bytes from file %s\n", size, file->path);

   evfs_smb_populate_fd(file);

   if (file->fd_p) bytes_read = smb_context->read(smb_context, file->fd_p, bytes, size);
   return bytes_read;
}

int
evfs_file_create(EvfsFilereference * file)
{
   char dir_path[1024];

   snprintf(dir_path, 1024, "smb:/%s", file->path);

   /*printf("SMB File create: %s\n", dir_path);*/

   file->fd_p =
      smb_context->open(smb_context, dir_path, O_CREAT | O_TRUNC | O_RDWR,
                        S_IRUSR | S_IWUSR);
   if (file->fd_p) {
	   file->fd = smb_fd_get_next(file->fd_p);
	   return file->fd;
   } else
	   return 0;
}

int
smb_evfs_file_mkdir(EvfsFilereference * file)
{
   char dir_path[1024];

   /*Does this command have an attached authentication object? */
   if (file->username)
     {
        printf("We have a username, adding to hash..\n");
        evfs_auth_structure_add(file->username, file->password,
                                file->path);
     }

   snprintf(dir_path, 1024, "smb:/%s", file->path);

   printf("SMB File mkdir: %s\n", dir_path);

   smb_context->mkdir(smb_context, dir_path, S_IRWXU);
   return 0;

}
