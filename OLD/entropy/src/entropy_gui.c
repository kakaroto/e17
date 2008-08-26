#include "entropy.h"
#include "entropy_gui.h"

entropy_gui_component_instance* entropy_gui_component_instance_new() {
	entropy_gui_component_instance* inst = entropy_malloc(sizeof(entropy_gui_component_instance));
	inst->active = COMPONENT_ACTIVE;

	return inst;
}

entropy_gui_component_instance_layout* entropy_gui_component_instance_layout_new() {
	entropy_gui_component_instance_layout* inst = entropy_malloc(sizeof(entropy_gui_component_instance_layout));
	((entropy_gui_component_instance*)inst)->active = COMPONENT_ACTIVE;

	return inst;
}

void entropy_gui_component_instance_disable(entropy_gui_component_instance* instance) {
	instance->active = COMPONENT_INACTIVE;
}

void entropy_gui_component_instance_enable(entropy_gui_component_instance* instance) {
	instance->active = COMPONENT_ACTIVE;
}


/*Miscellaneous structures*/

entropy_generic_file* 
entropy_gui_component_instance_layout_current_folder_get(
entropy_gui_component_instance_layout* layout) {
	return layout->current_folder;
}


entropy_file_gui_component* entropy_file_gui_component_new() 
{
	entropy_file_gui_component* comp = entropy_malloc(sizeof(entropy_file_gui_component));
	return comp;
}

 
entropy_file_gui_component* entropy_file_gui_component_new_with_data(entropy_generic_file* file, 
			entropy_gui_component_instance* instance)
{
	entropy_file_gui_component* comp = entropy_malloc(sizeof(entropy_file_gui_component));

	comp->file = file;
	comp->instance = instance;
	
	return comp;
	
}

void entropy_file_gui_component_destroy(entropy_file_gui_component* comp)
{
	free(comp);
}
