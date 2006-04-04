#include <Etk.h>
#include "entropy.h"

typedef struct _Entropy_Etk_Properties_Dialog Entropy_Etk_Properties_Dialog;
struct _Entropy_Etk_Properties_Dialog {
	Etk_Widget* window;
};

void etk_properties_dialog_new(Entropy_Generic_File* file) 
{
	Entropy_Etk_Properties_Dialog* dialog;

	dialog = entropy_malloc(sizeof(Entropy_Etk_Properties_Dialog));

	dialog->window = etk_window_new();
	etk_window_title_set(ETK_WINDOW(dialog->window), file->filename);

	etk_widget_size_request_set(dialog->window, 450, 520);
	etk_widget_show_all(dialog->window);
}
