#ifndef __ENTROPY_THUMBNAILER_ENGINE_H_
#define __ENTROPY_THUMBNAILER_ENGINE_H_


#include "entropy.h"

entropy_plugin* entropy_thumbnailer_retrieve(char* mime_type);
Ecore_List* entropy_thumbnailer_child_retrieve(char* mime_type);

typedef struct entropy_thumbnail_request {
	entropy_generic_file* file;
	struct entropy_gui_component_instance* instance;
} entropy_thumbnail_request;

entropy_thumbnail_request* entropy_thumbnail_request_new();
void entropy_thumbnail_request_destroy(entropy_thumbnail_request* request);

#endif
