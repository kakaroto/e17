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
#include <pthread.h>
#include "md5.h"
#include "entropy_gui.h"
#include <Epsilon.h>

#define ENTROPY_CORE 1

static int requests = 0;
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

	
	/*printf ("Received message\n");*/

	if (e->major == ENTROPY_IPC_EVENT_CORE) {
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
	} else if (e->major == ENTROPY_IPC_EVENT_LAYOUT_NEW) {
		entropy_gui_component_instance* (*entropy_plugin_layout_create)(entropy_core*);

		printf("New layout requested!\n");
		entropy_plugin_layout_create = dlsym(core->layout_plugin->dl_ref, "entropy_plugin_layout_create");
		(*entropy_plugin_layout_create)(core);
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
	core->settings.layout_engine = "ewl";
		
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
		Ecore_Ipc_Server* server = ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER, IPC_TITLE,0 ,NULL);
		if (server) {
			printf("Sending message to server!\n");
			ecore_ipc_server_send(server, ENTROPY_IPC_EVENT_LAYOUT_NEW, 0, 0, 0, 0, NULL,0); 
		}
		/*printf("creating the IPC server failed! Entropy already running?
		 * FIXME we should launch another layout here\n");
		 */
		ecore_main_loop_iterate();
		exit(0);
	}

	//printf ("Adding client data hander...\n");
        ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DATA, ipc_client_data,core);


	/*Initialize the mime hint hash*/
	core->mime_action_hint = ecore_hash_new(ecore_str_hash, ecore_str_compare);

	/*Initialize the object assoc. hash*/
	core->object_associate_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);

	//printf ("Initialising the file cache..\n");
	core->file_interest_list = ecore_hash_new(ecore_str_hash, ecore_str_compare);

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
        ecore_list_goto_first(core->plugin_list);
        while ( (plugin = ecore_list_current(core->plugin_list)) ) {
                int res = entropy_plugin_load(core, plugin);
		if (res) {
			printf("Removing plugin from list..%s\n", plugin->filename);
			ecore_list_remove(core->plugin_list);
		} else {
			printf("Going to next plugin...%s\n", plugin->filename);
			ecore_list_next(core->plugin_list);
		}
		
        }


	/*Now register all thumbnailers with the distribution plugin*/
	{
		char* mime= NULL;
		Ecore_List* keys = ecore_hash_keys(core->entropy_thumbnailers_child);

		ecore_list_goto_first(keys);
		while ( (mime = ecore_list_remove_first(keys))) {
			ecore_hash_set(core->entropy_thumbnailers, mime, distrib_plugin);
		}
	}

	

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

	
	/*FIXME why doesn't the UI update unless we have an empty ecore_timer here? 
	 * Try removing this, and thumbnail a large dir without
	 * moving the mouse */
	ecore_timer_add(0.2, ecore_timer_enterer, NULL); 



	


	//printf("--------------- Running main\n");

	printf("Going to main..\n");
	(*entropy_plugin_layout_main)();

	


        /*Right lets go*/
        //printf("\n\n");


	return core;
}

void entropy_core_mime_action_add(char* mime_type, char* action) {
	entropy_mime_action* action_o;
	
	if ( ! (action_o = ecore_hash_get  (core_core->mime_action_hint, mime_type))) {
			action_o = entropy_malloc(sizeof(entropy_mime_action));
			action_o->executable = strdup(action);
			ecore_hash_set(core_core->mime_action_hint, strdup(mime_type), action_o);
	} else {
			free(action_o->executable);
			action_o->executable = strdup(action);
	}
}

void entropy_core_config_load() {
	int count, new_count;
	Ecore_List* mime_type_actions;
	char* key;
	int i=0;
	char type[100];
	char* pos;
	entropy_mime_action* action;

	printf("Loading core config...\n");

	/*Set some defaults*/
	mime_type_actions = ecore_list_new();
	ecore_list_append(mime_type_actions, "image/jpeg:entice");
        ecore_list_append(mime_type_actions, "image/png:entice");
        ecore_list_append(mime_type_actions, "image/gif:entice");
        ecore_list_append(mime_type_actions, "text/html:/usr/bin/firefox");
        ecore_list_append(mime_type_actions, "text/csrc:gvim");
        ecore_list_append(mime_type_actions, "audio/x-mp3:xmms");
        ecore_list_append(mime_type_actions, "video/x-ms-wmv:mplayer");
        ecore_list_append(mime_type_actions, "video/mpeg:mplayer");
        ecore_list_append(mime_type_actions, "application/msword:abiword");
        ecore_list_append(mime_type_actions, "application/vnd.ms-excel:gnumeric");
        ecore_list_append(mime_type_actions, "video/x-msvideo:mplayer");

	new_count = ecore_list_nodes(mime_type_actions);

	if (  (!(count = entropy_config_int_get("core","mime_type_count"))) || count < new_count ) {	
		
	
		entropy_config_int_set("core", "mime_type_count", new_count);
		printf("Setting up initial mime types, writing %d (old was %d)..\n", new_count,count);

		while ( (key = ecore_list_remove_first(mime_type_actions))) {
			snprintf(type,50,"mimetype_action.%d", i);
		
			entropy_config_str_set("core", type, key);	
			printf("Wrote '%s' \n", key);

			i++;
		}

		ecore_list_destroy(mime_type_actions);
	}

	
	count = entropy_config_int_get("core","mime_type_count");
	printf("Have to load %d mime entries..\n", count);

	new_count=0;
	while (new_count < count) {
		snprintf(type,50,"mimetype_action.%d", new_count);
		key = entropy_config_str_get("core", type);
		pos = strrchr(key, ':');

		printf("Key is: '%s'\n", key);	

		if (pos >= 0) {
			*pos = '\0';
			printf("Loading '%s', is '%s' -> '%s'\n", type, key, pos+1);

			action = entropy_malloc(sizeof(entropy_mime_action));
			action->executable = strdup(pos+1);
				
			ecore_hash_set(core_core->mime_action_hint, strdup(key), action);
			
		}
		free(key);
		
		new_count++;
	}

	
	

	
}

void entropy_core_config_save() {
	Ecore_List* keys;
	int count;
	char key[100];
	char executable[256];
	char* gkey;
	int i=0;
	
	

	/*Save the mime_action config*/
	keys = ecore_hash_keys(core_core->mime_action_hint);
	count = ecore_list_nodes(keys);

	/*Set the count of mime_types*/
	entropy_config_int_set("core","mime_type_count", count);

	/*Write the types*/
	while (  (gkey = ecore_list_remove_first(keys))  ) {
		snprintf(key, 100, "mimetype_action.%d", i);

		snprintf(executable,256,"%s:%s", gkey, 
		((entropy_mime_action*)ecore_hash_get(core_core->mime_action_hint, gkey))->executable);
		
		printf("Saving '%s' for '%s' using '%s'\n", 
			((entropy_mime_action*)ecore_hash_get(core_core->mime_action_hint, gkey))->executable, gkey, key); 

		entropy_config_str_set("core", key, executable);

		i++;
	}

	
	
}

entropy_mime_action* entropy_core_mime_hint_get(char* mime_type) {
	return ecore_hash_get(core_core->mime_action_hint, mime_type);
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


	/*Destroy the notification engines*/
	entropy_notification_engine_destroy(core->notify);


	ecore_list_goto_first(core->plugin_list);
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
	ecore_list_goto_first(hash_keys);
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


	/*Save the config*/
	entropy_core_config_save();

	entropy_config_destroy(core->config);

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


        if (type == ENTROPY_PLUGIN_THUMBNAILER_DISTRIBUTION) {
		entropy_gui_component_instance* instance;
                //printf(" ----------------------- Thumbnailer Plugin, registering with engine..\n");
		//
		entropy_plugin_init = dlsym(plugin->dl_ref, "entropy_plugin_init");
		instance = (*entropy_plugin_init)(core);
		instance->plugin = plugin;

		gui_event_callback = dlsym(plugin->dl_ref, "gui_event_callback");
		plugin->gui_event_callback_p = gui_event_callback;
	
                entropy_plugin_thumbnailer_register(core, plugin, THUMBNAILER_DISTRIBUTION);
	} else if (type == ENTROPY_PLUGIN_THUMBNAILER) {
		entropy_gui_component_instance* instance;
		
		entropy_plugin_init = dlsym(plugin->dl_ref, "entropy_plugin_init");
		instance = (*entropy_plugin_init)(core);
		instance->plugin = plugin;

		gui_event_callback = dlsym(plugin->dl_ref, "gui_event_callback");
		plugin->gui_event_callback_p = gui_event_callback;
	
                entropy_plugin_thumbnailer_register(core, plugin, THUMBNAILER_CHILD);
        } else if (type == ENTROPY_PLUGIN_MIME) {
                //printf("MIME Identifier Plugin, registering with engine..\n");
                entropy_plugin_mime_register(core->mime_plugins, plugin);

		/*Initializing..*/
		entropy_plugin_init = dlsym(plugin->dl_ref, "entropy_plugin_init");
		(*entropy_plugin_init)(core);


        } else if (type == ENTROPY_PLUGIN_GUI_LAYOUT) {
                char* id = entropy_plugin_plugin_identify(plugin);
		
		if (!strncmp(core->settings.layout_engine, id, strlen(core->settings.layout_engine))) {
			core->layout_plugin = entropy_plugin_layout_register(plugin);

			/*Initializing..*/
			entropy_plugin_init = dlsym(plugin->dl_ref, "entropy_plugin_init");
			(*entropy_plugin_init)(core);

			/*ID the global system toolkit*/
			plugin->toolkit = entropy_plugin_helper_toolkit_get(plugin);

			/*Set this as the default toolkit/plugin type*/
		} else {
			return 1;
		}
        } else if (type == ENTROPY_PLUGIN_GUI_COMPONENT) {
		/* TODO Get the subtype */

		gui_event_callback = dlsym(plugin->dl_ref, "gui_event_callback");
		plugin->gui_event_callback_p = gui_event_callback;
		plugin->toolkit = entropy_plugin_helper_toolkit_get(plugin);

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


char* entropy_layout_global_toolkit_get() {
	return core_core->layout_plugin->toolkit;
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

void entropy_plugin_thumbnailer_register(entropy_core* core, entropy_plugin* plugin, int type) {
        /*First off, query the plugin to see what MIME types it handles*/
        char* mime_type;
        Ecore_List* (*entropy_thumbnailer_plugin_mime_types_get)();
        Ecore_List* mime_types;

        //printf("Registering MIME types for thumbnailer..\n");

        entropy_thumbnailer_plugin_mime_types_get = dlsym(plugin->dl_ref, "entropy_thumbnailer_plugin_mime_types_get");
        mime_types = (*entropy_thumbnailer_plugin_mime_types_get)();
        ecore_list_goto_first(mime_types);
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
	strncpy(clone->path, file->path, 255);
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

	clone->thumbnail = file->thumbnail;
	clone->md5 = strdup(file->md5);
	
	return clone;
}


void entropy_core_selection_engine_init() {
	core_core->selected_files = ecore_list_new();
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
		
	while ( (file = ecore_list_remove_first(core_core->selected_files))) {
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

/*Register a component to get events created by others in the same layout container*/
void entropy_core_component_event_register(entropy_gui_component_instance* comp, char* event) {
	/*First we have to see if this layout is currently registered with the core..*/
	Ecore_Hash* event_hash = ecore_hash_get(core_core->layout_gui_events, comp->layout_parent);

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
	entropy_gui_component_instance* iter;
	Ecore_List* el;	
	Ecore_Hash* lay_hash;
	entropy_gui_component_instance* layout = NULL;

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
	lay_hash = ecore_hash_get(instance->core->layout_gui_events, layout);
	if (!lay_hash) {
		printf("Error: Attempted to raise event for unregistered layout container (%p)\n", layout);
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
		entropy_plugin* plugin;
		entropy_notify_event *ev;
		
		entropy_file_request* request = entropy_malloc(sizeof(entropy_file_request));

		/*A folder request's data is an entropy_generic_file*/
		
		//printf("File says: %s %s\n", ((entropy_generic_file*)event->data)->path, ((entropy_generic_file*)event->data)->filename);

		/*Check if we need to put a slash between the path/file*/

		if (((entropy_file_request*)event->data)->drill_down) {
			printf("Request for drill down\n");
		}

		request->file = ((entropy_file_request*)event->data)->file;
		request->requester = instance->layout_parent; /*Requester is the layout parent - after all - one dir per layout at one time*/
		request->core = instance->core;
		request->file_type = FILE_ALL;
		request->drill_down = ((entropy_file_request*)event->data)->drill_down;

		
		/*FIXME We should get the caller's current file plugin from the caller - i.e. the gui instance*/
	        plugin = entropy_plugins_type_get_first(ENTROPY_PLUGIN_BACKEND_FILE ,ENTROPY_PLUGIN_SUB_TYPE_ALL);
	       

		/*Make our new event for notification*/
		ev = entropy_notify_request_register(instance->core->notify, instance, ENTROPY_NOTIFY_FILELIST_REQUEST, 
					plugin, "filelist_get", request,  
					NULL);

		ecore_list_goto_first(el);
		while ( (iter = ecore_list_next(el)) ) {
			if (iter->active) entropy_notify_event_callback_add(ev, (void*)iter->plugin->gui_event_callback_p, iter);
		}

		/*Add the obj to be nuked once we've executed*/
		entropy_notify_event_cleanup_add(ev, request);
		
		/*Tell the notify engine we're ready to run*/
		entropy_notify_event_commit(instance->core->notify, ev);

		/*Nuke the file_request object that was passed to us*/
		free(event->data);

		
	} else if (!strcmp(event->event_type,ENTROPY_GUI_EVENT_FILE_CHANGE)) {
		entropy_notify_event* ev = entropy_notify_event_new();
		ev->event_type = ENTROPY_NOTIFY_FILE_CHANGE;
		
		//printf("Sending a file change event...\n");

		ecore_list_goto_first(el);
		while ( (iter = ecore_list_next(el)) ) {
			if (iter->active) (*iter->plugin->gui_event_callback_p)
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
			if (iter->active) (*iter->plugin->gui_event_callback_p)
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
			if (iter->active) (*iter->plugin->gui_event_callback_p)
				(ev, 
				 iter, 
				 event->data,   /*An entropy_generic_file*/
				 iter);
		}
		entropy_notify_event_destroy(ev);

	} else if (!strcmp(event->event_type,ENTROPY_GUI_EVENT_FILE_REMOVE_DIRECTORY)) {
		entropy_notify_event* ev = entropy_notify_event_new();
		ev->event_type = ENTROPY_NOTIFY_FILE_REMOVE_DIRECTORY;
		
		//printf("Sending a file create event...\n");

		ecore_list_goto_first(el);
		while ( (iter = ecore_list_next(el)) ) {
			if (iter->active) (*iter->plugin->gui_event_callback_p)
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
			
			if (iter->active) (*iter->plugin->gui_event_callback_p)
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
	        plugin = entropy_plugins_type_get_first(ENTROPY_PLUGIN_BACKEND_FILE ,ENTROPY_PLUGIN_SUB_TYPE_ALL);

		/*Make our new event for notification*/
		ev = entropy_notify_request_register(instance->core->notify, instance, ENTROPY_NOTIFY_FILE_STAT_EXECUTED, 
					plugin, "filestat_get",  request  ,  
					NULL);

		ecore_list_goto_first(el);
		while ( (iter = ecore_list_next(el)) ) {
			if (iter->active) entropy_notify_event_callback_add(ev, (void*)iter->plugin->gui_event_callback_p, iter);
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
			
			if (iter->active) (*iter->plugin->gui_event_callback_p)
				(ev, 
				 iter, 
				 event->data,   /*An entropy_file_stat*/
				 iter);
		}
		entropy_notify_event_destroy(ev);

	
	/*A thumbnail has been made available*/
	} else if (!strcmp(event->event_type,ENTROPY_GUI_EVENT_THUMBNAIL_AVAILABLE)) {
		entropy_notify_event* ev = entropy_notify_event_new();
		ev->event_type = ENTROPY_NOTIFY_THUMBNAIL_REQUEST; 

		
		/*Call the requestors*/
		ecore_list_goto_first(el);
		while ( (iter = ecore_list_next(el)) ) {
			//printf( "Calling callback at : %p\n", iter->plugin->gui_event_callback_p);
			
			if (iter->active) (*iter->plugin->gui_event_callback_p)
				(ev, 
				 iter, 
				 event->data,   /*An entropy_thumb*/
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
			
			if (iter->active) (*iter->plugin->gui_event_callback_p)
				(ev, 
				 iter, 
				 file_list,   /*An entropy_file_stat*/
				 iter);
		}
		entropy_notify_event_destroy(ev);
		
	} else if (!strcmp(event->event_type,ENTROPY_GUI_EVENT_FILE_PROGRESS)) {
		entropy_notify_event* ev = entropy_notify_event_new();
		ev->event_type = ENTROPY_NOTIFY_FILE_PROGRESS; 

		

		/*Call the requestors*/
		ecore_list_goto_first(el);
		while ( (iter = ecore_list_next(el)) ) {
			//printf( "Calling callback at : %p\n", iter->plugin->gui_event_callback_p);
			
			if (iter->active) (*iter->plugin->gui_event_callback_p)
				(ev, 
				 iter, 
				 event->data,   /*An evfs progress event*/
				 iter);
		}
		entropy_notify_event_destroy(ev);		
	} else if (!strcmp(event->event_type, ENTROPY_GUI_EVENT_USER_INTERACTION_YES_NO_ABORT)) {
		entropy_notify_event* ev = entropy_notify_event_new();
		ev->event_type = ENTROPY_NOTIFY_USER_INTERACTION_YES_NO_ABORT; 

		/*Call the requestors*/
		ecore_list_goto_first(el);
		while ( (iter = ecore_list_next(el)) ) {
			//printf( "Calling callback at : %p\n", iter->plugin->gui_event_callback_p);
			
			if (iter->active) (*iter->plugin->gui_event_callback_p)
				(ev, 
				 iter, 
				 event->data,   /*An evfs progress event*/
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


char* md5_entropy_path_file(char* plugin, char* folder, char* filename) {
        char* md5;
	char full_name[PATH_MAX];
        md5_state_t state;
        md5_byte_t digest[16];
        static const char hex[]="0123456789abcdef";
        int i;

	md5 = entropy_malloc(sizeof(char) * 33);

	/*printf("MD5'ing %s %s\n", path, filename);*/

	snprintf(full_name, 1024, "%s%s%s", plugin, folder, filename);
	
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


void entropy_core_file_cache_add(char* md5, entropy_file_listener* listener) {

	LOCK(&core_core->file_cache_mutex);
	
	if (!ecore_hash_get(core_core->file_interest_list, md5)) {
		ecore_hash_set(core_core->file_interest_list, md5, listener);
	} else {
		printf("*** BAD: Called set-reference with file already cached!\n");
		entropy_core_file_cache_add_reference(md5);
	}
	file_cache_size++;
	/*printf("File cache goes to %ld\n", file_cache_size);*/
	UNLOCK(&core_core->file_cache_mutex);
	
}

void entropy_core_file_cache_add_reference(char* md5) {
	LOCK(&core_core->file_cache_mutex);
	entropy_file_listener* listener = ecore_hash_get(core_core->file_interest_list, md5);

	if (listener) {
		listener->count++;
	}
	UNLOCK(&core_core->file_cache_mutex);
}

void entropy_core_file_cache_remove_reference(char* md5) {
	LOCK(&core_core->file_cache_mutex);

	entropy_file_listener* listener = ecore_hash_get(core_core->file_interest_list, md5);

	if (listener) {
		listener->count--;
		if (listener->count <= 0 && 0) {

			//printf("Freeing file '%s'\n", listener->file->filename);

			/*This should be a seperate function*/
			entropy_generic_file_destroy(listener->file);
			free(listener);

			ecore_hash_remove(core_core->file_interest_list, md5);

			file_cache_size--;

		} 
	}
	UNLOCK(&core_core->file_cache_mutex);
}

entropy_file_listener* entropy_core_file_cache_retrieve(char* md5) {
	entropy_file_listener* listen;
	
	LOCK(&core_core->file_cache_mutex);

	listen = ecore_hash_get(core_core->file_interest_list, md5);
	
	UNLOCK(&core_core->file_cache_mutex);

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
	entropy_generic_file* file = entropy_generic_file_new();
	evfs_file_uri_path* uri_path = evfs_parse_uri(uri);


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

	//printf("Made a file with plugin '%s', path '%s', filename '%s'\n", file->uri_base, file->path, file->filename);


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


char* entropy_core_generic_file_uri_create (entropy_generic_file* file, int drill_down) {
	entropy_generic_file* source_file;
	char* uri = malloc(PATH_MAX);
	char* uri_retrieve;
	char uri_build[255];
		
	/*If the file/location we are requesting has a 'parent' (i.e. it's inside another object),
	 * we have to grab the parent, not the file itself, as the base*/
	if (file->parent) {
		source_file = file->parent;
	} else
	source_file = file;


	/*Do we have login information*/
	if (!source_file->username) {
		snprintf(uri, 512, "%s://%s/%s",  source_file->uri_base, source_file->path, source_file->filename);
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


entropy_generic_file* evfs_filereference_to_entropy_generic_file(void* ref) 
{
	evfs_filereference* file_ref = (evfs_filereference*)ref;
	char *copy = strdup (file_ref->path);
	char *pos = strrchr (copy, '/');
	entropy_generic_file* file = entropy_generic_file_new();
	
	*pos = '\0';
	pos++;

	strncpy(file->path, copy, 255);
	strncpy(file->filename, pos, FILENAME_LENGTH);
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



