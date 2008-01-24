/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include "ewl_base.h"
#include "ewl_overlay.h"
#include "ewl_scrollpane.h"
#include "ewl_box.h"
#include "ewl_scrollbar.h"
#include "ewl_range.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

void ewl_scrollpane_cb_mouse_down(Ewl_Widget *w, void *ev, void *data);
void ewl_scrollpane_cb_mouse_up(Ewl_Widget *w, void *ev, void *data);
void ewl_scrollpane_cb_mouse_move(Ewl_Widget *w, void *ev, void *data);
static int ewl_scrollpane_cb_scroll_timer(void *data);
void ewl_scrollpane_cb_scroll(Ewl_Scrollpane *s, double x, double y,
							int *tx, int *ty);
void ewl_scrollpane_cb_destroy(Ewl_Widget *w, void *ev, void *data);

/**
 * @return Returns a new scrollpane on success, NULL on failure.
 * @brief Create a new scrollpane
 */
Ewl_Widget *
ewl_scrollpane_new(void)
{
	Ewl_Scrollpane *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = NEW(Ewl_Scrollpane, 1);
	if (!s)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_scrollpane_init(s)) {
		ewl_widget_destroy(EWL_WIDGET(s));
		s = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
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

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET(s, FALSE);

	w = EWL_WIDGET(s);

	if (!ewl_container_init(EWL_CONTAINER(s)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(w, EWL_SCROLLPANE_TYPE);
	ewl_widget_inherit(w, EWL_SCROLLPANE_TYPE);
	ewl_widget_focusable_set(EWL_WIDGET(s), TRUE);
	ewl_object_fill_policy_set(EWL_OBJECT(s), EWL_FLAG_FILL_ALL);

	ewl_container_callback_notify(EWL_CONTAINER(s), EWL_CALLBACK_FOCUS_IN);
	ewl_container_callback_notify(EWL_CONTAINER(s), EWL_CALLBACK_FOCUS_OUT);

	/* Remove the default focus out callback and replace with our own */
	ewl_callback_del(w, EWL_CALLBACK_FOCUS_OUT, ewl_widget_cb_focus_out);
	ewl_callback_append(w, EWL_CALLBACK_FOCUS_OUT,
				ewl_container_cb_container_focus_out, NULL);


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
			ewl_scrollpane_cb_configure, NULL);
	ewl_callback_append(w, EWL_CALLBACK_FOCUS_IN,
			ewl_scrollpane_cb_focus_jump, NULL);

	/*
	 * We need to know whent he scrollbars have value changes in order to
	 * know when to scroll.
	 */
	ewl_callback_append(s->hscrollbar, EWL_CALLBACK_VALUE_CHANGED,
					ewl_scrollpane_cb_hscroll, s);
	ewl_callback_append(s->vscrollbar, EWL_CALLBACK_VALUE_CHANGED,
					ewl_scrollpane_cb_vscroll, s);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_WHEEL,
				ewl_scrollpane_cb_wheel_scroll, NULL);

	/*
	 * Setup kinetic scrolling info here
	 */
	ewl_scrollpane_kinetic_scrolling_set(s, EWL_KINETIC_SCROLL_NONE);
	ewl_callback_append(w, EWL_CALLBACK_DESTROY,
				ewl_scrollpane_cb_destroy, NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param s: The scrollpane to setup
 * @param use: To use kinetic scrolling or not
 * @return Returns no value
 * @brief Sets up default values and callbacks for kinetic scrolling
 */
void
ewl_scrollpane_kinetic_scrolling_set(Ewl_Scrollpane *s, Ewl_Kinetic_Scroll type)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(s);
	DCHECK_TYPE(s, EWL_SCROLLPANE_TYPE);

	/* If set to current value we have nothing to do */
	if ((s->type) && (type == s->type))
		DRETURN(DLEVEL_STABLE);

	if (type == EWL_KINETIC_SCROLL_NORMAL)
	{
		ewl_callback_append(s->overlay, EWL_CALLBACK_MOUSE_DOWN,
				ewl_scrollpane_cb_mouse_down, s);
		ewl_callback_append(s->overlay, EWL_CALLBACK_MOUSE_UP,
				ewl_scrollpane_cb_mouse_up, s);
		ewl_callback_append(s->overlay, EWL_CALLBACK_MOUSE_MOVE,
				ewl_scrollpane_cb_mouse_move, s);
	}

	/* Only delete the callbacks if they were there originally */
	else if ((s->type != EWL_KINETIC_SCROLL_NONE) && (s->kinfo))
	{
		ewl_callback_del(s->overlay, EWL_CALLBACK_MOUSE_DOWN,
			       	ewl_scrollpane_cb_mouse_down);
		ewl_callback_del(s->overlay, EWL_CALLBACK_MOUSE_UP,
			       	ewl_scrollpane_cb_mouse_up);
		ewl_callback_del(s->overlay, EWL_CALLBACK_MOUSE_MOVE,
			       	ewl_scrollpane_cb_mouse_move);
	}

	/* Set up the scrollpane information */
	if (!s->kinfo)
	{
		s->kinfo = NEW(Ewl_Scrollpane_Scroll_Info, 1);
		s->kinfo->fps = 15;
		s->kinfo->vmax = 50.0;
		s->kinfo->vmin = 0.0;
		s->kinfo->dampen = 0.95;
	}

	s->type = type;
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: The scrollpane to use
 * @return Returns the type of scrolling used
 * @brief Gets the type of kinetic scrolling used
 */
Ewl_Kinetic_Scroll
ewl_scrollpane_kinetic_scrolling_get(Ewl_Scrollpane *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET(s, EWL_KINETIC_SCROLL_NONE);
	DCHECK_TYPE_RET(s, EWL_SCROLLPANE_TYPE, EWL_KINETIC_SCROLL_NONE);

	DRETURN_INT(s->type, DLEVEL_STABLE);
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
	DCHECK_PARAM_PTR(s);
	DCHECK_TYPE(s, EWL_SCROLLPANE_TYPE);

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
	DCHECK_PARAM_PTR(s);
	DCHECK_TYPE(s, EWL_SCROLLPANE_TYPE);

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
	DCHECK_PARAM_PTR_RET(s, 0);
	DCHECK_TYPE_RET(s, EWL_SCROLLPANE_TYPE, 0);

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
	DCHECK_PARAM_PTR_RET(s, 0);
	DCHECK_TYPE_RET(s, EWL_SCROLLPANE_TYPE, 0);

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
	DCHECK_PARAM_PTR_RET(s, 0.0);
	DCHECK_TYPE_RET(s, EWL_SCROLLPANE_TYPE, 0.0);

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
	DCHECK_PARAM_PTR_RET(s, 0.0);
	DCHECK_TYPE_RET(s, EWL_SCROLLPANE_TYPE, 0.0);

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
	DCHECK_PARAM_PTR(s);
	DCHECK_TYPE(s, EWL_SCROLLPANE_TYPE);

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
	DCHECK_PARAM_PTR(s);
	DCHECK_TYPE(s, EWL_SCROLLPANE_TYPE);

	ewl_scrollbar_value_set(EWL_SCROLLBAR(s->vscrollbar), val);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the scrollpane to retrieve its vertical scrollbar stepping
 * @return Returns the value of the stepping of the vertical scrollbar
 *		in @a s on success.
 * @brief Retrives the value of the stepping of the vertical scrollbar in @a s.
 */
double
ewl_scrollpane_hscrollbar_step_get(Ewl_Scrollpane *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET(s, 0.0);
	DCHECK_TYPE_RET(s, EWL_SCROLLPANE_TYPE, 0.0);

	DRETURN_FLOAT(ewl_scrollbar_step_get(EWL_SCROLLBAR(s->hscrollbar)),
								DLEVEL_STABLE);
}

/**
 * @param s: the scrollpane to retrieve its vertical scrollbar stepping
 * @return Returns the value of the stepping of the vertical scrollbar
 *		in @a s on success.
 * @brief Retrives the value of the stepping of the vertical scrollbar in @a s.
 */
double
ewl_scrollpane_vscrollbar_step_get(Ewl_Scrollpane *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET(s, 0.0);
	DCHECK_TYPE_RET(s, EWL_SCROLLPANE_TYPE, 0.0);

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
ewl_scrollpane_cb_configure(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Scrollpane *s;
	int vs_width = 0, hs_height = 0;
	int b_width, b_height;
	int content_w, content_h;
	unsigned int old_fill, box_fill = EWL_FLAG_FILL_FILL;
	double hstep = 1.0, vstep = 1.0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(w);
	DCHECK_TYPE(w, EWL_WIDGET_TYPE);

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
ewl_scrollpane_cb_focus_jump(Ewl_Widget *w, void *ev_data __UNUSED__,
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
	DCHECK_PARAM_PTR(w);
	DCHECK_TYPE(w, EWL_SCROLLPANE_TYPE);

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
ewl_scrollpane_cb_hscroll(Ewl_Widget *w __UNUSED__,
		void *ev_data __UNUSED__, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(user_data);

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
ewl_scrollpane_cb_vscroll(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
						void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(user_data);

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
ewl_scrollpane_cb_wheel_scroll(Ewl_Widget *cb, void *ev_data,
				void *user_data __UNUSED__)
{
	Ewl_Scrollpane *s;
	Ewl_Event_Mouse_Wheel *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(cb);
	DCHECK_TYPE(cb, EWL_WIDGET_TYPE);

	s = EWL_SCROLLPANE(cb);
	ev = ev_data;
	ewl_scrollpane_vscrollbar_value_set(s,
			ewl_scrollpane_vscrollbar_value_get(s) +
			ev->z * ewl_scrollpane_vscrollbar_step_get(s));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @ev_data: The Ewl_Event_Mouse_Down data
 * @param data: UNUSED
 * @return Returns no value
 * @brief The mouse down setting up kinetic scrolling
 */
void
ewl_scrollpane_cb_mouse_down(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Scrollpane *s;
	Ewl_Event_Mouse *md;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(ev);
	DCHECK_PARAM_PTR(data);
	DCHECK_TYPE(w, EWL_WIDGET_TYPE);

	s = EWL_SCROLLPANE(data);
	md = EWL_EVENT_MOUSE(ev);
	s->kinfo->vel_x = 0.0;
	s->kinfo->vel_y = 0.0;
	s->kinfo->x = md->x;
	s->kinfo->y = md->y;
	s->kinfo->clicked = !!TRUE;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @ev_data: The Ewl_Event_Mouse_Move data
 * @param data: UNUSED
 * @return Returns no value
 * @brief The mouse move callback for kinetic scrolling
 */
void
ewl_scrollpane_cb_mouse_move(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Scrollpane *s;
	Ewl_Event_Mouse *mm;
	int cx, cy;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(ev);
	DCHECK_PARAM_PTR(data);

	s = EWL_SCROLLPANE(data);
	mm = EWL_EVENT_MOUSE(ev);

	if (!s->kinfo->clicked)
		DRETURN(DLEVEL_STABLE);

	if (!s->kinfo->active)
	{
		ecore_timer_add(1.0/s->kinfo->fps,
					ewl_scrollpane_cb_scroll_timer, s);
		s->kinfo->active = !!TRUE;
	}

	s->kinfo->xc = mm->x;
	s->kinfo->yc = mm->y;
	cx = (s->kinfo->xc - s->kinfo->x);
	cy = (s->kinfo->yc - s->kinfo->y);

	/* v = (change in position / (width or height of scroll *
	 *	(range of velocities) + min))
	 */
	s->kinfo->vel_x = ((cx / 
		(double)ewl_object_current_w_get(EWL_OBJECT(w))) *
		(s->kinfo->vmax - s->kinfo->vmin)) + s->kinfo->vmin;

	s->kinfo->vel_y = ((cy /
		(double)ewl_object_current_h_get(EWL_OBJECT(w))) *
		(s->kinfo->vmax - s->kinfo->vmin)) + s->kinfo->vmin;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @ev_data: The Ewl_Event_Mouse_Up data
 * @param data: UNUSED
 * @return Returns no value
 * @brief The mouse up callback for kinetic scrolling
 */
void
ewl_scrollpane_cb_mouse_up(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Scrollpane *s;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(data);

	s = EWL_SCROLLPANE(data);
	s->kinfo->clicked = !!FALSE;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @internal
 * @param data: The scrollpane to work with
 * @return Returns 1 if the timer is to continue, 0 otherwise
 * @brief Performs some calculations then calls the scroll function
 */
static int ewl_scrollpane_cb_scroll_timer(void *data)
{
	Ewl_Scrollpane *s;
	double h, w;
	int tx = 0, ty = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET(data, FALSE);

	s = EWL_SCROLLPANE(data);

	/* If the mouse is down, accelerate and check velocity */
	if (!s->kinfo->clicked)
	{
		s->kinfo->vel_x *= s->kinfo->dampen;
		s->kinfo->vel_y *= s->kinfo->dampen;

		h = s->kinfo->vel_y * ((s->kinfo->vel_y < 0) ? -1 : 1);
		w = s->kinfo->vel_x * ((s->kinfo->vel_x < 0) ? -1 : 1);

		if ((w < 0.5) && (h < 0.5))
		{
			s->kinfo->active = !!FALSE;
			DRETURN_INT(0, DLEVEL_STABLE);
		}
	}

	/* Actually scroll the pane */
	ewl_scrollpane_cb_scroll(s, s->kinfo->vel_x, s->kinfo->vel_y, &tx, &ty);

	/* If at the end of a scrollbar, set x/y to current */
	if (!tx)
		s->kinfo->x = s->kinfo->xc;
	if (!ty)
		s->kinfo->y = s->kinfo->yc;

	DRETURN_INT(1, DLEVEL_STABLE);
}

/**
 * @internal
 * @param s: The scrollpane to work with
 * @param x: The horizontal velocity
 * @param y: The vertical velocity
 * @param tx: Pointer to integer tested
 * @param ty: Pointer to integer tested
 * @return Returns no value
 * @brief Scrolls the scrollpane based on the given parameters
 */
void
ewl_scrollpane_cb_scroll(Ewl_Scrollpane *s, double x, double y,
						int *tx, int *ty)
{
	double w, h;
	Ewl_Scrollbar *ry, *rx;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(s);
	DCHECK_TYPE(s, EWL_SCROLLPANE_TYPE);

	ry = EWL_SCROLLBAR(s->vscrollbar);
	rx = EWL_SCROLLBAR(s->hscrollbar);

	if (!((ewl_scrollpane_vscrollbar_value_get(s) == 1.0) &&
				(y > 0)) &&
			!((ewl_scrollpane_vscrollbar_value_get(s) == 0.0) &&
				(y < 0)))
	{
		h = ewl_scrollpane_vscrollbar_value_get(s) + (y *
				ewl_scrollpane_vscrollbar_step_get(s) / 100);

		/* If h is greater than possible setting, set to remainder */
		if (h > ewl_range_maximum_value_get(EWL_RANGE(ry->seeker)))
		{
			h = ewl_range_maximum_value_get(EWL_RANGE(ry->seeker));
			if (ty) *ty = FALSE;
		}
		else if (h < ewl_range_minimum_value_get(EWL_RANGE(ry->seeker)))
		{
			h = ewl_range_minimum_value_get(EWL_RANGE(ry->seeker));
			if (ty) *ty = FALSE;
		}
		else
			if (ty) *ty = TRUE;

		ewl_scrollpane_vscrollbar_value_set(s, h);
	}

	if (!((ewl_scrollpane_hscrollbar_value_get(s) == 1.0) &&
				(x > 0)) &&
			!((ewl_scrollpane_hscrollbar_value_get(s) == 0.0) &&
				(x < 0)))
	{
		w = ewl_scrollpane_hscrollbar_value_get(s) + (x *
				ewl_scrollpane_hscrollbar_step_get(s) / 100);

		/* And again for the w */
		if (w > ewl_range_maximum_value_get(EWL_RANGE(rx->seeker)))
		{
			w = ewl_range_maximum_value_get(EWL_RANGE(rx->seeker));
			if (tx) *tx = FALSE;
		}
		else if (w < ewl_range_minimum_value_get(EWL_RANGE(rx->seeker)))
		{
			w = ewl_range_minimum_value_get(EWL_RANGE(rx->seeker));
			if (tx) *tx = FALSE;
		}
		else
			if (tx) *tx = TRUE;

		ewl_scrollpane_hscrollbar_value_set(s, w);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: The scrollpane to work with
 * @param w: The maximum velocity
 * @return Returns no value
 * @brief Sets the maximum velocity for kinetic scrolling
 */
void
ewl_scrollpane_kinetic_max_velocity_set(Ewl_Scrollpane *s, double v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(s);
	DCHECK_TYPE(s, EWL_SCROLLPANE_TYPE);

	s->kinfo->vmax = v;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: The scrollpane to work with
 * @return Returns the maximum velocity
 * @brief Gets the maximum velocity for kinetic scrolling
 */
double
ewl_scrollpane_kinetic_max_velocity_get(Ewl_Scrollpane *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET(s, -1);
	DCHECK_TYPE_RET(s, EWL_SCROLLPANE_TYPE, -1);

	DRETURN_INT(s->kinfo->vmax, DLEVEL_STABLE);
}

/**
 * @param s: The scrollpane to work with
 * @param w: The minimum velocity
 * @return Returns no value
 * @brief Sets the minimum velocity for kinetic scrolling
 */
void
ewl_scrollpane_kinetic_min_velocity_set(Ewl_Scrollpane *s, double v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(s);
	DCHECK_TYPE(s, EWL_SCROLLPANE_TYPE);

	s->kinfo->vmin = v;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: The scrollpane to work with
 * @return Returns the minimum velocity
 * @brief Gets the minimum velocity for kinetic scrolling
 */
double
ewl_scrollpane_kinetic_min_velocity_get(Ewl_Scrollpane *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET(s, -1);
	DCHECK_TYPE_RET(s, EWL_SCROLLPANE_TYPE, -1);

	DRETURN_INT(s->kinfo->vmin, DLEVEL_STABLE);
}

/**
 * @param s: The scrollpane to work with
 * @param w: The multiplier to reduce velocity
 * @return Returns no value
 * @brief Sets the multiplier to reduce the velocity of kinetic scrolling
 */
void
ewl_scrollpane_kinetic_dampen_set(Ewl_Scrollpane *s, double d)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(s);
	DCHECK_TYPE(s, EWL_SCROLLPANE_TYPE);

	s->kinfo->dampen = d;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: The scrollpane to work with
 * @return Returns the minimum velocity
 * @brief Gets the minimum velocity for kinetic scrolling
 */
double
ewl_scrollpane_kinetic_dampen_get(Ewl_Scrollpane *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET(s, -1);
	DCHECK_TYPE_RET(s, EWL_SCROLLPANE_TYPE, -1);

	DRETURN_INT(s->kinfo->vmin, DLEVEL_STABLE);
}

/**
 * @param s: The scrollpane to work with
 * @param w: The desired frames per second
 * @return Returns no value
 * @brief Sets the number of times per second to recalculate velocity and update the tree
 */
void
ewl_scrollpane_kinetic_fps_set(Ewl_Scrollpane *s, int fps)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(s);
	DCHECK_TYPE(s, EWL_SCROLLPANE_TYPE);

	s->kinfo->fps = fps;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: The scrollpane to work with
 * @return Returns the current frames per second
 * brief Gets the times per second the timer used for scrolling will be called
 */
int
ewl_scrollpane_kinetic_fps_get(Ewl_Scrollpane *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET(s, -1);
	DCHECK_TYPE_RET(s, EWL_SCROLLPANE_TYPE, -1);

	DRETURN_INT(s->kinfo->fps, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to use
 * @parma ev: The event data
 * @param data: User data
 * @return Returns no value
 * @brief Frees data from the scrollpane
 */
void
ewl_scrollpane_cb_destroy(Ewl_Widget *w, void *ev, void *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR(w);
	DCHECK_TYPE(w, EWL_SCROLLPANE_TYPE);

	FREE(EWL_SCROLLPANE(w)->kinfo);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

