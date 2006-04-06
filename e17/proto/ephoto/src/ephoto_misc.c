#include "ephoto.h"

/****************CALLBACKS*******************/
void
destroy_cb(Ewl_Widget * w, void *event, void *data)
{
	/****Shutdown Ewl Completely(Main Window close)****/
	ewl_widget_destroy(w);
	ewl_main_quit();
	return;
	event = NULL;
	data = NULL;
	/*************************************************/
}

void
destroys_cb(Ewl_Widget * w, void *event, void *data)
{
	/****Shutdown the slideshow****/
	ewl_widget_destroy(w);
	ecore_timer_del(s->timer);
	wino = 0;
	if ( mainwin == 0 ) {
		ewl_main_quit();
	}
	return;
	data = NULL;
	/*****************************/
}

void
destroyp_cb(Ewl_Widget * w, void *event, void *data)
{
	/****Shut down the presentation****/
	ewl_widget_destroy(s->wins);
	wino = 0;
	if ( mainwin == 0 ) {
		ewl_main_quit();
	}
	return;
	data = NULL;
	/**********************************/
}
void
destroywin_cb(Ewl_Widget * w, void *event, void *data)
{
	ewl_widget_destroy(data);
	return;
	data = NULL;
}

void
rad_cb(Ewl_Widget * w, void *event, void *data)
{
	/****Setup Callbacks for the radio buttons for size/length****/
	if ( w == m->slidetime ) {
		ewl_checkbutton_checked_set(EWL_CHECKBUTTON(m->audiolen), FALSE);
	}
	if ( w == m->fullrad ) {
		ewl_text_text_set(EWL_TEXT(m->wsize), "Full");
		ewl_text_text_set(EWL_TEXT(m->hsize), "Full");
		ewl_entry_editable_set(EWL_ENTRY(m->wsize), 0);
		ewl_entry_editable_set(EWL_ENTRY(m->hsize), 0);
	}
	if ( w == m->rad4 ) {
		ewl_text_text_set(EWL_TEXT(m->wsize), argwidth);
		ewl_text_text_set(EWL_TEXT(m->hsize), argheight);
		ewl_entry_editable_set(EWL_ENTRY(m->wsize), 1);
		ewl_entry_editable_set(EWL_ENTRY(m->hsize), 1);
	}
	/****************************************************/
}
void
ephoto_men_cb(Ewl_Widget * w, void *event, void *data)
{
	Ewl_Widget *hwin;
	Ewl_Widget *vbox;
	Ewl_Widget *text;
	Ewl_Widget *cancel;
	static char *help_text = "-To view an image, select the simple image viewer tab, and then click an image from\n"
				 "the browser.To add this image to the slideshow/presentation, click the add to slideshow\n"
				 "button. Also, to add images to the slideshow/presentation, click the slideshow/presentation\n"
				 "tab, and then click an image from the browser.  Use the self explanatory settings box to add\n"
				 "features.  Length of slideshow is the amount of time each image will be shown in the slideshow.\n"
				 "Loop slideshow loops the slideshow. Fit to audio, fits the length of the slideshow to the\n"
				 "length of the audio.  You can set the width and height of the slideshow by manipulating the\n"
				 "Custom or Fullscreen settings.  Audio thats added via the combo box, will play during the\n"
				 "slideshow. Finally, select whether you want a slideshow, orpresentation via the button.\n"
				 "Command line options can be viewed by doing ephoto --help from a terminal.\n";
	static char *about_text = "-Ephoto is an advanced image viewer.  It is written in .c and ewl.  It allows you to\n"
				  "browse images, and view them either in a simple image viewer form, or in a slideshow or\n"
				  "controlled presentation.  If emotioned is compiled, you can have audio play while your\n"
				  "images are being shown in the slideshow.  For help see help from the ephoto menu. \n"
				  "For options, see ephoto --help from the terminal.\n";
	hwin = ewl_window_new();
        ewl_window_title_set(EWL_WINDOW(hwin), "Help");
        ewl_window_name_set(EWL_WINDOW(hwin), "Help");
        ewl_object_size_request(EWL_OBJECT(hwin), 400, 300);
        ewl_callback_append(hwin, EWL_CALLBACK_DELETE_WINDOW, destroywin_cb, hwin);
        ewl_widget_show(hwin);
	
        vbox = ewl_vbox_new();
        ewl_container_child_append(EWL_CONTAINER(hwin), vbox);
        ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
        ewl_widget_show(vbox);

        text = ewl_text_new();
        ewl_container_child_append(EWL_CONTAINER(vbox), text);
        ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
        ewl_text_font_size_set(EWL_TEXT(text), 12);
	if ( data == "help" ) {
        	ewl_text_text_set(EWL_TEXT(text), help_text);
	}
	if ( data == "about" ) {
		ewl_text_text_set(EWL_TEXT(text), about_text);
	}
	ewl_widget_show(text);
	
        cancel = ewl_button_new();
        ewl_button_label_set(EWL_BUTTON(cancel), "Close");
        ewl_container_child_append(EWL_CONTAINER(vbox), cancel);
        ewl_object_alignment_set(EWL_OBJECT(cancel), EWL_FLAG_ALIGN_CENTER);
        ewl_object_maximum_size_set(EWL_OBJECT(cancel), 80, 15);
        ewl_callback_append(cancel, EWL_CALLBACK_CLICKED, destroywin_cb, hwin);
        ewl_widget_show(cancel);
}

