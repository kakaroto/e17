#include <Ewl.h>

Ewl_Widget *main_win;
Ewl_Widget *button[11];

void __close_main_widow(Ewl_Widget * w, void *ev_data, void *user_data);

void __create_box_test_window(Ewl_Widget * w, void *ev_data,
			      void *user_data);

void __create_button_test_window(Ewl_Widget * w, void *ev_data,
				 void *user_data);

void __create_entry_test_window(Ewl_Widget * w, void *ev_data,
				void *user_data);

void __create_image_test_window(Ewl_Widget * w, void *ev_data,
				void *user_data);

void __create_notebook_test_window(Ewl_Widget * w, void *ev_data,
				   void *user_data);

void __create_seeker_test_window(Ewl_Widget * w, void *ev_data,
				 void *user_data);

void __create_spinner_test_window(Ewl_Widget * w, void *ev_data,
				  void *user_data);

void __create_table_test_window(Ewl_Widget * w, void *ev_data,
				void *user_data);

void __create_text_test_window(Ewl_Widget * w, void *ev_data,
			       void *user_data);

void
__close_main_widow(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy_recursive(main_win);

	ewl_main_quit();

	exit(1);

	ev_data = NULL;
	user_data = NULL;
}

int
main(int argc, char **argv)
{
	Ewl_Widget *main_hbox;
	Ewl_Widget *vbox;
	int i;

	ewl_init(argc, argv);

	main_win = ewl_window_new();
	ewl_window_set_title(main_win,
			     "The Enlightenment Widget Library Developer Test Program");
	ewl_callback_append(main_win, EWL_CALLBACK_DELETE_WINDOW,
			    __close_main_widow, NULL);
	ewl_window_resize(main_win, 237, 390);
	ewl_window_set_min_size(main_win, 237, 390);
	ewl_widget_show(main_win);

	main_hbox = ewl_hbox_new();
	ewl_box_set_spacing(main_hbox, 5);
	ewl_container_append_child(EWL_CONTAINER(main_win), main_hbox);
	ewl_theme_data_set(main_hbox,
			   "/appearance/box/horizontal/base/visible",
			   "no");
	ewl_widget_show(main_hbox);

	vbox = ewl_vbox_new();
	ewl_box_set_spacing(vbox, 5);
	ewl_object_set_padding(EWL_OBJECT(vbox), 0, 0, 10, 0);
	ewl_container_append_child(EWL_CONTAINER(main_hbox), vbox);
	ewl_theme_data_set(vbox,
			   "/appearance/box/vertical/base/visible", "no");
	ewl_widget_show(vbox);

	button[0] = ewl_button_new("Ewl_Box");
	ewl_object_set_custom_size(EWL_OBJECT(button[0]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(vbox), button[0]);
	ewl_callback_append(button[0], EWL_CALLBACK_CLICKED,
			    __create_box_test_window, NULL);
	ewl_widget_show(button[0]);

	button[1] = ewl_button_new("Ewl_Button");
	ewl_object_set_custom_size(EWL_OBJECT(button[1]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(vbox), button[1]);
	ewl_callback_append(button[1], EWL_CALLBACK_CLICKED,
			    __create_button_test_window, NULL);
	ewl_widget_show(button[1]);

	button[2] = ewl_button_new("Ewl_Entry");
	ewl_object_set_custom_size(EWL_OBJECT(button[2]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(vbox), button[2]);
	ewl_callback_append(button[2], EWL_CALLBACK_CLICKED,
			    __create_entry_test_window, NULL);
	ewl_widget_show(button[2]);

	button[3] = ewl_button_new("Ewl_Image");
	ewl_object_set_custom_size(EWL_OBJECT(button[3]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(vbox), button[3]);
	ewl_callback_append(button[3], EWL_CALLBACK_CLICKED,
			    __create_image_test_window, NULL);
	ewl_widget_show(button[3]);

	button[4] = ewl_button_new("Ewl_List");
	ewl_object_set_custom_size(EWL_OBJECT(button[4]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(vbox), button[4]);
	ewl_widget_show(button[4]);

	button[5] = ewl_button_new("Ewl_Notebook");
	ewl_object_set_custom_size(EWL_OBJECT(button[5]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(vbox), button[5]);
	ewl_callback_append(button[5], EWL_CALLBACK_CLICKED,
			    __create_notebook_test_window, NULL);
	ewl_widget_show(button[5]);

	button[6] = ewl_button_new("Ewl_Seeker");
	ewl_object_set_custom_size(EWL_OBJECT(button[6]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(vbox), button[6]);
	ewl_callback_append(button[6], EWL_CALLBACK_CLICKED,
			    __create_seeker_test_window, NULL);
	ewl_widget_show(button[6]);

	button[7] = ewl_button_new("Ewl_Spinner");
	ewl_object_set_custom_size(EWL_OBJECT(button[7]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(vbox), button[7]);
	ewl_callback_append(button[7], EWL_CALLBACK_CLICKED,
			    __create_spinner_test_window, NULL);
	ewl_widget_show(button[7]);

	button[8] = ewl_button_new("Ewl_Table");
	ewl_object_set_custom_size(EWL_OBJECT(button[8]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(vbox), button[8]);

	ewl_callback_append(button[8], EWL_CALLBACK_CLICKED,
			    __create_table_test_window, NULL);

	ewl_widget_show(button[8]);

	button[9] = ewl_button_new("Ewl_Text");
	ewl_object_set_custom_size(EWL_OBJECT(button[9]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(vbox), button[9]);

	ewl_callback_append(button[9], EWL_CALLBACK_CLICKED,
			    __create_text_test_window, NULL);

	ewl_widget_show(button[9]);

	for (i = 0; i < 10; i++) {
		ewl_object_set_alignment(EWL_OBJECT(button[i]),
					 EWL_ALIGNMENT_CENTER);
	}
	ewl_main();

	return 1;
}
