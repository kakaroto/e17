#include <Ewl.h>
#include "entropy.h"
#include "entropy_gui.h"
#include <limits.h>
#include <dlfcn.h>

static Ewl_Widget* rename_text_widget;

void ewl_entropy_user_interaction_dialog_cb(Ewl_Widget* w, void* ev_data, void* user_data)
{
	long id = (long)ewl_widget_data_get(w,"operation");
	
        entropy_plugin_operation_respond( id, (int)user_data );
	
	Ewl_Widget* window = ewl_widget_data_get(w, "window");
	ewl_widget_destroy(window);
}

void entropy_ewl_user_interaction_dialog_new(entropy_file_operation* op) 
{

		char buf[PATH_MAX];
		Ewl_Widget* window = ewl_window_new();
		Ewl_Widget* vbox= ewl_vbox_new();
		Ewl_Widget* hbox = ewl_hbox_new();
		Ewl_Widget* button;

		snprintf(buf, PATH_MAX, "Conrifm overwrite for operation ID %ld? ", op->id);
		
		ewl_object_minimum_w_set(EWL_OBJECT(window), 300);
		
		ewl_window_title_set(EWL_WINDOW(window), buf);
		
		ewl_container_child_append(EWL_CONTAINER(window), vbox);
		//ewl_container_child_append(EWL_CONTAINER(vbox), rename_text_widget);
		ewl_container_child_append(EWL_CONTAINER(vbox), hbox);

		button = ewl_button_new();
		ewl_button_label_set(EWL_BUTTON(button), "Yes");
		ewl_container_child_append(EWL_CONTAINER(hbox), button);
		ewl_widget_data_set(button, "window", window);
		ewl_widget_data_set(button, "operation", (long*)op->id);
		ewl_callback_append(button, EWL_CALLBACK_CLICKED, 
			ewl_entropy_user_interaction_dialog_cb, (int*)ENTROPY_USER_INTERACTION_RESPONSE_YES);
		ewl_widget_show(button);

		button = ewl_button_new();
		ewl_button_label_set(EWL_BUTTON(button), "No");
		ewl_container_child_append(EWL_CONTAINER(hbox), button);
		ewl_widget_data_set(button, "window", window);
		ewl_widget_data_set(button, "operation", (long*)op->id);
		ewl_callback_append(button, EWL_CALLBACK_CLICKED, 
			ewl_entropy_user_interaction_dialog_cb, (int*)ENTROPY_USER_INTERACTION_RESPONSE_NO);
		ewl_widget_show(button);

		button = ewl_button_new();
		ewl_button_label_set(EWL_BUTTON(button), "Abort");
		ewl_container_child_append(EWL_CONTAINER(hbox), button);
		ewl_widget_data_set(button, "window", window);
		ewl_widget_data_set(button, "operation", (long*)op->id);
		ewl_callback_append(button, EWL_CALLBACK_CLICKED, 
			ewl_entropy_user_interaction_dialog_cb, (int*)ENTROPY_USER_INTERACTION_RESPONSE_ABORT);
		ewl_widget_show(button);


		ewl_widget_show(window);
		ewl_widget_show(vbox);
		ewl_widget_show(hbox);
}
