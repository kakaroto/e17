#include "entropy.h"
#include <Etk.h>

typedef struct {
	Etk_Widget* window;
	Etk_Widget* entry;
	entropy_generic_file* orig_file;
	
} etk_file_rename_dialog;


void etk_file_rename_dialog_create_cb(Etk_Object* w, void* user_data) 
{
	etk_file_rename_dialog* dialog = user_data;
	const char* text = etk_entry_text_get(ETK_ENTRY(dialog->entry));

	if (strlen(text))
		entropy_plugin_filesystem_file_rename(dialog->orig_file, (char*)text); 

	etk_object_destroy(ETK_OBJECT(dialog->window));
	free(dialog);	
}

void etk_file_rename_dialog_cancel_cb(Etk_Object* w, void* user_data) 
{
	etk_file_rename_dialog* dialog = user_data;

	etk_object_destroy(ETK_OBJECT(dialog->window));
	free(dialog);
}




void etk_file_rename_dialog_create(entropy_generic_file* parent) 
{
	Etk_Widget* vbox;
	Etk_Widget* hbox;
	Etk_Widget* label;
	Etk_Widget* button;
	etk_file_rename_dialog* dialog = calloc(1, sizeof(etk_file_rename_dialog));

	dialog->orig_file = parent;

	dialog->window = etk_window_new();
	etk_window_title_set(ETK_WINDOW(dialog->window), "Rename file..");
	etk_window_wmclass_set(ETK_WINDOW(dialog->window), "renamedialog", "renamedialog");
	etk_widget_size_request_set(ETK_WIDGET(dialog->window), 250, 80);

	vbox =etk_vbox_new(ETK_FALSE,0);
	etk_container_add(ETK_CONTAINER(dialog->window), vbox);
	
	hbox = etk_hbox_new(ETK_FALSE,0);
	etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);
	
	label = etk_label_new("New Filename");
	etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);

	dialog->entry = etk_entry_new();
	etk_box_append(ETK_BOX(hbox), dialog->entry, ETK_BOX_START, ETK_BOX_NONE, 0);

	hbox = etk_hbox_new(ETK_FALSE,0);
	etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);



	button = etk_button_new_with_label("Rename");
	etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(etk_file_rename_dialog_create_cb), dialog);

	button = etk_button_new_with_label("Cancel");
	etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(etk_file_rename_dialog_cancel_cb), dialog);

	etk_widget_show_all(dialog->window);

	etk_entry_text_set(ETK_ENTRY(dialog->entry), parent->filename);
	
}
