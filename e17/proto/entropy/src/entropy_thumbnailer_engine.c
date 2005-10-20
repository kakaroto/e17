#include "include/entropy.h"

entropy_plugin* entropy_thumbnailer_retrieve(Ecore_Hash* thumbnail_plugins, char* mime_type) {
	return (entropy_plugin*)ecore_hash_get(thumbnail_plugins, mime_type);
}
