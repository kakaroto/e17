#include "include/entropy.h"
#include <dlfcn.h>

static Ecore_List* plugin_list = NULL;

void entropy_thumbnailer_plugin_print(Ecore_Hash* mime_register) {
	char* mime_type;
	
	Ecore_List* keys = ecore_hash_keys(mime_register);
	ecore_list_goto_first(keys);
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

entropy_plugin* entropy_plugins_type_get_first(int type, int subtype) {
        entropy_plugin* list_item;

	Ecore_List* plugins = entropy_core_get_core()->plugin_list;

        ecore_list_goto_first(plugins);
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
	
	ecore_list_goto_first(plugins);
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

void entropy_plugin_filesystem_file_remove(entropy_generic_file* file, entropy_gui_component_instance* instance) {
  	entropy_plugin *plugin =
	      entropy_plugins_type_get_first (
	ENTROPY_PLUGIN_BACKEND_FILE, ENTROPY_PLUGIN_SUB_TYPE_ALL);
	
	void (*del_func)(entropy_generic_file* source, entropy_gui_component_instance*);
	del_func = dlsym(plugin->dl_ref, "entropy_filesystem_file_remove");

	(*del_func)(file, instance);
}

void entropy_plugin_filesystem_directory_create(entropy_generic_file* file, char* dir) {
        entropy_plugin *plugin =
              entropy_plugins_type_get_first (
        ENTROPY_PLUGIN_BACKEND_FILE, ENTROPY_PLUGIN_SUB_TYPE_ALL);

        void (*dir_func)(entropy_generic_file*, char*);
        dir_func = dlsym(plugin->dl_ref, "entropy_filesystem_directory_create");

        (*dir_func)(file, dir);
}



int entropy_plugin_filesystem_file_copy(entropy_generic_file* source, char* dest, entropy_gui_component_instance* requester) {
  entropy_gui_component_instance *instance = requester;
  entropy_plugin *plugin =
    entropy_plugins_type_get_first (ENTROPY_PLUGIN_BACKEND_FILE,
				    ENTROPY_PLUGIN_SUB_TYPE_ALL);

  void (*copy_func) (entropy_generic_file * source, char *dest_uri,
		     entropy_gui_component_instance * requester);


  /*Get the func ref */
  copy_func = dlsym (plugin->dl_ref, "entropy_filesystem_file_copy");

  (*copy_func) (source, dest,
		  instance);

  return 1;
}
