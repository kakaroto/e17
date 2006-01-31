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

//static struct stat file_stat;
int smbc_remove_unused_server (SMBCCTX * context, SMBCSRV * srv);
static SMBCCTX *smb_context = NULL;
Ecore_List *auth_cache;

int smb_next_fd;
Ecore_Hash *smb_fd_hash;




static void smb_evfs_dir_list (evfs_client * client, evfs_command * command,
			       Ecore_List ** directory_list);
int smb_evfs_file_stat (evfs_command * command, struct stat *file_stat, int);
int evfs_file_open (evfs_client * client, evfs_filereference * file);
int evfs_file_close (evfs_filereference * file);
int evfs_file_seek (evfs_filereference * file, long offset, int whence);
int evfs_file_read (evfs_client * client, evfs_filereference * file,
		    char *bytes, long size);
int evfs_file_write (evfs_filereference * file, char *bytes, long size);
int evfs_file_create (evfs_filereference * file);
int smb_evfs_file_mkdir (evfs_filereference * file);
int evfs_file_remove (char *file);



int
evfs_client_disconnect (evfs_client * client)
{
  printf ("Received disconnect for client at evfs_fs_samba.c for client %d\n",
	  client->id);
}



void
evfs_auth_structure_add (Ecore_List * cache, char *username, char *password,
			 char *path)
{
  evfs_auth_cache *obj = NEW (evfs_auth_cache);
  obj->username = strdup (username);
  obj->password = strdup (password);
  obj->path = strdup (path);

  printf ("Added %s:%s to '%s' for auth\n", username, password, path);

  ecore_list_append (cache, obj);
}

evfs_auth_cache *
evfs_auth_cache_get (Ecore_List * cache, char *path)
{
  evfs_auth_cache *obj;

  printf ("Looking for match for '%s' in auth cache\n", path);

  ecore_list_goto_first (cache);
  while ((obj = ecore_list_next (cache)))
    {
      if (!strncmp (obj->path, path, strlen (path)))
	{
	  printf ("Found match for path in cache: user '%s'\n",
		  obj->username);
	  return obj;
	}
    }

  return NULL;
}


int
smb_fd_get_next (void *ptr)
{
  smb_next_fd++;
  ecore_hash_set (smb_fd_hash, (int *) smb_next_fd, ptr);

  printf ("Wrote ptr %p for fd %d\n", ptr, smb_next_fd);

  return smb_next_fd;
}

void *
smb_fd_get_for_int (int fd)
{
  return ecore_hash_get (smb_fd_hash, (int *) fd);
}


void
evfs_smb_populate_fd (evfs_filereference * file)
{
  if (file->fd_p == NULL)
    {
      file->fd_p = smb_fd_get_for_int (file->fd);
    }
}


/*---------------------------------------------------------------------------*/
/*A temporary function until we build authentication into the protocol*/
void
auth_fn (const char *server, const char *share,
	 char *workgroup, int wgmaxlen, char *username, int unmaxlen,
	 char *password, int pwmaxlen)
{
  char temp[128];
  char path[PATH_MAX];
  evfs_auth_cache *obj;

  fprintf (stdout, "Need password for //%s/%s\n", server, share);
  snprintf (path, 512, "/%s/%s", server, share);

  /*Look for a cached auth for this */
  obj = evfs_auth_cache_get (auth_cache, path);
  if (obj)
    {
      strncpy (username, obj->username, unmaxlen);
      strncpy (password, obj->password, pwmaxlen);
      return;
    }

  fprintf (stdout, "Enter workgroup: [%s] ", workgroup);
  fgets (temp, sizeof (temp), stdin);

  if (temp[strlen (temp) - 1] == 0x0a)	/* A new line? */
    temp[strlen (temp) - 1] = 0x00;

  if (temp[0])
    strncpy (workgroup, temp, wgmaxlen - 1);

  fprintf (stdout, "Enter username: [%s] ", username);
  fgets (temp, sizeof (temp), stdin);

  if (temp[strlen (temp) - 1] == 0x0a)	/* A new line? */
    temp[strlen (temp) - 1] = 0x00;

  if (temp[0])
    strncpy (username, temp, unmaxlen - 1);

  fprintf (stdout, "Enter password: [%s] ", password);
  fgets (temp, sizeof (temp), stdin);

  if (temp[strlen (temp) - 1] == 0x0a)	/* A new line? */
    temp[strlen (temp) - 1] = 0x00;

  if (temp[0])
    strncpy (password, temp, pwmaxlen - 1);

}

/*------------------------------------------------------------------------*/


evfs_plugin_functions *
evfs_plugin_init ()
{
  int err;


  printf ("Initialising the samba plugin..\n");
  evfs_plugin_functions *functions = malloc (sizeof (evfs_plugin_functions));
  functions->evfs_dir_list = &smb_evfs_dir_list;
  functions->evfs_client_disconnect = &evfs_client_disconnect;
  functions->evfs_file_open = &evfs_file_open;
  functions->evfs_file_close = &evfs_file_close;
  functions->evfs_file_seek = &evfs_file_seek;
  functions->evfs_file_read = &evfs_file_read;
  functions->evfs_file_write = &evfs_file_write;
  functions->evfs_file_create = &evfs_file_create;
  functions->evfs_file_stat = &smb_evfs_file_stat;
  functions->evfs_file_lstat = &smb_evfs_file_stat;	/*Windows file systems have no concept
							   of 'lstat' */


  functions->evfs_file_mkdir = &smb_evfs_file_mkdir;
  functions->evfs_file_remove = &evfs_file_remove;
  printf ("Samba stat func at '%p'\n", &smb_evfs_file_stat);

  auth_cache = ecore_list_new ();
  smb_fd_hash = ecore_hash_new (ecore_direct_hash, ecore_direct_compare);
  smb_next_fd = 0;

  //Initialize samba (temporarily borrowed from gnomevfs)
  smb_context = smbc_new_context ();
  if (smb_context != NULL)
    {
      smb_context->debug = 0;
      smb_context->callbacks.auth_fn = auth_fn;

      if (!smbc_init_context (smb_context))
	{
	  printf ("Error initializing samba context..\n");
	  smbc_free_context (smb_context, FALSE);
	  smb_context = NULL;
	}
    }



  return functions;
}

char *
evfs_plugin_uri_get ()
{
  return "smb";
}

int
smb_evfs_file_stat (evfs_command * command, struct stat *file_stat,
		    int number)
{

  int err = 0;
  int fd = 0;
  char dir[PATH_MAX];
  static struct stat smb_stat;

  //struct stat* file_stat = calloc(1,sizeof(struct stat));
  //
  /*Does this command have an attached authentication object? */
  if (command->file_command.files[0]->username)
    {
      printf ("We have a username, adding to hash..\n");
      evfs_auth_structure_add (auth_cache,
			       command->file_command.files[number]->username,
			       command->file_command.files[number]->password,
			       command->file_command.files[number]->path);
    }


  sprintf (dir, "smb:/%s", command->file_command.files[number]->path);
  //printf("Getting stat on file '%s'\n", dir);

  err = smb_context->stat (smb_context, (const char *) dir, &smb_stat);
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
smb_evfs_dir_list (evfs_client * client, evfs_command * command,
		   /*Returns.. */
		   Ecore_List ** directory_list)
{

  char dir_path[PATH_MAX];

  int fd, dh1, dh2, dh3, dsize, dirc;
  int size;
  char dirbuf[8192];
  char *dirp;
  SMBCFILE *dir = NULL;
  struct smbc_dirent *entry = NULL;
  Ecore_List *files = ecore_list_new ();

  /*Does this command have an attached authentication object? */
  if (command->file_command.files[0]->username)
    {
      printf ("We have a username, adding to hash..\n");
      evfs_auth_structure_add (auth_cache,
			       command->file_command.files[0]->username,
			       command->file_command.files[0]->password,
			       command->file_command.files[0]->path);
    }

  //Reappend smb protocol header for libsmbclient..
  snprintf (dir_path, 1024, "smb:/%s", command->file_command.files[0]->path);

  printf ("evfs_fs_samba: Listing directory %s\n", dir_path);


  if ((dir = smb_context->opendir (smb_context, dir_path)) >= 0)
    {

      while (entry = smb_context->readdir (smb_context, dir))
	{

	  /*Make sure we don't use . or .. */
	  if (strcmp (entry->name, ".") && strcmp (entry->name, ".."))
	    {
	      evfs_filereference *reference = NEW (evfs_filereference);

	      if (entry->smbc_type == SMBC_FILE)
		reference->file_type = EVFS_FILE_NORMAL;
	      else if (entry->smbc_type == SMBC_DIR)
		reference->file_type = EVFS_FILE_DIRECTORY;
	      else if (entry->smbc_type == SMBC_WORKGROUP)
		reference->file_type = EVFS_FILE_SMB_WORKGROUP;
	      else if (entry->smbc_type == SMBC_SERVER)
		reference->file_type = EVFS_FILE_SMB_SERVER;
	      else if (entry->smbc_type == SMBC_FILE_SHARE)
		reference->file_type = EVFS_FILE_SMB_FILE_SHARE;
	      else if (entry->smbc_type == SMBC_PRINTER_SHARE)
		reference->file_type = EVFS_FILE_SMB_PRINTER_SHARE;
	      else if (entry->smbc_type == SMBC_COMMS_SHARE)
		reference->file_type = EVFS_FILE_SMB_COMMS_SHARE;
	      else if (entry->smbc_type == SMBC_IPC_SHARE)
		reference->file_type = EVFS_FILE_SMB_IPC_SHARE;
	      else if (entry->smbc_type == SMBC_LINK)
		reference->file_type = EVFS_FILE_LINK;

	      size =
		(sizeof (char) *
		 strlen (command->file_command.files[0]->path)) +
		(sizeof (char) * strlen (entry->name)) + (sizeof (char) * 2);
	      reference->path = malloc (size);

	      snprintf (reference->path, size, "%s/%s",
			command->file_command.files[0]->path, entry->name);

	      /*printf("File '%s' is of type '%d'\n", reference->path, reference->file_type); */


	      reference->plugin_uri = strdup ("smb");
	      ecore_list_append (files, reference);

	      ecore_main_loop_iterate ();
	    }
	}
      smb_context->closedir (smb_context, dir);

      /*Set the return pointer.. */
      *directory_list = evfs_file_list_sort (files);
    }
  else
    {
      printf ("Could not open [%s] (%d:%s)\n", dir_path, errno,
	      strerror (errno));
    }



}


int
evfs_file_open (evfs_client * client, evfs_filereference * file)
{
  char dir_path[1024];
  snprintf (dir_path, 1024, "smb:/%s", file->path);

  /*Does this command have an attached authentication object? */
  if (file->username)
    {
      printf ("We have a username, adding to hash..\n");
      evfs_auth_structure_add (auth_cache, file->username, file->password,
			       file->path);
    }

  printf ("Opening file '%s' in samba\n", dir_path);

  file->fd_p = smb_context->open (smb_context, dir_path, O_RDONLY, S_IRUSR);
  file->fd = smb_fd_get_next (file->fd_p);

  return file->fd;
}

int
evfs_file_remove (char *file)
{
  char file_smb[PATH_MAX];
  static struct stat file_stat;

  snprintf (file_smb, PATH_MAX, "smb:/%s", file);


  int rr =
    smb_context->stat (smb_context, (const char *) file_smb, &file_stat);
  if (!rr)
    {
      /*File stat successful */
      if (S_ISDIR (file_stat.st_mode))
	{
	  printf ("Rmdiring '%s'\n", file_smb);
	  return smb_context->rmdir (smb_context, file_smb);
	}
      else
	{
	  printf ("Unlinking '%s'\n", file_smb);
	  return smb_context->unlink (smb_context, file_smb);
	}
    }
  else
    {
      printf ("Could not stat '%s'\n", file_smb);
    }


}

int
evfs_file_close (evfs_filereference * file)
{
  printf ("SMB close: closing\n");

#ifdef HAVE_SAMBA_OLD_CLOSE
  smb_context->close (smb_context, file->fd_p);
#else
  smb_context->close_fn (smb_context, file->fd_p);
#endif

  return 0;
}

int
evfs_file_write (evfs_filereference * file, char *bytes, long size)
{
  ssize_t i;

  /*printf ("SMB write: %d bytes\n", size); */

  i = smb_context->write (smb_context, file->fd_p, bytes, size);
  /*printf("Wrote %d bytes\n", i); */

  return 0;
}

int
evfs_file_seek (evfs_filereference * file, long pos, int whence)
{
  //printf ("Seeking file to %ld\n", pos);

  smb_context->lseek (smb_context, file->fd_p, pos, SEEK_SET);

  return 0;
}

int
evfs_file_read (evfs_client * client, evfs_filereference * file, char *bytes,
		long size)
{
  int bytes_read = 0;
  //printf("Reading %ld bytes from file %s\n", size, file->path);

  evfs_smb_populate_fd (file);
  //printf("FD Pointer: %p\n", file->fd_p);

  bytes_read = smb_context->read (smb_context, file->fd_p, bytes, size);
  return bytes_read;
}



int
evfs_file_create (evfs_filereference * file)
{
  char dir_path[1024];
  snprintf (dir_path, 1024, "smb:/%s", file->path);

  printf ("SMB File create: %s\n", dir_path);

  file->fd_p =
    smb_context->open (smb_context, dir_path, O_CREAT | O_TRUNC | O_RDWR,
		       S_IRUSR | S_IWUSR);
  return 0;
}


int
smb_evfs_file_mkdir (evfs_filereference * file)
{
  char dir_path[1024];
  snprintf (dir_path, 1024, "smb:/%s", file->path);

  printf ("SMB File mkdir: %s\n", dir_path);

  smb_context->mkdir (smb_context, dir_path, S_IRWXU);
  return 0;

}
