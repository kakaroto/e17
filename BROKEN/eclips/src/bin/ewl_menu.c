#include "ewl_menu.h"

static Ewl_Widget *menu_button = NULL;

void            __item_down(Ewl_Widget * w, void *ev_data, void *user_data);

void
__destroy_menu_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy(w);

	ewl_callback_append(menu_button, EWL_CALLBACK_CLICKED,
			    __create_menu_test_window, NULL);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}


void
__menu_select(Ewl_Widget * w, void *ev_data, void *user_data)
{
	char           *text;

	text = ewl_text_get_text(EWL_TEXT(EWL_MENU_ITEM(w)->text));
	printf("%s item down\n", text);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_menu_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget     *menu_win;
	Ewl_Widget     *menu1, *menu2;
	Ewl_Widget     *item;


	ewl_callback_del(w, EWL_CALLBACK_CLICKED, __create_menu_test_window);

	menu_button = w;

	menu_win = ewl_window_new();
	ewl_callback_append(menu_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_menu_test_window, NULL);
	ewl_widget_show(menu_win);

	/*
	 * Create the menu
	 */
	menu1 = ewl_menu_new("/usr/share/pixmaps/wilber.png", "test menu");
	ewl_container_append_child(EWL_CONTAINER(menu_win), menu1);
	ewl_widget_show(menu1);

	/*
	 * Append some test items
	 */
	item = ewl_menu_item_new("/usr/share/pixmaps/mozilla.xpm", "dia");
	ewl_container_append_child(EWL_CONTAINER(menu1), item);
	ewl_callback_append(item, EWL_CALLBACK_SELECT, __menu_select, NULL);
	ewl_widget_show(item);

	item = ewl_menu_item_new("/usr/share/pixmaps/wilber.png", "gimp");
	ewl_container_append_child(EWL_CONTAINER(menu1), item);
	ewl_callback_append(item, EWL_CALLBACK_SELECT, __menu_select, NULL);
	ewl_widget_show(item);

	item = ewl_menu_item_new(NULL, "button");
	ewl_container_append_child(EWL_CONTAINER(menu1), item);
	ewl_callback_append(item, EWL_CALLBACK_SELECT, __menu_select, NULL);
	ewl_widget_show(item);

	/*
	 * Create a sub-menu
	 */
	menu2 = ewl_menu_new(NULL, "Sub menu");
	ewl_container_append_child(EWL_CONTAINER(menu1), menu2);
	ewl_widget_show(menu2);

	item = ewl_menu_item_new(NULL, "button");
	ewl_container_append_child(EWL_CONTAINER(menu2), item);
	ewl_callback_append(item, EWL_CALLBACK_SELECT, __menu_select, NULL);
	ewl_widget_show(item);

	item = ewl_menu_item_new(NULL, "button");
	ewl_container_append_child(EWL_CONTAINER(menu2), item);
	ewl_callback_append(item, EWL_CALLBACK_SELECT, __menu_select, NULL);
	ewl_widget_show(item);

	/*
	 * ewl_menu_set_title_expandable(EWL_MENU(menu));
	 */

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}
