#include "entropy.h"
#include <Etk.h>

typedef struct {
	Etk_Widget* window;
	Etk_Widget* entry;
	entropy_generic_file* parent;
	
} etk_directory_add_dialog;


void etk_directory_add_dialog_create_cb(Etk_Object* w, void* user_data) 
{
	etk_directory_add_dialog* dialog = user_data;
	const char* child;

	child = etk_entry_text_get(ETK_ENTRY(dialog->entry));
	if (child && child[0] != 0) {
		entropy_plugin_filesystem_directory_create(dialog->parent, child);
	}

	etk_object_destroy(ETK_OBJECT(dialog->window));
	free(dialog);	
}

void etk_directory_add_dialog_cancel_cb(Etk_Object* w, void* user_data) 
{
	etk_directory_add_dialog* dialog = user_data;

	etk_object_destroy(ETK_OBJECT(dialog->window));
	free(dialog);
}




void etk_directory_add_dialog_create(entropy_generic_file* parent) 
{
	Etk_Widget* vbox;
	Etk_Widget* hbox;
	Etk_Widget* label;
	Etk_Widget* button;
	etk_directory_add_dialog* dialog = calloc(1, sizeof(etk_directory_add_dialog));

	dialog->parent = parent;

	dialog->window = etk_window_new();
	etk_window_title_set(ETK_WINDOW(dialog->window), "Create Directory..");
	etk_window_wmclass_set(ETK_WINDOW(dialog->window), "dirdialog", "dirdialog");
	etk_widget_size_request_set(ETK_WIDGET(dialog->window), 250, 80);

	vbox =etk_vbox_new(ETK_FALSE,0);
	etk_container_add(ETK_CONTAINER(dialog->window), vbox);
	
	hbox = etk_hbox_new(ETK_FALSE,0);
	etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);
	
	label = etk_label_new("Directory Name");
	etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

	dialog->entry = etk_entry_new();
	etk_box_append(ETK_BOX(hbox), dialog->entry, ETK_BOX_START, ETK_BOX_NONE, 0);

	hbox = etk_hbox_new(ETK_FALSE,0);
	etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);



	button = etk_button_new_with_label("Create");
	etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(etk_directory_add_dialog_create_cb), dialog);

	button = etk_button_new_with_label("Cancel");
	etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(etk_directory_add_dialog_cancel_cb), dialog);

	

	etk_widget_show_all(dialog->window);
	
}
