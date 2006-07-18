#ifndef _ENTROPY_ETK_CONTEXT_MENU_H_
#define _ENTROPY_ETK_CONTEXT_MENU_H_

void _entropy_etk_context_menu_directory_add_cb(Etk_Object *object, void *data);
void entropy_etk_context_menu_init();
void entropy_etk_context_menu_popup(
	entropy_gui_component_instance* instance, entropy_generic_file* current_file);
void _entropy_etk_list_viewer_properties_cb(Etk_Object *object, void *data);

void entropy_etk_context_menu_stat_cb_register(
	void (*cb)(void*, entropy_generic_file*), void* data);

void
entropy_etk_context_menu_metadata_groups_populate();

void entropy_etk_context_menu_popup_multi(entropy_gui_component_instance* instance, 
		Ecore_List* current_files);

#endif
