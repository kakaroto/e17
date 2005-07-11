#include "ewl_test.h"


static Ewl_Widget* datepicker_button = NULL;


static void
 __destroy_datepicker_window(Ewl_Widget *main_win , void *ev_data __UNUSED__, void *user_data __UNUSED__)
 {
        ewl_widget_destroy(main_win);
	ewl_callback_append(datepicker_button, EWL_CALLBACK_CLICKED,
                            __create_datepicker_test_window, NULL);
 }

void
__create_datepicker_test_window(Ewl_Widget * w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
		Ewl_Widget* datepicker;
		Ewl_Widget* datepicker_win;
	
		datepicker_button = w;	
		datepicker_win= ewl_window_new();

		if (w) {
                	ewl_callback_del(w, EWL_CALLBACK_CLICKED,
                                        __create_datepicker_test_window);
	                ewl_callback_append(datepicker_win, EWL_CALLBACK_DELETE_WINDOW,
                                    __destroy_datepicker_window, NULL);
        	} else {
                	ewl_callback_append(datepicker_win, EWL_CALLBACK_DELETE_WINDOW,
                                        __close_main_window, NULL);
		}
	
		


		
		ewl_window_title_set(EWL_WINDOW(datepicker_win), "DatePicker");
		ewl_window_name_set(EWL_WINDOW(datepicker_win), "DatePicker");
		ewl_window_class_set(EWL_WINDOW(datepicker_win), "DatePicker");
		ewl_object_size_request(EWL_OBJECT(datepicker_win), 160,40);

		
		datepicker = ewl_datepicker_new();


		ewl_object_fill_policy_set(EWL_OBJECT(datepicker_win), EWL_FLAG_FILL_ALL);
		ewl_container_child_append(EWL_CONTAINER(datepicker_win), datepicker);
		
		ewl_widget_show(datepicker_win);
		ewl_widget_show(datepicker);

		/*ewl_callback_append(EWL_WIDGET(cal), EWL_CALLBACK_VALUE_CHANGED,ewl_calendar_test, cal);*/


}
