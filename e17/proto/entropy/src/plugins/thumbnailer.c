#include "entropy.h"

static Ecore_List *types = NULL;
#define THUMBNAILER_LOCAL_PLUGIN "posix"

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

entropy_gui_component_instance *
entropy_plugin_init (entropy_core * core)
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
  entropy_thumbnail *thumb;

  /*This thumbnailer is only for local files */
  if (strcmp (request->file->uri_base, "posix"))
    return NULL;

  if (!request->file->thumbnail) {
    thumb = entropy_thumbnail_create (request->file);
  }
  else {
    return request->file->thumbnail;
  }
  /*Set the file up for this thumbnail. TODO this probably violates convention to do this here,
   * but we create the thumbnail downstream, and from here, so there's not much choice.. */

  if (thumb) {
    /*printf("Created thumbnail '%s'\n", thumb->thumbnail_filename); */
    thumb->parent = request->file;
    request->file->thumbnail = thumb;
  }
  else {
    //printf ("Returned thumb was null, assuming error...\n");
    return NULL;
  }

  return thumb;
}

void
gui_event_callback (entropy_notify_event * eevent, void *requestor, void *obj,
		    entropy_gui_component_instance * comp)
{
}
