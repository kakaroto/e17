#include <Ewl.h>
#include "entropy.h"
#include "entropy_gui.h"
#include "entropy_config.h"
#include <dlfcn.h>
#include <time.h>

void ewl_mime_dialog_display() {
	Ewl_Widget* window = ewl_window_new();
	ewl_object_custom_size_set(EWL_OBJECT(window), 250, 400);
	ewl_widget_show(window);
}
