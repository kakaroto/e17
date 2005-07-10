#include "ewl_test.h"

Ewl_Widget* cal;
Ewl_Widget* win;
Ewl_Widget *calendar_button;

void
 __destroy_calendar_test_window(Ewl_Widget *w, void *ev_data, void *user_data)
 {
        ewl_widget_destroy(w);
        ewl_callback_append(calendar_button, EWL_CALLBACK_CLICKED,
                            __create_calendar_test_window, NULL);


        return;
 }

void
ewl_calendar_test(Ewl_Widget *main_win, void *ev_data, void *user_data)
 {
	 char date[1024];
	 ewl_calendar_ascii_time_get(EWL_CALENDAR(user_data), date);
	 printf ("Selected: %s\n", date);
 }


void
__create_calendar_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
		calendar_button = w;
	
		win= ewl_window_new();
		
		ewl_window_title_set(EWL_WINDOW(win), "Calendar");
		ewl_window_name_set(EWL_WINDOW(win), "Calendar");
		ewl_window_class_set(EWL_WINDOW(win), "Calendar");
		ewl_object_size_request(EWL_OBJECT(win), 159,160);

		if (w) {
                	ewl_callback_del(w, EWL_CALLBACK_CLICKED,
                                        __create_calendar_test_window);
	                ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW,
                                    __destroy_calendar_test_window, NULL);
        	} else
                	ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW,
                                        __close_main_window, NULL);


		
		cal = ewl_calendar_new();


		ewl_object_fill_policy_set(EWL_OBJECT(win), EWL_FLAG_FILL_ALL);
		ewl_container_child_append(EWL_CONTAINER(win), cal);
		
		ewl_widget_show(win);
		ewl_widget_show(cal);

		 ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW,
                            __destroy_calendar_test_window, NULL);

		ewl_callback_append(EWL_WIDGET(cal), EWL_CALLBACK_VALUE_CHANGED,ewl_calendar_test, cal);


}
