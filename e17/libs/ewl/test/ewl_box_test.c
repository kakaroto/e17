#include <Ewl.h>

extern int box_win_realized;

void
__destroy_box_test_window(Ewl_Widget * w, void * ev_data, void * user_data)
{
	ewl_widget_destroy(w);

	box_win_realized = 0;

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__toggle_child_fill_policy(Ewl_Widget * w, void * ev_data, void * user_data)
{
	Ewl_Fill_Policy f;

	f = ewl_object_get_fill_policy(EWL_OBJECT(w));

	if (f == EWL_FILL_POLICY_NORMAL)
	  {
		ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FILL_POLICY_FILL);
		ewl_object_set_custom_size(EWL_OBJECT(w), 0, 0);
		ewl_widget_configure(w->parent);
		ewl_button_set_label(w, "Fill");
	  }
	else
	  {
		ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FILL_POLICY_NORMAL);
		ewl_object_set_custom_size(EWL_OBJECT(w), 100, 17);
		ewl_widget_configure(w->parent);
		ewl_button_set_label(w, "Normal");
	  }
}

void
__toggle_child_horizontal_align(Ewl_Widget * w, void * ev_data,void * user_data)
{
	Ewl_Alignment a;
	char l[10];

	a = ewl_object_get_alignment(EWL_OBJECT(w));

	if (a == EWL_ALIGNMENT_LEFT)
	  {
		a = EWL_ALIGNMENT_CENTER;
		snprintf(l, 10, "Center");
	  }
	else if (a == EWL_ALIGNMENT_CENTER)
	  {
		a = EWL_ALIGNMENT_RIGHT;
		snprintf(l, 10, "Right");
	  }
	else if (a == EWL_ALIGNMENT_RIGHT)
	  {
		a = EWL_ALIGNMENT_LEFT;
		snprintf(l, 10, "Left");
	  }

	ewl_button_set_label(w, l);

	ewl_object_set_alignment(EWL_OBJECT(w), a);

	if (w->parent)
		ewl_widget_configure(w->parent);
}

void
__toggle_child_vertical_align(Ewl_Widget * w, void * ev_data,void * user_data)
{
        Ewl_Alignment a;
	char l[10];

        a = ewl_object_get_alignment(EWL_OBJECT(w));

        if (a == EWL_ALIGNMENT_TOP)
	  {
                a = EWL_ALIGNMENT_CENTER;
		snprintf(l, 10, "Center");
	  }
        else if (a == EWL_ALIGNMENT_CENTER)
	  {
                a = EWL_ALIGNMENT_BOTTOM;
		snprintf(l, 10, "Bottom");
	  }
        else if (a == EWL_ALIGNMENT_BOTTOM)
	  {
                a = EWL_ALIGNMENT_TOP;
		snprintf(l, 10, "Top");
	  }

	ewl_button_set_label(w, l);

        ewl_object_set_alignment(EWL_OBJECT(w), a);

        if (w->parent)
                ewl_widget_configure(w->parent);
}

void
__create_box_test_window(Ewl_Widget * w, void * ev_data, void * user_data)
{
	Ewl_Widget * box_win;
	Ewl_Widget * main_vbox;
	Ewl_Widget * vbox[2];
	Ewl_Widget * vbox_button[2][3];
	Ewl_Widget * hbox[3];
	Ewl_Widget * hbox_button[2][3];

	if (box_win_realized)
		return;

	box_win = ewl_window_new();
	ewl_callback_append(box_win, EWL_CALLBACK_DELETE_WINDOW,
			__destroy_box_test_window, NULL);
	ewl_widget_show(box_win);

	main_vbox = ewl_vbox_new();
	ewl_container_append_child(EWL_CONTAINER(box_win), main_vbox);
	ewl_widget_show(main_vbox);

	hbox[0] = ewl_hbox_new();
	ewl_container_append_child(EWL_CONTAINER(main_vbox), hbox[0]);
	ewl_widget_show(hbox[0]);

	/****************************************************************/

	vbox[0] = ewl_vbox_new();
	ewl_container_append_child(EWL_CONTAINER(hbox[0]), vbox[0]);
	ewl_widget_show(vbox[0]);

	vbox_button[0][0] = ewl_button_new("Left");
	ewl_container_append_child(EWL_CONTAINER(vbox[0]), vbox_button[0][0]);
	ewl_object_set_custom_size(vbox_button[0][0], 100, 17);
	ewl_object_set_alignment(EWL_OBJECT(vbox_button[0][0]),
			EWL_ALIGNMENT_LEFT);
        ewl_callback_append(vbox_button[0][0], EWL_CALLBACK_CLICKED,
                        __toggle_child_horizontal_align, NULL);
	ewl_widget_show(vbox_button[0][0]);

        vbox_button[0][1] = ewl_button_new("Center");
        ewl_container_append_child(EWL_CONTAINER(vbox[0]), vbox_button[0][1]);
	ewl_object_set_custom_size(vbox_button[0][1], 100, 17);
        ewl_object_set_alignment(EWL_OBJECT(vbox_button[0][1]),
			EWL_ALIGNMENT_CENTER);
        ewl_callback_append(vbox_button[0][1], EWL_CALLBACK_CLICKED,
                        __toggle_child_horizontal_align, NULL);
        ewl_widget_show(vbox_button[0][1]);

        vbox_button[0][2] = ewl_button_new("Right");
        ewl_container_append_child(EWL_CONTAINER(vbox[0]), vbox_button[0][2]);
	ewl_object_set_custom_size(vbox_button[0][2], 100, 17);
        ewl_object_set_alignment(EWL_OBJECT(vbox_button[0][2]),
			EWL_ALIGNMENT_RIGHT);
        ewl_callback_append(vbox_button[0][2], EWL_CALLBACK_CLICKED,
                        __toggle_child_horizontal_align, NULL);
        ewl_widget_show(vbox_button[0][2]);

	/****************************************************************/

	vbox[1] = ewl_vbox_new();
	ewl_container_append_child(EWL_CONTAINER(hbox[0]), vbox[1]);
	ewl_widget_show(vbox[1]);

        vbox_button[1][0] = ewl_button_new("Normal");
        ewl_container_append_child(EWL_CONTAINER(vbox[1]), vbox_button[1][0]);
        ewl_object_set_custom_size(vbox_button[1][0], 100, 17);
        ewl_object_set_fill_policy(EWL_OBJECT(vbox_button[1][0]),
			EWL_FILL_POLICY_NORMAL);
/*	ewl_callback_append(vbox_button[1][0], EWL_CALLBACK_CLICKED,
			__toggle_child_fill_policy, NULL);
	FIXME etox is broken atm, we can use this when its fixed. */
        ewl_widget_show(vbox_button[1][0]);

        vbox_button[1][1] = ewl_button_new("Fill");
        ewl_container_append_child(EWL_CONTAINER(vbox[1]), vbox_button[1][1]);
        ewl_object_set_fill_policy(EWL_OBJECT(vbox_button[1][1]),
			EWL_FILL_POLICY_FILL);
/*	ewl_callback_append(vbox_button[1][1], EWL_CALLBACK_CLICKED,
                        __toggle_child_fill_policy, NULL);
	FIXME etox is broken atm, we can use this when its fixed. */
        ewl_widget_show(vbox_button[1][1]);

        vbox_button[1][2] = ewl_button_new("Normal");
        ewl_container_append_child(EWL_CONTAINER(vbox[1]), vbox_button[1][2]);
        ewl_object_set_custom_size(vbox_button[1][2], 100, 17);
        ewl_object_set_fill_policy(EWL_OBJECT(vbox_button[1][2]),
			EWL_FILL_POLICY_NORMAL);
/*	ewl_callback_append(vbox_button[1][2], EWL_CALLBACK_CLICKED,
		__toggle_child_fill_policy, NULL);
	FIXME etox is broken atm, we can use this when its fixed. */
        ewl_widget_show(vbox_button[1][2]);

	/****************************************************************/

	hbox[1] = ewl_hbox_new();
	ewl_container_append_child(EWL_CONTAINER(main_vbox), hbox[1]);
	ewl_widget_show(hbox[1]);

        hbox_button[0][0] = ewl_button_new("Top");
        ewl_container_append_child(EWL_CONTAINER(hbox[1]), hbox_button[0][0]);
        ewl_object_set_custom_size(hbox_button[0][0], 100, 17);
        ewl_object_set_alignment(EWL_OBJECT(hbox_button[0][0]),
			EWL_ALIGNMENT_TOP);
        ewl_callback_append(hbox_button[0][0], EWL_CALLBACK_CLICKED,
                        __toggle_child_vertical_align, NULL);
        ewl_widget_show(hbox_button[0][0]);

        hbox_button[0][1] = ewl_button_new("Center");
        ewl_container_append_child(EWL_CONTAINER(hbox[1]), hbox_button[0][1]);
        ewl_object_set_custom_size(hbox_button[0][1], 100, 17);
        ewl_object_set_alignment(EWL_OBJECT(hbox_button[0][1]),
			EWL_ALIGNMENT_CENTER);
        ewl_callback_append(hbox_button[0][1], EWL_CALLBACK_CLICKED,
                        __toggle_child_vertical_align, NULL);
        ewl_widget_show(hbox_button[0][1]);

        hbox_button[0][2] = ewl_button_new("Bottom");
        ewl_container_append_child(EWL_CONTAINER(hbox[1]), hbox_button[0][2]);
        ewl_object_set_custom_size(hbox_button[0][2], 100, 17);
        ewl_object_set_alignment(EWL_OBJECT(hbox_button[0][2]),
			EWL_ALIGNMENT_BOTTOM);
        ewl_callback_append(hbox_button[0][2], EWL_CALLBACK_CLICKED,
                        __toggle_child_vertical_align, NULL);
        ewl_widget_show(hbox_button[0][2]);

	/****************************************************************/

	hbox[2] = ewl_hbox_new();
	ewl_container_append_child(EWL_CONTAINER(main_vbox), hbox[2]);
	ewl_widget_show(hbox[2]);

        hbox_button[1][0] = ewl_button_new("Normal");
        ewl_container_append_child(EWL_CONTAINER(hbox[2]), hbox_button[1][0]);
        ewl_object_set_custom_size(hbox_button[1][0], 100, 17);
        ewl_object_set_fill_policy(EWL_OBJECT(hbox_button[1][0]),
			EWL_FILL_POLICY_NORMAL);
/*      ewl_callback_append(hbox_button[1][0], EWL_CALLBACK_CLICKED,
                        __toggle_child_fill_policy, NULL);
        FIXME etox is broken atm, we can use this when its fixed. */
        ewl_widget_show(hbox_button[1][0]);

        hbox_button[1][1] = ewl_button_new("Fill");
        ewl_container_append_child(EWL_CONTAINER(hbox[2]), hbox_button[1][1]);
        ewl_object_set_fill_policy(EWL_OBJECT(hbox_button[1][1]),
			EWL_FILL_POLICY_FILL);
/*      ewl_callback_append(hbox_button[1][1], EWL_CALLBACK_CLICKED,
                        __toggle_child_fill_policy, NULL);
        FIXME etox is broken atm, we can use this when its fixed. */
        ewl_widget_show(hbox_button[1][1]);

        hbox_button[1][2] = ewl_button_new("Normal");
        ewl_container_append_child(EWL_CONTAINER(hbox[2]), hbox_button[1][2]);
        ewl_object_set_custom_size(hbox_button[1][2], 100, 17);
        ewl_object_set_fill_policy(EWL_OBJECT(hbox_button[1][2]),
			EWL_FILL_POLICY_NORMAL);
/*      ewl_callback_append(hbox_button[1][2], EWL_CALLBACK_CLICKED,
                        __toggle_child_fill_policy, NULL);
        FIXME etox is broken atm, we can use this when its fixed. */
        ewl_widget_show(hbox_button[1][2]);

	box_win_realized = 1;

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}
