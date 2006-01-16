#include "entropy.h"
#include "Ewl.h"
#include "ewl_about_dialog.h"

void entropy_ewl_about_dialog_destroy_cb(Ewl_Widget* w, void* ev_data, void* user_data)
{
	ewl_widget_destroy(EWL_WIDGET(user_data));
}

void entropy_ewl_about_dialog_display() 
{
	Ewl_Widget* about_dialog = ewl_window_new();
	Ewl_Widget* vbox = ewl_vbox_new();
	Ewl_Widget* button = ewl_button_new();
	Ewl_Widget* version_text = ewl_text_new();
	Ewl_Widget* logo = ewl_image_new();
	Ewl_Widget* hbox = ewl_hbox_new();
	
	char* version = VERSION;
	ewl_image_file_set(EWL_IMAGE(logo), PACKAGE_DATA_DIR "/icons/entropy.png", NULL);
	ewl_window_title_set(EWL_WINDOW(about_dialog), "About Entropy...");

	ewl_text_text_set(EWL_TEXT(version_text), version);
	ewl_button_label_set(EWL_BUTTON(button), "OK");
	
	ewl_object_custom_size_set(EWL_OBJECT(about_dialog), 300, 350);
	ewl_object_custom_h_set(EWL_OBJECT(button), 15);

	ewl_container_child_append(EWL_CONTAINER(about_dialog), vbox);

	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_container_child_append(EWL_CONTAINER(hbox), logo);
	ewl_container_child_append(EWL_CONTAINER(hbox), version_text);
	
	ewl_container_child_append(EWL_CONTAINER(vbox), button);


	ewl_callback_append(button, EWL_CALLBACK_CLICKED, entropy_ewl_about_dialog_destroy_cb, about_dialog);
	
	ewl_widget_show(about_dialog);
	ewl_widget_show(button);
	ewl_widget_show(vbox);
	ewl_widget_show(version_text);
	ewl_widget_show(hbox);
	ewl_widget_show(logo);
}
