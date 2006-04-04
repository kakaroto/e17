#include <Etk.h>
#include "entropy.h"

typedef struct _Entropy_Etk_Properties_Dialog Entropy_Etk_Properties_Dialog;
struct _Entropy_Etk_Properties_Dialog {
	Etk_Widget* window;
	Entropy_Generic_File* file;
};

Etk_Bool _etk_window_deleted_cb (Etk_Object * object, void *data) 
{
	Entropy_Etk_Properties_Dialog* dialog= data;

	/*Remove the file reference*/
	entropy_core_file_cache_remove_reference(dialog->file);

	etk_object_destroy(ETK_OBJECT(dialog->window));
	entropy_free(dialog);

	return ETK_TRUE;
}

void etk_properties_dialog_new(Entropy_Generic_File* file) 
{
	Entropy_Etk_Properties_Dialog* dialog;

	/*First up, add a reference to this file*/
	printf("Adding reference to '%s/%s'- > %s\n", file->path, file->filename, file->md5);
	entropy_core_file_cache_add_reference(file->md5);

	dialog = entropy_malloc(sizeof(Entropy_Etk_Properties_Dialog));
	dialog->file = file;

	dialog->window = etk_window_new();
	etk_window_title_set(ETK_WINDOW(dialog->window), file->filename);

	etk_widget_size_request_set(dialog->window, 450, 520);
	etk_widget_show_all(dialog->window);
}
