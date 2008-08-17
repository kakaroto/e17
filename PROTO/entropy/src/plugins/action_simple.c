#include "entropy.h"
#include "entropy_gui.h"
#include "entropy_config.h"
#include <unistd.h>
#include <limits.h>




int
entropy_plugin_type_get ()
{
  return ENTROPY_PLUGIN_ACTION_PROVIDER;
}

char *
entropy_plugin_identify ()
{
  return (char *) "Simple action execution plugin";
}


/*FIXME - this is vomit.  In serious need of optimization*/
char* entropy_action_simple_str_replace(entropy_generic_file* file, char* exe, char* args)
{
	/*A function to replace %p (path), %pf (path+filename), and %u (uri), with their
	 * real equivalents*/
	int i=0;
	char* str = entropy_malloc(sizeof(char)*PATH_MAX);
	char* currStr = exe;

	while (currStr) {
		i=0;
		while (i < strlen(currStr)) {
			int ud = 0;

			if ( i +3 <= strlen(currStr) && !strncmp(currStr+i, "\%pf", 3)) {
				printf("Subbing path and filename\n");
			
				strcat(str, "\"");
				strcat(str, file->path);					
				strcat(str, "/");
				strcat(str, file->filename);
				strcat(str, "\"");
				ud = 1;
				i+=3;
			}

			if ( i +2 <= strlen(currStr) && !strncmp(currStr+i, "\%p", 2)) {
				printf("Subbing path only\n");
			
				strcat(str, "\"");
				strcat(str, file->path);					
				strcat(str, "\"");
				ud = 1;
				i+=2;
			}

			if ( i +2 <= strlen(currStr) && !strncmp(currStr+i, "\%u", 2)) {
				printf("Subbing uri\n");
				
				char* uri = entropy_core_generic_file_uri_create(file, 0);
				strcat(str,"\"");
				strcat(str, uri);
				strcat(str, "\"");
				ud = 1;
				i+=2;
				free(uri);
			}

			if (!ud) {
				strncat(str, currStr+i, 1);
				i += 1;
			}

			ud  = 0;
			
		}


		if (currStr == exe) {
			currStr = args;
			strcat(str, " ");
		} else
			currStr = NULL;
	}

	return str;
}


void
gui_event_callback (entropy_notify_event * eevent, void *requestor, void *obj,
		    entropy_gui_component_instance * comp)
{
  char *fullname;
  entropy_gui_event *gui_event;
  entropy_core *core = ((entropy_gui_component_instance *) requestor)->core;
  Entropy_Config_Mime_Binding_Action *app;
  char *uri;

  entropy_generic_file *file = (entropy_generic_file *) obj;

  if (!strcmp (file->mime_type, "file/folder") && !file->parent) {
	  
     if (!(eevent->hints & ENTROPY_GUI_EVENT_HINT_WINDOW_NEW)) {
	     entropy_file_request *request =
	      entropy_malloc (sizeof (entropy_file_request));
	    request->file = file;

	    /*Send an event to the core */
	    gui_event = entropy_malloc (sizeof (entropy_gui_event));
	    gui_event->event_type =
	      entropy_core_gui_event_get (ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS);
	    gui_event->data = request;
	    entropy_core_layout_notify_event ((entropy_gui_component_instance *)
				      requestor, gui_event,
				      ENTROPY_EVENT_LOCAL);
	    return;
     } else {
	     Ecore_Ipc_Server* server = ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER, IPC_TITLE,0 ,NULL);
	     if (server) {
			printf("Sending message to server!\n");
			ecore_ipc_server_send(server, ENTROPY_IPC_EVENT_LAYOUT_NEW, 0, 0, 0, 0, 
				file->uri, strlen(file->uri)+1); 
	     }
     }
  } else if ((uri = entropy_core_descent_for_mime_get (core, file->mime_type))
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
    entropy_core_mime_hint_get (((entropy_generic_file *) obj)->mime_type, eevent->key);
  if (app) {
	  fullname = entropy_action_simple_str_replace((entropy_generic_file*)obj, app->executable, app->args);
	  printf("'%s'\n", fullname);


    //printf ("Hit action callback\n");
    //printf("Exe: %s\n", fullname);

    ecore_exe_pipe_run (fullname, ECORE_EXE_PIPE_AUTO, NULL);
  }
  else {
    printf ("action_simple: No action associated with %s\n",
	    ((entropy_generic_file *) obj)->mime_type);
  }

}

entropy_plugin*
entropy_plugin_init(entropy_core* core)
{
	Entropy_Plugin_Gui* plugin;

	plugin = entropy_malloc(sizeof(Entropy_Plugin_Gui));

	return ENTROPY_PLUGIN(plugin);
}


entropy_gui_component_instance *
entropy_plugin_gui_instance_new (entropy_core * core)
{
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
