#ifndef __EVFS_COMMON_H_
#define __EVFS_COMMON_H_

evfs_plugin *evfs_get_plugin_for_uri(evfs_server * server, char *uri_base);
evfs_filereference* evfs_filereference_clone(evfs_filereference* source);
int evfs_filereference_sanitise(evfs_filereference* ref);
evfs_plugin* evfs_meta_plugin_get_for_type(evfs_server* server, char* mime);
	

#endif
