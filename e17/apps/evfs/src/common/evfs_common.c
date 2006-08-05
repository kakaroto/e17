#include "evfs.h"

evfs_plugin*
evfs_meta_plugin_get_for_type(evfs_server* server, char* mime)
{
	return ecore_hash_get(server->plugin_meta_hash, mime);
}

evfs_plugin *
evfs_get_plugin_for_uri(evfs_server * server, char *uri_base)
{
   return ecore_hash_get(server->plugin_uri_hash, uri_base);
}

/*Make sure as file is all good*/
int evfs_filereference_sanitise(evfs_filereference* ref)
{
	if (!ref->plugin) {
		ref->plugin = evfs_get_plugin_for_uri(evfs_server_get(), ref->plugin_uri);
	}
}

evfs_filereference* evfs_filereference_clone(evfs_filereference* source)
{
	evfs_filereference* dest = calloc(1,sizeof(evfs_filereference));

	dest->plugin_uri = strdup(source->plugin_uri);
	dest->plugin = source->plugin;

	/*TODO - handle nested files*/

	dest->file_type = source->file_type;
	dest->path = strdup(source->path);

	if (source->username) dest->username = strdup(source->username);
	if (source->password) dest->password = strdup(source->password);

	/*FIXME - do we assume this file is closed or open?*/
	dest->fd = 0;
	dest->fd_p = NULL;

	return dest;
	
}

