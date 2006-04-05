#ifndef _ETK_PROGRESS_DIALOG_H_
#define _ETK_PROGRESS_DIALOG_H_

#include "entropy_gui.h"

entropy_file_progress_window* entropy_etk_progress_window_create();

void entropy_etk_progress_dialog_set_file_from_to(entropy_file_progress_window* 
		dialog, 
	entropy_generic_file* file_from, entropy_generic_file* file_to);

void entropy_etk_progress_dialog_set_progress_pct(entropy_file_progress_window*
		dialog, float* pct);
void entropy_etk_progress_dialog_show(entropy_file_progress_window* dialog);
void entropy_etk_progress_dialog_hide(entropy_file_progress_window* dialog);
void entropy_etk_progress_dialog_destroy(entropy_file_progress_window* dialog);


#endif
