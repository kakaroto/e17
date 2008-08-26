#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <dirent.h>
#include "plugin_base.h"
#include <Ecore.h>
#include <evfs.h>
#include <limits.h>

#include "entropy.h"
#include <ctype.h>
#include "md5.h"
#include "entropy_gui.h"
#include <Epsilon.h>
#include <Evas.h>

#include <X11/Xlib.h>

#define ENTROPY_CORE 1

static long file_cache_size = 0;
static entropy_plugin* distrib_plugin = NULL; /* FIXME - handle distrib plugins better*/
entropy_core* core_core;

#define FILE_FREE_QUEUE_SIZE 500;


entropy_core* entropy_core_get_core() {
	return core_core;
}


int ecore_timer_enterer(void* data) {
	return 1;
}


int
ipc_client_data(void *data, int type, void *event)
{
	Ecore_Ipc_Event_Client_Data *e = (Ecore_Ipc_Event_Client_Data*) event;
	entropy_core* core = data;
	entropy_notify_event_cb_data* cb_data;
	int destroy_struct =0;

	/*If it's not from us, ignore*/
	if (ecore_ipc_client_server_get(e->client) != core->server) return 1;

	
	/*printf ("Received message\n");*/

	if (e->major == ENTROPY_IPC_EVENT_CORE) {
		entropy_notify_event* eevent;

		ecore_list_first_goto(core->notify->exe_queue);
		if ( (eevent = ecore_list_next(core->notify->exe_queue)) ) {
			if (!eevent->processed) {
				printf("Pulled an unprocessed event off the queue!\n");
			}
			
			ecore_list_first_remove(core->notify->exe_queue);
			
				
			/*If the return struct is null, don't call the callbacks.  The requester has taken responsibility
			 * for calling the requesters of this type when the data is available, which is obviously not now*/
			
			if (eevent->return_struct) {

				/*printf("******* PROCESSING %p\n", eevent);*/
				ecore_list_first_goto(eevent->cb_list);
				while ( (cb_data = ecore_list_next(eevent->cb_list)) ) {
					(*cb_data->cb)(eevent, eevent->requestor_data, eevent->return_struct, cb_data->data);
				}
				destroy_struct = 1;
				/*printf("******** DONE\n");*/
			} else {
				entropy_log ("ipc_client_data: RETURN was NULL.  Caller will notify when data ready\n", ENTROPY_LOG_WARN);
				destroy_struct = 0;
			}

			entropy_notify_event_destroy(eevent);		
		}
	} else if (e->major == ENTROPY_IPC_EVENT_LAYOUT_NEW) {
		entropy_gui_component_instance* (*entropy_plugin_layout_create)(entropy_core*);
		entropy_gui_component_instance* instance;
		entropy_generic_file* file = NULL;

		printf("New layout requested! - '%s'\n", (char*)e->data);

		entropy_plugin_layout_create = dlsym(core->layout_plugin->dl_ref, "entropy_plugin_layout_create");
		instance = (*entropy_plugin_layout_create)(core);
		instance->plugin = core->layout_plugin;

		if (e->data) {
			file = entropy_core_uri_generic_file_retrieve((char*)e->data);
			if (!file) {
				file = entropy_core_parse_uri((char*)e->data);
			}

			if (file) {
				entropy_mime_file_identify(file);
				printf("'%s/%s'...\n", file->path, file->filename);
				entropy_event_action_file(file, instance);
			}
		}
	}

	return 1;

}

entropy_core* entropy_core_new() {
	entropy_core* core = entropy_malloc(sizeof(entropy_core));

	return core;
}

void entropy_core_args_parse(entropy_core* core, int argc, char** argv) 
{
	int ac = 1;

	/*Defaults*/
	core->settings.layout_engine = "etk";
		
	while (ac < argc) {
		printf("Parsing '%s'...\n", argv[ac]);

		if (!strncmp(argv[ac], "--layout=", 9)) {
				core->settings.layout_engine = argv[ac]+9;
				printf("Layout engine is '%s'\n", core->settings.layout_engine);
		}
		
		ac++;
	}
}

entropy_core* entropy_core_init(int argc, char** argv) {
	entropy_plugin* plugin;
	void (*entropy_plugin_layout_main)();
	entropy_gui_component_instance* (*entropy_plugin_layout_create)(entropy_core*);
	entropy_gui_component_instance* layout;

        struct dirent* de;
        DIR* dir;
	char plugin_path[1024];

	
	entropy_core* core = entropy_core_new();
	core_core=core;

	/*Read inbound arguments*/
	entropy_core_args_parse(core, argc,argv);

	/*Initialise the ecore ipc system*/
	if (ecore_ipc_init() < 1) {
		printf("Couldn't connect to the ecore_ipc subsystem\n");
		exit(1);
	}

	//printf("Creating the ipc server...\n");
	core->server = ecore_ipc_server_add(ECORE_IPC_LOCAL_USER, IPC_TITLE, 0, NULL);

	if (core->server == NULL) {
		Ecore_Ipc_Server* server = ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER, IPC_TITLE,0 ,NULL);
		if (server) {
			printf("Sending message to server!\n");
			ecore_ipc_server_send(server, ENTROPY_IPC_EVENT_LAYOUT_NEW, 0, 0, 0, 0, 
				NULL, 0); 
		}
		ecore_main_loop_iterate();
		exit(0);
	}

	//printf ("Adding client data hander...\n");
        ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DATA, ipc_client_data,core);


	/*Initialize the mime hint hash*/
	core->mime_action_hint = ecore_hash_new(ecore_str_hash, ecore_str_compare);

	/*Initialize the object assoc. hash*/
	core->object_associate_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);

	/*Initialize the gui event handler hash*/
	core->gui_event_handlers = ecore_hash_new(ecore_str_hash, ecore_str_compare);

	//printf ("Initialising the file cache..\n");
	core->file_interest_list = ecore_hash_new(ecore_str_hash, ecore_str_compare);
	core->uri_reference_list = ecore_hash_new(ecore_str_hash, ecore_str_compare);

	//Misc setup (thumbnail path)
	core->user_home_dir = strdup(getenv("HOME"));
	core->thumbnail_path = entropy_malloc((strlen(core->user_home_dir) + 21)*sizeof(char));
	snprintf(core->thumbnail_path, strlen(core->user_home_dir) + 21, "%s/%s", core->user_home_dir, ".thumbnails/normal/");


	//Initialise the config system
	core->config = entropy_config_init(core);


	/*Load any config related core stuff*/
	entropy_core_config_load();

	/*Initialise epsilon thumbnailer*/
	epsilon_init();

	//Load the plugins
	core->plugin_list = ecore_list_new();
        //printf("Reading plugins from: %s\n", PACKAGE_DATA_DIR "/plugins/");
        dir = opendir(PACKAGE_DATA_DIR "/plugins/");
        if (dir) {
                while ( (de = readdir(dir)) ) {

                   if (!strncmp(de->d_name + strlen(de->d_name) -3, ".so", 3)) {
                        snprintf(plugin_path, 1024,"%s/%s", PACKAGE_DATA_DIR "/plugins/", de->d_name);
		
			/*printf("Loading '%s'...\n", plugin_path);*/
			ecore_list_append(core->plugin_list, create_plugin_object(plugin_path));
                   }
                }
		closedir(dir);
        } else {
                fprintf(stderr, "Entropy: Could not location plugin directory '%s'\n", PACKAGE_DATA_DIR "/plugins/");
                exit(1);
        }

	/*Initialise the layout event hash*/
	core->layout_gui_events = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);

	//printf("Creating the global layout object..\n");
	/*Create the global layout object*/
	layout = entropy_malloc(sizeof(entropy_gui_component_instance));
	layout->core = core;
	core->layout_global = layout;
	entropy_core_layout_register(core, layout);
	printf("Registered global layout %p...\n", core->layout_global);

        /*Init the mime register */
        core->entropy_thumbnailers = entropy_thumbnailers_register_init();
	core->entropy_thumbnailers_child = entropy_thumbnailers_register_init();

	
        core->mime_plugins = entropy_mime_register_init();

        /*Load plugins*/
        ecore_list_first_goto(core->plugin_list);
        while ( (plugin = ecore_list_current(core->plugin_list)) ) {
                int res = entropy_plugin_load(core, plugin);
		if (res) {
			/*printf("Removing plugin from list..%s\n", plugin->filename);*/
			ecore_list_remove(core->plugin_list);
		} else {
			/*printf("Going to next plugin...%s\n", plugin->filename);*/
			ecore_list_next(core->plugin_list);
		}
		
        }


	/*Now register all thumbnailers with the distribution plugin*/
	{
		char* mime= NULL;
		Ecore_List* keys = ecore_hash_keys(core->entropy_thumbnailers_child);

		ecore_list_first_goto(keys);
		while ( (mime = ecore_list_first_remove(keys))) {
			ecore_hash_set(core->entropy_thumbnailers, mime, distrib_plugin);
		}
	}

	/*Register GUI event handlers*/
	entropy_core_gui_event_handler_add(ENTROPY_GUI_EVENT_FILE_CREATE, entropy_event_handler_file_create_handler);
	entropy_core_gui_event_handler_add(ENTROPY_GUI_EVENT_FILE_REMOVE, entropy_event_handler_file_remove_handler);
	entropy_core_gui_event_handler_add(ENTROPY_GUI_EVENT_FILE_REMOVE_DIRECTORY, entropy_event_handler_file_remove_directory_handler);
	entropy_core_gui_event_handler_add(ENTROPY_GUI_EVENT_FILE_STAT, entropy_event_handler_file_stat_handler);
	entropy_core_gui_event_handler_add(ENTROPY_GUI_EVENT_ACTION_FILE, entropy_event_handler_file_action_handler);
	entropy_core_gui_event_handler_add(ENTROPY_GUI_EVENT_FILE_STAT_AVAILABLE, entropy_event_handler_file_stat_available_handler);
	entropy_core_gui_event_handler_add(ENTROPY_GUI_EVENT_THUMBNAIL_AVAILABLE, entropy_event_handler_thumbnail_available_handler);
	entropy_core_gui_event_handler_add(ENTROPY_GUI_EVENT_FILE_PROGRESS, entropy_event_handler_progress_handler);
	entropy_core_gui_event_handler_add(ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS, entropy_event_handler_folder_change_handler);
	entropy_core_gui_event_handler_add(ENTROPY_GUI_EVENT_FILE_CHANGE, entropy_event_handler_file_change_handler);
	entropy_core_gui_event_handler_add(ENTROPY_GUI_EVENT_FILE_METADATA, entropy_event_handler_metadata_request_handler);
	entropy_core_gui_event_handler_add(ENTROPY_GUI_EVENT_FILE_METADATA_AVAILABLE, entropy_event_handler_metadata_available_handler);
	entropy_core_gui_event_handler_add(ENTROPY_GUI_EVENT_USER_INTERACTION_YES_NO_ABORT, entropy_event_handler_user_interaction_handler);
	entropy_core_gui_event_handler_add(ENTROPY_GUI_EVENT_EXTENDED_STAT, entropy_event_handler_extended_stat_handler);
	entropy_core_gui_event_handler_add(ENTROPY_GUI_EVENT_METADATA_GROUPS, entropy_event_handler_metadata_groups_handler);
	entropy_core_gui_event_handler_add(ENTROPY_GUI_EVENT_COPY_REQUEST, entropy_event_handler_copy_request_handler);
	entropy_core_gui_event_handler_add(ENTROPY_GUI_EVENT_CUT_REQUEST, entropy_event_handler_cut_request_handler);
	entropy_core_gui_event_handler_add(ENTROPY_GUI_EVENT_PASTE_REQUEST, entropy_event_handler_paste_request_handler);
	entropy_core_gui_event_handler_add(ENTROPY_GUI_EVENT_HOVER, entropy_event_handler_hover_request_handler);	
	entropy_core_gui_event_handler_add(ENTROPY_GUI_EVENT_DEHOVER, entropy_event_handler_dehover_request_handler);
	entropy_core_gui_event_handler_add(ENTROPY_GUI_EVENT_AUTH_REQUEST, entropy_event_handler_auth_request_handler);
	entropy_core_gui_event_handler_add(ENTROPY_GUI_EVENT_META_ALL_REQUEST, entropy_event_handler_meta_all_request_handler);

	//printf("\n\nDetails of thumbnailers:\n");

        /*Show some details*/
        //entropy_thumbnailer_plugin_print(core->entropy_thumbnailers);

	/*Initialize the selection engine*/
	entropy_core_selection_engine_init(core);


	/*Initialize the descent hash - this should be loaded from EVFS - FIXME*/
	/*Descent hash init - move this to a call to EVFS*/
	core->descent_hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);
	ecore_hash_set(core->descent_hash, "application/x-bzip2", "bzip2:///#tar");
	ecore_hash_set(core->descent_hash, "application/x-gtar", "gzip:///#tar");
	ecore_hash_set(core->descent_hash, "application/x-tar", "tar");



	/*Initial layout initialize...*/

        /*If layout plugin is NULL, we have no way to display anything.  For now, bomb out */
        if (core->layout_plugin == NULL) {
                printf("Sorry, but you haven't loaded a layout plugin.  Please configure one.\n");
                exit(0);
        }

        /*-----------------------------------------------------------------*/
        /*Initialize notification_engine*/
        core->notify = entropy_notification_engine_init();

	/*Initialize the layout manager plugin*/
	/*Initializing..*/ /*TODO RENAME THIS API*/
	entropy_plugin_layout_create = dlsym(core->layout_plugin->dl_ref, "entropy_plugin_layout_create");
	layout = (*entropy_plugin_layout_create)(core);
	layout->plugin = core->layout_plugin;
	core->layout_plugin->data = layout;
	
	
	entropy_plugin_layout_main = dlsym(core->layout_plugin->dl_ref, "entropy_plugin_layout_main");

	ecore_idle_enterer_add(entropy_notify_loop, core->notify); 

	//printf("--------------- Running main\n");

	printf("Going to main..\n");
	(*entropy_plugin_layout_main)();

	


        /*Right lets go*/
        //printf("\n\n");


	return core;
}

Ecore_List* entropy_core_gui_event_handlers_get(char* event)
{
	return ecore_hash_get(entropy_core_get_core()->gui_event_handlers, event);
}

void entropy_core_gui_event_handler_add(char* gui_event, 
		Entropy_Gui_Event_Handler* (*handler_func)())
{
	Entropy_Gui_Event_Handler* handler;
	Ecore_List* list;

	if ( !(list = entropy_core_gui_event_handlers_get(gui_event))) {
		list = ecore_list_new();
		ecore_hash_set(entropy_core_get_core()->gui_event_handlers, gui_event, list);
	}

	handler = (*handler_func)();
	ecore_list_append(list, handler);
}

void entropy_core_mime_action_remove(char* mime_type)
{
	Entropy_Config_Mime_Binding* binding;
	Evas_List* l;

	for (l = core_core->config->Loaded_Config->mime_bindings; l; ) {
		binding = l->data;

		/*FIXME: This leaks a binding*/
		if (!strcmp(binding->mime_type, mime_type)) {
			printf("Removing binding '%s'...\n", mime_type);
			core_core->config->Loaded_Config->mime_bindings = 
				evas_list_remove(core_core->config->Loaded_Config->mime_bindings, binding);
		}
		l = l->next;
	}

}

void entropy_core_mime_action_remove_app(char* mime_type, Entropy_Config_Mime_Binding_Action* action)
{
	Entropy_Config_Mime_Binding* binding;
	Evas_List* l;

	for (l = core_core->config->Loaded_Config->mime_bindings; l; ) {
		binding = l->data;

		/*FIXME: This leaks a binding*/
		if (!strcmp(binding->mime_type, mime_type)) {
			binding->actions = evas_list_remove(binding->actions, action);
		}
		l = l->next;
	}
}

void entropy_core_mime_action_add(char* mime_type, char* desc) 
{
	Entropy_Config_Mime_Binding* binding;
	Evas_List* l;
	int found = 0;
	
	/*entropy_mime_action* action_o;
	
	if ( ! (action_o = ecore_hash_get  (core_core->mime_action_hint, mime_type))) {
			action_o = entropy_malloc(sizeof(entropy_mime_action));
			action_o->executable = strdup(action);
			ecore_hash_set(core_core->mime_action_hint, strdup(mime_type), action_o);
	} else {
			free(action_o->executable);
			action_o->executable = strdup(action);
	}*/

	for (l = core_core->config->Loaded_Config->mime_bindings; l; ) {
		binding = l->data;

		/*If this exists, change the desc to whatever they say*/
		if (!strcmp(binding->mime_type, mime_type)) {
			found = 1;
			binding->desc = strdup(desc);
		}
		
		l = l->next;
	}

	/*IF not found, make a new one*/
	if (!found) {
		binding = entropy_malloc(sizeof(Entropy_Config_Mime_Binding));
		binding->mime_type = strdup(mime_type);
		binding->desc = strdup(desc);
		core_core->config->Loaded_Config->mime_bindings = 
			evas_list_append(core_core->config->Loaded_Config->mime_bindings, binding);
	}

	
}

void entropy_core_mime_application_add(char* mime_type, char* name, char* executable, char* args)
{
	Entropy_Config_Mime_Binding* binding;
	Entropy_Config_Mime_Binding_Action* action;
	
	Evas_List* l;


	for (l = core_core->config->Loaded_Config->mime_bindings; l; ) {
		binding = l->data;

		if (!strcmp(binding->mime_type, mime_type)) {
			/*Create a new action, based on this mime_type*/
			action = entropy_malloc(sizeof(Entropy_Config_Mime_Binding_Action));

			action->app_description = strdup(name);
			action->executable = strdup(executable);
			action->args = strdup(args);

			binding->actions = evas_list_append(binding->actions, action);
		}
		
		l = l->next;
	}


}

void entropy_core_config_load() 
{
	/*DEPRECATED*/
}

void entropy_core_config_save() {
	/*DEPRECATED*/
}

Entropy_Config_Mime_Binding_Action* entropy_core_mime_hint_get(char* mime_type, int key) {

	Entropy_Config_Mime_Binding* binding;
	Entropy_Config_Mime_Binding_Action* action;
	Evas_List* l;

	for (l = core_core->config->Loaded_Config->mime_bindings; l; ) {
		binding = l->data;
		
		if (!strcmp(mime_type , binding->mime_type)) {
			/*Return nth element*/
			action = evas_list_nth(binding->actions, key);

			printf("Action exe: %s, Action args: %s\n", action->executable, action->args);
			return action;
		}

		l = l->next;
	}
	
	return NULL;
}


/*
 * Get the status of tooltip displays 
 */
int entropy_core_tooltip_status_get() {
	int status = entropy_config_int_get("core","tooltip_show");
	/*printf("Status of tooltip: %d\n", status);*/

	if (!status) {
		/*Key doesn't exist, set it to the 'display' value by default*/
		entropy_config_int_set("core","tooltip_show", 2);
		
	}
	
	return status;
}


void entropy_core_destroy(entropy_core* core) {
	entropy_plugin* plugin;
	entropy_file_listener* listener;
	Ecore_List* hash_keys;
	char* key;

	//printf("Destroying config subsystem...\n");
	/*Save the config*/
	entropy_core_config_save();

	entropy_config_destroy(core->config);


	/*Destroy the notification engines*/
	entropy_notification_engine_destroy(core->notify);


	ecore_list_first_goto(core->plugin_list);
	while ( (plugin = ecore_list_next(core->plugin_list) )) {
		void (*entropy_plugin_destroy)(void* data);

		if (plugin->type == ENTROPY_PLUGIN_GUI_LAYOUT) {
			entropy_plugin_destroy = dlsym(plugin->dl_ref, "entropy_plugin_destroy");
			(*entropy_plugin_destroy)(plugin->data);
		}
		
		
		entropy_free(plugin);
	}

	//printf("Destroying plugin list..\n");
	ecore_list_destroy(core->plugin_list);

	//printf("Destroying thumbnailers..\n");
	ecore_hash_destroy(core->entropy_thumbnailers);



	//Now destroy chars
	entropy_free(core->user_home_dir);
	entropy_free(core->thumbnail_path);


	

	//printf("Destroying file objects...\n");
	/*Kill the file list*/
	hash_keys = ecore_hash_keys(core->file_interest_list);
	ecore_list_first_goto(hash_keys);
	while ( (key = ecore_list_next(hash_keys)) ) {
		listener = ecore_hash_get(core->file_interest_list, key);

		entropy_generic_file_destroy(listener->file);
		entropy_free(listener);
		ecore_hash_remove(core->file_interest_list, key);


	}

	ecore_list_destroy(hash_keys);
	ecore_hash_destroy(core->file_interest_list);
	

	/*Destroy the plugins*/
	/*Destroy the eLists/eHashes*/

}

void entropy_layout_run(entropy_core* core) {

}


/*A function we need, unfortunately, because of the way dlopen works with string refs, and the need of ecore_hash for 
 * a pointer. This isn't a big inefficiency - this is only called on plugin create */
char* entropy_core_gui_event_get(char* event) {
	if (!strcmp(event, ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS)) {
		return "entropy_gui_event_folder_change_contents";
	} else if (!strcmp(event, ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS_EXTERNAL)) {
		return "entropy_gui_event_folder_change_contents_external";
	} else if (!strcmp(event, ENTROPY_GUI_EVENT_FILE_CHANGE)) {
		return "entropy_gui_event_file_change";
	} else if (!strcmp(event, ENTROPY_GUI_EVENT_FILE_CREATE)) {
		return "entropy_gui_event_file_create";
	} else if (!strcmp(event, ENTROPY_GUI_EVENT_FILE_REMOVE)) {
		return "entropy_gui_event_file_remove";
	} else if (!strcmp(event, ENTROPY_GUI_EVENT_FILE_REMOVE_DIRECTORY)) {
		return "entropy_gui_event_file_remove_directory";
	} else if (!strcmp(event, ENTROPY_GUI_EVENT_ACTION_FILE)) {
		return "entropy_gui_event_action_file";
	} else if (!strcmp(event, ENTROPY_GUI_EVENT_FILE_STAT)) {
		return "entropy_gui_event_file_stat";
	} else if (!strcmp(event, ENTROPY_GUI_EVENT_FILE_STAT_AVAILABLE)) {
		return "entropy_gui_event_file_stat_available";
	} else if (!strcmp(event, ENTROPY_GUI_EVENT_FILE_PROGRESS)) {
		return "entropy_gui_event_file_progress";
	} else if (!strcmp(event, ENTROPY_GUI_EVENT_THUMBNAIL_AVAILABLE)) {
		return "entropy_gui_event_thumbnail_available";
	} else if (!strcmp(event, ENTROPY_GUI_EVENT_USER_INTERACTION_YES_NO_ABORT)) {
		return "entropy_gui_event_user_interaction_yes_no_abort";
	} else if (!strcmp(event,  ENTROPY_GUI_EVENT_FILE_METADATA)) {
		return "entropy_gui_event_file_metadata";
	} else if (!strcmp(event,  ENTROPY_GUI_EVENT_FILE_METADATA_AVAILABLE)) {
		return "entropy_gui_event_file_metadata_available";
	} else if (!strcmp(event,  ENTROPY_GUI_EVENT_EXTENDED_STAT)) {
		return "entropy_gui_event_extended_stat";
	} else if (!strcmp(event,  ENTROPY_GUI_EVENT_METADATA_GROUPS)) {
		return "entropy_gui_event_metadata_groups";
	} else if (!strcmp(event, ENTROPY_GUI_EVENT_COPY_REQUEST)) {
		return "entropy_gui_event_copy_request";
	} else if (!strcmp(event, ENTROPY_GUI_EVENT_CUT_REQUEST)) {
		return "entropy_gui_event_cut_request";
	} else if (!strcmp(event, ENTROPY_GUI_EVENT_PASTE_REQUEST)) {
		return "entropy_gui_event_paste_request";
	} else if (!strcmp(event, ENTROPY_GUI_EVENT_HOVER)) {
		return "entropy_gui_event_hover";		
	} else if (!strcmp(event, ENTROPY_GUI_EVENT_DEHOVER)) {
		return "entropy_gui_event_dehover";	
	} else if (!strcmp(event, ENTROPY_GUI_EVENT_AUTH_REQUEST)) {
		return "entropy_gui_event_auth_request";	
	} else if (!strcmp(event, ENTROPY_GUI_EVENT_META_ALL_REQUEST)) {
		return "entropy_gui_event_meta_all_request";	
	} else 
		return "";
}

void generic_file_print(entropy_generic_file* file) {
	//printf("File is: %s/%s\n", file->path,file->filename);
}




int entropy_plugin_load(entropy_core* core, entropy_plugin* plugin) {
        char* (*entropy_plugin_identify)();
        int (*entropy_plugin_type_get)();
	int (*entropy_plugin_sub_type_get)();
	void* (*entropy_plugin_init)(entropy_core* core);
	

	void (*gui_event_callback)(void*,void*);

        int type;


        //printf("Loading plugin %s\n", plugin->filename);
        plugin->dl_ref = dlopen(plugin->filename, RTLD_LAZY);

        /*Get the plugin to identify itself..*/
        entropy_plugin_identify =dlsym(plugin->dl_ref, "entropy_plugin_identify");
        entropy_plugin_type_get = dlsym(plugin->dl_ref, "entropy_plugin_type_get");
	entropy_plugin_sub_type_get = dlsym(plugin->dl_ref, "entropy_plugin_sub_type_get");
        /*printf("Plugin IDs as: '%s'\n", (*entropy_plugin_identify)());*/

        type = (*entropy_plugin_type_get)();
	plugin->type = type;

	/*Load the subtype, if there is one*/
	if (entropy_plugin_sub_type_get) {
		plugin->subtype = (*entropy_plugin_sub_type_get)();
	}


        if (type == ENTROPY_PLUGIN_THUMBNAILER_DISTRIBUTION) {
		entropy_gui_component_instance* instance;
                //printf(" ----------------------- Thumbnailer Plugin, registering with engine..\n");
		//
		entropy_plugin_init = dlsym(plugin->dl_ref, "entropy_plugin_gui_instance_new");
		instance = (*entropy_plugin_init)(core);
		instance->plugin = plugin;

		gui_event_callback = dlsym(plugin->dl_ref, "gui_event_callback");
		plugin->gui_event_callback_p = gui_event_callback;
	
                entropy_plugin_thumbnailer_register(core, plugin, THUMBNAILER_DISTRIBUTION);
	} else if (type == ENTROPY_PLUGIN_THUMBNAILER) {
		entropy_gui_component_instance* instance;
		
		entropy_plugin_init = dlsym(plugin->dl_ref, "entropy_plugin_gui_instance_new");
		instance = (*entropy_plugin_init)(core);
		instance->plugin = plugin;

		gui_event_callback = dlsym(plugin->dl_ref, "gui_event_callback");
		plugin->gui_event_callback_p = gui_event_callback;
	
                entropy_plugin_thumbnailer_register(core, plugin, THUMBNAILER_CHILD);
        } else if (type == ENTROPY_PLUGIN_MIME) {
                //printf("MIME Identifier Plugin, registering with engine..\n");
                entropy_plugin_mime_register(core->mime_plugins, plugin);

        } else if (type == ENTROPY_PLUGIN_GUI_LAYOUT) {
                char* id = entropy_plugin_plugin_identify(plugin);
		
		if (!strncmp(core->settings.layout_engine, id, strlen(core->settings.layout_engine))) {
			core->layout_plugin = entropy_plugin_layout_register(plugin);

			/*ID the global system toolkit*/
			plugin->toolkit = entropy_plugin_helper_toolkit_get(plugin);

			gui_event_callback = dlsym(plugin->dl_ref, "gui_event_callback");
			plugin->gui_event_callback_p = gui_event_callback;
		} else {
			return 1;
		}
        } else if (type == ENTROPY_PLUGIN_GUI_COMPONENT) {
		/* TODO Get the subtype */

		gui_event_callback = dlsym(plugin->dl_ref, "gui_event_callback");
		plugin->gui_event_callback_p = gui_event_callback;
		plugin->toolkit = entropy_plugin_helper_toolkit_get(plugin);

		printf("GUI event callback (%s) registered as: %p\n", plugin->name, plugin->gui_event_callback_p);

	} else if (type == ENTROPY_PLUGIN_BACKEND_FILE) {
	} else if (type == ENTROPY_PLUGIN_ACTION_PROVIDER) {
		entropy_gui_component_instance* instance;

		//printf ("Processing an action provider...\n");

		/*Initializing..*/
		entropy_plugin_init = dlsym(plugin->dl_ref, "entropy_plugin_gui_instance_new");
		instance = (*entropy_plugin_init)(core);
		instance->plugin = plugin;

		gui_event_callback = dlsym(plugin->dl_ref, "gui_event_callback");
		plugin->gui_event_callback_p = gui_event_callback;

		//printf ("SETTING ACTION callback at : %p\n", gui_event_callback);
	} else if (type == ENTROPY_PLUGIN_METADATA_READ) {
		entropy_gui_component_instance* instance;

		/*Initializing..*/
		entropy_plugin_init = dlsym(plugin->dl_ref, "entropy_plugin_gui_instance_new");
		instance = (*entropy_plugin_init)(core);
		instance->plugin = plugin;

		gui_event_callback = dlsym(plugin->dl_ref, "gui_event_callback");
		plugin->gui_event_callback_p = gui_event_callback;
	}





	return 0;
}


char* entropy_layout_global_toolkit_get() {
	return core_core->layout_plugin->toolkit;
}


entropy_plugin* create_plugin_object(char* filename) {
        entropy_plugin* plugin;
	void* dl_ref;
	entropy_plugin* (*entropy_plugin_init)(entropy_core* core);

	dl_ref = dlopen(filename, RTLD_LAZY);
	if (!dl_ref) {
		printf("Could not open plugin object %s - Corrupt file? abort\n", filename);
		return NULL; /*Could not open plugin*/
	}

	entropy_plugin_init = dlsym(dl_ref, ENTROPY_PLUGIN_INIT_FUNCTION);

	if (!entropy_plugin_init) {
		printf("Registered plugin did not contain function '%s', abort\n",ENTROPY_PLUGIN_INIT_FUNCTION );
		return NULL;
	}
	
	plugin = (*entropy_plugin_init)(entropy_core_get_core());
	plugin->dl_ref = dl_ref;

	plugin->functions.entropy_plugin_init = entropy_plugin_init;
        strncpy(plugin->filename, filename, 254);

	plugin->name = entropy_plugin_plugin_identify(plugin);

	return plugin;
}


Ecore_Hash* entropy_thumbnailers_register_init() {
        return ecore_hash_new(ecore_str_hash, ecore_str_compare);
}

Ecore_List* entropy_mime_register_init() {
        return ecore_list_new();
}

entropy_plugin* entropy_plugin_layout_register(entropy_plugin* plugin) {
        return plugin;
}

void entropy_plugin_mime_register(Ecore_List* mime_plugins, entropy_plugin* plugin) {
        //printf("Registering MIME plugin: '%s'\n", plugin->filename);
        ecore_list_append(mime_plugins, plugin);
}

void entropy_plugin_thumbnailer_register(entropy_core* core, entropy_plugin* plugin, int type) {
        /*First off, query the plugin to see what MIME types it handles*/
        char* mime_type;
        Ecore_List* (*entropy_thumbnailer_plugin_mime_types_get)();
        Ecore_List* mime_types;

        //printf("Registering MIME types for thumbnailer..\n");

        entropy_thumbnailer_plugin_mime_types_get = dlsym(plugin->dl_ref, "entropy_thumbnailer_plugin_mime_types_get");
        mime_types = (*entropy_thumbnailer_plugin_mime_types_get)();
        ecore_list_first_goto(mime_types);
        while ( (mime_type = ecore_list_next(mime_types)) ) {
		if (type == THUMBNAILER_DISTRIBUTION) { 
			distrib_plugin = plugin;
		} else if (type == THUMBNAILER_CHILD) {
			Ecore_List* list = NULL;
			
			if (! (list = ecore_hash_get(core_core->entropy_thumbnailers_child, mime_type))) {
				list = ecore_list_new();
				ecore_hash_set(core->entropy_thumbnailers_child, mime_type,list); 
			}
			ecore_list_append(list, plugin);


			/*Assume for now that all distribution plugins can handle proxying to all child thumbnailers
			 * This is a pretty big jump - but *generally* you'll only have one distribution plugin
			 * FIXME - maybe.
			 */
		}
        }

	/*Clean up the list that we got from the plugin*/
	ecore_list_destroy(mime_types);
}

entropy_generic_file* entropy_generic_file_clone(entropy_generic_file* file) {
	entropy_generic_file* clone = entropy_malloc(sizeof(entropy_generic_file));

	strncpy(clone->filename, file->filename, FILENAME_LENGTH);
	strncpy(clone->path, file->path, PATH_MAX);
	strncpy(clone->mime_type, file->mime_type, MIME_LENGTH);
	strncpy(clone->uri_base, file->uri_base, 15);

	if (file->username) 
		clone->username = strdup(file->username);
	else
		clone->username = NULL;

	if (file->password) 
		clone->password = strdup(file->password);
	else
		clone->password = NULL;

	if (file->uri)
		clone->uri = strdup(file->uri);
	else
		clone->uri = NULL;

	clone->parent = file->parent;

	if (file->thumbnail) {
		/*FIXME - we should make this more generic*/
		clone->thumbnail = entropy_thumbnail_new();
		strcpy(clone->thumbnail->thumbnail_filename, file->thumbnail->thumbnail_filename);
		clone->thumbnail->parent = (void*)clone;
	}
	
	clone->md5 = strdup(file->md5);
	
	return clone;
}


void entropy_core_selection_engine_init() {
	core_core->selected_files = ecore_list_new();
}

void entropy_core_selection_type_set(Entropy_Selection_Type stype)
{
	core_core->selection_type = stype;
}

Entropy_Selection_Type entropy_core_selection_type_get()
{
	return core_core->selection_type;
}

void entropy_core_selected_file_add(entropy_generic_file* file) {
	ecore_list_append(core_core->selected_files, file);

	/*We need this file to stay around, so add a reference*/
	entropy_core_file_cache_add_reference(file->md5);

	/*Print the filename, for ref*/
	//printf("Retrieved file to clipboard: %s\n", file->filename);
}

Ecore_List* entropy_core_selected_files_get() {
	return core_core->selected_files;
}


void entropy_core_selected_files_clear() {
	entropy_generic_file* file;
		
	while ( (file = ecore_list_first_remove(core_core->selected_files))) {
		entropy_core_file_cache_remove_reference(file->md5);
	}
	
}



/*Register a layout object, so that gui components that are members of this layout can send notifications to each other
 * We do this here so that the layout component can be "logically" in control of its components. without having to code an
 * event engine inside each layout plugin.  This keeps in line with the main philosophy of entropy - keeping a plugin doing
 * what it says it does */
void entropy_core_layout_register(entropy_core* core, entropy_gui_component_instance* comp) {
	Ecore_Hash* hash;
	
	/*First, make a new hash of event types for this new layout component, this will be the event hash*/
	if (comp) {
		printf("Registered layout: %p\n", comp);
		hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
		ecore_hash_set(core->layout_gui_events, comp, hash);

		//printf("Registered a layout component with the core, layout at: %p.., new hash at %p\n", comp,hash);
	} else {
		//printf ("Cannot register a null layout container\n");
	}
}

/*Deregister a layout object..*/
void entropy_core_layout_deregister(entropy_core* core, entropy_gui_component_instance* comp) 
{
	Ecore_Hash* hash;
	
	/*First, make a new hash of event types for this new layout component, this will be the event hash*/
	if (comp) {
		printf("DeRegistered layout: %p\n", comp);
		hash = ecore_hash_get(core->layout_gui_events, comp);
		if (hash) {
			ecore_hash_destroy(hash);
			ecore_hash_remove(core->layout_gui_events, comp);
		}
		//printf("DeRegistered a layout component with the core, layout at: %p.., new hash at %p\n", comp,hash);
	} else {
		//printf ("Cannot deregister a null layout container\n");
	}
	
}

/*Register a component to get events created by others in the same layout container*/
void entropy_core_component_event_register(entropy_gui_component_instance* comp, char* event) {
	entropy_gui_component_instance* layout;
	entropy_gui_component_instance* iter;
	int found = 0;


	if (!comp->layout_parent) {
		/*This must be a layout (or the programmer has messed up,
		 * use this as the layout*/
		layout = comp;
		/*printf(" ******* Layout registered itself to receive events\n");*/
	} else {
		layout = comp->layout_parent;
	}
	
	/*First we have to see if this layout is currently registered with the core..*/
	Ecore_Hash* event_hash = ecore_hash_get(core_core->layout_gui_events, layout);

	if (!event_hash) {
		printf("Alert! - tried to register events for unreg layout component, %p\n", layout);
	} else {
		Ecore_List* event_list = ecore_hash_get(event_hash, event);

		if (!event_list) {
			//printf("No events of this type(%p) yet registered for this layout, making a new list..\n", event);
			event_list = ecore_list_new();
			ecore_hash_set(event_hash, event, event_list);
		} else {
			ecore_list_first_goto(event_list);
			while ((iter = ecore_list_next(event_list))) {
				if (iter == comp) {
					found = 1;
				}
			}
		}

		if (!found) {
			/*Now add this component to this list*/
			ecore_list_append(event_list, comp);
			/*printf("Registered interest in '%s' for this gui component\n", event);*/
		}
	}
}

void entropy_core_component_event_deregister(entropy_gui_component_instance* comp, char* event)
{
	entropy_gui_component_instance* layout;
	entropy_gui_component_instance* iter;

	if (!comp->layout_parent) {
		/*This must be a layout (or the programmer has messed up,
		 * use this as the layout*/
		layout = comp;
	} else {
		layout = comp->layout_parent;
	}

	/*First we have to see if this layout is currently registered with the core..*/
	Ecore_Hash* event_hash = ecore_hash_get(core_core->layout_gui_events, layout);

	if (!event_hash) {
		printf("Alert! - tried to de-register events for unreg layout component, %p\n", layout);
	} else {
		Ecore_List* event_list = ecore_hash_get(event_hash, event);

		if (event_list) {
			ecore_list_first_goto(event_list);
			while ((iter = ecore_list_current(event_list))) {
				if (iter == comp) {
					ecore_list_remove(event_list);
				} else {
					ecore_list_next(event_list);
				}
			}
		}

	}
}

void entropy_core_layout_notify_event(entropy_gui_component_instance* instance, entropy_gui_event* event, int event_type) {
	/*First get the layout hash*/
	entropy_gui_component_instance* iter;
	Ecore_List* el;	
	Ecore_Hash* lay_hash;
	entropy_gui_component_instance* layout = NULL;
	Entropy_Gui_Event_Handler* handler;
	Ecore_List* handlers;

	if (!instance) {
		printf("entropy_core_layout_notify_event: instance was NULL\n");	
		return;
	}
	
	if (event_type == ENTROPY_EVENT_LOCAL) {

		//If layout_parent is null, assume passed object *is* a layout.  FIXME bad - we should probably set a prop to layout
		if (instance->layout_parent == NULL) {
			layout = instance;
		} else {
			layout = instance->layout_parent;
		}
	} else if (event_type == ENTROPY_EVENT_GLOBAL) {
		layout = entropy_core_global_layout_get(instance->core);
	}
	//printf("Instance's core reference: instance: %p, %p\n", instance, instance->core);
	lay_hash = ecore_hash_get(core_core->layout_gui_events, layout);
	if (!lay_hash) {
		printf("Error: Attempted to raise event for unregistered layout container (%p)\n", layout);
		entropy_free(event);
		return;
	}

	/*printf ("Got layout hash %p for layout at %p\n", lay_hash, layout);
	printf("Called event call, for event: '%s'\n", event->event_type);*/

	/*Otherwise, get the list of instances that care about this event*/
	el = ecore_hash_get(lay_hash, event->event_type);

	if (!el) {
		/*No-one cares about this event - perhaps we shouldn't exit here.  The caller may still want it to run*/
		printf("entropy_core: Nobody registered to receive this event (%s)\n", event->event_type);
		entropy_free(event);
		return;
	}


	handlers = entropy_core_gui_event_handlers_get(event->event_type);
	if (handlers) {
		Entropy_Gui_Event_Handler_Instance_Data* data = NULL;	
		
		ecore_list_first_goto(handlers);
		while ( (handler = ecore_list_next(handlers))) {

			data = (*handler->notify_event_cb)(event,instance);
			
			if (data->notify) {
				ecore_list_first_goto(el);
				while ( (iter = ecore_list_next(el)) ) {
					if (iter->active && data->notify->return_struct) {
						(*iter->plugin->gui_event_callback_p)
						(data->notify, 
						 instance, 
						 data->notify->return_struct,   /*An entropy_generic_file*/
						 iter);
					}
				}
			} else {
				/*printf("No notify event returned!\n");*/
			}

			(*handler->cleanup_cb)(data);

		}

		entropy_free(event);
		return;
	} else {
		printf(" *** No registered handlers for this event (%s)\n", event->event_type);
	}
	

	if (!strcmp(event->event_type,ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS_EXTERNAL)) {
		Ecore_List* file_list;
		entropy_file_request* request;
		
		entropy_notify_event* ev = entropy_notify_event_new();
		ev->event_type = ENTROPY_NOTIFY_FILELIST_REQUEST_EXTERNAL; 
				ev->processed = 1;

	
		/*The first thing in the calling event data (an ecore_list) is the original request*/
		file_list = event->data;
		ecore_list_first_goto(file_list);
		request = ecore_list_next(file_list);

		//printf("Path of the file inside the request inside the list: '%s'\n", request->file->path);

		/*Assign the file request to the outgoing event data*/
		ev->data = request;
		ev->return_struct = file_list;

		/*Now get rid of the request, so we're left with a virginal list of files*/
		ecore_list_first_remove(file_list);

		/*Register this folder as being the current for this layout*/
		if (((entropy_gui_component_instance*)request->requester)->layout_parent)  {
			( (entropy_gui_component_instance_layout*)
			       ((entropy_gui_component_instance*)request->requester)->layout_parent)->current_folder = 
				request->file;
		}
		
		/*Call the requestors*/
		ecore_list_first_goto(el);
		while ( (iter = ecore_list_next(el)) ) {
			//printf( "Calling callback at : %p\n", iter->plugin->gui_event_callback_p);
			
			if (iter->active) (*iter->plugin->gui_event_callback_p)
				(ev, 
				 iter, 
				 file_list,   /*An entropy_file_stat*/
				 iter);
		}
		entropy_notify_event_destroy(ev);
		
	} else {
		fprintf(stderr, "entropy_core: Unknown event type called\n");
	}


	entropy_free(event);
	/*free(request);*/
}
	


void entropy_core_string_lowcase(char *lc) {
		for(;*lc;lc++)
			*lc = (unsigned char)tolower((unsigned char)*lc);
}

char* md5_entropy_local_file(char* filename)
{
	return md5_entropy_path_file("file",filename,"");
}

char* md5_entropy_path_file(char* plugin, char* folder, char* filename) {
        char* md5;
	char full_name[PATH_MAX];
        md5_state_t state;
        md5_byte_t digest[16];
        static const char hex[]="0123456789abcdef";
        int i;

	md5 = entropy_malloc(sizeof(char) * 33);

	/*printf("MD5'ing %s %s\n", path, filename);*/

	if (strlen(filename))
		snprintf(full_name, 1024, "%s%s/%s", plugin, folder, filename);
	else
		snprintf(full_name, 1024, "%s%s", plugin, folder);
	
       md5_init(&state);
       md5_append(&state, (const md5_byte_t*)full_name, strlen(full_name));
       md5_finish(&state, digest);
       for (i=0;i<16;i++) {
               md5[2*i] = hex[digest[i]>>4];
               md5[2*i+1]=hex[digest[i]&0x0f];
       }
       md5[2*i] = '\0';

       return md5;
}


Ecore_List* entropy_core_file_cache_keys_retrieve() 
{
	Ecore_List* ret = NULL;
	
	ret = ecore_hash_keys(core_core->file_interest_list);

	return ret;
}


void entropy_core_file_cache_add(char* md5, entropy_file_listener* listener) {

	
	if (!ecore_hash_get(core_core->file_interest_list, md5)) {
		ecore_hash_set(core_core->file_interest_list, md5, listener);
	} else {
		printf("*** BAD: Called set-reference with file already cached!\n");

		entropy_core_file_cache_add_reference(md5);
	}

	entropy_generic_file_uri_set(listener->file);
	
	file_cache_size++;
	/*printf("File cache goes to %ld\n", file_cache_size);*/
	
}

entropy_generic_file* entropy_core_uri_generic_file_retrieve(char* uri) {
	entropy_generic_file* file;
	
	file = ecore_hash_get(core_core->uri_reference_list, uri);
	return file;
}


void entropy_core_file_cache_add_reference(char* md5) {
	entropy_file_listener* listener = ecore_hash_get(core_core->file_interest_list, md5);

	if (listener) {
		listener->count++;

		/*At this point, check if the file needs a uri generating..
		 * And generate if necesary */
		entropy_generic_file_uri_set(listener->file);

		//printf("+ Ref count for '%s/%s' -> %d..\n", listener->file->path, listener->file->filename, listener->count);
	}
}

void entropy_core_file_cache_remove_reference(char* md5) {
	entropy_file_listener* listener = ecore_hash_get(core_core->file_interest_list, md5);

	if (listener) {
		listener->count--;

		/*printf("- Ref count for (%p) '%s/%s' -> %d..\n", listener->file, listener->file->path, listener->file->filename, listener->count);*/
		if (listener->count <= 0) {
			/*printf("  Cleaning up above...\n");*/

			ecore_hash_remove(core_core->uri_reference_list, listener->file->uri);

			ecore_hash_remove(core_core->file_interest_list, md5);
			
			entropy_generic_file_destroy(listener->file);
			free(listener);

			

			file_cache_size--;

		} 
	}
}

entropy_file_listener* entropy_core_file_cache_retrieve(char* md5) {
	entropy_file_listener* listen;
	

	listen = ecore_hash_get(core_core->file_interest_list, md5);
	

	return listen;
}

entropy_gui_component_instance* entropy_core_global_layout_get(entropy_core* core) {
	return core->layout_global;
}



char* entropy_core_home_dir_get() {
	return core_core->user_home_dir;
}

char* entropy_thumbnail_dir_get() {
	return core_core->thumbnail_path;
}



entropy_generic_file* entropy_core_parse_uri(char* uri) {
	char* pos;
	entropy_generic_file* file; 	
	evfs_file_uri_path* uri_path; 
	entropy_file_listener* listener;

	/*If we already have this file made, don't remake it..*/
	/*printf("Looking for file with uri: %s...\n", uri);*/
	if ( (file = entropy_core_uri_generic_file_retrieve(uri))) {
		/*printf("Found %p!, returning...\n", file);*/
		return file;
	}

	file = entropy_generic_file_new();
	uri_path = evfs_parse_uri(uri);

	/*printf("Direct after evfs: %s \n", uri_path->files[0]->path);*/

	/*Get the last "/", after this is the filename (or dir name, or whatever)*/
	/*printf("Path: '%s'\n", uri_path->files[0]->path);*/
	pos = rindex(uri_path->files[0]->path, '/');
	strncpy(file->filename, pos+1, strlen(uri_path->files[0]->path));
	*pos = '\0';

	/*If length is 0, it must have been a '/', therefore make this the new path*/
	if (strlen(pos+1)) {
		strncpy(file->path,  uri_path->files[0]->path, strlen(uri_path->files[0]->path));
	} else {
		//strcpy(file->path, "/");
		strcpy(file->filename, "/");
	}

	if (uri_path->files[0]->username) {
		file->username = strdup(uri_path->files[0]->username);
		file->password = strdup(uri_path->files[0]->password);
	}
	
	strcpy(file->uri_base, uri_path->files[0]->plugin_uri);

	/*printf("After EVFS: %s/%s\n", file->path,file->filename);*/

	/*Cache the file*/
	listener = entropy_malloc(sizeof(entropy_file_listener));
	file->md5 = md5_entropy_path_file(file->uri_base, file->path, file->filename);
	listener->file = file;
	listener->count = 1;
	entropy_core_file_cache_add(file->md5, listener);

	/*Cleanup*/ /*--FIXME*/
	free(uri_path);

	return file;
}



char* entropy_core_descent_for_mime_get(entropy_core* core, char* mime) {
	return ecore_hash_get(core->descent_hash, mime);
}




void entropy_log(char* message, const int level) {
	/*Levels:
	 * ENTROPY_LOG_INFO
	 * ENTROPY_LOG_WARN
	 * ENTROPY_LOG_ERROR
	 * ENTROPy_LOG_DEBUG
	 */

	if (DEBUG_LEVEL >= level) {
		fprintf(stderr, message);
	}


}


entropy_generic_file* entropy_core_parent_folder_file_get(entropy_generic_file* file) 
{
	char* md5;
	char* tmp;
	char* pos;
	entropy_file_listener* listen;
	entropy_generic_file* return_file = NULL;

	/*First get the md5sum of the file that will be this file's parent folder...*/
	tmp = strdup(file->path);
	pos = strrchr(tmp, '/');
	if (pos) 
		*pos = '\0';
	else
		return NULL; /*No parent - this is the root, i.e. '/' */
		
	md5 = md5_entropy_path_file(file->uri_base, tmp, pos+1);
	listen = entropy_core_file_cache_retrieve(md5);
		
	if (listen) {
		return_file = listen->file;
	}
	free(tmp);
	free(md5);

	return return_file;

}

char* entropy_core_generic_file_uri_create (entropy_generic_file* file, int drill_down) {
	entropy_generic_file* source_file;
	char* uri = entropy_malloc(PATH_MAX);
	char* uri_retrieve;
	char uri_build[PATH_MAX];
		
	/*If the file/location we are requesting has a 'parent' (i.e. it's inside another object),
	 * we have to grab the parent, not the file itself, as the base*/
	if (file->parent) {
		source_file = file->parent;
	} else
	source_file = file;


	/*Do we have login information*/
	if (!source_file->username) {
		
		/*Handle root path*/
		if (strlen(source_file->path) > 0) { 
			if (!strcmp(source_file->path, "/")) {
					snprintf(uri, 512, "%s:///%s",  source_file->uri_base, 
					source_file->filename);						
			} else {
					snprintf(uri, 512, "%s://%s/%s",  source_file->uri_base, 
					source_file->path, source_file->filename);			
			}
		} else
			snprintf(uri, 512, "%s://%s",  source_file->uri_base, source_file->filename);
	} else {
		snprintf(uri, 512, "%s://%s:%s@%s/%s",  source_file->uri_base, 
		source_file->username, source_file->password, 
		source_file->path, source_file->filename);
	}
	
			
	/*printf("EVFS says that this file descends through '%s'\n", uri);*/
		
	if (drill_down || file->parent) {
		/*If we're a 'drill-down', we're at the root - so request the root*/
		if (drill_down) {
			uri_retrieve = entropy_core_descent_for_mime_get(core_core,file->mime_type);
			snprintf(uri_build, 255, "#%s:///", uri_retrieve);
			/*printf("URI build says: '%s'\n", uri_build);*/
			strcat(uri, uri_build); 
		} else if (file->parent) {
			/*printf("Retrieving mime-descend from parent...'%s' for file with name '%s'\n", 
			file->parent->mime_type, file->parent->filename);*/

			uri_retrieve = entropy_core_descent_for_mime_get(core_core,file->parent->mime_type);

			/*Special case handler for the root dir - FIXME*/
			/*printf("Path: '%s', filename '%s'\n", file->path, file->filename);*/
			if (!strcmp(file->path,"/")) {
				snprintf(uri_build, 255, "#%s://%s%s", uri_retrieve, file->path, file->filename);
			} else {
				snprintf(uri_build, 255, "#%s://%s/%s", uri_retrieve, file->path, file->filename);
			}
			strcat(uri, uri_build);
		}
	}

	return uri;
}


entropy_generic_file* EvfsFilereference_to_entropy_generic_file(void* ref) 
{
	EvfsFilereference* file_ref = (EvfsFilereference*)ref;
	char *copy = strdup (file_ref->path);
	char *pos = strrchr (copy, '/');
	entropy_generic_file* file = entropy_generic_file_new();
	
	*pos = '\0';
	pos++;

	strncpy(file->path, copy, 255);
	strncpy(file->filename, pos, FILENAME_LENGTH);
	if (file_ref->plugin_uri) 
		strncpy(file->uri_base, file_ref->plugin_uri, 15);

	free(copy);
	return file;
	
}


/* Associate an object with an entropy_generic_file - e.g. an ewl_widget with a file - 
 * Mostly used for transparent DND between objects */
void entropy_core_object_file_associate(void* object, entropy_generic_file* file) {
	if (object) ecore_hash_set(core_core->object_associate_hash, object, file);
}

void entropy_core_object_file_disassociate(void* object) {
	if (object) ecore_hash_remove(core_core->object_associate_hash, object);
}

entropy_generic_file* entropy_core_object_file_association_get(void* object) {
	return ecore_hash_get(core_core->object_associate_hash, object);
}


void* entropy_malloc(size_t size) {
	return calloc(1,size);
}

void entropy_free(void* ref) {
	if (ref) {
		//printf("                   ********************** %d requests for mem (DOWN)\n", requests);
		free(ref);
	} else {
		printf ("Bad! Freeing null!\n");
	}
}



