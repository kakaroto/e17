#ifndef __PLUGIN_BASE_H_
#define __PLUGIN_BASE_H_

#include <limits.h>
#include <Ecore_Data.h>
#include <Evas.h>
#include <sys/types.h>
#include "entropy.h"

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
	ENTROPY_PLUGIN_GUI_COMPONENT_INFO_PROVIDER,
	ENTROPY_PLUGIN_GUI_COMPONENT_HOVER_PROVIDER

};

enum ENTROPY_MIME_PLUGIN_PRIORITY_TYPES {
	ENTROPY_MIME_PLUGIN_PRIORITY_HIGH
};

struct entropy_plugin {
	int type;
	int subtype;
	char filename[PATH_MAX];
	void* dl_ref;
	void (*gui_event_callback_p)();

	void* data; 
	char* toolkit;
	char* name;

	struct {
		Entropy_Plugin* (*entropy_plugin_init)(entropy_core*);
	} functions;
};
#define ENTROPY_PLUGIN(plugin) ((entropy_plugin *) plugin)

struct Entropy_Plugin_Gui {
	Entropy_Plugin base;
	char* toolkit;	

	struct {
		entropy_gui_component_instance* (*layout_create)(entropy_core * core);	
		char* (*toolkit_get)();
		void (*layout_main)();
	
	} gui_functions;
};

struct Entropy_Plugin_File {
	entropy_plugin base;

	struct {
		Ecore_List* (*structurelist_get)(char*);
		struct stat* (*filestat_get)(entropy_file_request*);
		Ecore_List* (*filelist_get) (entropy_file_request*);
		void (*file_copy)(entropy_generic_file*,char*, entropy_gui_component_instance*);
		void (*file_copy_multi)(Ecore_List* files,char*, entropy_gui_component_instance*);
		void (*file_move_multi)(Ecore_List* files,char*, entropy_gui_component_instance*);
		void (*file_move)(entropy_generic_file*,char*, entropy_gui_component_instance*);

		void (*file_rename) (entropy_generic_file * file_from, entropy_generic_file * file_to);
		void (*operation_respond)(long id, int response);
		void (*directory_create) (entropy_generic_file * parent, const char* child_name);
		void (*file_remove) (entropy_generic_file * file, entropy_gui_component_instance* instance);
		void (*group_file_add) (entropy_generic_file * file,char*);
		void (*group_file_remove) (entropy_generic_file * file,char*);

		void (*file_trash_restore) (Ecore_List* files, entropy_gui_component_instance* instance);
		void (*auth_respond) (char*,char*,char*);
	} file_functions;

	struct {
		void (*groups_get)(entropy_gui_component_instance* instance);
		 Ecore_List* (*groups_retrieve)();
		void (*meta_all_get)(entropy_gui_component_instance* instance);
		void (*vfolder_create)(char* name,Eina_List* entries);
	} misc_functions;
};
#define ENTROPY_PLUGIN_FILE(plugin) ((Entropy_Plugin_File *) plugin)

struct Entropy_Plugin_Thumbnailer {
	Entropy_Plugin base;
};

struct Entropy_Plugin_Thumbnailer_Distribution {
	Entropy_Plugin_Thumbnailer base;
};

struct Entropy_Plugin_Thumbnailer_Child {
	Entropy_Plugin_Thumbnailer base;
};

struct Entropy_Plugin_Mime {
	Entropy_Plugin base;
};

typedef struct entropy_mime_object entropy_mime_object;
struct entropy_mime_object {
	char mime_type[100];
	entropy_plugin* plugin;
	
};

#endif
