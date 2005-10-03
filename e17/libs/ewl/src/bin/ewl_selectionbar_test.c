#include "ewl_test.h"

static Ewl_Widget *sbar_button = NULL;
		
static void
__destroy_selectionbar_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
				    void *user_data __UNUSED__)
{
	ewl_widget_destroy(w);
	ewl_callback_append(sbar_button, EWL_CALLBACK_CLICKED,
			    __create_selectionbar_test_window, NULL);
}

void
__create_selectionbar_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
				   void *user_data __UNUSED__)
{
	Ewl_Widget *sbar;
	Ewl_Widget *sbar_win;
	Ewl_Widget *box;
	Ewl_Widget *box_button;
	Ewl_Widget *child[3];

	sbar_button = w;

	sbar_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(sbar_win), "Selectionbar Test");
	ewl_window_name_set(EWL_WINDOW(sbar_win), "EWL Test Application");
	ewl_window_class_set(EWL_WINDOW(sbar_win), "EFL Test Application");

	if (w) {
		ewl_callback_del(w, EWL_CALLBACK_CLICKED,
			 __create_selectionbar_test_window);
		ewl_callback_append(sbar_win, EWL_CALLBACK_DELETE_WINDOW,
				    __destroy_selectionbar_test_window, NULL);
	} else
		ewl_callback_append(sbar_win, EWL_CALLBACK_DELETE_WINDOW,
					__close_main_window, NULL);
	ewl_widget_show(sbar_win);

	box = ewl_vbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(box), EWL_FLAG_FILL_FILL);
	ewl_container_child_append(EWL_CONTAINER(sbar_win), box);
	ewl_theme_data_str_set(box, 
			"/appearance/box/vertical/base/visible", "yes");
	ewl_widget_show(box);
	
	box_button = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(box_button), "test test test");
	ewl_object_fill_policy_set(EWL_OBJECT(box_button),
			EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(box), box_button);
	ewl_widget_show(box_button);
	
	sbar = ewl_selectionbar_new(box);
	ewl_object_fill_policy_set(EWL_OBJECT(sbar), EWL_FLAG_FILL_FILL);
	ewl_container_child_append(EWL_CONTAINER(sbar_win), sbar);
	ewl_widget_show(sbar);

	child[0] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(child[0]), "child 1");
	ewl_container_child_append(EWL_CONTAINER(sbar), child[0]);
	ewl_widget_show(child[0]);

	child[1] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(child[1]), "child 2");
	ewl_container_child_append(EWL_CONTAINER(sbar), child[1]);
	ewl_widget_show(child[1]);

	child[2] = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(child[2]), "child 3");
	ewl_container_child_append(EWL_CONTAINER(sbar), child[2]);
	ewl_widget_show(child[2]);
}

