#include "ewl_test.h"

static Ewl_Widget *combo_button = NULL;

void            __item_down(Ewl_Widget * w, void *ev_data, void *user_data);

void
__destroy_combo_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy(w);

	ewl_callback_append(combo_button, EWL_CALLBACK_CLICKED,
			    __create_combo_test_window, NULL);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
__combo_value_changed(Ewl_Widget * w, void *ev_data, void *user_data)
{
	char           *text = ev_data;

	printf("value changed to %s\n", text);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
__combo_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	printf("Combo configured to (%d, %d) %dx%d\n", CURRENT_X(w),
			CURRENT_Y(w), CURRENT_W(w), CURRENT_H(w));
	ev_data = NULL;
	user_data = NULL;
}

void
__create_combo_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget     *combo_win;
	Ewl_Widget     *combo_box;
	Ewl_Widget     *combo1;
	Ewl_Widget     *item;

	combo_button = w;

	combo_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(combo_win), "Combo Test");
	ewl_window_name_set(EWL_WINDOW(combo_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(combo_win), "EFL Test Application");

	if (w) {
		ewl_callback_del(w, EWL_CALLBACK_CLICKED, 
						__create_combo_test_window);
		ewl_callback_append(combo_win, EWL_CALLBACK_DELETE_WINDOW,
					    __destroy_combo_test_window, NULL);
	} else
		ewl_callback_append(combo_win, EWL_CALLBACK_DELETE_WINDOW,
					__close_main_window, NULL);
	ewl_widget_show(combo_win);

	/*
	 * Create the main box for holding the widgets
	 */
	combo_box = ewl_vbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(combo_box), EWL_FLAG_FILL_FILL);
	ewl_container_child_append(EWL_CONTAINER(combo_win), combo_box);
	ewl_widget_show(combo_box);

	/*
	 * Create the menu
	 */
	combo1 = ewl_combo_new("test menu");
	ewl_container_child_append(EWL_CONTAINER(combo_box), combo1);
	ewl_callback_append(combo1, EWL_CALLBACK_CONFIGURE,
			    __combo_configure, NULL);
	ewl_callback_append(combo1, EWL_CALLBACK_VALUE_CHANGED,
											__combo_value_changed, NULL);
	ewl_widget_show(combo1);

	/*
	 * Append some test items
	 */
	item = ewl_menu_item_new("/usr/share/pixmaps/dia-diagram.png", "dia");
	ewl_container_child_append(EWL_CONTAINER(combo1), item);
	ewl_widget_show(item);

	item = EWL_WIDGET(ewl_menu_separator_new());
	ewl_container_child_append(EWL_CONTAINER(combo1), item);
	ewl_widget_show(item);

	item = ewl_menu_item_new("/usr/share/pixmaps/wilber.png", "gimp");
	ewl_container_child_append(EWL_CONTAINER(combo1), item);
	ewl_widget_show(item);

	item = ewl_menu_item_new(NULL, "button");
	ewl_container_child_append(EWL_CONTAINER(combo1), item);
	ewl_widget_show(item);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}
