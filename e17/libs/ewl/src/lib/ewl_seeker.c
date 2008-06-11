/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_seeker.h"
#include "ewl_button.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

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
        DCHECK_PARAM_PTR_RET(s, FALSE);

        w = EWL_WIDGET(s);

        if (!ewl_range_init(EWL_RANGE(w)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        /*
         * Initialize the widget fields and set default orientation and type
         */
        ewl_widget_appearance_set(w, "hseeker");
        ewl_widget_inherit(w, EWL_SEEKER_TYPE);
        ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_HFILL |
                                EWL_FLAG_FILL_HSHRINKABLE);

        ewl_container_show_notify_set(EWL_CONTAINER(w),
                                      ewl_seeker_cb_child_show);

        /*
         * Create and add the button portion of the seeker
         */
        s->button = ewl_button_new();
        ewl_widget_internal_set(s->button, TRUE);
        ewl_container_child_append(EWL_CONTAINER(s), s->button);
        ewl_widget_show(s->button);
        ewl_widget_appearance_set(s->button, "hbutton");

        /*
         * Set the starting orientation
         */
        s->orientation = EWL_ORIENTATION_HORIZONTAL;

        /*
         * Add necessary configuration callbacks
         */
        ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, ewl_seeker_cb_configure,
                            NULL);
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
                            ewl_seeker_cb_mouse_down, NULL);
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_UP,
                            ewl_seeker_cb_mouse_up, NULL);
        ewl_callback_prepend(w, EWL_CALLBACK_DESTROY,
                            ewl_seeker_cb_mouse_up, NULL);
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
                            ewl_seeker_cb_mouse_move, NULL);
        ewl_callback_append(w, EWL_CALLBACK_KEY_DOWN,
                            ewl_seeker_cb_key_down, NULL);
        ewl_callback_append(w, EWL_CALLBACK_FOCUS_IN,
                            ewl_container_cb_widget_focus_in, NULL);
        ewl_callback_append(w, EWL_CALLBACK_FOCUS_OUT,
                            ewl_container_cb_widget_focus_out, NULL);

        /*
         * Append a callback for catching mouse movements on the button and
         * add the button to the seeker
         */
        ewl_callback_append(s->button, EWL_CALLBACK_MOUSE_DOWN,
                            ewl_seeker_cb_button_mouse_down, NULL);
        ewl_callback_append(s->button, EWL_CALLBACK_MOUSE_UP,
                            ewl_seeker_cb_button_mouse_up, NULL);

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
        DCHECK_PARAM_PTR(s);
        DCHECK_TYPE(s, EWL_SEEKER_TYPE);

        if (o == s->orientation)
                DRETURN(DLEVEL_STABLE);

        s->orientation = o;
        if (o == EWL_ORIENTATION_HORIZONTAL) {
                ewl_widget_appearance_set(EWL_WIDGET(s), "hseeker");
                ewl_widget_appearance_set(s->button, "hbutton");
                ewl_object_fill_policy_set(EWL_OBJECT(s), EWL_FLAG_FILL_HFILL |
                                EWL_FLAG_FILL_HSHRINKABLE);
        }
        else {
                ewl_widget_appearance_set(EWL_WIDGET(s), "vseeker");
                ewl_widget_appearance_set(s->button, "vbutton");
                ewl_object_fill_policy_set(EWL_OBJECT(s), EWL_FLAG_FILL_VFILL |
                                EWL_FLAG_FILL_VSHRINKABLE);
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
        DCHECK_PARAM_PTR_RET(s, EWL_ORIENTATION_HORIZONTAL);
        DCHECK_TYPE_RET(s, EWL_SEEKER_TYPE, EWL_ORIENTATION_HORIZONTAL);

        DRETURN_INT(s->orientation, DLEVEL_STABLE);
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
        DCHECK_PARAM_PTR(s);
        DCHECK_TYPE(s, EWL_SEEKER_TYPE);

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
        DCHECK_PARAM_PTR_RET(s, 0);
        DCHECK_TYPE_RET(s, EWL_SEEKER_TYPE, 0);

        DRETURN_INT(abs(s->autohide), DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief On a configure event we need to adjust the seeker to fit into it's
 * new coords and position as well as move the button to the correct size and
 * position.
 */
void
ewl_seeker_cb_configure(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Seeker *s;
        Ewl_Range *r;
        double s1, s2;
        double range;
        int dx, dy;
        int dw, dh;
        int nw, nh;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_SEEKER_TYPE);

        s = EWL_SEEKER(w);
        r = EWL_RANGE(w);

        if (!s->button)
                DRETURN(DLEVEL_STABLE);

        dx = CURRENT_X(s);
        dy = CURRENT_Y(s);
        dw = CURRENT_W(s);
        dh = CURRENT_H(s);

        range = r->max_val - r->min_val;
        /*
         * First determine the size based on the number of steps to span from
         * min to max values. Then reduce the total scale to keep the button on
         * the seeker, then position the button.
         */
        s1 = r->step / range;
        if (s->autohide && s1 >= 1.0) {
                ewl_widget_hide(w);
                s->autohide = -abs(s->autohide);
        }

        if (r->invert)
                s2 = (r->max_val - r->value) / range;
        else
                s2 = (r->value - r->min_val) / range;

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

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: The Ewl_Event_Mouse_Down data
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The button mouse down callback
 */
void
ewl_seeker_cb_button_mouse_down(Ewl_Widget *w, void *ev_data,
                                        void *user_data __UNUSED__)
{
        Ewl_Event_Mouse *ev;
        Ewl_Seeker *s;
        int xx, yy, ww, hh;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(ev_data);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);
        DCHECK_TYPE(w->parent, EWL_SEEKER_TYPE);

        ev = ev_data;
        s = EWL_SEEKER(w->parent);

        ewl_object_current_geometry_get(EWL_OBJECT(w), &xx, &yy, &ww, &hh);

        if (s->orientation == EWL_ORIENTATION_HORIZONTAL)
                s->dragstart = ev->x - (xx + ((ww + 1) / 2));
        else
                s->dragstart = ev->y - (yy + ((hh + 1) / 2));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The button mouse up callback
 */
void
ewl_seeker_cb_button_mouse_up(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);
        DCHECK_TYPE(w->parent, EWL_SEEKER_TYPE);

        EWL_SEEKER(w->parent)->dragstart = 0;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: The Ewl_Event_Mouse_Move data
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief Move the cursor to the correct position
 */
void
ewl_seeker_cb_mouse_move(Ewl_Widget *w, void *ev_data,
                         void *user_data __UNUSED__)
{
        Ewl_Event_Mouse *ev;
        Ewl_Seeker *s;
        Ewl_Range *r;
        double scale;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(ev_data);
        DCHECK_TYPE(w, EWL_SEEKER_TYPE);

        s = EWL_SEEKER(w);
        r = EWL_RANGE(w);

        if (r->step == r->max_val - r->min_val)
                DRETURN(DLEVEL_STABLE);

        ev = ev_data;

        /*
         * If the button is pressed, then continue to calculate it's value.
         */
        if (!ewl_widget_state_has(s->button, EWL_FLAG_STATE_PRESSED)) {

                if (s->orientation == EWL_ORIENTATION_HORIZONTAL) {
                        if (ewl_widget_state_has(s, EWL_FLAG_STATE_PRESSED))
                                s->dragstart = ev->x;
                }
                else {
                        if (ewl_widget_state_has(s, EWL_FLAG_STATE_PRESSED))
                                s->dragstart = ev->y;
                }
                DRETURN(DLEVEL_STABLE);
        }

        scale = ewl_seeker_mouse_value_map(s, ev->x, ev->y);

        ewl_range_value_set(r, scale);


        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: The Ewl_Event_Mouse_Down data
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The mouse down callback
 */
void
ewl_seeker_cb_mouse_down(Ewl_Widget *w, void *ev_data,
                                void *user_data __UNUSED__)
{
        Ewl_Seeker *s;
        Ewl_Range *r;
        Ewl_Event_Mouse *ev;
        double value, step = 0;
        int xx, yy, ww, hh;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(ev_data);
        DCHECK_TYPE(w, EWL_SEEKER_TYPE);

        ev = ev_data;
        s = EWL_SEEKER(w);
        r = EWL_RANGE(w);

        if (ewl_widget_state_has(s->button, EWL_FLAG_STATE_PRESSED))
                DRETURN(DLEVEL_STABLE);

        ewl_object_current_geometry_get(EWL_OBJECT(s->button),
                                        &xx, &yy, &ww, &hh);

        value = r->value;

        /*
         * Increment or decrement the value based on the position of the click
         * relative to the button and the orientation of the seeker.
         */
        if (s->orientation == EWL_ORIENTATION_HORIZONTAL) {
                s->dragstart = ev->x;
                if (ev->x < xx) {
                        step = -r->step;
                }
                else if (ev->x > xx + ww) {
                        step = r->step;
                }
        }
        else {
                s->dragstart = ev->y;
                if (ev->y < yy)
                        step = -r->step;
                else if (ev->y > yy + hh)
                        step = r->step;
        }

        if (r->invert)
                step = -step;
        value += step;

        ewl_range_value_set(r, value);

        s->start_time = ecore_time_get();
        s->timer = ecore_timer_add(0.5, ewl_seeker_timer, s);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The mouse up callback
 */
void
ewl_seeker_cb_mouse_up(Ewl_Widget *w, void *ev_data __UNUSED__,
                                void *user_data __UNUSED__)
{
        Ewl_Seeker *s = EWL_SEEKER(w);

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_SEEKER_TYPE);

        if (s->timer)
                ecore_timer_del(s->timer);

        s->timer = NULL;
        s->start_time = 0;
        s->dragstart = 0;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: The Ewl_Event_Key_Down data
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The key down callback
 */
void
ewl_seeker_cb_key_down(Ewl_Widget *w, void *ev_data,
                                void *user_data __UNUSED__)
{
        Ewl_Range *r;
        Ewl_Event_Key *ev;
        double start, end;

        void (*increase)(Ewl_Range *r);
        void (*decrease)(Ewl_Range *r);

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(ev_data);
        DCHECK_TYPE(w, EWL_SEEKER_TYPE);

        ev = ev_data;
        r = EWL_RANGE(w);

        if (!r->invert) {
                increase = ewl_range_increase;
                decrease = ewl_range_decrease;
                start = r->min_val;
                end = r->max_val;
        }
        else {
                increase = ewl_range_decrease;
                decrease = ewl_range_increase;
                start = r->max_val;
                end = r->min_val;
        }

        if (!strcmp(ev->keyname, "Home"))
                ewl_range_value_set(r, start);
        else if (!strcmp(ev->keyname, "End"))
                ewl_range_value_set(r, end);
        else if (!strcmp(ev->keyname, "Left")
                        || !strcmp(ev->keyname, "KP_Left")
                        || !strcmp(ev->keyname, "Up")
                        || !strcmp(ev->keyname, "KP_Up"))
                decrease(r);
        else if (!strcmp(ev->keyname, "Right")
                        || !strcmp(ev->keyname, "KP_Right")
                        || !strcmp(ev->keyname, "Down")
                        || !strcmp(ev->keyname, "KP_Down"))
                increase(r);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param p: The container to work with
 * @param w: The widget to work with
 * @return Returns no value
 * @brief The child show callback
 */
void
ewl_seeker_cb_child_show(Ewl_Container *p, Ewl_Widget *w)
{
        int pw, ph;
        Ewl_Range *r;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(p);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(p, EWL_SEEKER_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        r = EWL_RANGE(p);

        pw = ewl_object_preferred_w_get(EWL_OBJECT(w));
        ph = ewl_object_preferred_h_get(EWL_OBJECT(w));

        if (EWL_SEEKER(r)->orientation == EWL_ORIENTATION_HORIZONTAL)
                pw *= (r->max_val - r->min_val) / r->step;
        else
                ph *= (r->max_val - r->min_val) / r->step;

        ewl_object_preferred_inner_size_set(EWL_OBJECT(p), pw, ph);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static double
ewl_seeker_mouse_value_map(Ewl_Seeker *s, int mx, int my)
{
        Ewl_Range *r;
        int m;
        int dc, dg;
        int adjust;
        double scale;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, 0.0);
        DCHECK_TYPE_RET(s, EWL_SEEKER_TYPE, 0.0);

        r = EWL_RANGE(s);

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
        scale = (r->max_val - r->min_val) * (double)(m - dc) / (double)dg;

        if (!r->invert)
                scale = r->min_val + scale;
        else
                scale = r->max_val - scale;

        DRETURN_FLOAT(scale, DLEVEL_STABLE);
}

static int
ewl_seeker_timer(void *data)
{
        Ewl_Seeker *s;
        double value, posval, step;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(data, ECORE_CALLBACK_CANCEL);
        DCHECK_TYPE_RET(data, EWL_SEEKER_TYPE, ECORE_CALLBACK_CANCEL);

        s = EWL_SEEKER(data);
        value = ewl_range_value_get(EWL_RANGE(s));
        step = ewl_range_step_get(EWL_RANGE(s));

        /*
         * Find the value based on mouse position
         */
        posval = ewl_seeker_mouse_value_map(s, s->dragstart * 2,
                                                s->dragstart * 2);

        /*
         * Limit the value to the intersection with the mouse.
         */
        if (posval > value) {
                if (value + step > posval) {
                        value = posval;
                }
                else {
                        value += step;
                }
        }
        else {
                if (value - step < posval) {
                        value = posval;
                }
                else {
                        value -= step;
                }
        }

        ewl_range_value_set(EWL_RANGE(s), value);

        DRETURN_INT(ECORE_CALLBACK_RENEW, DLEVEL_STABLE);
}

