#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static double ewl_seeker_mouse_value_map(Ewl_Seeker *s, int mx, int my);
static int ewl_seeker_timer(void *data);

/**
 * @return Returns NULL on failure, or a pointer to the new seeker on success.
 * @brief Allocate and initialize a new seeker with default orientation
 */
Ewl_Widget *
ewl_seeker_new(void)
{
	Ewl_Seeker *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = NEW(Ewl_Seeker, 1);
	if (!s)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_seeker_init(s)) {
		ewl_widget_destroy(EWL_WIDGET(s));
		s = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * @return Returns NULL on failure, or a pointer to the new seeker on success.
 * @brief Allocate and initialize a new seeker with horizontal orientation
 */
Ewl_Widget *
ewl_hseeker_new(void)
{
	Ewl_Widget *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = ewl_seeker_new();
	if (!s)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_seeker_orientation_set(EWL_SEEKER(s), EWL_ORIENTATION_HORIZONTAL);

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * @return Returns NULL on failure, or a pointer to the new seeker on success.
 * @brief Allocate and initialize a new seeker with vertical orientation
 */
Ewl_Widget *
ewl_vseeker_new(void)
{
	Ewl_Widget *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = ewl_seeker_new();
	if (!s)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_seeker_orientation_set(EWL_SEEKER(s), EWL_ORIENTATION_VERTICAL);

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * @param s: the seeker to be initialized
 * @return Returns no value.
 * @brief Initialize the seeker to some sane starting values
 *
 * Initializes the seeker @a s to the orientation @a orientation to default
 * values and callbacks.
 */
int
ewl_seeker_init(Ewl_Seeker *s)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, FALSE);

	w = EWL_WIDGET(s);

	if (!ewl_container_init(EWL_CONTAINER(w)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	/*
	 * Initialize the widget fields and set default orientation and type
	 */
	ewl_widget_appearance_set(w, "hseeker");
	ewl_widget_inherit(w, EWL_SEEKER_TYPE);
	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_HFILL |
				EWL_FLAG_FILL_HSHRINK);

	ewl_container_show_notify_set(EWL_CONTAINER(w),
				      ewl_seeker_child_show_cb);

	/*
	 * Create and add the button portion of the seeker
	 */
	s->button = ewl_button_new();
	ewl_widget_internal_set(s->button, TRUE);
	ewl_container_child_append(EWL_CONTAINER(s), s->button);
	ewl_widget_show(s->button);
	ewl_widget_appearance_set(s->button, "hbutton");

	/*
	 * Set the starting orientation, range and values
	 */
	s->orientation = EWL_ORIENTATION_HORIZONTAL;
	s->range = 100.0;
	s->value = 0.0;
	s->step = 10.0;

	/*
	 * Add necessary configuration callbacks
	 */
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, ewl_seeker_configure_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
			    ewl_seeker_mouse_down_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_UP,
			    ewl_seeker_mouse_up_cb, NULL);
	ewl_callback_prepend(w, EWL_CALLBACK_DESTROY,
			    ewl_seeker_mouse_up_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
			    ewl_seeker_mouse_move_cb, NULL);

	/*
	 * Append a callback for catching mouse movements on the button and
	 * add the button to the seeker
	 */
	ewl_callback_append(s->button, EWL_CALLBACK_MOUSE_DOWN,
			    ewl_seeker_button_mouse_down_cb, NULL);
	ewl_callback_append(s->button, EWL_CALLBACK_MOUSE_UP,
			    ewl_seeker_button_mouse_up_cb, NULL);

	/*
	 * We want to catch mouse movement events from the button.
	 */
	ewl_container_callback_notify(EWL_CONTAINER(s),
			EWL_CALLBACK_MOUSE_MOVE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param s: the seeker to change orientation
 * @param o: the new orientation for the seeker
 * @returns Returns no value.
 * @brief Changes the orientation of the given seeker.
 */
void
ewl_seeker_orientation_set(Ewl_Seeker *s, Ewl_Orientation o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_TYPE("s", s, EWL_SEEKER_TYPE);

	if (o == s->orientation)
		DRETURN(DLEVEL_STABLE);

	s->orientation = o;
	if (o == EWL_ORIENTATION_HORIZONTAL) {
		ewl_widget_appearance_set(EWL_WIDGET(s), "hseeker");
                ewl_widget_appearance_set(s->button, "hbutton");
		ewl_object_fill_policy_set(EWL_OBJECT(s), EWL_FLAG_FILL_HFILL |
				EWL_FLAG_FILL_HSHRINK);
	}
	else {
		ewl_widget_appearance_set(EWL_WIDGET(s), "vseeker");
                ewl_widget_appearance_set(s->button, "vbutton");
		ewl_object_fill_policy_set(EWL_OBJECT(s), EWL_FLAG_FILL_VFILL |
				EWL_FLAG_FILL_VSHRINK);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: The seeker to get the orientation from
 * @return Returns the orientation set on the seeker
 * @brief This will retrieve the current orientation set on the seeker
 */
Ewl_Orientation 
ewl_seeker_orientation_get(Ewl_Seeker *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, EWL_ORIENTATION_HORIZONTAL);
	DCHECK_TYPE_RET("s", s, EWL_SEEKER_TYPE, EWL_ORIENTATION_HORIZONTAL);

	DRETURN_INT(s->orientation, DLEVEL_STABLE);
}

/**
 *
 * @param s: the seeker whose value will be changed
 * @param v: the new value of the locator, checked against the valid range
 * @return Returns no value.
 * @brief Set the value of pointer of the seekers locator
 */
void
ewl_seeker_value_set(Ewl_Seeker *s, double v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_TYPE("s", s, EWL_SEEKER_TYPE);

	if (v == s->value)
		DRETURN(DLEVEL_STABLE);

	if (v < 0) v = 0;
	if (v > s->range) v = s->range;

	s->value = v;

	ewl_widget_configure(EWL_WIDGET(s));
	ewl_callback_call(EWL_WIDGET(s), EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @param s: the seekers to retrieve the value
 * @return Returns 0 on failure, the value of the seekers locator on success.
 * @brief Retrieve the current value of the seekers locator
 */
double
ewl_seeker_value_get(Ewl_Seeker *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, -1);
	DCHECK_TYPE_RET("s", s, EWL_SEEKER_TYPE, -1);

	DRETURN_FLOAT(s->value, DLEVEL_STABLE);
}

/**
 * @param s: the seeker to change the range
 * @param r: the largest bound on the range of the seekers value
 * @return Returns no value.
 * @brief specify the range of values represented by the seeker
 */
void
ewl_seeker_range_set(Ewl_Seeker *s, double r)
{
	int new_val;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_TYPE("s", s, EWL_SEEKER_TYPE);

	new_val = r * (s->value / s->range);

	s->range = r;
	s->value = new_val;

	ewl_widget_configure(EWL_WIDGET(s));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the seeker to return the range of values
 * @return Returns 0 on failure, or the upper bound on the seeker on success.
 * @brief Retrieve the range of values represented by the seeker
 */
double
ewl_seeker_range_get(Ewl_Seeker *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, -1);
	DCHECK_TYPE_RET("s", s, EWL_SEEKER_TYPE, -1);

	DRETURN_FLOAT(s->range, DLEVEL_STABLE);
}

/**
 * @param s: the seeker to change step
 * @param step: the new step value for the seeker
 * @return Returns no value.
 * @brief Set the steps between increments
 *
 * Changes the amount that each increment or decrement changes the value of the
 * seeker @a s.
 */
void
ewl_seeker_step_set(Ewl_Seeker *s, double step)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_TYPE("s", s, EWL_SEEKER_TYPE);

	if (step > s->range)
		step = s->range;

	s->step = step;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the seeker to retrieve step size
 * @return Returns the step size of the seeker @a s.
 * @brief Retrieve the step size of the seeker
 */
double
ewl_seeker_step_get(Ewl_Seeker *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, -1);
	DCHECK_TYPE_RET("s", s, EWL_SEEKER_TYPE, -1);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
	DRETURN_FLOAT(s->step, DLEVEL_STABLE);
}

/**
 * @param s: the seeker to change autohide
 * @param v: the new boolean value for autohiding
 * @return Returns no value.
 * @brief Changes the autohide setting on the seeker to @a v.
 *
 * Alter the autohide boolean of the seeker @a s to value @a v. If @a v is
 * TRUE, the seeker will be hidden whenever the button is the full size of
 * the seeker.
 */
void
ewl_seeker_autohide_set(Ewl_Seeker *s, int v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_TYPE("s", s, EWL_SEEKER_TYPE);

	if (s->autohide == v || s->autohide == -v)
		DRETURN(DLEVEL_STABLE);

	if (!v) {
		s->autohide = v;
		if (REALIZED(s))
			ewl_widget_show(EWL_WIDGET(s));
	}
	else if (s->autohide < 0)
		s->autohide = -v;
	else
		s->autohide = v;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the seeker to retrieve autohide value
 * @return Returns TRUE if autohide set, otherwise FALSE.
 * @brief Retrieves the current autohide setting on a seeker
 */
int
ewl_seeker_autohide_get(Ewl_Seeker *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, 0);
	DCHECK_TYPE_RET("s", s, EWL_SEEKER_TYPE, 0);

	DRETURN_INT(abs(s->autohide), DLEVEL_STABLE);
}

/**
 * @param s: the seeker to set invert property
 * @param invert: the new value for the seekers invert property
 * @return Returns no value.
 * @brief Changes the invert property on the seeker for inverting it's scale.
 */
void
ewl_seeker_invert_set(Ewl_Seeker *s, int invert)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_TYPE("s", s, EWL_SEEKER_TYPE);

	if (s->invert != invert) {
		s->invert = invert;
		ewl_widget_configure(EWL_WIDGET(s));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the seeker to retrieve invert property value
 * @return Returns the current value of the invert property in the seeker.
 * @brief Retrieve the current invert value from a seeker.
 */
int
ewl_seeker_invert_get(Ewl_Seeker *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, FALSE);
	DCHECK_TYPE_RET("s", s, EWL_SEEKER_TYPE, FALSE);

	DRETURN_INT(s->invert, DLEVEL_STABLE);
}

/**
 * @param s: the seeker to increase
 * @return Returns no value.
 * @brief Increase the value of a seeker by it's step size
 *
 * Increases the value of the seeker @a s by one increment of it's step size.
 */
void
ewl_seeker_increase(Ewl_Seeker *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_TYPE("s", s, EWL_SEEKER_TYPE);

	s->value += s->step;

	if (s->value > s->range)
		s->value = s->range;
	else if (s->value < 0.0)
		s->value = 0.0;

	ewl_widget_configure(EWL_WIDGET(s));

	ewl_callback_call(EWL_WIDGET(s), EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the seeker to decrease
 * @return Returns no value.
 * @brief Decrease the value of a seeker by it's step size
 *
 * Decreases the value of the seeker @a s by one increment of it's step size.
 */
void
ewl_seeker_decrease(Ewl_Seeker *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);
	DCHECK_TYPE("s", s, EWL_SEEKER_TYPE);

	s->value -= s->step;

	if (s->value > s->range)
		s->value = s->range;
	else if (s->value < 0.0)
		s->value = 0.0;

	ewl_widget_configure(EWL_WIDGET(s));

	ewl_callback_call(EWL_WIDGET(s), EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * On a configure event we need to adjust the seeker to fit into it's new
 * coords and position as well as move the button to the correct size and
 * position.
 */
void
ewl_seeker_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Seeker *s;
	double s1, s2;
	int dx, dy;
	int dw, dh;
	int nw, nh;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	s = EWL_SEEKER(w);
	if (!s->button)
		DRETURN(DLEVEL_STABLE);

	dx = CURRENT_X(s);
	dy = CURRENT_Y(s);
	dw = CURRENT_W(s);
	dh = CURRENT_H(s);

	/*
	 * First determine the size based on the number of steps to span from
	 * min to max values. Then reduce the total scale to keep the button on
	 * the seeker, then position the button.
	 */
	s1 = s->step / s->range;
	if (s->autohide && s1 >= 1.0) {
		ewl_widget_hide(w);
		s->autohide = -abs(s->autohide);
	}

	if (s->invert)
		s2 = (s->range - s->value) / s->range;
	else
		s2 = s->value / s->range;

	if (s->orientation == EWL_ORIENTATION_VERTICAL) {
		dh *= s1;
	}
	else {
		dw *= s1;
	}

	ewl_object_size_request(EWL_OBJECT(s->button), dw, dh);

	/*
	 * Get the resulting geometry to reposition the button appropriately.
	 */
	nw = ewl_object_current_w_get(EWL_OBJECT(s->button));
	nh = ewl_object_current_h_get(EWL_OBJECT(s->button));

	if (s->orientation == EWL_ORIENTATION_VERTICAL) {
		dy += (CURRENT_H(s) - nh) * s2;
	}
	else {
		dx += (CURRENT_W(s) - nw) * s2;
	}

	ewl_object_place(EWL_OBJECT(s->button), dx, dy, nw, nh);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_seeker_button_mouse_down_cb(Ewl_Widget *w, void *ev_data,
					void *user_data __UNUSED__)
{
	Ewl_Event_Mouse_Down *ev;
	Ewl_Seeker *s;
	int xx, yy, ww, hh;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("ev_data", ev_data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ev = ev_data;
	s = EWL_SEEKER(w->parent);

	ewl_object_current_geometry_get(EWL_OBJECT(w), &xx, &yy, &ww, &hh);

	if (s->orientation == EWL_ORIENTATION_HORIZONTAL)
		s->dragstart = ev->x - (xx + ((ww + 1) / 2));
	else
		s->dragstart = ev->y - (yy + ((hh + 1) / 2));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_seeker_button_mouse_up_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	EWL_SEEKER(w->parent)->dragstart = 0;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Move the cursor to the correct position
 */
void
ewl_seeker_mouse_move_cb(Ewl_Widget *w, void *ev_data,
			 void *user_data __UNUSED__)
{
	Ewl_Event_Mouse_Move *ev;
	Ewl_Seeker *s;
	double scale;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("ev_data", ev_data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	s = EWL_SEEKER(w);

	if (s->step == s->range)
		DRETURN(DLEVEL_STABLE);

	ev = ev_data;

	/*
	 * If the button is pressed, then continue to calculate it's value.
	 */
	if (!ewl_object_state_has(EWL_OBJECT(s->button), EWL_FLAG_STATE_PRESSED)) {

		if (s->orientation == EWL_ORIENTATION_HORIZONTAL) {
			if (ewl_object_state_has(EWL_OBJECT(s),
						EWL_FLAG_STATE_PRESSED)) {
				s->dragstart = ev->x;
			}
		}
		else {
			if (ewl_object_state_has(EWL_OBJECT(s),
						EWL_FLAG_STATE_PRESSED)) {
				s->dragstart = ev->y;
			}
		}
		DRETURN(DLEVEL_STABLE);
	}

	scale = ewl_seeker_mouse_value_map(s, ev->x, ev->y);

	ewl_seeker_value_set(s, scale);


	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_seeker_mouse_down_cb(Ewl_Widget *w, void *ev_data,
				void *user_data __UNUSED__)
{
	Ewl_Seeker *s;
	Ewl_Event_Mouse_Down *ev;
	double value, step = 0;
	int xx, yy, ww, hh;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("ev_data", ev_data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ev = ev_data;
	s = EWL_SEEKER(w);

	if (ewl_object_state_has(EWL_OBJECT(s->button), EWL_FLAG_STATE_PRESSED))
		DRETURN(DLEVEL_STABLE);

	ewl_object_current_geometry_get(EWL_OBJECT(s->button),
					&xx, &yy, &ww, &hh);

	value = s->value;

	/*
	 * Increment or decrement the value based on the position of the click
	 * relative to the button and the orientation of the seeker.
	 */
	if (s->orientation == EWL_ORIENTATION_HORIZONTAL) {
		s->dragstart = ev->x;
		if (ev->x < xx) {
			step = -s->step;
		}
		else if (ev->x > xx + ww) {
			step = s->step;
		}
	}
	else {
		s->dragstart = ev->y;
		if (ev->y < yy)
			step = -s->step;
		else if (ev->y > yy + hh)
			step = s->step;
	}

	if (s->invert)
		step = -step;
	value += step;

	ewl_seeker_value_set(s, value);

	s->start_time = ecore_time_get();
	s->timer = ecore_timer_add(0.5, ewl_seeker_timer, s);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_seeker_mouse_up_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
				void *user_data __UNUSED__)
{
	Ewl_Seeker *s = EWL_SEEKER(w);

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_SEEKER_TYPE);

	if (s->timer)
		ecore_timer_del(s->timer);

	s->timer = NULL;
	s->start_time = 0;
	s->dragstart = 0;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_seeker_child_show_cb(Ewl_Container *p, Ewl_Widget *w)
{
	int pw, ph;
	Ewl_Seeker *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("p", p, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	s = EWL_SEEKER(p);

	pw = ewl_object_preferred_w_get(EWL_OBJECT(w));
	ph = ewl_object_preferred_h_get(EWL_OBJECT(w));

	if (s->orientation == EWL_ORIENTATION_HORIZONTAL)
		pw *= s->range / s->step;
	else
		ph *= s->range / s->step;

	ewl_object_preferred_inner_size_set(EWL_OBJECT(p), pw, ph);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static double
ewl_seeker_mouse_value_map(Ewl_Seeker *s, int mx, int my)
{
	int m;
	int dc, dg;
	int adjust;
	double scale;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, 0.0);
	DCHECK_TYPE_RET("s", s, EWL_SEEKER_TYPE, 0.0);

	if (s->orientation == EWL_ORIENTATION_HORIZONTAL) {
		m = mx;

		dc = CURRENT_X(s);
		dg = CURRENT_W(s);

		adjust = ewl_object_current_w_get(EWL_OBJECT(s->button));
	}
	else {
		m = my;
		dc = CURRENT_Y(s);
		dg = CURRENT_H(s);

		adjust = ewl_object_current_h_get(EWL_OBJECT(s->button));
	}

	/*
	 * Adjust mouse position based on drag starting point.
	 */
	m -= s->dragstart;

	/*
	 * Adjust the scale to align on the center of the drag bar.
	 */
	dg -= adjust;
	adjust /= 2;
	dc += adjust;

	/*
	 * Bounds checking on the value.
	 */
	if (m < dc)
		m = dc;
	else if (m > dc + dg)
		m = dc + dg;

	/*
	 * Calculate the new value based on the range, sizes and new position.
	 */
	scale = s->range * (double)(m - dc) / (double)dg;
	if (s->invert)
		scale = s->range - scale;

	DRETURN_FLOAT(scale, DLEVEL_STABLE);
}

static int
ewl_seeker_timer(void *data)
{
	Ewl_Seeker *s;
	double value, posval;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("data", data, FALSE);
	DCHECK_TYPE_RET("data", data, EWL_SEEKER_TYPE, FALSE);

	s = EWL_SEEKER(data);
	value = ewl_seeker_value_get(s);

	/*
	 * Find the value based on mouse position
	 */
	posval = ewl_seeker_mouse_value_map(s, s->dragstart, s->dragstart);

	/*
	 * Limit the value to the intersection with the mouse.
	 */
	if (posval > value) {
		if (value + s->step > posval) {
			value = posval;
		}
		else {
			value += s->step;
		}
	}
	else {
		if (value - s->step < posval) {
			value = posval;
		}
		else {
			value -= s->step;
		}
	}

	ewl_seeker_value_set(EWL_SEEKER(s), value);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

