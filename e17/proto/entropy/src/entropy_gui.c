#include <entropy.h>

entropy_gui_component_instance* entropy_gui_component_instance_new() {
	entropy_gui_component_instance* inst = entropy_malloc(sizeof(entropy_gui_component_instance));
	inst->active = COMPONENT_ACTIVE;
}
