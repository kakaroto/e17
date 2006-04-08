#ifndef __PLUGIN_HELPER_H_
#define __PLUGIN_HELPER_H_

#include <Ecore.h>

void entropy_thumbnailer_plugin_print(Ecore_Hash* mime_register);
Ecore_List* entropy_plugins_type_get(int type, int subtype);
char* entropy_plugin_plugin_identify(entropy_plugin* plugin);
char* entropy_plugin_helper_toolkit_get(entropy_plugin* plugin);

void entropy_plugin_thumbnail_request(entropy_gui_component_instance* requestor, entropy_generic_file* file, 
				void (*cb)());
void entropy_plugin_filesystem_file_remove(entropy_generic_file* file, entropy_gui_component_instance*);
int entropy_plugin_filesystem_file_copy(entropy_generic_file* source, char* dest, entropy_gui_component_instance* requester);
void entropy_plugin_filesystem_directory_create(entropy_generic_file* file, char* dir);
Ecore_List* entropy_plugin_filesystem_filelist_get(entropy_file_request* request);
void entropy_plugin_filesystem_filestat_get(entropy_file_request* request);
void entropy_plugin_operation_respond(long operation, int response);
void entropy_plugin_filesystem_file_rename(entropy_generic_file* file, entropy_generic_file* dest);

entropy_plugin* entropy_plugin_gui_get_by_name_toolkit(char* toolkit, char* name);

void entropy_event_stat_request(Entropy_Generic_File* file, entropy_gui_component_instance* instance);

#endif
