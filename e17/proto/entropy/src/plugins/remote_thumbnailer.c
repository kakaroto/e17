#include "entropy.h"
#include <dlfcn.h>
#include "entropy_gui.h"
#include <limits.h>
#include "Epsilon_Request.h"
#include <Epsilon.h>


static Ecore_List *types = NULL;
static Ecore_Hash *file_instance_hash = NULL;
static Ecore_Hash *_ecore_thumb_file_instance_hash = NULL;
static Ecore_Event_Handler *thumb_done = NULL;

/*There will only ever be one instance of a thumbnail plugin, therefore this is safe*/
static entropy_gui_component_instance *local_instance;


#define THUMBNAILER_LOCAL_PLUGIN "file"
#define TMP_THUMBNAIL "file:///tmp/"

int
entropy_plugin_type_get ()
{
  return ENTROPY_PLUGIN_THUMBNAILER;
}

char *
entropy_plugin_identify ()
{
  return (char *) "Simple MIME plugin for images";
}

Ecore_List *
entropy_thumbnailer_plugin_mime_types_get ()
{
  if (types == NULL) {
    //printf("Making new ecore_list..\n");
    types = ecore_list_new ();
    ecore_list_append (types, "image/jpeg");
    ecore_list_append (types, "image/png");
    ecore_list_append (types, "image/gif");
    ecore_list_append (types, "application/pdf");

  }

  return types;
}

int thumb_complete_remote_cb(void *data, int type, void *event)
{
        Epsilon_Request *thumb = event;
	entropy_thumbnail* thumbnail;
	entropy_gui_event* gui_event;
	entropy_gui_component_instance* instance = NULL;
	entropy_file_request* req;
	char* md5;

	if (!thumb) {
		printf("Received NULL thumbnail - abort!\n");
		return 0;
	}

	md5 = md5_entropy_local_file(thumb->path);

	req = ecore_hash_get(_ecore_thumb_file_instance_hash, md5);	
	if (req && thumb->dest) {
		instance = req->requester;
		ecore_hash_remove(_ecore_thumb_file_instance_hash, md5);

		thumbnail = entropy_thumbnail_new();
		strcpy(thumbnail->thumbnail_filename, thumb->dest);
		thumbnail->parent = req->file;
		req->file->thumbnail = thumbnail;

	    gui_event = entropy_malloc (sizeof (entropy_gui_event));
	    gui_event->event_type =
	      entropy_core_gui_event_get
	      (ENTROPY_GUI_EVENT_THUMBNAIL_AVAILABLE);
	    gui_event->data = thumbnail;

	    /*Remove the original file*/
	    entropy_plugin_filesystem_file_remove (req->file2, instance);

	    /*Free tmp file*/
	    entropy_generic_file_destroy(req->file2);


	    /*Call the callback stuff */
	    entropy_core_layout_notify_event (instance, gui_event,
					      ENTROPY_EVENT_LOCAL);

	    entropy_core_file_cache_remove_reference(req->file->md5);

	    free(req);
	    free(md5);
	} else {
		/*Another thumbnailer?*/
		return 1;
	}

        return 0;
}



entropy_thumbnail *
entropy_thumbnailer_thumbnail_get (entropy_thumbnail_request * request)
{
  /*This thumbnailer is only for remote files */
  if (!strcmp (request->file->uri_base, "file"))
    return NULL;

  /*If we already have a thumbnail, don't bother remaking it */
  if (request->file->thumbnail) {
    return request->file->thumbnail;
  }

  entropy_core_file_cache_add_reference (request->file->md5);

  entropy_plugin_filesystem_file_copy (request->file, TMP_THUMBNAIL, local_instance);
  ecore_hash_set (file_instance_hash, request->file->md5, request->instance);

  return NULL;
}


entropy_plugin*
entropy_plugin_init(entropy_core* core)
{
	entropy_plugin* plugin;

	plugin = entropy_malloc(sizeof(Entropy_Plugin_Thumbnailer_Child));

	return plugin;
}

entropy_gui_component_instance *
entropy_plugin_gui_instance_new (entropy_core * core)
{

  entropy_gui_component_instance *instance =
    entropy_gui_component_instance_new ();
  entropy_gui_component_instance *layout =
    entropy_core_global_layout_get (core);
  instance->layout_parent = layout;
  instance->core = core;
  local_instance = instance;


  file_instance_hash = ecore_hash_new (ecore_str_hash, ecore_str_compare);
  _ecore_thumb_file_instance_hash = ecore_hash_new (ecore_str_hash, ecore_str_compare);

  ecore_hash_free_key_cb_set(_ecore_thumb_file_instance_hash, free);

  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_PROGRESS));
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_USER_INTERACTION_YES_NO_ABORT));

  thumb_done = ecore_event_handler_add(EPSILON_EVENT_DONE, thumb_complete_remote_cb, NULL);

  return instance;
}

void
gui_event_callback (entropy_notify_event * eevent, void *requestor, void *obj,
		    entropy_gui_component_instance * comp)
{
  entropy_file_progress *progress;

  switch (eevent->event_type) {
  case ENTROPY_NOTIFY_USER_INTERACTION_YES_NO_ABORT: {
	/*Always overwrite here - we're in /tmp, and we don't want to prompt
	 * the user*/
	entropy_file_operation* op = (entropy_file_operation*)obj;

        entropy_plugin_operation_respond( op->id, ENTROPY_USER_INTERACTION_RESPONSE_YES );

  }
  break; 

  case ENTROPY_NOTIFY_FILE_PROGRESS:{
      progress = obj;

      if (progress->type == TYPE_END) {
	entropy_file_listener *listener;
	entropy_file_request* o_request;
	char new_path[PATH_MAX];
	char *md5 = NULL;
	entropy_generic_file* tmp_file;

	entropy_gui_component_instance *instance = NULL;

	/*printf("From: %s/%s\n", progress->file_from->path, progress->file_from->filename);*/
	
	md5 = md5_entropy_path_file (progress->file_from->uri_base, progress->file_from->path, progress->file_from->filename);
	instance = ecore_hash_get (file_instance_hash, md5);
	//printf("Remote thumbnailer file copy finish! - %p\n", instance);      

	/*Actually make the thumbnail */
	if (instance && (listener = entropy_core_file_cache_retrieve (md5))) {
	  	  snprintf (new_path, PATH_MAX, "/tmp/%s", progress->file_from->filename);

		  tmp_file = entropy_generic_file_new ();
		  strcpy (tmp_file->uri_base, "file");
		  strcpy (tmp_file->path, "/tmp");
		  strncpy (tmp_file->filename, listener->file->filename,
			   FILENAME_LENGTH);
		  
		  o_request = entropy_malloc(sizeof(entropy_file_request));	
		  o_request->file = listener->file;
		  o_request->file2 = tmp_file;
		  o_request->requester = instance;
   
		  ecore_hash_set (_ecore_thumb_file_instance_hash, md5_entropy_local_file(new_path), o_request);

		  epsilon_request_add(new_path, EPSILON_THUMB_NORMAL, NULL);
		  
		  entropy_core_file_cache_remove_reference (listener->file->md5);
	}
	else {
	  printf ("Remote thumbnailer: Couldn't retrieve file reference\n");
	}

	ecore_hash_remove (file_instance_hash, md5);

	free (md5);
      }
    }
    break;

  }

}
