#include "ewl_test.h"

static Ewl_Widget *statusbar_button;
static int current_num = 0;

static void
__button_push_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Statusbar *sb;
	char msg[20];
	
	sb = user_data;

	snprintf(msg, 20, "at %d", current_num++);
	ewl_statusbar_push(sb, msg);

	return;
	w = NULL;
	ev_data = NULL;
}

static void
__button_pop_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Statusbar *sb;

	sb = user_data;
	ewl_statusbar_pop(sb);

	return;
	w = NULL;
	ev_data = NULL;
}

void
__destroy_statusbar_test_window(Ewl_Widget *w, void *ev_data, void *user_data)
{
    ewl_widget_destroy(w);

    ewl_callback_append(statusbar_button, EWL_CALLBACK_CLICKED,
                            __create_statusbar_test_window, NULL);
    return;
    ev_data = NULL;
    user_data = NULL;
}

void
__create_statusbar_test_window(Ewl_Widget *w, void *ev_data, void *user_data)
{
    Ewl_Widget *statusbar_win = NULL, *box = NULL;
    Ewl_Widget *statusbar = NULL, *button = NULL;

    ewl_callback_del(w, EWL_CALLBACK_CLICKED, 
                            __create_statusbar_test_window);

    statusbar_button = w;

    statusbar_win = ewl_window_new();
    ewl_window_title_set(EWL_WINDOW(statusbar_win), "Statusbar Test");
    ewl_window_name_set(EWL_WINDOW(statusbar_win), "EWL Test Application");
    ewl_window_class_set(EWL_WINDOW(statusbar_win), "EFL Test Application");
    ewl_object_minimum_size_set(EWL_OBJECT(statusbar_win), 100, 100);
    ewl_callback_append(statusbar_win, EWL_CALLBACK_DELETE_WINDOW,
                        __destroy_statusbar_test_window, NULL);
    ewl_widget_show(statusbar_win);

    box = ewl_vbox_new();
    ewl_container_child_append(EWL_CONTAINER(statusbar_win), box);
    ewl_widget_show(box);

    statusbar = ewl_statusbar_new();
	ewl_container_child_append(EWL_CONTAINER(box), statusbar);
	ewl_statusbar_left_hide(statusbar);
    ewl_widget_show(statusbar);

	button = ewl_button_new("push");
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, __button_push_cb,
								statusbar);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(box), button);
	ewl_widget_show(button);

	button = ewl_button_new("pop");
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, __button_pop_cb,
								statusbar);
	ewl_object_fill_policy_set(EWL_OBJECT(button), EWL_FLAG_FILL_SHRINK);
	ewl_container_child_append(EWL_CONTAINER(box), button);
	ewl_widget_show(button);

    return;
    ev_data = NULL;
    user_data = NULL;
}



