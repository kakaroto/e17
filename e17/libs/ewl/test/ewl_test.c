#include <Ewl.h>
#include <ewl-config.h>

#define BUTTONS 18

typedef struct _test_set test_set;
struct _test_set
{
	char *name;
	Ewl_Callback_Function func;
};

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
void            __create_seeker_test_window(Ewl_Widget * w, void *ev_data,
					      void *user_data);
void            __create_spinner_test_window(Ewl_Widget * w, void *ev_data,
					     void *user_data);
void            __create_table_test_window(Ewl_Widget * w, void *ev_data,
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
__close_main_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
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
	static test_set       tests[] = {
		{ "Box", __create_box_test_window },
		{ "Button", __create_button_test_window },
		{ "Entry", __create_entry_test_window },
		{ "Floater", __create_floater_test_window },
		{ "Image", __create_image_test_window },
		{ "Spinner", __create_spinner_test_window },
		{ "Textarea", __create_textarea_test_window },
		{ 0, 0 }
	};

	ewl_init(argc, argv);

	main_win = ewl_window_new();
	ewl_window_set_title(EWL_WINDOW(main_win),
			     "The Enlightenment Widget Library");
	ewl_callback_append(main_win, EWL_CALLBACK_DELETE_WINDOW,
			    __close_main_window, NULL);
	ewl_widget_show(main_win);

	/*
	 * Create the main box for holding the button widgets
	 */
	main_box = ewl_vbox_new();
	ewl_container_append_child(EWL_CONTAINER(main_win), main_box);
	ewl_box_set_spacing(EWL_BOX(main_box), 6);
	ewl_widget_show(main_box);

	i = 0;
	while (tests[i].func) {

		/*
		 * Create the widget and it's test start from the array
		 */
		button[i] = ewl_button_new(tests[i].name);
		ewl_callback_append(button[i], EWL_CALLBACK_CLICKED,
				    tests[i].func, NULL);

		/*
		 * Add the button to the box, and setup it's alignment and
		 * fill.
		 */
		ewl_container_append_child(EWL_CONTAINER(main_box), button[i]);
		ewl_object_set_fill_policy(EWL_OBJECT(button[i]),
					   EWL_FLAG_FILL_NONE);
		ewl_object_set_alignment(EWL_OBJECT(button[i]),
					 EWL_FLAG_ALIGN_CENTER);
		ewl_widget_show(button[i]);
		i++;
	}

	ewl_main();

	return 1;
}
