#ifndef __ENTROPY_EWL_PROGRESS_DIALOG_H
#define __ENTROPY_EWL_PROGRESS_DIALOG_H

typedef struct entropy_file_progress_window {
	Ewl_Widget* progress_window;
	Ewl_Widget* file_from;
	Ewl_Widget* file_to;
	Ewl_Widget* progressbar;
} entropy_file_progress_window;

void ewl_progress_window_create(entropy_file_progress_window* progress);

#endif

