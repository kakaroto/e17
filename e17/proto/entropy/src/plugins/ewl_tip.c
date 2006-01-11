#include <Ewl.h>
#include "entropy.h"
#include "entropy_gui.h"
#include "entropy_config.h"
#include <dlfcn.h>
#include <time.h>

void ewl_entropy_tip_window_destroy_cb(Ewl_Widget* w, void* ev_data, void* user_data);

void ewl_entropy_tip_window_destroy_cb(Ewl_Widget* w, void* ev_data, void* user_data) 
{
	ewl_widget_destroy(w);
}


void ewl_entropy_tip_window_display() 
{
	Ewl_Widget* tip_window = ewl_window_new();
	ewl_object_custom_size_set(EWL_OBJECT(tip_window), 300, 150);
	ewl_callback_append(tip_window, EWL_CALLBACK_DELETE_WINDOW, ewl_entropy_tip_window_destroy_cb, NULL);
	ewl_widget_show(tip_window);
}
