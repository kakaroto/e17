
#include <Ewl.h>

void            __ewl_scrollpane_realize(Ewl_Widget * w, void *ev_data,
					 void *user_data);
void            __ewl_scrollpane_configure(Ewl_Widget * w, void *ev_data,
					   void *user_data);
void            __ewl_scrollpane_hscroll(Ewl_Widget * w, void *ev_data,
					 void *user_data);
void            __ewl_scrollpane_vscroll(Ewl_Widget * w, void *ev_data,
					 void *user_data);

void            __ewl_scrollpane_add(Ewl_Container * parent,
				     Ewl_Widget * child);
void            __ewl_scrollpane_child_resize(Ewl_Container * parent,
					      Ewl_Widget * child, int size,
					      Ewl_Orientation o);

void            __ewl_scrollpane_body_configure(Ewl_Widget * w, void *ev_data,
						void *user_data);

void            __ewl_scrollpane_box_add(Ewl_Container * p, Ewl_Widget * child);
void            __ewl_scrollpane_box_resize(Ewl_Container * p, Ewl_Widget * w,
					    int size, Ewl_Orientation o);

/**
 * ewl_scrollpane_new - create a new scrollpane
 *
 * Returns a pointer to a newly allocated scrollpane on success, NULL on
 * failure.
 */
Ewl_Widget     *ewl_scrollpane_new(void)
{
	Ewl_ScrollPane *s;

	DENTER_FUNCTION(DLEVEL_UNSTABLE);

	s = NEW(Ewl_ScrollPane, 1);

	ZERO(s, Ewl_ScrollPane, 1);

	ewl_scrollpane_init(s);

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_UNSTABLE);
}

/**
 * ewl_scrollpane_init - initialize the fields of a scrollpane
 * @s: the scrollpane to initialize
 *
 * Returns no value. Sets up default callbacks and field values for the
 * scrollpane @s.
 */
void ewl_scrollpane_init(Ewl_ScrollPane * s)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_UNSTABLE);
	DCHECK_PARAM_PTR("s", s);

	w = EWL_WIDGET(s);

	ewl_container_init(EWL_CONTAINER(s), "/scrollpane/default",
			   __ewl_scrollpane_add, __ewl_scrollpane_child_resize);

	/*
	 * Create the container to hold the contents and it's configure
	 * callback to position it's child.
	 */
	s->box = NEW(Ewl_Container, 1);
	ZERO(s->box, Ewl_Container, 1);
	ewl_container_init(EWL_CONTAINER(s->box),
			   "/scrollpane/default/box",
			   __ewl_scrollpane_box_add,
			   __ewl_scrollpane_box_resize);

	ewl_callback_append(s->box, EWL_CALLBACK_CONFIGURE,
			    __ewl_scrollpane_body_configure, s);

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

	/*
	 * Append necessary callbacks for the scrollpane.
	 */
	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    __ewl_scrollpane_realize, NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    __ewl_scrollpane_configure, NULL);

	/*
	 * We need to know whent he scrollbars have value changes in order to
	 * know when to scroll.
	 */
	ewl_callback_append(s->hscrollbar, EWL_CALLBACK_VALUE_CHANGED,
			    __ewl_scrollpane_hscroll, s);
	ewl_callback_append(s->vscrollbar, EWL_CALLBACK_VALUE_CHANGED,
			    __ewl_scrollpane_vscroll, s);

	DLEAVE_FUNCTION(DLEVEL_UNSTABLE);
}

/**
 * ewl_scrollpane_set_hscrollbar_flag - set flags for horizontal scrollbar
 * @s: the scrollpane that contains the scrollbar to change
 * @f: the flags to set on the horizontal scrollbar in @s
 *
 * Returns no value. The scrollbar flags for the horizontal scrollbar are set
 * to @f.
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
 * ewl_scrollpane_set_vscrollbar_flag - set flags for vertical scrollbar
 * @s: the scrollpane that contains the scrollbar to change
 * @f: the flags to set on the vertical scrollbar in @s
 *
 * Returns no value. The scrollbar flags for the vertical scrollbar are set
 * to @f.
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
 * ewl_scrollpane_get_hscrollbar_flag - get flags for horizontal scrollbar
 * @s: the scrollpane that contains the scrollbar to retrieve
 *
 * Returns the flags of the horizontal scrollbar on success, NULL on failure.
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
 * ewl_scrollpane_get_vscrollbar_flag - get flags for vertical scrollbar
 * @s: the scrollpane that contains the scrollbar to retrieve
 *
 * Returns the flags of the vertical scrollbar on success, NULL on failure.
 */
Ewl_ScrollBar_Flags ewl_scrollpane_get_vscrollbar_flag(Ewl_ScrollPane * s)
{
	Ewl_ScrollBar_Flags f;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, 0);

	f = ewl_scrollbar_get_flag(EWL_SCROLLBAR(s->vscrollbar));

	DRETURN_INT(f, DLEVEL_STABLE);
}

void __ewl_scrollpane_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_widget_realize(EWL_SCROLLPANE(w)->hscrollbar);
	ewl_widget_realize(EWL_SCROLLPANE(w)->vscrollbar);
	ewl_widget_realize(EWL_SCROLLPANE(w)->box);
}

/*
 * Move the contents of the scrollbar into place
 */
void __ewl_scrollpane_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	int             vs_width = 0;
	int             hs_height = 0;
	int             b_width, b_height;
	Ewl_ScrollPane *s;
	int             content_w, content_h;

	/* Ewl_ScrollBar_Flags hf, vf; */

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SCROLLPANE(w);

	/*
	 * Get the space needed by the scrolbars.
	 */
	vs_width = ewl_object_get_preferred_w(EWL_OBJECT(s->vscrollbar));
	hs_height = ewl_object_get_preferred_h(EWL_OBJECT(s->hscrollbar));

	/*
	 * Determine the space used by the contents.
	 */
	content_w = CURRENT_W(w) - (INSET_LEFT(w) + INSET_RIGHT(w) + vs_width);
	content_h = CURRENT_H(w) - (INSET_TOP(w) + INSET_BOTTOM(w) + hs_height);

	/*
	 * Position the horizontal scrollbar.
	 */
	ewl_object_request_geometry(EWL_OBJECT(s->hscrollbar),
				    CURRENT_X(w) + INSET_LEFT(w),
				    CURRENT_Y(w) + content_h, content_w,
				    hs_height);

	/*
	 * Position the vertical scrollbar.
	 */
	ewl_object_request_geometry(EWL_OBJECT(s->vscrollbar),
				    CURRENT_X(w) + content_w,
				    CURRENT_Y(w) + INSET_TOP(w), vs_width,
				    content_h);

	/*
	 * Base the sizing on the preferred size of the box.
	 */
	ewl_object_get_preferred_size(EWL_OBJECT(s->box), &b_width, &b_height);

	/*
	 * Update the fill percentage on the scrollbars based on available space
	 * for the content.
	 */
	ewl_scrollbar_set_fill_percentage(EWL_SCROLLBAR(s->hscrollbar),
					  (double) content_w /
					  (double) b_width);
	ewl_scrollbar_set_fill_percentage(EWL_SCROLLBAR(s->vscrollbar),
					  (double) content_h /
					  (double) b_height);

	/*
	 * Now move the box into position. For the scrollpane to work we move
	 * the box relative to the scroll value.
	 */
	ewl_object_request_geometry(EWL_OBJECT(s->box),
				    CURRENT_X(w) + INSET_LEFT(w),
				    CURRENT_Y(w) + INSET_TOP(w),
				    content_w, content_h);

	/*
	 * Configure the parts of the scrollpane.
	 */
	ewl_widget_configure(s->box);
	ewl_widget_configure(s->hscrollbar);
	ewl_widget_configure(s->vscrollbar);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Configure the inner container of the scrollpane.
 */
void
__ewl_scrollpane_body_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	double          hfill, vfill;
	int             woffset, hoffset;
	Ewl_Container  *c;
	Ewl_ScrollPane *s;
	Ewl_Object     *child;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("user_data", user_data);

	c = EWL_CONTAINER(w);
	s = EWL_SCROLLPANE(user_data);

	/*
	 * Get the value of the scrollbars.
	 */
	hfill = ewl_scrollbar_get_value(EWL_SCROLLBAR(s->hscrollbar));
	vfill = ewl_scrollbar_get_value(EWL_SCROLLBAR(s->vscrollbar));

	/*
	 * Get the child to configure and return if none present
	 */
	child = ewd_list_goto_first(c->children);
	if (!child)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Give the child it's preferred size.
	 */
	ewl_object_request_size(EWL_OBJECT(child),
				ewl_object_get_preferred_w(child),
				ewl_object_get_preferred_h(child));

	/*
	 * Calculate the usable space of the container.
	 */
	woffset = CURRENT_W(w) - (INSET_LEFT(w) + INSET_RIGHT(w));
	hoffset = CURRENT_H(w) - (INSET_TOP(w) + INSET_BOTTOM(w));

	/*
	 * Determine the distance to offset the position of the child.
	 */
	woffset = hfill * (CURRENT_W(child) - woffset);
	hoffset = vfill * (CURRENT_H(child) - hoffset);

	/*
	 * Protect against scrolling small items.
	 */
	if (woffset < 0)
		woffset = 0;

	if (hoffset < 0)
		hoffset = 0;

	/*
	 * Now position the child correctly.
	 */
	ewl_object_request_position(EWL_OBJECT(child),
				    CURRENT_X(w) + INSET_LEFT(w) - woffset,
				    CURRENT_Y(w) + INSET_TOP(w) - hoffset);

	ewl_widget_configure(EWL_WIDGET(child));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * When a horizontal scrollbar is clicked we need to move the contents of the
 * scrollpane horizontally.
 */
void __ewl_scrollpane_hscroll(Ewl_Widget * w, void *ev_data, void *user_data)
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
void __ewl_scrollpane_vscroll(Ewl_Widget * w, void *ev_data, void *user_data)
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
void __ewl_scrollpane_add(Ewl_Container * parent, Ewl_Widget * child)
{
	Ewl_ScrollPane *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("parent", parent);
	DCHECK_PARAM_PTR("child", child);

	s = EWL_SCROLLPANE(parent);

	if (child != s->box && child != s->hscrollbar && child != s->vscrollbar)
		ewl_container_append_child(EWL_CONTAINER
					   (EWL_SCROLLPANE(parent)->box),
					   child);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Update the preferred size of the scrollpane when the inner box changes.
 */
void
__ewl_scrollpane_child_resize(Ewl_Container * parent, Ewl_Widget * child,
			      int size, Ewl_Orientation o)
{
	Ewl_ScrollPane *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = EWL_SCROLLPANE(parent);

	/*
	 * We don't really care when the scrollbars are resized.
	 */
	if (child != s->box)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Update the preferred size of the scrollpane to that of the box.
	 */
	if (o == EWL_ORIENTATION_HORIZONTAL)
		ewl_object_set_preferred_w(EWL_OBJECT(parent),
				ewl_object_get_preferred_w(EWL_OBJECT(s->box))
				+ ewl_object_get_preferred_w(EWL_OBJECT(s->vscrollbar)));
	else
		ewl_object_set_preferred_h(EWL_OBJECT(parent),
				ewl_object_get_preferred_h(EWL_OBJECT(s->box))
				+ ewl_object_get_preferred_h(EWL_OBJECT(s->hscrollbar)));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * When adding a child to the container, remove any other children.
 */
void __ewl_scrollpane_box_add(Ewl_Container * p, Ewl_Widget * child)
{
	/*
	 * Remove all the children before the newly added one.
	 */
	while (ewd_list_goto_first(p->children) != child)
		ewd_list_remove_first(p->children);

	/*
	 * Remove all the children after the newly added one.
	 */
	while (ewd_list_goto_last(p->children) != child)
		ewd_list_remove_last(p->children);

}

void
__ewl_scrollpane_box_resize(Ewl_Container * p, Ewl_Widget * w, int size,
			    Ewl_Orientation o)
{
	/*
	 * Set the new preferred dimensions of the box.
	 */
	ewl_object_set_preferred_w(EWL_OBJECT(p),
				   ewl_object_get_preferred_w(EWL_OBJECT(w)));
	ewl_object_set_preferred_h(EWL_OBJECT(p),
				   ewl_object_get_preferred_h(EWL_OBJECT(w)));
}
