#include "include/entropy.h"
#include <dlfcn.h>

char* entropy_mime_file_identify(entropy_generic_file* file) {	
	entropy_plugin* plugin;
	char* (*entropy_mime_plugin_identify_file)(char* path, char* filename);
	char* type = NULL;

	/*IF we already have a mime type, return what we have*/
	if (strlen(file->mime_type) > 0) return file->mime_type;
	
	/*printf("Identifying a file..\n");	*/
	ecore_list_first_goto(entropy_core_get_core()->mime_plugins);
	while ((plugin = ecore_list_next(entropy_core_get_core()->mime_plugins)) != NULL) {
		/*printf ("Querying '%s' for MIME\n", plugin->filename);*/
		entropy_mime_plugin_identify_file = dlsym(plugin->dl_ref, "entropy_mime_plugin_identify_file");
		type = (*entropy_mime_plugin_identify_file)(file->path, file->filename);
		if (type) goto found_mime;
	}
	
	goto null_mime;
	
	found_mime:
	strcpy(file->mime_type, type);
	/*printf ("Found a mime type: %s\n", type);*/
	return type;

	null_mime:
	return "object/unidentified";
	
}
