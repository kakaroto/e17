#include <Ewl.h>

static Ewl_Widget *sbook_button = NULL;


void            __create_selectionbook_test_window(Ewl_Widget * w,
						   void *ev_data,
						   void *user_data);

void
__destroy_selectionbook_test_window(Ewl_Widget * w, void *ev_data,
				    void *user_data)
{
	ewl_widget_destroy(w);

	ewl_callback_append(sbook_button, EWL_CALLBACK_CLICKED,
			    __create_selectionbook_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_selectionbook_test_window(Ewl_Widget * w, void *ev_data,
				   void *user_data)
{
	Ewl_Widget     *sbook;
	Ewl_Widget     *sbook_win;
	Ewl_Widget     *spinner;
	Ewl_Widget     *separator;
	Ewl_Widget     *tab[2];
	Ewl_Widget     *panel[2];

	ewl_callback_del(w, EWL_CALLBACK_CLICKED,
			 __create_selectionbook_test_window);

	sbook_button = w;


	sbook_win = ewl_window_new();
	ewl_callback_append(sbook_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_selectionbook_test_window, NULL);
	ewl_widget_show(sbook_win);

	sbook = ewl_selectionbook_new();
	ewl_object_set_fill_policy(EWL_OBJECT(sbook), EWL_FLAG_FILL_FILL);
	ewl_container_append_child(EWL_CONTAINER(sbook_win), sbook);
	ewl_widget_show(sbook);

	tab[0] = ewl_button_new("Tab - ONE");
	ewl_object_set_alignment(EWL_OBJECT(tab[0]), EWL_FLAG_ALIGN_LEFT);
	ewl_object_set_custom_size(EWL_OBJECT(tab[0]), 100, 17);

	tab[1] = ewl_button_new("Tab - TWO");
	ewl_object_set_alignment(EWL_OBJECT(tab[1]), EWL_FLAG_ALIGN_LEFT);
	ewl_object_set_custom_size(EWL_OBJECT(tab[1]), 100, 17);


	panel[0] = ewl_vbox_new();
	ewl_object_set_fill_policy(EWL_OBJECT(panel[0]), EWL_FLAG_FILL_FILL);
	ewl_object_set_alignment(EWL_OBJECT(panel[0]), EWL_FLAG_ALIGN_LEFT);
	ewl_theme_data_set_str(panel[0],
			       "/appearance/box/vertical/base/visible", "yes");

	panel[1] = ewl_vbox_new();
	ewl_object_set_fill_policy(EWL_OBJECT(panel[1]), EWL_FLAG_FILL_FILL);
	ewl_object_set_alignment(EWL_OBJECT(panel[1]), EWL_FLAG_ALIGN_LEFT);
	ewl_theme_data_set_str(panel[1],
			       "/appearance/box/vertical/base/visible", "yes");


	spinner = ewl_spinner_new();
	ewl_container_append_child(EWL_CONTAINER(panel[0]), spinner);
	ewl_widget_show(spinner);


	separator = ewl_hseparator_new();
	ewl_container_append_child(EWL_CONTAINER(panel[1]), separator);
	ewl_widget_show(separator);


	ewl_selectionbook_add_page(EWL_SELECTIONBOOK(sbook), tab[0], panel[0]);
	ewl_selectionbook_add_page(EWL_SELECTIONBOOK(sbook), tab[1], panel[1]);


	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}
