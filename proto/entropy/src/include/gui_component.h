#ifndef _GUI_COMPONENT_H_
#define _GUI_COMPONENT_H_

#include "entropy.h"

#define COMPONENT_ACTIVE 1
#define COMPONENT_INACTIVE 0

struct entropy_gui_component_instance {
	entropy_core* core; /*A reference to the core we came from */
			    /*TODO when we absract this out, change this to a reference to the layout container*/

	entropy_gui_component_instance* layout_parent; /*A layout, or NULL if we are a layout;*/
	int active; /*Are we active or not? I.e. able to consume messages*/

	void* gui_object; /*A reference to a structure of your choice representing a viewable object.
			    In the case of the EWL components, this is the Root widget that the container creates*/
	void* data; /* A reference to a structure containing any additional data we need */
	entropy_plugin* plugin; /*The plugin that is responsible for handling this component */
};

struct entropy_gui_component_instance_layout {
	struct entropy_gui_component_instance base;

	entropy_generic_file* current_folder;
};


entropy_gui_component_instance* entropy_gui_component_instance_new();
entropy_gui_component_instance_layout* entropy_gui_component_instance_layout_new();

entropy_generic_file* 
entropy_gui_component_instance_layout_current_folder_get(
entropy_gui_component_instance_layout* layout);

#endif
