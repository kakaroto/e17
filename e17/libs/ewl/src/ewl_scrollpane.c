#include <Ewl.h>

/**
 * @return Returns a new scrollpane on success, NULL on failure.
 * @brief Create a new scrollpane
 */
Ewl_Widget     *ewl_scrollpane_new(void)
{
	Ewl_ScrollPane *s;

	DENTER_FUNCTION(DLEVEL_UNSTABLE);

	s = NEW(Ewl_ScrollPane, 1);
	if (!s)
		DRETURN_PTR(NULL, DLEVEL_UNSTABLE);

	ewl_scrollpane_init(s);

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_UNSTABLE);
}

/**
 * @param s: the scrollpane to initialize
 * @return Returns no value.
 * @brief Initialize the fields of a scrollpane
 *
 * Sets up default callbacks and field values for the scrollpane @a s.
 */
void ewl_scrollpane_init(Ewl_ScrollPane * s)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_UNSTABLE);
	DCHECK_PARAM_PTR("s", s);

	w = EWL_WIDGET(s);

	ewl_container_init(EWL_CONTAINER(s), "scrollpane",
			   ewl_scrollpane_add_cb,
			   ewl_scrollpane_child_resize_cb, NULL);
	ewl_object_set_fill_policy(EWL_OBJECT(s), EWL_FLAG_FILL_FILL |
			EWL_FLAG_FILL_SHRINK);

	/*
	 * Create the container to hold the contents and it's configure
	 * callback to position it's child.
	 */
	s->box = ewl_vbox_new();
	ewl_object_set_fill_policy(EWL_OBJECT(s->box), EWL_FLAG_FILL_SHRINK |
			EWL_FLAG_FILL_FILL);

	/*
	 * Create the scrollbars for the scrollpane.
	 */
	s->hscrollbar = ewl_hscrollbar_new();
	s->vscrollbar = ewl_vscrollbar_new();

	/*
	 * Add the parts to the scrollpane
	 */
	ewl_container_append_child(EWL_CONTAINER(s), s->box);
	ewl_container_append_child(EWL_CONTAINER(s), s->hscrollbar);
	ewl_container_append_child(EWL_CONTAINER(s), s->vscrollbar);

	ewl_widget_show(s->box);
	ewl_widget_show(s->hscrollbar);
	ewl_widget_show(s->vscrollbar);

	/*
	 * Append necessary callbacks for the scrollpane.
	 */
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    ewl_scrollpane_configure_cb, NULL);

	/*
	 * We need to know whent he scrollbars have value changes in order to
	 * know when to scroll.
	 */
	ewl_callback_append(s->hscrollbar, EWL_CALLBACK_VALUE_CHANGED,
			    ewl_scrollpane_hscroll_cb, s);
	ewl_callback_append(s->vscrollbar, EWL_CALLBACK_VALUE_CHANGED,
			    ewl_scrollpane_vscroll_cb, s);

	DLEAVE_FUNCTION(DLEVEL_UNSTABLE);
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
ewl_scrollpane_set_hscrollbar_flag(Ewl_ScrollPane * s, Ewl_ScrollBar_Flags f)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	ewl_scrollbar_set_flag(EWL_SCROLLBAR(s->hscrollbar), f);

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
ewl_scrollpane_set_vscrollbar_flag(Ewl_ScrollPane * s, Ewl_ScrollBar_Flags f)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	ewl_scrollbar_set_flag(EWL_SCROLLBAR(s->vscrollbar), f);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the scrollpane that contains the scrollbar to retrieve
 * @return Returns the flags of the horizontal scrollbar, 0 on failure.
 * @brief Get flags for horizontal scrollbar
 */
Ewl_ScrollBar_Flags ewl_scrollpane_get_hscrollbar_flag(Ewl_ScrollPane * s)
{
	Ewl_ScrollBar_Flags f;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, 0);

	f = ewl_scrollbar_get_flag(EWL_SCROLLBAR(s->hscrollbar));

	DRETURN_INT(f, DLEVEL_STABLE);
}

/**
 * @param s: the scrollpane that contains the scrollbar to retrieve
 * @return Returns the flags of the vertical scrollbar on success, 0 on failure.
 * @brief Get flags for vertical scrollbar
 */
Ewl_ScrollBar_Flags ewl_scrollpane_get_vscrollbar_flag(Ewl_ScrollPane * s)
{
	Ewl_ScrollBar_Flags f;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, 0);

	f = ewl_scrollbar_get_flag(EWL_SCROLLBAR(s->vscrollbar));

	DRETURN_INT(f, DLEVEL_STABLE);
}

/**
 * @param s: the scrollpane to retrieve it's horizontal scrollbar value
 * @return Returns the value of the horizontal scrollbar in @a s on success.
 * @brief Retrieves the value of the horizontal scrollbar in @a s.
 */
double ewl_scrollpane_get_hscrollbar_value(Ewl_ScrollPane *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, 0.0);

	DRETURN_FLOAT(ewl_scrollbar_get_value(EWL_SCROLLBAR(s->hscrollbar)),
			DLEVEL_STABLE);
}

/**
 * @param s: the scrollpane to retrieve it's vertical scrollbar value
 * @return Returns the value of the vertical scrollbar in @a s on success.
 * @brief Retrieves the value of the vertical scrollbar in @a s.
 */
double ewl_scrollpane_get_vscrollbar_value(Ewl_ScrollPane *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, 0.0);

	DRETURN_FLOAT(ewl_scrollbar_get_value(EWL_SCROLLBAR(s->vscrollbar)),
			DLEVEL_STABLE);
}

/*
 * Move the contents of the scrollbar into place
 */
void ewl_scrollpane_configure_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	int             b_width, b_height;
	Ewl_ScrollPane *s;
	int             vs_width = 0;
	int             hs_height = 0;
	int             content_w, content_h;
	double          step;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SCROLLPANE(w);

	/*
	 * Get the space needed by the scrolbars.
	 */
	vs_width = ewl_object_get_current_w(EWL_OBJECT(s->vscrollbar));
	hs_height = ewl_object_get_current_h(EWL_OBJECT(s->hscrollbar));

	/*
	 * Determine the space used by the contents.
	 */
	content_w = CURRENT_W(w) - vs_width;
	content_h = CURRENT_H(w) - hs_height;

	/*
	 * Position the horizontal scrollbar.
	 */
	ewl_object_request_geometry(EWL_OBJECT(s->hscrollbar),
				    CURRENT_X(w), CURRENT_Y(w) + content_h,
				    content_w, hs_height);

	/*
	 * Position the vertical scrollbar.
	 */
	ewl_object_request_geometry(EWL_OBJECT(s->vscrollbar),
				    CURRENT_X(w) + content_w, CURRENT_Y(w),
				    vs_width, content_h);

	/*
	 * A rare case where we need to know the preferred size over the
	 * minimum size.
	 */
	b_width = ewl_object_get_preferred_w(EWL_OBJECT(s->box));
	b_height = ewl_object_get_preferred_h(EWL_OBJECT(s->box));

	/*
	 * Adjust the scrollbar internal stepping to match the contents.
	 */
	if (content_w < b_width) {
		step = (double)content_w / (double)b_width;
		b_width = (int)ewl_scrollbar_get_value(EWL_SCROLLBAR(
					s->hscrollbar)) *
			(double)(b_width - content_w);
	}
	else {
		step = 1.0;
		b_width = 0;
	}

	ewl_scrollbar_set_step(EWL_SCROLLBAR(s->hscrollbar), step);

	if (content_h < b_height) {
		step = (double)content_h / (double)b_height;
		b_height= (int)(ewl_scrollbar_get_value(EWL_SCROLLBAR(
						s->vscrollbar)) *
				(double)(b_height - content_h));
	}
	else {
		step = 1.0;
		b_height = 0;
	}

	ewl_scrollbar_set_step(EWL_SCROLLBAR(s->vscrollbar), step);

	/*
	 * Now move the box into position. For the scrollpane to work we move
	 * the box relative to the scroll value.
	 */
	ewl_object_request_geometry(EWL_OBJECT(s->box),
				    CURRENT_X(w) - b_width,
				    CURRENT_Y(w) - b_height,
				    content_w + b_width, content_h + b_height);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * When a horizontal scrollbar is clicked we need to move the contents of the
 * scrollpane horizontally.
 */
void ewl_scrollpane_hscroll_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);

	ewl_widget_configure(user_data);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * When a vertical scrollbar is clicked we need to move the contents of the
 * scrollpane vertically.
 */
void ewl_scrollpane_vscroll_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);

	ewl_widget_configure(user_data);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Actually add the child to the box rather than the scrollpane itself.
 */
void ewl_scrollpane_add_cb(Ewl_Container * parent, Ewl_Widget * child)
{
	Ewl_ScrollPane *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("parent", parent);
	DCHECK_PARAM_PTR("child", child);

	s = EWL_SCROLLPANE(parent);

	if ((child != s->box) && (child != s->hscrollbar) &&
			(child != s->vscrollbar)) {
		ewl_container_append_child(EWL_CONTAINER
					   (EWL_SCROLLPANE(parent)->box),
					   child);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Update the preferred size of the scrollpane when the inner box changes.
 */
void
ewl_scrollpane_child_resize_cb(Ewl_Container * parent, Ewl_Widget * child,
			      int size, Ewl_Orientation o)
{
	Ewl_ScrollPane *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = EWL_SCROLLPANE(parent);

	/*
	 * Update the preferred size of the scrollpane to that of the box.
	 */
	if (o == EWL_ORIENTATION_HORIZONTAL) {
		ewl_object_set_preferred_w(EWL_OBJECT(parent),
				ewl_object_get_preferred_w(EWL_OBJECT(s->box)) +
				ewl_object_get_preferred_h(EWL_OBJECT(s->vscrollbar)));
	}
	else
		ewl_object_set_preferred_h(EWL_OBJECT(parent),
				ewl_object_get_preferred_w(EWL_OBJECT(s->box)) +
				ewl_object_get_preferred_h(EWL_OBJECT(s->hscrollbar)));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
