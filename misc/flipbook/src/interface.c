
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "menus.h"


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
	GtkWidget *play_button;
	GtkWidget *hscale3;
	GtkWidget *forward_button;
	GtkWidget *hbox2;
	GtkWidget *scrolledwindow1;
	GtkWidget *text1;
	GtkWidget *pixmap1;
	GtkWidget *pixmap2;
	GtkWidget *pixmap3;
	GtkWidget *pixmap4;
	GtkWidget *hbox1;
	GtkWidget *lodalabel_;
	GtkWidget *progressbar2;
	GtkWidget *percentdone;
	GtkTooltips *tooltips;

	tooltips = gtk_tooltips_new();

	VA_Flipbook = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_object_set_data(GTK_OBJECT(VA_Flipbook), "VA_Flipbook", VA_Flipbook);
	gtk_widget_set_usize(VA_Flipbook, 505, 210);
	GTK_WIDGET_SET_FLAGS(VA_Flipbook, GTK_CAN_FOCUS);
	GTK_WIDGET_SET_FLAGS(VA_Flipbook, GTK_CAN_DEFAULT);
	gtk_window_set_title(GTK_WINDOW(VA_Flipbook), "VA Flipbook (Main Window)");
	gtk_window_set_modal(GTK_WINDOW(VA_Flipbook), TRUE);

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox1);
	gtk_object_set_data_full(GTK_OBJECT(VA_Flipbook), "vbox1", vbox1,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox1);
	gtk_container_add(GTK_CONTAINER(VA_Flipbook), vbox1);

	menubar1 = gtk_menu_bar_new();
	gtk_widget_ref(menubar1);
	gtk_object_set_data_full(GTK_OBJECT(VA_Flipbook), "menubar1", menubar1,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(menubar1);
	gtk_box_pack_start(GTK_BOX(vbox1), menubar1, FALSE, FALSE, 0);

	file1 = CreateBarSubMenu(menubar1,"File");
	{
		GtkWidget *menuitem;
		menuitem = CreateMenuItem(file1,"Open Movie","",
				"Open an existing Movie", NULL,"open movie");
		menuitem = CreateMenuItem(file1,"Open Movie Using Template","",
				"Open a new Movie Using the Template Form", NULL,
				"open movie using template");
		menuitem = CreateMenuItem(file1,"Open Movie Using List in File","",
				"Open a new Movie Using a List of Filenames in a File", NULL,
				"open movie using file");
		menuitem = CreateMenuItem(file1,NULL,NULL,NULL,NULL,NULL);
		menuitem = CreateMenuItem(file1,"Close Movie","","Close an open Movie",
				NULL,"close movie");
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
	}


	help1 = gtk_menu_item_new_with_label ("Help");
	gtk_widget_ref (help1);
	gtk_object_set_data_full (GTK_OBJECT (VA_Flipbook), "help1", help1,
			(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (help1);
	gtk_container_add (GTK_CONTAINER (menubar1), help1);
	gtk_menu_item_right_justify (GTK_MENU_ITEM (help1));

	hbox9 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox9);
	gtk_object_set_data_full(GTK_OBJECT(VA_Flipbook), "hbox9", hbox9,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox9);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox9, TRUE, TRUE, 0);

	pixmap2 = create_pixmap(VA_Flipbook, "rewind.xpm");
	gtk_widget_ref(pixmap2);
	gtk_object_set_data_full(GTK_OBJECT(VA_Flipbook), "pixmap2", pixmap2,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pixmap2);

	rewind_button = gtk_button_new();
	gtk_widget_ref(rewind_button);
	gtk_object_set_data_full(GTK_OBJECT(VA_Flipbook), "rewind_button",
		   	rewind_button, (GtkDestroyNotify) gtk_widget_unref);
	gtk_container_add(GTK_CONTAINER(rewind_button), pixmap2);
	gtk_widget_show(rewind_button);
	gtk_box_pack_start(GTK_BOX(hbox9), rewind_button, FALSE, FALSE, 0);

	pixmap3 = create_pixmap(VA_Flipbook, "play.xpm");
	gtk_widget_ref(pixmap3);
	gtk_object_set_data_full(GTK_OBJECT(VA_Flipbook), "pixmap3", pixmap3,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pixmap3);

	play_button = gtk_button_new();
	gtk_widget_ref(play_button);
	gtk_object_set_data_full(GTK_OBJECT(VA_Flipbook), "play_button",
		   	play_button, (GtkDestroyNotify) gtk_widget_unref);
	gtk_container_add(GTK_CONTAINER(play_button), pixmap3);
	gtk_widget_show(play_button);
	gtk_box_pack_start(GTK_BOX(hbox9), play_button, FALSE, FALSE, 0);

	hscale3 = gtk_hscale_new(GTK_ADJUSTMENT(gtk_adjustment_new(0, 0, 101, 1,
				   	1, 1)));
	gtk_widget_ref(hscale3);
	gtk_object_set_data_full(GTK_OBJECT(VA_Flipbook), "hscale3", hscale3,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hscale3);
	gtk_box_pack_start(GTK_BOX(hbox9), hscale3, TRUE, TRUE, 0);

	pixmap4 = create_pixmap(VA_Flipbook, "fastforward.xpm");
	gtk_widget_ref(pixmap4);
	gtk_object_set_data_full(GTK_OBJECT(VA_Flipbook), "pixmap4", pixmap4,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pixmap4);

	forward_button = gtk_button_new();
	gtk_widget_ref(forward_button);
	gtk_object_set_data_full(GTK_OBJECT(VA_Flipbook), "forward_button",
		   	forward_button, (GtkDestroyNotify) gtk_widget_unref);
	gtk_container_add(GTK_CONTAINER(forward_button), pixmap4);
	gtk_widget_show(forward_button);
	gtk_box_pack_start(GTK_BOX(hbox9), forward_button, FALSE, FALSE, 0);

	hbox2 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox2);
	gtk_object_set_data_full(GTK_OBJECT(VA_Flipbook), "hbox2", hbox2,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox2);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox2, TRUE, TRUE, 0);

	scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow1);
	gtk_object_set_data_full(GTK_OBJECT(VA_Flipbook), "scrolledwindow1",
		   	scrolledwindow1, (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow1);
	gtk_box_pack_start(GTK_BOX(hbox2), scrolledwindow1, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow1),
		   	GTK_POLICY_NEVER, GTK_POLICY_NEVER);

	text1 = gtk_text_new(NULL, NULL);
	gtk_widget_ref(text1);
	gtk_object_set_data_full(GTK_OBJECT(VA_Flipbook), "text1", text1,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(text1);
	gtk_container_add(GTK_CONTAINER(scrolledwindow1), text1);
	gtk_tooltips_set_tip(tooltips, text1, "Current Movie Information", NULL);
	gtk_widget_realize(text1);
	gtk_text_insert(GTK_TEXT(text1), NULL, NULL, NULL,
					"\n\n\n\n Framerate: ###.## frames/sec\n Drawrate : "
					"###.## MB/sec "
					"(###.## Mpixel/sec)\n Missed   : #### frames "
					"of #### (## %)", 111);

	pixmap1 = create_pixmap(VA_Flipbook, "valogo.xpm");
	gtk_widget_ref(pixmap1);
	gtk_object_set_data_full(GTK_OBJECT(VA_Flipbook), "pixmap1", pixmap1,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pixmap1);
	gtk_box_pack_start(GTK_BOX(hbox2), pixmap1, TRUE, TRUE, 0);

	hbox1 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox1);
	gtk_object_set_data_full(GTK_OBJECT(VA_Flipbook), "hbox1", hbox1,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox1);
	gtk_box_pack_start(GTK_BOX(vbox1), hbox1, TRUE, TRUE, 0);

	lodalabel_ = gtk_label_new("Loading :");
	gtk_widget_ref(lodalabel_);
	gtk_object_set_data_full(GTK_OBJECT(VA_Flipbook), "lodalabel_", lodalabel_,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(lodalabel_);
	gtk_box_pack_start(GTK_BOX(hbox1), lodalabel_, FALSE, FALSE, 0);

	progressbar2 = gtk_progress_bar_new();
	gtk_widget_ref(progressbar2);
	gtk_object_set_data_full(GTK_OBJECT(VA_Flipbook), "progressbar2",
		   	progressbar2, (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(progressbar2);
	gtk_box_pack_start(GTK_BOX(hbox1), progressbar2, FALSE, FALSE, 0);

	percentdone = gtk_label_new("##:##:## remaining\n##:##:## done");
	gtk_widget_ref(percentdone);
	gtk_object_set_data_full(GTK_OBJECT(VA_Flipbook), "percentdone",
		   	percentdone, (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(percentdone);
	gtk_box_pack_start(GTK_BOX(hbox1), percentdone, FALSE, FALSE, 0);
	gtk_label_set_justify(GTK_LABEL(percentdone), GTK_JUSTIFY_LEFT);

	gtk_widget_grab_default(VA_Flipbook);
	gtk_object_set_data(GTK_OBJECT(VA_Flipbook), "tooltips", tooltips);

	return VA_Flipbook;
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
	gtk_object_set_data(GTK_OBJECT(choose_segment), "choose_segment",
		   	choose_segment);
	gtk_window_set_title(GTK_WINDOW(choose_segment), "Choose Movie Segment");
	GTK_WIDGET_SET_FLAGS(choose_segment, GTK_CAN_FOCUS);
	GTK_WIDGET_SET_FLAGS(choose_segment, GTK_CAN_DEFAULT);
	gtk_window_set_modal(GTK_WINDOW(choose_segment), TRUE);

	vbox2 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox2);
	gtk_object_set_data_full(GTK_OBJECT(choose_segment), "vbox2", vbox2,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox2);
	gtk_container_add(GTK_CONTAINER(choose_segment), vbox2);

	Memory_Avail = gtk_label_new("Available Memory Permits Viewing ## "
			"% of Movie");
	gtk_widget_ref(Memory_Avail);
	gtk_object_set_data_full(GTK_OBJECT(choose_segment), "Memory_Avail",
		   	Memory_Avail, (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(Memory_Avail);
	gtk_box_pack_start(GTK_BOX(vbox2), Memory_Avail, FALSE, FALSE, 0);

	vbox3 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox3);
	gtk_object_set_data_full(GTK_OBJECT(choose_segment), "vbox3", vbox3,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox3);
	gtk_box_pack_start(GTK_BOX(vbox2), vbox3, TRUE, TRUE, 0);

	hscale2 = gtk_hscale_new(GTK_ADJUSTMENT(gtk_adjustment_new(0, 0, 101, 1,
						                    1, 1)));

	gtk_widget_ref(hscale2);
	gtk_object_set_data_full(GTK_OBJECT(choose_segment), "hscale2", hscale2,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hscale2);
	gtk_box_pack_start(GTK_BOX(vbox3), hscale2, TRUE, TRUE, 0);

	hbox4 = gtk_hbox_new(TRUE, 0);
	gtk_widget_ref(hbox4);
	gtk_object_set_data_full(GTK_OBJECT(choose_segment), "hbox4", hbox4,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox4);
	gtk_box_pack_start(GTK_BOX(vbox3), hbox4, TRUE, TRUE, 0);

	frames1 = gtk_label_new("Frame ####");
	gtk_widget_ref(frames1);
	gtk_object_set_data_full(GTK_OBJECT(choose_segment), "frames1", frames1,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frames1);
	gtk_box_pack_start(GTK_BOX(hbox4), frames1, FALSE, FALSE, 0);

	frames2 = gtk_label_new("####");
	gtk_widget_ref(frames2);
	gtk_object_set_data_full(GTK_OBJECT(choose_segment), "frames2", frames2,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frames2);
	gtk_box_pack_start(GTK_BOX(hbox4), frames2, FALSE, FALSE, 0);
	gtk_label_set_justify(GTK_LABEL(frames2), GTK_JUSTIFY_FILL);

	label6 = gtk_label_new("####");
	gtk_widget_ref(label6);
	gtk_object_set_data_full(GTK_OBJECT(choose_segment), "label6", label6,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label6);
	gtk_box_pack_start(GTK_BOX(hbox4), label6, FALSE, FALSE, 0);
	gtk_label_set_justify(GTK_LABEL(label6), GTK_JUSTIFY_RIGHT);

	hbox5 = gtk_hbox_new(TRUE, 0);
	gtk_widget_ref(hbox5);
	gtk_object_set_data_full(GTK_OBJECT(choose_segment), "hbox5", hbox5,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox5);
	gtk_box_pack_start(GTK_BOX(vbox2), hbox5, TRUE, TRUE, 0);

	segment_ok = gtk_button_new_with_label("Okay");
	gtk_widget_ref(segment_ok);
	gtk_object_set_data_full(GTK_OBJECT(choose_segment), "segment_ok",
		   	segment_ok, (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(segment_ok);
	gtk_box_pack_start(GTK_BOX(hbox5), segment_ok, FALSE, FALSE, 0);

	segment_cancel = gtk_button_new_with_label("Cancel");
	gtk_widget_ref(segment_cancel);
	gtk_object_set_data_full(GTK_OBJECT(choose_segment), "segment_cancel",
		   	segment_cancel, (GtkDestroyNotify) gtk_widget_unref);
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
	GtkWidget *alignment1;
	GSList *alignment1_group = NULL;
	GtkWidget *radiobutton1;
	GtkWidget *alignment2;
	GtkWidget *radiobutton2;
	GtkWidget *hbox8;
	GtkWidget *button8;
	GtkWidget *button9;

	Preferences_Window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_object_set_data(GTK_OBJECT(Preferences_Window), "Preferences_Window",
		   	Preferences_Window);
	GTK_WIDGET_SET_FLAGS(Preferences_Window, GTK_CAN_FOCUS);
	GTK_WIDGET_SET_FLAGS(Preferences_Window, GTK_CAN_DEFAULT);
	gtk_window_set_modal(GTK_WINDOW(Preferences_Window), TRUE);


	gtk_window_set_title(GTK_WINDOW(Preferences_Window),
		   	"VA Flipbook (Preferences)");

	vbox4 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox4);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "vbox4", vbox4,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox4);
	gtk_container_add(GTK_CONTAINER(Preferences_Window), vbox4);

	table1 = gtk_table_new(2, 3, FALSE);
	gtk_widget_ref(table1);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "table1", table1,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(table1);
	gtk_box_pack_start(GTK_BOX(vbox4), table1, TRUE, TRUE, 0);

	pref_label1 = gtk_label_new("Use Screen: ");
	gtk_widget_ref(pref_label1);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "pref_label1",
		   	pref_label1, (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pref_label1);
	gtk_table_attach(GTK_TABLE(table1), pref_label1, 0, 1, 0, 1,
					 (GtkAttachOptions) (0),
					 (GtkAttachOptions) (0), 0, 0);

	pref_label2 = gtk_label_new("Use Screen: ");
	gtk_widget_ref(pref_label2);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "pref_label2",
		   	pref_label2, (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pref_label2);
	gtk_table_attach(GTK_TABLE(table1), pref_label2, 0, 1, 1, 2,
					 (GtkAttachOptions) (0),
					 (GtkAttachOptions) (0), 0, 0);

	entry1 = gtk_entry_new_with_max_length(200);
	gtk_widget_ref(entry1);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "entry1", entry1,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entry1);
	gtk_table_attach(GTK_TABLE(table1), entry1, 1, 2, 0, 1,
					 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
					 (GtkAttachOptions) (0), 0, 0);
	gtk_entry_set_text(GTK_ENTRY(entry1), ":1");

	entry2 = gtk_entry_new_with_max_length(200);
	gtk_widget_ref(entry2);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "entry2", entry2,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entry2);
	gtk_table_attach(GTK_TABLE(table1), entry2, 1, 2, 1, 2,
					 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
					 (GtkAttachOptions) (0), 0, 0);
	gtk_entry_set_text(GTK_ENTRY(entry2), ":0");

	pref_label3 = gtk_label_new("For Playing Movie");
	gtk_widget_ref(pref_label3);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "pref_label3",
		   	pref_label3, (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pref_label3);
	gtk_table_attach(GTK_TABLE(table1), pref_label3, 2, 3, 0, 1,
					 (GtkAttachOptions) (0),
					 (GtkAttachOptions) (0), 0, 0);

	pref_label4 = gtk_label_new("For This GUI");
	gtk_widget_ref(pref_label4);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "pref_label4",
		   	pref_label4, (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pref_label4);
	gtk_table_attach(GTK_TABLE(table1), pref_label4, 2, 3, 1, 2,
					 (GtkAttachOptions) (0),
					 (GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify(GTK_LABEL(pref_label4), GTK_JUSTIFY_LEFT);

	hbox6 = gtk_hbox_new(TRUE, 0);
	gtk_widget_ref(hbox6);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "hbox6", hbox6,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox6);
	gtk_box_pack_start(GTK_BOX(vbox4), hbox6, TRUE, TRUE, 0);

	pref_label5 = gtk_label_new("Use");
	gtk_widget_ref(pref_label5);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "pref_label5",
		   	pref_label5, (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pref_label5);
	gtk_box_pack_start(GTK_BOX(hbox6), pref_label5, FALSE, FALSE, 0);
	gtk_label_set_justify(GTK_LABEL(pref_label5), GTK_JUSTIFY_RIGHT);

	display_options = gtk_option_menu_new();
	gtk_widget_ref(display_options);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "display_options",
		   	display_options, (GtkDestroyNotify) gtk_widget_unref);
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
	gtk_widget_ref(pref_label6);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "pref_label6",
		   	pref_label6, (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(pref_label6);
	gtk_box_pack_start(GTK_BOX(hbox6), pref_label6, FALSE, FALSE, 0);
	gtk_label_set_justify(GTK_LABEL(pref_label6), GTK_JUSTIFY_LEFT);

	hbox7 = gtk_hbox_new(TRUE, 0);
	gtk_widget_ref(hbox7);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "hbox7", hbox7,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox7);
	gtk_box_pack_start(GTK_BOX(vbox4), hbox7, TRUE, TRUE, 0);

	prefs_label7 = gtk_label_new("Full Screen Resolution: ");
	gtk_widget_ref(prefs_label7);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "prefs_label7",
		   	prefs_label7, (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(prefs_label7);
	gtk_box_pack_start(GTK_BOX(hbox7), prefs_label7, FALSE, FALSE, 0);
	gtk_label_set_justify(GTK_LABEL(prefs_label7), GTK_JUSTIFY_RIGHT);

	optionmenu2 = gtk_option_menu_new();
	gtk_widget_ref(optionmenu2);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "optionmenu2",
		   	optionmenu2, (GtkDestroyNotify) gtk_widget_unref);
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
	gtk_widget_ref(table2);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "table2", table2,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(table2);
	gtk_box_pack_start(GTK_BOX(vbox4), table2, TRUE, TRUE, 0);

	prefs_label8 = gtk_label_new("Use: ");
	gtk_widget_ref(prefs_label8);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "prefs_label8",
		   	prefs_label8, (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(prefs_label8);
	gtk_table_attach(GTK_TABLE(table2), prefs_label8, 0, 1, 0, 1,
					 (GtkAttachOptions) (0),
					 (GtkAttachOptions) (0), 0, 0);

	label15 = gtk_label_new("");
	gtk_widget_ref(label15);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "label15", label15,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label15);
	gtk_table_attach(GTK_TABLE(table2), label15, 0, 1, 1, 2,
					 (GtkAttachOptions) (0),
					 (GtkAttachOptions) (0), 0, 0);

	alignment1 = gtk_alignment_new(0.5, 7.45058e-09, 1, 1);
	gtk_widget_ref(alignment1);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "alignment1",
		   	alignment1, (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(alignment1);
	gtk_table_attach(GTK_TABLE(table2), alignment1, 1, 2, 0, 1,
					 (GtkAttachOptions) (0),
					 (GtkAttachOptions) (0), 0, 0);

	radiobutton1 = gtk_radio_button_new_with_label(alignment1_group,
		   	"## % of system memory");
	alignment1_group = gtk_radio_button_group(GTK_RADIO_BUTTON(radiobutton1));
	gtk_widget_ref(radiobutton1);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "radiobutton1",
		   	radiobutton1, (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(radiobutton1);
	gtk_container_add(GTK_CONTAINER(alignment1), radiobutton1);

	alignment2 = gtk_alignment_new(0.5, 0, 1, 1);
	gtk_widget_ref(alignment2);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "alignment2",
		   	alignment2, (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(alignment2);
	gtk_table_attach(GTK_TABLE(table2), alignment2, 1, 2, 1, 2,
					 (GtkAttachOptions) (0),
					 (GtkAttachOptions) (0), 0, 0);

	radiobutton2 = gtk_radio_button_new_with_label(alignment1_group,
		   	"## Megabytes of system memory");
	alignment1_group = gtk_radio_button_group(GTK_RADIO_BUTTON(radiobutton2));
	gtk_widget_ref(radiobutton2);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "radiobutton2",
		   	radiobutton2, (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(radiobutton2);
	gtk_container_add(GTK_CONTAINER(alignment2), radiobutton2);

	hbox8 = gtk_hbox_new(TRUE, 0);
	gtk_widget_ref(hbox8);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "hbox8", hbox8,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox8);
	gtk_box_pack_start(GTK_BOX(vbox4), hbox8, TRUE, TRUE, 0);

	button8 = gtk_button_new_with_label("Okay");
	gtk_widget_ref(button8);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "button8", button8,
							 (GtkDestroyNotify) gtk_widget_unref);

	gtk_widget_show(button8);
	gtk_box_pack_start(GTK_BOX(hbox8), button8, FALSE, FALSE, 0);

	button9 = gtk_button_new_with_label("Cancel");
	gtk_widget_ref(button9);
	gtk_object_set_data_full(GTK_OBJECT(Preferences_Window), "button9", button9,
							 (GtkDestroyNotify) gtk_widget_unref);
	gtk_signal_connect (GTK_OBJECT (button9), "clicked",
				            GTK_SIGNAL_FUNC (on_preferences1_close), NULL);

	gtk_widget_show(button9);
	gtk_box_pack_start(GTK_BOX(hbox8), button9, FALSE, FALSE, 0);

	return Preferences_Window;
}
