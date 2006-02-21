#include "entropy.h"
#include <dlfcn.h>
#include "entropy_gui.h"
#include <limits.h>



static Ecore_List *types = NULL;
static Ecore_Hash *file_instance_hash = NULL;
static entropy_generic_file *tmp_file;

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

  }

  return types;
}

entropy_thumbnail *
entropy_thumbnailer_thumbnail_get (entropy_thumbnail_request * request)
{
  entropy_thumbnail *thumb = NULL;
  entropy_plugin *plugin =
    entropy_plugins_type_get_first (ENTROPY_PLUGIN_BACKEND_FILE,
				    ENTROPY_PLUGIN_SUB_TYPE_ALL);
  void (*copy_func) (entropy_generic_file * source, char *dest_uri,
		     entropy_gui_component_instance * requester);
  copy_func = dlsym (plugin->dl_ref, "entropy_filesystem_file_copy");

  /*This thumbnailer is only for remotel files */
  if (!strcmp (request->file->uri_base, "file"))
    return NULL;

  /*If we already have a thumbnail, don't bother remaking it */
  if (request->file->thumbnail) {
    return request->file->thumbnail;
  }

  //printf("Adding reference to '%s'\n", request->file->md5);
  entropy_core_file_cache_add_reference (request->file->md5);


  //printf("Requested a remote thumbnail for '%s://%s/%s\n", request->file->uri_base, request->file->path, request->file->filename);

  (*copy_func) (request->file, TMP_THUMBNAIL, local_instance);
  ecore_hash_set (file_instance_hash, request->file->md5, request->instance);
  //printf("md5: '%s'\n", md5);

  return NULL;
}


entropy_gui_component_instance *
entropy_plugin_init (entropy_core * core)
{

  entropy_gui_component_instance *instance =
    entropy_gui_component_instance_new ();
  entropy_gui_component_instance *layout =
    entropy_core_global_layout_get (core);
  instance->layout_parent = layout;
  instance->core = core;
  local_instance = instance;


  file_instance_hash = ecore_hash_new (ecore_str_hash, ecore_str_compare);

  tmp_file = entropy_generic_file_new ();
  strcpy (tmp_file->uri_base, "file");
  strcpy (tmp_file->path, "/tmp");

  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_PROGRESS));
  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_USER_INTERACTION_YES_NO_ABORT));

  return instance;
}

void
gui_event_callback (entropy_notify_event * eevent, void *requestor, void *obj,
		    entropy_gui_component_instance * comp)
{

  switch (eevent->event_type) {
  case ENTROPY_NOTIFY_USER_INTERACTION_YES_NO_ABORT: {
	/*Always overwrite here - we're in /tmp, and we don't want to prompt
	 * the user*/
        void (*operation_func)(long id,int response);
        entropy_plugin* plugin;
	long id = (long)obj;

        //FIXME
        plugin = entropy_plugins_type_get_first( ENTROPY_PLUGIN_BACKEND_FILE ,ENTROPY_PLUGIN_SUB_TYPE_ALL);
        operation_func = dlsym(plugin->dl_ref, "entropy_filesystem_operation_respond");
        (*operation_func)( id, ENTROPY_USER_INTERACTION_RESPONSE_YES );

  }
  case ENTROPY_NOTIFY_FILE_PROGRESS:{
      entropy_file_progress *progress = obj;

      if (progress->type == TYPE_END) {
	entropy_file_listener *listener;
	char new_path[PATH_MAX];
	entropy_thumbnail *thumb;
	entropy_gui_event *gui_event;
	char *md5 = NULL;
	entropy_plugin *plugin;

	entropy_gui_component_instance *instance = NULL;

	//printf("From: %s/%s\n", progress->file_from->path, progress->file_from->filename);
	
	md5 = md5_entropy_path_file (progress->file_from->uri_base, progress->file_from->path, progress->file_from->filename);
	instance = ecore_hash_get (file_instance_hash, md5);
	//printf("Remote thumbnailer file copy finish! - %p\n", instance);      

	/*Actually make the thumbnail */
	if (instance && (listener = entropy_core_file_cache_retrieve (md5))) {
	  snprintf (new_path, PATH_MAX, "/tmp/%s", progress->file_from->filename);

	  strncpy (tmp_file->filename, listener->file->filename,
		   FILENAME_LENGTH);
	  //printf("Thumbnailing: %s/%s\n", tmp_file->path, tmp_file->filename);


	  thumb = entropy_thumbnail_create (tmp_file);

	  if (thumb) {
	    listener->file->thumbnail = thumb;
	    thumb->parent = listener->file;

	    /*Build up the gui_event wrapper */
	    gui_event = entropy_malloc (sizeof (entropy_gui_event));
	    gui_event->event_type =
	      entropy_core_gui_event_get
	      (ENTROPY_GUI_EVENT_THUMBNAIL_AVAILABLE);
	    gui_event->data = thumb;
	    //
	    /*Call the callback stuff */
	    entropy_core_layout_notify_event (instance, gui_event,
					      ENTROPY_EVENT_LOCAL);

	    //printf("Removing reference to '%s'\n", listener->file->md5);
	    entropy_core_file_cache_remove_reference (listener->file->md5);

	    /*Remove the d/led image */
	    plugin =
	      entropy_plugins_type_get_first (ENTROPY_PLUGIN_BACKEND_FILE,
					      ENTROPY_PLUGIN_SUB_TYPE_ALL);

	    entropy_plugin_filesystem_file_remove (plugin, tmp_file);
	  }
	  else {
	    printf ("Remote thumbnailer: Couldn't make thumbnail\n");
	  }
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
