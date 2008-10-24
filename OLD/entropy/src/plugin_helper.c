#include "include/entropy.h"
#include <dlfcn.h>

static Ecore_List* plugin_list = NULL;

void entropy_thumbnailer_plugin_print(Ecore_Hash* mime_register) {
	char* mime_type;
	
	Ecore_List* keys = ecore_hash_keys(mime_register);
	ecore_list_first_goto(keys);
	while ( (mime_type = ecore_list_next(keys)) ) {
		printf ("We have a handler for '%s', it is: %s\n", mime_type, 
		((entropy_plugin*)ecore_hash_get(mime_register, mime_type))->filename );

	}
	ecore_list_destroy(keys);

}

char* entropy_plugin_helper_toolkit_get(entropy_plugin* plugin) {
	char* (*_entropy_plugin_toolkit_get)();	
	char* res;
	_entropy_plugin_toolkit_get = dlsym(plugin->dl_ref, "entropy_plugin_toolkit_get");

	if (_entropy_plugin_toolkit_get) {
		res = (*_entropy_plugin_toolkit_get)();
		return res;
	} else {
		return NULL;
	}
}



int entropy_core_plugin_type_get(entropy_plugin* plugin) {
	int (*entropy_plugin_type_get)();
	int res;

	entropy_plugin_type_get = dlsym(plugin->dl_ref, "entropy_plugin_type_get");
	res = (*entropy_plugin_type_get)();

	return res;
		
	
}

int entropy_core_plugin_sub_type_get(entropy_plugin* plugin) {
	int (*entropy_plugin_sub_type_get)();
	int res;

	entropy_plugin_sub_type_get = dlsym(plugin->dl_ref, "entropy_plugin_sub_type_get");
	if (entropy_plugin_sub_type_get) {
		res = (*entropy_plugin_sub_type_get)();
	} else {
		res = -1;
	}

	return res;
		
	
}

entropy_plugin* entropy_plugin_gui_get_by_name_toolkit(char* toolkit, char* name) {
        entropy_plugin* list_item;

	Ecore_List* plugins = entropy_core_get_core()->plugin_list;

        ecore_list_first_goto(plugins);
        while ( (list_item = ecore_list_next(plugins)) ) {
		/*printf("Scanning '%s' for first\n", list_item->filename);*/
                if (list_item->toolkit && list_item->name) {
			if (!strcmp(list_item->toolkit, toolkit) && !(strcmp(list_item->name, name))) {
				return list_item;
	                }
		}
        }

	return NULL;
}

entropy_plugin* entropy_plugins_type_get_first(int type, int subtype) {
        entropy_plugin* list_item;

	Ecore_List* plugins = entropy_core_get_core()->plugin_list;

        ecore_list_first_goto(plugins);
        while ( (list_item = ecore_list_next(plugins)) ) {
		/*printf("Scanning '%s' for first\n", list_item->filename);*/
                if (list_item->type == type && (subtype == ENTROPY_PLUGIN_SUB_TYPE_ALL || subtype == list_item->subtype)
		 && (type != ENTROPY_PLUGIN_GUI_COMPONENT || 
		(type == ENTROPY_PLUGIN_GUI_COMPONENT && 
		 !strcmp(list_item->toolkit, entropy_layout_global_toolkit_get())))) {
			return list_item;
                }
        }

	return NULL;

}


char* entropy_plugin_plugin_identify(entropy_plugin* plugin) {
	char* (*entropy_plugin_identify)(void);
	entropy_plugin_identify = dlsym(plugin->dl_ref, "entropy_plugin_identify");
	
	return (*entropy_plugin_identify)();
}


Ecore_List* entropy_plugins_type_get(int type, int subtype) {
	entropy_plugin* list_item;

	Ecore_List* plugins = entropy_core_get_core()->plugin_list;
	
	if (plugin_list) {
		ecore_list_destroy(plugin_list);
	}

	plugin_list = ecore_list_new();
	
	ecore_list_first_goto(plugins);
	while ( (list_item = ecore_list_next(plugins)) ) {
		/*printf("Scanning plugin: %s\n", list_item->filename);*/
		if (list_item->type == type && 
		   (subtype == ENTROPY_PLUGIN_SUB_TYPE_ALL || subtype == list_item->subtype)
		   && (type != ENTROPY_PLUGIN_GUI_COMPONENT || 
		(type == ENTROPY_PLUGIN_GUI_COMPONENT && 
		 !strcmp(list_item->toolkit, entropy_layout_global_toolkit_get())))) {
			ecore_list_append(plugin_list, list_item);
		}
	}
	
	return plugin_list;	
}

Ecore_List* entropy_plugin_filesystem_filelist_get(entropy_file_request* request)
{
	Entropy_Plugin_File* plugin;
	Ecore_List* ret;

	/*FIXME We should get the caller's current file plugin from the caller - i.e. the gui instance*/
        plugin = ENTROPY_PLUGIN_FILE(entropy_plugins_type_get_first(ENTROPY_PLUGIN_BACKEND_FILE ,ENTROPY_PLUGIN_SUB_TYPE_ALL));
	ret = (*plugin->file_functions.filelist_get)(request);

	return ret;
		
}

void entropy_plugin_filesystem_auth_respond(char* loc, char* user, char* password) 
{
	Entropy_Plugin_File* plugin;
	
        plugin = ENTROPY_PLUGIN_FILE(entropy_plugins_type_get_first(ENTROPY_PLUGIN_BACKEND_FILE ,ENTROPY_PLUGIN_SUB_TYPE_ALL));
	(*plugin->file_functions.auth_respond)(loc,user,password);	
}

void entropy_plugin_filesystem_filestat_get(entropy_file_request* request)
{
	Entropy_Plugin_File* plugin;

	/*FIXME We should get the caller's current file plugin from the caller - i.e. the gui instance*/
        plugin = ENTROPY_PLUGIN_FILE(entropy_plugins_type_get_first(ENTROPY_PLUGIN_BACKEND_FILE ,ENTROPY_PLUGIN_SUB_TYPE_ALL));

	(*plugin->file_functions.filestat_get)(request);
}

void entropy_plugin_thumbnail_request(entropy_gui_component_instance* requestor, entropy_generic_file* file, 
		void (*cb)())
{
	if (file->mime_type) {
		entropy_plugin* thumb = entropy_thumbnailer_retrieve (file->mime_type);
	
		if (thumb) {
			entropy_thumbnail_request *request = entropy_thumbnail_request_new ();
			request->file = file;
			request->instance = requestor;

			/*Add a reference to this file, so it doesn't get cleaned up*/
			entropy_core_file_cache_add_reference (file->md5);

			entropy_notify_event *ev =
			  entropy_notify_request_register (requestor,
				   ENTROPY_NOTIFY_THUMBNAIL_REQUEST,
				   thumb,
				   "entropy_thumbnailer_thumbnail_get",
				   request, NULL);

			entropy_notify_event_callback_add (ev, cb, requestor);
			entropy_notify_event_cleanup_add (ev, request);
	
			entropy_notify_event_commit (ev);
		}

	} else {
		printf("Thumb request on mime-less file\n");
	}
}

void entropy_plugin_filesystem_file_remove(entropy_generic_file* file, entropy_gui_component_instance* instance) 
{
	Entropy_Plugin_File* fileplugin;
  	entropy_plugin *plugin =
	      entropy_plugins_type_get_first (
	ENTROPY_PLUGIN_BACKEND_FILE, ENTROPY_PLUGIN_SUB_TYPE_ALL);

	fileplugin = ENTROPY_PLUGIN_FILE(plugin);
	(*fileplugin->file_functions.file_remove)(file, instance);
}

void entropy_plugin_filesystem_directory_create(entropy_generic_file* file, const char* dir) 
{
	Entropy_Plugin_File* fileplugin;
        entropy_plugin *plugin =
              entropy_plugins_type_get_first (
        ENTROPY_PLUGIN_BACKEND_FILE, ENTROPY_PLUGIN_SUB_TYPE_ALL);

	fileplugin = ENTROPY_PLUGIN_FILE(plugin);
        (*fileplugin->file_functions.directory_create)(file, dir);
}



int entropy_plugin_filesystem_file_copy(entropy_generic_file* source, char* dest, entropy_gui_component_instance* requester) 
{
  entropy_gui_component_instance *instance = requester;
  Entropy_Plugin_File* plugin =
    ENTROPY_PLUGIN_FILE(entropy_plugins_type_get_first (ENTROPY_PLUGIN_BACKEND_FILE,
				    ENTROPY_PLUGIN_SUB_TYPE_ALL));

  if (plugin) {
	  (*plugin->file_functions.file_copy) (source, dest,
		  instance);
  }

  return 1;
}

int entropy_plugin_filesystem_file_copy_multi(Ecore_List* files, char* dest, entropy_gui_component_instance* requester) 
{
  entropy_gui_component_instance *instance = requester;
  Entropy_Plugin_File* plugin =
    ENTROPY_PLUGIN_FILE(entropy_plugins_type_get_first (ENTROPY_PLUGIN_BACKEND_FILE,
				    ENTROPY_PLUGIN_SUB_TYPE_ALL));

  if (plugin) {
	  (*plugin->file_functions.file_copy_multi) (files, dest, instance);
  }

  return 1;
}

int entropy_plugin_filesystem_file_move_multi(Ecore_List* files, char* dest, entropy_gui_component_instance* requester) 
{
  entropy_gui_component_instance *instance = requester;
  Entropy_Plugin_File* plugin =
    ENTROPY_PLUGIN_FILE(entropy_plugins_type_get_first (ENTROPY_PLUGIN_BACKEND_FILE,
				    ENTROPY_PLUGIN_SUB_TYPE_ALL));

  if (plugin) {
	  (*plugin->file_functions.file_move_multi) (files, dest, instance);
  }

  return 1;
}

int entropy_plugin_filesystem_file_trash_restore(Ecore_List* files, entropy_gui_component_instance* requester) 
{
  entropy_gui_component_instance *instance = requester;
  Entropy_Plugin_File* plugin =
    ENTROPY_PLUGIN_FILE(entropy_plugins_type_get_first (ENTROPY_PLUGIN_BACKEND_FILE,
				    ENTROPY_PLUGIN_SUB_TYPE_ALL));

  if (plugin) {
	  (*plugin->file_functions.file_trash_restore) (files, instance);
  }

  return 1;
}


int entropy_plugin_filesystem_file_move(entropy_generic_file* source, char* dest, entropy_gui_component_instance* requester) 
{
  entropy_gui_component_instance *instance = requester;
  Entropy_Plugin_File* plugin =
    ENTROPY_PLUGIN_FILE(entropy_plugins_type_get_first (ENTROPY_PLUGIN_BACKEND_FILE,
				    ENTROPY_PLUGIN_SUB_TYPE_ALL));

  if (plugin) {
	  (*plugin->file_functions.file_move) (source, dest,
		  instance);
  }

  return 1;
}

void entropy_plugin_operation_respond(long operation, int response)
{
  Entropy_Plugin_File* plugin =
    ENTROPY_PLUGIN_FILE(entropy_plugins_type_get_first (ENTROPY_PLUGIN_BACKEND_FILE,
				    ENTROPY_PLUGIN_SUB_TYPE_ALL));

  if (plugin) {
  	(*plugin->file_functions.operation_respond) (operation, response);
  }
}

void entropy_plugin_filesystem_file_rename(entropy_generic_file* file, char* dest)
{
    Entropy_Plugin_File* plugin =
    ENTROPY_PLUGIN_FILE(entropy_plugins_type_get_first (ENTROPY_PLUGIN_BACKEND_FILE,
				    ENTROPY_PLUGIN_SUB_TYPE_ALL));

  if (plugin) {
        entropy_generic_file* clone = entropy_generic_file_clone(file);
	strncpy(clone->filename, dest, strlen(dest)+1);
	  
  	(*plugin->file_functions.file_rename) (file,clone);
	entropy_generic_file_destroy(clone);
  }

}

void entropy_plugin_filesystem_metadata_groups_get(entropy_gui_component_instance* instance) 
{
    Entropy_Plugin_File* plugin =
    ENTROPY_PLUGIN_FILE(entropy_plugins_type_get_first (ENTROPY_PLUGIN_BACKEND_FILE,
				    ENTROPY_PLUGIN_SUB_TYPE_ALL));

  if (plugin) {
  	(*plugin->misc_functions.groups_get) (instance);
   }
}

Ecore_List* entropy_plugin_filesystem_metadata_groups_retrieve() 
{
    Entropy_Plugin_File* plugin =
    ENTROPY_PLUGIN_FILE(entropy_plugins_type_get_first (ENTROPY_PLUGIN_BACKEND_FILE,
				    ENTROPY_PLUGIN_SUB_TYPE_ALL));

  if (plugin) {
  	return (*plugin->misc_functions.groups_retrieve) ();
   } else return NULL;
}

void entropy_plugin_filesystem_metadata_all_get(entropy_gui_component_instance* instance) 
{
    Entropy_Plugin_File* plugin =
    ENTROPY_PLUGIN_FILE(entropy_plugins_type_get_first (ENTROPY_PLUGIN_BACKEND_FILE,
				    ENTROPY_PLUGIN_SUB_TYPE_ALL));

  if (plugin) {
  	return (*plugin->misc_functions.meta_all_get)(instance);
   }
}

void entropy_plugin_filesystem_vfolder_create(char* name, Eina_List* entries) 
{
    Entropy_Plugin_File* plugin =
    ENTROPY_PLUGIN_FILE(entropy_plugins_type_get_first (ENTROPY_PLUGIN_BACKEND_FILE,
				    ENTROPY_PLUGIN_SUB_TYPE_ALL));

  if (plugin) {
  	return (*plugin->misc_functions.vfolder_create)(name,entries);
   }
}


void entropy_plugin_filesystem_file_group_add(entropy_generic_file* file, char* group)
{
    Entropy_Plugin_File* plugin =
    ENTROPY_PLUGIN_FILE(entropy_plugins_type_get_first (ENTROPY_PLUGIN_BACKEND_FILE,
				    ENTROPY_PLUGIN_SUB_TYPE_ALL));

  if (plugin) {
  	(*plugin->file_functions.group_file_add) (file,group);
   }

}

void entropy_plugin_filesystem_file_group_remove(entropy_generic_file* file, char* group)
{
    Entropy_Plugin_File* plugin =
    ENTROPY_PLUGIN_FILE(entropy_plugins_type_get_first (ENTROPY_PLUGIN_BACKEND_FILE,
				    ENTROPY_PLUGIN_SUB_TYPE_ALL));

  if (plugin) {
  	(*plugin->file_functions.group_file_remove) (file,group);
   }

}

void entropy_event_stat_request(Entropy_Generic_File* file, entropy_gui_component_instance* instance)
{
    entropy_gui_event *gui_event;
	
    gui_event = entropy_malloc (sizeof (entropy_gui_event));
    gui_event->event_type =
      entropy_core_gui_event_get (ENTROPY_GUI_EVENT_FILE_STAT);
    gui_event->data = file;
    entropy_core_layout_notify_event (instance, gui_event,
				      ENTROPY_EVENT_LOCAL);
    
}

void entropy_event_extended_stat_expect(Entropy_Generic_File* file, entropy_gui_component_instance* instance)
{
    entropy_gui_event *gui_event;
	
    gui_event = entropy_malloc (sizeof (entropy_gui_event));
    gui_event->event_type =
      entropy_core_gui_event_get (ENTROPY_GUI_EVENT_EXTENDED_STAT);
    gui_event->data = file;
    entropy_core_layout_notify_event (instance, gui_event,
				      ENTROPY_EVENT_LOCAL);
    
}

void entropy_event_action_file(Entropy_Generic_File* file, entropy_gui_component_instance* instance) 
{
    entropy_gui_event *gui_event;

	
    gui_event = entropy_malloc (sizeof (entropy_gui_event));
    gui_event->event_type =
	entropy_core_gui_event_get (ENTROPY_GUI_EVENT_ACTION_FILE);
    gui_event->data = file;
    entropy_core_layout_notify_event (instance, gui_event, ENTROPY_EVENT_GLOBAL); 
}

void entropy_event_copy_request(entropy_gui_component_instance* instance) 
{
    entropy_gui_event *gui_event;

	
    gui_event = entropy_malloc (sizeof (entropy_gui_event));
    gui_event->event_type =
	entropy_core_gui_event_get (ENTROPY_GUI_EVENT_COPY_REQUEST);
    gui_event->data = NULL;
    entropy_core_layout_notify_event (instance, gui_event, ENTROPY_EVENT_LOCAL); 
}

void entropy_event_cut_request(entropy_gui_component_instance* instance) 
{
    entropy_gui_event *gui_event;

	
    gui_event = entropy_malloc (sizeof (entropy_gui_event));
    gui_event->event_type =
	entropy_core_gui_event_get (ENTROPY_GUI_EVENT_CUT_REQUEST);
    gui_event->data = NULL;
    entropy_core_layout_notify_event (instance, gui_event, ENTROPY_EVENT_LOCAL); 
}

void entropy_event_paste_request(entropy_gui_component_instance* instance) 
{
    entropy_gui_event *gui_event;

	
    gui_event = entropy_malloc (sizeof (entropy_gui_event));
    gui_event->event_type =
	entropy_core_gui_event_get (ENTROPY_GUI_EVENT_PASTE_REQUEST);
    gui_event->data = NULL;
    entropy_core_layout_notify_event (instance, gui_event, ENTROPY_EVENT_LOCAL); 
}

void entropy_event_hover_request(entropy_gui_component_instance* instance, entropy_generic_file* file, int x, int y) 
{
    entropy_gui_event *gui_event;

	
    gui_event = entropy_malloc (sizeof (entropy_gui_event));
    gui_event->event_type =
	entropy_core_gui_event_get (ENTROPY_GUI_EVENT_HOVER);
    gui_event->data = file;
    entropy_core_file_cache_add_reference (file->md5);

    /*FIXME: BAD reuse of fields - should gui/notify events have a hash of key/values?*/
    gui_event->key = x;
    gui_event->hints = y;
    entropy_core_layout_notify_event (instance, gui_event, ENTROPY_EVENT_LOCAL); 
}

void entropy_event_dehover_request(entropy_gui_component_instance* instance, entropy_generic_file* file) 
{
    entropy_gui_event *gui_event;

	
    gui_event = entropy_malloc (sizeof (entropy_gui_event));
    gui_event->event_type =
	entropy_core_gui_event_get (ENTROPY_GUI_EVENT_DEHOVER);
    gui_event->data = file;
    entropy_core_file_cache_add_reference (file->md5);

    printf("Sending dehover..\n");
    entropy_core_layout_notify_event (instance, gui_event, ENTROPY_EVENT_LOCAL); 
}

void entropy_event_auth_request(entropy_gui_component_instance* instance,char* location) 
{
    entropy_gui_event *gui_event;

    gui_event = entropy_malloc (sizeof (entropy_gui_event));
    gui_event->event_type =
	entropy_core_gui_event_get (ENTROPY_GUI_EVENT_AUTH_REQUEST);
    gui_event->data = strdup(location);

    entropy_core_layout_notify_event (instance, gui_event, ENTROPY_EVENT_LOCAL); 
}



entropy_generic_file* entropy_layout_current_folder_get(entropy_gui_component_instance_layout* layout)
{
	return layout->current_folder;
}

