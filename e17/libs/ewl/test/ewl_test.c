#include <Ewl.h>


void __close_main_widow(Ewl_Widget * w, void *ev_data, void *user_data);

void __create_box_test_window(Ewl_Widget * w, void *ev_data, void *user_data);

void __create_button_test_window(Ewl_Widget * w, void *ev_data,
				 void *user_data);

void __create_entry_test_window(Ewl_Widget * w, void *ev_data,
				void *user_data);

void __create_text_test_window(Ewl_Widget * w, void *ev_data,
			       void *user_data);

void
__close_main_widow(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy_recursive(w);

	ewl_main_quit();

	exit(1);

	ev_data = NULL;
	user_data = NULL;
}

int
main(int argc, char **argv)
{
	Ewl_Widget *main_win;
	Ewl_Widget *main_hbox;
	Ewl_Widget *vbox[3];
	Ewl_Widget *button[9];

	ewl_init(argc, argv);

	main_win = ewl_window_new();
	ewl_window_set_title(main_win,
			     "The Enlightenment Widget Library Developer Test Program");
	ewl_callback_append(main_win, EWL_CALLBACK_DELETE_WINDOW,
			    __close_main_widow, NULL);
	ewl_window_resize(main_win, 297, 68);
	ewl_window_set_min_size(main_win, 307, 68);
	ewl_widget_show(main_win);

	main_hbox = ewl_hbox_new();
	ewl_box_set_spacing(main_hbox, 5);
	ewl_container_append_child(EWL_CONTAINER(main_win), main_hbox);
	ewl_theme_data_set(main_hbox,
			   "/appearance/box/horizontal/base/visible", "no");
	ewl_widget_show(main_hbox);

	vbox[0] = ewl_vbox_new();
	ewl_box_set_spacing(vbox[0], 5);
	ewl_container_append_child(EWL_CONTAINER(main_hbox), vbox[0]);
	ewl_theme_data_set(vbox[0],
			   "/appearance/box/vertical/base/visible", "no");
	ewl_widget_show(vbox[0]);

	button[0] = ewl_button_new("Ewl_Box");
	ewl_object_set_custom_size(EWL_OBJECT(button[0]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(vbox[0]), button[0]);
	ewl_callback_append(button[0], EWL_CALLBACK_CLICKED,
			    __create_box_test_window, NULL);
	ewl_widget_show(button[0]);

	button[1] = ewl_button_new("Ewl_Button");
	ewl_object_set_custom_size(EWL_OBJECT(button[1]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(vbox[0]), button[1]);
	ewl_callback_append(button[1], EWL_CALLBACK_CLICKED,
			    __create_button_test_window, NULL);
	ewl_widget_show(button[1]);

	button[2] = ewl_button_new("Ewl_Entry");
	ewl_object_set_custom_size(EWL_OBJECT(button[2]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(vbox[0]), button[2]);
	ewl_callback_append(button[2], EWL_CALLBACK_CLICKED,
			    __create_entry_test_window, NULL);
	ewl_widget_show(button[2]);

	vbox[1] = ewl_vbox_new();
	ewl_box_set_spacing(vbox[1], 5);
	ewl_container_append_child(EWL_CONTAINER(main_hbox), vbox[1]);
	ewl_theme_data_set(vbox[1],
			   "/appearance/box/vertical/base/visible", "no");
	ewl_widget_show(vbox[1]);

	button[3] = ewl_button_new("Ewl_Image");
	ewl_object_set_custom_size(EWL_OBJECT(button[3]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(vbox[1]), button[3]);
	ewl_widget_show(button[3]);

	button[4] = ewl_button_new("Ewl_List");
	ewl_object_set_custom_size(EWL_OBJECT(button[4]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(vbox[1]), button[4]);
	ewl_widget_show(button[4]);

	button[5] = ewl_button_new("Ewl_Seeker");
	ewl_object_set_custom_size(EWL_OBJECT(button[5]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(vbox[1]), button[5]);
	ewl_widget_show(button[5]);


	vbox[2] = ewl_vbox_new();
	ewl_box_set_spacing(vbox[2], 5);
	ewl_container_append_child(EWL_CONTAINER(main_hbox), vbox[2]);
	ewl_theme_data_set(vbox[2],
			   "/appearance/box/vertical/base/visible", "no");
	ewl_widget_show(vbox[2]);

	button[6] = ewl_button_new("Ewl_Spinner");
	ewl_object_set_custom_size(EWL_OBJECT(button[6]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(vbox[2]), button[6]);
	ewl_widget_show(button[6]);

	button[7] = ewl_button_new("Ewl_Table");
	ewl_object_set_custom_size(EWL_OBJECT(button[7]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(vbox[2]), button[7]);
	ewl_widget_show(button[7]);

	button[8] = ewl_button_new("Ewl_Text");
	ewl_object_set_custom_size(EWL_OBJECT(button[8]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(vbox[2]), button[8]);

	ewl_callback_append(button[8], EWL_CALLBACK_CLICKED,
				__create_text_test_window, NULL);

	ewl_widget_show(button[8]);

	ewl_main();

	return 1;
}
