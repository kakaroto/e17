#ifndef __EVFS_PLUGIN_H_
#define __EVFS_PLUGIN_H_

typedef struct evfs_plugin_functions evfs_plugin_functions;
struct evfs_plugin_functions {
	int (*evfs_file_remove)(char* path);
	int (*evfs_monitor_start)(evfs_client* client, evfs_command* command);
	int (*evfs_monitor_stop)(evfs_client* client, evfs_command* command);
	int (*evfs_file_rename)(evfs_client* client, evfs_command* command);
	void (*evfs_file_stat)(evfs_client* client, evfs_command* command);
	void (*evfs_dir_list)(evfs_client* client, evfs_command* command);
};

typedef struct evfs_plugin evfs_plugin;
struct evfs_plugin {
	evfs_plugin_functions* functions;
	
	void* dl_ref;
	char* uri;

};


#endif
