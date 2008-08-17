#include "entropy.h"
#include <Etk.h>
#include "entropy_gui.h"
#include <limits.h>

entropy_file_progress_window* entropy_etk_progress_window_create() 
{
	entropy_file_progress_window* dialog = entropy_malloc(sizeof(entropy_file_progress_window));
	Etk_Widget* vbox;
	

	dialog->progress_window = etk_window_new();
	etk_window_title_set(ETK_WINDOW(dialog->progress_window), _("File Progress"));
	
	vbox = etk_vbox_new(ETK_TRUE, 5);
	etk_container_add(ETK_CONTAINER(dialog->progress_window), vbox);

	dialog->file_from = etk_label_new( _("File From"));
	etk_box_append(ETK_BOX(vbox), dialog->file_from, ETK_BOX_START, ETK_BOX_EXPAND_FILL,0);
	
	dialog->file_to = etk_label_new(_("File To"));
 	etk_box_append(ETK_BOX(vbox), dialog->file_to, ETK_BOX_START, ETK_BOX_EXPAND_FILL,0);
	
	dialog->progressbar = etk_progress_bar_new_with_text(_("0% done"));
	etk_box_append(ETK_BOX(vbox), dialog->progressbar, ETK_BOX_END, ETK_BOX_EXPAND_FILL,0 );
	
	return dialog;
}

void entropy_etk_progress_dialog_set_file_from_to(entropy_file_progress_window* dialog, 
		entropy_generic_file* file_from, entropy_generic_file* file_to)
{

	char buffer[PATH_MAX];

	if (file_from) {
		snprintf(buffer, PATH_MAX, "%s://%s/%s", file_from->uri_base, file_from->path, file_from->filename);
		etk_label_set(ETK_LABEL(dialog->file_from), buffer);
	} else {
		etk_label_set(ETK_LABEL(dialog->file_from), "");
	}

	if (file_to) {
		snprintf(buffer, PATH_MAX, "%s://%s/%s", file_to->uri_base, file_to->path, file_to->filename);
		etk_label_set(ETK_LABEL(dialog->file_to), buffer);
	} else {
		etk_label_set(ETK_LABEL(dialog->file_to), "");
	}

}

void entropy_etk_progress_dialog_set_progress_pct(entropy_file_progress_window* dialog, float* pct) 
{
	char text[32];
	
	snprintf(text, 32, "%.2f%% done", *pct);
	etk_progress_bar_text_set(ETK_PROGRESS_BAR(dialog->progressbar), text);

	etk_progress_bar_fraction_set(ETK_PROGRESS_BAR(dialog->progressbar), *pct/100.0);
}

void entropy_etk_progress_dialog_show(entropy_file_progress_window* dialog)
{
	etk_widget_show_all(ETK_WIDGET(dialog->progress_window));
}

void entropy_etk_progress_dialog_destroy(entropy_file_progress_window* dialog)
{
	etk_object_destroy(ETK_OBJECT(dialog->progress_window));
	entropy_free(dialog);
}

void entropy_etk_progress_dialog_hide(entropy_file_progress_window* dialog)
{
	etk_widget_hide(ETK_WIDGET(dialog->progress_window));
}

