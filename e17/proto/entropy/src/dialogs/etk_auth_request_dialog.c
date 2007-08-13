#include "entropy.h"
#include <Etk.h>

typedef struct {
	Etk_Widget* window;
	Etk_Widget* username;
	Etk_Widget* password;
} etk_auth_request_dialog;


void etk_auth_request_dialog_cb(Etk_Object* w, void* user_data) 
{
	etk_auth_request_dialog* dialog = user_data;
	const char* username = etk_entry_text_get(ETK_ENTRY(dialog->username));
	const char* password = etk_entry_text_get(ETK_ENTRY(dialog->password));
	char* location = etk_object_data_get(ETK_OBJECT(dialog->window), "location");

	entropy_plugin_filesystem_auth_respond(location,(char*)username,(char*)password);

	etk_object_destroy(ETK_OBJECT(dialog->window));
	free(dialog);	
	free(location);
}

void etk_auth_request_dialog_cancel_cb(Etk_Object* w, void* user_data) 
{
	etk_auth_request_dialog* dialog = user_data;
	char* location = etk_object_data_get(ETK_OBJECT(dialog->window), "location");	

	etk_object_destroy(ETK_OBJECT(dialog->window));
	free(location);
	free(dialog);
}




void etk_auth_request_dialog_create(char* location)
{
	Etk_Widget* vbox;
	Etk_Widget* hbox;
	Etk_Widget* label;
	Etk_Widget* button;
	etk_auth_request_dialog* dialog = calloc(1, sizeof(etk_auth_request_dialog));


	dialog->window = etk_window_new();
	etk_window_title_set(ETK_WINDOW(dialog->window), "Request for Authentication");
	etk_window_wmclass_set(ETK_WINDOW(dialog->window), "authdialog", "authdialog");
	etk_widget_size_request_set(ETK_WIDGET(dialog->window), 250, 150);

	vbox =etk_vbox_new(ETK_FALSE,0);
	etk_container_add(ETK_CONTAINER(dialog->window), vbox);

	label =etk_label_new(location);
	etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);
	
	hbox = etk_hbox_new(ETK_FALSE,0);
	etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);
	label = etk_label_new("Username");
	etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);
	dialog->username = etk_entry_new();
	etk_box_append(ETK_BOX(hbox), dialog->username, ETK_BOX_START, ETK_BOX_NONE, 0);

	hbox = etk_hbox_new(ETK_FALSE,0);
	etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);
	label = etk_label_new("Password");
	etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 0);
	dialog->password = etk_entry_new();
	etk_entry_password_mode_set(ETK_ENTRY(dialog->password), ETK_TRUE);
	etk_box_append(ETK_BOX(hbox), dialog->password, ETK_BOX_START, ETK_BOX_NONE, 0);

	hbox = etk_hbox_new(ETK_FALSE,0);
	etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);

	button = etk_button_new_with_label("OK");
	etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(etk_auth_request_dialog_cb), dialog);

	button = etk_button_new_with_label("Cancel");
	etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("pressed", ETK_OBJECT(button), ETK_CALLBACK(etk_auth_request_dialog_cancel_cb), dialog);

	etk_object_data_set(ETK_OBJECT(dialog->window), "location", location);
	etk_widget_show_all(dialog->window);

}
