#include "ewl_test.h"

static Ewl_Widget *imenu_button = NULL;

static void
__destroy_imenu_test_window(Ewl_Widget * w, void *ev_data __UNUSED__, 
						void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(imenu_button, EWL_CALLBACK_CLICKED,
			    __create_imenu_test_window, NULL);
}

static void
__imenu_focus_in(Ewl_Widget * w, void *ev_data __UNUSED__, 
				void *user_data __UNUSED__)
{
	const char *text;

	text = ewl_button_label_get(EWL_BUTTON(w));
	printf("%s item down\n", text);
}

static void
__imenu_configure(Ewl_Widget * w, void *ev_data __UNUSED__,
				void *user_data __UNUSED__)
{
	printf("Imenu configured to (%d, %d) %dx%d\n", CURRENT_X(w),
			CURRENT_Y(w), CURRENT_W(w), CURRENT_H(w));
}

void
__create_imenu_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Widget     *imenu_win;
	Ewl_Widget     *imenu_box;
	Ewl_Widget     *imenu1, *imenu2;
	Ewl_Widget     *item;

	imenu_button = w;

	imenu_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(imenu_win), "Imenu Test");
	ewl_window_name_set(EWL_WINDOW(imenu_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(imenu_win), "EFL Test Application");

	if (w) {
		ewl_callback_del(w, EWL_CALLBACK_CLICKED, 
					__create_imenu_test_window);
		ewl_callback_append(imenu_win, EWL_CALLBACK_DELETE_WINDOW,
					__destroy_imenu_test_window, NULL);
	} else
		ewl_callback_append(imenu_win, EWL_CALLBACK_DELETE_WINDOW,
					__close_main_window, NULL);
	ewl_widget_show(imenu_win);

	/*
	 * Create the main box for holding the widgets
	 */
	imenu_box = ewl_vbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(imenu_box), EWL_FLAG_FILL_FILL);
	ewl_container_child_append(EWL_CONTAINER(imenu_win), imenu_box);
	ewl_widget_show(imenu_box);

	/*
	 * Create the menu
	 */
	imenu1 = ewl_imenu_new();
	ewl_button_label_set(EWL_BUTTON(imenu1), "test menu");
	ewl_object_fill_policy_set(EWL_OBJECT(imenu1), EWL_FLAG_FILL_NONE);
	ewl_container_child_append(EWL_CONTAINER(imenu_box), imenu1);
	ewl_widget_show(imenu1);

	/*
	 * Append some test items
	 */
	item = ewl_menu_item_new();
	ewl_button_image_set(EWL_BUTTON(item), "/usr/share/pixmaps/dia-diagram.png", NULL);
	ewl_button_label_set(EWL_BUTTON(item), "dia");
	ewl_container_child_append(EWL_CONTAINER(imenu1), item);
	ewl_callback_append(item, EWL_CALLBACK_FOCUS_IN, __imenu_focus_in, NULL);
	ewl_callback_append(item, EWL_CALLBACK_CONFIGURE,
			    __imenu_configure, NULL);
	ewl_widget_show(item);

	item = ewl_separator_new();
	ewl_container_child_append(EWL_CONTAINER(imenu1), item);
	ewl_widget_show(item);

	item = ewl_menu_item_new();
	ewl_button_image_set(EWL_BUTTON(item), "/usr/share/pixmaps/wilber.png", NULL);
	ewl_button_label_set(EWL_BUTTON(item), "gimp");
	ewl_container_child_append(EWL_CONTAINER(imenu1), item);
	ewl_callback_append(item, EWL_CALLBACK_FOCUS_IN, __imenu_focus_in, NULL);
	ewl_widget_show(item);

	item = ewl_menu_item_new();
	ewl_button_label_set(EWL_BUTTON(item), "button");
	ewl_container_child_append(EWL_CONTAINER(imenu1), item);
	ewl_callback_append(item, EWL_CALLBACK_FOCUS_IN, __imenu_focus_in, NULL);
	ewl_widget_show(item);

	/*
	 * Create a sub-menu
	 */
	imenu2 = ewl_imenu_new();
	ewl_button_label_set(EWL_BUTTON(imenu2), "Sub menu");
	ewl_container_child_append(EWL_CONTAINER(imenu1), imenu2);
	ewl_widget_show(imenu2);

	item = ewl_menu_item_new();
	ewl_button_label_set(EWL_BUTTON(item), "button");
	ewl_container_child_append(EWL_CONTAINER(imenu2), item);
	ewl_callback_append(item, EWL_CALLBACK_FOCUS_IN, __imenu_focus_in, NULL);
	ewl_widget_show(item);

	item = ewl_menu_item_new();
	ewl_button_label_set(EWL_BUTTON(item), "button");
	ewl_container_child_append(EWL_CONTAINER(imenu2), item);
	ewl_callback_append(item, EWL_CALLBACK_FOCUS_IN, __imenu_focus_in, NULL);
	ewl_widget_show(item);
}

