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
void entropy_plugin_filesystem_auth_respond(char* loc, char* user, char* password);
int entropy_plugin_filesystem_file_copy_multi(Ecore_List* files, char* dest, entropy_gui_component_instance* requester);
int entropy_plugin_filesystem_file_move_multi(Ecore_List* files, char* dest, entropy_gui_component_instance* requester);
int entropy_plugin_filesystem_file_trash_restore(Ecore_List* files, entropy_gui_component_instance* requester);
int entropy_plugin_filesystem_file_move(entropy_generic_file* source, char* dest, entropy_gui_component_instance* requester);
void entropy_plugin_filesystem_directory_create(entropy_generic_file* file, const char* dir);
Ecore_List* entropy_plugin_filesystem_filelist_get(entropy_file_request* request);
void entropy_plugin_filesystem_filestat_get(entropy_file_request* request);
void entropy_plugin_operation_respond(long operation, int response);
void entropy_plugin_filesystem_file_rename(entropy_generic_file* file, char* dest);
void entropy_event_extended_stat_expect(Entropy_Generic_File* file, entropy_gui_component_instance* instance);


void entropy_plugin_filesystem_metadata_groups_get(entropy_gui_component_instance* instance) ;

entropy_plugin* entropy_plugin_gui_get_by_name_toolkit(char* toolkit, char* name);

void entropy_event_stat_request(Entropy_Generic_File* file, entropy_gui_component_instance* instance);
void entropy_event_action_file(Entropy_Generic_File* file, entropy_gui_component_instance* instance);
entropy_generic_file* entropy_layout_current_folder_get(entropy_gui_component_instance_layout* layout);
Ecore_List* entropy_plugin_filesystem_metadata_groups_retrieve() ;
void entropy_plugin_filesystem_file_group_add(entropy_generic_file* file, char* group);
void entropy_plugin_filesystem_file_group_remove(entropy_generic_file* file, char* group);

void entropy_event_copy_request(entropy_gui_component_instance* instance);
void entropy_event_cut_request(entropy_gui_component_instance* instance);
void entropy_event_paste_request(entropy_gui_component_instance* instance);
void entropy_event_hover_request(entropy_gui_component_instance* instance, entropy_generic_file* file, int x, int y);
void entropy_event_dehover_request(entropy_gui_component_instance* instance, entropy_generic_file* file);

void entropy_event_auth_request(entropy_gui_component_instance* instance,char*);
void entropy_plugin_filesystem_metadata_all_get(entropy_gui_component_instance* instance);
void entropy_plugin_filesystem_vfolder_create(char* name, Eina_List* entries);

#endif
