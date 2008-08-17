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
int EvfsFilereference_sanitise(EvfsFilereference* ref)
{
	if (!ref->plugin) {
		ref->plugin = evfs_get_plugin_for_uri(evfs_server_get(), ref->plugin_uri);
	}
	return 1;
}

evfs_command* evfs_file_command_single_build(EvfsFilereference* ref)
{
	evfs_command* c = NEW(evfs_command);
	EvfsFilereference_sanitise(ref);
	c->file_command = NEW(evfs_command_file);
	c->file_command->files = evas_list_append(c->file_command->files,ref);

	return c;
}

void evfs_file_command_file_add(evfs_command* command, EvfsFilereference* ref)
{
	command->file_command->files = evas_list_append(command->file_command->files, ref);
}

EvfsFilereference* EvfsFilereference_clone(EvfsFilereference* source)
{
	EvfsFilereference* dest = calloc(1,sizeof(EvfsFilereference));

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

