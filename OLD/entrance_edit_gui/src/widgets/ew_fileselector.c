#include "Entrance_Widgets.h"

Entrance_Dialog
ew_fileselector_new(const char *title, const char *directory, int multiple, int showdot,
			void (*response)(void *, int, void *), void *data) {
	Entrance_Dialog dialog = ew_notice_new(title);
	dialog->hbox = etk_filechooser_widget_new();

	if(directory)
		etk_filechooser_widget_current_folder_set(ETK_FILECHOOSER_WIDGET(dialog->hbox), directory);
	
	etk_filechooser_widget_select_multiple_set(ETK_FILECHOOSER_WIDGET(dialog->hbox), multiple);
	etk_filechooser_widget_show_hidden_set(ETK_FILECHOOSER_WIDGET(dialog->hbox), showdot);
	
	etk_box_append(ETK_BOX(dialog->box), dialog->hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

	ew_notice_cancel_button_add(dialog, response, data);
	ew_notice_ok_button_add(dialog, NULL, NULL);
	
	return dialog;
}

const char *
ew_fileselector_file_get(Entrance_Dialog dialog) {
	return etk_filechooser_widget_selected_file_get(ETK_FILECHOOSER_WIDGET(dialog->hbox));
}

Eina_List *
ew_fileselector_file_list_get(Entrance_Dialog dialog) {
	return etk_filechooser_widget_selected_files_get(ETK_FILECHOOSER_WIDGET(dialog->hbox));
}
