#include "ephoto.h"

static void close_dialog(Ewl_Widget *w, void *event, void *data);
static void add_ok(Ewl_Widget *w, void *event, void *data);
Ewl_Widget *name_entry, *desc_entry;

static void 
close_dialog(Ewl_Widget *w, void *event, void *data)
{
	Ewl_Widget *win;
	
	win = data;

	ewl_widget_destroy(win);
}

void 
about_dialog(Ewl_Widget *w, void *event, void *data)
{
	Ewl_Widget *window, *button, *image, *vbox, *sp, *text;
	
	window = add_window("About Ephoto", 200, 300, NULL, NULL);
        ewl_callback_append(window, EWL_CALLBACK_DELETE_WINDOW, close_dialog, 
					window);

        vbox = add_box(window, EWL_ORIENTATION_VERTICAL, 3);
        ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);

	image = add_image(vbox, PACKAGE_DATA_DIR "/images/photo_lens.png", 0, 
					NULL, NULL);
	ewl_image_constrain_set(EWL_IMAGE(image), 144);
	ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(image), EWL_FLAG_FILL_SHRINK);

	sp = ewl_scrollpane_new();
	ewl_object_fill_policy_set(EWL_OBJECT(sp), EWL_FLAG_FILL_ALL);
	ewl_container_child_append(EWL_CONTAINER(vbox), sp);
	ewl_widget_show(sp);

	text = add_text(sp, "Ephoto is an advanced image viewer that allows\n"
		       "you to view images using several methods. They\n"
		       "include an icon view, a list view, and a single\n"
		       "image view.  You can also view exif data, view\n"
		       "images in a fullscreen mode, and view images in a\n"
		       "slideshow.  The edit view offers simple and advanced\n"
		       "editing options.");

	ewl_text_wrap_set(EWL_TEXT(text), EWL_TEXT_WRAP_WORD);
	ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);

	button = add_button(vbox, "Close",
				PACKAGE_DATA_DIR "/images/dialog-close.png",
				close_dialog, window);
        ewl_button_image_size_set(EWL_BUTTON(button), 25, 25);
	ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_CENTER);	

	return;
}

static void
add_ok(Ewl_Widget *w, void *event, void *data)
{
        Ewl_Widget *win = data;
        char *name, *desc;

        name = ewl_text_text_get(EWL_TEXT(name_entry));
        desc = ewl_text_text_get(EWL_TEXT(desc_entry));

        if (name)
        {
                ephoto_db_add_album(em->db, name, desc);
                populate_albums(NULL, NULL, NULL);
        }

        ewl_widget_destroy(win);
}

void 
add_album_dialog(Ewl_Widget *w, void *event, void *data)
{
        Ewl_Widget *window, *vbox, *hbox;

        window = add_window("Add Album", 340, 160, NULL, NULL);
        ewl_callback_append(window, EWL_CALLBACK_DELETE_WINDOW, close_dialog, 
					window);

        vbox = add_box(window, EWL_ORIENTATION_VERTICAL, 5);
        ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);

        add_text(vbox, "Please provide a name for the new album:");

        name_entry = add_entry(vbox, "New Album", NULL, NULL);

        add_text(vbox, "Please provide a short description for this album:");

        desc_entry = add_entry(vbox, "Album Description", NULL, NULL);

        hbox = add_box(vbox, EWL_ORIENTATION_HORIZONTAL, 5);
        ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_SHRINK);

        add_button(hbox, "Save", NULL, add_ok, window);

        add_button(hbox, "Cancel", NULL, close_dialog, window);
}
