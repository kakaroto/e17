#include <Ewl.h>
#include "entropy.h"
#include "entropy_gui.h"
#include <limits.h>

void ewl_rename_dialog_cancel_cb(Ewl_Widget* w, void* ev_data, void* user_data)
{
	Ewl_Widget* window = ewl_widget_data_get(w, "window");
	entropy_file_gui_component* comp = user_data;
	
	entropy_file_gui_component_destroy(comp);
	ewl_widget_destroy(window);
}

void entropy_ewl_rename_dialog_new(entropy_file_gui_component* component_file) 
{

	if (component_file && component_file->file) {
		char buf[PATH_MAX];
		Ewl_Widget* window = ewl_window_new();
		Ewl_Widget* textbox = ewl_entry_new();
		Ewl_Widget* vbox= ewl_vbox_new();
		Ewl_Widget* hbox = ewl_hbox_new();
		Ewl_Widget* button;

		snprintf(buf, PATH_MAX, "Rename '%s'", component_file->file->filename);
		
		ewl_object_minimum_w_set(EWL_OBJECT(window), 300);
		
		ewl_window_title_set(EWL_WINDOW(window), buf);
		ewl_text_text_set(EWL_TEXT(textbox), component_file->file->filename);
		
		ewl_container_child_append(EWL_CONTAINER(window), vbox);
		ewl_container_child_append(EWL_CONTAINER(vbox), textbox);
		ewl_container_child_append(EWL_CONTAINER(vbox), hbox);

		button = ewl_button_new();
		ewl_button_label_set(EWL_BUTTON(button), "Rename");
		ewl_container_child_append(EWL_CONTAINER(hbox), button);
		ewl_widget_show(button);

		button = ewl_button_new();
		ewl_button_label_set(EWL_BUTTON(button), "Cancel");
		ewl_container_child_append(EWL_CONTAINER(hbox), button);
		ewl_widget_data_set(button, "window", window);
		ewl_callback_append(button, EWL_CALLBACK_CLICKED, ewl_rename_dialog_cancel_cb, component_file);
		ewl_widget_show(button);
		
		ewl_widget_show(window);
		ewl_widget_show(textbox);
		ewl_widget_show(vbox);
		ewl_widget_show(hbox);
	}
}
