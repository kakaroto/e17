#include "ephoto.h"

static void close_dialog(Ewl_Widget *w, void *event, void *data);

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
	char *txt, title[PATH_MAX];
	Ewl_Widget *window, *button, *image, *vbox, *sp, *text;
	
	window = add_window("About Ephoto", 300, 375, NULL, NULL);
	ewl_window_dialog_set(EWL_WINDOW(window), TRUE);
        ewl_callback_append(window, EWL_CALLBACK_DELETE_WINDOW, close_dialog, 
					window);

        vbox = add_box(window, EWL_ORIENTATION_VERTICAL, 0);
        ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);

	image = add_image(vbox, PACKAGE_DATA_DIR "/images/photo_lens.png", 0, 
					NULL, NULL);
	ewl_image_constrain_set(EWL_IMAGE(image), 144);
	ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(image), EWL_FLAG_FILL_SHRINK);

	sp = ewl_scrollpane_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), sp);
	ewl_widget_show(sp);

	snprintf(title, PATH_MAX, "Ephoto - Version %s", VERSION);
	txt = "Ephoto is a photo management application that "
		"organizes images and allows viewing and editing of "
		"them.  It includes a standard thumbnail browser "
		"and a single picture browser that allows for "
		"editing.  Features include viewing exif data, "
		"viewing slideshows, and exporting your images.";

	text = ewl_text_new();
	ewl_text_wrap_set(EWL_TEXT(text), EWL_TEXT_WRAP_WORD);
	ewl_text_font_source_set(EWL_TEXT(text), ewl_theme_path_get(), 
						"ewl/default/bold");
	ewl_text_font_size_set(EWL_TEXT(text), 20);
	ewl_text_align_set(EWL_TEXT(text), EWL_FLAG_ALIGN_CENTER);
	ewl_text_styles_set(EWL_TEXT(text), EWL_TEXT_STYLE_SOFT_SHADOW);
	ewl_text_text_append(EWL_TEXT(text), title);
	ewl_text_align_set(EWL_TEXT(text), EWL_FLAG_ALIGN_LEFT);
	ewl_text_styles_set(EWL_TEXT(text), EWL_TEXT_STYLE_NONE);
	ewl_text_font_set(EWL_TEXT(text), NULL);
	ewl_text_font_size_set(EWL_TEXT(text), 14);
	ewl_text_text_append(EWL_TEXT(text), "\n\n");
	ewl_text_text_append(EWL_TEXT(text), txt);
	ewl_container_child_append(EWL_CONTAINER(sp), text);
	ewl_widget_show(text);

	button = add_button(vbox, "Close",
				PACKAGE_DATA_DIR "/images/dialog-close.png",
				close_dialog, window);
        ewl_button_image_size_set(EWL_BUTTON(button), 25, 25);
	ewl_object_alignment_set(EWL_OBJECT(button), EWL_FLAG_ALIGN_CENTER);	

	return;
}

