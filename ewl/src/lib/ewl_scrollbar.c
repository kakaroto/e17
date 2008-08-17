/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include <math.h>
#include "ewl_base.h"
#include "ewl_scrollbar.h"
#include "ewl_button.h"
#include "ewl_seeker.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

static int ewl_scrollbar_timer(void *data);

/**
 * @return Returns NULL on failure, or a pointer to a new scrollbar on success.
 * @brief Allocate and initialize a new scrollbar widget
 */
Ewl_Widget *
ewl_scrollbar_new(void)
{
        Ewl_Scrollbar *s;

        DENTER_FUNCTION(DLEVEL_STABLE);

        s = NEW(Ewl_Scrollbar, 1);
        if (!s)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        /*
         * Initialize the objects fields.
         */
        if (!ewl_scrollbar_init(s)) {
                ewl_widget_destroy(EWL_WIDGET(s));
                s = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * @return Returns NULL on failure, or a pointer to a new scrollbar on success.
 * @brief Allocate and initialize a new horizontal scrollbar widget
 */
Ewl_Widget *
ewl_hscrollbar_new(void)
{
        Ewl_Widget *s;

        DENTER_FUNCTION(DLEVEL_STABLE);

        s = ewl_scrollbar_new();
        if (!s)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        ewl_scrollbar_orientation_set(EWL_SCROLLBAR(s),
                                      EWL_ORIENTATION_HORIZONTAL);

        DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * @return Returns NULL on failure, or a pointer to a new scrollbar on success.
 * @brief Allocate and initialize a new vertical scrollbar widget
 */
Ewl_Widget *
ewl_vscrollbar_new(void)
{
        Ewl_Widget *s;

        DENTER_FUNCTION(DLEVEL_STABLE);

        s = ewl_scrollbar_new();
        if (!s)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        ewl_scrollbar_orientation_set(EWL_SCROLLBAR(s),
                                      EWL_ORIENTATION_VERTICAL);

        DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * @param s: the scrollbar to initialize
 * @return Returns no value.
 * @brief Initialize a scrollbar to default values
 */
int
ewl_scrollbar_init(Ewl_Scrollbar *s)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, FALSE);

        w = EWL_WIDGET(s);

        if (!ewl_box_init(EWL_BOX(w)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_box_orientation_set(EWL_BOX(w), EWL_ORIENTATION_HORIZONTAL);
        ewl_widget_appearance_set(w, "hscrollbar");
        ewl_widget_inherit(w, EWL_SCROLLBAR_TYPE);
        ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_HFILL |
                                                  EWL_FLAG_FILL_HSHRINKABLE);

        /*
         * Create the basic widgets that are contained in the scrollbar.
         */
        s->decrement = ewl_button_new();
        ewl_widget_internal_set(s->decrement, TRUE);
        ewl_object_alignment_set(EWL_OBJECT(s->decrement),
                        EWL_FLAG_ALIGN_CENTER);
        ewl_object_fill_policy_set(EWL_OBJECT(s->decrement),
                                   EWL_FLAG_FILL_NONE);
        ewl_widget_show(s->decrement);

        /*
         * Create the increment button.
         */
        s->increment = ewl_button_new();
        ewl_widget_internal_set(s->increment, TRUE);
        ewl_object_alignment_set(EWL_OBJECT(s->increment),
                        EWL_FLAG_ALIGN_CENTER);
        ewl_object_fill_policy_set(EWL_OBJECT(s->increment),
                                   EWL_FLAG_FILL_NONE);
        ewl_widget_show(s->increment);

        /*
         * Setup the seeker portion
         */
        s->seeker = ewl_hseeker_new();
        ewl_widget_internal_set(s->seeker, TRUE);
        ewl_object_alignment_set(EWL_OBJECT(s->seeker), EWL_FLAG_ALIGN_CENTER);
        ewl_object_fill_policy_set(EWL_OBJECT(s->seeker),
                                   EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_HSHRINK);
        ewl_widget_show(s->seeker);

        /*
         * Attach callbacks to the buttons and seeker to handle the various
         * events.
         */
        ewl_callback_append(s->increment, EWL_CALLBACK_MOUSE_DOWN,
                            ewl_scrollbar_cb_scroll_start, s);
        ewl_callback_append(s->increment, EWL_CALLBACK_MOUSE_UP,
                            ewl_scrollbar_cb_scroll_stop, s);
        ewl_callback_append(s->decrement, EWL_CALLBACK_MOUSE_DOWN,
                            ewl_scrollbar_cb_scroll_start, s);
        ewl_callback_append(s->decrement, EWL_CALLBACK_MOUSE_UP,
                            ewl_scrollbar_cb_scroll_stop, s);
        ewl_callback_append(s->decrement, EWL_CALLBACK_DESTROY,
                            ewl_scrollbar_cb_scroll_stop, s);

        /*
         * Set the default alignment for the buttons.
         */
        ewl_object_alignment_set(EWL_OBJECT(s->decrement),
                                 EWL_FLAG_ALIGN_CENTER);
        ewl_object_alignment_set(EWL_OBJECT(s->increment),
                                 EWL_FLAG_ALIGN_CENTER);

        /*
         * Set the default amount of space that the seeker should fill.
         */
        s->fill_percentage = 1.0;

        /*
         * Set the default for horizontal standard scrolling
         */
        s->invert = 1;

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
        ewl_range_maximum_value_set(EWL_RANGE(s->seeker), 1.0);
        ewl_range_step_set(EWL_RANGE(s->seeker), 0.05);

        /*
         * Set the appearance strings for the parts of the scrollbar
         */
        ewl_widget_appearance_set(s->decrement, "decrement");
        ewl_widget_appearance_set(s->increment, "increment");

        if (s->buttons_alignment & EWL_FLAG_ALIGN_LEFT) {
                /*
                 * Place in decrement, increment, seeker order.
                 */
                ewl_container_child_append(EWL_CONTAINER(s), s->decrement);
                ewl_container_child_append(EWL_CONTAINER(s), s->increment);
                ewl_container_child_append(EWL_CONTAINER(s), s->seeker);
        }
        else if (s->buttons_alignment & EWL_FLAG_ALIGN_RIGHT) {
                /*
                 * Place in seeker, decrement, increment order.
                 */
                ewl_container_child_append(EWL_CONTAINER(s), s->seeker);
                ewl_container_child_append(EWL_CONTAINER(s), s->decrement);
                ewl_container_child_append(EWL_CONTAINER(s), s->increment);
        }
        else {
                /*
                 * Place in decrement, seeker, increment order.
                 */
                ewl_container_child_append(EWL_CONTAINER(s), s->decrement);
                ewl_container_child_append(EWL_CONTAINER(s), s->seeker);
                ewl_container_child_append(EWL_CONTAINER(s), s->increment);
        }

        /*
         * Set the default value to the beginning of the seeker.
         */
        ewl_range_value_set(EWL_RANGE(s->seeker), 0.0);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param s: the scrollbar to change orientation
 * @param o: the new orientation to use on the scrollbar
 * @return Returns no value.
 * @brief Change the orientation of a scrollbar.
 */
void
ewl_scrollbar_orientation_set(Ewl_Scrollbar *s, Ewl_Orientation o)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(s);
        DCHECK_TYPE(s, EWL_SCROLLBAR_TYPE);

        if (o == ewl_box_orientation_get(EWL_BOX(s)))
                DRETURN(DLEVEL_STABLE);

        ewl_box_orientation_set(EWL_BOX(s), o);
        ewl_seeker_orientation_set(EWL_SEEKER(s->seeker), o);

        /*
         * Swap scroll direction on orientation change
         */
        s->invert = -s->invert;

        if (o == EWL_ORIENTATION_HORIZONTAL) {
                ewl_widget_appearance_set(EWL_WIDGET(s), "hscrollbar");
                ewl_object_fill_policy_set(EWL_OBJECT(s),
                                EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_HSHRINKABLE);
                ewl_object_fill_policy_set(EWL_OBJECT(s->seeker),
                                EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_HSHRINK);
        }
        else {
                ewl_widget_appearance_set(EWL_WIDGET(s), "vscrollbar");
                ewl_object_fill_policy_set(EWL_OBJECT(s),
                                EWL_FLAG_FILL_VFILL | EWL_FLAG_FILL_VSHRINKABLE);
                ewl_object_fill_policy_set(EWL_OBJECT(s->seeker),
                                EWL_FLAG_FILL_VFILL | EWL_FLAG_FILL_VSHRINK);
        }

        /*
         * Set the alignment of the buttons to the seeker.
         */
        s->buttons_alignment = ewl_theme_data_int_get(EWL_WIDGET(s),
                                                      "button_order");

        ewl_container_child_remove(EWL_CONTAINER(s), s->decrement);
        ewl_container_child_remove(EWL_CONTAINER(s), s->increment);
        ewl_container_child_remove(EWL_CONTAINER(s), s->seeker);

        /*
         * Setup a few orientation specific variables, such as appearance and
         * packing order.
         */
        if (o == EWL_ORIENTATION_HORIZONTAL) {
                if (s->buttons_alignment & EWL_FLAG_ALIGN_LEFT) {
                        /*
                         * Place in decrement, increment, seeker order.
                         */
                        ewl_container_child_append(EWL_CONTAINER(s),
                                                   s->decrement);
                        ewl_container_child_append(EWL_CONTAINER(s),
                                                   s->increment);
                        ewl_container_child_append(EWL_CONTAINER(s), s->seeker);
                }
                else if (s->buttons_alignment & EWL_FLAG_ALIGN_RIGHT) {
                        /*
                         * Place in seeker, decrement, increment order.
                         */
                        ewl_container_child_append(EWL_CONTAINER(s), s->seeker);
                        ewl_container_child_append(EWL_CONTAINER(s),
                                                   s->decrement);
                        ewl_container_child_append(EWL_CONTAINER(s),
                                                   s->increment);
                }
                else {
                        /*
                         * Place in decrement, seeker, increment order.
                         */
                        ewl_container_child_append(EWL_CONTAINER(s),
                                                   s->decrement);
                        ewl_container_child_append(EWL_CONTAINER(s), s->seeker);
                        ewl_container_child_append(EWL_CONTAINER(s),
                                                   s->increment);
                }
        }
        else {
                if (s->buttons_alignment & EWL_FLAG_ALIGN_TOP) {
                        /*
                         * Place in increment, decrement, seeker order.
                         */
                        ewl_container_child_append(EWL_CONTAINER(s),
                                                   s->increment);
                        ewl_container_child_append(EWL_CONTAINER(s),
                                                   s->decrement);
                        ewl_container_child_append(EWL_CONTAINER(s), s->seeker);
                }
                else if (s->buttons_alignment & EWL_FLAG_ALIGN_BOTTOM) {
                        /*
                         * Place in seeker, increment, decrement order.
                         */
                        ewl_container_child_append(EWL_CONTAINER(s), s->seeker);
                        ewl_container_child_append(EWL_CONTAINER(s),
                                                   s->increment);
                        ewl_container_child_append(EWL_CONTAINER(s),
                                                   s->decrement);
                }
                else {
                        /*
                         * Place in increment, seeker, decrement order.
                         */
                        ewl_container_child_append(EWL_CONTAINER(s),
                                                   s->increment);
                        ewl_container_child_append(EWL_CONTAINER(s), s->seeker);
                        ewl_container_child_append(EWL_CONTAINER(s),
                                                   s->decrement);
                }
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the scrollbar to change orientation
 * @return Returns no value.
 * @brief Change the orientation of a scrollbar.
 */
Ewl_Orientation
ewl_scrollbar_orientation_get(Ewl_Scrollbar *s)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, EWL_ORIENTATION_HORIZONTAL);
        DCHECK_TYPE_RET(s, EWL_SCROLLBAR_TYPE, EWL_ORIENTATION_HORIZONTAL);

        DRETURN_INT(ewl_box_orientation_get(EWL_BOX(s)), DLEVEL_STABLE);
}

/**
 * @param s: the scrollbar to set inverted scrolling value
 * @param i: value to set for inverse scrolling
 * @return Returns no value.
 * @brief Sets the inverse scrolling flag on a scrollbar.
 */
void
ewl_scrollbar_inverse_scroll_set(Ewl_Scrollbar *s, char i)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(s);
        DCHECK_TYPE(s, EWL_SCROLLBAR_TYPE);

        if (i >= 0) i = 1;
        else i = -1;

        s->direction = i;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: the scrollbar to get inverted scrolling value
 * @return Returns the current value for inverted scrolling.
 * @brief Checks the inverse scrolling flag on a scrollbar.
 */
char
ewl_scrollbar_inverse_scroll_get(Ewl_Scrollbar *s)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, 1);
        DCHECK_TYPE_RET(s, EWL_SCROLLBAR_TYPE, 1);

        DRETURN_INT(s->invert, DLEVEL_STABLE);
}

/**
 * @param s: the scrollbar to get the current value
 * @return Returns the current value of the scrollbar @a s.
 * @brief Get the current value of the dragbar
 */
double
ewl_scrollbar_value_get(Ewl_Scrollbar *s)
{
        double v;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, -1);
        DCHECK_TYPE_RET(s, EWL_SCROLLBAR_TYPE, -1);

        v = ewl_range_value_get(EWL_RANGE(s->seeker));

        DRETURN_FLOAT(v, DLEVEL_STABLE);
}

/**
 * @param s: the scrollbar to set the current value
 * @param v: the new value for the scrollbar
 * @return Returns no value.
 * @brief Set the current value of the dragbar
 *
 * Sets the current value of the scrollbar @a s.
 */
void
ewl_scrollbar_value_set(Ewl_Scrollbar *s, double v)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(s);
        DCHECK_TYPE(s, EWL_SCROLLBAR_TYPE);

        ewl_range_value_set(EWL_RANGE(s->seeker), v);

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
        DCHECK_PARAM_PTR_RET(s, 0);
        DCHECK_TYPE_RET(s, EWL_SCROLLBAR_TYPE, 0);

        DRETURN_INT(ewl_range_step_get(EWL_RANGE(s->seeker)), DLEVEL_STABLE);
}

/**
 * @param s: the scrollbar to change step size
 * @param v: the new step size of the scrollbar
 * @return Returns no value.
 * @brief Change the step size of a scrollbar
 *
 * Changes the step size of the scrollbar @a s to @a v.
 */
void
ewl_scrollbar_step_set(Ewl_Scrollbar *s, double v)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(s);
        DCHECK_TYPE(s, EWL_SCROLLBAR_TYPE);

        ewl_range_step_set(EWL_RANGE(s->seeker), v);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: The scrollbar
 * @return Returns no value
 * @brief Decrement the value of the scrollbar's seeker portion
 */
void
ewl_scrollbar_cb_scroll_start(Ewl_Widget *w, void *ev_data __UNUSED__,
                                                        void *user_data)
{
        Ewl_Scrollbar *s;
        Ewl_Orientation o;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(user_data);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);
        DCHECK_TYPE(user_data, EWL_SCROLLBAR_TYPE);

        s = EWL_SCROLLBAR(user_data);
        if (w == s->increment)
                s->direction = 1;
        else
                s->direction = -1;

        /*
         * Need to scroll in the opposite direction for the vertical
         * scrollbar.
         */
        o = ewl_box_orientation_get(EWL_BOX(s));
        s->direction = s->direction * s->invert;

        s->start_time = ecore_time_get();
        s->timer = ecore_timer_add(0.02, ewl_scrollbar_timer, s);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev_data: UNUSED
 * @param user_data: The Ewl_Scrollbar
 * @return Returns no value
 * @brief The scroll stop callback
 */
void
ewl_scrollbar_cb_scroll_stop(Ewl_Widget *w __UNUSED__,
                             void *ev_data __UNUSED__, void *user_data)
{
        Ewl_Scrollbar *s;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(user_data);
        DCHECK_TYPE(user_data, EWL_SCROLLBAR_TYPE);

        s = EWL_SCROLLBAR(user_data);

        if (s->timer)
                ecore_timer_del(s->timer);

        s->timer = NULL;
        s->direction = 0;
        s->start_time = 0;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ewl_scrollbar_timer(void *data)
{
        Ewl_Scrollbar *s;
        double dt;
        double value;
        int velocity;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(data, ECORE_CALLBACK_CANCEL);

        s = EWL_SCROLLBAR(data);

        dt = ecore_time_get() - s->start_time;
        value = ewl_range_value_get(EWL_RANGE(s->seeker));

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

        ewl_range_value_set(EWL_RANGE(s->seeker), value);

        DRETURN_INT(ECORE_CALLBACK_RENEW, DLEVEL_STABLE);
}
