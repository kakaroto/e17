#include <Ewl.h>

static Ewl_Widget *imenu_button = NULL;

void            __create_imenu_test_window(Ewl_Widget * w, void *ev_data,
					   void *user_data);
void            __item_down(Ewl_Widget * w, void *ev_data, void *user_data);

void
__destroy_imenu_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy(w);

	ewl_callback_append(imenu_button, EWL_CALLBACK_CLICKED,
			    __create_imenu_test_window, NULL);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}


void
__imenu_select(Ewl_Widget * w, void *ev_data, void *user_data)
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
__create_imenu_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget     *imenu_win;
	Ewl_Widget     *imenu_box;
	Ewl_Widget     *imenu1, *imenu2;
	Ewl_Widget     *item;


	ewl_callback_del(w, EWL_CALLBACK_CLICKED, __create_imenu_test_window);

	imenu_button = w;

	imenu_win = ewl_window_new();
	ewl_window_resize(EWL_WINDOW(imenu_win), 300, 300);
	ewl_callback_append(imenu_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_imenu_test_window, NULL);
	ewl_widget_show(imenu_win);

	/*
	 * Create the main box for holding the widgets
	 */
	imenu_box = ewl_vbox_new();
	ewl_object_set_fill_policy(EWL_OBJECT(imenu_box), EWL_FILL_POLICY_FILL);
	ewl_container_append_child(EWL_CONTAINER(imenu_win), imenu_box);
	ewl_theme_data_set_str(imenu_box,
			       "/appearance/box/vertical/base/visible", "yes");
	ewl_widget_show(imenu_box);

	/*
	 * Create the menu
	 */
	imenu1 = ewl_imenu_new(NULL, "test menu");
	ewl_container_append_child(EWL_CONTAINER(imenu_box), imenu1);
	ewl_widget_show(imenu1);

	/*
	 * Append some test items
	 */
	item = ewl_menu_item_new("/usr/share/pixmaps/dia.xpm", "dia");
	ewl_container_append_child(EWL_CONTAINER(imenu1), item);
	ewl_callback_append(item, EWL_CALLBACK_SELECT, __imenu_select, NULL);
	ewl_widget_show(item);

	item = ewl_menu_item_new("/usr/share/pixmaps/wilber.xpm", "gimp");
	ewl_container_append_child(EWL_CONTAINER(imenu1), item);
	ewl_callback_append(item, EWL_CALLBACK_SELECT, __imenu_select, NULL);
	ewl_widget_show(item);

	item = ewl_menu_item_new(NULL, "button");
	ewl_container_append_child(EWL_CONTAINER(imenu1), item);
	ewl_callback_append(item, EWL_CALLBACK_SELECT, __imenu_select, NULL);
	ewl_widget_show(item);

	/*
	 * Create a sub-menu
	 */
	imenu2 = ewl_imenu_new(NULL, "Sub menu");
	ewl_container_append_child(EWL_CONTAINER(imenu1), imenu2);
	ewl_widget_show(imenu2);

	item = ewl_menu_item_new(NULL, "button");
	ewl_container_append_child(EWL_CONTAINER(imenu2), item);
	ewl_callback_append(item, EWL_CALLBACK_SELECT, __imenu_select, NULL);
	ewl_widget_show(item);

	item = ewl_menu_item_new(NULL, "button");
	ewl_container_append_child(EWL_CONTAINER(imenu2), item);
	ewl_callback_append(item, EWL_CALLBACK_SELECT, __imenu_select, NULL);
	ewl_widget_show(item);

	/*
	 * ewl_imenu_set_title_expandable(EWL_IMENU(imenu));
	 */

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}
