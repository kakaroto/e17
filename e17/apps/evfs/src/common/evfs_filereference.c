#include "evfs.h"

EvfsFilereference* evfs_filereference_new(char* plugin, char* path, int filetype)
{
	EvfsFilereference* ref = NEW(EvfsFilereference);
	ref->path = strdup(path);
	ref->plugin_uri = strdup(plugin);
	ref->file_type = filetype;
	
	if (!evfs_object_client_is_get()) 
		ref->plugin = evfs_get_plugin_for_uri(evfs_server_get(), plugin);

	return ref;
}

/*Helper function for local files*/
EvfsFilereference* evfs_filereference_local_new(char* path)
{
	EvfsFilereference* ref = evfs_filereference_new("file", path, 0);
	return ref;
}
