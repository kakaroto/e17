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

#define NO_HIDDEN 1
#define URI_POSIX "file"



Ecore_Hash *folder_monitor_hash;
Ecore_Hash *stat_request_hash;
Ecore_Hash *file_copy_hash;
Ecore_Hash *evfs_dir_requests;
entropy_core *filesystem_core;	/*A reference to the core */


//TODO: In its current implementation - stats can only be tracked by one instance at a time.
//      I.e. if a request is made for a file by one instance, and another instance requests a stat on that file,
//      whilst we're waiting for a response on the first, we'll have a collision.  This will almost *never* happen, 
//      but it would be a good idea to fix.

evfs_connection *con;

void
callback (evfs_event * data, void *obj)
{

  //printf("Callback hit..%d\n", data->type);


  switch (data->type) {
  case EVFS_EV_SUB_MONITOR_NOTIFY:{
      /*Find a better way to do this, possibly modify evfs to pass a reference object */
      Ecore_List *watchers;
      void *key;
      entropy_gui_event *gui_event;

      char *folder = strdup ((char *) data->file_monitor.filename);
      char *pos = rindex (folder, '/');
      *pos = '\0';

      //printf("DEMO: Received a file monitor notification\n");
      //printf("DEMO: For file: '%s'\n", (char*)data->file_monitor.filename);
      //printf("DEMO: Directory '%s\n", folder);
      //printf("DEMO: Plugin: '%s'\n", (char*)data->file_monitor.plugin);
      //

      printf("Got a monitor event for folder '%s'..\n", folder);


      watchers = ecore_hash_keys (folder_monitor_hash);
      ecore_list_goto_first (watchers);
      while ((key = ecore_list_next (watchers))) {
	printf("Scanning watcher... %s:%s\n", ((evfs_file_uri_path *)
	      ecore_hash_get (folder_monitor_hash, key))->files[0]->path, folder );
	      
	if (!strcmp
	    (((evfs_file_uri_path *)
	      ecore_hash_get (folder_monitor_hash, key))->files[0]->path,
	     folder)) {
	  char *md5 = md5_entropy_path_file (data->file_monitor.plugin, folder, pos + 1);
	  entropy_file_listener *listener;


	  printf("Creating event for instance/layout %p..\n", key);


	  entropy_generic_file *file;
	  if (!(listener = entropy_core_file_cache_retrieve (md5))) {
	    file = entropy_generic_file_new ();
	    /*For now, just make an entropy_generic_file for this object */

	    strncpy (file->path, folder, 255);
	    strncpy (file->filename, pos + 1, strlen (pos + 1) + 1);
	    strncpy (file->uri_base, data->file_monitor.plugin,
		     strlen (data->file_monitor.plugin));
	    file->md5 = strdup (md5);
	    
	    if (data->file_monitor.filetype == EVFS_FILE_DIRECTORY) {
		    strcpy(file->mime_type, "file/folder");
		    file->filetype = FILE_FOLDER;
	    }

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

	  switch (data->file_monitor.fileev_type) {
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


      char *md5;
      char *folder =
	strdup ((char *) data->resp_command.file_command.files[0]->path);
      char *pos = rindex (folder, '/');



      *pos = '\0';

      //printf("Folder: '%s'\nFilename: '%s'\n", folder, pos+1);
      md5 = md5_entropy_path_file (data->resp_command.file_command.files[0]->plugin_uri, folder, pos + 1);
      instance = ecore_hash_get (stat_request_hash, md5);

      /*Build a file<->stat structure to pass to requester */
      file_stat = entropy_malloc (sizeof (entropy_file_stat));
      file_stat->stat_obj = entropy_malloc (sizeof (struct stat));


      file_stat->stat_obj->st_uid = data->stat.stat_obj.st_uid;
      file_stat->stat_obj->st_gid = data->stat.stat_obj.st_gid;
      file_stat->stat_obj->st_size = data->stat.stat_obj.st_size;
      file_stat->stat_obj->st_atime = data->stat.stat_obj.ist_atime;
      file_stat->stat_obj->st_mtime = data->stat.stat_obj.ist_mtime;
      file_stat->stat_obj->st_ctime = data->stat.stat_obj.ist_ctime;

      /*Retrieve the file. This is bad - the file might not exist anymore! */
      listener = entropy_core_file_cache_retrieve (md5);
      if (listener) {
	      file_stat->file = listener->file;

	      /*Pop stats*/
	      listener->file->properties.st_uid = data->stat.stat_obj.st_uid;
	      listener->file->properties.st_gid = data->stat.stat_obj.st_gid;
	      listener->file->properties.st_size = data->stat.stat_obj.st_size;
	      listener->file->properties.st_atime = data->stat.stat_obj.ist_atime;
	      listener->file->properties.st_mtime = data->stat.stat_obj.ist_mtime;
	      listener->file->properties.st_ctime = data->stat.stat_obj.ist_ctime;


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
	      printf("Error! Couldn't fine listener for '%s'\n", data->resp_command.file_command.files[0]->path);
      }

      /*Do some freeing */
      entropy_free (file_stat);
      entropy_free (file_stat->stat_obj);
      ecore_hash_remove (stat_request_hash, md5);
      entropy_free (folder);
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
      evfs_filereference *ref;
      char *filename;
      char *folder;
      char *pos;

      //printf("Received a directory listing..\nFiles:\n\n");
      //printf("Original uri: %s\n", data->resp_command.file_command.files[0]->path);

      if (!data->file_list.list) {
	//printf("No directory received - invalid uri?\n");
	ecore_list_destroy (file_list);
	return;
      }
      else {
	//printf("File list at %p\n", data->file_list.list);
      }

      ecore_list_goto_first (data->file_list.list);
      while ((ref = ecore_list_next (data->file_list.list))) {
	/*printf("(%s) Received file type for file: %d\n", ref->path, ref->file_type); */

	folder = strdup ((char *) ref->path);
	pos = rindex (folder, '/');
	*pos = '\0';

	filename = strdup (pos + 1);

	/*If we are the root dir (i.e. we only have 
	 * one "/", replace it (so we can use it below),
	 * and assume this filename has length (BAD).  */
	if (!strlen (folder)) {
	  *pos = '/';
	  *(pos + 1) = '\0';
	}

	/*printf("Calling folder is '%s'\n", folder); */

	/*If the calling request is currently NULL, we must go to the hash to retrieve that caller */
	if (!calling_request) {

	  printf("Looking for calling request with folder '%s'\n", folder);
	  calling_request = ecore_hash_get (evfs_dir_requests, folder);
	  printf("Received a file from the hash, path is '%s'\n", calling_request->file->path);

	  /*Append the original request as the first item in the list */
	  /*It's not nice having to do this, but we're kind of stuck.  Should be ok as long
	   * as it gets documented*/
	  ecore_list_append (file_list, calling_request);

	}

	//printf("Folder name: '%s', filename '%s'\n", folder, pos+1);

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
	  listener->count = 0;
	  entropy_core_file_cache_add (md5, listener);


	}
	else {
	  file = listener->file;
	  entropy_free (md5);	/*We don't need this one, we're returning an old ref */
	}

	free (folder);
	free (filename);


	/*Add this file to our list */
	ecore_list_append (file_list, file);
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

    }

    break;
  case EVFS_EV_FILE_PROGRESS:{
      entropy_gui_event *gui_event;
      entropy_gui_component_instance *instance = NULL;
      entropy_file_progress *request =
	entropy_malloc (sizeof (entropy_file_progress));
      char *uri = NULL;

      ecore_list_goto_first(data->file_list.list);
      request->file_from = evfs_filereference_to_entropy_generic_file(ecore_list_current(data->file_list.list));
      ecore_list_next(data->file_list.list);
      request->file_to = evfs_filereference_to_entropy_generic_file(ecore_list_current(data->file_list.list));
      request->progress = data->progress->file_progress;

      if (data->progress->type == EVFS_PROGRESS_TYPE_CONTINUE)
	request->type = TYPE_CONTINUE;
      else
	request->type = TYPE_END;

      /*Find who called us */
      uri =
	evfs_filereference_to_string (data->resp_command.file_command.
				      files[0]);
      instance = ecore_hash_get (file_copy_hash, uri);

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

      if (data->progress->type == EVFS_PROGRESS_TYPE_DONE) {
	/*TODO free the key */

      }

      free (uri);
      entropy_generic_file_destroy (request->file_from);
      entropy_generic_file_destroy (request->file_to);
      free (request);
    }
    break;

  case EVFS_EV_OPERATION: {
	char *uri = NULL;
	entropy_gui_component_instance* instance;
	entropy_gui_event* gui_event;
	entropy_file_operation* op;

	printf("EVFS requested feedback on an operation!\n");

      /*Find who called us */
      uri =
	evfs_filereference_to_string (data->resp_command.file_command.
				      files[0]);
      instance = ecore_hash_get (file_copy_hash, uri);

      if (instance) {
         /*Build up the gui_event wrapper */
         gui_event = entropy_malloc (sizeof (entropy_gui_event));
         gui_event->event_type =
	 entropy_core_gui_event_get (ENTROPY_GUI_EVENT_USER_INTERACTION_YES_NO_ABORT);

	 op = entropy_malloc(sizeof(entropy_file_operation));
	 op->file = data->op->misc_str;
	 op->id = data->op->id;
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
    while ( (key = ecore_list_remove_first(keys))) {
	    compare = ecore_hash_get(folder_monitor_hash, key);
	    if (compare && evfs_filereference_equal_is(dir->files[0], compare->files[0]))
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

void
entropy_plugin_init (entropy_core * core)
{
  folder_monitor_hash =
    ecore_hash_new (ecore_direct_hash, ecore_direct_compare);
  stat_request_hash = ecore_hash_new (ecore_str_hash, ecore_str_compare);
  evfs_dir_requests = ecore_hash_new (ecore_str_hash, ecore_str_compare);
  file_copy_hash = ecore_hash_new (ecore_str_hash, ecore_str_compare);

  con = evfs_connect (&callback, NULL);

  filesystem_core = core;
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
  evfs_file_uri_path *path;
  char *uri = uri = entropy_core_generic_file_uri_create (request->file, 0);

  //printf("Getting a stat from evfs...\n");


  path = evfs_parse_uri (uri);
  char *md5 =
    md5_entropy_path_file (request->file->uri_base, request->file->path, request->file->filename);
  ecore_hash_set (stat_request_hash, md5, request->requester);
  evfs_client_file_stat (con, path->files[0]);

  free (uri);


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
  evfs_file_uri_path *dir_path;
  entropy_file_listener *listener;

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
	  && (!NO_HIDDEN || de->d_name[0] != '.')) {
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
	    listener->count = 0;


	    entropy_core_file_cache_add (md5, listener);
	  }
	  else {
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
    evfs_client_dir_list (con, path->files[0]);


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

    /*If it is a drilldown request - we must be at the root.
     *Anything else, and we must be an 'embedded' request with a parent*/
    if (request->drill_down) {
      ecore_hash_set (evfs_dir_requests, "/", new_request);
    }
    else {
      printf("Setting dir request hash to '%s'\n", path->files[0]->path);
      ecore_hash_set (evfs_dir_requests, path->files[0]->path, new_request);
    }

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
  char *original;

  char copy_buffer[PATH_MAX];

  char uri_from[512];
  char uri_to[512];



  snprintf (uri_from, 512, "%s://%s/%s", file->uri_base, file->path,
	    file->filename);
  snprintf (uri_to, 512, "%s/%s", path_to, file->filename);

  //printf ("Copying file %s to %s\n", uri_from, uri_to);



  uri_path_from = evfs_parse_uri (uri_from);
  uri_path_to = evfs_parse_uri (uri_to);


  //printf("Original uri is: '%s'\n", original);
  //
  /*Track the copy action */
  snprintf (copy_buffer, PATH_MAX, "%s%s", uri_from, uri_to);
  original = evfs_filereference_to_string (uri_path_from->files[0]);
  ecore_hash_set (file_copy_hash, original, instance);

  evfs_client_file_copy (con, uri_path_from->files[0], uri_path_to->files[0]);

  //TODO - free the file containers here
}


/*
 * Remove file function
 */
void
entropy_filesystem_file_remove (entropy_generic_file * file)
{
  evfs_file_uri_path *uri_path_from;

  char *uri = entropy_core_generic_file_uri_create (file, 0);
  //printf("Deleting uri '%s'\n", uri);

  uri_path_from = evfs_parse_uri (uri);
  evfs_client_file_remove (con, uri_path_from->files[0]);


  free (uri);

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

  uri_path_from = evfs_parse_uri (uri_from);
  uri_path_to = evfs_parse_uri (uri_to);


  evfs_client_file_rename (con, uri_path_from->files[0],
			   uri_path_to->files[0]);


  free (uri_from);
  free (uri_to);
  free (uri_path_from);
  free (uri_path_to);

}

void entropy_filesystem_operation_respond(long id, int response) 
{
	printf("Received response for %ld -> %d\n", id, response);
	evfs_client_operation_respond(con, id, response);
}
