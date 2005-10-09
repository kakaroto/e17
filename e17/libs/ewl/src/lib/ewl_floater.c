#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @return Returns NULL on failure, or the new floater widget on success.
 * @brief Allocate a new floater widget
 */
Ewl_Widget     *ewl_floater_new(void)
{
	Ewl_Widget     *f;

	DENTER_FUNCTION(DLEVEL_STABLE);

	f = NEW(Ewl_Floater, 1);
	if (!f)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_floater_init(EWL_FLOATER(f));

	DRETURN_PTR(EWL_WIDGET(f), DLEVEL_STABLE);
}


/**
 * @param f: the floater widget
 * @return Returns no value.
 * @brief Initialize a floater to default values
 * 
 * Sets the fields and callbacks of the floater @a f to their defaults.
 */
int ewl_floater_init(Ewl_Floater * f)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("f", f, FALSE);

	w = EWL_WIDGET(f);

	/*
	 * Initialize the inherited box fields, set the fill policy to
	 * normal, and the widget to follow.
	 */
	if (!ewl_box_init(EWL_BOX(w))) {
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	ewl_box_orientation_set(EWL_BOX(w), EWL_ORIENTATION_VERTICAL);
	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_NORMAL);
	ewl_widget_appearance_set(w, "floater");
	ewl_widget_inherit(w, "floater");

	ewl_callback_append(EWL_WIDGET(f), EWL_CALLBACK_DESTROY,
			    ewl_floater_follow_destroy_cb, w);

	DRETURN_INT(FALSE, DLEVEL_STABLE);
}

void ewl_floater_follow_set(Ewl_Floater *f, Ewl_Widget *p)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("f", f);

	/*
	 * Don't follow the old parent.
	 */
	if (f->follows) {
		ewl_callback_del_with_data(p, EWL_CALLBACK_CONFIGURE,
				 ewl_floater_follow_configure_cb, f);
	}

	/*
	 * Need to add callbacks to the window that contains it as well the
	 * widget it follows, if they are not the same.
	 */
	if (p) {
		ewl_callback_append(p, EWL_CALLBACK_CONFIGURE,
				    ewl_floater_follow_configure_cb, f);
	}

	f->follows = p;

	ewl_widget_configure(EWL_WIDGET(f));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_Widget *ewl_floater_follow_get(Ewl_Floater *f)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("f", f, NULL);

	DRETURN_PTR(f->follows, DLEVEL_STABLE);
}

/**
 * @param f: Ewl_Floater * - the floater widget
 * @param x: int - start x coordinate from the top
 * @param y: int - start y coordinate from the top
 * @return Returns no value
 * @brief Set the start x/y position of the floater
 */
void ewl_floater_position_set(Ewl_Floater * f, int x, int y)
{
	DENTER_FUNCTION(DLEVEL_UNSTABLE);

	DCHECK_PARAM_PTR("f", f);

	if (x == f->x && y == f->y)
		DRETURN(DLEVEL_UNSTABLE);

	/*
	 * Set the coordinates of the floater, this will be used for either
	 * absolute or relative positioning.
	 */
	f->x = x;
	f->y = y;

	if (f->follows)
		ewl_widget_configure(f->follows);
	else
		ewl_object_position_request(EWL_OBJECT(f), f->x, f->y);

	DLEAVE_FUNCTION(DLEVEL_UNSTABLE);
}

/**
 * @param f: the floater to change positioning relation
 * @param w: the widget to position the floater relative
 * @return Returns no value.
 * @brief Set a floater positioned relative to a widget
 *
 * Sets the floater @a f to be positioned relative to the
 * position of the widget @a w.
 */
void ewl_floater_relative_set(Ewl_Floater * f, Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_UNSTABLE);

	DCHECK_PARAM_PTR("f", f);

	if (f->follows == w)
		return;

	/*
	 * Remove the callback attached to the configure event for the
	 * followed widget.
	 */
	if (f->follows)
		ewl_callback_del(f->follows, EWL_CALLBACK_CONFIGURE,
				 ewl_floater_follow_configure_cb);

	/*
	 * Set the widget that the floater follows.
	 */
	f->follows = w;

	ewl_widget_configure(EWL_WIDGET(w));

	DLEAVE_FUNCTION(DLEVEL_UNSTABLE);
}

/*
 * Use this to ensure the floater gets configured when the parent/window is.
 */
void
ewl_floater_follow_configure_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
							void *user_data)
{
	int             align, x, y;
	Ewl_Floater    *f;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("user_data", user_data);

	w = user_data;
	f = EWL_FLOATER(w);

	/*
	 * Determine actual coordinates based on absolute or relative
	 * positioning.
	 */
	if (f->follows) {
		x = ewl_object_current_x_get(EWL_OBJECT(f->follows)) + f->x;
		y = ewl_object_current_y_get(EWL_OBJECT(f->follows)) + f->y;
	} else {
		x = f->x;
		y = f->y;
	}

	/*
	 * Store the alignment in a temporary variable for fast access to
	 * determine positioning.
	 */
	align = ewl_object_alignment_get(EWL_OBJECT(w));

	/*
	 * Determine the horizontal placement of the widget based on alignment
	 */
	if (align & EWL_FLAG_ALIGN_RIGHT) {
		x -= CURRENT_W(w);
	} else if (!(align & EWL_FLAG_ALIGN_LEFT)) {
		x -= CURRENT_W(w) / 2;
	}

	/*
	 * Determine the vertical placement of the widget based on alignment
	 */
	if (align & EWL_FLAG_ALIGN_BOTTOM) {
		y -= CURRENT_H(w);
	} else if (!(align & EWL_FLAG_ALIGN_TOP)) {
		y -= CURRENT_H(w) / 2;
	}

	/*
	 * Now request the calculated coordinates for the floater.
	 */
	ewl_object_position_request(EWL_OBJECT(w), x, y);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_floater_follow_destroy_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	EWL_FLOATER(w)->follows = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
