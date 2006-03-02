#include "entropy.h"
#include "entropy_gui.h"
#include <limits.h>
#include <dlfcn.h>
#include <Etk.h>

void etk_entropy_user_interaction_dialog_cb(Etk_Object* w, void* user_data)
{
        void (*operation_func)(long id,int response);
        entropy_plugin* plugin;
	long id = (long)etk_object_data_get(ETK_OBJECT(w),"operation");
	Etk_Widget* window = NULL;
	
	//FIXME 
        plugin = entropy_plugins_type_get_first( ENTROPY_PLUGIN_BACKEND_FILE ,ENTROPY_PLUGIN_SUB_TYPE_ALL);
        operation_func = dlsym(plugin->dl_ref, "entropy_filesystem_operation_respond");
        (*operation_func)( id, (int)user_data );
	
	window = etk_object_data_get(ETK_OBJECT(w), "window");
	etk_object_destroy(ETK_OBJECT(window));
}

void entropy_etk_user_interaction_dialog_new(long operation_id) 
{

		char buf[PATH_MAX];
		Etk_Widget* window = etk_window_new();
		Etk_Widget* vbox= etk_vbox_new(ETK_TRUE,5);
		Etk_Widget* hbox = etk_hbox_new(ETK_TRUE,5);
		Etk_Widget* button;

		snprintf(buf, PATH_MAX, "Conrifm overwrite for operation ID %ld? ", operation_id);
		
		etk_window_title_set(ETK_WINDOW(window), buf);
		
		etk_container_add(ETK_CONTAINER(window), vbox);
		etk_container_add(ETK_CONTAINER(vbox), hbox);

		button = etk_button_new_with_label("Yes");
		etk_container_add(ETK_CONTAINER(hbox), button);
		
		etk_object_data_set(ETK_OBJECT(button), "window", window);
		etk_object_data_set(ETK_OBJECT(button), "operation", (long*)operation_id);

		etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(etk_entropy_user_interaction_dialog_cb), 
				(int*)ENTROPY_USER_INTERACTION_RESPONSE_YES );
		etk_widget_show(button);

		button = etk_button_new();
		etk_button_label_set(ETK_BUTTON(button), "No");
		etk_container_add(ETK_CONTAINER(hbox), button);
		
		etk_object_data_set(ETK_OBJECT(button), "window", window);
		etk_object_data_set(ETK_OBJECT(button), "operation", (long*)operation_id);
		
		etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(etk_entropy_user_interaction_dialog_cb), 
				(int*)ENTROPY_USER_INTERACTION_RESPONSE_NO );
		
		etk_widget_show(button);

		button = etk_button_new();
		etk_button_label_set(ETK_BUTTON(button), "Abort");
		etk_container_add(ETK_CONTAINER(hbox), button);
		etk_object_data_set(ETK_OBJECT(button), "window", window);
		etk_object_data_set(ETK_OBJECT(button), "operation", (long*)operation_id);
	
		etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(etk_entropy_user_interaction_dialog_cb), 
				(int*)ENTROPY_USER_INTERACTION_RESPONSE_ABORT );
		
		etk_widget_show(button);


		etk_widget_show(window);
		etk_widget_show(vbox);
		etk_widget_show(hbox);
}
