#include "entropy.h"

static Ecore_List *types = NULL;

int
entropy_plugin_type_get ()
{
  return ENTROPY_PLUGIN_THUMBNAILER;
}

char *
entropy_plugin_identify ()
{
  return (char *) "Simple system thumbnailer (folders, known types, etc)";
}

Ecore_List *
entropy_thumbnailer_plugin_mime_types_get ()
{
  if (types == NULL) {
    //printf("Making new ecore_list..\n");
    types = ecore_list_new ();
    ecore_list_append (types, "file/folder");
    ecore_list_append (types, "text/x-perl");
    ecore_list_append (types, "video/x-ms-wmv");
    ecore_list_append (types, "application/msword");
    ecore_list_append (types, "application/pdf");
    ecore_list_append (types, "application/vnd.ms-excel");
    ecore_list_append (types, "application/x-gtar");
    ecore_list_append (types, "audio/x-mp3");
    ecore_list_append (types, "text/x-java");
    ecore_list_append (types, "application/x-jar");
    ecore_list_append (types, "text/xml");
    ecore_list_append (types, "text/html");
    ecore_list_append (types, "text/csrc");
    ecore_list_append (types, "video/mpeg");
    ecore_list_append (types, "application/x-bzip2");
    ecore_list_append (types, "application/x-tar");
    ecore_list_append (types, "text/plain");
    ecore_list_append (types, "video/x-msvideo");
    ecore_list_append (types, "video/quicktime");

  }

  return types;
}

entropy_thumbnail *
entropy_thumbnailer_thumbnail_get (entropy_thumbnail_request * request)
{
  if (request->file->thumbnail) {
    return request->file->thumbnail;
  }

  entropy_thumbnail *thumb = entropy_thumbnail_new ();

  if (!strcmp (request->file->mime_type, "file/folder")) {
    if (!request->file->attach) {
	    strcpy (thumb->thumbnail_filename, PACKAGE_DATA_DIR "/icons/folder.png");
    } else {
	    printf("Object has attach data: %s\n", request->file->attach);
	    if (!strcmp(request->file->attach, "video_hint")) 
		    strcpy (thumb->thumbnail_filename, PACKAGE_DATA_DIR "/icons/hint_videos.png");
	    else if (!strcmp(request->file->attach, "audio_hint")) 
		    strcpy (thumb->thumbnail_filename, PACKAGE_DATA_DIR "/icons/hint_audio.png");
	    else if (!strcmp(request->file->attach, "image_hint"))
		    strcpy (thumb->thumbnail_filename, PACKAGE_DATA_DIR "/icons/hint_image.png");
    }
  }
  else if (!strcmp (request->file->mime_type, "text/x-perl")) {
    strcpy (thumb->thumbnail_filename, PACKAGE_DATA_DIR "/icons/perl.png");
  }
  else if (!strcmp (request->file->mime_type, "video/x-ms-wmv")) {
    strcpy (thumb->thumbnail_filename, PACKAGE_DATA_DIR "/icons/wmv.png");
  }
  else if (!strcmp (request->file->mime_type, "application/msword")) {
    strcpy (thumb->thumbnail_filename,
	    PACKAGE_DATA_DIR "/icons/document.png");
  }
  else if (!strcmp (request->file->mime_type, "application/pdf")) {
    strcpy (thumb->thumbnail_filename, PACKAGE_DATA_DIR "/icons/pdf.png");
  }
  else if (!strcmp (request->file->mime_type, "application/vnd.ms-excel")) {
    strcpy (thumb->thumbnail_filename,
	    PACKAGE_DATA_DIR "/icons/spreadsheet.png");
  }
  else if (!strcmp (request->file->mime_type, "application/x-gtar")) {
    strcpy (thumb->thumbnail_filename, PACKAGE_DATA_DIR "/icons/tgz.png");
  }
  else if (!strcmp (request->file->mime_type, "audio/x-mp3")) {
    strcpy (thumb->thumbnail_filename, PACKAGE_DATA_DIR "/icons/mp3.png");
  }
  else if (!strcmp (request->file->mime_type, "text/x-java")) {
    strcpy (thumb->thumbnail_filename, PACKAGE_DATA_DIR "/icons/java.png");
  }
  else if (!strcmp (request->file->mime_type, "text/xml")) {
    strcpy (thumb->thumbnail_filename, PACKAGE_DATA_DIR "/icons/xml.png");
  }
  else if (!strcmp (request->file->mime_type, "application/x-jar")) {
    strcpy (thumb->thumbnail_filename, PACKAGE_DATA_DIR "/icons/jar.png");
  }
  else if (!strcmp (request->file->mime_type, "text/html")) {
    strcpy (thumb->thumbnail_filename, PACKAGE_DATA_DIR "/icons/html.png");
  }
  else if (!strcmp (request->file->mime_type, "text/csrc")) {
    strcpy (thumb->thumbnail_filename, PACKAGE_DATA_DIR "/icons/c.png");
  }
  else if (!strcmp (request->file->mime_type, "video/mpeg")) {
    strcpy (thumb->thumbnail_filename, PACKAGE_DATA_DIR "/icons/mpeg.png");
  }
  else if (!strcmp (request->file->mime_type, "application/x-bzip2")) {
    strcpy (thumb->thumbnail_filename, PACKAGE_DATA_DIR "/icons/bz2.png");
  }
  else if (!strcmp (request->file->mime_type, "application/x-tar")) {
    strcpy (thumb->thumbnail_filename, PACKAGE_DATA_DIR "/icons/tar.png");
  }
  else if (!strcmp (request->file->mime_type, "text/plain")) {
    strcpy (thumb->thumbnail_filename, PACKAGE_DATA_DIR "/icons/txt.png");
  }
  else if (!strcmp (request->file->mime_type, "video/x-msvideo")) {
    strcpy (thumb->thumbnail_filename, PACKAGE_DATA_DIR "/icons/video.png");
  }
  else if (!strcmp (request->file->mime_type, "video/quicktime")) {
    strcpy (thumb->thumbnail_filename, PACKAGE_DATA_DIR "/icons/video.png");
  }

  thumb->parent = request->file;
  request->file->thumbnail = thumb;

  return thumb;
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

  return instance;
}


void
gui_event_callback (entropy_notify_event * eevent, void *requestor, void *obj,
		    entropy_gui_component_instance * comp)
{
}
