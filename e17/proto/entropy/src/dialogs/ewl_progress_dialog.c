#include "entropy.h"
#include <Ewl.h>
#include "entropy_gui.h"
#include "ewl_progress_dialog.h"

void ewl_progress_window_create(entropy_file_progress_window* progress) {
	Ewl_Widget* vbox;
	
	progress->progress_window = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(progress->progress_window), "File Copy");
	ewl_object_custom_size_set(EWL_OBJECT(progress->progress_window), 400, 150);
	
	vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(progress->progress_window), vbox);
	ewl_widget_show(vbox);

	progress->file_from = ewl_text_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), progress->file_from);
	ewl_widget_show(progress->file_from);

	progress->file_to = ewl_text_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), progress->file_to);
	ewl_widget_show(progress->file_to);

	progress->progressbar = ewl_progressbar_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), progress->progressbar);
	ewl_progressbar_range_set(EWL_PROGRESSBAR(progress->progressbar), 100);
	ewl_object_custom_h_set(EWL_OBJECT(progress->progressbar), 30);
	ewl_widget_show(progress->progressbar);

}
