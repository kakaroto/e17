#include "welcome.h"

Welcome        *welcome = NULL;
Credits        *credits = NULL;

void
open_welcome(void)
{
	if (welcome != NULL)
		return;

	welcome = malloc(sizeof(Welcome));

	welcome->win = ewl_window_new();
	ewl_window_title_set((Ewl_Window *) welcome->win, "Welcome to E-Notes");
	ewl_window_class_set((Ewl_Window *) welcome->win, "Enotes");
	ewl_widget_show(welcome->win);
	ewl_callback_append(welcome->win, EWL_CALLBACK_DELETE_WINDOW,
			    (void *) close_welcome_cb, (void *) NULL);

	welcome->wvbox = ewl_vbox_new();
	ewl_container_child_append((Ewl_Container *) welcome->win,
				   welcome->wvbox);
	ewl_widget_show(welcome->wvbox);

	welcome->title = ewl_text_new();
//      ewl_text_font_set((Ewl_Text *) welcome->title, "vera"); //, 18);
	ewl_text_color_set((Ewl_Text *) welcome->title, 255, 255, 255, 255);
	ewl_text_styles_set((Ewl_Text *) welcome->title,
			    EWL_TEXT_STYLE_OUTLINE);
	ewl_text_color_set((Ewl_Text *) welcome->title, 200, 200, 200, 255);
	ewl_text_text_insert((Ewl_Text *) welcome->title, "Welcome to E-Notes",
			     0);
	ewl_object_padding_set((Ewl_Object *) welcome->title, 5, 5, 5, 5);
	ewl_container_child_append((Ewl_Container *) welcome->wvbox,
				   welcome->title);
	ewl_widget_show(welcome->title);

	welcome->label = ewl_text_new();
//      ewl_text_font_set((Ewl_Text *) welcome->label, "vera"); //, 12);
	ewl_text_color_set((Ewl_Text *) welcome->label, 0, 0, 0, 255);
	ewl_text_text_insert((Ewl_Text *) welcome->label,
			     "E-Notes is a sticky notes system\nbased on the enlightenment\nfoundation libraries.  Please\nview the credits and report\nany bugs to:\n",
			     0);
	ewl_text_color_set((Ewl_Text *) welcome->label, 69, 98, 224, 255);
	ewl_text_text_append((Ewl_Text *) welcome->label,
			     "\ntom.fletcher@gmail.com");
	ewl_container_child_append((Ewl_Container *) welcome->wvbox,
				   welcome->label);
	ewl_object_padding_set((Ewl_Object *) welcome->label, 5, 5, 0, 5);
	ewl_widget_show(welcome->label);

	/*  --- here ---  */

	welcome->whbox = ewl_hbox_new();
	ewl_container_child_append((Ewl_Container *) welcome->wvbox,
				   welcome->whbox);
	ewl_widget_show(welcome->whbox);

	welcome->closebtn = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(welcome->closebtn), "Close");
	ewl_container_child_append((Ewl_Container *) welcome->whbox,
				   welcome->closebtn);
	ewl_widget_show(welcome->closebtn);
	ewl_callback_append(welcome->closebtn, EWL_CALLBACK_CLICKED,
			    (void *) close_welcome_cb, (void *) NULL);

	welcome->creditsbtn = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(welcome->creditsbtn), "Credits");
	ewl_container_child_append((Ewl_Container *) welcome->whbox,
				   welcome->creditsbtn);
	ewl_widget_show(welcome->creditsbtn);
	ewl_callback_append(welcome->creditsbtn, EWL_CALLBACK_CLICKED,
			    (void *) open_credits_cb, (void *) NULL);

	return;
}

void
close_welcome()
{
	ewl_widget_destroy(welcome->win);
	free(welcome);
	welcome = NULL;
	return;
}

void
close_welcome_cb(void *data)
{
	close_welcome();
}

void
open_credits_cb(void *data)
{
	open_credits();
}

void
close_credits_cb(void *data)
{
	close_credits();
}

void
open_credits()
{
	if (credits != NULL)
		return;

	credits = malloc(sizeof(Welcome));

	credits->win = ewl_window_new();
	ewl_window_title_set((Ewl_Window *) credits->win, "Welcome to E-Notes");
	ewl_widget_show(credits->win);
	ewl_callback_append(credits->win, EWL_CALLBACK_DELETE_WINDOW,
			    (void *) close_credits_cb, (void *) NULL);

	credits->credits = ewl_text_new();
	ewl_object_padding_set((Ewl_Object *) credits->credits, 5, 5, 5, 5);
//      ewl_text_font_set((Ewl_Text *) credits->credits, "vera"); //, 12);
	ewl_text_color_set((Ewl_Text *) credits->credits, 0, 0, 0, 255);
	ewl_text_text_insert((Ewl_Text *) credits->credits,
			     "Credits:\n\nDeveloper: Thomas [Fletch]er\nMaintainer: Andrew Williams (HandyandE)\nArtwork: Corey Donohoe (Atmos)\nArtwork: Andrew Williams (HandyAndE)\n",
			     0);
	ewl_container_child_append((Ewl_Container *) credits->win,
				   credits->credits);
	ewl_widget_show(credits->credits);
}

void
close_credits()
{
	ewl_widget_destroy(credits->win);
	free(credits);
	credits = NULL;
	return;
}
