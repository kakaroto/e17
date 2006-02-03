#include "evfs.h"
#include <dlfcn.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>


/*---------------------------------------------------*/
/*Move these functions somewhere*/

int
evfs_uri_open (evfs_client * client, evfs_filereference * uri)
{
  evfs_plugin *plugin =
    evfs_get_plugin_for_uri (client->server, uri->plugin_uri);
  if (plugin) {
    printf ("Opening file..\n");
    return (*plugin->functions->evfs_file_open) (client, uri);
  }
  else {
    printf ("Could not get plugin for uri '%s' at evfs_uri_open\n",
	    uri->plugin_uri);
    return -1;
  }
}

int
evfs_uri_close (evfs_client * client, evfs_filereference * uri)
{
  evfs_plugin *plugin =
    evfs_get_plugin_for_uri (client->server, uri->plugin_uri);
  if (plugin) {
    printf ("Closing file..\n");
    return (*plugin->functions->evfs_file_close) (uri);
  }
  else {
    printf ("Could not get plugin for uri '%s' at evfs_uri_close\n",
	    uri->plugin_uri);
    return -1;
  }

}


int
evfs_uri_read (evfs_client * client, evfs_filereference * uri, char *bytes,
	       long size)
{
  evfs_plugin *plugin =
    evfs_get_plugin_for_uri (client->server, uri->plugin_uri);
  if (plugin) {
    return (*plugin->functions->evfs_file_read) (client, uri, bytes, size);
  }
  else {
    printf ("Could not get plugin for uri '%s' at evfs_uri_open\n",
	    uri->plugin_uri);
    return -1;
  }
}

/*---------------------------------------------------*/








void
evfs_handle_monitor_start_command (evfs_client * client,
				   evfs_command * command)
{
  /*First get the plugin responsible for this file */


  if (command->file_command.num_files > 0) {
    evfs_plugin *plugin = evfs_get_plugin_for_uri (client->server,
						   command->file_command.
						   files[0]->plugin_uri);

    if (!plugin) {
      printf ("No plugin able to monitor this uri type\n");
    }
    else {
      printf
	("Requesting a file monitor from this plugin for uri type '%s'\n",
	 command->file_command.files[0]->plugin_uri);
      (*plugin->functions->evfs_monitor_start) (client, command);

    }
  }
  else {
    printf ("No files to monitor!\n");
  }
}

void
evfs_handle_monitor_stop_command (evfs_client * client,
				  evfs_command * command)
{
  /*First get the plugin responsible for this file */

  void (*evfs_monitor_start) (evfs_client * client, evfs_command * command);

  if (command->file_command.num_files > 0) {
    evfs_plugin *plugin = evfs_get_plugin_for_uri (client->server,
						   command->file_command.
						   files[0]->plugin_uri);

    if (!plugin) {
      printf ("No plugin able to monitor this uri type\n");
    }
    else {
      printf
	("Requesting a file monitor end from this plugin for uri type '%s'\n",
	 command->file_command.files[0]->plugin_uri);

      evfs_monitor_start = dlsym (plugin->dl_ref, EVFS_FUNCTION_MONITOR_STOP);
      if (evfs_monitor_start) {
	(*evfs_monitor_start) (client, command);
      }

    }
  }
  else {
    printf ("No files to monitor!\n");
  }
}

void
evfs_handle_file_remove_command (evfs_client * client, evfs_command * command)
{
  struct stat file_stat;

  //printf("At remove handle for %s\n", command->file_command.files[0]->path );

  evfs_plugin *plugin = evfs_get_plugin_for_uri (client->server,
						 command->file_command.
						 files[0]->plugin_uri);
  if (plugin) {
    (*plugin->functions->evfs_file_lstat) (command, &file_stat, 0);
    //printf("ST_MODE: %d\n", file_stat.st_mode);

    //printf("Pointer here: %p\n", plugin->functions->evfs_file_remove);

    /*If we're not a dir, simple remove command */
    if (!S_ISDIR (file_stat.st_mode)) {

      (*plugin->functions->evfs_file_remove) (command->file_command.
					      files[0]->path);
      //printf("REMOVE FIL: '%s'\n", command->file_command.files[0]->path);
      //
      /*Iterate */
      ecore_main_loop_iterate ();

    }
    else {
      //printf("IS LINK RES: %d, for %s\n", S_ISLNK(file_stat.st_mode), command->file_command.files[0]->path);

      if (!S_ISLNK (file_stat.st_mode)) {

	/*It's a directory, recurse into it */
	Ecore_List *directory_list = NULL;
	evfs_filereference *file = NULL;

	/*First, we need a directory list... */
	(*plugin->functions->evfs_dir_list) (client, command,
					     &directory_list);
	if (directory_list) {
	  while ((file = ecore_list_remove_first (directory_list))) {
	    evfs_command *recursive_command = NEW (evfs_command);

	    recursive_command->file_command.files =
	      malloc (sizeof (evfs_filereference *) * 1);
	    recursive_command->type = EVFS_CMD_REMOVE_FILE;
	    recursive_command->file_command.files[0] = file;
	    recursive_command->file_command.num_files = 1;

	    evfs_handle_file_remove_command (client, recursive_command);
	    evfs_cleanup_command (recursive_command,
				  EVFS_CLEANUP_FREE_COMMAND);

	  }
	}

	//printf("REMOVE DIR: '%s'\n", command->file_command.files[0]->path);
	(*plugin->functions->evfs_file_remove) (command->file_command.
						files[0]->path);
      }
      else {
	printf ("Not recursing - LINK directory!\n");
      }


    }
  }
  else {
    printf ("No plugin!\n");
  }
}


void
evfs_handle_file_rename_command (evfs_client * client, evfs_command * command)
{
  printf ("At rename handle\n");

  evfs_plugin *plugin = evfs_get_plugin_for_uri (client->server,
						 command->file_command.
						 files[0]->plugin_uri);
  if (plugin) {
    printf ("Pointer here: %p\n", plugin->functions->evfs_file_rename);

    if (plugin->functions->evfs_file_rename) {
      if (command->file_command.num_files == 2)
	(*plugin->functions->evfs_file_rename) (client, command);
      else
	printf ("ERR: Wrong number of files to rename\n");
    }
    else
      printf ("Rename not supported\n");
  }
}

void
evfs_handle_file_stat_command (evfs_client * client, evfs_command * command)
{
  struct stat file_stat;

  printf ("At file stat handler\n");
  printf ("Looking for plugin for '%s'\n",
	  command->file_command.files[0]->plugin_uri);
  evfs_plugin *plugin = evfs_get_plugin_for_uri (client->server,
						 command->file_command.
						 files[0]->plugin_uri);
  if (plugin) {
    printf ("Pointer here: %p\n", plugin->functions->evfs_file_stat);
    (*(plugin->functions->evfs_file_stat)) (command, &file_stat, 0);



    evfs_stat_event_create (client, command, &file_stat);
  }
  printf ("Handled event, client is %p\n", client);

}

void
evfs_handle_file_open_command (evfs_client * client, evfs_command * command)
{

  printf ("At file open handler\n");
  printf ("Looking for plugin for '%s'\n",
	  command->file_command.files[0]->plugin_uri);
  evfs_plugin *plugin = evfs_get_plugin_for_uri (client->server,
						 command->file_command.
						 files[0]->plugin_uri);
  if (plugin) {
    printf ("Pointer here: %p\n", plugin->functions->evfs_file_open);
    (*(plugin->functions->evfs_file_open)) (client,
					    command->file_command.files[0]);


    fprintf (stderr, "Opened file, fd is: %d\n",
	     command->file_command.files[0]->fd);
    evfs_open_event_create (client, command);
  }
  printf ("Handled event, client is %p\n", client);

}


void
evfs_handle_file_read_command (evfs_client * client, evfs_command * command)
{
  char *bytes;
  int b_read = 0;


  //printf ("At file read handler, fd is: %d\n", command->file_command.files[0]->fd);
  //printf ("Reading %d bytes\n", command->file_command.extra);

  bytes = malloc (sizeof (char) * command->file_command.extra);

  //printf("Looking for plugin for '%s'\n", command->file_command.files[0]->plugin_uri);
  evfs_plugin *plugin = evfs_get_plugin_for_uri (client->server,
						 command->file_command.
						 files[0]->plugin_uri);
  if (plugin) {
    //printf("Pointer here: %p\n", plugin->functions->evfs_file_read);
    b_read =
      (*(plugin->functions->evfs_file_read)) (client,
					      command->file_command.
					      files[0], bytes,
					      command->file_command.extra);

    //printf("Bytes read: %d\n", b_read);


    evfs_read_event_create (client, command, bytes, b_read);
  }
  //printf("Handled event, client is %p\n", client);

}



void
evfs_handle_dir_list_command (evfs_client * client, evfs_command * command)
{


  evfs_plugin *plugin = evfs_get_plugin_for_uri (client->server,
						 command->file_command.
						 files[0]->plugin_uri);
  if (plugin) {
    Ecore_List *directory_list = NULL;
    (*plugin->functions->evfs_dir_list) (client, command, &directory_list);

    if (directory_list) {
      evfs_list_dir_event_create (client, command, directory_list);
    }
    else {
      printf
	("evfs_handle_dir_list_command: Recevied null from plugin for directory_list\n");
    }
  }
  else {
    printf ("No plugin for '%s'\n",
	    command->file_command.files[0]->plugin_uri);
  }

}


/* TODO:
 * 1. check if file exists before open/write.  if so, wait-and-lock for user
 *    choice
 * 2. in main loop, if file read bytes < expected, delete file, and retry a 
 *    preset number of times, before error.
 */
void
evfs_handle_file_copy (evfs_client * client, evfs_command * command,
		       evfs_command * root_command)
{
  evfs_plugin *plugin;
  evfs_plugin *dst_plugin;

  char bytes[COPY_BLOCKSIZE];
  long count;
  char destination_file[PATH_MAX];
  long read_write_bytes = 0;

  struct stat file_stat;
  struct stat dest_stat;

  int progress = 0;
  int last_notify_progress = 0;
  int b_read = 0, b_write = 0;
  int res;
  evfs_operation *op;

  /*Make a new evfs_operation, for client communication */
  op = evfs_operation_new ();


  plugin =
    evfs_get_plugin_for_uri (client->server,
			     command->file_command.files[0]->plugin_uri);
  dst_plugin =
    evfs_get_plugin_for_uri (client->server,
			     command->file_command.files[1]->plugin_uri);



  if (plugin && dst_plugin) {

    /*Check for supported options */
    if (!(plugin->functions->evfs_file_lstat &&
	  plugin->functions->evfs_file_open &&
	  dst_plugin->functions->evfs_file_create)) {
      printf ("AHH! Copy Not supported!\n");
      evfs_operation_destroy (op);
      return;
    }


    /*Get the source file size */
    (*plugin->functions->evfs_file_lstat) (command, &file_stat, 0);
    res = (*dst_plugin->functions->evfs_file_lstat) (command, &dest_stat, 1);

    /*If this file exists, lock our operation until we get confirmation
     * from the user that they want to overwrite, or not */
    if (res != EVFS_ERROR) {
      printf ("File overwrite\n");
      evfs_operation_status_set (op, EVFS_OPERATION_STATUS_USER_WAIT);

      evfs_operation_user_dispatch (client, command, op);
      while (op->status == EVFS_OPERATION_STATUS_USER_WAIT) {
	ecore_main_loop_iterate ();
	usleep (1);
      }

      if (op->response == EVFS_OPERATION_RESPONSE_NEGATE) {
	      printf("User opted not to overwrite file!\n");
	      goto CLEANUP;
      } else {
	      printf("User opted to overwrite file!\n");
      }
    }

    if (!S_ISDIR (file_stat.st_mode)) {
      int fd = (*plugin->functions->evfs_file_open) (client,
					    command->file_command.files[0]);

      if (fd <= 0) {
	      printf("*************************  Could not open file!\n");
	      goto CLEANUP;
      }
	    
      (*dst_plugin->functions->evfs_file_create) (command->file_command.
						  files[1]);


      count = 0;
      while (count < file_stat.st_size) {
	//(*plugin->functions->evfs_file_seek)(command->file_command.files[0], count, SEEK_SET);

	read_write_bytes =
	  (file_stat.st_size >
	   count +
	   COPY_BLOCKSIZE) ? COPY_BLOCKSIZE : (file_stat.st_size - count);

	//printf("Reading/writing %d bytes\n", read_write_bytes);

	//TODO: Implement error checking here
	b_read =
	  (*plugin->functions->evfs_file_read) (client,
						command->file_command.
						files[0], bytes,
						read_write_bytes);
	if (b_read > 0) {
	  b_write =
	    (*dst_plugin->functions->evfs_file_write) (command->
						       file_command.
						       files[1],
						       bytes, b_read);
	  //printf("%d  -> %d\n", b_read, b_write);
	}
	count += b_read;



	progress =
	  (double) ((double) count / (double) file_stat.st_size * 100);
	if (progress % 1 == 0 && last_notify_progress < progress) {
	  /*printf ("Percent complete: %d\n", progress); */
	  evfs_file_progress_event_create (client, command,
					   root_command, progress,
					   EVFS_PROGRESS_TYPE_CONTINUE);
	  last_notify_progress = progress;
	}

	//printf("Bytes to go: %d\n", file_stat.st_size - count);



	/*Iterate */
	ecore_main_loop_iterate ();
      }
      (*dst_plugin->functions->evfs_file_close) (command->file_command.
						 files[1]);
      (*plugin->functions->evfs_file_close) (command->file_command.files[0]);
    }
    else {
      Ecore_List *directory_list = NULL;

      /*First, we need a directory list... */
      (*plugin->functions->evfs_dir_list) (client, command, &directory_list);
      if (directory_list) {
	int ret = 0;
	evfs_filereference *file = NULL;

	/*OK, so the directory exists at the source, and contains files.
	 * Let's make the destination directory first..*/
	printf ("Making new directory '%s'",
		command->file_command.files[1]->path);
	ret =
	  (*dst_plugin->functions->evfs_file_mkdir) (command->
						     file_command.files[1]);
	printf ("....ret was %d\n", ret);


	//printf("Recursive directory list for '%s' received..\n", command->file_command.files[0]->path);
	while ((file = ecore_list_remove_first (directory_list))) {
	  evfs_filereference *source = NEW (evfs_filereference);
	  evfs_filereference *dest = NEW (evfs_filereference);
	  evfs_command *recursive_command = NEW (evfs_command);



	  snprintf (destination_file, PATH_MAX, "%s%s",
		    command->file_command.files[1]->path,
		    strrchr (file->path, '/'));

	  source->path = strdup (file->path);
	  source->plugin_uri =
	    strdup (command->file_command.files[0]->plugin_uri);
	  source->parent = NULL;	/*TODO - handle nested uris */
	  dest->path = strdup (destination_file);
	  dest->plugin_uri =
	    strdup (command->file_command.files[1]->plugin_uri);
	  dest->parent = NULL;	/*TODO - handle nested uris */

	  recursive_command->file_command.files =
	    malloc (sizeof (evfs_filereference *) * 2);
	  recursive_command->type = EVFS_CMD_FILE_COPY;
	  recursive_command->file_command.files[0] = source;
	  recursive_command->file_command.files[1] = dest;
	  recursive_command->file_command.num_files = 2;

	  //printf("Copy file '%s' to %s\n", file->path, destination_file);

	  evfs_handle_file_copy (client, recursive_command, root_command);

	  evfs_cleanup_filereference (file);
	  evfs_cleanup_command (recursive_command, EVFS_CLEANUP_FREE_COMMAND);
	}
	ecore_list_destroy (directory_list);

      }


    }

    /*Only send '100%' event when we're back at the top, or we aren't recursive */
    if (command == root_command)
      evfs_file_progress_event_create (client, command, root_command, 100,
				       EVFS_PROGRESS_TYPE_DONE);


  }
  else {
    printf ("Could not get plugins for both source and dest: (%s:%s)\n",
	    command->file_command.files[0]->plugin_uri,
	    command->file_command.files[1]->plugin_uri);
  }

  CLEANUP:
  evfs_operation_destroy (op);

}

void
evfs_handle_ping_command (evfs_client * client, evfs_command * command)
{
  printf ("Ping!\n");
}


void evfs_handle_operation_command(evfs_client* client, evfs_command* command) {
	evfs_operation* op_get = NULL;

	op_get = evfs_operation_get_by_id(command->op->id);
	if (op_get) {
		op_get->status = EVFS_OPERATION_STATUS_NORMAL;
		op_get->response = command->op->response;
		printf("*** Received operation response for op %ld -> %d\n", command->op->id, command->op->response);
	}
}
