#include "entropy.h"
#include "entropy_gui.h"
#include "entropy_config.h"
#include <unistd.h>





int
entropy_plugin_type_get ()
{
  return ENTROPY_PLUGIN_ACTION_PROVIDER;
}

char *
entropy_plugin_identify ()
{
  return (char *) "Simple MIME identifier";
}


void
gui_event_callback (entropy_notify_event * eevent, void *requestor, void *obj,
		    entropy_gui_component_instance * comp)
{
  char fullname[1024];
  entropy_gui_event *gui_event;
  entropy_core *core = ((entropy_gui_component_instance *) requestor)->core;
  entropy_mime_action *app;
  char *uri;
  char *pos;

  entropy_generic_file *file = (entropy_generic_file *) obj;

  if (!strcmp (file->mime_type, "file/folder") && !file->parent) {
    entropy_file_request *request =
      entropy_malloc (sizeof (entropy_file_request));
    request->file = file;

    //printf("Action on a folder - change dirs!\n\n");


    /*Send an event to the core */
    gui_event = entropy_malloc (sizeof (entropy_gui_event));
    gui_event->event_type =
      entropy_core_gui_event_get (ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS);
    gui_event->data = request;
    entropy_core_layout_notify_event ((entropy_gui_component_instance *)
				      requestor, gui_event,
				      ENTROPY_EVENT_LOCAL);

    return;
  }
  else if ((uri = entropy_core_descent_for_mime_get (core, file->mime_type))
	   || (file->parent && !strcmp (file->mime_type, "file/folder"))) {

    entropy_file_request *request =
      entropy_malloc (sizeof (entropy_file_request));

    printf ("Requested a list of a descendable object\n");
    /*printf("URI would be: '%s://%s/%s#tar:///'\n", file->uri_base, file->path, file->filename); */


    request->file = file;
    if (uri) {
      request->drill_down = 1;
    }
    else {
      request->drill_down = 0;
    }

    //printf("Action on a folder - change dirs!\n\n");


    /*Send an event to the core */
    gui_event = entropy_malloc (sizeof (entropy_gui_event));
    gui_event->event_type =
      entropy_core_gui_event_get (ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS);
    gui_event->data = request;
    entropy_core_layout_notify_event ((entropy_gui_component_instance *)
				      requestor, gui_event,
				      ENTROPY_EVENT_LOCAL);

    return;
  }





  /*First get the app associated with this mime type */
  app =
    entropy_core_mime_hint_get (((entropy_generic_file *) obj)->mime_type);
  if (app) {
    /*First do a replace */
    if ((pos = strstr (app->executable, "\%u"))) {
      bzero (fullname, 1024);
      uri = entropy_core_generic_file_uri_create (file, 0);

      printf ("Action '%s' contains a URI replace reference\n",
	      app->executable);

      /*This is some evil shit - TODO make a proper strreplace function */
      strncat (fullname, app->executable, pos - app->executable);
      strcat (fullname, uri);
      pos += 2;
      strcat (fullname, pos);


      printf ("'%s'\n", fullname);

      free (uri);
    }
    else {
      sprintf (fullname, "%s \"%s/%s\"", app->executable,
	       ((entropy_generic_file *) obj)->path,
	       ((entropy_generic_file *) obj)->filename);
    }




    //printf ("Hit action callback\n");
    //printf("Exe: %s\n", fullname);

    ecore_exe_run (fullname, NULL);
  }
  else {
    printf ("action_simple: No action associated with %s\n",
	    ((entropy_generic_file *) obj)->mime_type);
  }

}


entropy_gui_component_instance *
entropy_plugin_init (entropy_core * core)
{
  int config;

  //printf ("initialising action provider..\n");

  entropy_gui_component_instance *instance =
    entropy_gui_component_instance_new ();
  entropy_gui_component_instance *layout =
    entropy_core_global_layout_get (core);
  instance->layout_parent = layout;
  instance->core = core;

  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_ACTION_FILE));


  return instance;
}
