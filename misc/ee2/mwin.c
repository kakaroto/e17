/***********************************
 * controlling window for ee2      *
 ***********************************/

#include "ee2.h"

/* pixmaps */
#include "icons/red_mod.xpm"
#include "icons/green_mod.xpm"
#include "icons/blue_mod.xpm"
#include "icons/alpha_mod.xpm"
#include "icons/scale_mod.xpm"
#include "icons/sgrab_mod.xpm"
#include "icons/cam_mod.xpm"
#include "icons/rotate_mod.xpm"
#include "icons/main_mod.xpm"
#include "icons/brite_mod.xpm"
#include "icons/texmap_mod.xpm"
#include "icons/fx_mod.xpm"

void exit_button_cb(GtkWidget *widget, gpointer data);
gint mod_key_press(GtkWidget *widget, GdkEventKey *event);

void
mod_init(void)
{
  GtkWidget *box1, *box2, *box3, *box4; /* packing boxes */
  GtkWidget *frame1, *frame2, *frame3; /* frames */
  GtkWidget *btn;
  gchar *titles[1]={"Electric Eyes 2"};
  gchar *clabels[255][1] = { {""} };
  int eek;

  ModWin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width(GTK_CONTAINER(ModWin), 2);
  gtk_signal_connect_object(GTK_OBJECT(ModWin), "delete_event",
			    GTK_SIGNAL_FUNC(mod_hide), (gpointer) NULL);
  gtk_signal_connect_object(GTK_OBJECT(ModWin), "destroy",
			    GTK_SIGNAL_FUNC(mod_hide), (gpointer) NULL);
  gtk_signal_connect(GTK_OBJECT(ModWin), "key_press_event",
			    GTK_SIGNAL_FUNC(mod_key_press), (gpointer) NULL);
  gtk_window_set_title(GTK_WINDOW(ModWin), "Electric Eyes 2 - Image Controls");

  box1 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(box1);
  gtk_container_add(GTK_CONTAINER(ModWin), box1);

  box2 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(box2);
  gtk_box_pack_start(GTK_BOX(box1), box2, TRUE, TRUE, 0);

  box3 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(box3);
  gtk_box_pack_start(GTK_BOX(box1), box3, TRUE, TRUE, 0);

  ModList = gtk_clist_new_with_titles(1, titles);
  gtk_clist_set_row_height(GTK_CLIST(ModList), 21);
  gtk_widget_show(ModList);
  gtk_widget_set_usize(ModList, 130, 267);
  for(eek = 0; eek < 13; eek++){
    gtk_clist_append(GTK_CLIST(ModList), clabels[1]);
  }
  gtk_box_pack_start(GTK_BOX(box2), ModList, TRUE, TRUE, 0);
  /* set pixmaps for each row */
  g_pixmap = gdk_pixmap_create_from_xpm_d(GDK_ROOT_PARENT(), &g_bitmap, NULL,
					  main_mod);
  gtk_clist_set_pixtext(GTK_CLIST(ModList), 0, 0,
			"Main", 4, g_pixmap, g_bitmap);
  g_pixmap = gdk_pixmap_create_from_xpm_d(GDK_ROOT_PARENT(), &g_bitmap, NULL,
					  red_mod);
  gtk_clist_set_pixtext(GTK_CLIST(ModList), 1, 0,
			"Red Modifiers", 4, g_pixmap, g_bitmap);
  g_pixmap = gdk_pixmap_create_from_xpm_d(GDK_ROOT_PARENT(), &g_bitmap, NULL,
					  green_mod);
  gtk_clist_set_pixtext(GTK_CLIST(ModList), 2, 0,
			"Green Modifiers", 4, g_pixmap, g_bitmap);
  g_pixmap = gdk_pixmap_create_from_xpm_d(GDK_ROOT_PARENT(), &g_bitmap, NULL,
					  blue_mod);
  gtk_clist_set_pixtext(GTK_CLIST(ModList), 3, 0,
			"Blue Modifiers", 4, g_pixmap, g_bitmap);
  g_pixmap = gdk_pixmap_create_from_xpm_d(GDK_ROOT_PARENT(), &g_bitmap, NULL,
					  alpha_mod);
  gtk_clist_set_pixtext(GTK_CLIST(ModList), 4, 0,
			"Alpha Modifiers", 4, g_pixmap, g_bitmap);
  g_pixmap = gdk_pixmap_create_from_xpm_d(GDK_ROOT_PARENT(), &g_bitmap, NULL,
					  brite_mod);
  gtk_clist_set_pixtext(GTK_CLIST(ModList), 5, 0,
			"Brightness/Contrast", 4, g_pixmap, g_bitmap);
  g_pixmap = gdk_pixmap_create_from_xpm_d(GDK_ROOT_PARENT(), &g_bitmap, NULL,
					  scale_mod);
  gtk_clist_set_pixtext(GTK_CLIST(ModList), 6, 0,
			"Image Scaling", 4, g_pixmap, g_bitmap);
  g_pixmap = gdk_pixmap_create_from_xpm_d(GDK_ROOT_PARENT(), &g_bitmap, NULL,
					  rotate_mod);
  gtk_clist_set_pixtext(GTK_CLIST(ModList), 7, 0,
			"Image Rotation", 4, g_pixmap, g_bitmap);
  g_pixmap = gdk_pixmap_create_from_xpm_d(GDK_ROOT_PARENT(), &g_bitmap, NULL,
					  fx_mod);
  gtk_clist_set_pixtext(GTK_CLIST(ModList), 8, 0,
			"Effects/Filters", 4, g_pixmap, g_bitmap);
  g_pixmap = gdk_pixmap_create_from_xpm_d(GDK_ROOT_PARENT(), &g_bitmap, NULL,
					  texmap_mod);
  gtk_clist_set_pixtext(GTK_CLIST(ModList), 9, 0,
			"Texture Mapping", 4, g_pixmap, g_bitmap);
  g_pixmap = gdk_pixmap_create_from_xpm_d(GDK_ROOT_PARENT(), &g_bitmap, NULL,
					  sgrab_mod);
  gtk_clist_set_pixtext(GTK_CLIST(ModList), 10, 0,
			"Screenshot", 4, g_pixmap, g_bitmap);
  g_pixmap = gdk_pixmap_create_from_xpm_d(GDK_ROOT_PARENT(), &g_bitmap, NULL,
					  cam_mod);
  gtk_clist_set_pixtext(GTK_CLIST(ModList), 11, 0,
			"WebCam Viewer", 4, g_pixmap, g_bitmap);
  gtk_signal_connect(GTK_OBJECT(ModList), "select_row",
		     GTK_SIGNAL_FUNC(mod_b), NULL);

#if 0
        /* Mr. McFeely sold dphase some bad crack.
           But you didn't hear that from me. */

	/* set colors for first row */
	gcmap = gdk_colormap_get_system();
	clr1.red   = 0.30;
	clr1.green = 0.30;
	clr1.blue  = 0.38;	
	clr2.red   = 1.00;
	clr2.green = 1.00;
	clr2.blue  = 1.00;
	gdk_color_alloc(gcmap, &clr1);
	gdk_color_alloc(gcmap, &clr2);

	gtk_clist_set_background(GTK_CLIST(ModList), 0, &clr1);
	gtk_clist_set_foreground(GTK_CLIST(ModList), 0, &clr2);
#endif

  frame1 = gtk_frame_new("Image Preview");
  gtk_container_set_border_width(GTK_CONTAINER(frame1), 2);
  gtk_widget_show(frame1);
  gtk_box_pack_start(GTK_BOX(box2), frame1, TRUE, TRUE, 0);

  frame2 = gtk_frame_new(NULL);
  gtk_container_set_border_width(GTK_CONTAINER(frame2), 2);
  gtk_widget_show(frame2);
  gtk_container_add(GTK_CONTAINER(frame1), frame2);
  gtk_frame_set_shadow_type(GTK_FRAME(frame2), GTK_SHADOW_IN);

  area2 = gtk_drawing_area_new();
  gtk_signal_connect_after(GTK_OBJECT(area2), "configure_event",
			   GTK_SIGNAL_FUNC(b_config), NULL);
  gtk_widget_set_usize(area2, 135, 135);
  gtk_container_add(GTK_CONTAINER(frame2), area2);
  gtk_widget_show(area2);

  ModMdi = gtk_notebook_new();
  gtk_widget_show(ModMdi);
  gtk_widget_set_usize(ModMdi, 400, 440);
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(ModMdi), FALSE);
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(ModMdi), FALSE);
  gtk_box_pack_start(GTK_BOX(box3), ModMdi, TRUE, TRUE, 0);

  frame3 = gtk_frame_new(NULL);
  gtk_container_set_border_width(GTK_CONTAINER(frame3), 3);
  gtk_widget_show(frame3);
  gtk_box_pack_start(GTK_BOX(box3), frame3, FALSE, FALSE, 0);

  box4 = gtk_hbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(box4), 3);
  gtk_container_add(GTK_CONTAINER(frame3), box4);
  gtk_widget_show(box4);

  btn = gtk_button_new_with_label("Load");
  gtk_signal_connect_object(GTK_OBJECT(btn), "clicked",
			    GTK_SIGNAL_FUNC(mod_cb), (gpointer) 1);
  gtk_box_pack_start(GTK_BOX(box4), btn, TRUE, TRUE, 0);
  gtk_widget_show(btn);

  btn = gtk_button_new_with_label("Save");
  gtk_signal_connect_object(GTK_OBJECT(btn), "clicked",
			    GTK_SIGNAL_FUNC(mod_cb), (gpointer) 2);
  gtk_box_pack_start(GTK_BOX(box4), btn, TRUE, TRUE, 0);
  gtk_widget_show(btn);

  btn = gtk_button_new_with_label("Save As");
  gtk_signal_connect_object(GTK_OBJECT(btn), "clicked",
			    GTK_SIGNAL_FUNC(mod_cb), (gpointer) 3);
  gtk_box_pack_start(GTK_BOX(box4), btn, TRUE, TRUE, 0);
  gtk_widget_show(btn);

  btn = gtk_button_new_with_label("Refresh");
  gtk_signal_connect_object(GTK_OBJECT(btn), "clicked",
			    GTK_SIGNAL_FUNC(mod_cb), (gpointer) 4);
  gtk_box_pack_start(GTK_BOX(box4), btn, TRUE, TRUE, 0);
  gtk_widget_show(btn);

  btn = gtk_button_new_with_label("About");
  gtk_signal_connect_object(GTK_OBJECT(btn), "clicked",
			    GTK_SIGNAL_FUNC(about_show), (gpointer) NULL);
  gtk_box_pack_start(GTK_BOX(box4), btn, TRUE, TRUE, 0);
  gtk_widget_show(btn);

  btn = gtk_button_new_with_label("Exit");
  gtk_signal_connect_object(GTK_OBJECT(btn), "clicked",
			    GTK_SIGNAL_FUNC(exit_button_cb), (gpointer) NULL);
  gtk_box_pack_start(GTK_BOX(box4), btn, TRUE, TRUE, 0);
  gtk_widget_show(btn);

  /* initialize the other panels now */
  browser_init();
  redm_init();
  grm_init();
  blm_init();
  am_init();
  brite_init();
  fx_init();

  /* The following is entirely gratuitous and serves only as a filler
     until the other notebook pages are ready. */
  if (1) {
    GtkWidget *dummy_box, *dummy_label;

    dummy_box = gtk_vbox_new(FALSE, 0);
    dummy_label = gtk_label_new("Image Rotation");
    /* Should actually be 7, but due to scale_init() not being called yet, it's 6. */
    gtk_notebook_insert_page(GTK_NOTEBOOK(ModMdi), dummy_box, dummy_label, 6);
    gtk_widget_show(dummy_label);
    gtk_widget_show(dummy_box);

    dummy_box = gtk_vbox_new(FALSE, 0);
    dummy_label = gtk_label_new("Texture Mapping");
    gtk_notebook_insert_page(GTK_NOTEBOOK(ModMdi), dummy_box, dummy_label, 9);
    gtk_widget_show(dummy_label);
    gtk_widget_show(dummy_box);

    dummy_box = gtk_vbox_new(FALSE, 0);
    dummy_label = gtk_label_new("Screenshot");
    gtk_notebook_insert_page(GTK_NOTEBOOK(ModMdi), dummy_box, dummy_label, 10);
    gtk_widget_show(dummy_label);
    gtk_widget_show(dummy_box);

    dummy_box = gtk_vbox_new(FALSE, 0);
    dummy_label = gtk_label_new("Webcam Viewer");
    gtk_notebook_insert_page(GTK_NOTEBOOK(ModMdi), dummy_box, dummy_label, 11);
    gtk_widget_show(dummy_label);
    gtk_widget_show(dummy_box);
  }

}

void
mod_show(void)
{
  gtk_widget_show(ModWin);
}

void
mod_hide(void)
{
  if (!GTK_WIDGET_VISIBLE(MainWindow))
     gtk_main_quit();
  else
    gtk_widget_hide(ModWin);
}

void
mod_cb(gpointer item)
{
  int i = (int) item;
  switch(i){
  case 1: gtk_widget_show(FileSel); break;
  case 2: SaveImage(NULL, NULL); break;
  case 3: gtk_widget_show(SaveSel); break;
  case 4: LoadImage(currentimage); break;
  default: break;
  }
}

void
mod_b(GtkWidget *widget, gint r, gint c, GdkEventButton *event, gpointer data)
{
	/* switch the pages */
	if(r >= 0){
    gtk_notebook_set_page(GTK_NOTEBOOK(ModMdi), r);
  }
}

void
exit_button_cb(GtkWidget *widget, gpointer data)
{
   gtk_main_quit();
}

gint
mod_key_press(GtkWidget *widget, GdkEventKey *event)
{
  switch (event->keyval) {
    case 'q':
    case 'Q':
      if (event->state & GDK_CONTROL_MASK) gtk_main_quit();
      break;
  }
  return TRUE;
}
