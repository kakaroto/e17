/*****************************************************************************/
/* VA Linux Systems Flipbook demo                                            */
/*****************************************************************************/
/*
 * Copyright (C) 2000 Brad Grantham, Geoff Harrison, and VA Linux Systems
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "menus.h"
#include "hooks.h"
#include "controls.h"

extern GtkWidget *control_slider;
extern GtkWidget *percentdone;
extern GtkWidget *play_button;
extern GtkWidget *play_pixmap;
extern GtkWidget *pause_pixmap;
extern GtkWidget *status_text;

GtkWidget *
 create_VA_Flipbook(void)
{
	GtkWidget *VA_Flipbook;
	GtkWidget *vbox1;
	GtkWidget *menubar1;
	GtkWidget *file1;
	GtkWidget *edit1;
	GtkWidget *view1;
	GtkWidget *help1;
	GtkWidget *hbox9;
	GtkWidget *rewind_button;
	GtkWidget *forward_button;
	GtkWidget *hbox2;
	GtkWidget *pixmap1;
	GtkWidget *pixmap2;
	GtkWidget *pixmap4;
	GtkWidget *hbox1;
	GtkWidget *lodalabel_;
	GtkTooltips *tooltips;

	tooltips = gtk_tooltips_new();

	VA_Flipbook = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_object_set_data(GTK_OBJECT(VA_Flipbook), "VA_Flipbook", VA_Flipbook);
	/* gtk_widget_set_usize(VA_Flipbook, 505, 210); */
	gtk_widget_set_usize(VA_Flipbook, 330, 135);
	GTK_WIDGET_SET_FLAGS(VA_Flipbook, GTK_CAN_FOCUS);
	GTK_WIDGET_SET_FLAGS(VA_Flipbook, GTK_CAN_DEFAULT);
	gtk_window_set_title(GTK_WINDOW(VA_Flipbook), "VA Flipbook (Main Window)");

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox1);
	gtk_container_add(GTK_CONTAINER(VA_Flipbook), vbox1);

	menubar1 = gtk_menu_bar_new();
	gtk_widget_show(menubar1);
	gtk_box_pack_start(GTK_BOX(vbox1), menubar1, FALSE, FALSE, 0);

	file1 = CreateBarSubMenu(menubar1,"File");
	{
		GtkWidget *menuitem;

		menuitem = CreateMenuItem(file1,"Open Movie","",
				"Open an existing Movie", NULL,"open movie");
		gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
			   	GTK_SIGNAL_FUNC (on_open_from_file), NULL);
		menuitem = CreateMenuItem(file1,"Open Movie Using Template","",
				"Open a new Movie Using the Template Form", NULL,
				"open movie using template");
		gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
			   	GTK_SIGNAL_FUNC (on_open_from_template), NULL);
		menuitem = CreateMenuItem(file1,"Open Movie Using List in File","",
				"Open a new Movie Using a List of Filenames in a File", NULL,
				"open movie using file");
		gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
			   	GTK_SIGNAL_FUNC (on_open_from_file), NULL);
		menuitem = CreateMenuItem(file1,NULL,NULL,NULL,NULL,NULL);
		menuitem = CreateMenuItem(file1,"Close Movie","","Close an open Movie",
				NULL,"close movie");
		gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
			   	GTK_SIGNAL_FUNC (on_close_movie), NULL);
		menuitem = CreateMenuItem(file1,"Exit","","Quit this application",
				NULL,"exit");
		gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
			   	GTK_SIGNAL_FUNC (on_exit_application), NULL);
	}

	edit1 = CreateBarSubMenu(menubar1,"Edit");
	{
		GtkWidget *menuitem;
		menuitem = CreateMenuItem(edit1,"Undo","",
				"Undo the previous operation", NULL,"undo");
		menuitem = CreateMenuItem(edit1,"Redo","",
				"Redo the previous operation", NULL, "redo");
		menuitem = CreateMenuItem(edit1,NULL,NULL,NULL,NULL,NULL);
		menuitem = CreateMenuItem(edit1,"Cut","", "Cut the current Segment",
			   	NULL, "cut");
		menuitem = CreateMenuItem(edit1,"Copy","","Copy the current Segment",
				NULL,"copy");
		menuitem = CreateMenuItem(edit1,"Paste","","Paste the current Segment",
				NULL,"paste");
		menuitem = CreateMenuItem(edit1,NULL,NULL,NULL,NULL,NULL);
		menuitem = CreateMenuItem(edit1,"Preferences","",
				"Edit your Preferences", NULL,"preferences");
		gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
			   	GTK_SIGNAL_FUNC (on_preferences1_activate), NULL);
	}

	view1 = CreateBarSubMenu(menubar1,"View");
	{
		GtkWidget *menuitem;
		menuitem = CreateMenuItem(view1,"Choose Movie Segment","",
				"Choose a Movie Segment", NULL,"choose movie segment");
		gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
			   	GTK_SIGNAL_FUNC (on_choose_movie_segment1_activate), NULL);
		menuitem = CreateMenuItem(view1,"Movie Info","",
				"Information about the current Movie", NULL, "movie info");
		gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
			   	GTK_SIGNAL_FUNC (on_movie_info), NULL);
	}


	help1 = CreateRightAlignBarSubMenu(menubar1,"Help");
	{
		GtkWidget *menuitem;
		menuitem = CreateMenuItem(help1,"About VA Flipbook","",
				"About VA Flipbook", NULL,"about va flipbook");
		menuitem = CreateMenuItem(help1,"Documentation","",
				"Read Documentation about VA Flipbook", NULL, "help");
	}

	hbox9 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox9);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox9, TRUE, TRUE, 0);

	pixmap2 = create_pixmap(VA_Flipbook, "rewind.xpm");
	gtk_widget_show(pixmap2);

	rewind_button = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(rewind_button), pixmap2);
	gtk_widget_show(rewind_button);
	gtk_box_pack_start(GTK_BOX(hbox9), rewind_button, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (rewind_button), "clicked",
				            GTK_SIGNAL_FUNC (on_rewind_button), NULL);

	play_pixmap = create_pixmap(VA_Flipbook, "play.xpm");
	gtk_widget_show(play_pixmap);

	play_button = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(play_button), play_pixmap);
	gtk_widget_show(play_button);
	gtk_box_pack_start(GTK_BOX(hbox9), play_button, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (play_button), "clicked",
				            GTK_SIGNAL_FUNC (on_play_button), NULL);

	control_slider = gtk_hscale_new(GTK_ADJUSTMENT(gtk_adjustment_new(0, 0,
				   	1, 1, 1, 1)));
	gtk_scale_set_digits(GTK_SCALE(control_slider),0);
	gtk_widget_show(control_slider);
	gtk_box_pack_start(GTK_BOX(hbox9), control_slider, TRUE, TRUE, 0);

	pixmap4 = create_pixmap(VA_Flipbook, "fastforward.xpm");
	gtk_widget_show(pixmap4);

	forward_button = gtk_button_new();
	gtk_container_add(GTK_CONTAINER(forward_button), pixmap4);
	gtk_widget_show(forward_button);
	gtk_box_pack_start(GTK_BOX(hbox9), forward_button, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (forward_button), "clicked",
				            GTK_SIGNAL_FUNC (on_forward_button), NULL);

	hbox2 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox2);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox2, TRUE, TRUE, 0);

	status_text = gtk_text_new(NULL, NULL);
	gtk_widget_show(status_text);
	gtk_widget_set_usize(status_text,5,5);
	gtk_box_pack_start(GTK_BOX(hbox2), status_text, TRUE, TRUE, 0);
	gtk_tooltips_set_tip(tooltips, status_text, "Current Movie Information",
		   	NULL);
	gtk_widget_realize(status_text);
	{
		char my_string[255];
		sprintf(my_string,"\n Framerate: %s\n Drawrate : %s\n Missed   : %s\n",
				get_current_framerate(),get_current_drawrate(),
				get_current_missed());
		gtk_text_insert(GTK_TEXT(status_text),NULL,NULL,NULL,my_string,
				strlen(my_string));
	}

	pixmap1 = create_pixmap(VA_Flipbook, "valogo.xpm");
	gtk_widget_show(pixmap1);
	gtk_box_pack_start(GTK_BOX(hbox2), pixmap1, FALSE, FALSE, 3);
	gtk_tooltips_set_tip(tooltips, pixmap1, "Brought to you by the fine folks\n"
			"at VA Linux Systems", NULL);

	hbox1 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox1);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox1, TRUE, TRUE, 0);

	lodalabel_ = gtk_label_new("Loading:");
	gtk_widget_show(lodalabel_);
	gtk_box_pack_start(GTK_BOX(hbox1), lodalabel_, FALSE, FALSE, 3);

	progress_bar = gtk_progress_bar_new();
	gtk_widget_show(progress_bar);
	gtk_box_pack_start(GTK_BOX(hbox1), progress_bar, FALSE, FALSE, 0);

	percentdone = gtk_label_new("00:00:00:00 remaining\n00:00:00:00 done");
	gtk_widget_show(percentdone);
	gtk_box_pack_start(GTK_BOX(hbox1), percentdone, FALSE, FALSE, 3);
	gtk_label_set_justify(GTK_LABEL(percentdone), GTK_JUSTIFY_LEFT);

	gtk_widget_grab_default(VA_Flipbook);
	gtk_object_set_data(GTK_OBJECT(VA_Flipbook), "tooltips", tooltips);

	return VA_Flipbook;
}

GtkWidget *
create_open_template(void)
{

	GtkWidget *open_template;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *entry;
	GtkWidget *radiobutton;
	GtkWidget *button;
	GSList *alignment_group = NULL;


	open_template = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(open_template),5);
	gtk_object_set_data(GTK_OBJECT(open_template), "open_template",
		   	open_template);
	gtk_window_set_title(GTK_WINDOW(open_template),
		   	"Open Movie Using Template");
	GTK_WIDGET_SET_FLAGS(open_template, GTK_CAN_FOCUS);
	GTK_WIDGET_SET_FLAGS(open_template, GTK_CAN_DEFAULT);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(open_template), vbox);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 3);

	label = gtk_label_new("Template: ");
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 3);

	entry = gtk_entry_new_with_max_length(200);
	gtk_widget_show(entry);
	gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, TRUE, 0);

	label = gtk_label_new("(use printf (3) syntax)");
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 3);

	label = gtk_label_new("First Frame Index: ");
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 3);

	entry = gtk_entry_new_with_max_length(200);
	gtk_widget_show(entry);
	gtk_box_pack_start(GTK_BOX(hbox), entry, FALSE, FALSE, 0);

	radiobutton = gtk_radio_button_new_with_label(alignment_group,
				            "Read as many frames as exist");
	alignment_group = gtk_radio_button_group(GTK_RADIO_BUTTON(radiobutton));
	gtk_widget_show(radiobutton);
	gtk_box_pack_start(GTK_BOX(vbox), radiobutton, FALSE, FALSE, 0);

	radiobutton = gtk_radio_button_new_with_label(alignment_group,
				            "Frame Index");
	alignment_group = gtk_radio_button_group(GTK_RADIO_BUTTON(radiobutton));
	gtk_widget_show(radiobutton);
	gtk_box_pack_start(GTK_BOX(vbox), radiobutton, FALSE, FALSE, 0);

	hbox = gtk_hbox_new(TRUE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

	button = gtk_button_new_with_label("Okay");
	gtk_widget_show(button);
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);

	button = gtk_button_new_with_label("Cancel");
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
				            GTK_SIGNAL_FUNC (on_open_template_close), NULL);

	gtk_widget_show(button);
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);


	return open_template;
}

GtkWidget *
create_movie_info(void)
{

	char s[1024];
	GtkWidget *movie_info;
	GtkWidget *vbox;
	GtkWidget *label;
	GtkWidget *button;

	movie_info = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(movie_info),5);
	gtk_object_set_data(GTK_OBJECT(movie_info), "movie_info",
		   	movie_info);
	gtk_window_set_title(GTK_WINDOW(movie_info), "Current Movie Info");
	GTK_WIDGET_SET_FLAGS(movie_info, GTK_CAN_FOCUS);
	GTK_WIDGET_SET_FLAGS(movie_info, GTK_CAN_DEFAULT);

	vbox = gtk_vbox_new(FALSE,0);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(movie_info), vbox);

	sprintf(s,"Opened From File: %s",get_movie_name());
	label = gtk_label_new(s);
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 3);

	sprintf(s,"%d Frames in Movie",get_total_frames());
	label = gtk_label_new(s);
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 3);

	sprintf(s,"Width: %d     Height: %d",get_width(),get_height());
	label = gtk_label_new(s);
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 3);

	sprintf(s,"%d :  Megs Total in Movie",((get_width() * get_height() 
				* 3 * get_total_frames()) / 1024) / 1024);
	label = gtk_label_new(s);
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 3);

	sprintf(s,"%d :  Megs Total in Movie",((get_width() * get_height() 
				* 3 * get_total_frames()) / 1024) / 1024);
	label = gtk_label_new(s);
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 3);

	button = gtk_button_new_with_label("Okay");
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
				            GTK_SIGNAL_FUNC (on_movie_info_close), NULL);

	gtk_widget_show(button);
	gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);

	return movie_info;

}

GtkWidget *
 create_choose_segment(void)
{
	GtkWidget *choose_segment;
	GtkWidget *vbox2;
	GtkWidget *Memory_Avail;
	GtkWidget *vbox3;
	GtkWidget *hscale2;
	GtkWidget *hbox4;
	GtkWidget *frames1;
	GtkWidget *frames2;
	GtkWidget *label6;
	GtkWidget *hbox5;
	GtkWidget *segment_ok;
	GtkWidget *segment_cancel;

	choose_segment = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(choose_segment),5);
	gtk_object_set_data(GTK_OBJECT(choose_segment), "choose_segment",
		   	choose_segment);
	gtk_window_set_title(GTK_WINDOW(choose_segment), "Choose Movie Segment");
	GTK_WIDGET_SET_FLAGS(choose_segment, GTK_CAN_FOCUS);
	GTK_WIDGET_SET_FLAGS(choose_segment, GTK_CAN_DEFAULT);

	vbox2 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox2);
	gtk_container_add(GTK_CONTAINER(choose_segment), vbox2);

	Memory_Avail = gtk_label_new("Available Memory Permits Viewing ## "
			"% of Movie");
	gtk_widget_show(Memory_Avail);
	gtk_box_pack_start(GTK_BOX(vbox2), Memory_Avail, FALSE, FALSE, 0);

	vbox3 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox3);
	gtk_box_pack_start(GTK_BOX(vbox2), vbox3, TRUE, TRUE, 0);

	hscale2 = gtk_hscale_new(GTK_ADJUSTMENT(gtk_adjustment_new(0, 0, 101, 1,
						                    1, 1)));

	gtk_widget_show(hscale2);
	gtk_box_pack_start(GTK_BOX(vbox3), hscale2, TRUE, TRUE, 0);

	hbox4 = gtk_hbox_new(TRUE, 0);
	gtk_widget_show(hbox4);
	gtk_box_pack_start(GTK_BOX(vbox3), hbox4, TRUE, TRUE, 0);

	frames1 = gtk_label_new("Frame ####");
	gtk_widget_show(frames1);
	gtk_box_pack_start(GTK_BOX(hbox4), frames1, FALSE, FALSE, 0);

	frames2 = gtk_label_new("####");
	gtk_widget_show(frames2);
	gtk_box_pack_start(GTK_BOX(hbox4), frames2, FALSE, FALSE, 0);
	gtk_label_set_justify(GTK_LABEL(frames2), GTK_JUSTIFY_FILL);

	label6 = gtk_label_new("####");
	gtk_widget_show(label6);
	gtk_box_pack_start(GTK_BOX(hbox4), label6, FALSE, FALSE, 0);
	gtk_label_set_justify(GTK_LABEL(label6), GTK_JUSTIFY_RIGHT);

	hbox5 = gtk_hbox_new(TRUE, 0);
	gtk_widget_show(hbox5);
	gtk_box_pack_start(GTK_BOX(vbox2), hbox5, TRUE, TRUE, 0);

	segment_ok = gtk_button_new_with_label("Okay");
	gtk_widget_show(segment_ok);
	gtk_box_pack_start(GTK_BOX(hbox5), segment_ok, FALSE, FALSE, 0);
	gtk_signal_connect (GTK_OBJECT (segment_ok), "clicked",
				            GTK_SIGNAL_FUNC (on_chose_segment),
						   	NULL);

	segment_cancel = gtk_button_new_with_label("Cancel");
	gtk_signal_connect (GTK_OBJECT (segment_cancel), "clicked",
				            GTK_SIGNAL_FUNC (on_choose_movie_segment1_close),
						   	NULL);
	gtk_widget_show(segment_cancel);
	gtk_box_pack_start(GTK_BOX(hbox5), segment_cancel, FALSE, FALSE, 0);

	return choose_segment;
}

GtkWidget *
 create_Preferences_Window(void)
{
	GtkWidget *Preferences_Window;
	GtkWidget *vbox4;
	GtkWidget *table1;
	GtkWidget *pref_label1;
	GtkWidget *pref_label2;
	GtkWidget *entry1;
	GtkWidget *entry2;
	GtkWidget *pref_label3;
	GtkWidget *pref_label4;
	GtkWidget *hbox6;
	GtkWidget *pref_label5;
	GtkWidget *display_options;
	GtkWidget *display_options_menu;
	GtkWidget *glade_menuitem;
	GtkWidget *pref_label6;
	GtkWidget *hbox7;
	GtkWidget *prefs_label7;
	GtkWidget *optionmenu2;
	GtkWidget *optionmenu2_menu;
	GtkWidget *table2;
	GtkWidget *prefs_label8;
	GtkWidget *label15;
	GSList *alignment1_group = NULL;
	GtkWidget *radiobutton1;
	GtkWidget *radiobutton2;
	GtkWidget *hbox8;
	GtkWidget *button8;
	GtkWidget *button9;

	Preferences_Window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(Preferences_Window),5);

	gtk_object_set_data(GTK_OBJECT(Preferences_Window), "Preferences_Window",
		   	Preferences_Window);
	GTK_WIDGET_SET_FLAGS(Preferences_Window, GTK_CAN_FOCUS);
	GTK_WIDGET_SET_FLAGS(Preferences_Window, GTK_CAN_DEFAULT);


	gtk_window_set_title(GTK_WINDOW(Preferences_Window),
		   	"VA Flipbook (Preferences)");

	vbox4 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox4);
	gtk_container_add(GTK_CONTAINER(Preferences_Window), vbox4);

	table1 = gtk_table_new(2, 3, FALSE);
	gtk_widget_show(table1);
	gtk_box_pack_start(GTK_BOX(vbox4), table1, TRUE, TRUE, 0);

	pref_label1 = gtk_label_new("Use Screen: ");
	gtk_label_set_justify(GTK_LABEL(pref_label1),GTK_JUSTIFY_RIGHT);
	gtk_widget_show(pref_label1);
	gtk_table_attach(GTK_TABLE(table1), pref_label1, 0, 1, 0, 1,
					 (GtkAttachOptions) (0),
					 (GtkAttachOptions) (0), 0, 0);

	pref_label2 = gtk_label_new("Use Screen: ");
	gtk_label_set_justify(GTK_LABEL(pref_label2),GTK_JUSTIFY_RIGHT);
	gtk_widget_show(pref_label2);
	gtk_table_attach(GTK_TABLE(table1), pref_label2, 0, 1, 1, 2,
					 (GtkAttachOptions) (0),
					 (GtkAttachOptions) (0), 0, 0);

	entry1 = gtk_entry_new_with_max_length(200);
	gtk_widget_show(entry1);
	gtk_table_attach(GTK_TABLE(table1), entry1, 1, 2, 0, 1,
					 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
					 (GtkAttachOptions) (0), 0, 0);
	gtk_entry_set_text(GTK_ENTRY(entry1), ":1");

	entry2 = gtk_entry_new_with_max_length(200);
	gtk_widget_show(entry2);
	gtk_table_attach(GTK_TABLE(table1), entry2, 1, 2, 1, 2,
					 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
					 (GtkAttachOptions) (0), 0, 0);
	gtk_entry_set_text(GTK_ENTRY(entry2), ":0");

	pref_label3 = gtk_label_new("For Playing Movie");
	gtk_widget_show(pref_label3);
	gtk_table_attach(GTK_TABLE(table1), pref_label3, 2, 3, 0, 1,
					 (GtkAttachOptions) (0),
					 (GtkAttachOptions) (0), 0, 0);

	pref_label4 = gtk_label_new("For This GUI");
	gtk_widget_show(pref_label4);
	gtk_table_attach(GTK_TABLE(table1), pref_label4, 2, 3, 1, 2,
					 (GtkAttachOptions) (0),
					 (GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify(GTK_LABEL(pref_label4), GTK_JUSTIFY_LEFT);

	hbox6 = gtk_hbox_new(TRUE, 0);
	gtk_widget_show(hbox6);
	gtk_box_pack_start(GTK_BOX(vbox4), hbox6, TRUE, TRUE, 0);

	pref_label5 = gtk_label_new("Use");
	gtk_widget_show(pref_label5);
	gtk_box_pack_start(GTK_BOX(hbox6), pref_label5, FALSE, FALSE, 0);
	gtk_label_set_justify(GTK_LABEL(pref_label5), GTK_JUSTIFY_RIGHT);

	display_options = gtk_option_menu_new();
	gtk_widget_show(display_options);
	gtk_box_pack_start(GTK_BOX(hbox6), display_options, FALSE, FALSE, 0);
	display_options_menu = gtk_menu_new();
	glade_menuitem = gtk_menu_item_new_with_label("Full Screen");
	gtk_widget_show(glade_menuitem);
	gtk_menu_append(GTK_MENU(display_options_menu), glade_menuitem);
	glade_menuitem = gtk_menu_item_new_with_label("OpenGL DrawPixels");
	gtk_widget_show(glade_menuitem);
	gtk_menu_append(GTK_MENU(display_options_menu), glade_menuitem);
	glade_menuitem = gtk_menu_item_new_with_label("X11 Shared Memory");
	gtk_widget_show(glade_menuitem);
	gtk_menu_append(GTK_MENU(display_options_menu), glade_menuitem);
	gtk_option_menu_set_menu(GTK_OPTION_MENU(display_options),
		   	display_options_menu);

	pref_label6 = gtk_label_new("For Movie Display");
	gtk_widget_show(pref_label6);
	gtk_box_pack_start(GTK_BOX(hbox6), pref_label6, FALSE, FALSE, 0);
	gtk_label_set_justify(GTK_LABEL(pref_label6), GTK_JUSTIFY_LEFT);

	hbox7 = gtk_hbox_new(TRUE, 0);
	gtk_widget_show(hbox7);
	gtk_box_pack_start(GTK_BOX(vbox4), hbox7, TRUE, TRUE, 0);

	prefs_label7 = gtk_label_new("Full Screen Resolution: ");
	gtk_label_set_justify(GTK_LABEL(prefs_label7),GTK_JUSTIFY_RIGHT);
	gtk_widget_show(prefs_label7);
	gtk_box_pack_start(GTK_BOX(hbox7), prefs_label7, FALSE, FALSE, 0);

	optionmenu2 = gtk_option_menu_new();
	gtk_widget_show(optionmenu2);
	gtk_box_pack_start(GTK_BOX(hbox7), optionmenu2, FALSE, FALSE, 0);
	optionmenu2_menu = gtk_menu_new();
	glade_menuitem = gtk_menu_item_new_with_label("1880x1440");
	gtk_widget_show(glade_menuitem);
	gtk_menu_append(GTK_MENU(optionmenu2_menu), glade_menuitem);
	glade_menuitem = gtk_menu_item_new_with_label("1600x1200");
	gtk_widget_show(glade_menuitem);
	gtk_menu_append(GTK_MENU(optionmenu2_menu), glade_menuitem);
	glade_menuitem = gtk_menu_item_new_with_label("1280x1024");
	gtk_widget_show(glade_menuitem);
	gtk_menu_append(GTK_MENU(optionmenu2_menu), glade_menuitem);
	glade_menuitem = gtk_menu_item_new_with_label("1024x768");
	gtk_widget_show(glade_menuitem);
	gtk_menu_append(GTK_MENU(optionmenu2_menu), glade_menuitem);
	glade_menuitem = gtk_menu_item_new_with_label("800x600");
	gtk_widget_show(glade_menuitem);
	gtk_menu_append(GTK_MENU(optionmenu2_menu), glade_menuitem);
	glade_menuitem = gtk_menu_item_new_with_label("640x480");
	gtk_widget_show(glade_menuitem);
	gtk_menu_append(GTK_MENU(optionmenu2_menu), glade_menuitem);
	glade_menuitem = gtk_menu_item_new_with_label("320x240");
	gtk_widget_show(glade_menuitem);
	gtk_menu_append(GTK_MENU(optionmenu2_menu), glade_menuitem);
	gtk_option_menu_set_menu(GTK_OPTION_MENU(optionmenu2), optionmenu2_menu);

	table2 = gtk_table_new(2, 2, TRUE);
	gtk_widget_show(table2);
	gtk_box_pack_start(GTK_BOX(vbox4), table2, TRUE, TRUE, 0);

	prefs_label8 = gtk_label_new("Use: ");
	gtk_widget_show(prefs_label8);
	gtk_table_attach(GTK_TABLE(table2), prefs_label8, 0, 1, 0, 1,
					 (GtkAttachOptions) (0),
					 (GtkAttachOptions) (0), 0, 0);

	label15 = gtk_label_new("");
	gtk_widget_show(label15);
	gtk_table_attach(GTK_TABLE(table2), label15, 0, 1, 1, 2,
					 (GtkAttachOptions) (0),
					 (GtkAttachOptions) (0), 0, 0);

	{
		GtkWidget *label;
		GtkWidget *entry;
		GtkWidget *hbox;

		radiobutton1 = gtk_radio_button_new(alignment1_group);
		hbox = gtk_hbox_new(FALSE,0);
		gtk_widget_show(hbox);
		gtk_container_add(GTK_CONTAINER(radiobutton1), hbox);
		entry = gtk_entry_new();
		gtk_entry_set_text(GTK_ENTRY(entry), "100");
		gtk_widget_show(entry);
		gtk_box_pack_start(GTK_BOX(hbox), entry, FALSE, FALSE, 0);
		label = gtk_label_new("% of system memory");
		gtk_widget_show(label);
		gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

	}

	alignment1_group = gtk_radio_button_group(GTK_RADIO_BUTTON(radiobutton1));
	gtk_widget_show(radiobutton1);
	gtk_table_attach(GTK_TABLE(table2), radiobutton1, 1, 2, 0, 1,
					 (GtkAttachOptions) (!GTK_EXPAND | !GTK_SHRINK | GTK_FILL),
					 (GtkAttachOptions) (0), 0, 0);

	{
		GtkWidget *label;
		GtkWidget *entry;
		GtkWidget *hbox;

		radiobutton2 = gtk_radio_button_new(alignment1_group);
		hbox = gtk_hbox_new(FALSE,0);
		gtk_widget_show(hbox);
		gtk_container_add(GTK_CONTAINER(radiobutton2), hbox);
		entry = gtk_entry_new();
		gtk_entry_set_text(GTK_ENTRY(entry), "500");
		gtk_widget_show(entry);
		gtk_box_pack_start(GTK_BOX(hbox), entry, FALSE, FALSE, 0);
		label = gtk_label_new("Megabytes of system memory");
		gtk_widget_show(label);
		gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

	}

	alignment1_group = gtk_radio_button_group(GTK_RADIO_BUTTON(radiobutton2));
	gtk_widget_show(radiobutton2);
	gtk_table_attach(GTK_TABLE(table2), radiobutton2, 1, 2, 1, 2,
					 (GtkAttachOptions) (!GTK_EXPAND | !GTK_SHRINK | GTK_FILL),
					 (GtkAttachOptions) (0), 0, 0);

	hbox8 = gtk_hbox_new(TRUE, 0);
	gtk_widget_show(hbox8);
	gtk_box_pack_start(GTK_BOX(vbox4), hbox8, TRUE, TRUE, 0);

	button8 = gtk_button_new_with_label("Okay");
	gtk_signal_connect (GTK_OBJECT (button8), "clicked",
				            GTK_SIGNAL_FUNC (on_save_preferences), NULL);

	gtk_widget_show(button8);
	gtk_box_pack_start(GTK_BOX(hbox8), button8, FALSE, FALSE, 0);

	button9 = gtk_button_new_with_label("Cancel");
	gtk_signal_connect (GTK_OBJECT (button9), "clicked",
				            GTK_SIGNAL_FUNC (on_preferences1_close), NULL);

	gtk_widget_show(button9);
	gtk_box_pack_start(GTK_BOX(hbox8), button9, FALSE, FALSE, 0);

	return Preferences_Window;
}
