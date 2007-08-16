#include "evfs.h"

EvfsFilereference* evfs_filereference_new(char* plugin, char* path, int filetype)
{
	EvfsFilereference* ref = NEW(EvfsFilereference);
	ref->path = strdup(path);
	ref->plugin_uri = strdup(plugin);
	ref->file_type = filetype;
	ref->plugin = evfs_get_plugin_for_uri(evfs_server_get(), plugin);

	return ref;
}
