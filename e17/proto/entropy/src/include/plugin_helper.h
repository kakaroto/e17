#ifndef __PLUGIN_HELPER_H_
#define __PLUGIN_HELPER_H_

#include <Ecore.h>

void entropy_thumbnailer_plugin_print(Ecore_Hash* mime_register);
Ecore_List* entropy_plugins_type_get(int type, int subtype);
char* entropy_plugin_plugin_identify(entropy_plugin* plugin);
char* entropy_plugin_helper_toolkit_get(entropy_plugin* plugin);

void entropy_plugin_filesystem_file_remove(entropy_generic_file* file);
int entropy_plugin_filesystem_file_copy(entropy_generic_file* source, char* dest, entropy_gui_component_instance* requester);
void entropy_plugin_filesystem_directory_create(entropy_generic_file* file, char* dir);

#endif
