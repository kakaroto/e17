#include <Ewl.h>

static Ewl_Widget *scrollbar_button;

void __create_scrollbar_test_window(Ewl_Widget * w, void *ev_data,
				    void *user_data);


void
__destroy_scrollbar_test_window(Ewl_Widget * w, void *ev_data,
				void *user_data)
{
	ewl_widget_destroy_recursive(w);

	ewl_callback_append(scrollbar_button, EWL_CALLBACK_CLICKED,
			    __create_scrollbar_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_scrollbar_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget *scrollbar_win;
	Ewl_Widget *hscrollbar, *vscrollbar;

	ewl_callback_del(w, EWL_CALLBACK_CLICKED,
			 __create_scrollbar_test_window);

	scrollbar_button = w;

	scrollbar_win = ewl_window_new();
	ewl_box_set_spacing(EWL_BOX(scrollbar_win), 10);
	ewl_callback_append(scrollbar_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_scrollbar_test_window, NULL);
	ewl_widget_show(scrollbar_win);

	hscrollbar = ewl_hscrollbar_new();
	ewl_object_set_padding(EWL_OBJECT(hscrollbar), 10, 10, 10, 0);
	ewl_container_append_child(EWL_CONTAINER(scrollbar_win), hscrollbar);
	ewl_widget_show(hscrollbar);

	vscrollbar = ewl_vscrollbar_new();
	ewl_object_set_padding(EWL_OBJECT(vscrollbar), 10, 10, 10, 10);
	ewl_container_append_child(EWL_CONTAINER(scrollbar_win), vscrollbar);
	ewl_widget_show(vscrollbar);

	return;
	ev_data = NULL;
	user_data = NULL;
}
