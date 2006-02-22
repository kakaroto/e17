#include "ewl_test.h"

static Ewl_Widget *menubar_button;

static void
__destroy_menubar_test_window(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(menubar_button, EWL_CALLBACK_CLICKED,
					__create_menubar_test_window, NULL);
}

void
__create_menubar_test_window(Ewl_Widget *w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Widget *menubar_win = NULL, *box = NULL;
	Ewl_Widget *h_menubar = NULL, *v_menubar = NULL;

	menubar_button = w;

	menubar_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(menubar_win), "Menubar Test");
	ewl_window_name_set(EWL_WINDOW(menubar_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(menubar_win), "EFL Test Application");
	ewl_object_minimum_size_set(EWL_OBJECT(menubar_win), 300, 300);

	if (w) {
		ewl_callback_del(w, EWL_CALLBACK_CLICKED, 
					__create_menubar_test_window);
		ewl_callback_append(menubar_win, EWL_CALLBACK_DELETE_WINDOW,
					__destroy_menubar_test_window, NULL);
	} else
		ewl_callback_append(menubar_win, EWL_CALLBACK_DELETE_WINDOW,
					__close_main_window, NULL);
	ewl_widget_show(menubar_win);

	box = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(menubar_win), box);
	ewl_widget_show(box);

	h_menubar = ewl_hmenubar_new();
	{
		int i;
		Ewl_Widget *item;
		Ewl_Widget *foo;
		char *m_items[] = {"File",
				   "Test",
				   "About",
				   NULL};
		for(i = 0; m_items[i] != NULL; i++) {
			item = ewl_menu_new();
			ewl_button_label_set(EWL_BUTTON(item), m_items[i]);
			ewl_container_child_append(EWL_CONTAINER(h_menubar), item);
			ewl_widget_show(item);

			foo = ewl_menu_item_new();
			ewl_button_label_set(EWL_BUTTON(foo), "foo");
			ewl_container_child_append(EWL_CONTAINER(item), foo);
			ewl_widget_show(foo);

			foo = ewl_menu_item_new();
			ewl_button_label_set(EWL_BUTTON(foo), "foobar");
			ewl_container_child_append(EWL_CONTAINER(item), foo);
			ewl_widget_show(foo);

			if (i != 0 && (i % 1) == 0) {
				Ewl_Widget *sep;

				sep = ewl_vseparator_new();
				ewl_container_child_append(EWL_CONTAINER(h_menubar), sep);
				ewl_widget_show(sep);
			}
		}

		item = ewl_spacer_new();
		ewl_object_fill_policy_set(EWL_OBJECT(item), EWL_FLAG_FILL_ALL);
		ewl_container_child_append(EWL_CONTAINER(h_menubar), item);
		ewl_widget_show(item);

		item = ewl_menu_new();
		ewl_button_label_set(EWL_BUTTON(item), "Help");
		ewl_container_child_append(EWL_CONTAINER(h_menubar), item);
		ewl_widget_show(item);

		foo = ewl_menu_item_new();
		ewl_button_label_set(EWL_BUTTON(foo), "foo");
		ewl_container_child_append(EWL_CONTAINER(item), foo);
		ewl_widget_show(foo);

		foo = ewl_menu_item_new();
		ewl_button_label_set(EWL_BUTTON(foo), "foobar");
		ewl_container_child_append(EWL_CONTAINER(item), foo);
		ewl_widget_show(foo);
	}
	ewl_container_child_append(EWL_CONTAINER(box), h_menubar);
	ewl_widget_show(h_menubar);

	v_menubar = ewl_vmenubar_new();
	{
		int i;
		Ewl_Widget *item;
		char *m_items[] = {"About",
				   "Left",
				   "Right",
				   "foo",
				   NULL};
		for(i = 0; m_items[i] != NULL; i++) {
			Ewl_Widget *foo;

			item = ewl_menu_new();
			ewl_button_label_set(EWL_BUTTON(item), m_items[i]);
			ewl_container_child_append(EWL_CONTAINER(v_menubar), item);
			ewl_widget_show(item);

			foo = ewl_menu_item_new();
			ewl_button_label_set(EWL_BUTTON(foo), "foo");
			ewl_container_child_append(EWL_CONTAINER(item), foo);
			ewl_widget_show(foo);

			foo = ewl_menu_item_new();
			ewl_button_label_set(EWL_BUTTON(foo), "foobar");
			ewl_container_child_append(EWL_CONTAINER(item), foo);
			ewl_widget_show(foo);

			if (i != 0 && (i % 2) == 0) 
			{
				Ewl_Widget *sep;

				sep = ewl_hseparator_new();
				ewl_container_child_append(EWL_CONTAINER(v_menubar), sep);
				ewl_widget_show(sep);
			}
		}
	}
	ewl_container_child_append(EWL_CONTAINER(box), v_menubar);
	ewl_widget_show(v_menubar);
}



