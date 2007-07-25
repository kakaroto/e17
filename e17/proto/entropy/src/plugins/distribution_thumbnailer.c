#include "entropy.h"
#include <dlfcn.h>

static Ecore_List *types = NULL;
#define THUMBNAILER_LOCAL_PLUGIN "file"

int
entropy_plugin_type_get ()
{
  return ENTROPY_PLUGIN_THUMBNAILER_DISTRIBUTION;
}

char *
entropy_plugin_identify ()
{
  return (char *) "Central object distribution thumbnailer";
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
    ecore_list_append (types, "image/tga");
    ecore_list_append (types, "video/x-ms-wmv");
    ecore_list_append (types, "video/mpeg");
    ecore_list_append (types, "video/x-msvideo");
    ecore_list_append (types, "video/quicktime");
  }

  return types;
}

entropy_plugin*
entropy_plugin_init(entropy_core* core)
{
	entropy_plugin* plugin;

	plugin = entropy_malloc(sizeof(Entropy_Plugin_Thumbnailer_Distribution));

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

  return instance;
}

entropy_thumbnail *
entropy_thumbnailer_thumbnail_get (entropy_thumbnail_request * request)
{
  entropy_thumbnail *thumb = NULL;
  Ecore_List *thumbnailers =
    entropy_thumbnailer_child_retrieve (request->file->mime_type);
  entropy_thumbnail *(*get) (entropy_thumbnail_request *);

  /*printf("Called distribution thumbnailer...\n"); */

  if (thumbnailers) {
    entropy_plugin *plugin;


    /*printf("Received %d thumbnailers for '%s'\n", ecore_list_count(thumbnailers), request->file->filename); */

    ecore_list_first_goto (thumbnailers);
    while (!thumb && (plugin = ecore_list_next (thumbnailers))) {
      get = dlsym (plugin->dl_ref, "entropy_thumbnailer_thumbnail_get");
      thumb = (*get) (request);
    }


  }



  return thumb;

}

void
gui_event_callback (entropy_notify_event * eevent, void *requestor, void *obj,
		    entropy_gui_component_instance * comp)
{
}
