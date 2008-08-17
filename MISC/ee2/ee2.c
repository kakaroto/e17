/*****************************************
 * Electric Eyes 2                       *
 *****************************************
 * (c) 2000, Joshua Deere                *
 * dphase@locnet.net                     *
 *****************************************/

#include "ee2.h"

/* Xlib Stuff */
Display *disp = NULL;
Visual *vis = NULL;
Window root, win;
Colormap cm;
Pixmap pm;
int x;

/* globals */
int depth, imgw = 0, imgh = 0, winw = 0, winh = 0;
int xx = 0, yy = 0;
int onoff;
char currentimage[255];
char *imagefile = NULL;
Imlib_Image *bg = NULL;
Imlib_Image *im = NULL;
Imlib_Image *bimg = NULL;
GtkWidget *MainWindow, *area, *RootMenu, *FileSel, *SaveSel, *ModWin,
  *ModList, *ModMdi, *BrWin, *BrClist, *area2, *infol;

gint KeyPressedMain(GtkWidget *, GdkEventKey *);
gint KeyPressedMod(GtkWidget *, GdkEventKey *);

void
ee2_init(int argc, char **argv)
{
  GtkWidget *EventBox, *view;
  GtkObject *h_adj, *v_adj;

  /* The main window */
  MainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_policy(GTK_WINDOW(MainWindow), 1, 1, 1);
  gtk_window_set_title(GTK_WINDOW(MainWindow), "Electric Eyes 2");
  gtk_signal_connect(GTK_OBJECT(MainWindow), "delete_event",
		     GTK_SIGNAL_FUNC(CloseWindow), NULL);
  gtk_signal_connect(GTK_OBJECT(MainWindow), "destroy",
		     GTK_SIGNAL_FUNC(CloseWindow), NULL);

  /* The event box that contains the image's viewport */
  EventBox = gtk_event_box_new();
  gtk_container_add(GTK_CONTAINER(MainWindow), EventBox);
  gtk_signal_connect_object(GTK_OBJECT(EventBox), "button_press_event",
			    GTK_SIGNAL_FUNC(ButtonPressed), NULL);
  gtk_widget_set_events(EventBox, GDK_BUTTON_PRESS_MASK);

  /* a viewport to hold the drawing area */
  h_adj = gtk_adjustment_new(0.0,0.0,0.0,1.0,1.0,1.0);
  v_adj = gtk_adjustment_new(0.0,0.0,0.0,1.0,1.0,1.0);
  view = gtk_viewport_new (GTK_ADJUSTMENT(h_adj), GTK_ADJUSTMENT(v_adj));
  gtk_widget_show(view);

  /* The drawing area itself */
  area = gtk_drawing_area_new();
  gtk_signal_connect_after(GTK_OBJECT(area), "configure_event",
			   GTK_SIGNAL_FUNC(a_config), NULL);
  gtk_widget_show(area);
  gtk_container_add(GTK_CONTAINER(EventBox), view);
  gtk_container_add(GTK_CONTAINER(view), area);
  gtk_signal_connect(GTK_OBJECT(MainWindow), "key_press_event",
			   GTK_SIGNAL_FUNC(KeyPressedMain), NULL);
  dnd_init(view);

  FileSel = gtk_file_selection_new("Open Image...");
  gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(FileSel)->cancel_button),
		     "clicked", (GtkSignalFunc) CloseFileSel, FileSel);
  gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(FileSel)->ok_button),
		     "clicked", (GtkSignalFunc) FileOpen, FileSel);

  SaveSel = gtk_file_selection_new("Save Image As...");
  gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(SaveSel)->cancel_button),
		     "clicked", (GtkSignalFunc) CloseSaveSel, SaveSel);
  gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(SaveSel)->ok_button),
		     "clicked", (GtkSignalFunc) SaveImageAs, SaveSel);

  mod_init();
  about_init();
  webcam_init();

  if (argc == 2) {
    sprintf(currentimage, "%s", argv[1]);
    AddList(argv[1]);
  } else if (argc > 2) {
    int c;

    for (c = 1; c != argc; c++) {
      AddList(argv[c]);
    }
    mod_show();
  } else {
    sprintf(currentimage, SPLASHSCREEN);
    LoadImage(currentimage);
  }

  scale_init();  /* Can't do this until after images are loaded. */
	
  /* make sure everything is peachy for caching */
  check_cache();

  gtk_window_set_default_size(GTK_WINDOW(MainWindow), imlib_image_get_width(), imlib_image_get_height());

  gtk_widget_show(EventBox);
  gtk_widget_show(MainWindow);
}

int 
main(int argc, char **argv)
{
  gtk_init(&argc, &argv);
  ee2_init(argc, argv);
  gtk_main();
  return 0;
}

void 
LoadImage(char *imagetoload)
{
  D(("Loading image %s\n", imagetoload));

  /* free the image if there is one */
  if (im) {
    imlib_context_set_image(im);
    imlib_free_image();
    imgw = 0;
    imgh = 0;
  }
  if ((im = imlib_load_image(imagetoload)) == NULL) {
     /* FIXME: display a suitable error in the GUI */
     g_warning("Failed to load image.");
     /* Should probably check that this doesn't fail either. */
     im = imlib_load_image(SPLASHSCREEN); 
  }
  imlib_context_set_image(im);
}


void 
DrawChecks(void)
{
  /* renders the checkerboard scratch pad a la gimp */
  bg = imlib_create_image(CHECKS, CHECKS);
  imlib_context_set_image(bg);

  for (yy = 0; yy < CHECKS; yy += (CHECKS / 2)) {
    onoff = (yy / (CHECKS / 2)) & 0x1;
    for (xx = 0; xx < CHECKS; xx += (CHECKS / 2)) {
      if (onoff)
	imlib_context_set_color(144, 144, 144, 255);
      else
	imlib_context_set_color(100, 100, 100, 255);
      imlib_image_fill_rectangle(xx, yy, (CHECKS / 2), (CHECKS / 2));
      onoff++;
      if (onoff == 2)
	onoff = 0;
    }
  }
}

void 
Checks(int image_w, int image_h)
{
  for (yy = 0; yy < image_h; yy += CHECKS)
    for (xx = 0; xx < image_w; xx += CHECKS)
      imlib_blend_image_onto_image(bg, 0, 0, 0, CHECKS, CHECKS, xx, yy, CHECKS, CHECKS);
}

void 
CloseWindow(GtkWidget * widget, gpointer data)
{
  if (!GTK_WIDGET_VISIBLE(ModWin))
	  gtk_main_quit();
	else
		gtk_widget_hide(MainWindow);
}

void 
CloseFileSel(GtkWidget * widget, gpointer data)
{
  gtk_widget_hide(FileSel);
}

void 
FileOpen(GtkWidget * widget, GtkFileSelection * fs)
{
  imagefile = gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs));
  sprintf(currentimage, "%s", imagefile);
  gtk_widget_hide(FileSel);
	AddList(imagefile);
  LoadImage(imagefile);
  DrawImage(im, 0, 0);
}

void 
DrawImage(Imlib_Image * im, int w, int h)
{
  Pixmap pm;
  int x, y, c_test;

  D(("DrawImage(%8p, %d, %d)\n", im, w, h));

  if (!disp) {
    gtk_widget_realize(area);
    disp = GDK_WINDOW_XDISPLAY(area->window);
    win = GDK_WINDOW_XWINDOW(area->window);
    vis = GDK_VISUAL_XVISUAL(gtk_widget_get_visual(area));
    cm = GDK_COLORMAP_XCOLORMAP(gtk_widget_get_colormap(area));
    root = GDK_WINDOW_XWINDOW(area->window);
    depth = imlib_get_visual_depth(disp, vis);
  }
  if (!im)
    return;

  imlib_context_set_display(disp);
  imlib_context_set_visual(vis);
  imlib_context_set_colormap(cm);


  imlib_context_set_image(im);
  imgw = imlib_image_get_width();
  imgh = imlib_image_get_height();

  if (w == 0)
    w = imgw;
  if (h == 0)
    h = imgh;

  D(("w %d, h %d, imgw %d, imgh %d, winw %d, winh %d\n", w, h, imgw, imgh, winw, winh));
  winw = w;
  winh = h;

  gtk_window_set_default_size(GTK_WINDOW(MainWindow), (gint) w, (gint) h);
  gtk_widget_set_usize(area, w, h);

  pm = XCreatePixmap(disp, win, w, h, depth);
  imlib_context_set_drawable(pm);

  if(imlib_image_has_alpha()){
    if(bg == NULL){
      DrawChecks();
    }
    if(bimg){
      imlib_context_set_image(bimg);
      imlib_free_image();
    }
    bimg = imlib_create_image(w, h);
    imlib_context_set_image(bimg);
    Checks(w, h);

    if(w > imgw){
      x = (w - imgw) / 2;
      w = imgw;
    } else {
      x = 0;
    }
    
    if (h > imgh) {
      y = (h - imgh) / 2;
      h = imgh;
    } else {
      y = 0;
    }
    imlib_blend_image_onto_image(im, 1, 0, 0, w, h, x, y, w, h);

    imlib_render_image_on_drawable(0, 0);
    XSetWindowBackgroundPixmap(disp, win, pm);
    XClearWindow(disp, win);
  } else {
    imlib_context_set_image(im);
    imlib_render_image_on_drawable_at_size(0, 0, w, h);
    XSetWindowBackgroundPixmap(disp, win, pm);
    XClearWindow(disp, win);
  }

  /* decide whether to draw a thumbnail or not... */
  c_test = check_cache_file(currentimage);
  if(c_test == 0){ /* draw one */
    /*thumb = XCreatePixmap(disp, win, 100, 100, depth);
      imlib_context_set_drawable(thumb);
      imlib_render_image_on_drawable_at_size(0, 0, 100, 100);
      imlib_image_set_format("argb");
      printf("%s\n", cache);
      imlib_save_image("/home/tv/.ee2/blah.argb");*/
  }
	
  XFreePixmap(disp, pm);
  imlib_context_set_drawable(None);
}

void 
SaveImage(GtkWidget * widget, gpointer data)
{
  D(("Saving %s\n...", currentimage));
  imlib_context_set_image(im);
  imlib_save_image(currentimage);
  DrawImage(im, 0, 0);
}

void 
CloseSaveSel(GtkWidget * widget, gpointer data)
{
  gtk_widget_hide(SaveSel);
}

void 
SaveImageAs(GtkWidget * widget, GtkFileSelection * fs)
{
  imagefile = gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs));
  imlib_context_set_image(im);
  gtk_widget_hide(SaveSel);
  imlib_save_image(imagefile);
  DrawImage(im, 0, 0);
}

gint 
ButtonPressed(GtkWidget * widget, GdkEvent * event, gpointer data)
{
  GdkEventButton *event_button;

  if (event->type == GDK_BUTTON_PRESS) {
    event_button = (GdkEventButton *) event;
    if (event_button->button == 3) {
      /*gtk_menu_popup(GTK_MENU(RootMenu), NULL, NULL, NULL, NULL,
	event_button->button, event_button->time);*/
		if (GTK_WIDGET_VISIBLE(ModWin))
	      gtk_widget_hide(ModWin);
		else
			gtk_widget_show(ModWin);
      return TRUE;
    }
  }
  return FALSE;
}

gint
KeyPressedMain(GtkWidget *widget, GdkEventKey *event)
{
  GList *sel;
  switch (event->keyval) {
    case GDK_Page_Up:
    case GDK_BackSpace:
      sel = GTK_CLIST(BrClist)->selection;
      gtk_clist_select_row(GTK_CLIST(BrClist), (gint)sel->data - 1, 0);
      break;

    case GDK_Page_Down:
    case GDK_space:
      sel = GTK_CLIST(BrClist)->selection;
      gtk_clist_select_row(GTK_CLIST(BrClist), (gint)sel->data + 1, 0);
      break;

    case 'q':
    case 'Q':
      if (event->state & GDK_CONTROL_MASK) gtk_main_quit();
      break;
  }
  return FALSE;
}

gboolean 
a_config(GtkWidget * widget, GdkEventConfigure * event, gpointer user_data)
{
  D(("configure event\n"));
  if (event->width != winw || event->height != winh) {
    DrawImage(im, event->width, event->height);
  }
  return TRUE;
}
