#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static void ewl_datepicker_calendar_position_set(Ewl_Datepicker *dp);
static void ewl_datepicker_dropdown_cb(Ewl_Widget *w, void *ev_data, 
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
	if (!ib) {
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

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
	DCHECK_PARAM_PTR_RET("dp", dp, FALSE);

	if (!ewl_text_init(EWL_TEXT(dp)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_text_text_set(EWL_TEXT(dp), "Date Test");

	/* Init ewl setup */
	ewl_widget_appearance_set(EWL_WIDGET(dp), EWL_DATEPICKER_TYPE);
	ewl_widget_inherit(EWL_WIDGET(dp), EWL_DATEPICKER_TYPE);

	dp->calendar_window = ewl_window_new();
	ewl_object_custom_size_set(EWL_OBJECT(dp->calendar_window), 159, 170);
	ewl_object_fill_policy_set(EWL_OBJECT(dp->calendar_window), 
						EWL_FLAG_FILL_FILL);
        ewl_widget_layer_set(dp->calendar_window, 1000);
        ewl_window_borderless_set(EWL_WINDOW(dp->calendar_window));

	dp->calendar = ewl_calendar_new();
	ewl_container_child_append(EWL_CONTAINER(dp->calendar_window), 
							dp->calendar);
	ewl_callback_append(EWL_WIDGET(dp->calendar), 
				EWL_CALLBACK_VALUE_CHANGED, 
				ewl_datepicker_value_changed_cb, dp);
	ewl_widget_show(dp->calendar);

	ewl_datepicker_calendar_position_set(dp);

	ewl_callback_prepend(EWL_WIDGET(dp), EWL_CALLBACK_DESTROY, 
				ewl_datepicker_destroy_cb, dp);
	ewl_callback_append(EWL_WIDGET(dp), EWL_CALLBACK_CONFIGURE,
	                            ewl_datepicker_configure_cb, NULL);
	ewl_callback_append(EWL_WIDGET(dp), EWL_CALLBACK_MOUSE_DOWN, 
				ewl_datepicker_dropdown_cb, NULL);
	
	ewl_callback_call(EWL_WIDGET(dp->calendar), EWL_CALLBACK_VALUE_CHANGED);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

void
ewl_datepicker_destroy_cb(Ewl_Widget *w, void *ev __UNUSED__,
					void *user_data __UNUSED__) 
{
	Ewl_Datepicker *dp;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	dp = EWL_DATEPICKER(w);
	ewl_widget_destroy(dp->calendar_window);
	ewl_widget_destroy(dp->calendar);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_datepicker_configure_cb(Ewl_Widget *w, void *ev __UNUSED__, 
					void *user_data __UNUSED__) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_datepicker_calendar_position_set(EWL_DATEPICKER(w));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_datepicker_value_changed_cb(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
							void *user_data) 
{
	char *date;
	Ewl_Datepicker* dp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);

	dp = EWL_DATEPICKER(user_data);
	ewl_widget_hide(dp->calendar_window);

	date = ewl_calendar_ascii_time_get(EWL_CALENDAR(dp->calendar));
	ewl_text_text_set(EWL_TEXT(dp), date);
	FREE(date);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_datepicker_realize_cb(Ewl_Widget *w, void *ev __UNUSED__, 
						void *user_data __UNUSED__) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_datepicker_calendar_position_set(EWL_DATEPICKER(w));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_datepicker_calendar_position_set(Ewl_Datepicker *dp) 
{
	int x, y;
	int sx, sy;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("dp", dp);
	DCHECK_TYPE("dp", dp, EWL_DATEPICKER_TYPE);

	/* Get the position of the parent */
	emb = ewl_embed_widget_find(EWL_WIDGET(dp));
	if (emb) {
		ewl_window_position_get(EWL_WINDOW(emb), &x, &y);
		ewl_object_current_size_get(EWL_OBJECT(dp), &sx, &sy);
		ewl_window_move(EWL_WINDOW(dp->calendar_window), x + (sx / 4), 
								y + sy + 3);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_datepicker_dropdown_cb(Ewl_Widget *w, void *ev __UNUSED__, 
					void *user_data __UNUSED__) 
{
	Ewl_Datepicker *dp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	dp = EWL_DATEPICKER(w);
	ewl_widget_show(dp->calendar_window);
	ewl_window_raise(EWL_WINDOW(dp->calendar_window));
	ewl_datepicker_calendar_position_set(dp);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


