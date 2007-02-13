#include "Empower.h"

void display_window(int argc, char** argv)
{	
	if(!ewl_init(&argc, argv))
	{
		printf("Unable to init ewl\n");
		return;
	}
	
	Ewl_Widget *entry = NULL;
	Ewl_Widget *ok_button = NULL;
	Ewl_Widget *cancel_button = NULL;
	Ewl_Widget *progtext = NULL;
	Ewl_Widget *vbox = NULL, *hbox = NULL;
	Ewl_Widget *image = NULL;
	uid_t user;
	struct passwd *user_name;
	char username[256];
	
	user = getuid();
	if ((user_name = getpwuid(user)) != NULL)
		snprintf(username, 256, "%s's password", user_name->pw_name);
	else
		snprintf(username, 256, "Your Password");
	
	win = ewl_dialog_new();
	ewl_window_title_set(EWL_WINDOW(win), "Empower!");
	ewl_window_name_set(EWL_WINDOW(win), "Empower!");
	ewl_window_class_set(EWL_WINDOW(win), "Empower!");
	ewl_window_dialog_set(EWL_WINDOW(win), 1);
	ewl_window_keyboard_grab_set(EWL_WINDOW(win), 1);
	ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, destroy_cb, NULL);
	ewl_callback_prepend(win, EWL_CALLBACK_REVEAL, reveal_cb, NULL);
	ewl_callback_append(win, EWL_CALLBACK_KEY_DOWN, key_down_cb, NULL);
	ewl_widget_show(win);
	
	ewl_dialog_active_area_set(EWL_DIALOG(win), EWL_POSITION_TOP);

	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(win), hbox);
	ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
	ewl_object_padding_set(EWL_OBJECT(hbox), 15, 15, 15, 5);
	ewl_widget_show(hbox);

	image = ewl_image_new();
	ewl_image_file_set(EWL_IMAGE(image), 
		ewl_icon_theme_icon_path_get(EWL_ICON_SYSTEM_LOCK_SCREEN, 46), 
		EWL_ICON_SYSTEM_LOCK_SCREEN);
	ewl_container_child_append(EWL_CONTAINER(hbox), image);
	ewl_object_padding_set(EWL_OBJECT(image), 5, 20, 0, 0);
	ewl_widget_show(image);

	vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(hbox), vbox);
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_HFILL);
	ewl_widget_show(vbox);

	progtext = ewl_text_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), progtext);
	ewl_text_font_size_set(EWL_TEXT(progtext), 12);
	ewl_object_alignment_set(EWL_OBJECT(progtext), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(progtext), EWL_FLAG_FILL_NONE);
	ewl_text_text_set(EWL_TEXT(progtext), username);
	ewl_widget_show(progtext);
	
	entry = ewl_password_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), entry);
	ewl_callback_append(entry, EWL_CALLBACK_VALUE_CHANGED, pipe_to_sudo_cb, 
					    entry);
	ewl_widget_focus_send(entry);
	ewl_widget_show(entry);

	ewl_dialog_active_area_set(EWL_DIALOG(win), EWL_POSITION_BOTTOM);

	ok_button = ewl_button_new();
	ewl_stock_type_set(EWL_STOCK(ok_button), EWL_STOCK_OK);
	ewl_object_fill_policy_set(EWL_OBJECT(ok_button), EWL_FLAG_FILL_NONE
					| EWL_FLAG_FILL_HFILL);
	ewl_container_child_append(EWL_CONTAINER(win), ok_button);
	ewl_callback_append(ok_button, EWL_CALLBACK_CLICKED, pipe_to_sudo_cb, 
	                    entry);
	ewl_widget_show(ok_button);
	
	cancel_button = ewl_button_new();
	ewl_stock_type_set(EWL_STOCK(cancel_button), EWL_STOCK_CANCEL);
	ewl_object_fill_policy_set(EWL_OBJECT(cancel_button), 
	                           EWL_FLAG_FILL_NONE | EWL_FLAG_FILL_HFILL);
	ewl_container_child_append(EWL_CONTAINER(win), cancel_button);
	ewl_callback_append(cancel_button, EWL_CALLBACK_CLICKED, destroy_cb, 
				NULL);
	ewl_widget_show(cancel_button);
	
	ewl_main();	
}
