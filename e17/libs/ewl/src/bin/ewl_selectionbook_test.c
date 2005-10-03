#include "ewl_test.h"

static Ewl_Widget *sbook_button = NULL;

static void
__destroy_selectionbook_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
				    void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(sbook_button, EWL_CALLBACK_CLICKED,
			    __create_selectionbook_test_window, NULL);
}

void
__create_selectionbook_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
				   void *user_data __UNUSED__)
{
	Ewl_Widget     *sbook;
	Ewl_Widget     *sbook_win;
	Ewl_Widget     *spinner;
	Ewl_Widget     *separator;
	Ewl_Widget     *tab[2];
	Ewl_Widget     *panel[2];

	sbook_button = w;

	sbook_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(sbook_win), "Selectionbook Test");
	ewl_window_name_set(EWL_WINDOW(sbook_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(sbook_win), "EFL Test Application");

	if (w) {
		ewl_callback_del(w, EWL_CALLBACK_CLICKED,
			 __create_selectionbook_test_window);
		ewl_callback_append(sbook_win, EWL_CALLBACK_DELETE_WINDOW,
				    __destroy_selectionbook_test_window, NULL);
	} else
		ewl_callback_append(sbook_win, EWL_CALLBACK_DELETE_WINDOW,
					__close_main_window, NULL);
	ewl_widget_show(sbook_win);

	sbook = ewl_selectionbook_new();
	ewl_object_fill_policy_set(EWL_OBJECT(sbook), EWL_FLAG_FILL_FILL);
	ewl_container_child_append(EWL_CONTAINER(sbook_win), sbook);
	ewl_widget_show(sbook);

	tab[0] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(tab[0]), "Tab - ONE");
	ewl_object_alignment_set(EWL_OBJECT(tab[0]), EWL_FLAG_ALIGN_LEFT);
	ewl_object_custom_size_set(EWL_OBJECT(tab[0]), 100, 17);

	tab[1] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(tab[1]), "Tab - TWO");
	ewl_object_alignment_set(EWL_OBJECT(tab[1]), EWL_FLAG_ALIGN_LEFT);
	ewl_object_custom_size_set(EWL_OBJECT(tab[1]), 100, 17);

	panel[0] = ewl_vbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(panel[0]), EWL_FLAG_FILL_FILL);
	ewl_object_alignment_set(EWL_OBJECT(panel[0]), EWL_FLAG_ALIGN_LEFT);
	ewl_theme_data_str_set(panel[0],
			       "/appearance/box/vertical/base/visible", "yes");

	panel[1] = ewl_vbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(panel[1]), EWL_FLAG_FILL_FILL);
	ewl_object_alignment_set(EWL_OBJECT(panel[1]), EWL_FLAG_ALIGN_LEFT);
	ewl_theme_data_str_set(panel[1],
			       "/appearance/box/vertical/base/visible", "yes");

	spinner = ewl_spinner_new();
	ewl_container_child_append(EWL_CONTAINER(panel[0]), spinner);
	ewl_widget_show(spinner);

	separator = ewl_hseparator_new();
	ewl_container_child_append(EWL_CONTAINER(panel[1]), separator);
	ewl_widget_show(separator);

	ewl_selectionbook_page_add(EWL_SELECTIONBOOK(sbook), tab[0], panel[0]);
	ewl_selectionbook_page_add(EWL_SELECTIONBOOK(sbook), tab[1], panel[1]);
}

