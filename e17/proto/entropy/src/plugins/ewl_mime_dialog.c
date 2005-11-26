#include <Ewl.h>
#include "entropy.h"
#include "entropy_gui.h"
#include "entropy_config.h"
#include <dlfcn.h>
#include <time.h>

Ewl_Widget* mime_tree;
Ewl_Widget* entry_type;
Ewl_Widget* entry_action;

void window_dismiss_cb(Ewl_Widget *item, void *ev_data, void *user_data) {
	ewl_widget_destroy(EWL_WIDGET(user_data));
}


void mime_add_cb(Ewl_Widget *item, void *ev_data, void *user_data) {
	char *entries[3];
	char *type_text = ewl_text_text_get(EWL_TEXT(entry_type));
	char *action_text = ewl_text_text_get(EWL_TEXT(entry_action));
	
	
	entries[0] = type_text;
	entries[1] = action_text;
	entries[2] = NULL;

	ewl_tree_text_row_add(EWL_TREE(mime_tree), NULL,entries);
	entropy_core_mime_action_add(type_text, action_text);

	/*free(type_text);
	free(action_text);*/

	ewl_widget_destroy(EWL_WIDGET(user_data));
	
}


void entropy_ewl_mime_add_display_cb(Ewl_Widget *item, void *ev_data, void *user_data) {
	Ewl_Widget* layout_box = ewl_vbox_new();
	Ewl_Widget* window = ewl_window_new();
	Ewl_Widget* hbox;
	
	Ewl_Widget* label;
	Ewl_Widget* button;
	
	ewl_widget_show(layout_box);
	ewl_object_custom_size_set(EWL_OBJECT(window), 300, 150);
	ewl_container_child_append(EWL_CONTAINER(window), layout_box);


	/*---*/
	hbox = ewl_hbox_new();
	ewl_widget_show(hbox);
	ewl_container_child_append(EWL_CONTAINER(layout_box), hbox);

	label = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(label), "MIME Type");
	ewl_widget_show(label);
	ewl_container_child_append(EWL_CONTAINER(hbox), label);

	entry_type = ewl_entry_new();
	ewl_container_child_append(EWL_CONTAINER(hbox), entry_type);
	ewl_widget_show(entry_type);
	/*---*/

	/*---*/
	hbox = ewl_hbox_new();
	ewl_widget_show(hbox);
	ewl_container_child_append(EWL_CONTAINER(layout_box), hbox);

	label = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(label), "Action");
	ewl_widget_show(label);
	ewl_container_child_append(EWL_CONTAINER(hbox), label);

	entry_action = ewl_entry_new();
	ewl_container_child_append(EWL_CONTAINER(hbox), entry_action);
	ewl_widget_show(entry_action);
	/*---*/

	
	hbox = ewl_hbox_new();
	ewl_widget_show(hbox);
	ewl_container_child_append(EWL_CONTAINER(layout_box), hbox);

	button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Add");
	ewl_object_maximum_h_set(EWL_OBJECT(button), 15);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, mime_add_cb, window);
	ewl_container_child_append(EWL_CONTAINER(hbox), button);
	ewl_widget_show(button);

	button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Cancel");
	ewl_object_maximum_h_set(EWL_OBJECT(button), 15);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, window_dismiss_cb, window);
	ewl_container_child_append(EWL_CONTAINER(hbox), button);
	ewl_widget_show(button);


	
	
	ewl_widget_show(window);
	
}



void entropy_ewl_mime_dialog_display() {
	Ewl_Widget* window = ewl_window_new();
	Ewl_Widget* box = ewl_vbox_new();
	Ewl_Widget* button = ewl_button_new();
	Ewl_Widget* hbox;
	
	entropy_core* core = entropy_core_get_core();
	char* entries[3];
	char* key;
	entropy_mime_action* action;
	Ecore_List* keys;


	/*Init the mime tree*/
	mime_tree = ewl_tree_new(2);

	ewl_tree_mode_set(EWL_TREE(mime_tree), EWL_TREE_MODE_SINGLE);
	ewl_object_minimum_size_set(EWL_OBJECT(window), 300, 400);
	ewl_window_title_set(EWL_WINDOW(window), "Edit MIME Actions..");
	ewl_container_child_append(EWL_CONTAINER(window), box);
	ewl_container_child_append(EWL_CONTAINER(box), mime_tree);
	
	ewl_widget_show(box);

	keys = ecore_hash_keys(core->mime_action_hint);
	while ((key = ecore_list_remove_first(keys))) {
		entries[0] = key;
		entries[1] = ((entropy_mime_action*)ecore_hash_get(core->mime_action_hint, key))->executable;
		entries[2] = NULL;

		ewl_tree_text_row_add(EWL_TREE(mime_tree), NULL,entries);
	}


	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(box), hbox);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_SHRINK);
	ewl_widget_show(hbox);


	/*Add Button*/
	ewl_button_label_set(EWL_BUTTON(button), "Add MIME Action");
	ewl_object_maximum_h_set(EWL_OBJECT(button), 15);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, entropy_ewl_mime_add_display_cb, NULL);
	ewl_container_child_append(EWL_CONTAINER(hbox), button);
	ewl_widget_show(button);

	
	/*OK Button*/
	button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(button), "Close");
	ewl_object_maximum_h_set(EWL_OBJECT(button), 15);
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, window_dismiss_cb, window);
	ewl_container_child_append(EWL_CONTAINER(hbox), button);
	ewl_widget_show(button);

	
	
	
	ewl_widget_show(window);
	ewl_widget_show(mime_tree);
	
}
