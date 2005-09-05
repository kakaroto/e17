#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static int  ewl_scrollbar_timer(void *data);

/**
 * @param orientation: the desired orientation of the scrollbar
 * @return Returns NULL on failure, or a pointer to a new scrollbar on success.
 * @brief Allocate and initialize a new scrollbar widget
 */
Ewl_Widget     *ewl_scrollbar_new(Ewl_Orientation orientation)
{
	Ewl_Scrollbar  *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = NEW(Ewl_Scrollbar, 1);
	if (!s)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	/*
	 * Initialize the objects fields.
	 */
	ewl_scrollbar_init(s, orientation);

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * @param s: the scrollbar to initialize
 * @param orientation: the orientation for the scrollbar
 * @return Returns no value.
 * @brief Initialize a scrollbar to default values
 */
void ewl_scrollbar_init(Ewl_Scrollbar * s, Ewl_Orientation orientation)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	w = EWL_WIDGET(s);

	ewl_box_init(EWL_BOX(w), orientation);
	ewl_widget_inherit(w, "scrollbar");

	/*
	 * Create the basic widgets that are contained in the scrollbar.
	 */
	s->button_decrement = ewl_button_new(NULL);
	ewl_widget_internal_set(s->button_decrement, TRUE);
	ewl_object_alignment_set(EWL_OBJECT(s->button_decrement),
			EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(s->button_decrement),
				   EWL_FLAG_FILL_NONE);
	ewl_widget_show(s->button_decrement);

	/*
	 * Create the increment button.
	 */
	s->button_increment = ewl_button_new(NULL);
	ewl_widget_internal_set(s->button_increment, TRUE);
	ewl_object_alignment_set(EWL_OBJECT(s->button_increment),
			EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(s->button_increment),
				   EWL_FLAG_FILL_NONE);
	ewl_widget_show(s->button_increment);

	/*
	 * Setup the seeker portion
	 */
	s->seeker = ewl_seeker_new(orientation);
	ewl_widget_internal_set(s->seeker, TRUE);
	ewl_widget_show(s->seeker);

	/*
	 * Attach callbacks to the buttons and seeker to handle the various
	 * events.
	 */
	if (orientation == EWL_ORIENTATION_HORIZONTAL) {
		ewl_callback_append(s->button_increment,
				EWL_CALLBACK_MOUSE_DOWN,
				ewl_scrollbar_scroll_start_cb, s);
		ewl_callback_append(s->button_increment,
				EWL_CALLBACK_MOUSE_UP,
				ewl_scrollbar_scroll_stop_cb, s);
		ewl_callback_append(s->button_decrement,
				EWL_CALLBACK_MOUSE_DOWN,
				ewl_scrollbar_scroll_start_cb, s);
		ewl_callback_append(s->button_decrement,
				EWL_CALLBACK_MOUSE_UP,
				ewl_scrollbar_scroll_stop_cb, s);
	}
	else {
		ewl_callback_append(s->button_increment,
				EWL_CALLBACK_MOUSE_DOWN,
				ewl_scrollbar_scroll_start_cb, s);
		ewl_callback_append(s->button_increment,
				EWL_CALLBACK_MOUSE_UP,
				ewl_scrollbar_scroll_stop_cb, s);
		ewl_callback_append(s->button_decrement,
				EWL_CALLBACK_MOUSE_DOWN,
				ewl_scrollbar_scroll_start_cb, s);
		ewl_callback_append(s->button_decrement,
				EWL_CALLBACK_MOUSE_UP,
				ewl_scrollbar_scroll_stop_cb, s);
	}

	/*
	 * Set the default alignment for the buttons.
	 */
	ewl_object_alignment_set(EWL_OBJECT(s->button_decrement),
				 EWL_FLAG_ALIGN_CENTER);
	ewl_object_alignment_set(EWL_OBJECT(s->button_increment),
				 EWL_FLAG_ALIGN_CENTER);

	/*
	 * Set the default amount of space that the seeker should fill.
	 */
	s->fill_percentage = 1.0;

	/*
	 * Append a value change callback to the seeker to catch when it
	 * moves.
	 */
	ewl_container_callback_notify(EWL_CONTAINER(s),
			EWL_CALLBACK_VALUE_CHANGED);

	/*
	 * Define the maximum value that the seeker can reach, and the
	 * default increments it takes to get there.
	 */
	ewl_seeker_range_set(EWL_SEEKER(s->seeker), 1.0);
	ewl_seeker_step_set(EWL_SEEKER(s->seeker), 0.05);

	/*
	 * Set the appearance strings for the parts of the scrollbar
	 */
	ewl_widget_appearance_set(s->button_decrement, "button_decrement");
	ewl_widget_appearance_set(s->button_increment, "button_increment");

	/*
	 * Set the alignment of the buttons to the seeker.
	 */
	s->buttons_alignment = ewl_theme_data_int_get(EWL_WIDGET(s),
						      "button_order");

	/*
	 * Setup a few orientation specific variables, such as appearance and
	 * packing order.
	 */
	if (orientation == EWL_ORIENTATION_HORIZONTAL) {

		if (s->buttons_alignment & EWL_FLAG_ALIGN_LEFT) {

			/*
			 * Place in decrement, increment, seeker order.
			 */
			ewl_container_child_append(EWL_CONTAINER(s),
						   s->button_decrement);
			ewl_container_child_append(EWL_CONTAINER(s),
						   s->button_increment);
			ewl_container_child_append(EWL_CONTAINER(s), s->seeker);
		}
		else if (s->buttons_alignment & EWL_FLAG_ALIGN_RIGHT) {

			/*
			 * Place in seeker, decrement, increment order.
			 */
			ewl_container_child_append(EWL_CONTAINER(s), s->seeker);
			ewl_container_child_append(EWL_CONTAINER(s),
						   s->button_decrement);
			ewl_container_child_append(EWL_CONTAINER(s),
						   s->button_increment);
		}
		else {

			/*
			 * Place in decrement, seeker, increment order.
			 */
			ewl_container_child_append(EWL_CONTAINER(s),
						   s->button_decrement);
			ewl_container_child_append(EWL_CONTAINER(s), s->seeker);
			ewl_container_child_append(EWL_CONTAINER(s),
						   s->button_increment);
		}

		/*
		 * Set the default value to the beginning of the seeker.
		 */
		ewl_seeker_value_set(EWL_SEEKER(s->seeker), 0);
		ewl_widget_appearance_set(w, "hscrollbar");
		ewl_object_fill_policy_set(EWL_OBJECT(s),
				EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_HSHRINK);
	}
	else {

		if (s->buttons_alignment & EWL_FLAG_ALIGN_TOP) {

			/*
			 * Place in increment, decrement, seeker order.
			 */
			ewl_container_child_append(EWL_CONTAINER(s),
						   s->button_increment);
			ewl_container_child_append(EWL_CONTAINER(s),
						   s->button_decrement);
			ewl_container_child_append(EWL_CONTAINER(s), s->seeker);
		}
		else if (s->buttons_alignment & EWL_FLAG_ALIGN_BOTTOM) {

			/*
			 * Place in seeker, increment, decrement order.
			 */
			ewl_container_child_append(EWL_CONTAINER(s), s->seeker);
			ewl_container_child_append(EWL_CONTAINER(s),
						   s->button_increment);
			ewl_container_child_append(EWL_CONTAINER(s),
						   s->button_decrement);
		}
		else {

			/*
			 * Place in increment, seeker, decrement order.
			 */
			ewl_container_child_append(EWL_CONTAINER(s),
						   s->button_increment);
			ewl_container_child_append(EWL_CONTAINER(s), s->seeker);
			ewl_container_child_append(EWL_CONTAINER(s),
						   s->button_decrement);
		}

		/*
		 * Set the default value to the beginning of the seeker.
		 */
		ewl_seeker_value_set(EWL_SEEKER(s->seeker), 0);
		ewl_widget_appearance_set(w, "vscrollbar");
		ewl_object_fill_policy_set(EWL_OBJECT(s),
				EWL_FLAG_FILL_VFILL | EWL_FLAG_FILL_VSHRINK);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the scrollbar to get the current value
 * @return Returns the current value of the scrollbar @a s.
 * @brief Get the current value of the dragbar
 */
double ewl_scrollbar_value_get(Ewl_Scrollbar * s)
{
	double          v;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, -1);

	v = ewl_seeker_value_get(EWL_SEEKER(s->seeker));

	/*
	if (EWL_BOX(s)->orientation == EWL_ORIENTATION_VERTICAL)
		v = 1.0 - v;
		*/

	DRETURN_FLOAT(v, DLEVEL_UNSTABLE);
}

/**
 * @param s: the scrollbar to set the current value
 * @param v: the new value for the scrollbar
 * @return Returns no value.
 * @brief Set the current value of the dragbar
 *
 * Sets the current value of the scrollbar @a s.
 */
void ewl_scrollbar_value_set(Ewl_Scrollbar * s, double v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	/*
	if (EWL_BOX(s)->orientation == EWL_ORIENTATION_VERTICAL)
		v = 1.0 - v;
		*/

	ewl_seeker_value_set(EWL_SEEKER(s->seeker), v);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the scrollbar to retrieve step size
 * @return Returns the current step size of the scrollbar.
 * @brief Get the current step size of the scrollbar
 */
double
ewl_scrollbar_step_get(Ewl_Scrollbar *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("s", s, 0);

	DRETURN_INT(ewl_seeker_step_get(EWL_SEEKER(s->seeker)), DLEVEL_STABLE);
}

/**
 * @param s: the scrollbar to change step size
 * @param v: the new step size of the scrollbar
 * @return Returns no value.
 * @brief Change the step size of a scrollbar
 *
 * Changes the step size of the scrollbar @a s to @a v.
 */
void ewl_scrollbar_step_set(Ewl_Scrollbar *s, double v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("s", s);

	ewl_seeker_step_set(EWL_SEEKER(s->seeker), v);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Decrement the value of the scrollbar's seeker portion
 */
void
ewl_scrollbar_scroll_start_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
							void *user_data)
{
	Ewl_Scrollbar  *s;
	Ewl_Orientation o;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SCROLLBAR(user_data);
	if (w == s->button_increment)
		s->direction = 1;
	else
		s->direction = -1;

	/*
	 * Need to scroll in the opposite direction for the vertical
	 * scrollbar.
	 */
	o = ewl_box_orientation_get(EWL_BOX(s));
	if (o == EWL_ORIENTATION_VERTICAL)
		s->direction = -s->direction;

	s->start_time = ecore_time_get();
	s->timer = ecore_timer_add(0.02, ewl_scrollbar_timer, s);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_scrollbar_scroll_stop_cb(Ewl_Widget * w __UNUSED__,
				void *ev_data __UNUSED__, void *user_data)
{
	Ewl_Scrollbar *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = EWL_SCROLLBAR(user_data);

	ecore_timer_del(s->timer);

	s->timer = NULL;
	s->direction = 0;
	s->start_time = 0;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int ewl_scrollbar_timer(void *data)
{
	Ewl_Scrollbar  *s;
	double          dt;
	double          value;
	int             velocity;

	s = EWL_SCROLLBAR(data);

	dt = ecore_time_get() - s->start_time;
	value = ewl_seeker_value_get(EWL_SEEKER(s->seeker));

	/*
	 * Check the theme for a velocity setting and bring it within normal
	 * useable bounds.
	 */
	velocity = ewl_theme_data_int_get(EWL_WIDGET(s), "velocity");
	if (velocity < 1)
		velocity = 1;
	else if (velocity > 10)
		velocity = 10;

	/*
	 * Move the value of the seeker based on the direction of it's motion
	 * and the velocity setting.
	 */
	value += (double)(s->direction) * 10 * (1 - exp(-dt)) *
		 ((double)(velocity) / 100.0);

	ewl_seeker_value_set(EWL_SEEKER(s->seeker), value);

	return 1;
}
