/*****************************************
 * Electric Eyes 2                       *
 *****************************************
 * (c) 2000, Joshua Deere                *
 * dphase@locnet.net                     *
 *****************************************/

#include "ee2.h"

static GtkWidget *BrWin, *BrClist, *area2, *infol;
static void browser_cb(gpointer);
static gchar *cimg = NULL;

Display *disp_t = NULL;
Visual *vis_t = NULL;
Window root_t, win_t;
Colormap cm_t;
Pixmap pm_t;
int d;

void
browser_init(void)
{
  GtkWidget *scroller, *hbox1, *vbox1, *frame1, *frame2, *btn, *sep;
  gchar *titles[1]={"Images"};

  BrWin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width(GTK_CONTAINER(BrWin), 2);
  gtk_window_set_title(GTK_WINDOW(BrWin), "Electric Eyes2 Image Browser");
  gtk_signal_connect_object(GTK_OBJECT(BrWin), "delete_event",
														GTK_SIGNAL_FUNC(browser_hide), (gpointer) NULL);
  gtk_signal_connect_object(GTK_OBJECT(BrWin), "destroy",
														GTK_SIGNAL_FUNC(browser_hide), (gpointer) NULL);
	
  hbox1 = gtk_hbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(BrWin), hbox1);

  scroller = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller),
																 GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  gtk_box_pack_start(GTK_BOX(hbox1), scroller, TRUE, TRUE, 0);

  /* clist stuff */
  BrClist = gtk_clist_new_with_titles(1, titles);
  gtk_widget_set_usize(BrClist, 410, 150);
  gtk_container_add(GTK_CONTAINER(scroller), BrClist);
  gtk_clist_set_selection_mode(GTK_CLIST(BrClist), GTK_SELECTION_BROWSE);
  gtk_signal_connect(GTK_OBJECT(BrClist), "select_row",
										 GTK_SIGNAL_FUNC(browser_sel), NULL);
  gtk_widget_show(BrClist);
	
	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox1), vbox1, TRUE, TRUE, 0);
	gtk_widget_show(vbox1);
	
	frame1 = gtk_frame_new("Image Preview");
	gtk_box_pack_start(GTK_BOX(vbox1), frame1, TRUE, TRUE, 0);
	gtk_widget_show(frame1);
	
	area2 = gtk_drawing_area_new();
	gtk_widget_set_usize(area2, 125, 125);
	gtk_container_add(GTK_CONTAINER(frame1), area2);
	gtk_widget_show(area2);
	
	frame2 = gtk_frame_new(NULL);
	gtk_box_pack_start(GTK_BOX(vbox1), frame2, TRUE, TRUE, 0);
	gtk_frame_set_shadow_type(GTK_FRAME(frame2), GTK_SHADOW_IN);
	gtk_widget_show(frame2);
	
	infol = gtk_label_new("\n"
												"Waiting...\n"
												"\n");
	gtk_label_set_justify(GTK_LABEL(infol), GTK_JUSTIFY_LEFT);
	gtk_container_add(GTK_CONTAINER(frame2), infol);
	gtk_widget_show(infol);
	
	btn = gtk_button_new_with_label("Load Image");
	gtk_signal_connect_object(GTK_OBJECT(btn), "clicked",
														GTK_SIGNAL_FUNC(browser_cb), (gpointer) 1);
	gtk_box_pack_start(GTK_BOX(vbox1), btn, TRUE, TRUE, 0);
	gtk_widget_show(btn);
	
	btn = gtk_button_new_with_label("Delete Image");
  gtk_box_pack_start(GTK_BOX(vbox1), btn, TRUE, TRUE, 0);
  gtk_widget_show(btn);
	
	btn = gtk_button_new_with_label("Rename Image");
  gtk_box_pack_start(GTK_BOX(vbox1), btn, TRUE, TRUE, 0);
  gtk_widget_show(btn);
	
	btn = gtk_button_new_with_label("Clear Image List");
  gtk_box_pack_start(GTK_BOX(vbox1), btn, TRUE, TRUE, 0);
  gtk_widget_show(btn);
	
	sep = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox1), sep, TRUE, TRUE, 0);
	gtk_widget_show(sep);
	
	btn = gtk_button_new_with_label("Hide");
  gtk_box_pack_start(GTK_BOX(vbox1), btn, TRUE, TRUE, 0);
	gtk_signal_connect_object(GTK_OBJECT(btn), "clicked",
														GTK_SIGNAL_FUNC(browser_hide), (gpointer) NULL);
  gtk_widget_show(btn);
	
	btn = gtk_button_new_with_label("Exit");
  gtk_box_pack_start(GTK_BOX(vbox1), btn, TRUE, TRUE, 0);
	gtk_signal_connect_object(GTK_OBJECT(btn), "clicked",
														GTK_SIGNAL_FUNC(CloseWindow), (gpointer) NULL);
  gtk_widget_show(btn);

  gtk_widget_show(scroller);
	gtk_widget_show(hbox1);
}

void
browser_show(void)
{
  gtk_widget_show(BrWin);
}

void
browser_hide(void)
{
  gtk_widget_hide(BrWin);
}

void 
AddList(char *foo)
{
  gtk_clist_append(GTK_CLIST(BrClist), (gchar **) &foo);
}

static void
browser_cb(gpointer item)
{
	int i = (int) item;
	
	switch(i){
	 case 1: gtk_widget_show(FileSel); break;
	 default: break;
	}
}

void
browser_sel(GtkWidget *clist, gint row, gint column,
						GdkEventButton *event, gpointer data)
{
  gchar lblt[255];
  char alp[255];
  int w, h;
	
  if (cimg) g_free(cimg);
  cimg = NULL;
  gtk_clist_get_text(GTK_CLIST(clist), row, 0, &cimg);
  
  if(cimg){
	   cimg = g_strdup(cimg);
		GetFileStats(cimg);
		LoadImage(cimg);
	   imlib_context_set_image(im);
		DrawImage(im, 0, 0);
  }
	if(im){
		if(imlib_image_has_alpha())
			sprintf(alp, "YES");
		else
			sprintf(alp, "NO");
		sprintf(lblt, "Resolution: %dx%d\n"
						"File Size: %d bytes\n"
						"Last Mod: %s"
						"Has Alpha: %s",
						imlib_image_get_width(),
						imlib_image_get_height(),
						EFile.Size,
						(ctime(&EFile.ModTime)),
						alp);
		gtk_label_set_text(GTK_LABEL(infol), lblt);
		prev_draw(im, area2->allocation.width, area2->allocation.height);
	   prev_draw(im, area2->allocation.width, area2->allocation.height);
  }
}

void
prev_draw(Imlib_Image *im, int w, int h)
{
	Pixmap pm_t, mask, thumb;
	int ww, hh;
	
	if(!disp_t){
		gtk_widget_realize(area2);
		disp_t = GDK_WINDOW_XDISPLAY(area2->window);
		win_t = GDK_WINDOW_XWINDOW(area2->window);
		vis_t = GDK_VISUAL_XVISUAL(gtk_widget_get_visual(area2));
		cm_t = GDK_COLORMAP_XCOLORMAP(gtk_widget_get_colormap(area2));
		root_t = GDK_WINDOW_XWINDOW(area2->window);
		d = imlib_get_visual_depth(disp_t, vis_t);
	}
	imlib_context_set_display(disp_t);
	imlib_context_set_visual(vis_t);
	imlib_context_set_colormap(cm_t);
	
	if(!im)
		return;
	
	gtk_widget_set_usize(area2, 125, 125);
	
	imlib_context_set_image(im);
	ww = imlib_image_get_width();
	hh = imlib_image_get_height();
	
	pm_t = XCreatePixmap(disp_t, win_t, w, h, d);
	imlib_context_set_drawable(pm_t);

	bg = NULL;
	DrawChecks();
	if(bimg){
		imlib_context_set_image(bimg);
		imlib_free_image();
	}
	bimg = imlib_create_image(ww, hh);
	imlib_context_set_image(bimg);
	Checks(ww, hh);
	
	imlib_blend_image_onto_image(im, 1, 0, 0, ww, hh, 0, 0, ww, hh);
	imlib_render_image_on_drawable_at_size(0, 0, w, h);
	
	XSetWindowBackgroundPixmap(disp_t, win_t, pm_t);
	XClearWindow(disp_t, win_t);
	XFreePixmap(disp_t, pm_t);
	imlib_context_set_drawable(None);
}
