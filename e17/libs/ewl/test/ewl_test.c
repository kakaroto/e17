#include <Ewl.h>

#define BUTTONS 14

Ewl_Widget *main_win;
Ewl_Widget *button[BUTTONS];

void __close_main_widow(Ewl_Widget * w, void *ev_data, void *user_data);

void __create_box_test_window(Ewl_Widget * w, void *ev_data, void *user_data);

void __create_button_test_window(Ewl_Widget * w, void *ev_data,
				 void *user_data);

void __create_floater_test_window(Ewl_Widget * w, void *ev_data,
				 void *user_data);

void __create_entry_test_window(Ewl_Widget * w, void *ev_data,
				void *user_data);

void __create_image_test_window(Ewl_Widget * w, void *ev_data,
				void *user_data);

void __create_list_test_window(Ewl_Widget * w, void *ev_data,
			       void *user_data);

void __create_notebook_test_window(Ewl_Widget * w, void *ev_data,
				   void *user_data);

void __create_scrollbar_test_window(Ewl_Widget * w, void *ev_data,
				    void *user_data);
void __create_scrollpane_test_window(Ewl_Widget * w, void *ev_data,
				     void *user_data);

void __create_seeker_test_window(Ewl_Widget * w, void *ev_data,
				 void *user_data);

void __create_spinner_test_window(Ewl_Widget * w, void *ev_data,
				  void *user_data);

void __create_table_test_window(Ewl_Widget * w, void *ev_data,
				void *user_data);

void __create_text_test_window(Ewl_Widget * w, void *ev_data,
			       void *user_data);
void __create_textarea_test_window(Ewl_Widget * w, void *ev_data,
				   void *user_data);

void
__close_main_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy_recursive(main_win);

	ewl_main_quit();

	exit(1);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

int
main(int argc, char **argv)
{
	int i;

	ewl_init(argc, argv);

	main_win = ewl_window_new();
	ewl_box_set_spacing(EWL_BOX(main_win), 6);
	ewl_window_set_title(main_win,
			     "The Enlightenment Widget Library Developer Test Program");
	ewl_callback_append(main_win, EWL_CALLBACK_DELETE_WINDOW,
			    __close_main_window, NULL);
	ewl_window_resize(main_win, 237, 390);
	ewl_window_set_min_size(main_win, 237, 390);
	ewl_widget_show(main_win);

	i = 0;

	button[i] = ewl_button_new("Ewl_Box");
	ewl_object_set_custom_size(EWL_OBJECT(button[i]), 100, 17);
	ewl_object_set_padding(EWL_OBJECT(button[i]), 0, 0, 13, 0);
	ewl_container_append_child(EWL_CONTAINER(main_win), button[i]);
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_box_test_window, NULL);
	i++;

	button[i] = ewl_button_new("Ewl_Button");
	ewl_object_set_custom_size(EWL_OBJECT(button[i]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(main_win), button[i]);
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_button_test_window, NULL);
	i++;

	button[i] = ewl_button_new("Ewl_Entry");
	ewl_object_set_custom_size(EWL_OBJECT(button[i]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(main_win), button[i]);
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_entry_test_window, NULL);
	i++;

	button[i] = ewl_button_new("Ewl_Image");
	ewl_object_set_custom_size(EWL_OBJECT(button[i]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(main_win), button[i]);
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_image_test_window, NULL);
	i++;

	button[i] = ewl_button_new("Ewl_Floater");
	ewl_object_set_custom_size(EWL_OBJECT(button[i]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(main_win), button[i]);
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_floater_test_window, NULL);
	i++;

	button[i] = ewl_button_new("Ewl_List");
	ewl_object_set_custom_size(EWL_OBJECT(button[i]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(main_win), button[i]);
	ewl_widget_disable(button[i]);
	i++;

	button[i] = ewl_button_new("Ewl_Notebook");
	ewl_object_set_custom_size(EWL_OBJECT(button[i]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(main_win), button[i]);
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_notebook_test_window, NULL);
	i++;

	button[i] = ewl_button_new("Ewl_Scrollbar");
	ewl_object_set_custom_size(EWL_OBJECT(button[i]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(main_win), button[i]);
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_scrollbar_test_window, NULL);
	i++;

	button[i] = ewl_button_new("Ewl_ScrollPane");
	ewl_object_set_custom_size(EWL_OBJECT(button[i]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(main_win), button[i]);
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_scrollpane_test_window, NULL);
	i++;

	button[i] = ewl_button_new("Ewl_Seeker");
	ewl_object_set_custom_size(EWL_OBJECT(button[i]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(main_win), button[i]);
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_seeker_test_window, NULL);
	i++;

	button[i] = ewl_button_new("Ewl_Spinner");
	ewl_object_set_custom_size(EWL_OBJECT(button[i]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(main_win), button[i]);
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_spinner_test_window, NULL);
	i++;

	button[i] = ewl_button_new("Ewl_Table");
	ewl_object_set_custom_size(EWL_OBJECT(button[i]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(main_win), button[i]);
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_table_test_window, NULL);
	i++;

	button[i] = ewl_button_new("Ewl_Text");
	ewl_object_set_custom_size(EWL_OBJECT(button[i]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(main_win), button[i]);
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_text_test_window, NULL);
	i++;

	button[i] = ewl_button_new("Ewl_TextArea");
	ewl_object_set_custom_size(EWL_OBJECT(button[i]), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(main_win), button[i]);
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_textarea_test_window, NULL);

	for (i = 0; i < BUTTONS; i++)
	  {
		  ewl_object_set_alignment(EWL_OBJECT(button[i]),
					   EWL_ALIGNMENT_CENTER);
		  ewl_widget_show(button[i]);
	  }

	ewl_main();

	return 1;
}
