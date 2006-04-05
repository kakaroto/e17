#include "Empower.h"

void display_window(int argc, char** argv)
{	
	if(!ecore_x_init(NULL))
	{
		printf("Unable to init ecore\n");
		return;
	}
	
	if(!ewl_init(&argc, argv))
	{
		printf("Unable to init ewl\n");
		return;
	}
	
	Ewl_Widget *entry = NULL;
	Ewl_Widget *ok_button = NULL;
	Ewl_Widget *cancel_button = NULL;
	Ewl_Widget *progtext = NULL;
	Ewl_Widget *vbox=NULL, *hbox = NULL;
	
	int num_roots=0;
	int root_w=0, root_h=0;
	Ecore_X_Window *root_list = NULL;
	root_list = ecore_x_window_root_list(&num_roots);
	
	/*FIXME: add checks for xinerama*/
	ecore_x_window_size_get(root_list[0], &root_w, &root_h);
	
	free(root_list);
	
	xpos = (root_w/2)-(WIDTH/2);
	ypos = (root_h/2)-(HEIGHT/2);
	
	win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(win), "Empower!");
	ewl_window_name_set(EWL_WINDOW(win), "Empower!");
	ewl_window_class_set(EWL_WINDOW(win), "Empower!");
	ewl_object_size_request(EWL_OBJECT(win), WIDTH, HEIGHT);
	ewl_window_move(EWL_WINDOW(win), xpos, ypos);
	ewl_window_borderless_set(EWL_WINDOW(win));
	ewl_window_raise(EWL_WINDOW(win));
	ewl_window_keyboard_grab_set(EWL_WINDOW(win), 1);
	ewl_window_pointer_grab_set(EWL_WINDOW(win), 1);
	ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, destroy_cb, NULL);
	ewl_callback_append(win, EWL_CALLBACK_REVEAL, reveal_cb, NULL);
	ewl_callback_append(win, EWL_CALLBACK_KEY_DOWN, key_down_cb, NULL);
	ewl_widget_show(win);
	
	vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(win), vbox);
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
	ewl_widget_show(vbox);
	
	char user[256];
	
	snprintf(user, 256, "%s's Password:", userinfo->pw_name);
	//strcat(user, userinfo->pw_name);
	
	progtext = ewl_text_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), progtext);
	ewl_text_font_size_set(EWL_TEXT(progtext), 14);
	ewl_text_styles_set(EWL_TEXT(progtext), EWL_TEXT_STYLE_SOFT_SHADOW);
	ewl_text_shadow_color_set(EWL_TEXT(progtext), 30,30,30,50);
	ewl_object_padding_set(EWL_OBJECT(progtext),10,0,3,3);
	ewl_object_minimum_size_set(EWL_OBJECT(progtext), 75, 20);
	ewl_text_text_set(EWL_TEXT(progtext), user);
	ewl_widget_show(progtext);
	
	entry = ewl_password_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), entry);
	ewl_object_padding_set(EWL_OBJECT(entry),8,8,0,0);
	ewl_object_size_request(EWL_OBJECT(entry), 50, 20);
	ewl_callback_append(entry, EWL_CALLBACK_VALUE_CHANGED, pipe_to_sudo_cb, entry);
	ewl_widget_show(entry);
	
	Ewl_Widget *separator = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), separator);
	ewl_widget_color_set(EWL_WIDGET(separator),200,200,200,200);
	ewl_widget_show(separator);
	
	hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
	ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_SHRINK);
	ewl_object_padding_set(EWL_OBJECT(hbox),0,0,0,0);
	ewl_widget_show(hbox);
	
	ok_button = ewl_button_new();
	ewl_button_stock_type_set(EWL_BUTTON(ok_button), EWL_STOCK_OK);
	ewl_object_minimum_size_set(EWL_OBJECT(ok_button), 60, 15);
	ewl_object_maximum_size_set(EWL_OBJECT(ok_button), 60, 15);
	ewl_object_fill_policy_set(EWL_OBJECT(ok_button), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(hbox), ok_button);
	ewl_callback_append(ok_button, EWL_CALLBACK_CLICKED, pipe_to_sudo_cb, entry);
	ewl_widget_show(ok_button);
	
	cancel_button = ewl_button_new();
	ewl_button_stock_type_set(EWL_BUTTON(cancel_button), EWL_STOCK_CANCEL);
	ewl_object_maximum_size_set(EWL_OBJECT(cancel_button), 60, 15);
	ewl_object_fill_policy_set(EWL_OBJECT(cancel_button), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(hbox), cancel_button);
	ewl_callback_append(cancel_button, EWL_CALLBACK_CLICKED, destroy_cb, NULL);
	ewl_widget_show(cancel_button);
	
	ewl_main();	
}
