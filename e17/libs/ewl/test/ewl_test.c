#include <Ewl.h>
#include <ewl-config.h>

#define BUTTONS 18

void            __close_main_widow(Ewl_Widget * w, void *ev_data,
				   void *user_data);

void            __create_box_test_window(Ewl_Widget * w, void *ev_data,
					 void *user_data);

void            __create_button_test_window(Ewl_Widget * w, void *ev_data,
					    void *user_data);

void            __create_fileselector_test_window(Ewl_Widget * w, void *ev_data,
						  void *user_data);

void            __create_filedialog_test_window(Ewl_Widget * w, void *ev_data,
						  void *user_data);

void            __create_floater_test_window(Ewl_Widget * w, void *ev_data,
					     void *user_data);

void            __create_entry_test_window(Ewl_Widget * w, void *ev_data,
					   void *user_data);

void            __create_image_test_window(Ewl_Widget * w, void *ev_data,
					   void *user_data);

void            __create_imenu_test_window(Ewl_Widget * w, void *ev_data,
					   void *user_data);

void            __create_menu_test_window(Ewl_Widget * w, void *ev_data,
					   void *user_data);

void            __create_notebook_test_window(Ewl_Widget * w, void *ev_data,
					      void *user_data);

void            __create_scrollpane_test_window(Ewl_Widget * w, void *ev_data,
						void *user_data);

void            __create_spinner_test_window(Ewl_Widget * w, void *ev_data,
					     void *user_data);

void            __create_table_test_window(Ewl_Widget * w, void *ev_data,
					   void *user_data);

void            __create_text_test_window(Ewl_Widget * w, void *ev_data,
					  void *user_data);
void            __create_textarea_test_window(Ewl_Widget * w, void *ev_data,
					      void *user_data);
void            __create_tree_test_window(Ewl_Widget * w, void *ev_data,
					      void *user_data);
void            __create_selectionbook_test_window(Ewl_Widget * w,
						   void *ev_data,
						   void *user_data);

void            __create_selectionbar_test_window(Ewl_Widget * w,
						   void *ev_data,
						   void *user_data);

void
__focus_tree_test_window(Ewl_Widget *w, void *ev_data, void *user_data)
{
	printf("Button focus IN\n");
}

void
__unfocus_tree_test_window(Ewl_Widget *w, void *ev_data, void *user_data)
{
	printf("Button focus OUT\n");
}

void
__close_main_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	int x, y, width, height;

	ewl_window_get_geometry(EWL_WINDOW(w), &x, &y, &width, &height);

	ewl_widget_destroy(w);

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
	int             i;
	Ewl_Widget     *main_win;
	Ewl_Widget     *main_box;
	Ewl_Widget     *button[BUTTONS];

	ewl_init(argc, argv);

	main_win = ewl_window_new();
	ewl_window_set_title(EWL_WINDOW(main_win),
			     "The Enlightenment Widget Library Developer Test Program");
	ewl_callback_append(main_win, EWL_CALLBACK_DELETE_WINDOW,
			    __close_main_window, NULL);
	ewl_window_resize(EWL_WINDOW(main_win), 237, 450);
	ewl_window_set_auto_size(EWL_WINDOW(main_win), TRUE);
	ewl_widget_show(main_win);

	i = 0;

	/*
	 * Create the main box for holding the button widgets
	 */
	main_box = ewl_vbox_new();
	ewl_container_append_child(EWL_CONTAINER(main_win), main_box);
	ewl_box_set_spacing(EWL_BOX(main_box), 6);
	ewl_widget_show(main_box);

	/*
	 * Create the button for the box test and add it to the box.
	 */
	button[i] = ewl_button_new("Ewl_Box");
	ewl_object_set_padding(EWL_OBJECT(button[i]), 0, 0, 10, 5);
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_box_test_window, NULL);
	i++;

	/*
	 * Create the button for the button test and add it to the box.
	 */
	button[i] = ewl_button_new("Ewl_Button");
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_button_test_window, NULL);
	i++;

	/*
	 * Create the button for the text entry test and add it to the box.
	 */
	button[i] = ewl_button_new("Ewl_Entry");
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_entry_test_window, NULL);
	i++;

	/*
	 * Create the button for the image test and add it to the box.
	 */
	button[i] = ewl_button_new("Ewl_Image");
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_image_test_window, NULL);
	i++;

	/*
	 * Create the button for the fileselector test and add it to the box.
	 */
	button[i] = ewl_button_new("Ewl_FileSelector");
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_fileselector_test_window, NULL);
	i++;


	/*
	 * Create the button for the fileselector test and add it to the box.
	 */
	button[i] = ewl_button_new("Ewl_FileDialog");
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_filedialog_test_window, NULL);
	i++;

	
	/*
	 * Create the button for the floater test and add it to the box.
	 */
	button[i] = ewl_button_new("Ewl_Floater");
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_floater_test_window, NULL);
	i++;

	/*
	 * Create the button for the imenu test and add it to the box.
	 */
	button[i] = ewl_button_new("Ewl_IMenu");
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_imenu_test_window, NULL);
	i++;

	/*
	 * Create the button for the menu test and add it to the box.
	 */
	button[i] = ewl_button_new("Ewl_Menu");
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_menu_test_window, NULL);
	i++;

	/*
	 * Create the button for the list test and add it to the box.
	 */
	button[i] = ewl_button_new("Ewl_List");
	/*
	 * ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
	 * __create_list_test_window, NULL);
	 */
	ewl_widget_disable(button[i]);
	i++;

	/*
	 * Create the button for the notepad test and add it to the box.
	 */
	button[i] = ewl_button_new("Ewl_Notebook");
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_notebook_test_window, NULL);
	i++;

	/*
	 * Create the button for the selectionbook test and add it to the box.
	 */
	button[i] = ewl_button_new("Ewl_Selectionbook");
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_selectionbook_test_window, NULL);
	i++;

	/*
	 * Create the button for the selectionbook test and add it to the box.
	 */
	button[i] = ewl_button_new("Ewl_Selectionbar");
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_selectionbar_test_window, NULL);
	i++;

	
	/*
	 * Create the button for the spinner test and add it to the box.
	 */
	button[i] = ewl_button_new("Ewl_Spinner");
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_spinner_test_window, NULL);
	i++;

	/*
	 * Create the button for the table test and add it to the box.
	 */
	button[i] = ewl_button_new("Ewl_Table");
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_table_test_window, NULL);
	i++;

	/*
	 * Create the button for the text test and add it to the box.
	 */
	button[i] = ewl_button_new("Ewl_Text");
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_text_test_window, NULL);
	i++;

	/*
	 * Create the button for the text area test and add it to the box.
	 */
	button[i] = ewl_button_new("Ewl_TextArea");
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_textarea_test_window, NULL);
	i++;

	/*
	 * Create the button for the text area test and add it to the box.
	 */
	button[i] = ewl_button_new("Ewl_Tree");
	ewl_callback_append(button[i], EWL_CALLBACK_FOCUS_IN,
			    __focus_tree_test_window, NULL);
	ewl_callback_append(button[i], EWL_CALLBACK_FOCUS_OUT,
			    __unfocus_tree_test_window, NULL);
	ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
			    __create_tree_test_window, NULL);
	i++;

	for (i = 0; i < BUTTONS; i++) {
		ewl_container_append_child(EWL_CONTAINER(main_box), button[i]);
		ewl_object_set_fill_policy(EWL_OBJECT(button[i]),
					   EWL_FILL_POLICY_NONE);
		ewl_object_set_alignment(EWL_OBJECT(button[i]),
					 EWL_ALIGNMENT_CENTER);
		ewl_widget_show(button[i]);
	}

	ewl_main();

	return 1;
}
