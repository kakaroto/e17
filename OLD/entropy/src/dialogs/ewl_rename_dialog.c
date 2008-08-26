#include <Ewl.h>
#include "entropy.h"
#include "entropy_gui.h"
#include <limits.h>
#include <dlfcn.h>

static Ewl_Widget* rename_text_widget;

void ewl_rename_dialog_rename_cb(Ewl_Widget* w, void* ev_data, void* user_data)
{
	Ewl_Widget* window = ewl_widget_data_get(w, "window");
	entropy_file_gui_component* comp = user_data;
	char* new_name = ewl_text_text_get(EWL_TEXT(rename_text_widget));
	
	entropy_plugin_filesystem_file_rename(comp->file, new_name);
	
	entropy_file_gui_component_destroy(comp);
	ewl_widget_destroy(window);
}


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
		Ewl_Widget* vbox= ewl_vbox_new();
		Ewl_Widget* hbox = ewl_hbox_new();
		Ewl_Widget* button;

		rename_text_widget =ewl_entry_new();

		snprintf(buf, PATH_MAX, "Rename '%s'", component_file->file->filename);
		
		ewl_object_minimum_w_set(EWL_OBJECT(window), 300);
		
		ewl_window_title_set(EWL_WINDOW(window), buf);
		ewl_text_text_set(EWL_TEXT(rename_text_widget), component_file->file->filename);
		
		ewl_container_child_append(EWL_CONTAINER(window), vbox);
		ewl_container_child_append(EWL_CONTAINER(vbox), rename_text_widget);
		ewl_container_child_append(EWL_CONTAINER(vbox), hbox);

		button = ewl_button_new();
		ewl_button_label_set(EWL_BUTTON(button), "Rename");
		ewl_container_child_append(EWL_CONTAINER(hbox), button);
		ewl_widget_data_set(button, "window", window);
		ewl_callback_append(button, EWL_CALLBACK_CLICKED, ewl_rename_dialog_rename_cb, component_file);
		ewl_widget_show(button);

		button = ewl_button_new();
		ewl_button_label_set(EWL_BUTTON(button), "Cancel");
		ewl_container_child_append(EWL_CONTAINER(hbox), button);
		ewl_widget_data_set(button, "window", window);
		ewl_callback_append(button, EWL_CALLBACK_CLICKED, ewl_rename_dialog_cancel_cb, component_file);
		ewl_widget_show(button);
		
		ewl_widget_show(window);
		ewl_widget_show(rename_text_widget);
		ewl_widget_show(vbox);
		ewl_widget_show(hbox);
	}
}
