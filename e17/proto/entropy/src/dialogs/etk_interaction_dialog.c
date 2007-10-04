#include "entropy.h"
#include "entropy_gui.h"
#include "etk_user_interaction_dialog.h"
#include <limits.h>
#include <dlfcn.h>
#include <Etk.h>


void etk_entropy_user_interaction_dialog_cb(Etk_Object* w, void* user_data)
{
	long id = (long)etk_object_data_get(ETK_OBJECT(w),"operation");
	Etk_Widget* window = NULL;
	
        entropy_plugin_operation_respond( id, (int)user_data );

	window = etk_object_data_get(ETK_OBJECT(w), "window");
	etk_object_destroy(ETK_OBJECT(window));
}

void etk_entropy_delete_dialog_cb(Etk_Object* w, void* user_data)
{
	Etk_Widget* window = NULL;
	int result = (int)user_data;
	entropy_generic_file* file;
	Ecore_List* files;
	entropy_gui_component_instance* instance;
	
	window = etk_object_data_get(ETK_OBJECT(w), "window");
	files= etk_object_data_get(ETK_OBJECT(window), "files");
	instance = etk_object_data_get(ETK_OBJECT(window), "instance");

	switch (result) {
		case ENTROPY_USER_INTERACTION_RESPONSE_YES: {
			ecore_list_first_goto(files);
			while ((file = ecore_list_first_remove(files))) {
				entropy_plugin_filesystem_file_remove(file, instance);
			}
		}
		break;

		case ENTROPY_USER_INTERACTION_RESPONSE_NO: {
		}
		break;
		
		default: break;
	}

	ecore_list_destroy(files);
	etk_object_destroy(ETK_OBJECT(window));
}

void entropy_etk_delete_dialog_single_new(entropy_gui_component_instance* instance, entropy_generic_file* file)
{
	Ecore_List* files;

	files = ecore_list_new();
	ecore_list_append(files,file);

	entropy_etk_delete_dialog_new(instance,files);
}

void entropy_etk_delete_dialog_new(entropy_gui_component_instance* instance, Ecore_List* files) 
{
	char buf[PATH_MAX];
	Etk_Widget* window = etk_window_new();
	Etk_Widget* vbox= etk_vbox_new(ETK_TRUE,5);
	Etk_Widget* hbox = etk_hbox_new(ETK_TRUE,5);
	Etk_Widget* button;
	Etk_Widget* label;

	if (ecore_list_count(files) == 1) {
		ecore_list_first_goto(files);
		entropy_generic_file* file = ((entropy_generic_file*)ecore_list_current(files));
		snprintf(buf, PATH_MAX, "Are you sure you want to delete '%s/%s'? ", file->path, 
			file->filename);
	} else {
		snprintf(buf, PATH_MAX, "Are you sure you want to delete these %d files'? ", ecore_list_count(files));
	}

	etk_window_title_set(ETK_WINDOW(window), buf);
		
	etk_container_add(ETK_CONTAINER(window), vbox);

	label = etk_label_new(buf);
	etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 5);
	etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 5);

	button = etk_button_new_with_label("Yes");
	etk_container_add(ETK_CONTAINER(hbox), button);
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(etk_entropy_delete_dialog_cb), 
			(int*)ENTROPY_USER_INTERACTION_RESPONSE_YES );
	etk_object_data_set(ETK_OBJECT(button), "window", window);
	etk_widget_show(button);

	button = etk_button_new();
	etk_button_label_set(ETK_BUTTON(button), "No");
	etk_container_add(ETK_CONTAINER(hbox), button);
		
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(etk_entropy_delete_dialog_cb), 
			(int*)ENTROPY_USER_INTERACTION_RESPONSE_NO );
	etk_object_data_set(ETK_OBJECT(button), "window", window);
	etk_widget_show(button);

	etk_object_data_set(ETK_OBJECT(window), "files", files);
	etk_object_data_set(ETK_OBJECT(window), "instance", instance);

	etk_widget_show_all(window);
}

void entropy_etk_user_interaction_dialog_new(entropy_file_operation* interact) 
{

		char buf[PATH_MAX];
		Etk_Widget* window = etk_window_new();
		Etk_Widget* vbox= etk_vbox_new(ETK_TRUE,5);
		Etk_Widget* hbox = etk_hbox_new(ETK_TRUE,5);
		Etk_Widget* button;
		Etk_Widget* label;
		
		snprintf(buf, PATH_MAX, "Confirm overwrite for '%s'? ", interact->file);
		
		etk_window_title_set(ETK_WINDOW(window), buf);
		
		etk_container_add(ETK_CONTAINER(window), vbox);

		label = etk_label_new(buf);
		etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 5);
		etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 5);

		button = etk_button_new_with_label("Yes");
		etk_container_add(ETK_CONTAINER(hbox), button);
		
		etk_object_data_set(ETK_OBJECT(button), "window", window);
		etk_object_data_set(ETK_OBJECT(button), "operation", (long*)interact->id);

		etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(etk_entropy_user_interaction_dialog_cb), 
				(int*)ENTROPY_USER_INTERACTION_RESPONSE_YES );
		etk_widget_show(button);

		button = etk_button_new_with_label("Yes to all");
		etk_container_add(ETK_CONTAINER(hbox), button);
		
		etk_object_data_set(ETK_OBJECT(button), "window", window);
		etk_object_data_set(ETK_OBJECT(button), "operation", (long*)interact->id);

		etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(etk_entropy_user_interaction_dialog_cb), 
				(int*)ENTROPY_USER_INTERACTION_RESPONSE_YES_TO_ALL );
		etk_widget_show(button);

		button = etk_button_new();
		etk_button_label_set(ETK_BUTTON(button), "No");
		etk_container_add(ETK_CONTAINER(hbox), button);
		
		etk_object_data_set(ETK_OBJECT(button), "window", window);
		etk_object_data_set(ETK_OBJECT(button), "operation", (long*)interact->id);
		
		etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(etk_entropy_user_interaction_dialog_cb), 
				(int*)ENTROPY_USER_INTERACTION_RESPONSE_NO );
		
		etk_widget_show(button);

		button = etk_button_new_with_label("No to all");
		etk_container_add(ETK_CONTAINER(hbox), button);
		
		etk_object_data_set(ETK_OBJECT(button), "window", window);
		etk_object_data_set(ETK_OBJECT(button), "operation", (long*)interact->id);

		etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(etk_entropy_user_interaction_dialog_cb), 
				(int*)ENTROPY_USER_INTERACTION_RESPONSE_NO_TO_ALL );
		etk_widget_show(button);

		button = etk_button_new();
		etk_button_label_set(ETK_BUTTON(button), "Abort");
		etk_container_add(ETK_CONTAINER(hbox), button);
		etk_object_data_set(ETK_OBJECT(button), "window", window);
		etk_object_data_set(ETK_OBJECT(button), "operation", (long*)interact->id);
	
		etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(etk_entropy_user_interaction_dialog_cb), 
				(int*)ENTROPY_USER_INTERACTION_RESPONSE_ABORT );
		
		etk_widget_show(button);


		etk_widget_show_all(window);

		etk_widget_show(vbox);
		etk_widget_show(hbox);
}
