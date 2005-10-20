#include "entropy.h"

static Ecore_List* types= NULL;

int entropy_plugin_type_get() {
        return ENTROPY_PLUGIN_THUMBNAILER;
}

char* entropy_plugin_identify() {
	        return (char*)"Simple MIME plugin for images";
}

Ecore_List* entropy_thumbnailer_plugin_mime_types_get() {
	if (types == NULL) {
		//printf("Making new ecore_list..\n");
		types = ecore_list_new();
		ecore_list_append(types, "image/jpeg");
		ecore_list_append(types, "image/png");
		ecore_list_append(types, "image/gif");
		
	}

	return types;
}

entropy_thumbnail* entropy_thumbnailer_thumbnail_get(entropy_generic_file* file) {
	entropy_thumbnail* thumb;
	if (!file->thumbnail) {
		thumb = entropy_thumbnail_create(file);
	} else {
		return file->thumbnail;
	}
	/*Set the file up for this thumbnail. TODO this probably violates convention to do this here,
	 * but we create the thumbnail downstream, and from here, so there's not much choice.. */

	if (thumb) {
		/*printf("Created thumbnail '%s'\n", thumb->thumbnail_filename);*/
		thumb->parent = file;
		file->thumbnail = thumb;
	} else {
		//printf ("Returned thumb was null, assuming error...\n");
		return NULL;
	}

	return thumb;
}


