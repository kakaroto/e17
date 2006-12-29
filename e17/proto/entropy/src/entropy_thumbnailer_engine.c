#include "include/entropy.h"

entropy_plugin* entropy_thumbnailer_retrieve(char* mime_type) {
	return (entropy_plugin*)ecore_hash_get(entropy_core_get_core()->entropy_thumbnailers, mime_type);
}

Ecore_List* entropy_thumbnailer_child_retrieve(char* mime_type) {
	return (Ecore_List*)ecore_hash_get(entropy_core_get_core()->entropy_thumbnailers_child, mime_type);
}

entropy_thumbnail_request* entropy_thumbnail_request_new() {
	return entropy_malloc(sizeof(entropy_thumbnail_request));
}

void entropy_thumbnail_request_destroy(entropy_thumbnail_request* request) {
	free(request);
}
