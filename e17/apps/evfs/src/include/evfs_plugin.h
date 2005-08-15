#ifndef __EVFS_PLUGIN_H_
#define __EVFS_PLUGIN_H_

typedef struct evfs_plugin evfs_plugin;
struct evfs_plugin {
	void* dl_ref;
	char* uri;
};


#endif
