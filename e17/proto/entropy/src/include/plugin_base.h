#ifndef __PLUGIN_BASE_H_
#define __PLUGIN_BASE_H_

#include <limits.h>

enum ENTROPY_PLUGIN_TYPES {
	ENTROPY_PLUGIN_BACKEND_FILE ,
	ENTROPY_PLUGIN_MIME,
	ENTROPY_PLUGIN_THUMBNAILER,
	ENTROPY_PLUGIN_THUMBNAILER_DISTRIBUTION,
	ENTROPY_PLUGIN_GUI_COMPONENT,
	ENTROPY_PLUGIN_GUI_LAYOUT,
	ENTROPY_PLUGIN_ACTION_PROVIDER,
	ENTROPY_PLUGIN_METADATA_READ,
};

enum ENTROPY_PLUGIN_SUB_TYPES {
	ENTROPY_PLUGIN_SUB_TYPE_ALL,
        ENTROPY_PLUGIN_GUI_COMPONENT_STRUCTURE_VIEW,
        ENTROPY_PLUGIN_GUI_COMPONENT_LOCAL_VIEW,
	ENTROPY_PLUGIN_GUI_COMPONENT_INFO_PROVIDER

};

enum ENTROPY_MIME_PLUGIN_PRIORITY_TYPES {
	ENTROPY_MIME_PLUGIN_PRIORITY_HIGH
};

typedef struct entropy_plugin entropy_plugin;
struct entropy_plugin {
	int type;
	int subtype;
	char filename[PATH_MAX];
	void* dl_ref;
	void (*gui_event_callback_p)();

	void* data; 
	char* toolkit;
};

typedef struct entropy_mime_object entropy_mime_object;
struct entropy_mime_object {
	char mime_type[100];
	entropy_plugin* plugin;
	
};

#endif
