#include "evfs.h"

static evfs_server *server;

evfs_server* 
evfs_server_get()
{
	return server;
}

evfs_server* evfs_server_new()
{
	server = NEW(evfs_server);
	return server;
}
