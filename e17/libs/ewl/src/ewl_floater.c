#include <Ewl.h>
#include "ewl_floater.h"

/*
 * Local callbacks used for acting based on events to parent widgets.
 */
static void     __ewl_floater_parent_configure(Ewl_Widget * w, void *ev_data,
					       void *user_data);

/*
 * Local callbacks for events that occur to the floaters.
 */
static void     __ewl_floater_configure(Ewl_Widget * parent, void *ev_data,
					void *user_data);
static void     __ewl_floater_realize(Ewl_Widget * parent, void *ev_data,
				      void *user_data);
static void     __ewl_floater_destroy(Ewl_Widget * parent, void *ev_data,
				      void *user_data);
static void     __ewl_floater_reparent(Ewl_Widget * parent, void *ev_data,
				       void *user_data);

extern void     __ewl_widget_reparent(Ewl_Widget * parent, void *ev_data,
				      void *user_data);

/**
 * ewl_floater_new - allocate a new floater widget
 * @parent: the parent widget to follow if desired
 *
 * Returns NULL on failure, or a pointer to the newly allocated floater
 * widget on success. The @parent widget should be either a widget to follow
 * relative too, or a window for absolute positioning.
 */
Ewl_Widget     *
ewl_floater_new(Ewl_Widget * parent)
{
	Ewl_Widget     *f;

	DENTER_FUNCTION(DLEVEL_STABLE);

	f = NEW(Ewl_Floater, 1);
	if (!f)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	memset(f, 0, sizeof(Ewl_Floater));

	ewl_floater_init(EWL_FLOATER(f), parent);


	DRETURN_PTR(EWL_WIDGET(f), DLEVEL_STABLE);
}


/**
 * ewl_floater_init - initialize a floater to default values
 * @f: the floater widget
 * @parent: the parent widget, we need this to get the evas and clip_box
 * 
 * Returns no value. Sets the fields and callbacks of the floater @f to their
 * defaults.
 */
void
ewl_floater_init(Ewl_Floater * f, Ewl_Widget * parent)
{
	Ewl_Widget     *w;
	Ewl_Window     *window;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("f", f);
	DCHECK_PARAM_PTR("parent", parent);

	w = EWL_WIDGET(f);

	/*
	 * Initialize the inherited box fields, set the fill policy to
	 * normal, and the widget to follow.
	 */
	ewl_box_init(EWL_BOX(w), EWL_ORIENTATION_VERTICAL);
	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FILL_POLICY_NORMAL);
	ewl_widget_set_appearance(w, "/appearance/floater");
	f->follows = parent;

	window = ewl_window_find_window_by_widget(parent);

	/*
	 * Need to add callbacks to the window that contains it as well the
	 * widget it follows, if they are not the same.
	 */
	ewl_callback_append(EWL_WIDGET(window), EWL_CALLBACK_CONFIGURE,
			    __ewl_floater_parent_configure, w);


	/*
	 * Setup the basic callbacks for special events. 
	 */
	ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE,
			     __ewl_floater_configure, NULL);
	ewl_callback_prepend(w, EWL_CALLBACK_REALIZE,
			     __ewl_floater_realize, NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESTROY, __ewl_floater_destroy,
			    NULL);
	ewl_callback_insert_after(w, EWL_CALLBACK_REPARENT,
				  __ewl_floater_reparent, window,
				  __ewl_widget_reparent, NULL);


	/* 
	 * add the floater to the window 
	 */
	ewl_container_append_child(EWL_CONTAINER(window), w);

	/*
	 * Set the layer for this floater and increment the windows layering
	 * for the floaters.
	 */
	LAYER(w) = LAYER(window) + 3000;
	LAYER(window)++;

	f->x = CURRENT_X(EWL_OBJECT(parent));
	f->y = CURRENT_Y(EWL_OBJECT(parent));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_floater_set_position - set the start x/y position of the floater
 * @f: Ewl_Floater * - the floater widget
 * @x: int - start x coordinate from the top
 * @y: int - start y coordinate from the top
 *
 * Returns no value
 */
void
ewl_floater_set_position(Ewl_Floater * f, int x, int y)
{
	DENTER_FUNCTION(DLEVEL_UNSTABLE);

	DCHECK_PARAM_PTR("f", f);

	/*
	 * Set the coordinates of the floater, this will be used for either
	 * absolute or relative positioning.
	 */
	f->x = x;
	f->y = y;

	ewl_widget_configure(EWL_WIDGET(f));

	DLEAVE_FUNCTION(DLEVEL_UNSTABLE);
}

/**
 * ewl_floater_set_relative - set a floater positioned relative to a widget
 * @f: the floater to change positioning relation
 * @w: the widget to position the floater relative
 *
 * Returns no value. Sets the floater @f to be positioned relative to the
 * position of the widget @w.
 */
void
ewl_floater_set_relative(Ewl_Floater * f, Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_UNSTABLE);

	DCHECK_PARAM_PTR("f", f);

	if (f->follows == w)
		return;

	/*
	 * Remove the callback attached to the configure event for the
	 * followed widget.
	 */
	ewl_callback_del(f->follows, EWL_CALLBACK_CONFIGURE,
			 __ewl_floater_configure);

	/*
	 * Set the widget that the floater follows.
	 */
	f->follows = w;

	ewl_widget_configure(EWL_WIDGET(f));

	DLEAVE_FUNCTION(DLEVEL_UNSTABLE);
}

/*
 * Use this to ensure the floater gets configured when the parent/window is.
 */
static void
__ewl_floater_parent_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);

	ewl_widget_configure(EWL_WIDGET(user_data));

}

static void
__ewl_floater_reparent(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);


	LAYER(w) = LAYER(user_data) + 3000;

	if (REALIZED(w))
		ewl_widget_theme_update(w);

}

/*
 * Configure the floater so that the positioning is relative to a followed
 * widget if appropriate.
 */
static void
__ewl_floater_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	int             align, x, y;
	Ewl_Floater    *f;

	DENTER_FUNCTION(DLEVEL_STABLE);

	f = EWL_FLOATER(w);

	/*
	 * Determine actual coordinates based on absolute or relative
	 * positioning.
	 */
	if (f->follows) {
		x = CURRENT_X(f->follows) + f->x;
		y = CURRENT_Y(f->follows) + f->y;
	} else {
		x = f->x;
		y = f->y;
	}

	/*
	 * Store the alignment in a temporary variable for fast access to
	 * determine positioning.
	 */
	align = ewl_object_get_alignment(EWL_OBJECT(w));

	/*
	 * Determine the horizontal placement of the widget based on alignment
	 */
	if (align & EWL_ALIGNMENT_RIGHT) {
		x -= CURRENT_W(w);
	} else if (!(align & EWL_ALIGNMENT_LEFT)) {
		x -= CURRENT_W(w) / 2;
	}

	/*
	 * Determine the vertical placement of the widget based on alignment
	 */
	if (align & EWL_ALIGNMENT_BOTTOM) {
		y -= CURRENT_H(w);
	} else if (!(align & EWL_ALIGNMENT_TOP)) {
		y -= CURRENT_H(w) / 2;
	}

	/*
	 * Now request the calculated coordinates for the floater.
	 */
	ewl_object_request_position(EWL_OBJECT(w), x, y);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Grab some necessary information from the parent when realizing this widget
 */
static void
__ewl_floater_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Floater    *f;

	DENTER_FUNCTION(DLEVEL_STABLE);

	f = EWL_FLOATER(w);

	ewl_object_request_size(EWL_OBJECT(w), CURRENT_W(f->follows), 45);


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Be sure to remove the floater from the parent windows list of floaters when
 * it gets destroyed.
 */
static void
__ewl_floater_destroy(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
