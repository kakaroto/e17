#ifndef _ETK_LOCATION_ADD_DIALOG_H_
#define _ETK_LOCATION_ADD_DIALOG_H_

typedef struct _entropy_etk_location_add_dialog 
	entropy_etk_location_add_dialog;

struct _entropy_etk_location_add_dialog
{
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
	
	
	int screen_id;
	char* selected_uri;
};

void etk_location_add_dialog_create(entropy_gui_component_instance* instance);


#endif
