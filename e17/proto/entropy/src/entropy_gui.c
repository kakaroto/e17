#include <entropy.h>

entropy_gui_component_instance* entropy_gui_component_instance_new() {
	entropy_gui_component_instance* inst = entropy_malloc(sizeof(entropy_gui_component_instance));
	inst->active = COMPONENT_ACTIVE;
}

void entropy_gui_component_instance_disable(entropy_gui_component_instance* instance) {
	instance->active = COMPONENT_INACTIVE;
}

void entropy_gui_component_instance_enable(entropy_gui_component_instance* instance) {
	instance->active = COMPONENT_ACTIVE;
}
