#include "include/entropy.h"
#include <dlfcn.h>

static Ecore_List* plugin_list = NULL;

void entropy_thumbnailer_plugin_print(Ecore_Hash* mime_register) {
	char* mime_type;
	
	Ecore_List* keys = ecore_hash_keys(mime_register);
	ecore_list_goto_first(keys);
	while ( (mime_type = ecore_list_next(keys)) ) {
		printf ("We have a handler for '%s', it is: %s\n", mime_type, ((entropy_plugin*)ecore_hash_get(mime_register, mime_type))->filename );

	}
	ecore_list_destroy(keys);

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
                if (list_item->type == type && (subtype == ENTROPY_PLUGIN_SUB_TYPE_ALL || subtype == list_item->subtype)) {
			return list_item;
                }
        }

	return NULL;

}

Ecore_List* entropy_plugins_type_get(int type) {
	entropy_plugin* list_item;

	Ecore_List* plugins = entropy_core_get_core()->plugin_list;
	
	if (plugin_list) {
		ecore_list_destroy(plugin_list);
	}

	plugin_list = ecore_list_new();
	
	ecore_list_goto_first(plugins);
	while ( (list_item = ecore_list_next(plugins)) ) {
		/*printf("Scanning plugin: %s\n", list_item->filename);*/
		if (list_item->type == type) {
			ecore_list_append(plugin_list, list_item);
		}
	}
	
	return plugin_list;	
}
