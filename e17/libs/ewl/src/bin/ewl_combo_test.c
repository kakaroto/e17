#include "ewl_test.h"

static Ewl_Widget *combo_button = NULL;

static void
__destroy_combo_test_window(Ewl_Widget *w, void *ev_data __UNUSED__, 
					void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(combo_button, EWL_CALLBACK_CLICKED,
			    __create_combo_test_window, NULL);
}

static void
__combo_value_changed(Ewl_Widget *w __UNUSED__, void *ev_data, 
					void *user_data __UNUSED__)
{
	Ewl_Widget *entry;
	const char *text;

	entry = EWL_WIDGET(ev_data);
	text = ewl_button_label_get(EWL_BUTTON(entry));

	printf("value changed to %s\n", text);
    
	if (!strcmp(text, "button"))
	{
		char *t2;
		t2 = ewl_widget_data_get(entry, "dummy");
		printf("with data: %s\n", t2);
	}
}

void
__create_combo_test_window(Ewl_Widget * w, void *ev_data __UNUSED__, 
					void *user_data __UNUSED__)
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
	ewl_callback_append(combo1, EWL_CALLBACK_VALUE_CHANGED,
				__combo_value_changed, NULL);
	ewl_widget_show(combo1);

	/*
	 * Append some test items
	 */
	item = ewl_menu_item_new();
	ewl_button_label_set(EWL_BUTTON(item), "dia");
	ewl_button_image_set(EWL_BUTTON(item),
				"/usr/share/pixmaps/dia-diagram.png", NULL);
	ewl_container_child_append(EWL_CONTAINER(combo1), item);
	ewl_widget_show(item);

	item = EWL_WIDGET(ewl_separator_new());
	ewl_container_child_append(EWL_CONTAINER(combo1), item);
	ewl_widget_show(item);

	item = ewl_menu_item_new();
	ewl_button_label_set(EWL_BUTTON(item), "gimp");
	ewl_button_image_set(EWL_BUTTON(item),
				"/usr/share/pixmaps/wilber.png", NULL);
	ewl_container_child_append(EWL_CONTAINER(combo1), item);
	ewl_widget_show(item);

	item = ewl_menu_item_new();
	ewl_button_label_set(EWL_BUTTON(item), "button");
	ewl_widget_data_set(item, "dummy", "data");
	ewl_container_child_append(EWL_CONTAINER(combo1), item);
	ewl_widget_show(item);
}

