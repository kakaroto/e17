/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_spinner.h"
#include "ewl_box.h"
#include "ewl_button.h"
#include "ewl_entry.h"
#include "ewl_private.h"
#include "ewl_macros.h"
#include "ewl_debug.h"

static void ewl_spinner_entry_update(Ewl_Spinner *s);
static int ewl_spinner_timer(void *data);
static void ewl_spinner_child_handle(Ewl_Spinner *s);
/**
 * @return Returns a new spinner widget on success, NULL on failure.
 * @brief Allocate a new spinner widget
 */
Ewl_Widget *
ewl_spinner_new(void)
{
        Ewl_Spinner *s = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);

        s = NEW(Ewl_Spinner, 1);
        if (!s)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_spinner_init(s)) {
                ewl_widget_destroy(EWL_WIDGET(s));
                s = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * @param s: the spinner to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initialize a spinner to default values and callbacks
 *
 * Sets the fields and callbacks of the spinner @a s their default values.
 */
int
ewl_spinner_init(Ewl_Spinner *s)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, FALSE);

        w = EWL_WIDGET(s);

        if (!ewl_range_init(EWL_RANGE(w)))
        {
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }

        ewl_widget_appearance_set(w, EWL_SPINNER_TYPE);
        ewl_widget_inherit(w, EWL_SPINNER_TYPE);

        ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_HSHRINKABLE |
                                   EWL_FLAG_FILL_HFILL);

        ewl_callback_append(w, EWL_CALLBACK_REALIZE,
                                ewl_spinner_cb_realize, NULL);
        ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
                                ewl_spinner_cb_configure, NULL);
        ewl_callback_append(w, EWL_CALLBACK_VALUE_CHANGED,
                                ewl_spinner_cb_value_changed, NULL);
        ewl_callback_prepend(w, EWL_CALLBACK_DESTROY,
                                ewl_spinner_cb_destroy, NULL);
        ewl_callback_append(w, EWL_CALLBACK_FOCUS_IN,
                                ewl_container_cb_widget_focus_in, NULL);
        ewl_callback_append(w, EWL_CALLBACK_FOCUS_OUT,
                                ewl_container_cb_widget_focus_out, NULL);
        ewl_container_show_notify_set(EWL_CONTAINER(w),
                                ewl_spinner_cb_child_show);
        ewl_container_resize_notify_set(EWL_CONTAINER(w),
                                ewl_spinner_cb_child_resize);

        s->entry = ewl_entry_new();
        ewl_text_text_set(EWL_TEXT(s->entry), "0");
        ewl_container_child_append(EWL_CONTAINER(s), s->entry);
        ewl_object_fill_policy_set(EWL_OBJECT(s->entry), EWL_FLAG_FILL_HFILL |
                                   EWL_FLAG_FILL_HSHRINKABLE);
        ewl_object_alignment_set(EWL_OBJECT(s->entry), EWL_FLAG_ALIGN_LEFT);

        ewl_widget_internal_set(EWL_WIDGET(s->entry), TRUE);
        ewl_callback_del(s->entry, EWL_CALLBACK_KEY_DOWN,
                                ewl_entry_cb_key_down);

        ewl_callback_append(EWL_WIDGET(s), EWL_CALLBACK_KEY_DOWN,
                                ewl_spinner_cb_key_down, NULL);
        ewl_callback_append(EWL_WIDGET(s), EWL_CALLBACK_FOCUS_OUT,
                                ewl_spinner_cb_focus_out, NULL);
        ewl_callback_append(EWL_WIDGET(s), EWL_CALLBACK_MOUSE_WHEEL,
                                ewl_spinner_cb_wheel, NULL);

        ewl_widget_show(s->entry);

        s->vbox = ewl_vbox_new();
        ewl_container_child_append(EWL_CONTAINER(s), s->vbox);
        ewl_widget_appearance_set(s->vbox, "controls");
        ewl_widget_internal_set(s->vbox, TRUE);
        ewl_object_fill_policy_set(EWL_OBJECT(s->vbox), EWL_FLAG_FILL_NONE);
        ewl_widget_show(s->vbox);

        s->increment = ewl_button_new();
        ewl_container_child_append(EWL_CONTAINER(s->vbox), s->increment);
        ewl_object_alignment_set(EWL_OBJECT(s->increment),
                                        EWL_FLAG_ALIGN_CENTER);
        ewl_widget_appearance_set(s->increment, "increment");
        ewl_object_fill_policy_set(EWL_OBJECT(s->increment),
                                   EWL_FLAG_FILL_NONE);
        ewl_widget_internal_set(s->increment, TRUE);
        ewl_callback_append(s->increment, EWL_CALLBACK_MOUSE_DOWN,
                                ewl_spinner_cb_increase_value, w);
        ewl_callback_append(s->increment, EWL_CALLBACK_MOUSE_UP,
                                ewl_spinner_cb_value_stop, w);
        ewl_callback_append(s->increment, EWL_CALLBACK_KEY_DOWN,
                                ewl_spinner_cb_key_down, s);
        ewl_widget_show(s->increment);

        s->decrement = ewl_button_new();
        ewl_container_child_append(EWL_CONTAINER(s->vbox), s->decrement);
        ewl_object_alignment_set(EWL_OBJECT(s->decrement),
                                        EWL_FLAG_ALIGN_CENTER);
        ewl_widget_appearance_set(s->decrement, "decrement");
        ewl_object_fill_policy_set(EWL_OBJECT(s->decrement),
                                   EWL_FLAG_FILL_NONE);
        ewl_widget_internal_set(s->decrement, TRUE);
        ewl_callback_append(s->decrement, EWL_CALLBACK_MOUSE_DOWN,
                                ewl_spinner_cb_decrease_value, w);
        ewl_callback_append(s->decrement, EWL_CALLBACK_MOUSE_UP,
                                ewl_spinner_cb_value_stop, w);
        ewl_callback_append(s->decrement, EWL_CALLBACK_KEY_DOWN,
                                ewl_spinner_cb_key_down, s);
        ewl_widget_show(s->decrement);

        s->digits = 2;

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param s: the widget to change the number of digits displayed
 * @param digits: the number of digits to display for the spinner @a s
 * @return Returns no value.
 * @brief Set the number of digits displayed by a spinner
 *
 * Changes the digits displayed by @a s to @a digits.
 */
void
ewl_spinner_digits_set(Ewl_Spinner *s, unsigned char digits)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(s);
        DCHECK_TYPE(s, EWL_SPINNER_TYPE);

        s->digits = digits;
        ewl_spinner_entry_update(s);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: The spinnner to get the number of digits displayed
 * @return Returns the number of digits displayed by the spinner
 * @brief This will retrieve the number of digits displayed by the spinner
 */
unsigned char
ewl_spinner_digits_get(Ewl_Spinner *s)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, 0);
        DCHECK_TYPE_RET(s, EWL_SPINNER_TYPE, 0);

        DRETURN_INT(s->digits, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The realize callback
 */
void
ewl_spinner_cb_realize(Ewl_Widget *w, void *ev_data __UNUSED__,
                                void *user_data __UNUSED__)
{
        Ewl_Spinner *s;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        s = EWL_SPINNER(w);
        ewl_spinner_entry_update(s);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The value changed callback
 */
void
ewl_spinner_cb_value_changed(Ewl_Widget *w, void *ev_data __UNUSED__,
                                void *user_data __UNUSED__)
{
        Ewl_Spinner *s;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        s = EWL_SPINNER(w);
        ewl_spinner_entry_update(s);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The configure callback
 */
void
ewl_spinner_cb_configure(Ewl_Widget *w, void *ev_data __UNUSED__,
                                void *user_data __UNUSED__)
{
        Ewl_Spinner *s;
        int cx, cy, cw, ch;
        int pvw; /* the preferred w of the vbox */

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        s = EWL_SPINNER(w);

        cx = CURRENT_X(w);
        cy = CURRENT_Y(w);
        cw = CURRENT_W(w);
        ch = CURRENT_H(w);

        pvw = ewl_object_preferred_w_get(EWL_OBJECT(s->vbox));

        ewl_object_place(EWL_OBJECT(s->vbox), cx + cw - pvw, cy, pvw, ch);
        ewl_object_place(EWL_OBJECT(s->entry), cx, cy, cw - pvw, ch);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: The Ewl_Event_Key_Down data
 * @param user_data: The Ewl_Spinner
 * @return Returns no value
 * @brief The key down callback
 */
void
ewl_spinner_cb_key_down(Ewl_Widget *w, void *ev_data,
                                void *user_data __UNUSED__)
{
        Ewl_Entry *e;
        Ewl_Spinner *s;
        Ewl_Event_Key *ev;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(ev_data);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        s = EWL_SPINNER(w);
        e = EWL_ENTRY(s->entry);

        ev = ev_data;

        if (!strcmp(ev->keyname, "Up"))
                ewl_spinner_cb_increase_value(w, NULL, s);

        else if (!strcmp(ev->keyname, "Down"))
                ewl_spinner_cb_decrease_value(w, NULL, s);

        else if (!strcmp(ev->keyname, "Left"))
                ewl_entry_cursor_move_left(e);

        else if (!strcmp(ev->keyname, "Right"))
                ewl_entry_cursor_move_right(e);
/*
        else if (!strcmp(ev->keyname, "Home"))
                ewl_entry_cursor_home_move(e);

        else if (!strcmp(ev->keyname, "End"))
                ewl_entry_cursor_end_move(e);
*/
        else if (!strcmp(ev->keyname, "BackSpace"))
                ewl_entry_delete_left(e);

        else if (!strcmp(ev->keyname, "Delete"))
                ewl_entry_delete_right(e);

        else if (ev->keyname && (isdigit(ev->keyname[0]) ||
                                 ev->keyname[0] == '.' ||
                                 ev->keyname[0] == '-'))
        {
                ewl_entry_selection_clear(e);
                ewl_text_text_insert(EWL_TEXT(e), ev->keyname,
                                ewl_text_cursor_position_get(EWL_TEXT(e)));
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev_data: UNUSED
 * @param user_data: The Ewl_Spinner
 * @return Returns no value
 * @brief The focus out callback
 */
void
ewl_spinner_cb_focus_out(Ewl_Widget *w, void *ev_data __UNUSED__,
                                void *user_data __UNUSED__)
{
        Ewl_Spinner *s;
        char *str;
        float val;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        s = EWL_SPINNER(w);
        str = ewl_text_text_get(EWL_TEXT(s->entry));

        if (str && strlen(str)) {
                val = atof(str);
                ewl_range_value_set(EWL_RANGE(s), (double) (val));

        } else if (str)
                FREE(str);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: The Ewl_Event_Mouse_Wheel data
 * @param user_data: The Ewl_Spinner
 * @return Returns no value
 * @brief The mouse wheel callback
 */
void
ewl_spinner_cb_wheel(Ewl_Widget *w, void *ev_data,
                        void *user_data __UNUSED__)
{
        Ewl_Range *r;
        Ewl_Event_Mouse_Wheel *wheel;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(ev_data);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        r = EWL_RANGE(w);
        wheel = (Ewl_Event_Mouse_Wheel *)ev_data;

        ewl_range_value_set(r, r->value - (wheel->z * r->step));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_spinner_entry_update(Ewl_Spinner *s)
{
        Ewl_Range *r;
        char format[64];
        char str[64];

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(s);
        DCHECK_TYPE(s, EWL_SPINNER_TYPE);

        r = EWL_RANGE(s);

        snprintf(format, 64, "%%.%df", s->digits);
        snprintf(str, 64, format, r->value);

        ewl_text_text_set(EWL_TEXT(s->entry), str);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: The event data
 * @param user_data: The Ewl_Spinner
 * @return Returns no value
 * @brief The increase value callback
 */
void
ewl_spinner_cb_increase_value(Ewl_Widget *w __UNUSED__, void *ev_data,
                                        void *user_data)
{
        Ewl_Spinner *s;
        Ewl_Range *r;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(user_data);
        DCHECK_TYPE(user_data, EWL_SPINNER_TYPE);

        s = EWL_SPINNER(user_data);
        r = EWL_RANGE(s);
        ewl_spinner_cb_focus_out(EWL_WIDGET(s), NULL, NULL);

        if (!r->invert)
                ewl_range_increase(r);
        else
                ewl_range_decrease(r);

        if (ev_data) {
                s->direction = (!r->invert) ? 1 : -1;
                s->start_time = ecore_time_get();
                s->last_value = 0.0;
                s->timer = ecore_timer_add(0.02, ewl_spinner_timer, s);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev_data: UNUSED
 * @param user_data: The Ewl_Spinner
 * @return Returns no value
 * @brief The value stop callback
 */
void
ewl_spinner_cb_value_stop(Ewl_Widget *w __UNUSED__,
                        void *ev_data __UNUSED__, void *user_data)
{
        Ewl_Spinner *s;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(user_data);
        DCHECK_TYPE(user_data, EWL_SPINNER_TYPE);

        s = user_data;
        if (s->timer) {
                ecore_timer_del(s->timer);

                s->timer = NULL;
                s->direction = 0;
                s->start_time = 0;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: The event
 * @param user_data: The Ewl_Spinner
 * @return Returns no value
 * @brief The decrease value callback
 */
void
ewl_spinner_cb_decrease_value(Ewl_Widget *w __UNUSED__, void *ev_data,
                                        void *user_data)
{
        Ewl_Spinner *s;
        Ewl_Range *r;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(user_data);
        DCHECK_TYPE(user_data, EWL_SPINNER_TYPE);

        s = EWL_SPINNER(user_data);
        r = EWL_RANGE(s);

        ewl_spinner_cb_focus_out(EWL_WIDGET(s), NULL, NULL);

        if (!r->invert)
                ewl_range_decrease(r);
        else
                ewl_range_increase(r);

        if (ev_data) {
                s->direction = (!r->invert) ? -1 : 1;
                s->start_time = ecore_time_get();
                s->last_value = 0.0;
                s->timer = ecore_timer_add(0.02, ewl_spinner_timer, s);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The destroy callback
 */
void
ewl_spinner_cb_destroy(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Spinner *s;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_SPINNER_TYPE);

        s = EWL_SPINNER(w);
        if (s->timer) {
                ecore_timer_del(s->timer);
                s->timer = NULL;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ewl_spinner_timer(void *data)
{
        Ewl_Spinner *s;
        Ewl_Range *r;
        double dt, dv;
        double step;
        int velocity;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(data, ECORE_CALLBACK_CANCEL);

        s = EWL_SPINNER(data);
        r = EWL_RANGE(s);

        dt = ecore_time_get() - s->start_time;
        step = 0;

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
         * Move the value of the spinner based on the direction of it's motion
         * and the velocity setting.
         */
        dv = velocity * s->direction * 10.0 * r->step * dt*dt;

        while  (r->step < abs(dv - s->last_value - step)) {
                if (s->direction == 1)
                        step += r->step;
                else
                        step -= r->step;
        }

        ewl_range_value_set(r, r->value + step);
        s->last_value += step;

        DRETURN_INT(ECORE_CALLBACK_RENEW, DLEVEL_STABLE);
}

static void
ewl_spinner_child_handle(Ewl_Spinner *s)
{
        int pvw, pvh, pew, peh;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(s);
        DCHECK_TYPE(s, EWL_SPINNER_TYPE);

        pvw = ewl_object_preferred_w_get(EWL_OBJECT(s->vbox));
        pvh = ewl_object_preferred_h_get(EWL_OBJECT(s->vbox));

        pew = ewl_object_preferred_w_get(EWL_OBJECT(s->entry));
        peh = ewl_object_preferred_h_get(EWL_OBJECT(s->entry));

        ewl_object_preferred_inner_size_set(EWL_OBJECT(s), pvw + pew,
                                                        MAX(peh, pvh));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @return Returns no value
 * @brief The child show callback
 */
void
ewl_spinner_cb_child_show(Ewl_Container *c, Ewl_Widget *w __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_SPINNER_TYPE);

        ewl_spinner_child_handle(EWL_SPINNER(c));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @param size: UNUSED
 * @param o: UNUSED
 * @return Returns no value
 * @brief The child resize callback
 */
void
ewl_spinner_cb_child_resize(Ewl_Container *c, Ewl_Widget *w __UNUSED__,
                                int size __UNUSED__,
                                Ewl_Orientation o __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_SPINNER_TYPE);

        ewl_spinner_child_handle(EWL_SPINNER(c));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

