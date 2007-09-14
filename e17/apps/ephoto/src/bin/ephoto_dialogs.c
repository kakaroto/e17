#include "ephoto.h"

static void close_dialog(Ewl_Widget *w, void *event, void *data);

static Ewl_Widget *save_quality, *save_entry;

/*Close or Cancel the Dialog*/
static void close_dialog(Ewl_Widget *w, void *event, void *data)
{
	Ewl_Widget *win;
	
	win = data;

	ewl_widget_destroy(win);
}

/*Add an About Dialog*/
void about_dialog(Ewl_Widget *w, void *event, void *data)
{
	Ewl_Widget *window, *button, *vbox, *text;
	
	window = add_window("About Ephoto", 200, 100, NULL, NULL);
        ewl_callback_append(window, EWL_CALLBACK_DELETE_WINDOW, close_dialog, window);

        vbox = add_box(window, EWL_ORIENTATION_VERTICAL, 3);
        ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);

	text = add_text(vbox, "Ephoto is an advanced image viewer that allows\n"
		       "you to view images in several methods. They\n"
		       "include an icon view, a list view, and a single\n"
		       "image view.  You can also view exif data, view\n"
		       "images in a fullscreen mode, and view images in a\n"
		       "slideshow.  The edit view offers simple and advanced\n"
		       "editing options including rotations, flips, blurs,\n"
		       "sharpens, conversion to black and white, and\n"
		       "conversions to sepia.");

	ewl_text_wrap_set(EWL_TEXT(text), EWL_TEXT_WRAP_WORD);

	button = add_button(vbox, "Close",
                                PACKAGE_DATA_DIR "/images/dialog-close.png",
                                                                close_dialog, window);
        ewl_button_image_size_set(EWL_BUTTON(button), 25, 25);
	
	return;
}

static void save_image(Ewl_Widget *w, void *event, void *data)
{
	const char *file;
	char flags[PATH_MAX];
	char *ext;

	file = ewl_text_text_get(EWL_TEXT(save_entry));

	ext = strrchr(file, '.')+1;
	if (!strncmp(ext, "png", 3))
	{
		snprintf(flags, PATH_MAX, "compress=%i",
                 	       (int)ewl_range_value_get(EWL_RANGE(save_quality)));
	}
	else
	{
		double svalue;
		float jvalue;
 
		svalue = ewl_range_value_get(EWL_RANGE(save_quality));
		jvalue = (svalue / 9) * 100;

		snprintf(flags, PATH_MAX, "quality=%.0f", jvalue);
	}

	if(!file) return;

        if(VISIBLE(em->eimage))
        {
        	evas_object_image_save(EWL_IMAGE(em->eimage)->image,
                                                        file, NULL, flags);
        }
	
	ewl_widget_destroy(EWL_WIDGET(data));
}

void save_dialog(const char *file)
{
        Ewl_Widget *save_win, *vbox, *hbox, *button;
        
	save_win = add_window("Save Image", 300, 100, NULL, NULL);
	ewl_callback_append(save_win, EWL_CALLBACK_DELETE_WINDOW, close_dialog, save_win);        

	vbox = add_box(save_win, EWL_ORIENTATION_VERTICAL, 5);
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);

	add_label(vbox, "Save As:");

	save_entry = add_entry(vbox, "default.jpg", NULL, NULL);

	add_label(vbox, "Quality/Compression:");

	save_quality = ewl_hseeker_new();
	ewl_range_minimum_value_set(EWL_RANGE(save_quality), 1);
	ewl_range_maximum_value_set(EWL_RANGE(save_quality), 9);
	ewl_range_step_set(EWL_RANGE(save_quality), 1);
	ewl_range_value_set(EWL_RANGE(save_quality), 7);
	ewl_container_child_append(EWL_CONTAINER(vbox), save_quality);
	ewl_widget_show(save_quality);

	hbox = add_box(vbox, EWL_ORIENTATION_HORIZONTAL, 5);
	ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_SHRINK);

	button = add_button(hbox, "Save", 
				PACKAGE_DATA_DIR "/images/stock_save.png", 
							save_image, save_win);
	ewl_button_image_size_set(EWL_BUTTON(button), 25, 25);	

	button = add_button(hbox, "Close", 
				PACKAGE_DATA_DIR "/images/dialog-close.png", 
							close_dialog, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 25, 25);

	return;
}

