#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @return Returns a new scrollpane on success, NULL on failure.
 * @brief Create a new scrollpane
 */
Ewl_Widget *
ewl_scrollpane_new(void)
{
	Ewl_Scrollpane *s;

	DENTER_FUNCTION(DLEVEL_UNSTABLE);

	s = NEW(Ewl_Scrollpane, 1);
	if (!s)
		DRETURN_PTR(NULL, DLEVEL_UNSTABLE);

	if (!ewl_scrollpane_init(s)) {
		ewl_widget_destroy(EWL_WIDGET(s));
		s = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_UNSTABLE);
}

/**
 * @param s: the scrollpane to initialize
 * @return Returns no value.
 * @brief Initialize the fields of a scrollpane
 *
 * Sets up default callbacks and field values for the scrollpane @a s.
 */
int
ewl_scrollpane_init(Ewl_Scrollpane *s)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_UNSTABLE);
	DCHECK_PARAM_PTR_RET("s", s, FALSE);

	w = EWL_WIDGET(s);

	if (!ewl_container_init(EWL_CONTAINER(s)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(w, EWL_SCROLLPANE_TYPE);
	ewl_widget_inherit(w, EWL_SCROLLPANE_TYPE);
	ewl_widget_focusable_set(EWL_WIDGET(s), TRUE);
	ewl_object_fill_policy_set(EWL_OBJECT(s), EWL_FLAG_FILL_ALL);

	ewl_container_show_notify_set(EWL_CONTAINER(s),
					ewl_scrollpane_child_resize_cb);
	ewl_container_resize_notify_set(EWL_CONTAINER(s),
					(Ewl_Child_Resize)
					ewl_scrollpane_child_resize_cb);
	ewl_container_hide_notify_set(EWL_CONTAINER(s),
					ewl_scrollpane_child_resize_cb);

	ewl_container_callback_notify(EWL_CONTAINER(s), EWL_CALLBACK_FOCUS_IN);
	ewl_container_callback_notify(EWL_CONTAINER(s), EWL_CALLBACK_FOCUS_OUT);

	s->hflag = EWL_SCROLLPANE_FLAG_AUTO_VISIBLE;
	s->vflag = EWL_SCROLLPANE_FLAG_AUTO_VISIBLE;

	s->overlay = ewl_overlay_new();
	ewl_object_fill_policy_set(EWL_OBJECT(s->overlay), EWL_FLAG_FILL_ALL);

	/*
	 * Create the container to hold the contents and it's configure
	 * callback to position it's child.
	 */
	s->box = ewl_vbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(s->box), EWL_FLAG_FILL_FILL);

	/*
	 * Create the scrollbars for the scrollpane.
	 */
	s->hscrollbar = ewl_hscrollbar_new();
	s->vscrollbar = ewl_vscrollbar_new();

	/*
	 * Add the parts to the scrollpane
	 */
	ewl_container_child_append(EWL_CONTAINER(s), s->overlay);
	ewl_container_child_append(EWL_CONTAINER(s->overlay), s->box);
	ewl_container_child_append(EWL_CONTAINER(s), s->hscrollbar);
	ewl_container_child_append(EWL_CONTAINER(s), s->vscrollbar);

	ewl_widget_internal_set(s->overlay, TRUE);
	ewl_widget_internal_set(s->box, TRUE);
	ewl_widget_internal_set(s->hscrollbar, TRUE);
	ewl_widget_internal_set(s->vscrollbar, TRUE);

	ewl_widget_show(s->overlay);
	ewl_widget_show(s->box);
	ewl_widget_show(s->hscrollbar);
	ewl_widget_show(s->vscrollbar);

	ewl_container_redirect_set(EWL_CONTAINER(s), EWL_CONTAINER(s->box));

	/*
	 * Append necessary callbacks for the scrollpane.
	 */
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			ewl_scrollpane_configure_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_FOCUS_IN,
			ewl_scrollpane_focus_jump_cb, NULL);

	/*
	 * We need to know whent he scrollbars have value changes in order to
	 * know when to scroll.
	 */
	ewl_callback_append(s->hscrollbar, EWL_CALLBACK_VALUE_CHANGED,
					ewl_scrollpane_hscroll_cb, s);
	ewl_callback_append(s->vscrollbar, EWL_CALLBACK_VALUE_CHANGED,
					ewl_scrollpane_vscroll_cb, s);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_WHEEL,
				ewl_scrollpane_wheel_scroll_cb, NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param s: the scrollpane that contains the scrollbar to change
 * @param f: the flags to set on the horizontal scrollbar in @a s
 * @return Returns no value.
 * @brief Set flags for horizontal scrollbar
 *
 * The scrollbar flags for the horizontal scrollbar are set to @a f.
 */
void
ewl_scrollpane_hscrollbar_flag_set(Ewl_Scrollpane *s, Ewl_Scrollpane_Flags f)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_TYPE("s", s, EWL_SCROLLPANE_TYPE);

	s->hflag = f;

	if (f & EWL_SCROLLPANE_FLAG_ALWAYS_HIDDEN) {
		unsigned int fill;
		fill = ewl_object_fill_policy_get(EWL_OBJECT(s->box));
		ewl_object_fill_policy_set(EWL_OBJECT(s->box),
				fill | EWL_FLAG_FILL_HSHRINK);
	}

	ewl_widget_configure(EWL_WIDGET(s));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the scrollpane that contains the scrollbar to change
 * @param f: the flags to set on the vertical scrollbar in @a s
 * @return Returns no value.
 * @brief Set flags for vertical scrollbar
 *
 * The scrollbar flags for the vertical scrollbar are set to @a f.
 */
void
ewl_scrollpane_vscrollbar_flag_set(Ewl_Scrollpane *s, Ewl_Scrollpane_Flags f)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_TYPE("s", s, EWL_SCROLLPANE_TYPE);

	s->vflag = f;

	if (f & EWL_SCROLLPANE_FLAG_ALWAYS_HIDDEN) {
		unsigned int fill;
		fill = ewl_object_fill_policy_get(EWL_OBJECT(s->box));
		ewl_object_fill_policy_set(EWL_OBJECT(s->box),
				fill | EWL_FLAG_FILL_VSHRINK);
	}

	ewl_widget_configure(EWL_WIDGET(s));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the scrollpane that contains the scrollbar to retrieve
 * @return Returns the flags of the horizontal scrollbar, 0 on failure.
 * @brief Get flags for horizontal scrollbar
 */
Ewl_Scrollpane_Flags
ewl_scrollpane_hscrollbar_flag_get(Ewl_Scrollpane *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, 0);
	DCHECK_TYPE_RET("s", s, EWL_SCROLLPANE_TYPE, 0);

	DRETURN_INT(s->hflag, DLEVEL_STABLE);
}

/**
 * @param s: the scrollpane that contains the scrollbar to retrieve
 * @return Returns the flags of the vertical scrollbar on success, 0 on failure.
 * @brief Get flags for vertical scrollbar
 */
Ewl_Scrollpane_Flags
ewl_scrollpane_vscrollbar_flag_get(Ewl_Scrollpane *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, 0);
	DCHECK_TYPE_RET("s", s, EWL_SCROLLPANE_TYPE, 0);

	DRETURN_INT(s->vflag, DLEVEL_STABLE);
}

/**
 * @param s: the scrollpane to retrieve it's horizontal scrollbar value
 * @return Returns the value of the horizontal scrollbar in @a s on success.
 * @brief Retrieves the value of the horizontal scrollbar in @a s.
 */
double
ewl_scrollpane_hscrollbar_value_get(Ewl_Scrollpane *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, 0.0);
	DCHECK_TYPE_RET("s", s, EWL_SCROLLPANE_TYPE, 0.0);

	DRETURN_FLOAT(ewl_scrollbar_value_get(EWL_SCROLLBAR(s->hscrollbar)),
								DLEVEL_STABLE);
}

/**
 * @param s: the scrollpane to retrieve it's vertical scrollbar value
 * @return Returns the value of the vertical scrollbar in @a s on success.
 * @brief Retrieves the value of the vertical scrollbar in @a s.
 */
double
ewl_scrollpane_vscrollbar_value_get(Ewl_Scrollpane *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, 0.0);
	DCHECK_TYPE_RET("s", s, EWL_SCROLLPANE_TYPE, 0.0);

	DRETURN_FLOAT(ewl_scrollbar_value_get(EWL_SCROLLBAR(s->vscrollbar)),
								DLEVEL_STABLE);
}

/**
 * @param s: the scrollpane to set the horizontal scrollbar value
 * @param val: the value to set the scrollbar too
 * @return Returns nothing
 * @brief Set the value of the horizontal scrollbar in @a s to @a val
 */
void
ewl_scrollpane_hscrollbar_value_set(Ewl_Scrollpane *s, double val)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_TYPE("s", s, EWL_SCROLLPANE_TYPE);

	ewl_scrollbar_value_set(EWL_SCROLLBAR(s->hscrollbar), val);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the scrollpane to set the vertical scrollbar value
 * @param val: the value to set the scrollbar too
 * @return Returns nothing
 * @brief Set the value of the vertical scrollbar in @a s to @a val
 */
void
ewl_scrollpane_vscrollbar_value_set(Ewl_Scrollpane *s, double val)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_TYPE("s", s, EWL_SCROLLPANE_TYPE);

	ewl_scrollbar_value_set(EWL_SCROLLBAR(s->vscrollbar), val);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the scrollpane to retrieve its vertical scrollbar stepping 
 * @return Returns the value of the stepping of the vertical scrollbar 
 *         in @a s on success.
 * @brief Retrives the value of the stepping of the vertical scrollbar in @a s.
 */
double
ewl_scrollpane_hscrollbar_step_get(Ewl_Scrollpane *s) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, 0.0);
	DCHECK_TYPE_RET("s", s, EWL_SCROLLPANE_TYPE, 0.0);

	DRETURN_FLOAT(ewl_scrollbar_step_get(EWL_SCROLLBAR(s->hscrollbar)),
								DLEVEL_STABLE);
}

/**
 * @param s: the scrollpane to retrieve its vertical scrollbar stepping 
 * @return Returns the value of the stepping of the vertical scrollbar 
 *         in @a s on success.
 * @brief Retrives the value of the stepping of the vertical scrollbar in @a s.
 */
double
ewl_scrollpane_vscrollbar_step_get(Ewl_Scrollpane *s) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, 0.0);
	DCHECK_TYPE_RET("s", s, EWL_SCROLLPANE_TYPE, 0.0);

	DRETURN_FLOAT(ewl_scrollbar_step_get(EWL_SCROLLBAR(s->vscrollbar)),
								DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Move the contents of the scrollbar into place
 */
void
ewl_scrollpane_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Scrollpane *s;
	int vs_width = 0;
	int hs_height = 0;
	int b_width, b_height;
	int content_w, content_h;
	unsigned int old_fill, box_fill = EWL_FLAG_FILL_FILL;
	double hstep = 1.0, vstep = 1.0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	s = EWL_SCROLLPANE(w);

	/*
	 * Get the space needed by the scrolbars.
	 */
	vs_width = ewl_object_preferred_w_get(EWL_OBJECT(s->vscrollbar));
	hs_height = ewl_object_preferred_h_get(EWL_OBJECT(s->hscrollbar));

	/*
	 * Determine the space used by the contents.
	 */
	content_w = CURRENT_W(w);
	content_h = CURRENT_H(w);

	/*
	 * FIXME: This is exposing box internals, should probably just make a
	 * dumb box for the scrollpane.
	 * Force the box to recalculate preferred size to work around children
	 * with shrink fill policies.
	 */
	ewl_container_largest_prefer(EWL_CONTAINER(s->box),
					EWL_ORIENTATION_HORIZONTAL);
	ewl_container_sum_prefer(EWL_CONTAINER(s->box),
					EWL_ORIENTATION_VERTICAL);

	/*
	 * Get the preferred size of contents to scroll correctly.
	 */
	b_width = ewl_object_preferred_w_get(EWL_OBJECT(s->box));
	b_height = ewl_object_preferred_h_get(EWL_OBJECT(s->box));

	/*
	 * Calculate initial steps.
	 */
	if (content_w < b_width)
		hstep = (double)content_w / (double)b_width;
	if (content_h < b_height)
		vstep = (double)content_h / (double)b_height;

	/*
	 * Determine visibility of scrollbars based on the flags.
	 */
	if (s->hflag == EWL_SCROLLPANE_FLAG_NONE ||
			(hstep < 1.0 &&
			 s->hflag == EWL_SCROLLPANE_FLAG_AUTO_VISIBLE))
		ewl_widget_show(s->hscrollbar);
	else {
		box_fill |= EWL_FLAG_FILL_HSHRINK;
		ewl_widget_hide(s->hscrollbar);
	}

	if (s->vflag == EWL_SCROLLPANE_FLAG_NONE ||
			(vstep < 1.0 &&
			 s->vflag == EWL_SCROLLPANE_FLAG_AUTO_VISIBLE))
		ewl_widget_show(s->vscrollbar);
	else {
		box_fill |= EWL_FLAG_FILL_VSHRINK;
		ewl_widget_hide(s->vscrollbar);
	}


	/*
	 * Adjust the step and width dependant on scrollbar visibility.
	 */
	if (VISIBLE(s->hscrollbar)) {
		content_h -= hs_height;
		if (content_h < b_height)
			vstep = (double)content_h / (double)b_height;
	}

	if (VISIBLE(s->vscrollbar)) {
		content_w -= vs_width;
		if (content_w < b_width)
			hstep = (double)content_w / (double)b_width;
	}

	/*
	 * Ensure the step is not negative.
	 */
	if (hstep == 1.0)
		b_width = content_w;

	if (vstep == 1.0)
		b_height = content_h;

	/*
	 * Calcuate the offset for the box position
	 */
	b_width = (int)(ewl_scrollbar_value_get(EWL_SCROLLBAR(s->hscrollbar)) *
			(double)(b_width - content_w));
	b_height = (int)(ewl_scrollbar_value_get(EWL_SCROLLBAR(s->vscrollbar)) *
			(double)(b_height - content_h));

	/*
	 * Assign the step values to the scrollbars to adjust scale.
	 */
	ewl_scrollbar_step_set(EWL_SCROLLBAR(s->hscrollbar), hstep);
	ewl_scrollbar_step_set(EWL_SCROLLBAR(s->vscrollbar), vstep);

	/*
	 * Set the fill policy on the box based on scrollbars visible.
	 */
	old_fill = ewl_object_fill_policy_get(EWL_OBJECT(s->box));
	ewl_object_fill_policy_set(EWL_OBJECT(s->box), box_fill);

	/*
	 * Position the horizontal scrollbar.
	 */
	ewl_object_geometry_request(EWL_OBJECT(s->hscrollbar),
					CURRENT_X(w), CURRENT_Y(w) + content_h,
					content_w, hs_height);

	/*
	 * Position the vertical scrollbar.
	 */
	ewl_object_geometry_request(EWL_OBJECT(s->vscrollbar),
					CURRENT_X(w) + content_w, CURRENT_Y(w),
					vs_width, content_h);

	/*
	 * Now move the box into position. For the scrollpane to work we move
	 * the box relative to the scroll value.
	 */
	ewl_object_geometry_request(EWL_OBJECT(s->overlay),
					CURRENT_X(w), CURRENT_Y(w),
					content_w, content_h);
	ewl_object_geometry_request(EWL_OBJECT(s->box),
					CURRENT_X(w) - b_width,
					CURRENT_Y(w) - b_height,
					content_w + b_width, 
					content_h + b_height);

	/*
	 * Reset the default fill policy on the box to get updated sizes..
	 */
	ewl_object_fill_policy_set(EWL_OBJECT(s->box), old_fill);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The focus jump callback
 */
void
ewl_scrollpane_focus_jump_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	int endcoord = 0;
	double value;
	int fx, fy, fw, fh;
	Ewl_Embed *emb;
	Ewl_Widget *focus;
	Ewl_Widget *bar = NULL;
	Ewl_Scrollpane *s = EWL_SCROLLPANE(w);

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_SCROLLPANE_TYPE);

	emb = ewl_embed_widget_find(w);
	if (!emb)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Get the focused widget and stop if its an internal one.
	 */
	focus = ewl_embed_focused_widget_get(emb);
	if (!focus || !ewl_widget_parent_of(s->box, focus) ||
			ewl_widget_onscreen_is(focus))
		DRETURN(DLEVEL_STABLE);

	ewl_object_current_geometry_get(EWL_OBJECT(focus), &fx, &fy, &fw, &fh);

	/*
	 * Adjust horizontally to show the focused widget
	 */
	if (fx < CURRENT_X(s->overlay)) {
		bar = s->hscrollbar;
		endcoord = fx;
	}
	else if (fx + fw > CURRENT_X(s->overlay) + CURRENT_W(s->overlay)) {
		bar = s->hscrollbar;
		endcoord = fx + fw;
	}

	if (bar) {
		value = (double)endcoord /
			(double)(ewl_object_current_x_get(EWL_OBJECT(s->box)) +
				 ewl_object_preferred_w_get(EWL_OBJECT(s->box)));
		ewl_scrollbar_value_set(EWL_SCROLLBAR(bar), value);
	}

	/*
	 * Adjust vertically to show the focused widget
	 */
	if (fy < CURRENT_Y(s->overlay)) {
		bar = s->vscrollbar;
		endcoord = fy;
	}
	else if (fy + fh > CURRENT_Y(s->overlay) + CURRENT_H(s->overlay)) {
		bar = s->vscrollbar;
		endcoord = fy + fh;
	}

	/*
	 * Adjust the value of the scrollbar to jump to the position
	 */
	if (bar) {
		value = (double)endcoord /
			(double)(ewl_object_current_y_get(EWL_OBJECT(s->box)) +
				 ewl_object_preferred_h_get(EWL_OBJECT(s->box)));
		ewl_scrollbar_value_set(EWL_SCROLLBAR(bar), value);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev_data: UNUSED
 * @param user_data: The scrollbar
 * @return Returns no value
 * @brief When a horizontal scrollbar is clicked we need to move the 
 * contents of the scrollpane horizontally.
 */
void
ewl_scrollpane_hscroll_cb(Ewl_Widget *w __UNUSED__,
		void *ev_data __UNUSED__, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);

	ewl_callback_call(user_data, EWL_CALLBACK_VALUE_CHANGED);
	ewl_widget_configure(user_data);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 */
/**
 * @internal
 * @param w: UNUSED
 * @param ev_data: UNUSED
 * @param user_data: The scrollbar
 * @return Returns no value
 * @brief When a vertical scrollbar is clicked we need to move the 
 * contents of the scrollpane vertically.
 */
void
ewl_scrollpane_vscroll_cb(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
						void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);

	ewl_callback_call(user_data, EWL_CALLBACK_VALUE_CHANGED);
	ewl_widget_configure(user_data);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param cb: The widget to work with
 * @param ev_data: The Ewl_Event_Mouse_Wheel data
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The wheel scroll callback
 */
void
ewl_scrollpane_wheel_scroll_cb(Ewl_Widget *cb, void *ev_data,
				void *user_data __UNUSED__)
{
	Ewl_Scrollpane *s;
	Ewl_Event_Mouse_Wheel *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cb", cb);
	DCHECK_TYPE("cb", cb, EWL_WIDGET_TYPE);

	s = EWL_SCROLLPANE(cb);
	ev = ev_data;
	ewl_scrollpane_vscrollbar_value_set(s,
			ewl_scrollpane_vscrollbar_value_get(s) +
			ev->z * ewl_scrollpane_vscrollbar_step_get(s));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param parent: The container to work with
 * @param child: The widget to work with
 * @return Returns no value
 * @brief This handles all of the various size affecting callbacks.
 */
void
ewl_scrollpane_child_resize_cb(Ewl_Container *parent, Ewl_Widget *child)
{
	int pw, ph;
	Ewl_Scrollpane *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("parent", parent);
	DCHECK_PARAM_PTR("child", child);
	DCHECK_TYPE("parent", parent, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("child", child, EWL_WIDGET_TYPE);

	s = EWL_SCROLLPANE(parent);

	pw = ewl_object_preferred_w_get(EWL_OBJECT(s->vscrollbar)) +
			ewl_object_preferred_w_get(EWL_OBJECT(s->box));
	ph = ewl_object_preferred_h_get(EWL_OBJECT(s->hscrollbar)) +
			ewl_object_preferred_h_get(EWL_OBJECT(s->box));

	ewl_object_preferred_inner_w_set(EWL_OBJECT(parent), pw);
	ewl_object_preferred_inner_h_set(EWL_OBJECT(parent), ph);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

