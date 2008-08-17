#include "entropy.h"
#include "Epsilon_Request.h"
#include <Epsilon.h>

static Ecore_List *types = NULL;
static Ecore_Event_Handler *thumb_done = NULL;
static Ecore_Hash *_ecore_thumb_file_instance_hash = NULL;
#define THUMBNAILER_LOCAL_PLUGIN "file"

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
    ecore_list_append (types, "video/x-ms-wmv");
    ecore_list_append (types, "video/mpeg");
    ecore_list_append (types, "video/x-msvideo");
    ecore_list_append (types, "video/quicktime");
    ecore_list_append (types, "image/tga");
    ecore_list_append (types, "application/pdf");


  }

  return types;
}

int thumb_complete_cb(void *data, int type, void *event)
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

	    /*Call the callback stuff */
	    entropy_core_layout_notify_event (instance, gui_event,
					      ENTROPY_EVENT_LOCAL);

		entropy_core_file_cache_remove_reference(req->file->md5);

		free(req);
		free(md5);
	} else {
		free(md5);
		if (req) free(req);
		/*Another thumbnailer?*/
		return 1;
	}
	

        return 0;
}

entropy_plugin*
entropy_plugin_init(entropy_core* core)
{
	Entropy_Plugin_Thumbnailer_Child* plugin;

	plugin = entropy_malloc(sizeof(Entropy_Plugin_Thumbnailer_Child));

	return ENTROPY_PLUGIN(plugin);
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

  _ecore_thumb_file_instance_hash = ecore_hash_new (ecore_str_hash, ecore_str_compare);

  epsilon_request_init();

  thumb_done = ecore_event_handler_add(EPSILON_EVENT_DONE, thumb_complete_cb, NULL);

  return instance;
}

entropy_thumbnail *
entropy_thumbnailer_thumbnail_get (entropy_thumbnail_request * request)
{
  char buffer[PATH_MAX];
  entropy_file_request* o_request;

  /*This thumbnailer is only for local files */
  if (strcmp (request->file->uri_base, THUMBNAILER_LOCAL_PLUGIN))
    return NULL;

  if (!request->file->thumbnail) {
    /*thumb = entropy_thumbnail_create (request->file);*/
    /*printf("Ecore thumb: request: '%s/%s'\n", request->file->path, request->file->filename);*/

    snprintf(buffer,PATH_MAX, "%s/%s", request->file->path, request->file->filename);

    /*FIXME this assumes that only one instance will request a thumb at one time*/
    o_request = entropy_malloc(sizeof(entropy_file_request));	
    o_request->file = request->file;
    o_request->requester = request->instance;
    
    ecore_hash_set (_ecore_thumb_file_instance_hash, request->file->md5, o_request);
    
    entropy_core_file_cache_add_reference(request->file->md5);

    /*printf("'%s' (%s)...\n", buffer, request->file->md5);*/
    epsilon_request_add(buffer, EPSILON_THUMB_NORMAL, NULL);
  }
  else {
    return request->file->thumbnail;
  }

  return NULL;
}

void
gui_event_callback (entropy_notify_event * eevent, void *requestor, void *obj,
		    entropy_gui_component_instance * comp)
{
}
