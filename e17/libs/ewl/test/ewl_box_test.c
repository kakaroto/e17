#include <Ewl.h>

static Ewl_Widget *box_button = NULL;

void            __create_box_test_window(Ewl_Widget * w, void *ev_data,
					 void *user_data);

void
__destroy_box_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_destroy(w);

	ewl_callback_append(box_button, EWL_CALLBACK_CLICKED,
			    __create_box_test_window, NULL);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__toggle_child_fill(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Fill_Policy f;

	f = ewl_object_get_fill_policy(EWL_OBJECT(w));

	if (f == EWL_FILL_POLICY_NONE) {
		ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FILL_POLICY_FILL);
		ewl_button_set_label(EWL_BUTTON(w), "Fill");
		ewl_widget_configure(w->parent);
	} else {
		ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FILL_POLICY_NONE);
		ewl_button_set_label(EWL_BUTTON(w), "None");
		ewl_widget_configure(w->parent);
	}

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__toggle_child_shrink(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Fill_Policy f;

	f = ewl_object_get_fill_policy(EWL_OBJECT(w));

	if (f == EWL_FILL_POLICY_NONE) {
		ewl_object_set_fill_policy(EWL_OBJECT(w),
					   EWL_FILL_POLICY_SHRINK);
		ewl_button_set_label(EWL_BUTTON(w),
				     "Shrink This Box To Fit It's Parent");
		ewl_widget_configure(w->parent);
	} else {
		ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FILL_POLICY_NONE);
		ewl_button_set_label(EWL_BUTTON(w),
				     "Don't shrink this box at all");
		ewl_widget_configure(w->parent);
	}

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__toggle_child_horizontal_align(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Alignment   a;
	char            l[10];

	a = ewl_object_get_alignment(EWL_OBJECT(w));

	if (a == EWL_ALIGNMENT_LEFT) {
		a = EWL_ALIGNMENT_CENTER;
		snprintf(l, 10, "Center");
	} else if (a == EWL_ALIGNMENT_CENTER) {
		a = EWL_ALIGNMENT_RIGHT;
		snprintf(l, 10, "Right");
	} else if (a == EWL_ALIGNMENT_RIGHT) {
		a = EWL_ALIGNMENT_LEFT;
		snprintf(l, 10, "Left");
	}

	ewl_button_set_label(EWL_BUTTON(w), l);

	ewl_object_set_alignment(EWL_OBJECT(w), a);

	if (w->parent)
		ewl_widget_configure(w->parent);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__toggle_child_vertical_align(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Alignment   a;
	char            l[10];

	a = ewl_object_get_alignment(EWL_OBJECT(w));

	if (a == EWL_ALIGNMENT_TOP) {
		a = EWL_ALIGNMENT_CENTER;
		snprintf(l, 10, "Center");
	} else if (a == EWL_ALIGNMENT_CENTER) {
		a = EWL_ALIGNMENT_BOTTOM;
		snprintf(l, 10, "Bottom");
	} else if (a == EWL_ALIGNMENT_BOTTOM) {
		a = EWL_ALIGNMENT_TOP;
		snprintf(l, 10, "Top");
	}

	ewl_button_set_label(EWL_BUTTON(w), l);

	ewl_object_set_alignment(EWL_OBJECT(w), a);

	if (w->parent)
		ewl_widget_configure(w->parent);

	return;
	ev_data = NULL;
	user_data = NULL;
}

void
__create_box_test_window(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Widget     *box_win;
	Ewl_Widget     *box_box;
	Ewl_Widget     *vbox[2];
	Ewl_Widget     *vbox_button[2][3];
	Ewl_Widget     *hbox[3];
	Ewl_Widget     *hbox_button[2][3];

	ewl_callback_del(w, EWL_CALLBACK_CLICKED, __create_box_test_window);

	box_button = w;

	box_win = ewl_window_new();
	ewl_callback_append(box_win, EWL_CALLBACK_DELETE_WINDOW,
			    __destroy_box_test_window, NULL);
	ewl_widget_show(box_win);

	/*
	 * Create the main box for holding the widgets
	 */
	box_box = ewl_vbox_new();
	ewl_container_append_child(EWL_CONTAINER(box_win), box_box);
	ewl_widget_show(box_box);

	/*
	 * Create the first horizontal box, this is positioned in the upper
	 * left corner.
	 */
	hbox[0] = ewl_hbox_new();
	ewl_container_append_child(EWL_CONTAINER(box_box), hbox[0]);
	ewl_widget_show(hbox[0]);

	/******************************************************************/
	/* Create a box for holding the horizontal alignment test buttons */

	/******************************************************************/
	vbox[0] = ewl_vbox_new();
	ewl_container_append_child(EWL_CONTAINER(hbox[0]), vbox[0]);
	ewl_widget_show(vbox[0]);

	/*
	 * Create and setup the button that starts in the left position.
	 */
	vbox_button[0][0] = ewl_button_new("Left");
	ewl_container_append_child(EWL_CONTAINER(vbox[0]), vbox_button[0][0]);
	ewl_object_set_fill_policy(EWL_OBJECT(vbox_button[0][0]),
				   EWL_FILL_POLICY_NONE);
	ewl_object_set_alignment(EWL_OBJECT(vbox_button[0][0]),
				 EWL_ALIGNMENT_LEFT);
	ewl_callback_append(vbox_button[0][0], EWL_CALLBACK_CLICKED,
			    __toggle_child_horizontal_align, NULL);
	ewl_widget_show(vbox_button[0][0]);

	/*
	 * Create and setup the button that starts in the center position.
	 */
	vbox_button[0][1] = ewl_button_new("Center");
	ewl_container_append_child(EWL_CONTAINER(vbox[0]), vbox_button[0][1]);
	ewl_object_set_fill_policy(EWL_OBJECT(vbox_button[0][1]),
				   EWL_FILL_POLICY_NONE);
	ewl_object_set_alignment(EWL_OBJECT(vbox_button[0][1]),
				 EWL_ALIGNMENT_CENTER);
	ewl_callback_append(vbox_button[0][1], EWL_CALLBACK_CLICKED,
			    __toggle_child_horizontal_align, NULL);
	ewl_widget_show(vbox_button[0][1]);

	/*
	 * Create and setup the button that starts in the right position.
	 */
	vbox_button[0][2] = ewl_button_new("Right");
	ewl_container_append_child(EWL_CONTAINER(vbox[0]), vbox_button[0][2]);
	ewl_object_set_fill_policy(EWL_OBJECT(vbox_button[0][2]),
				   EWL_FILL_POLICY_NONE);
	ewl_object_set_alignment(EWL_OBJECT(vbox_button[0][2]),
				 EWL_ALIGNMENT_RIGHT);
	ewl_callback_append(vbox_button[0][2], EWL_CALLBACK_CLICKED,
			    __toggle_child_horizontal_align, NULL);
	ewl_widget_show(vbox_button[0][2]);

	/****************************************************************/
	/* Create a box for holding the Fill test buttons               */

	/****************************************************************/
	vbox[1] = ewl_vbox_new();
	ewl_container_append_child(EWL_CONTAINER(hbox[0]), vbox[1]);
	ewl_widget_show(vbox[1]);

	/*
	 * Create and setup a button with no filling by default.
	 */
	vbox_button[1][0] = ewl_button_new("None");
	ewl_container_append_child(EWL_CONTAINER(vbox[1]), vbox_button[1][0]);
	ewl_object_set_fill_policy(EWL_OBJECT(vbox_button[1][0]),
				   EWL_FILL_POLICY_NONE);
	ewl_box_set_orientation(EWL_BOX(vbox_button[1][0]),
			EWL_ORIENTATION_VERTICAL);
	ewl_object_set_alignment(EWL_OBJECT(EWL_BUTTON(vbox_button[1][0])->label_object),
				 EWL_ALIGNMENT_CENTER);
	ewl_callback_append(vbox_button[1][0], EWL_CALLBACK_CLICKED,
			    __toggle_child_fill, NULL);
	ewl_widget_show(vbox_button[1][0]);

	/*
	 * Create and setup a button with filling by default.
	 */
	vbox_button[1][1] = ewl_button_new("Fill");
	ewl_container_append_child(EWL_CONTAINER(vbox[1]), vbox_button[1][1]);
	ewl_box_set_orientation(EWL_BOX(vbox_button[1][1]),
			EWL_ORIENTATION_VERTICAL);
	ewl_object_set_fill_policy(EWL_OBJECT(vbox_button[1][1]),
				   EWL_FILL_POLICY_FILL);
	ewl_object_set_alignment(EWL_OBJECT(EWL_BUTTON(vbox_button[1][1])->label_object),
				 EWL_ALIGNMENT_CENTER);
	ewl_callback_append(vbox_button[1][1], EWL_CALLBACK_CLICKED,
			    __toggle_child_fill, NULL);
	ewl_widget_show(vbox_button[1][1]);

	/*
	 * Create and setup a button with no filling by default.
	 */
	vbox_button[1][2] = ewl_button_new("None");
	ewl_container_append_child(EWL_CONTAINER(vbox[1]), vbox_button[1][2]);
	ewl_box_set_orientation(EWL_BOX(vbox_button[1][2]),
			EWL_ORIENTATION_VERTICAL);
	ewl_object_set_fill_policy(EWL_OBJECT(vbox_button[1][2]),
				   EWL_FILL_POLICY_NONE);
	ewl_object_set_alignment(EWL_OBJECT(EWL_BUTTON(vbox_button[1][2])->label_object),
				 EWL_ALIGNMENT_CENTER);
	ewl_callback_append(vbox_button[1][2], EWL_CALLBACK_CLICKED,
			    __toggle_child_fill, NULL);
	ewl_widget_show(vbox_button[1][2]);

	/****************************************************************/
	/* Create a box for holding the vertical alignment test buttons */

	/****************************************************************/
	hbox[1] = ewl_hbox_new();
	ewl_container_append_child(EWL_CONTAINER(box_box), hbox[1]);
	ewl_widget_show(hbox[1]);

	/*
	 * Create and setup a button with top alignment by default.
	 */
	hbox_button[0][0] = ewl_button_new("Top");
	ewl_container_append_child(EWL_CONTAINER(hbox[1]), hbox_button[0][0]);
	ewl_object_set_fill_policy(EWL_OBJECT(hbox_button[0][0]),
				   EWL_FILL_POLICY_NONE);
	ewl_object_set_alignment(EWL_OBJECT(hbox_button[0][0]),
				 EWL_ALIGNMENT_TOP);
	ewl_callback_append(hbox_button[0][0], EWL_CALLBACK_CLICKED,
			    __toggle_child_vertical_align, NULL);
	ewl_widget_show(hbox_button[0][0]);

	/*
	 * Create and setup a button with center alignment by default.
	 */
	hbox_button[0][1] = ewl_button_new("Center");
	ewl_container_append_child(EWL_CONTAINER(hbox[1]), hbox_button[0][1]);
	ewl_object_set_fill_policy(EWL_OBJECT(hbox_button[0][1]),
				   EWL_FILL_POLICY_NONE);
	ewl_object_set_alignment(EWL_OBJECT(hbox_button[0][1]),
				 EWL_ALIGNMENT_CENTER);
	ewl_callback_append(hbox_button[0][1], EWL_CALLBACK_CLICKED,
			    __toggle_child_vertical_align, NULL);
	ewl_widget_show(hbox_button[0][1]);

	/*
	 * Create and setup a button with bottom alignment by default.
	 */
	hbox_button[0][2] = ewl_button_new("Bottom");
	ewl_container_append_child(EWL_CONTAINER(hbox[1]), hbox_button[0][2]);
	ewl_object_set_fill_policy(EWL_OBJECT(hbox_button[0][2]),
				   EWL_FILL_POLICY_NONE);
	ewl_object_set_alignment(EWL_OBJECT(hbox_button[0][2]),
				 EWL_ALIGNMENT_BOTTOM);
	ewl_callback_append(hbox_button[0][2], EWL_CALLBACK_CLICKED,
			    __toggle_child_vertical_align, NULL);
	ewl_widget_show(hbox_button[0][2]);

	/****************************************************************/
	/* Create a box for holding the Shrink test buttons             */

	/****************************************************************/

	hbox[2] = ewl_hbox_new();
	ewl_container_append_child(EWL_CONTAINER(box_box), hbox[2]);
	ewl_widget_show(hbox[2]);

	/*
	 * Create and setup a button with no filling by default.
	 */
	hbox_button[1][0] =
		ewl_button_new("Shrink This Box To Fit It's Parent");
	ewl_container_append_child(EWL_CONTAINER(hbox[2]), hbox_button[1][0]);
	ewl_object_set_fill_policy(EWL_OBJECT(hbox_button[1][0]),
				   EWL_FILL_POLICY_SHRINK);
	ewl_callback_append(hbox_button[1][0], EWL_CALLBACK_CLICKED,
			    __toggle_child_shrink, NULL);
	ewl_widget_show(hbox_button[1][0]);

	/*
	 * Create and setup a button with shrinking by default.
	 */
	hbox_button[1][1] =
		ewl_button_new("Shrink This Box To Fit It's Parent");
	ewl_container_append_child(EWL_CONTAINER(hbox[2]), hbox_button[1][1]);
	ewl_object_set_fill_policy(EWL_OBJECT(hbox_button[1][1]),
				   EWL_FILL_POLICY_SHRINK);
	ewl_callback_append(hbox_button[1][1], EWL_CALLBACK_CLICKED,
			    __toggle_child_shrink, NULL);
	ewl_widget_show(hbox_button[1][1]);

	/*
	 * Create and setup a button with no filling by default.
	 */
	hbox_button[1][2] = ewl_button_new("Don't shrink this box at all");
	ewl_container_append_child(EWL_CONTAINER(hbox[2]), hbox_button[1][2]);
	ewl_object_set_fill_policy(EWL_OBJECT(hbox_button[1][2]),
				   EWL_FILL_POLICY_NONE);
	ewl_callback_append(hbox_button[1][2], EWL_CALLBACK_CLICKED,
			    __toggle_child_shrink, NULL);
	ewl_widget_show(hbox_button[1][2]);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}
