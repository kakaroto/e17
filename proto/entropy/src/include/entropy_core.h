#ifndef __ENTROPY_CORE_H_
#define __ENTROPY_CORE_H_

#include <Ecore.h>
#include "entropy.h"
#include "notification_engine.h"

typedef enum {
	ENTROPY_SELECTION_COPY,
	ENTROPY_SELECTION_CUT
} Entropy_Selection_Type;

typedef struct _entropy_core_settings entropy_core_settings;
struct _entropy_core_settings {
	char* layout_engine;
};


typedef struct entropy_core entropy_core;
struct entropy_core {
	struct Entropy_Config* config;

	Ecore_List* plugin_list;
	Ecore_List* mime_plugins;
	Ecore_Hash* entropy_thumbnailers;
	Ecore_Hash* entropy_thumbnailers_child;
	Ecore_Hash* layout_gui_events;
	Ecore_Timer* notify_executer;
	
	Ecore_Hash* file_interest_list; /*A file cache of all files we have loaded*/
	Ecore_Hash* uri_reference_list;

	Ecore_Hash* gui_event_handlers;
	
	Ecore_Ipc_Server* server;

	void* layout_global; /*The global layout context*/
	entropy_notification_engine* notify;
	
	struct entropy_plugin* layout_plugin; /* The main layout plugin that we are relying on */

	Ecore_List* selected_files;
	Entropy_Selection_Type selection_type;
	
	Ecore_Hash* descent_hash;
	Ecore_Hash* object_associate_hash;
	Ecore_Hash* mime_action_hint;

	char* user_home_dir;
	char* thumbnail_path;
	
	entropy_core_settings settings;
};

entropy_core* entropy_core_new();
entropy_core* entropy_core_init(int,char**);
void entropy_core_destroy(entropy_core* core);


#endif
