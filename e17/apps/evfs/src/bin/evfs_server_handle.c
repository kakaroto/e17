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
evfs_uri_open(evfs_client * client, evfs_filereference * uri)
{
   evfs_plugin *plugin =
      evfs_get_plugin_for_uri(client->server, uri->plugin_uri);
   if (plugin)
     {
        printf("Opening file..\n");
        return (*EVFS_PLUGIN_FILE(plugin)->functions->evfs_file_open) (client, uri);
     }
   else
     {
        printf("Could not get plugin for uri '%s' at evfs_uri_open\n",
               uri->plugin_uri);
        return -1;
     }
}

int
evfs_uri_close(evfs_client * client, evfs_filereference * uri)
{
   evfs_plugin *plugin =
      evfs_get_plugin_for_uri(client->server, uri->plugin_uri);
   if (plugin)
     {
        printf("Closing file..\n");
        return (*EVFS_PLUGIN_FILE(plugin)->functions->evfs_file_close) (uri);
     }
   else
     {
        printf("Could not get plugin for uri '%s' at evfs_uri_close\n",
               uri->plugin_uri);
        return -1;
     }

}

int
evfs_uri_read(evfs_client * client, evfs_filereference * uri, char *bytes,
              long size)
{
   evfs_plugin *plugin =
      evfs_get_plugin_for_uri(client->server, uri->plugin_uri);
   if (plugin)
     {
        return (*EVFS_PLUGIN_FILE(plugin)->functions->evfs_file_read) (client, uri, bytes, size);
     }
   else
     {
        printf("Could not get plugin for uri '%s' at evfs_uri_open\n",
               uri->plugin_uri);
        return -1;
     }
}

/*---------------------------------------------------*/

void
evfs_handle_monitor_start_command(evfs_client * client, evfs_command * command)
{
   /*First get the plugin responsible for this file */

   if (command->file_command.num_files > 0)
     {
        evfs_plugin *plugin = evfs_get_plugin_for_uri(client->server,
                                                      command->file_command.
                                                      files[0]->plugin_uri);

        if (!plugin)
          {
             printf("No plugin able to monitor this uri type\n");
          }
        else
          {
             printf
                ("Requesting a file monitor from this plugin for uri type '%s'\n",
                 command->file_command.files[0]->plugin_uri);
             (*EVFS_PLUGIN_FILE(plugin)->functions->evfs_monitor_start) (client, command);

          }
     }
   else
     {
        printf("No files to monitor!\n");
     }
}

void
evfs_handle_monitor_stop_command(evfs_client * client, evfs_command * command)
{
   /*First get the plugin responsible for this file */

   void (*evfs_monitor_start) (evfs_client * client, evfs_command * command);

   if (command->file_command.num_files > 0)
     {
        evfs_plugin *plugin = evfs_get_plugin_for_uri(client->server,
                                                      command->file_command.
                                                      files[0]->plugin_uri);

        if (!plugin)
          {
             printf("No plugin able to monitor this uri type\n");
          }
        else
          {
             printf
                ("Requesting a file monitor end from this plugin for uri type '%s'\n",
                 command->file_command.files[0]->plugin_uri);

             evfs_monitor_start =
                dlsym(plugin->dl_ref, EVFS_FUNCTION_MONITOR_STOP);
             if (evfs_monitor_start)
               {
                  (*evfs_monitor_start) (client, command);
               }

          }
     }
   else
     {
        printf("No files to monitor!\n");
     }
}

void
evfs_handle_file_remove_command(evfs_client * client, evfs_command * command, evfs_command* root_command)
{
   struct stat file_stat;
   evfs_operation_files* op;

   if (root_command == command) {
	   op = evfs_operation_files_new(client, root_command);
	   command->op = EVFS_OPERATION(op);
   } else {
	   op = EVFS_OPERATION_FILES(root_command->op);
   }


   evfs_plugin *plugin = evfs_get_plugin_for_uri(client->server,
                                                 command->file_command.
                                                 files[0]->plugin_uri);
   if (plugin)
     {
        (*EVFS_PLUGIN_FILE(plugin)->functions->evfs_file_lstat) (command, &file_stat, 0);

        /*If we're not a dir, simple remove command */
        if (!S_ISDIR(file_stat.st_mode))
          {

		  evfs_operation_remove_task_add(EVFS_OPERATION(op), 
		       evfs_filereference_clone(command->file_command.files[0]),
		       file_stat);		
		  
             /*Iterate */
             ecore_main_loop_iterate();

          }
        else
          {
             if (!S_ISLNK(file_stat.st_mode))
               {

                  /*It's a directory, recurse into it */
                  Ecore_List *directory_list = NULL;
                  evfs_filereference *file = NULL;

                  /*First, we need a directory list... */
                  (*EVFS_PLUGIN_FILE(plugin)->functions->evfs_dir_list) (client, command->file_command.files[0],
                                                       &directory_list);
                  if (directory_list)
                    {
                       while ((file = ecore_list_remove_first(directory_list)))
                         {
                            evfs_command *recursive_command = NEW(evfs_command);

                            recursive_command->file_command.files =
                               malloc(sizeof(evfs_filereference *) * 1);
                            recursive_command->type = EVFS_CMD_REMOVE_FILE;
                            recursive_command->file_command.files[0] = file;
                            recursive_command->file_command.num_files = 1;

                            evfs_handle_file_remove_command(client,
                                                            recursive_command, root_command);
                            evfs_cleanup_command(recursive_command,
                                                 EVFS_CLEANUP_FREE_COMMAND);

                         }
                    }

		  evfs_operation_remove_task_add(EVFS_OPERATION(op), 
		       evfs_filereference_clone(command->file_command.files[0]),
		       file_stat);	

               }
             else
               {
                  printf("Not recursing - LINK directory!\n");
               }

          }


        if (command == root_command) {
           /*evfs_file_progress_event_create(client, command, root_command, 100,
                                           EVFS_PROGRESS_TYPE_DONE);*/

	   evfs_operation_tasks_print(EVFS_OPERATION(op));

	   evfs_operation_queue_pending_add(EVFS_OPERATION(op));

	}
     }
   else
     {
        printf("No plugin!\n");
     }
}

void
evfs_handle_file_rename_command(evfs_client * client, evfs_command * command)
{
   printf("At rename handle\n");

   evfs_plugin *plugin = evfs_get_plugin_for_uri(client->server,
                                                 command->file_command.
                                                 files[0]->plugin_uri);
   if (plugin)
     {
        printf("Pointer here: %p\n", EVFS_PLUGIN_FILE(plugin)->functions->evfs_file_rename);

        if (EVFS_PLUGIN_FILE(plugin)->functions->evfs_file_rename)
          {
             if (command->file_command.num_files == 2)
                (*EVFS_PLUGIN_FILE(plugin)->functions->evfs_file_rename) (client, command);
             else
                printf("ERR: Wrong number of files to rename\n");
          }
        else
           printf("Rename not supported\n");
     }
}

void
evfs_handle_file_stat_command(evfs_client * client, evfs_command * command)
{
   struct stat file_stat;

   evfs_plugin *plugin = evfs_get_plugin_for_uri(client->server,
                                                 command->file_command.
                                                 files[0]->plugin_uri);
   if (plugin && EVFS_PLUGIN_FILE(plugin)->functions->evfs_file_stat )
     {
        (*(EVFS_PLUGIN_FILE(plugin)->functions->evfs_file_stat)) (command, &file_stat, 0);

	/*printf("Size: %lld for '%s'\n", file_stat.st_size, command->file_command.files[0]->path);*/

        evfs_stat_event_create(client, command, &file_stat);
     }
}

void
evfs_handle_file_open_command(evfs_client * client, evfs_command * command)
{

   printf("At file open handler\n");
   printf("Looking for plugin for '%s'\n",
          command->file_command.files[0]->plugin_uri);
   evfs_plugin *plugin = evfs_get_plugin_for_uri(client->server,
                                                 command->file_command.
                                                 files[0]->plugin_uri);
   if (plugin)
     {
        printf("Pointer here: %p\n", EVFS_PLUGIN_FILE(plugin)->functions->evfs_file_open);
        (*(EVFS_PLUGIN_FILE(plugin)->functions->evfs_file_open)) (client,
                                                command->file_command.files[0]);

        fprintf(stderr, "Opened file, fd is: %d\n",
                command->file_command.files[0]->fd);
        evfs_open_event_create(client, command);
     }

}

void
evfs_handle_file_read_command(evfs_client * client, evfs_command * command)
{
   char *bytes;
   int b_read = 0;

   //printf ("At file read handler, fd is: %d\n", command->file_command.files[0]->fd);
   //printf ("Reading %d bytes\n", command->file_command.extra);

   bytes = malloc(sizeof(char) * command->file_command.extra);

   //printf("Looking for plugin for '%s'\n", command->file_command.files[0]->plugin_uri);
   evfs_plugin *plugin = evfs_get_plugin_for_uri(client->server,
                                                 command->file_command.
                                                 files[0]->plugin_uri);
   if (plugin)
     {
        //printf("Pointer here: %p\n", EVFS_PLUGIN_FILE(plugin)->functions->evfs_file_read);
        b_read =
           (*(EVFS_PLUGIN_FILE(plugin)->functions->evfs_file_read)) (client,
                                                   command->file_command.
                                                   files[0], bytes,
                                                   command->file_command.extra);

        //printf("Bytes read: %d\n", b_read);

        evfs_read_event_create(client, command, bytes, b_read);
     }
   //printf("Handled event, client is %p\n", client);

}

void
evfs_handle_dir_list_command(evfs_client * client, evfs_command * command)
{

   evfs_plugin *plugin = evfs_get_plugin_for_uri(client->server,
                                                 command->file_command.
                                                 files[0]->plugin_uri);
   if (plugin) {
        Ecore_List *directory_list = NULL;
        (*EVFS_PLUGIN_FILE(plugin)->functions->evfs_dir_list) (client, command->file_command.files[0], &directory_list);
        if (directory_list) {
             evfs_list_dir_event_create(client, command, directory_list);
        } else {
             printf
                ("evfs_handle_dir_list_command: Recevied null from plugin for directory_list\n");
        }
  } else {
        printf("No plugin for '%s'\n",
               command->file_command.files[0]->plugin_uri);
  }
}



void evfs_handle_directory_create_command(evfs_client* client, evfs_command* command) 
{
	evfs_plugin* plugin = NULL;
	int ret = 0;
	
       plugin =
       evfs_get_plugin_for_uri(client->server,
                              command->file_command.files[0]->plugin_uri);
	
	if (plugin) {
		printf("Making new directory '%s'",
	        command->file_command.files[0]->path);
		  
	        ret =
	        (*EVFS_PLUGIN_FILE(plugin)->functions->evfs_file_mkdir) (command->file_command.files[0]);
		printf("....ret was %d\n", ret);
	}
}



/* TODO:
 * 1. check if file exists before open/write.  if so, wait-and-lock for user
 *    choice
 * 2. in main loop, if file read bytes < expected, delete file, and retry a 
 *    preset number of times, before error.
 */
void
evfs_handle_file_copy(evfs_client * client, evfs_command * command,
                      evfs_command * root_command, int move)
{
   evfs_plugin *plugin = NULL;
   evfs_plugin *dst_plugin = NULL;
   int num_files;
   int c_file;

   char destination_file[PATH_MAX];

   struct stat file_stat;
   struct stat dest_stat;

   int res;
   evfs_operation_files *op;

   num_files = command->file_command.num_files;
   printf("Num files at copy: %d\n", num_files);

   /* < 2 files, no copy, dude */
   if (num_files < 2) {
	   /*TODO error return here*/
	   return;
   }

   dst_plugin =
     evfs_get_plugin_for_uri(client->server,
    command->file_command.files[num_files-1]->plugin_uri);

   if (num_files > 2) {
	   /*Check if >2 files, dest file is a dir*/
	    res =
	    (*EVFS_PLUGIN_FILE(dst_plugin)->functions->evfs_file_lstat) (command, &dest_stat, num_files-1);

	    if (res != EVFS_SUCCESS || !S_ISDIR(dest_stat.st_mode)) {
		    printf("Copy > 2 files, and dest is not a directory\n");
		    return;
	    }
   }

   /*Make a new evfs_operation, for client communication */
   if (root_command == command) {
	   op = evfs_operation_files_new(client, root_command);
	   root_command->op = EVFS_OPERATION(op);
   } else {
	   op = EVFS_OPERATION_FILES(root_command->op);
   }

   for (c_file=0;c_file < num_files - 1; c_file++) {
	   plugin =
	      evfs_get_plugin_for_uri(client->server,
                 command->file_command.files[c_file]->plugin_uri);
	
	   if (plugin && dst_plugin) {

	        /*Check for supported options */
	        if (!(EVFS_PLUGIN_FILE(plugin)->functions->evfs_file_lstat &&
	              EVFS_PLUGIN_FILE(plugin)->functions->evfs_file_open &&
	              EVFS_PLUGIN_FILE(dst_plugin)->functions->evfs_file_create &&
		      EVFS_PLUGIN_FILE(plugin)->functions->evfs_file_read &&
		      EVFS_PLUGIN_FILE(dst_plugin)->functions->evfs_file_write))
        	  {
	             printf("ARGH! Copy Not supported!\n");
	             evfs_operation_destroy(EVFS_OPERATION(op));
	             return;
	          }

	        /*Get the source file size */
	        (*EVFS_PLUGIN_FILE(plugin)->functions->evfs_file_lstat) (command, &file_stat, c_file);

		/*Get destination file size*/
	        res =
	           (*EVFS_PLUGIN_FILE(dst_plugin)->functions->evfs_file_lstat) (command, &dest_stat, num_files-1);

		/*if (S_ISDIR(dest_stat.st_mode)) {
			printf("Dest is a dir: %s\n", command->file_command.files[num_files-1]->path);
			printf("Res: %d\n", res);
		} else {
			printf("Dest not a dir: %s://%s\n",  command->file_command.files[num_files-1]->plugin_uri,
				command->file_command.files[num_files-1]->path);
			printf("Res: %d\n", res);
		}*/

	        if (!S_ISDIR(file_stat.st_mode)) {
			  evfs_filereference* rewrite_dest = NULL;

			  rewrite_dest = 
			     evfs_filereference_clone(command->file_command.files[num_files-1]); 

			  /*If dest_file exists, and is a dir - the dest file is rewritten
			   * to be the original filename in the new dir*/
			  if (res == EVFS_SUCCESS && S_ISDIR(dest_stat.st_mode)) {
				evfs_command* new_command;
				int final_len;
				char *slashpos = strrchr(command->file_command.files[c_file]->path, '/');
				printf("Filename is: %s\n", slashpos);
				
				
				final_len =strlen(rewrite_dest->path) + strlen(slashpos) +1 /* \0 */; 
				char *path_to = malloc(final_len);
				
				
				if (strcmp(command->file_command.files[num_files-1]->path, "/")) 
					snprintf(path_to, final_len , "%s%s", command->file_command.files[num_files-1]->path, slashpos);
				else
					snprintf(path_to, final_len , "%s%s", command->file_command.files[num_files-1]->path, slashpos+1);
				printf("Multi file dest dir rewrite path: %s\n", path_to);

				free(rewrite_dest->path);
				rewrite_dest->path = path_to;

				new_command = evfs_file_command_single_build(rewrite_dest);
				res =
		 	          (*EVFS_PLUGIN_FILE(dst_plugin)->functions->evfs_file_lstat)
				  (new_command, &dest_stat, 0);

				evfs_cleanup_file_command_only(new_command);
			  }
			
			  evfs_operation_copy_task_add(EVFS_OPERATION(op), 
			       evfs_filereference_clone(command->file_command.files[c_file]),
			       rewrite_dest,
			       file_stat, dest_stat, res);

			  /*If we're a move, queue the delete of this dir..*/
			  if (move) {
			       evfs_operation_remove_task_add(EVFS_OPERATION(op), 
			     	  evfs_filereference_clone(command->file_command.files[c_file]),
				  file_stat);
			  }

			  printf("Rewritten destination: '%s'\n", rewrite_dest->path);

	          } else {
	             Ecore_List *directory_list = NULL;
		     int newlen;
		     int origlen;
		     char* pos;

		     evfs_filereference* newdir_rewrite;
		     
		     newdir_rewrite = evfs_filereference_clone(command->file_command.files[num_files-1]);
		     if (command == root_command && S_ISDIR(dest_stat.st_mode)) {
			     origlen = strlen(newdir_rewrite->path);
			     printf("Origlen is: %d (%s)\n", origlen, newdir_rewrite->path);
			     pos = strrchr(command->file_command.files[c_file]->path, '/');
			     printf("String after pos: '%s'\n", pos+1);
			     newlen = strlen(newdir_rewrite->path)+1+strlen(pos+1)+1;
			     printf("Newlen is: %d\n", newlen);
			     newdir_rewrite->path = realloc(newdir_rewrite->path, newlen);
			    
			     /*Handle the root dir*/
			     if (strcmp(newdir_rewrite->path, "/")) {
				     newdir_rewrite->path[origlen] = '/';
				     strncat(&newdir_rewrite->path[origlen+1], pos+1, strlen(pos)-1);
			     } else {
				     strncat(&newdir_rewrite->path[origlen], pos+1, strlen(pos)-1);
			     }
		     }
		     

		     printf("Done: '%s %s'\n",  newdir_rewrite->plugin_uri, newdir_rewrite->path);
		     
		     
	
		     evfs_operation_mkdir_task_add(EVFS_OPERATION(op), evfs_filereference_clone(command->file_command.files[c_file]), newdir_rewrite);
	
	             /*First, we need a directory list... */
	             (*EVFS_PLUGIN_FILE(plugin)->functions->evfs_dir_list) (client, command->file_command.files[c_file],
	                                                  &directory_list);
	             if (directory_list) {
        	          evfs_filereference *file = NULL;

                	  while ((file = ecore_list_remove_first(directory_list))) {
	                       evfs_filereference *source = NEW(evfs_filereference);
	                       evfs_filereference *dest = NEW(evfs_filereference);
	                       evfs_command *recursive_command = NEW(evfs_command);
	
        	               snprintf(destination_file, PATH_MAX, "%s%s",
                	                newdir_rewrite->path,
	                                strrchr(file->path, '/'));

        	               source->path = strdup(file->path);
	                       source->plugin_uri =
	                          strdup(command->file_command.files[c_file]->plugin_uri);
	                       source->parent = NULL;   /*TODO - handle nested uris */

	                       dest->path = strdup(destination_file);
	                       dest->plugin_uri = strdup(newdir_rewrite->plugin_uri);
	                       dest->parent = NULL;     /*TODO - handle nested uris */

        	               recursive_command->file_command.files =
	                          malloc(sizeof(evfs_filereference *) * 2);
	                       recursive_command->type = EVFS_CMD_FILE_COPY;
	                       recursive_command->file_command.files[0] = source;
	                       recursive_command->file_command.files[1] = dest;
        	               recursive_command->file_command.num_files = 2;

	                       //printf("Copy file '%s' to %s\n", file->path, destination_file);
	
        	               evfs_handle_file_copy(client, recursive_command,
                                             root_command, move);

	                       evfs_cleanup_filereference(file);
	                       evfs_cleanup_command(recursive_command,
                                            EVFS_CLEANUP_FREE_COMMAND);
	                  } //while
	                  ecore_list_destroy(directory_list);

			  /*If we're a move, queue the delete of this dir (it should be empty - unless perms denied it....*/
			  if (move) {
			       evfs_operation_remove_task_add(EVFS_OPERATION(op), 
			     	  evfs_filereference_clone(command->file_command.files[c_file]),
				  file_stat);
			  } //if
	               }// root_if
        	  } //is_dir if

     } else {
        printf("Could not get plugins for both source and dest: (%s:%s)\n",
               command->file_command.files[c_file]->plugin_uri,
               command->file_command.files[num_files-1]->plugin_uri);
     }
  } //for loop if

   /*Only send '100%' event when we're back at the top, or we aren't recursive */
   if (command == root_command) {
          /*evfs_file_progress_event_create(client, command, root_command, 100,
                                           EVFS_PROGRESS_TYPE_DONE);*/
	   evfs_operation_tasks_print(EVFS_OPERATION(op));
	   evfs_operation_queue_pending_add(EVFS_OPERATION(op));
  }

}

void
evfs_handle_ping_command(evfs_client * client __UNUSED__, evfs_command * command __UNUSED__)
{
   printf("Ping!\n");
}

void
evfs_handle_operation_command(evfs_client * client __UNUSED__, evfs_command * command)
{
   evfs_operation *op_get = NULL;

   op_get = evfs_operation_get_by_id(command->op->id);
   if (op_get)
     {
        op_get->status = EVFS_OPERATION_STATUS_REPLY_RECEIVED;
        op_get->response = command->op->response;
        printf("*** Received operation response for op %ld -> %d\n",
               command->op->id, command->op->response);
     }
}

void evfs_handle_metadata_command(evfs_client* client, evfs_command* command)
{
	char* type = "audio/x-mp3"; /*FIXME - make generic - id mimes..*/
	evfs_plugin_meta* plugin;
	Evas_List* ret_list;

	plugin = EVFS_PLUGIN_META(evfs_meta_plugin_get_for_type(evfs_server_get(),type));
	if (plugin) {
		ret_list = (*plugin->functions->evfs_file_meta_retrieve)(client,command);

		evfs_meta_data_event_create(client,command, ret_list);
	} else {
		printf("Could not find plugin to tag this type\n");
	}
	
}

void evfs_handle_metadata_string_file_set_command(evfs_client* client __UNUSED__, 
		evfs_command* command, char* key, char* value) 
{
	if (command->file_command.num_files > 0) {
		evfs_metadata_file_set_key_value_string(command->file_command.files[0], key, value);
	}
}

void evfs_handle_metadata_string_file_get_command(evfs_client* client __UNUSED__, 
		evfs_command* command, char* key) 
{
	char* value;
	if (command->file_command.num_files > 0) {
		value = evfs_metadata_file_get_key_value_string(command->file_command.files[0], key);
	}
}


void evfs_handle_metadata_groups_request_command(evfs_client* client, evfs_command* command) 
{
	Evas_List* group_list;

	if ( (group_list = evfs_metadata_groups_get())) {
		evfs_group_list_event_create(client, command, group_list);
	}
}

void evfs_handle_metadata_file_group_add(evfs_client* client, evfs_command* command)
{
	if (command->file_command.num_files > 0) {
		evfs_metadata_group_header_file_add(command->file_command.files[0], command->file_command.ref);
	}
}

void evfs_handle_metadata_file_group_remove(evfs_client* client, evfs_command* command)
{
	if (command->file_command.num_files > 0) {
		evfs_metadata_group_header_file_remove(command->file_command.files[0], command->file_command.ref);
	}	
}
