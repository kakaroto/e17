#include "entropy.h"
#include "entropy_gui.h"
#include <limits.h>
#include <dlfcn.h>
#include <Etk.h>

void etk_entropy_user_interaction_dialog_cb(Etk_Object* w, void* user_data)
{
	long id = (long)etk_object_data_get(ETK_OBJECT(w),"operation");
	Etk_Widget* window = NULL;
	
        entropy_plugin_operation_respond( id, (int)user_data );

	etk_signal_disconnect("pressed", ETK_OBJECT(w), ETK_CALLBACK(etk_entropy_user_interaction_dialog_cb));
	
	window = etk_object_data_get(ETK_OBJECT(w), "window");
	etk_object_destroy(ETK_OBJECT(window));
}

void entropy_etk_user_interaction_dialog_new(entropy_file_operation* interact) 
{

		const char buf[PATH_MAX];
		Etk_Widget* window = etk_window_new();
		Etk_Widget* vbox= etk_vbox_new(ETK_TRUE,5);
		Etk_Widget* hbox = etk_hbox_new(ETK_TRUE,5);
		Etk_Widget* button;
		Etk_Widget* label;
		
		snprintf(buf, PATH_MAX, "Confirm overwrite for '%s'? ", interact->file);
		
		etk_window_title_set(ETK_WINDOW(window), buf);
		
		etk_container_add(ETK_CONTAINER(window), vbox);

		label = etk_label_new(buf);
		etk_box_pack_start(ETK_BOX(vbox), label, ETK_TRUE, ETK_TRUE, 5);
		etk_box_pack_start(ETK_BOX(vbox), hbox, ETK_FALSE, ETK_FALSE, 5);

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
