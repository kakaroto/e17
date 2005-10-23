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

#include "entropy.h"
#include <ctype.h>
#include <pthread.h>
#include "md5.h"
#include "entropy_gui.h"

#define ENTROPY_CORE 1

static int requests = 0;
static long file_cache_size = 0;


int ecore_timer_enterer(void* data) {
	return 1;
}


int
ipc_client_data(void *data, int type, void *event)
{
	
	entropy_core* core = data;
	entropy_notify_event_cb_data* cb_data;
	int destroy_struct =0;

	
	/*printf ("Received message\n");*/

	pthread_mutex_lock(&core->notify->exe_queue_mutex);

	entropy_notify_event* eevent;
	

	ecore_list_goto_first(core->notify->exe_queue);
	if ( (eevent = ecore_list_next(core->notify->exe_queue)) ) {
		ecore_list_remove_first(core->notify->exe_queue);
		
		
		/*If the return struct is null, don't call the callbacks.  The requester has taken responsibility
		 * for calling the requesters of this type when the data is available, which is obviously not now*/
		
		if (eevent->return_struct) {
			ecore_list_goto_first(eevent->cb_list);
			while ( (cb_data = ecore_list_next(eevent->cb_list)) ) {
				(*cb_data->cb)(eevent, eevent->requestor_data, eevent->return_struct, cb_data->data);
			}
			destroy_struct = 1;
		} else {
			entropy_log ("ipc_client_data: RETURN was NULL.  Caller will notify when data ready\n", ENTROPY_LOG_WARN);
			destroy_struct = 0;
		}

		/*TODO move this to a dedicated cleanup function*/
		if (destroy_struct) {
				switch (eevent->event_type) {
					case ENTROPY_NOTIFY_FILELIST_REQUEST:
					/*It's a filelist request - return is an ecore list - destroy*/
					ecore_list_destroy(eevent->return_struct);
					break;

					case ENTROPY_NOTIFY_THUMBNAIL_REQUEST:
					/*A thumbnail - we want to keep this, don't destroy anything*/
					break;

					case ENTROPY_NOTIFY_FILE_STAT_AVAILABLE:
					//entropy_file_stat* stat = eevent->return_struct;
				
					break;
			
				}
		}
		
		entropy_notify_event_destroy(eevent);


		
	}
	pthread_mutex_unlock(&core->notify->exe_queue_mutex);


	return 1;

}

entropy_core* entropy_core_new() {
	entropy_core* core = entropy_malloc(sizeof(entropy_core));

	return core;
}



entropy_core* entropy_core_init() {
	entropy_plugin* plugin;
	void (*entropy_plugin_layout_main)();
	entropy_gui_component_instance* (*entropy_plugin_layout_create)(entropy_core*);
	entropy_gui_component_instance* layout;

        struct dirent* de;
        DIR* dir;
	char plugin_path[1024];

	
	entropy_core* core = entropy_core_new();

	/*Init the file cache mutex*/
	pthread_mutex_init(&core->file_cache_mutex, NULL);

	/*Initialise the ecore ipc system*/
	if (ecore_ipc_init() < 1) {
		printf("Couldn't connect to the ecore_ipc subsystem\n");
		exit(1);
	}

	//printf("Creating the ipc server...\n");
	core->server = ecore_ipc_server_add(ECORE_IPC_LOCAL_USER, IPC_TITLE, 0, NULL);

	if (core->server == NULL) {
		printf("creating the IPC server failed! Entropy already running? FIXME we should launch another layout here\n");
		exit(1);
	}

	//printf ("Adding client data hander...\n");
        ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DATA, ipc_client_data,core);



	//printf ("Initialising the file cache..\n");
	core->file_interest_list = ecore_hash_new(ecore_str_hash, ecore_str_compare);

	//Misc setup (thumbnail path)
	core->user_home_dir = strdup(getenv("HOME"));
	core->thumbnail_path = entropy_malloc((strlen(core->user_home_dir) + 21)*sizeof(char));
	snprintf(core->thumbnail_path, strlen(core->user_home_dir) + 21, "%s/%s", core->user_home_dir, ".thumbnails/normal/");


	//Initialise the config system
	core->config = entropy_config_init(core);

	//Load the plugins
	core->plugin_list = ecore_list_new();
        //printf("Reading plugins from: %s\n", PACKAGE_DATA_DIR "/plugins/");
        dir = opendir(PACKAGE_DATA_DIR "/plugins/");
        if (dir) {
                while ( (de = readdir(dir)) ) {

                   if (!strncmp(de->d_name + strlen(de->d_name) -3, ".so", 3)) {
                        snprintf(plugin_path, 1024,"%s/%s", PACKAGE_DATA_DIR "/plugins/", de->d_name);
			
			ecore_list_append(core->plugin_list, create_plugin_object(plugin_path));
                   }
                }
        } else {
                fprintf(stderr, "Entropy: Could not location plugin directory '%s'\n", PACKAGE_DATA_DIR "/plugins/");
                exit(1);
        }

	/*Initialise the layout event hash*/
	core->layout_gui_events = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);

	//printf("Creating the global layout object..\n");
	layout = entropy_malloc(sizeof(entropy_gui_component_instance));
	layout->core = core;
	core->layout_global = layout;
	entropy_core_layout_register(core, layout);

        /*Init the mime register */
        core->entropy_thumbnailers = entropy_thumbnailers_register_init();
        core->mime_plugins = entropy_mime_register_init();

        /*Load plugins*/
        ecore_list_goto_first(core->plugin_list);
        while ( (plugin = ecore_list_next(core->plugin_list)) ) {
                entropy_plugin_load(core, plugin);
        }

	//printf("\n\nDetails of thumbnailers:\n");

        /*Show some details*/
        //entropy_thumbnailer_plugin_print(core->entropy_thumbnailers);



	/*Initialize the selection engine*/
	entropy_core_selection_engine_init(core);



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
	
	entropy_plugin_layout_main = dlsym(core->layout_plugin->dl_ref, "entropy_plugin_layout_main");

	
	/*FIXME why doesn't the UI update unless we have an empty ecore_timer here? Try removing this, and thumbnail a large dir without
	 * moving the mouse */
	ecore_timer_add(0.5, ecore_timer_enterer, NULL); 



	


	//printf("--------------- Running main\n");
	(*entropy_plugin_layout_main)();

	


        /*Right lets go*/
        //printf("\n\n");


	return core;
}


void entropy_core_destroy(entropy_core* core) {
	entropy_plugin* plugin;
	entropy_file_listener* listener;
	Ecore_List* hash_keys;
	char* key;

	//printf("Destroying config subsystem...\n");
	entropy_config_destroy(core->config);

	/*Destroy the notification engines*/
	entropy_notification_engine_destroy(core->notify);


	ecore_list_goto_first(core->plugin_list);
	while ( (plugin = ecore_list_next(core->plugin_list) )) {
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
	ecore_list_goto_first(hash_keys);
	while ( (key = ecore_list_next(hash_keys)) ) {
		listener = ecore_hash_get(core->file_interest_list, key);

		entropy_generic_file_destroy(listener->file);
		entropy_free(listener);
		ecore_hash_remove(core->file_interest_list, key);
		free(key);


	}

	ecore_list_destroy(hash_keys);
	ecore_hash_destroy(core->file_interest_list);
	

	/*Destroy the plugins*/
	/*Destroy the eLists/eHashes*/

}

void entropy_layout_run(entropy_core* core) {

}


/*A function we need, unfortunately, because of the way dlopen works with string refs, and the need of ecore_hash for 
 * a pointer */
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
	} else if (!strcmp(event, ENTROPY_GUI_EVENT_ACTION_FILE)) {
		return "entropy_gui_event_action_file";
	} else if (!strcmp(event, ENTROPY_GUI_EVENT_FILE_STAT)) {
		return "entropy_gui_event_file_stat";
	} else if (!strcmp(event, ENTROPY_GUI_EVENT_FILE_STAT_AVAILABLE)) {
		return "entropy_gui_event_file_stat_available";
	} else {
		return "";
	}
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
        //printf("Plugin IDs as: '%s'\n", (*entropy_plugin_identify)());

        type = (*entropy_plugin_type_get)();
	plugin->type = type;

	/*Load the subtype, if there is one*/
	if (entropy_plugin_sub_type_get) {
		plugin->subtype = (*entropy_plugin_sub_type_get)();
	}


        if (type == ENTROPY_PLUGIN_THUMBNAILER) {
                //printf(" ----------------------- Thumbnailer Plugin, registering with engine..\n");
                entropy_plugin_thumbnailer_register(core, plugin);
        } else if (type == ENTROPY_PLUGIN_MIME) {
                //printf("MIME Identifier Plugin, registering with engine..\n");
                entropy_plugin_mime_register(core->mime_plugins, plugin);
        } else if (type == ENTROPY_PLUGIN_GUI_LAYOUT) {
                //printf("Found a layout manager.\n");
                core->layout_plugin = entropy_plugin_layout_register(plugin);
		

		/*Initializing..*/
		entropy_plugin_init = dlsym(plugin->dl_ref, "entropy_plugin_init");
		(*entropy_plugin_init)(core);
		
		
        } else if (type == ENTROPY_PLUGIN_GUI_COMPONENT) {
		/* TODO Get the subtype */

		gui_event_callback = dlsym(plugin->dl_ref, "gui_event_callback");
		plugin->gui_event_callback_p = gui_event_callback;

		//printf("Setting components event callback to %p\n", gui_event_callback);
		
	} else if (type == ENTROPY_PLUGIN_BACKEND_FILE) {
		entropy_plugin_init = dlsym(plugin->dl_ref, "entropy_plugin_init");
		(*entropy_plugin_init)(core);	
	} else if (type == ENTROPY_PLUGIN_ACTION_PROVIDER) {
		entropy_gui_component_instance* instance;

		//printf ("Processing an action provider...\n");

		/*Initializing..*/
		entropy_plugin_init = dlsym(plugin->dl_ref, "entropy_plugin_init");
		instance = (*entropy_plugin_init)(core);
		instance->plugin = plugin;

		gui_event_callback = dlsym(plugin->dl_ref, "gui_event_callback");
		plugin->gui_event_callback_p = gui_event_callback;

		//printf ("SETTING ACTION callback at : %p\n", gui_event_callback);
	}




	return 0;
}


entropy_plugin* create_plugin_object(char* filename) {
        entropy_plugin* plugin = entropy_malloc(sizeof(entropy_plugin));
        strncpy(plugin->filename, filename, 254);

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

void entropy_plugin_thumbnailer_register(entropy_core* core, entropy_plugin* plugin) {
        /*First off, query the plugin to see what MIME types it handles*/
        char* mime_type;
        Ecore_List* (*entropy_thumbnailer_plugin_mime_types_get)();
        Ecore_List* mime_types;

        //printf("Registering MIME types for thumbnailer..\n");

        entropy_thumbnailer_plugin_mime_types_get = dlsym(plugin->dl_ref, "entropy_thumbnailer_plugin_mime_types_get");
        mime_types = (*entropy_thumbnailer_plugin_mime_types_get)();
        ecore_list_goto_first(mime_types);
        while ( (mime_type = ecore_list_next(mime_types)) ) {
                //printf ("    Loading mime_type for this thumbnailer '%s'\n", mime_type);
                ecore_hash_set(core->entropy_thumbnailers, mime_type, plugin);
        }

	/*Clean up the list that we got from the plugin*/
	ecore_list_destroy(mime_types);





}

entropy_generic_file* entropy_generic_file_clone(entropy_generic_file* file) {
	entropy_generic_file* clone = entropy_malloc(sizeof(entropy_generic_file));
	
	memcpy(clone, file, sizeof(entropy_generic_file));
	return clone;
}


void entropy_core_selection_engine_init(entropy_core* core) {
	core->selected_files = ecore_list_new();
}

void entropy_core_selected_file_add(entropy_core* core, entropy_generic_file* file) {
	ecore_list_append(core->selected_files, file);

	/*We need this file to stay around, so add a reference*/
	entropy_core_file_cache_add_reference(core, file->md5);

	/*Print the filename, for ref*/
	//printf("Retrieved file to clipboard: %s\n", file->filename);
}

Ecore_List* entropy_core_selected_files_get(entropy_core* core) {
	return core->selected_files;
}


void entropy_core_selected_files_clear(entropy_core* core) {
	ecore_list_destroy(core->selected_files);
	core->selected_files = ecore_list_new();
}



/*Register a layout object, so that gui components that are members of this layout can send notifications to each other
 * We do this here so that the layout component can be "logically" in control of its components. without having to code an
 * event engine inside each layout plugin.  This keeps in line with the main philosophy of entropy - keeping a plugin doing
 * what it says it does */
void entropy_core_layout_register(entropy_core* core, entropy_gui_component_instance* comp) {
	Ecore_Hash* hash;
	
	/*First, make a new hash of event types for this new layout component, this will be the event hash*/
	if (comp) {
		hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
		ecore_hash_set(core->layout_gui_events, comp, hash);

		//printf("Registered a layout component with the core, layout at: %p.., new hash at %p\n", comp,hash);
	} else {
		//printf ("Cannot register a null layout container\n");
	}
}

/*Register a component to get events created by others in the same layout container*/
void entropy_core_component_event_register(entropy_core* core, entropy_gui_component_instance* comp, char* event) {
	/*First we have to see if this layout is currently registered with the core..*/
	Ecore_Hash* event_hash = ecore_hash_get(core->layout_gui_events, comp->layout_parent);

	//printf("   * Registering a component...\n");

	if (!event_hash) {
		printf("Alert! - tried to register events for unreg layout component, %p\n", comp->layout_parent);
	} else {
		Ecore_List* event_list = ecore_hash_get(event_hash, event);

		if (!event_list) {
			//printf("No events of this type(%p) yet registered for this layout, making a new list..\n", event);

			event_list = ecore_list_new();
			ecore_hash_set(event_hash, event, event_list);

		}

		/*Now add this component to this list*/
		ecore_list_append(event_list, comp);

		
		
		//printf("Registered interest in '%s' for this gui component\n", event);
		
		
	}
}

void entropy_core_layout_notify_event(entropy_gui_component_instance* instance, entropy_gui_event* event, int event_type) {
	/*First get the layout hash*/

	entropy_gui_component_instance* layout = NULL;
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
	
	Ecore_Hash* lay_hash = ecore_hash_get(instance->core->layout_gui_events, layout);
	entropy_gui_component_instance* iter;
	Ecore_List* el;	


	if (!lay_hash) {
		//printf("Error: Attempted to raise event for unregistered layout container\n");
		entropy_free(event);
		return;
	}

	//printf ("Got layout hash %p for layout at %p\n", lay_hash, layout);

	//printf("Called event call, for event: '%s'\n", event->event_type);

	/*Otherwise, get the list of instances that care about this event*/
	el = ecore_hash_get(lay_hash, event->event_type);

	if (!el) {
		/*No-one cares about this event - perhaps we shouldn't exit here.  The caller may still want it to run*/
		//printf("entropy_core: Nobody registered to receive this event\n");
		entropy_free(event);
		return;
	}

	

	if (!strcmp(event->event_type,ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS)) {
		char fullpath[1024];	
		entropy_plugin* plugin;
		entropy_notify_event *ev;
		
		entropy_file_request* request = entropy_malloc(sizeof(entropy_file_request));

		/*A folder request's data is an entropy_generic_file*/
		
		//printf("File says: %s %s\n", ((entropy_generic_file*)event->data)->path, ((entropy_generic_file*)event->data)->filename);

		/*Check if we need to put a slash between the path/file*/
		if (strcmp(((entropy_generic_file*)event->data)->filename, "/") && strcmp(((entropy_generic_file*)event->data)->path, "/")) {		
			sprintf(fullpath, "%s/%s", ((entropy_generic_file*)event->data)->path, ((entropy_generic_file*)event->data)->filename);
		} else {
			sprintf(fullpath, "%s%s", ((entropy_generic_file*)event->data)->path, ((entropy_generic_file*)event->data)->filename);
		}
		//printf("NOTIFY GETTING folder: %s\n", fullpath);

		request->file = event->data;
		request->requester = instance->layout_parent; /*Requester is the layout parent - after all - one dir per layout at one time*/
		request->core = instance->core;
		request->file_type = FILE_ALL;

		
		/*FIXME We should get the caller's current file plugin from the caller - i.e. the gui instance*/
	        plugin = entropy_plugins_type_get_first(instance->core->plugin_list, ENTROPY_PLUGIN_BACKEND_FILE ,ENTROPY_PLUGIN_SUB_TYPE_ALL);
	       

		/*Make our new event for notification*/
		ev = entropy_notify_request_register(instance->core->notify, instance, ENTROPY_NOTIFY_FILELIST_REQUEST, 
					plugin, "filelist_get", request,  
					NULL);

		ecore_list_goto_first(el);
		while ( (iter = ecore_list_next(el)) ) {
			entropy_notify_event_callback_add(ev, (void*)iter->plugin->gui_event_callback_p, iter);
		}

		/*Add the obj to be nuked once we've executed*/
		entropy_notify_event_cleanup_add(ev, request);
		
		/*Tell the notify engine we're ready to run*/
		entropy_notify_event_commit(instance->core->notify, ev);

		

		
	} else if (!strcmp(event->event_type,ENTROPY_GUI_EVENT_FILE_CHANGE)) {
		entropy_notify_event* ev = entropy_notify_event_new();
		ev->event_type = ENTROPY_NOTIFY_FILE_CHANGE;
		
		//printf("Sending a file change event...\n");

		ecore_list_goto_first(el);
		while ( (iter = ecore_list_next(el)) ) {
			(*iter->plugin->gui_event_callback_p)
				(ev, 
				 iter, 
				 event->data,   /*An entropy_generic_file*/
				 iter);
		}
		entropy_notify_event_destroy(ev);


	} else if (!strcmp(event->event_type,ENTROPY_GUI_EVENT_FILE_CREATE)) {
		entropy_notify_event* ev = entropy_notify_event_new();
		ev->event_type = ENTROPY_NOTIFY_FILE_CREATE;
		
		//printf("Sending a file create event...\n");

		ecore_list_goto_first(el);
		while ( (iter = ecore_list_next(el)) ) {
			(*iter->plugin->gui_event_callback_p)
				(ev, 
				 iter, 
				 event->data,   /*An entropy_generic_file*/
				 iter);
		}
		entropy_notify_event_destroy(ev);

	} else if (!strcmp(event->event_type,ENTROPY_GUI_EVENT_FILE_REMOVE)) {
		entropy_notify_event* ev = entropy_notify_event_new();
		ev->event_type = ENTROPY_NOTIFY_FILE_REMOVE;
		
		//printf("Sending a file create event...\n");

		ecore_list_goto_first(el);
		while ( (iter = ecore_list_next(el)) ) {
			(*iter->plugin->gui_event_callback_p)
				(ev, 
				 iter, 
				 event->data,   /*An entropy_generic_file*/
				 iter);
		}
		entropy_notify_event_destroy(ev);



	
	} else if (!strcmp(event->event_type,ENTROPY_GUI_EVENT_ACTION_FILE)) {
		entropy_notify_event* ev = entropy_notify_event_new();
		ev->event_type = ENTROPY_NOTIFY_FILE_ACTION; 

		
		//printf ("Requested an action execute\n");

		/*Call the requestors*/
		ecore_list_goto_first(el);
		while ( (iter = ecore_list_next(el)) ) {
			//printf( "Calling callback at : %p\n", iter->plugin->gui_event_callback_p);
			
			(*iter->plugin->gui_event_callback_p)
				(ev, 
				 instance,  /*We use instance here, because the action runner needs to know the caller*/
				 event->data,   /*An entropy_generic_file*/
				 instance);
		}
		entropy_notify_event_destroy(ev);
		
	} else if (!strcmp(event->event_type,ENTROPY_GUI_EVENT_FILE_STAT)) {
		entropy_plugin* plugin;
		entropy_notify_event *ev;
		entropy_file_request* request = entropy_malloc(sizeof(entropy_file_request));

		/*Set up the request..*/
		request->file = event->data;
		request->core = instance->core;
		request->requester = instance->layout_parent;
		/*--------------------------------------------*/
		
		//printf ("Requested a stat on a file...md5 %s, pointer %p, path '%s', filename '%s'\n", ((entropy_generic_file*)event->data)->md5, ((entropy_generic_file*)event->data), ((entropy_generic_file*)event->data)->path, ((entropy_generic_file*)event->data)->filename );

		/*FIXME We should get the caller's current file plugin from the caller - i.e. the gui instance*/
	        plugin = entropy_plugins_type_get_first(instance->core->plugin_list, ENTROPY_PLUGIN_BACKEND_FILE ,ENTROPY_PLUGIN_SUB_TYPE_ALL);

		/*Make our new event for notification*/
		ev = entropy_notify_request_register(instance->core->notify, instance, ENTROPY_NOTIFY_FILE_STAT_EXECUTED, 
					plugin, "filestat_get",  request  ,  
					NULL);

		ecore_list_goto_first(el);
		while ( (iter = ecore_list_next(el)) ) {
			entropy_notify_event_callback_add(ev, (void*)iter->plugin->gui_event_callback_p, iter);
		}

		/*Tell the notify engine we're ready to run*/
		entropy_notify_event_commit(instance->core->notify, ev);

	} else if (!strcmp(event->event_type,ENTROPY_GUI_EVENT_FILE_STAT_AVAILABLE)) {
		entropy_notify_event* ev = entropy_notify_event_new();
		ev->event_type = ENTROPY_NOTIFY_FILE_STAT_AVAILABLE; 

		
		//printf ("Stat available for consumption - %p\n", event->data);

		/*Call the requestors*/
		ecore_list_goto_first(el);
		while ( (iter = ecore_list_next(el)) ) {
			//printf( "Calling callback at : %p\n", iter->plugin->gui_event_callback_p);
			
			(*iter->plugin->gui_event_callback_p)
				(ev, 
				 iter, 
				 event->data,   /*An entropy_file_stat*/
				 iter);
		}
		entropy_notify_event_destroy(ev);
		
	} else if (!strcmp(event->event_type,ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS_EXTERNAL)) {
		Ecore_List* file_list;
		entropy_file_request* request;
		
		entropy_notify_event* ev = entropy_notify_event_new();
		ev->event_type = ENTROPY_NOTIFY_FILELIST_REQUEST_EXTERNAL; 

	
		/*The first thing in the calling event data (an ecore_list) is the original request*/
		file_list = event->data;
		ecore_list_goto_first(file_list);
		request = ecore_list_next(file_list);

		//printf("Path of the file inside the request inside the list: '%s'\n", request->file->path);

		/*Assign the file request to the outgoing event data*/
		ev->data = request;
		
		/*Now get rid of the request, so we're left with a virginal list of files*/
		ecore_list_remove_first(file_list);
		
		
		//printf ("External folder list (e.g. from evfs) ready - %p\n", event->data);

		/*Call the requestors*/
		ecore_list_goto_first(el);
		while ( (iter = ecore_list_next(el)) ) {
			//printf( "Calling callback at : %p\n", iter->plugin->gui_event_callback_p);
			
			(*iter->plugin->gui_event_callback_p)
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


char* md5_entropy_path_file(char* path, char* filename) {
        char* md5;
	char full_name[1024];
        md5_state_t state;
        md5_byte_t digest[16];
        static const char hex[]="0123456789abcdef";
        int i;

	md5 = entropy_malloc(sizeof(char) * 33);

	/*printf("MD5'ing %s %s\n", path, filename);*/

	snprintf(full_name, 1024, "%s%s", path, filename);
	
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


void entropy_core_file_cache_add(entropy_core* core, char* md5, entropy_file_listener* listener) {

	LOCK(&core->file_cache_mutex);
	
	ecore_hash_set(core->file_interest_list, md5, listener);
	file_cache_size++;
	/*printf("File cache goes to %ld\n", file_cache_size);*/
	UNLOCK(&core->file_cache_mutex);
	
}

void entropy_core_file_cache_add_reference(entropy_core* core, char* md5) {
	LOCK(&core->file_cache_mutex);
	entropy_file_listener* listener = ecore_hash_get(core->file_interest_list, md5);

	if (listener) {
		listener->count++;
	}
	UNLOCK(&core->file_cache_mutex);
}

void entropy_core_file_cache_remove_reference(entropy_core* core, char* md5) {
	LOCK(&core->file_cache_mutex);

	entropy_file_listener* listener = ecore_hash_get(core->file_interest_list, md5);

	if (listener) {
		listener->count--;
		if (listener->count == 0) {

			/*This should be a seperate function*/
			entropy_generic_file_destroy(listener->file);
			free(listener);

			ecore_hash_remove(core->file_interest_list, md5);
			free(md5);

			file_cache_size--;
		}
	}
	UNLOCK(&core->file_cache_mutex);
}

entropy_gui_component_instance* entropy_core_global_layout_get(entropy_core* core) {
	return core->layout_global;
}



char* entropy_core_home_dir_get(entropy_core* core) {
	return core->user_home_dir;
}

char* entropy_thumbnail_dir_get(entropy_core* core) {
	return core->thumbnail_path;
}



entropy_generic_file* entropy_core_parse_uri(char* uri) {
	char* pos;
	entropy_generic_file* file = entropy_generic_file_new();
	evfs_file_uri_path* uri_path = evfs_parse_uri(uri);


	/*Get the last "/", after this is the filename (or dir name, or whatever)*/
	pos = rindex(uri_path->files[0]->path, '/');
	strncpy(file->filename, pos+1, strlen(uri_path->files[0]->path));
	*pos = '\0';

	/*If length is 0, it must have been a '/', therefore make this the new path*/
	if (strlen(pos+1)) {
		strncpy(file->path,  uri_path->files[0]->path, strlen(uri_path->files[0]->path));
	} else {
		strcpy(file->path, "/");
		strcpy(file->filename, "/");
	}

	if (uri_path->files[0]->username) {
		file->username = strdup(uri_path->files[0]->username);
		file->password = strdup(uri_path->files[0]->password);
	}
	
	strcpy(file->uri_base, uri_path->files[0]->plugin_uri);

	//printf("Made a file with plugin '%s', path '%s', filename '%s'\n", file->uri_base, file->path, file->filename);


	return file;
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


void* entropy_malloc(size_t size) {
	requests++;

	//printf("                   ********************** %d requests for mem (UP)\n", requests);
	return calloc(1,size);
}

void entropy_free(void* ref) {
	if (ref) {
		requests--;
		//printf("                   ********************** %d requests for mem (DOWN)\n", requests);
		free(ref);
	} else {
		printf ("Bad! Freeing null!\n");
	}
}



