#include "evfs.h"

evfs_plugin *
evfs_get_plugin_for_uri(evfs_server * server, char *uri_base)
{
   return ecore_hash_get(server->plugin_uri_hash, uri_base);
}
