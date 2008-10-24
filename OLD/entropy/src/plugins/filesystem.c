#define _GNU_SOURCE

#include "plugin.h"
#include <stdio.h>
#include "plugin_base.h"
#include <Ecore.h>
#include "entropy.h"
#include "entropy_gui.h"
#include <evfs.h>
#include <sys/types.h>
#include <dirent.h>
#include <strings.h>
#include <limits.h>
#include <string.h>

#define NO_HIDDEN 1
#define URI_POSIX "file"



Ecore_Hash *folder_monitor_hash;
Ecore_Hash *evfs_dir_requests;
entropy_core *filesystem_core;	/*A reference to the core */
Ecore_List* metadata_groups = NULL;

Ecore_List *structurelist_get (char *base);
struct stat *filestat_get (entropy_file_request * request);
Ecore_List *filelist_get (entropy_file_request * request);
void entropy_filesystem_file_copy (entropy_generic_file * file, char *path_to, entropy_gui_component_instance * instance);
void entropy_filesystem_file_copy_multi (Ecore_List* files, char *path_to, entropy_gui_component_instance * instance);
void entropy_filesystem_file_move_multi (Ecore_List* files, char *path_to, entropy_gui_component_instance * instance);
void entropy_filesystem_file_trash_restore (Ecore_List* files, entropy_gui_component_instance * instance);
void entropy_filesystem_file_move (entropy_generic_file * file, char *path_to, entropy_gui_component_instance * instance);
void entropy_filesystem_file_rename (entropy_generic_file * file_from, entropy_generic_file * file_to);
void entropy_filesystem_operation_respond(long id, int response);
void entropy_filesystem_directory_create (entropy_generic_file * parent, const char* child_name);
void entropy_filesystem_file_remove (entropy_generic_file * file, entropy_gui_component_instance* instance);
void entropy_filesystem_metadata_groups_get(entropy_gui_component_instance* instance);
void entropy_filesystem_file_group_add(entropy_generic_file* file, char* group);
void entropy_filesystem_file_group_remove(entropy_generic_file* file, char* group);
Ecore_List* entropy_filesystem_metadata_groups_retrieve();
void entropy_filesystem_auth_response(char* location, char* user, char* password);
void entropy_filesystem_meta_all_retrieve();
void entropy_filesystem_vfolder_create(char* name, Eina_List* entries);

static evfs_connection *con;

void
callback (EvfsEvent * data, void *obj)
{

  //printf("Callback hit..%d\n", data->type);


  switch (data->type) {
  case EVFS_EV_SUB_MONITOR_NOTIFY:{
      EvfsFilereference* filemonitor = EVFS_EVENT_FILE_MONITOR(data)->file;
					  
      /*Find a better way to do this, possibly modify evfs to pass a reference object */
      Ecore_List *watchers;
      void *key;
      entropy_gui_event *gui_event;

      char *folder = strdup (filemonitor->path);
      char *pos = rindex (folder, '/');
      *pos = '\0';

      printf("Got a monitor event for folder '%s'..\n", folder);


      watchers = ecore_hash_keys (folder_monitor_hash);
      ecore_list_first_goto (watchers);
      while ((key = ecore_list_next (watchers))) {
	printf("Scanning watcher... %s:%s\n", ((evfs_file_uri_path *)
	      ecore_hash_get (folder_monitor_hash, key))->files[0]->path, folder );
	      
	if (!strcmp
	    (((evfs_file_uri_path *)
	      ecore_hash_get (folder_monitor_hash, key))->files[0]->path,
	     folder)) {
	  char *md5 = md5_entropy_path_file (filemonitor->plugin_uri, folder, pos + 1);
	  entropy_file_listener *listener;


	  printf("Creating event for instance/layout %p..\n", key);


	  entropy_generic_file *file;
	  if (!(listener = entropy_core_file_cache_retrieve (md5))) {
	    file = entropy_generic_file_new ();
	    /*For now, just make an entropy_generic_file for this object */

	    strncpy (file->path, folder, PATH_MAX);
	    strncpy (file->filename, pos + 1, strlen (pos + 1) + 1);
	    strncpy (file->uri_base, filemonitor->plugin_uri,
		     strlen (filemonitor->plugin_uri));
	    file->md5 = strdup (md5);
	    
	    if (filemonitor->file_type == EVFS_FILE_DIRECTORY) {
		    strcpy(file->mime_type, "file/folder");
		    file->filetype = FILE_FOLDER;
	    }

	    file->thumbnail = NULL;

	    /*Register a new listener for this file */
	    listener = entropy_malloc (sizeof (entropy_file_listener));
	    listener->file = file;
	    listener->count = 0;
	    entropy_core_file_cache_add (md5, listener);
	  }
	  else {
		  
	    file = listener->file;
	    entropy_free (md5);	/*We don't need this one, we're returning an old ref */
	  }


	  //printf("Found a watcher of this dir\n");




	  /*Create a GUI event to send to watchers */
	  gui_event = entropy_malloc (sizeof (entropy_gui_event));

	  switch (EVFS_EVENT_FILE_MONITOR(data)->type) {
	  case EVFS_FILE_EV_CREATE:	//printf("  Create event\n");
	    gui_event->event_type =
	      entropy_core_gui_event_get (ENTROPY_GUI_EVENT_FILE_CREATE);
	    break;
	  case EVFS_FILE_EV_CHANGE:	//printf("  Change event\n");
	    gui_event->event_type =
	      entropy_core_gui_event_get (ENTROPY_GUI_EVENT_FILE_CHANGE);
	    break;
	  case EVFS_FILE_EV_REMOVE:	//printf("  Change event\n");
	    gui_event->event_type =
	      entropy_core_gui_event_get (ENTROPY_GUI_EVENT_FILE_REMOVE);
	    break;
	  case EVFS_FILE_EV_REMOVE_DIRECTORY:	//printf("  Change event\n");
	    gui_event->event_type =
	      entropy_core_gui_event_get
	      (ENTROPY_GUI_EVENT_FILE_REMOVE_DIRECTORY);
	    break;



	  }

	  printf("Dispatching create event for '%s' to %p\n", file->filename, key);
	  
	  gui_event->data = file;
	  entropy_core_layout_notify_event (key, gui_event,
					    ENTROPY_EVENT_LOCAL);
	}
      }

      entropy_free (folder);
    }
    break;


  case EVFS_EV_STAT:{

      entropy_gui_event *gui_event;
      entropy_gui_component_instance *instance;
      entropy_file_stat *file_stat;
      entropy_file_listener* listener;
      EvfsFilereference* ref;
      char* md5;
      char *folder, *filename;
      char* pos;


      ref = evfs_command_first_file_get(data->command);

      pos = rindex (ref->path, '/');
      /*Set the file's filename*/
      filename = strdup (pos + 1);
	
      if (pos - ref->path == 0 && ref->path[0] == '/') 
   	folder = strdup("/");
      else {
	folder = calloc(pos - ref->path +1, 1 );
 	strncpy(folder, ref->path, pos - ref->path);
      }


      
      /*printf("Path: '%s' -> pos+1: '%s'\n", (char *) data->resp_command.file_command.files[0]->path, pos+1);*/

      //printf("Folder: '%s'\nFilename: '%s'\n", folder, pos+1);
      md5 = md5_entropy_path_file (ref->plugin_uri, folder, filename);
      instance = ecore_hash_get (evfs_dir_requests, (long*)data->command->client_identifier );

      /*Build a file<->stat structure to pass to requester */
      file_stat = entropy_malloc (sizeof (entropy_file_stat));
      file_stat->stat_obj = entropy_malloc (sizeof (struct stat));

      file_stat->stat_obj->st_mode = EVFS_EVENT_STAT(data)->stat->st_mode;
      file_stat->stat_obj->st_uid = EVFS_EVENT_STAT(data)->stat->st_uid;
      file_stat->stat_obj->st_gid = EVFS_EVENT_STAT(data)->stat->st_gid;
      file_stat->stat_obj->st_size = EVFS_EVENT_STAT(data)->stat->st_size;
      file_stat->stat_obj->st_atime = EVFS_EVENT_STAT(data)->stat->ist_atime;
      file_stat->stat_obj->st_mtime = EVFS_EVENT_STAT(data)->stat->ist_mtime;
      file_stat->stat_obj->st_ctime = EVFS_EVENT_STAT(data)->stat->ist_ctime;

      /*Retrieve the file. This is bad - the file might not exist anymore! */
      listener = entropy_core_file_cache_retrieve (md5);
      if (listener) {
	      file_stat->file = listener->file;

	      /*Pop stats*/
	      listener->file->properties.st_mode = EVFS_EVENT_STAT(data)->stat->st_mode;
	      listener->file->properties.st_uid = EVFS_EVENT_STAT(data)->stat->st_uid;
	      listener->file->properties.st_gid = EVFS_EVENT_STAT(data)->stat->st_gid;
	      listener->file->properties.st_size = EVFS_EVENT_STAT(data)->stat->st_size;
	      listener->file->properties.st_atime = EVFS_EVENT_STAT(data)->stat->ist_atime;
	      listener->file->properties.st_mtime = EVFS_EVENT_STAT(data)->stat->ist_mtime;
	      listener->file->properties.st_ctime = EVFS_EVENT_STAT(data)->stat->ist_ctime;
	      listener->file->retrieved_stat = 1;


	      /*Build up the gui_event wrapper */
	      gui_event = entropy_malloc (sizeof (entropy_gui_event));
	      gui_event->event_type =
		entropy_core_gui_event_get (ENTROPY_GUI_EVENT_FILE_STAT_AVAILABLE);
	      gui_event->data = file_stat;

	      //printf("File stat at %p\n", file_stat);

	      /*Call the callback stuff */
	      entropy_core_layout_notify_event (instance, gui_event,
					ENTROPY_EVENT_LOCAL);
      } else {
	      printf("Error! Couldn't find listener for '%s' '%s' '%s'\n",evfs_command_first_file_get(data->command)->plugin_uri, folder, filename);
      }

      /*Do some freeing */
      entropy_free (file_stat->stat_obj);
      entropy_free (file_stat);

      ecore_hash_remove (evfs_dir_requests, (long*)data->command->client_identifier);
      entropy_free (folder);
      entropy_free (filename);
      entropy_free (md5);

      /*No need to free event - notify_event frees it for us */


    }
    break;

  case EVFS_EV_DIR_LIST:{
      Ecore_List *file_list = ecore_list_new ();
      entropy_file_request *calling_request = NULL;
      entropy_generic_file *file;
      entropy_file_listener *listener;
      entropy_gui_event *gui_event;
      EvfsFilereference *ref;
      char *filename;
      char *folder;
      char *pos;

      printf("Looking for callers for dir list for: '%s'\n", evfs_command_first_file_get(data->command)->path);
      calling_request = ecore_hash_get (evfs_dir_requests, (long*)data->command->client_identifier);
      if (calling_request) {
	     /*Remove from directory requesters*/
	      ecore_hash_remove(evfs_dir_requests, (long*)data->command->client_identifier );
		
	      
	      ecore_list_append(file_list, calling_request);
	      
	      /*folder = data->resp_command.file_command.files[0]->path;*/

	      if (EVFS_EVENT_DIR_LIST(data)->files) {
		      Eina_List* l;
		      for (l= EVFS_EVENT_DIR_LIST(data)->files;l; ) {
			ref = l->data;
			l = l->next; 

			/*printf("(%s) Received file type for file: %d\n", ref->path, ref->file_type); */

			pos = rindex (ref->path, '/');

			/*Set the file's filename*/
			filename = strdup (pos + 1);
			
			if (pos - ref->path == 0 && ref->path[0] == '/') 
				folder = strdup("/");
			else {
				folder = calloc(pos - ref->path +1, 1 );
				strncpy(folder, ref->path, pos - ref->path);
			}

			/*Look for an existing file we have cached */
			char *md5 = md5_entropy_path_file (ref->plugin_uri, folder, filename);

			/*Now create, or grab, a file */
			if (!(listener = entropy_core_file_cache_retrieve (md5))) {
	
			  file = entropy_generic_file_new ();

			  /*For now, just make an entropy_generic_file for this object */
			  strncpy (file->path, folder, 255);
			  strncpy (file->filename, filename, strlen (filename) + 1);
			  file->md5 = strdup (md5);

			  /*Set the file type, if evfs provided it */
			  if (ref->file_type == EVFS_FILE_DIRECTORY) {
			    file->filetype = FILE_FOLDER;
			    strcpy (file->mime_type, "file/folder");
	
			    //printf("Marked this file as a directory\n");
			  }
			  else {
			    //printf("Didn't mark this file, type %d\n",ref->file_type);
			    file->filetype = FILE_STANDARD;
			    bzero (file->mime_type, MIME_LENGTH);
			  }

			  /*Set the attached data -> icon_hint, if there*/
			  if (ref->attach) file->attach = strdup(ref->attach);

			  if (calling_request
			      && (calling_request->drill_down
				  || calling_request->set_parent)) {
			    /*printf("Calling request had a parent...\n");
			       printf("File ('%s') parent's name is '%s'\n", 
			       file->filename, calling_request->reparent_file->filename); */
		
			    file->parent = calling_request->reparent_file;
	
			    /*We are referencing the parent, so - we need to tell the core that we *need* this
			     * file - i.e. don't clean it up*/
			    entropy_core_file_cache_add_reference (calling_request->
							   reparent_file->md5);
			  }
	
			  /*Mark the file's uri FIXME do this properly */
			  strcpy (file->uri_base, ref->plugin_uri);
			  /*printf("Assigned plugin URI: '%s'\n", ref->plugin_uri); */
	

			  /*Register a new listener for this file */
			  listener = entropy_malloc (sizeof (entropy_file_listener));
			  listener->file = file;
			  listener->count = 1;
			  entropy_core_file_cache_add (md5, listener);
			}
			else {
			  file = listener->file;
			  entropy_core_file_cache_add_reference (listener->file->md5);
			  entropy_free (md5);	/*We don't need this one, we're returning an old ref */
			}
		
			free (filename);
	
			/*Add this file to our list */
			ecore_list_append (file_list, file);

			/*Free folder*/
			free(folder);
		      }
		}

	      /*Create a GUI event to send to watchers */
	      gui_event = entropy_malloc (sizeof (entropy_gui_event));
	      gui_event->event_type =
		entropy_core_gui_event_get
		(ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS_EXTERNAL);
	      gui_event->data = file_list;
	      
	      if (calling_request) {
		      entropy_core_layout_notify_event (calling_request->requester, gui_event,
						ENTROPY_EVENT_LOCAL);
	      } else {
		      printf("  [*] Could not get calling request for dir list - Abort!\n");
	      }

	      free(calling_request);
	} else {
		ecore_list_destroy (file_list);
	}
    }

    break;
  case EVFS_EV_FILE_PROGRESS:{
      entropy_gui_event *gui_event;
      entropy_gui_component_instance *instance = NULL;
      entropy_file_progress *request =
	entropy_malloc (sizeof (entropy_file_progress));
      char *uri = NULL;

      request->identifier = data->command->client_identifier;

       if (EVFS_EVENT_PROGRESS(data)->from) {
	request->file_from = EvfsFilereference_to_entropy_generic_file(EVFS_EVENT_PROGRESS(data)->from);
       }
       if (EVFS_EVENT_PROGRESS(data)->to) {
	request->file_to = EvfsFilereference_to_entropy_generic_file(EVFS_EVENT_PROGRESS(data)->to);
       }


      request->progress = EVFS_EVENT_PROGRESS(data)->progressAmt;

      if (EVFS_EVENT_PROGRESS(data)->type == EVFS_PROGRESS_TYPE_CONTINUE)
	request->type = TYPE_CONTINUE;
      else
	request->type = TYPE_END;

      /*Find who called us */
      uri =
	EvfsFilereference_to_string (evfs_command_first_file_get(data->command));
      instance = ecore_hash_get (evfs_dir_requests, (long*)data->command->client_identifier);

      if (instance) {
	/*Build up the gui_event wrapper */
        gui_event = entropy_malloc (sizeof (entropy_gui_event));
        gui_event->event_type =
   	entropy_core_gui_event_get (ENTROPY_GUI_EVENT_FILE_PROGRESS);
        gui_event->data = request;
	      
	entropy_core_layout_notify_event (instance, gui_event,
					  ENTROPY_EVENT_LOCAL);
      }
      else {
	printf ("Could not get file copy caller for '%s'\n", uri);
      }

      if (EVFS_EVENT_PROGRESS(data)->type == EVFS_PROGRESS_TYPE_DONE) {
	ecore_hash_remove(evfs_dir_requests, (long*)data->command->client_identifier);
      }

      if (uri) free (uri);
      if (request->file_from) entropy_generic_file_destroy (request->file_from);
      if (request->file_to) entropy_generic_file_destroy (request->file_to);
      free (request);
    }
    break;

  case EVFS_EV_OPERATION: {
	char *uri = NULL;
	entropy_gui_component_instance* instance = NULL;
	entropy_gui_event* gui_event;
	entropy_file_operation* op;

	printf("EVFS requested feedback on an operation!, id %ld\n",  EVFS_EVENT_OPERATION(data)->operation->id);

      /*Find who called us */
      uri =
	EvfsFilereference_to_string (evfs_command_first_file_get(data->command));
      instance = ecore_hash_get (evfs_dir_requests, (long*)data->command->client_identifier);
      
      if (instance) {
         /*Build up the gui_event wrapper */
         gui_event = entropy_malloc (sizeof (entropy_gui_event));
         gui_event->event_type =
	 entropy_core_gui_event_get (ENTROPY_GUI_EVENT_USER_INTERACTION_YES_NO_ABORT);

	 op = entropy_malloc(sizeof(entropy_file_operation));

	 printf("Operation file is: %s\n", EVFS_EVENT_OPERATION(data)->misc);
	 op->file = EVFS_EVENT_OPERATION(data)->misc;
	 op->id = EVFS_EVENT_OPERATION(data)->operation->id;
	 gui_event->data = op;
	
	      
	entropy_core_layout_notify_event (instance, gui_event,
					  ENTROPY_EVENT_LOCAL);

	 printf("Requesting send of operation stat to instance %p, evfs operation ID %ld\n", instance, op->id);


	 free(op);
      }
      else {
	printf ("Could not get file copy caller for '%s'\n", uri);
	//free (gui_event);
      }

  }
  break;

  case EVFS_EV_METADATA_GROUPS: {
	
					
	Eina_List* l;
	if (metadata_groups) {
		char* obj;
		while ((obj = ecore_list_first_remove(metadata_groups))) {
			free(obj);
		}
	}
	for (l = EVFS_EVENT_METADATA_GROUPS(data)->string_list; l; ) {
		ecore_list_append(metadata_groups, strdup( ((EvfsMetadataGroup*)l->data)->name));

		l=l->next;
	}
	
  }
  break;

  case EVFS_EV_METAALL: {
	entropy_gui_component_instance* instance = NULL;
	entropy_gui_event* gui_event;

        instance = ecore_hash_get (evfs_dir_requests, (long*)data->command->client_identifier);
        ecore_hash_remove (evfs_dir_requests, (long*)data->command->client_identifier);



        if (instance) {
		/*Build up the gui_event wrapper */
	         gui_event = entropy_malloc (sizeof (entropy_gui_event));
        	 gui_event->event_type =
		 entropy_core_gui_event_get (ENTROPY_GUI_EVENT_META_ALL_REQUEST);

		 gui_event->data = EVFS_EVENT_META_ALL(data)->meta;
		      
		entropy_core_layout_notify_event (instance, gui_event,
					  ENTROPY_EVENT_LOCAL);
      }
  }
  break;

  case EVFS_EV_AUTH_REQUIRED: {
	entropy_file_request* calling_request = NULL;

 	printf("Received 'auth required' event\n");
	calling_request = ecore_hash_get (evfs_dir_requests, (long*)data->command->client_identifier);
	if (calling_request) {
		char loc[PATH_MAX];
		snprintf(loc,sizeof(loc),"%s://%s/%s", calling_request->file->uri_base, calling_request->file->path, calling_request->file->filename);
		printf("Found original request..\n");
		
		entropy_event_auth_request(calling_request->requester, loc);
	} else {
		printf("Could not find original request\n");
	}
  }
  break;

  default:
    printf ("Received an EVFS message we don't recognise!\n");
    break;
  }

}



/*Internal functions*/
void
filesystem_monitor_directory (void *requester, evfs_file_uri_path * folder)
{ 
  printf("Monitoring '%s' for '%p'\n", folder->files[0]->path, requester);
  ecore_hash_set (folder_monitor_hash, requester, folder);
}

void
filesystem_demonitor_directory (void *requester)
{
  /*We assume we only monitor one dir at a time, per requester */
  evfs_file_uri_path *dir = ecore_hash_get (folder_monitor_hash, requester);
  Ecore_List* keys;
  entropy_gui_component_instance* key;
  evfs_file_uri_path* compare;
  int found = 0;
  
  if (dir) {
    ecore_hash_remove(folder_monitor_hash, requester);
   	  
    //fprintf(stderr, "Demonitoring '%s' for '%p'...\n", dir->files[0]->path, requester);

    /*Now check if anyone else is monitoring this dir - if not, remove the evfs listener*/
    keys = ecore_hash_keys(folder_monitor_hash);  
    while ( (key = ecore_list_first_remove(keys))) {
	    compare = ecore_hash_get(folder_monitor_hash, key);
	    if (compare && EvfsFilereference_equal_is(dir->files[0], compare->files[0]))
		    found = 1;
    }
    
    if (!found) {
	    //printf("Removing last watcher on directory!\n");
	    evfs_monitor_remove (con, dir->files[0]);
    } else {
	    //printf("More watchers on directory!\n");
    }

    evfs_cleanup_file_uri_path(dir);
  }
  else {
    //fprintf(stderr, "This requester has no monitored directories\n");
  }
}

/*---------------------------*/



char *
entropy_plugin_identify ()
{
  return (char *) "Simple file backend";
}

int
entropy_plugin_type_get ()
{
  return ENTROPY_PLUGIN_BACKEND_FILE;
}

Entropy_Plugin*
entropy_plugin_init (entropy_core * core)
{
  Entropy_Plugin_File* plugin;
  Entropy_Plugin* base;
	
  /*Initialise misc. hashes*/
  folder_monitor_hash =
    ecore_hash_new (ecore_direct_hash, ecore_direct_compare);
  evfs_dir_requests = ecore_hash_new (ecore_direct_hash, ecore_direct_compare);

  /*Connect to evfs*/
  con = evfs_connect (&callback, NULL);

  /*Save a core reference to avoid lookups*/
  filesystem_core = core;

  /*Somewhere to store metadata groups */
  metadata_groups = ecore_list_new();

  plugin = entropy_malloc(sizeof(Entropy_Plugin_File));
  base = ENTROPY_PLUGIN(plugin);

  plugin->file_functions.structurelist_get = &structurelist_get;
  plugin->file_functions.filestat_get = &filestat_get;
  plugin->file_functions.filelist_get = &filelist_get;
  plugin->file_functions.file_copy = &entropy_filesystem_file_copy;
  plugin->file_functions.file_copy_multi = &entropy_filesystem_file_copy_multi;
  plugin->file_functions.file_move_multi = &entropy_filesystem_file_move_multi;
  plugin->file_functions.file_move = &entropy_filesystem_file_move;
  plugin->file_functions.file_trash_restore = &entropy_filesystem_file_trash_restore;

  plugin->file_functions.file_rename = &entropy_filesystem_file_rename;
  plugin->file_functions.operation_respond = &entropy_filesystem_operation_respond;
  plugin->file_functions.directory_create = &entropy_filesystem_directory_create;
  plugin->file_functions.file_remove = &entropy_filesystem_file_remove;
  plugin->misc_functions.groups_get = &entropy_filesystem_metadata_groups_get;
  plugin->misc_functions.groups_retrieve = &entropy_filesystem_metadata_groups_retrieve;
  plugin->file_functions.group_file_add = &entropy_filesystem_file_group_add;
  plugin->file_functions.group_file_remove = &entropy_filesystem_file_group_remove;
  plugin->file_functions.auth_respond = entropy_filesystem_auth_response;
  plugin->misc_functions.meta_all_get = &entropy_filesystem_meta_all_retrieve;
  plugin->misc_functions.vfolder_create = &entropy_filesystem_vfolder_create;
  
  return base; 

  
}

void
entropy_plugin_destroy (void *obj)
{
}

Ecore_List *
structurelist_get (char *base)
{
  struct dirent *de;
  struct stat st;
  DIR *dir;
  entropy_generic_file *ef;
  Ecore_List *el = ecore_list_new ();
  char full_name[1024];

  dir = opendir (base);
  while ((de = readdir (dir))) {
    snprintf (full_name, 1024, "%s/%s", base, de->d_name);
    stat (full_name, &st);

    if (S_ISDIR (st.st_mode) && strcmp (de->d_name, ".")
	&& strcmp (de->d_name, "..")) {
      //printf ("%s is a directory\n", de->d_name);

      ef = entropy_generic_file_new ();

      /*TODO don't use this - make an internal representation,
       * otherwise not generic */

      ef->filetype = FILE_FOLDER;
      strcpy (ef->filename, de->d_name);
      strcpy (ef->path, base);
      ecore_list_append (el, ef);
    }
  }

  return el;

}


struct stat *
filestat_get (entropy_file_request * request)
{
  EvfsFilereference *path;
  char *uri = uri = entropy_core_generic_file_uri_create (request->file, 0);
  long id;

  path = evfs_parse_uri_single (uri);
  if (request->file->attach) 
	  path->attach = strdup(request->file->attach);

  id = evfs_client_file_stat (con, path);
  ecore_hash_set (evfs_dir_requests, (long*)id, request->requester);

  evfs_cleanup_filereference (path);
  free(uri);


  return NULL;
}



Ecore_List *
filelist_get (entropy_file_request * request)
{
  struct dirent *de;
  struct stat st;
  DIR *dir;
  entropy_generic_file *ef;
  Ecore_List *el = ecore_list_new ();

  char dire[255];
  char full_name[1024];
  int count = 0;
  char posix_name[1024];
  char *md5;
  int filetype = -1;
  int showhidden;
  evfs_file_uri_path *dir_path;
  entropy_file_listener *listener;

  showhidden = entropy_config_misc_is_set("general.hiddenbackup");

  if ((!strcmp (request->file->uri_base, URI_POSIX)) && !request->drill_down
      && !request->file->parent) {
    printf ("Listing standard posix directory...\n");

    /*If either the path, or the filename, is the root dir, we don't need another slash */
    if (strcmp (request->file->filename, "/")
	&& strcmp (request->file->path, "/")) {
      snprintf (dire, 255, "%s/%s", request->file->path,
		request->file->filename);
    }
    else {
      snprintf (dire, 255, "%s%s", request->file->path,
		request->file->filename);
    }


    //printf("Opening dir: %s\n", dire);


    if (access (dire, X_OK)) {
      return NULL;
    }

    snprintf (posix_name, 1024, "file://%s", dire);
    dir_path = evfs_parse_uri (posix_name);

    /*First demonitor this reqeuestor's last directory, if any */
    filesystem_demonitor_directory (request->requester);


    /*Start monitoring this directory */
    //printf("Monitoring '%s'\n", dir_path->files[0]->path);
    evfs_monitor_add (con, dir_path->files[0]);
    filesystem_monitor_directory (request->requester, dir_path);

    dir = opendir (dire);
    while ((de = readdir (dir))) {
      if (strcmp (de->d_name, ".") && strcmp (de->d_name, "..")
	  && ((de->d_name[0] != '.') || showhidden)) {
	snprintf (full_name, 1024, "%s/%s", dire, de->d_name);
	stat (full_name, &st);
	if (S_ISDIR (st.st_mode)) {
	  filetype = FILE_FOLDER;
	  /*printf ("This is a folder\n"); */
	}
	else {
	  /*printf( "Not a folder\n"); */
	  filetype = -1;
	}

	if (request->file_type == FILE_ALL || filetype == request->file_type) {
	  /*printf ("Adding..\n"); */

	  md5 = md5_entropy_path_file ("file", dire, de->d_name);

	  /*printf("Looking for key %s\n", md5);  */
	  if (!(listener = entropy_core_file_cache_retrieve (md5))) {
	    /*printf ("File not found in hash, making new instance\n"); */

	    ef = entropy_generic_file_new ();

	    strcpy (ef->uri_base, URI_POSIX);	/*We're a fallback posix handler, so set to posix */

	    /*TODO don't use this - make an internal representation, otherwise not generic */
	    ef->filetype = (filetype);
	    ef->md5 = md5;

	    strcpy (ef->filename, de->d_name);
	    strcpy (ef->path, dire);

	    ecore_list_append (el, ef);


	    /*Save this file's stat structure */
	    ef->retrieved_stat = 1;
	    memcpy (&ef->properties, &st, sizeof (struct stat));

	    /*Register a new listener for this file */
	    listener = entropy_malloc (sizeof (entropy_file_listener));
	    listener->file = ef;
	    listener->count = 1;


	    entropy_core_file_cache_add (md5, listener);
	  }
	  else {
            entropy_core_file_cache_add_reference (listener->file->md5);
		  
	
	    ecore_list_append (el, listener->file);
	    entropy_free (md5);	/*We don't need this one, we're returning an old ref */
	  }
	}
	else {
	  /*printf ("Not adding...\n"); */
	}
	//

      }
      count++;
    }
    closedir (dir);
    printf ("Done!\n");

    return entropy_generic_file_list_sort (el);
  }
  else {			/*Not a posix call for a dir list - don't use our local optim function */
    entropy_file_request *new_request;	/*We need to make a copy of the request object
					   because the original will be destroyed */
    long request_id;

    entropy_generic_file *source_file;
    char *uri;
    evfs_file_uri_path *path;


    /*If the file/location we are requesting has a 'parent' (i.e. it's inside another object),
     * we have to grab the parent, not the file itself, as the base*/
    if (request->file->parent) {
      source_file = request->file->parent;
    }
    else
      source_file = request->file;


    uri =
      entropy_core_generic_file_uri_create (request->file,
					    request->drill_down);

    printf ("URI: %s\n", uri);

    path = evfs_parse_uri (uri);
    request_id = evfs_client_dir_list (con, path->files[0], 0);


    new_request = entropy_malloc (sizeof (entropy_file_request));

    /*If this request/file has a parent, the new file listing's parent will be 
     * the same file - not the request file */

    new_request->file = request->file;

    if (request->file->parent || request->drill_down) {
      new_request->reparent_file = source_file;
      new_request->set_parent = 1;
    }

    new_request->core = request->core;
    new_request->drill_down = request->drill_down;
    new_request->requester = request->requester;
    new_request->file_type = request->file_type;

    ecore_hash_set (evfs_dir_requests,(long*)request_id, new_request);

    entropy_free (uri);

    return NULL;
  }
}

void
entropy_plugin_backend_file_deregister_file_list ()
{

}

void
entropy_filesystem_file_copy (entropy_generic_file * file, char *path_to,
			      entropy_gui_component_instance * instance)
{
  evfs_file_uri_path *uri_path_from;
  evfs_file_uri_path *uri_path_to;
  char uri_from[512];
  char uri_to[512];
  long id;



  snprintf (uri_from, 512, "%s://%s/%s", file->uri_base, file->path,
	    file->filename);
  snprintf (uri_to, 512, "%s/%s", path_to, file->filename);

  //printf ("Copying file %s to %s\n", uri_from, uri_to);



  uri_path_from = evfs_parse_uri (uri_from);
  uri_path_to = evfs_parse_uri (uri_to);


  //printf("Original uri is: '%s'\n", original);
  //
  /*Track the copy action */
  id = evfs_client_file_copy (con, uri_path_from->files[0], uri_path_to->files[0]);
  ecore_hash_set(evfs_dir_requests, (long*)id, instance);

  evfs_cleanup_file_uri_path(uri_path_from);
  evfs_cleanup_file_uri_path(uri_path_to);
}


void
entropy_filesystem_file_copy_multi (Ecore_List* files, char *path_to,
			      entropy_gui_component_instance * instance)
{
  long id;
  entropy_generic_file* file;
  Ecore_List* evfs_files;

  evfs_files = ecore_list_new();
  ecore_list_first_goto(files);
  while ( (file = ecore_list_next(files)) ) { 
	  printf("Parsing %s for multi copy\n", file->uri);
	  ecore_list_append(evfs_files, evfs_parse_uri_single(file->uri));
  }
  
  /*Track the copy action */
  id = evfs_client_file_copy_multi (con, evfs_files, evfs_parse_uri_single(path_to));
  ecore_hash_set(evfs_dir_requests, (long*)id, instance);

}

void
entropy_filesystem_file_move_multi (Ecore_List* files, char *path_to,
			      entropy_gui_component_instance * instance)
{
  long id;
  entropy_generic_file* file;
  Ecore_List* evfs_files;

  evfs_files = ecore_list_new();
  ecore_list_first_goto(files);
  while ( (file = ecore_list_next(files)) ) { 
	  printf("Parsing %s for multi move\n", file->uri);
	  ecore_list_append(evfs_files, evfs_parse_uri_single(file->uri));
  }
  
  /*Track the copy action */
  id = evfs_client_file_move_multi (con, evfs_files, evfs_parse_uri_single(path_to));
  ecore_hash_set(evfs_dir_requests, (long*)id, instance);

}


void
entropy_filesystem_file_move (entropy_generic_file * file, char *path_to,
			      entropy_gui_component_instance * instance)
{
  evfs_file_uri_path *uri_path_from;
  evfs_file_uri_path *uri_path_to;
  char uri_from[512];
  char uri_to[512];
  long id;



  snprintf (uri_from, 512, "%s://%s/%s", file->uri_base, file->path,
	    file->filename);
  snprintf (uri_to, 512, "%s/%s", path_to, file->filename);

  uri_path_from = evfs_parse_uri (uri_from);
  uri_path_to = evfs_parse_uri (uri_to);

  /*Track the move action */
  id = evfs_client_file_move (con, uri_path_from->files[0], uri_path_to->files[0]);
  ecore_hash_set(evfs_dir_requests, (long*)id, instance);

  evfs_cleanup_file_uri_path(uri_path_from);
  evfs_cleanup_file_uri_path(uri_path_to);
}



/*
 * Remove file function
 */
void
entropy_filesystem_file_remove (entropy_generic_file * file, entropy_gui_component_instance* instance)
{
  evfs_file_uri_path *uri_path_from;
  char *uri = entropy_core_generic_file_uri_create (file, 0);
  long id;

  uri_path_from = evfs_parse_uri (uri);
  id = evfs_client_file_remove (con, uri_path_from->files[0]);

  ecore_hash_set (evfs_dir_requests, (long*)id, instance);

  free (uri);
  evfs_cleanup_file_uri_path(uri_path_from);

}


/*
 * Directory create function
 */
void
entropy_filesystem_directory_create (entropy_generic_file * parent, const char* child_name)
{
  evfs_file_uri_path *uri_path_from;
  entropy_generic_file* new_dir;



  new_dir = entropy_generic_file_clone(parent);
  snprintf(new_dir->path, PATH_MAX, "%s/%s", parent->path, parent->filename);
  snprintf(new_dir->filename, FILENAME_LENGTH, "%s", child_name);
  if (new_dir->uri)
	  free(new_dir->uri);

  new_dir->uri = entropy_core_generic_file_uri_create (new_dir, 0);

  if (new_dir->md5)
	  free(new_dir->md5);

  new_dir->md5 = md5_entropy_path_file(new_dir->uri_base, new_dir->path, new_dir->filename);
  

  uri_path_from = evfs_parse_uri (new_dir->uri);
  printf("Making directory '%s'....\n", uri_path_from->files[0]->path);

  evfs_client_directory_create (con, uri_path_from->files[0]);


  entropy_generic_file_destroy(new_dir);
}


/*
 * Rename file function
 */
void
entropy_filesystem_file_rename (entropy_generic_file * file_from,
				entropy_generic_file * file_to)
{
  evfs_file_uri_path *uri_path_from;
  evfs_file_uri_path *uri_path_to;

  char *uri_from = entropy_core_generic_file_uri_create (file_from, 0);
  char *uri_to = entropy_core_generic_file_uri_create (file_to, 0);

  printf("Uri from/to: %s -> %s\n", uri_from, uri_to);

  

  uri_path_from = evfs_parse_uri (uri_from);
  uri_path_to = evfs_parse_uri (uri_to);

  evfs_client_file_rename (con, uri_path_from->files[0],
			   uri_path_to->files[0]);


  free (uri_from);
  free (uri_to);
  free (uri_path_from);
  free (uri_path_to);

}

void entropy_filesystem_file_group_add(entropy_generic_file* file, char* group)
{
  evfs_file_uri_path *uri_path_from;
  char* uri_from;
  
  uri_from = entropy_core_generic_file_uri_create (file, 0);
  uri_path_from = evfs_parse_uri (uri_from);

  evfs_client_metadata_group_file_add(con, uri_path_from->files[0], group);

  free(uri_from);
  free(uri_path_from);
  
}

void entropy_filesystem_file_group_remove(entropy_generic_file* file, char* group)
{
  evfs_file_uri_path *uri_path_from;
  char* uri_from;
  
  uri_from = entropy_core_generic_file_uri_create (file, 0);
  uri_path_from = evfs_parse_uri (uri_from);

  evfs_client_metadata_group_file_remove(con, uri_path_from->files[0], group);

  free(uri_from);
  evfs_cleanup_file_uri_path(uri_path_from); 
}

void entropy_filesystem_auth_response(char* location, char* user, char* password)
{
	printf("Parsing uri %s\n", location);
	EvfsFilereference* ref = evfs_parse_uri_single(location);
	evfs_client_auth_send(con,ref,user,password);

	
}

void entropy_filesystem_operation_respond(long id, int response) 
{
	printf("Received response for %ld -> %d\n", id, response);
	evfs_client_operation_respond(con, id, response);
}


/*Metadata file groups get*/
void 
entropy_filesystem_metadata_groups_get(entropy_gui_component_instance* instance)
{
	long id = evfs_client_metadata_groups_get(con);
	ecore_hash_set (evfs_dir_requests, (long*)id, instance);
}

Ecore_List*
entropy_filesystem_metadata_groups_retrieve()
{
	return metadata_groups;
}


void entropy_filesystem_file_trash_restore (Ecore_List* files, entropy_gui_component_instance * instance)
{
  long id;
  int flag;
  entropy_generic_file* file;
  Ecore_List* evfs_files;
  EvfsFilereference* ref;
  char path[PATH_MAX];

  flag = 0;

  evfs_files = ecore_list_new();
  ecore_list_first_goto(files);
  while ( (file = ecore_list_next(files)) ) { 
	  if (file->attach) {
		  snprintf(path, PATH_MAX, "%s%s%s", URI_POSIX, "://", file->attach);
		  printf("Parsing %s\n", path);
		  if ( (ref =  evfs_parse_uri_single(path))) {
			  ecore_list_append(evfs_files, ref);
		  }
		  flag++;
	  }
  }
  
  if (flag) {
	  /*Track the restore action */
	  id = evfs_client_file_trash_restore (con, evfs_files);
	  ecore_hash_set(evfs_dir_requests, (long*)id, instance);
	
	  ecore_list_destroy(evfs_files); 
  } else {
	  printf("No files with attached uris to de-trash\n");
  }
}

void entropy_filesystem_meta_all_retrieve(entropy_gui_component_instance* instance)
{
	long id;
	
	id = evfs_client_meta_list_all(con);
	ecore_hash_set (evfs_dir_requests, (long*)id, instance);
}

void entropy_filesystem_vfolder_create(char* name, Eina_List* entries)
{
	evfs_command* com;
	EvfsVfolderEntry* e;
	Eina_List* l;

	com = evfs_vfolder_create_command_new(name);
	for (l=entries;l;) {
		e=l->data;
	
		evfs_vfolder_command_entry_add(com,e->type,e->name,e->value);
		
		l=l->next;
	}
	evfs_vfolder_command_send(con,com);
}
