#include <Ewl.h>
#include <time.h>
#include "ewl_debug.h"
#include <Evas.h>
#include "ewl_macros.h"
#include <stdlib.h>
#include <assert.h>

/**
 * @return Returns NULL on failure, a new Ewl_DatePicker on success
 * @brief Creates a new Ewl_DatePicker
 */
Ewl_Widget *ewl_datepicker_new() {
	Ewl_DatePicker* ib;
	DENTER_FUNCTION (DLEVEL_STABLE);

	ib = NEW(Ewl_DatePicker, 1);
	if (!ib) {
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	if (!ewl_datepicker_init(ib)) {
		printf("Failed datepicker init...\n");
		FREE(ib);
		ib = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(ib), DLEVEL_STABLE);
}

void ewl_datepicker_calendar_position_set(Ewl_DatePicker* dp) {
	int x,y;
	int sx,sy;
	Ewl_Embed* emb;

	/* Get the position of the parent */
	emb = ewl_embed_widget_find(EWL_WIDGET(dp));
	ewl_window_position_get(EWL_WINDOW(emb), &x,&y);
	ewl_object_current_size_get(EWL_OBJECT(dp), &sx,&sy);
	ewl_window_move(EWL_WINDOW(dp->calendar_window), x+(sx/4),y+sy+3);

}

void
ewl_datepicker_dropdown(Ewl_Widget *w, void *ev_data, void *user_data) {
	
	Ewl_DatePicker *dp = EWL_DATEPICKER(w);
	/*printf("Drop down..\n");*/	
	ewl_widget_show(dp->calendar_window);
	ewl_window_raise(EWL_WINDOW(dp->calendar_window));
	ewl_datepicker_calendar_position_set(dp);
	

	//ewl_container_child_append(EWL_CONTAINER(dp), dp->calendar_window);


}

void
ewl_datepicker_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data) {
	ewl_datepicker_calendar_position_set(EWL_DATEPICKER(w));
}

void
ewl_datepicker_value_changed_cb(Ewl_Widget *w, void *ev_data, void *user_data) {
	static char date[1024];
	Ewl_DatePicker* dp = EWL_DATEPICKER(user_data);
	ewl_widget_hide(dp->calendar_window);

	ewl_calendar_ascii_time_get(EWL_CALENDAR(dp->calendar), date);
	ewl_text_text_set(EWL_TEXT(dp), date);
}

void ewl_datepicker_realize_cb(Ewl_Widget *w, void *ev_data, void *user_data) {
	ewl_datepicker_calendar_position_set(EWL_DATEPICKER(w));
}

int ewl_datepicker_init(Ewl_DatePicker* dp) {
	Ewl_Widget* w	;
	
	w = EWL_WIDGET(dp);
	
	if (!ewl_text_init(EWL_TEXT(dp), "Date Test"))
			DRETURN_INT(FALSE, DLEVEL_STABLE);

	/* Init ewl setup */
	ewl_widget_appearance_set(EWL_WIDGET(w), "datepicker");
	ewl_widget_inherit(EWL_WIDGET(w), "datepicker");

	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN, ewl_datepicker_dropdown, NULL);
	dp->calendar = NULL;
	dp->calendar_window = NULL;
	
	
	dp->calendar_window = ewl_window_new();
	dp->calendar = ewl_calendar_new();
	ewl_object_custom_size_set(EWL_OBJECT(dp->calendar_window), 159, 170);
	ewl_object_fill_policy_set(EWL_OBJECT(dp->calendar_window), EWL_FLAG_FILL_FILL);
	ewl_container_child_append(EWL_CONTAINER(dp->calendar_window), dp->calendar);
        ewl_window_borderless_set(EWL_WINDOW(dp->calendar_window));
        /*ewl_widget_internal_set(dp->calendar_window, TRUE);*/
        ewl_widget_layer_set(dp->calendar_window, 1000);

	ewl_callback_append(EWL_WIDGET(dp), EWL_CALLBACK_CONFIGURE,
                            ewl_datepicker_configure_cb, NULL);
	ewl_widget_show(dp->calendar);

	ewl_datepicker_calendar_position_set(dp);
	/*ewl_widget_show(dp->calendar_window);
	ewl_widget_hide(dp->calendar_window);*/	

	ewl_callback_append(EWL_WIDGET(dp->calendar), EWL_CALLBACK_VALUE_CHANGED, ewl_datepicker_value_changed_cb, dp);
	ewl_callback_call(EWL_WIDGET(dp->calendar), EWL_CALLBACK_VALUE_CHANGED);


	DRETURN_INT(TRUE, DLEVEL_STABLE);
}


