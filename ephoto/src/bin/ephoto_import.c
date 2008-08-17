#include "ephoto.h"

static void close_dialog(Ewl_Widget *w, void *event, void *data);
static void import(Ewl_Widget *w, void *event, void *data);
Ewl_Widget *name_entry, *desc_entry;

static void
close_dialog(Ewl_Widget *w, void *event, void *data)
{
        Ewl_Widget *win;

        win = data;

        ewl_widget_destroy(win);
}

static void 
import(Ewl_Widget *w, void *event, void *data)
{
	char *name, *desc, *image;
	Ecore_List  *selected_images;
	Ewl_Event_Action_Response *e;
	Ewl_Widget *win;

	win = data;

	e = event;

	if (e->response == EWL_STOCK_CANCEL)
	{
		ewl_widget_destroy(win);
		return;
	}

	selected_images = ewl_filepicker_selected_files_get(EWL_FILEPICKER(w));
	if (ecore_list_empty_is(selected_images))
		return;

	name = ewl_text_text_get(EWL_TEXT(name_entry));
	desc = ewl_text_text_get(EWL_TEXT(desc_entry));

	if (!name)
		return;
	else
		ephoto_db_add_album(em->db, name, desc);

	while (!ecore_list_empty_is(selected_images))
	{
		image = ecore_list_first_remove(selected_images);
		ephoto_db_add_image(em->db, name, basename(image), image);
	}

	ewl_widget_destroy(win);
	populate_albums(NULL, NULL, NULL);

	return;
}

void 
ephoto_import_dialog(Ewl_Widget *w, void *event, void *data)
{
	Ecore_List *filters;
        Ewl_Widget *fpick, *vbox, *window;
	
	filters = ecore_list_new();

	ecore_list_append(filters, "image/gif");
        ecore_list_append(filters, "image/jpeg");
        ecore_list_append(filters, "image/png");
        ecore_list_append(filters, "image/svg+xml");
        ecore_list_append(filters, "image/tiff");

        window = add_window("Import Photos", 400, 400, NULL, NULL);
        ewl_callback_append(window, EWL_CALLBACK_DELETE_WINDOW, close_dialog,
                                        window);

        vbox = add_box(window, EWL_ORIENTATION_VERTICAL, 10);
        ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);

        add_label(vbox, "Please provide a name for the new album:");

        name_entry = add_entry(vbox, "New Album", NULL, NULL);

        add_label(vbox, "Please provide a short description for this album:");

        desc_entry = add_entry(vbox, "Album Description", NULL, NULL);

	fpick = ewl_filepicker_new();
	ewl_filepicker_directory_set(EWL_FILEPICKER(fpick), em->current_directory);
	ewl_filepicker_multiselect_set(EWL_FILEPICKER(fpick), TRUE);
	ewl_filepicker_filter_add(EWL_FILEPICKER(fpick), "Images", ".gif, .jpg, .png", filters);
	ewl_container_child_append(EWL_CONTAINER(vbox), fpick);
	ewl_callback_append(fpick, EWL_CALLBACK_VALUE_CHANGED, import, window);
	ewl_widget_show(fpick);

        return;
}

