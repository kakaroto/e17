#ifndef __ENTROPY_H_
#define __ENTROPY_H_

typedef struct entropy_config entropy_config;

#include <Ecore.h>
#include <Ecore_Ipc.h>
#include <Ecore_Data.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

#include "entropy_generic.h"
#include "string.h"
#include "plugin_base.h"
#include "plugin_helper.h"
#include "entropy_mime_engine.h"
#include "entropy_thumbnailer_engine.h"
#include "thumbnail_generic.h"
#include "notification_engine.h"
#include "entropy_core.h"
#include "gui_component.h"
#include "entropy_file.h"
#include "entropy_macros.h"
#include "entropy_config.h"

#define IPC_TITLE "entropy"


/*Plugin related functions*/
int entropy_plugin_load(entropy_core* core, entropy_plugin* plugin);
entropy_plugin* create_plugin_object(char* filename);
void entropy_plugin_thumbnailer_register(entropy_core* core, entropy_plugin* plugin);
Ecore_List* entropy_mime_register_init();
Ecore_Hash* entropy_thumbnailers_register_init();
void entropy_plugin_mime_register(Ecore_List*, entropy_plugin*);
entropy_plugin* entropy_plugin_layout_register(entropy_plugin* plugin);
int entropy_core_plugin_type_get(entropy_plugin* plugin);
int entropy_core_plugin_sub_type_get(entropy_plugin* plugin);
entropy_plugin* entropy_plugins_type_get_first(Ecore_List* plugins, int type, int subtype);
void entropy_core_layout_register(entropy_core* core, entropy_gui_component_instance* comp);
entropy_thumbnail* entropy_thumbnail_create(entropy_generic_file* e_file);

/*Event hierarchy functions*/
void entropy_core_component_event_register(entropy_gui_component_instance* comp, char* event);
void entropy_core_layout_notify_event(entropy_gui_component_instance* instance, entropy_gui_event* event, int event_type);
char* entropy_core_gui_event_get(char* event);


/*File/File cache functions*/
void entropy_core_file_cache_add_reference(char* md5);
void entropy_core_file_cache_add(char* md5, entropy_file_listener* listener);
void entropy_core_file_cache_remove_reference(char* md5);
entropy_file_listener* entropy_core_file_cache_retrieve(char* md5);
void generic_file_print(entropy_generic_file* file);
entropy_generic_file* entropy_generic_file_clone(entropy_generic_file* file);
char* md5_entropy_path_file(char* path, char* filename);


/*FS Interaction/EVFS functions*/
entropy_generic_file* entropy_core_parse_uri(char* uri);
char* entropy_core_generic_file_uri_create (entropy_generic_file* file, int drill_down);


/*Selection engine functions*/
void entropy_core_selection_engine_init();
void entropy_core_selected_file_add(entropy_generic_file* file);
Ecore_List* entropy_core_selected_files_get();
void entropy_core_selected_files_clear();

/*Config functions*/
void entropy_core_config_load();
char* entropy_core_home_dir_get();
char* entropy_thumbnail_dir_get();
int entropy_config_int_get(char* module, char* key);
void entropy_config_int_set(char* module, char* key, int value);


/*Global layout object functions*/
entropy_gui_component_instance* entropy_core_global_layout_get(entropy_core* core);


/*Helpers*/
entropy_core* entropy_core_get_core();
void entropy_core_string_lowcase(char *lc);
void* entropy_malloc(size_t);
void entropy_free(void* ref);
char* entropy_core_descent_for_mime_get(entropy_core*, char*);
entropy_mime_action* entropy_core_mime_hint_get(char* mime_type);

/*Logging stuff*/

void entropy_log(char* message, const int level);
#define DEBUG_LEVEL 0
enum LOG_LEVEL {
        ENTROPY_LOG_INFO,
         ENTROPY_LOG_WARN,
         ENTROPY_LOG_ERROR,
         ENTROPy_LOG_DEBUG
        

};

enum LOG_CLASS {
	ENTROPY_CLASS_CORE	
};

#ifndef ENTROPY_CORE
	extern int allocated_events;
	extern int allocated_files;
	extern int allocated_thumbnails;
	extern int allocated_gui_file;
#endif 

void print_allocation();


/* Random defines */
#define ENTROPY_NULL_MIME "object/unidentified"

#endif
