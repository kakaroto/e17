#include <Ewl.h>

static Ewl_Widget *sbar_button = NULL;
		

void __create_selectionbar_test_window(Ewl_Widget * w, void *ev_data,
					void *user_data);

void
__destroy_selectionbar_test_window(Ewl_Widget * w, void *ev_data,
				    void *user_data)
{
	ewl_widget_destroy(w);

	ewl_callback_append(sbar_button, EWL_CALLBACK_CLICKED,
			    __create_selectionbar_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_selectionbar_test_window(Ewl_Widget * w, void *ev_data,
				   void *user_data)
{
	Ewl_Widget *sbar;
	Ewl_Widget *sbar_win;
	Ewl_Widget *box;
	Ewl_Widget *box_button;
	Ewl_Widget *child[3];

	
	ewl_callback_del(w, EWL_CALLBACK_CLICKED,
			 __create_selectionbar_test_window);

	sbar_button = w;


	sbar_win = ewl_window_new();
	ewl_callback_append(sbar_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_selectionbar_test_window, NULL);
	ewl_widget_show(sbar_win);

	box = ewl_vbox_new();
	ewl_object_set_fill_policy(EWL_OBJECT(box), EWL_FLAG_FILL_FILL);
	ewl_container_append_child(EWL_CONTAINER(sbar_win), box);
	ewl_theme_data_set_str(box, 
			"/appearance/box/vertical/base/visible", "yes");
	ewl_widget_show(box);
	
	box_button = ewl_button_new("test test test");
	ewl_object_set_fill_policy(EWL_OBJECT(box_button),
			EWL_FLAG_FILL_SHRINK);
	ewl_container_append_child(EWL_CONTAINER(box), box_button);
	ewl_widget_show(box_button);
	
	sbar = ewl_selectionbar_new(box);
	ewl_object_set_fill_policy(EWL_OBJECT(sbar), EWL_FLAG_FILL_FILL);
	ewl_container_append_child(EWL_CONTAINER(sbar_win), sbar);
	ewl_widget_show(sbar);

	child[0] = ewl_button_new("child 1");
	ewl_container_append_child(EWL_CONTAINER(sbar), child[0]);
	ewl_widget_show(child[0]);

	child[1] = ewl_button_new("child 2");
	ewl_container_append_child(EWL_CONTAINER(sbar), child[1]);
	ewl_widget_show(child[1]);

	child[2] = ewl_button_new("child 3");
	ewl_container_append_child(EWL_CONTAINER(sbar), child[2]);
	ewl_widget_show(child[2]);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}
