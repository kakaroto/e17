#include "ewl_test.h"

static Ewl_Widget *menu_button = NULL;

static void
__destroy_menu_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(menu_button, EWL_CALLBACK_CLICKED,
			    __create_menu_test_window, NULL);
}

static void
__menu_focus_in(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	const char *text;

	text = ewl_button_label_get(EWL_BUTTON(w));
	printf("%s item down\n", text);
}

void
__create_menu_test_window(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Widget     *menu_win;
	Ewl_Widget     *menu1, *menu2;
	Ewl_Widget     *item;

	menu_button = w;

	menu_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(menu_win), "Menu Test");
	ewl_window_name_set(EWL_WINDOW(menu_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(menu_win), "EFL Test Application");

	if (w) {
		ewl_callback_del(w, EWL_CALLBACK_CLICKED, 
					__create_menu_test_window);
		ewl_callback_append(menu_win, EWL_CALLBACK_DELETE_WINDOW,
					__destroy_menu_test_window, NULL);
	} else
		ewl_callback_append(menu_win, EWL_CALLBACK_DELETE_WINDOW,
					__close_main_window, NULL);
	ewl_widget_show(menu_win);

	/*
	 * Create the menu
	 */
	menu1 = ewl_menu_new();
	ewl_button_image_set(EWL_BUTTON(menu1), PACKAGE_DATA_DIR "/images/Draw.png", NULL);
	ewl_button_label_set(EWL_BUTTON(menu1), "test menu");
	ewl_container_child_append(EWL_CONTAINER(menu_win), menu1);
	ewl_widget_show(menu1);

	/*
	 * Append some test items
	 */
	item = ewl_menu_item_new();
	ewl_button_image_set(EWL_BUTTON(item), PACKAGE_DATA_DIR "/images/Open.png", NULL);
	ewl_button_label_set(EWL_BUTTON(item), "dia");
	ewl_container_child_append(EWL_CONTAINER(menu1), item);
	ewl_callback_append(item, EWL_CALLBACK_FOCUS_IN, __menu_focus_in, NULL);
	ewl_widget_show(item);

	item = ewl_menu_item_new();
	ewl_button_image_set(EWL_BUTTON(item), PACKAGE_DATA_DIR "/images/Package.png", NULL);
	ewl_button_label_set(EWL_BUTTON(item), "gimp");
	ewl_container_child_append(EWL_CONTAINER(menu1), item);
	ewl_callback_append(item, EWL_CALLBACK_FOCUS_IN, __menu_focus_in, NULL);
	ewl_widget_show(item);

	item = ewl_menu_item_new();
	ewl_button_stock_type_set(EWL_BUTTON(item), EWL_STOCK_OK);
	ewl_container_child_append(EWL_CONTAINER(menu1), item);
	ewl_callback_append(item, EWL_CALLBACK_FOCUS_IN, __menu_focus_in, NULL);
	ewl_widget_show(item);

	item = ewl_menu_item_new();
	ewl_button_label_set(EWL_BUTTON(item), "button");
	ewl_container_child_append(EWL_CONTAINER(menu1), item);
	ewl_callback_append(item, EWL_CALLBACK_FOCUS_IN, __menu_focus_in, NULL);
	ewl_widget_show(item);

	/*
	 * Create a sub-menu
	 */
	menu2 = ewl_menu_new();
	ewl_button_label_set(EWL_BUTTON(menu2), "Sub menu");
	ewl_container_child_append(EWL_CONTAINER(menu1), menu2);
	ewl_widget_show(menu2);

	item = ewl_menu_item_new();
	ewl_button_label_set(EWL_BUTTON(item), "button 1");
	ewl_container_child_append(EWL_CONTAINER(menu2), item);
	ewl_callback_append(item, EWL_CALLBACK_FOCUS_IN, __menu_focus_in, NULL);
	ewl_widget_show(item);

	item = ewl_menu_item_new();
	ewl_button_label_set(EWL_BUTTON(item), "button 2");
	ewl_container_child_append(EWL_CONTAINER(menu2), item);
	ewl_callback_append(item, EWL_CALLBACK_FOCUS_IN, __menu_focus_in, NULL);
	ewl_widget_show(item);
}
