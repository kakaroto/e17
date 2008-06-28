/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_datepicker.h"
#include "ewl_calendar.h"
#include "ewl_popup.h"
#include "ewl_private.h"
#include "ewl_macros.h"
#include "ewl_debug.h"

static void ewl_datepicker_cb_dropdown(Ewl_Widget *w, void *ev_data,
                                                        void *user_data);

/**
 * @return Returns NULL on failure, a new Ewl_Datepicker on success
 * @brief Creates a new Ewl_Datepicker
 */
Ewl_Widget *
ewl_datepicker_new(void)
{
        Ewl_Datepicker *ib;

        DENTER_FUNCTION(DLEVEL_STABLE);

        ib = NEW(Ewl_Datepicker, 1);
        if (!ib)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_datepicker_init(ib)) {
                ewl_widget_destroy(EWL_WIDGET(ib));
                ib = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(ib), DLEVEL_STABLE);
}

/**
 * @param dp: The Ewl_Datepicker to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initialize the date picker to default values.
 */
int
ewl_datepicker_init(Ewl_Datepicker *dp)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(dp, FALSE);

        if (!ewl_text_init(EWL_TEXT(dp)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(EWL_WIDGET(dp), EWL_DATEPICKER_TYPE);
        ewl_widget_inherit(EWL_WIDGET(dp), EWL_DATEPICKER_TYPE);
        ewl_object_fill_policy_set(EWL_OBJECT(dp), EWL_FLAG_FILL_NONE);

        dp->calendar_window = ewl_popup_new();
        ewl_widget_appearance_set(dp->calendar_window,
                                EWL_DATEPICKER_TYPE"/"EWL_POPUP_TYPE);
        ewl_object_fill_policy_set(EWL_OBJECT(dp->calendar_window),
                                                EWL_FLAG_FILL_HFILL);
        ewl_popup_type_set(EWL_POPUP(dp->calendar_window),
                                        EWL_POPUP_TYPE_MENU_VERTICAL);
        ewl_popup_follow_set(EWL_POPUP(dp->calendar_window), EWL_WIDGET(dp));
        ewl_popup_fit_to_follow_set(EWL_POPUP(dp->calendar_window), TRUE);
        ewl_window_keyboard_grab_set(EWL_WINDOW(dp->calendar_window), TRUE);
        ewl_window_pointer_grab_set(EWL_WINDOW(dp->calendar_window), TRUE);
        ewl_callback_append(dp->calendar_window, EWL_CALLBACK_MOUSE_DOWN,
                                ewl_datepicker_cb_window_mouse_down, dp);

        dp->calendar = ewl_calendar_new();
        ewl_object_fill_policy_set(EWL_OBJECT(dp->calendar),
                                                        EWL_FLAG_FILL_HFILL);
        ewl_container_child_append(EWL_CONTAINER(dp->calendar_window),
                                                        dp->calendar);
        ewl_callback_append(EWL_WIDGET(dp->calendar),
                                EWL_CALLBACK_VALUE_CHANGED,
                                ewl_datepicker_cb_value_changed, dp);
        ewl_widget_show(dp->calendar);

        ewl_callback_prepend(EWL_WIDGET(dp), EWL_CALLBACK_DESTROY,
                                ewl_datepicker_cb_destroy, dp);
        ewl_callback_append(EWL_WIDGET(dp), EWL_CALLBACK_MOUSE_DOWN,
                                ewl_datepicker_cb_dropdown, NULL);

        ewl_callback_call(EWL_WIDGET(dp->calendar), EWL_CALLBACK_VALUE_CHANGED);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The destroy callback
 */
void
ewl_datepicker_cb_destroy(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Datepicker *dp;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_DATEPICKER_TYPE);

        dp = EWL_DATEPICKER(w);
        ewl_widget_destroy(dp->calendar_window);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: UNUSED
 * @param ev: UNUSED
 * @param user_data: The datepicker
 * @return Returns no value
 * @brief Callback for when the datepicker calendar window value changes
 */
void
ewl_datepicker_cb_value_changed(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                        void *user_data)
{
        char *date;
        Ewl_Datepicker* dp;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(user_data);
        DCHECK_TYPE(user_data, EWL_DATEPICKER_TYPE);

        dp = EWL_DATEPICKER(user_data);
        ewl_widget_hide(dp->calendar_window);

        date = ewl_calendar_ascii_time_get(EWL_CALENDAR(dp->calendar));
        ewl_text_text_set(EWL_TEXT(dp), date);
        FREE(date);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: the calendar window
 * @param ev: UNUSED
 * @param user_data: The datepicker
 * @return Returns no value
 * @brief Callback to hide the calendar window
 */
void
ewl_datepicker_cb_window_mouse_down(Ewl_Widget *w, void *ev __UNUSED__,
                                                void *user_data __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_EMBED_TYPE);

        if (w == ewl_embed_focused_widget_get(EWL_EMBED(w)))
                ewl_widget_hide(w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_datepicker_cb_dropdown(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Datepicker *dp;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_DATEPICKER_TYPE);

        dp = EWL_DATEPICKER(w);
        ewl_widget_show(dp->calendar_window);
        ewl_window_raise(EWL_WINDOW(dp->calendar_window));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

