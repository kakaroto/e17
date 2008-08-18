#ifndef __EVFS_COMMON_H_
#define __EVFS_COMMON_H_

evfs_plugin *evfs_get_plugin_for_uri(evfs_server * server, char *uri_base);
EvfsFilereference* EvfsFilereference_clone(EvfsFilereference* source);
int EvfsFilereference_sanitise(EvfsFilereference* ref);
evfs_plugin* evfs_meta_plugin_get_for_type(evfs_server* server, char* mime);
evfs_command* evfs_file_command_single_build(EvfsFilereference* ref);
void evfs_file_command_file_add(evfs_command* command, EvfsFilereference* ref);
	

#endif
