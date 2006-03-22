#ifndef _ETK_LOCATION_ADD_DIALOG_H_
#define _ETK_LOCATION_ADD_DIALOG_H_

#include "entropy_gui.h"

typedef struct _entropy_etk_location_add_dialog 
	entropy_etk_location_add_dialog;

struct _entropy_etk_location_add_dialog
{
	entropy_gui_component_instance* instance;
	
	Etk_Widget* window;
	Etk_Widget* frame;

	Etk_Widget* vbox1;
	Etk_Widget* vbox2;

	Etk_Widget* name_entry;
	Etk_Widget* path_entry;

	Etk_Widget* host_widget_label;
	Etk_Widget* host_widget_entry;
	Etk_Widget* username_widget_label;
	Etk_Widget* username_widget_entry;
	Etk_Widget* password_widget_label;
	Etk_Widget* password_widget_entry;

	void (*add_callback)(entropy_gui_component_instance*,
			Entropy_Config_Structure*);
	
	int screen_id;
	char* selected_uri;
};

void etk_location_add_dialog_create(entropy_gui_component_instance* instance,
		void (*add_callback)(entropy_gui_component_instance*, Entropy_Config_Structure*) );


#endif
