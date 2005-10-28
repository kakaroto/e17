#include "entropy.h"
#include "entropy_gui.h"
#include "entropy_config.h"
#include <unistd.h>





int entropy_plugin_type_get() {
        return ENTROPY_PLUGIN_ACTION_PROVIDER;
}

char* entropy_plugin_identify() {
	        return (char*)"Simple MIME identifier";
}


void gui_event_callback(entropy_notify_event* eevent, void* requestor, void* obj, entropy_gui_component_instance* comp) {
	char fullname[1024];
	entropy_gui_event* gui_event;
	entropy_core* core = ((entropy_gui_component_instance*)requestor)->core;
	char* app;
	char* uri;

	entropy_generic_file* file = (entropy_generic_file*)obj;

	if (!strcmp(file->mime_type, "file/folder") && !file->parent) {
		entropy_file_request* request = entropy_malloc(sizeof(entropy_file_request));
		request->file = file;
		
		//printf("Action on a folder - change dirs!\n\n");

		
		/*Send an event to the core*/
		gui_event = entropy_malloc(sizeof(entropy_gui_event));
		gui_event->event_type = entropy_core_gui_event_get(ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS);
		gui_event->data = request;
		entropy_core_layout_notify_event((entropy_gui_component_instance*)requestor, gui_event, ENTROPY_EVENT_LOCAL); 

		return;
	} else if ( (uri = entropy_core_descent_for_mime_get(core,file->mime_type)) || file->parent) {
		entropy_file_request* request = entropy_malloc(sizeof(entropy_file_request));
		
		printf("Requested a list of a descendable object\n");
		/*printf("URI would be: '%s://%s/%s#tar:///'\n", file->uri_base, file->path, file->filename);*/
		
		
		request->file = file;
		if (uri) {
			request->drill_down = 1;
		} else {
			request->drill_down = 0;
		}

		//printf("Action on a folder - change dirs!\n\n");

		
		/*Send an event to the core*/
		gui_event = entropy_malloc(sizeof(entropy_gui_event));
		gui_event->event_type = entropy_core_gui_event_get(ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS);
		gui_event->data = request;
		entropy_core_layout_notify_event((entropy_gui_component_instance*)requestor, gui_event, ENTROPY_EVENT_LOCAL); 

		return;
	}


	/*First get the app associated with this mime type*/
	app = entropy_config_str_get("action_simple", ((entropy_generic_file*)obj)->mime_type);
	if (app) {
		sprintf(fullname, "%s \"%s/%s\"", app, ((entropy_generic_file*)obj)->path, ((entropy_generic_file*)obj)->filename);
	
		//printf ("Hit action callback\n");
		//printf("Exe: %s\n", fullname);

		ecore_exe_run(fullname, NULL);
		entropy_free(app);
	} else {
		printf("action_simple: No action associated with %s\n", ((entropy_generic_file*)obj)->mime_type);
	}
	
}


entropy_gui_component_instance* entropy_plugin_init(entropy_core* core) {
	int config;
	
	//printf ("initialising action provider..\n");
	
	entropy_gui_component_instance* instance = entropy_malloc(sizeof(entropy_gui_component_instance));
	entropy_gui_component_instance* layout = entropy_core_global_layout_get(core);
	instance->layout_parent = layout;
	instance->core = core;
	
	entropy_core_component_event_register(core, instance, entropy_core_gui_event_get(ENTROPY_GUI_EVENT_ACTION_FILE));


	/*Load config*/
	config = entropy_config_int_get("action_simple", "init");
	if (!config) {
		printf (" *** Initializing action provider config\n");
		entropy_config_int_set("action_simple", "init", 1);

		/*Set default apps to run*/
		entropy_config_str_set("action_simple", "image/jpeg", "entice");
		entropy_config_str_set("action_simple", "image/png", "entice");
		entropy_config_str_set("action_simple", "image/gif", "entice");
		entropy_config_str_set("action_simple", "text/html", "/usr/bin/firefox");
		entropy_config_str_set("action_simple", "text/csrc", "gvim");
		entropy_config_str_set("action_simple", "audio/x-mp3", "xmms");
		entropy_config_str_set("action_simple", "video/x-ms-wmv", "mplayer");
		entropy_config_str_set("action_simple", "video/mpeg", "mplayer");
		entropy_config_str_set("action_simple", "application/msword", "abiword");
		entropy_config_str_set("action_simple", "application/vnd.ms-excel", "gnumeric");

		/*Archives*/
		entropy_config_str_set("action_simple", "application/x-gtar", "file-roller");
	}

	return instance;
}
