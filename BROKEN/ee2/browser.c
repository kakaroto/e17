/*****************************************
 * Electric Eyes 2                       *
 *****************************************
 * (c) 2000, Joshua Deere                *
 * dphase@locnet.net                     *
 *****************************************/

#include "ee2.h"

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
  GtkWidget *scroller, *vbox1;
  GtkWidget *nfr1, *nlbl;
	
  gchar *titles[1]={"Images"};

  nlbl = gtk_label_new("Main");
  nfr1 = gtk_frame_new("Main");
  gtk_container_set_border_width(GTK_CONTAINER(nfr1), 3);
  gtk_widget_show(nfr1);
  gtk_notebook_insert_page(GTK_NOTEBOOK(ModMdi), nfr1, nlbl, 0);
  
  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(nfr1), vbox1);

  scroller = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller),
				 GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  gtk_box_pack_start(GTK_BOX(vbox1), scroller, TRUE, TRUE, 0);
	
  /* clist stuff */
  BrClist = gtk_clist_new_with_titles(1, titles);
  dnd_init(BrClist);
  gtk_clist_set_row_height(GTK_CLIST(BrClist), 37);
  gtk_widget_set_usize(BrClist, 360, 365);
  gtk_container_add(GTK_CONTAINER(scroller), BrClist);
  gtk_clist_set_selection_mode(GTK_CLIST(BrClist), GTK_SELECTION_BROWSE);
  gtk_signal_connect(GTK_OBJECT(BrClist), "select_row",
		     GTK_SIGNAL_FUNC(browser_sel), NULL);
  gtk_widget_show(BrClist);
	
/*  cbtn = gtk_check_button_new_with_label("Hide Image Window");
  gtk_widget_show(cbtn);
  gtk_signal_connect(GTK_OBJECT(cbtn), "clicked",
		     GTK_SIGNAL_FUNC(check_callback), NULL);
  gtk_box_pack_start(GTK_BOX(vbox1), cbtn, TRUE, TRUE, 0);
*/

  gtk_widget_show(scroller);
  gtk_widget_show(vbox1);
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
  case 2: gtk_clist_clear(GTK_CLIST(BrClist)); break;
  default: break;
  }
}

void
check_callback(GtkWidget * widget, gpointer data)
{
  if(GTK_TOGGLE_BUTTON(widget)->active){
    gtk_widget_hide(MainWindow);
  } else {
    gtk_widget_show(MainWindow);
  }
}

void
browser_sel(GtkWidget *clist, gint row, gint column,
	    GdkEventButton *event, gpointer data)
{
  /* if this is a 2X click, show the image window if it's hidden */
  if (event)
    if (event->type == GDK_2BUTTON_PRESS && !GTK_WIDGET_VISIBLE(MainWindow))
      gtk_widget_show(MainWindow);

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
    /*if(imlib_image_has_alpha())
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
	    alp);*/
    /*gtk_label_set_text(GTK_LABEL(infol), lblt);*/
    prev_draw(im, area2->allocation.width, area2->allocation.height);
    /* the below is the thumbnail, disregard now, though it works */
    /*printf("%s\n", currentimage);
    tp = gdk_pixmap_new(area2->window, 35, 35, d);
    imlib_context_set_drawable(GDK_WINDOW_XWINDOW(tp));
    imlib_render_image_on_drawable_at_size(0, 0, 35, 35);
    gtk_clist_set_pixtext(GTK_CLIST(BrClist), row, column,
    currentimage, 5, tp, NULL);*/
  }
}

void
prev_draw(Imlib_Image *im, int w, int h)
{
  Pixmap pm_t;
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
	
  imlib_context_set_image(im);
  ww = imlib_image_get_width();
  hh = imlib_image_get_height();
	
  pm_t = XCreatePixmap(disp_t, win_t, w, h, d);
  imlib_context_set_drawable(pm_t);

  if(imlib_image_has_alpha()){
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
  } else {
    imlib_context_set_image(im);
    imlib_render_image_on_drawable_at_size(0, 0, w, h);
    XSetWindowBackgroundPixmap(disp_t, win_t, pm_t);
    XClearWindow(disp_t, win_t);
  }

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

  XFreePixmap(disp_t, pm_t);
  imlib_context_set_drawable(None);
}

gboolean
b_config(GtkWidget *widget, GdkEventConfigure *event, gpointer user_data)
{
  prev_draw(im, event->width, event->height);
  return TRUE;
}
