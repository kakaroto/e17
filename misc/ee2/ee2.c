/*****************************************
 * Electric Eyes 2                       *
 *****************************************
 * (c) 2000, Joshua Deere                *
 * dphase@locnet.net                     *
 *****************************************/

#include "ee2.h"

GtkWidget *EventBox;

GtkWidget *MainWindow, *FileSel, *SaveSel,
			 *RootMenu, *area, *vbox1;

GtkWidget *FileMenu, *FileItem, *FOpen, *FSave, *FSaveAs,
			 *FSep1, *FExit;

GtkWidget *ImageMenu, *ImageItem, *IRefresh, *ISep1, *IBlur,
			 *IBlurMore, *ISep2, *ISharpen, *ISharpenMore, *ISep3,
			 *IFlip1, *IFlip2, *IFlip3, *ISep4, *IScale;

Display *disp;
Visual *vis;
Window root, win;
Colormap cm;
int depth, imgw = 0, imgh = 0;
int ww = 0, wh = 0;
int i = 0, xx = 0, yy = 0;
int onoff;
gint simgw = 0, simgh = 0;
char currentimage[255];
char *imagefile = NULL;
char *splashfile = NULL;
Imlib_Image *bg = NULL;
Imlib_Image *im = NULL;

int main(int argc, char **argv)
{
	gtk_init (&argc, &argv);
	
	/* tell the widgets what they are */
	MainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	EventBox = gtk_event_box_new();
	FileSel = gtk_file_selection_new("Open Image...");
	SaveSel = gtk_file_selection_new("Save Image As...");
	RootMenu = gtk_menu_new();
	area = gtk_drawing_area_new();
	vbox1 = gtk_vbox_new(FALSE, 0);
	FileMenu = gtk_menu_new();
	FileItem = gtk_menu_item_new_with_label("File");
	ImageMenu = gtk_menu_new();
	ImageItem = gtk_menu_item_new_with_label("ImageOps");
	FOpen = gtk_menu_item_new_with_label("Open Image");
	FSave = gtk_menu_item_new_with_label("Save Image");
	FSaveAs = gtk_menu_item_new_with_label("Save Image As");
	FSep1 = gtk_menu_item_new();
	gtk_widget_set_sensitive(FSep1, FALSE);
	FExit = gtk_menu_item_new_with_label("Exit");
	IRefresh = gtk_menu_item_new_with_label("Refresh Image");
	ISep1 = gtk_menu_item_new();
	gtk_widget_set_sensitive(ISep1, FALSE);
	IBlur = gtk_menu_item_new_with_label("Blur");
	IBlurMore = gtk_menu_item_new_with_label("Blur More");
	ISep2 = gtk_menu_item_new();
	gtk_widget_set_sensitive(ISep2, FALSE);
	ISharpen = gtk_menu_item_new_with_label("Sharpen");
	ISharpenMore = gtk_menu_item_new_with_label("Sharpen More");
	ISep3 = gtk_menu_item_new();
	gtk_widget_set_sensitive(ISep3, FALSE);
	IFlip1 = gtk_menu_item_new_with_label("Flip Horizontal");
	IFlip2 = gtk_menu_item_new_with_label("Flip Vertical");
	IFlip3 = gtk_menu_item_new_with_label("Flip Diagonal");
	ISep4 = gtk_menu_item_new();
	gtk_widget_set_sensitive(ISep4, FALSE);
	IScale = gtk_menu_item_new_with_label("Scale Image");
	
	gtk_window_set_title(GTK_WINDOW(MainWindow), "Electric Eyes 2");
	gtk_container_add(GTK_CONTAINER(MainWindow), vbox1);
	
	/* menu stuff */
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(FileItem), FileMenu);
	gtk_menu_append(GTK_MENU(RootMenu), FileItem);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(ImageItem), ImageMenu);
	gtk_menu_append(GTK_MENU(RootMenu), ImageItem);
	
	gtk_menu_append(GTK_MENU(FileMenu), FOpen);
	gtk_menu_append(GTK_MENU(FileMenu), FSave);
	gtk_menu_append(GTK_MENU(FileMenu), FSaveAs);
	gtk_menu_append(GTK_MENU(FileMenu), FSep1);
	gtk_menu_append(GTK_MENU(FileMenu), FExit);
	
	gtk_menu_append(GTK_MENU(ImageMenu), IRefresh);
	gtk_menu_append(GTK_MENU(ImageMenu), ISep1);
	gtk_menu_append(GTK_MENU(ImageMenu), IBlur);
	gtk_menu_append(GTK_MENU(ImageMenu), IBlurMore);
	gtk_menu_append(GTK_MENU(ImageMenu), ISep2);
	gtk_menu_append(GTK_MENU(ImageMenu), ISharpen);
	gtk_menu_append(GTK_MENU(ImageMenu), ISharpenMore);
	gtk_menu_append(GTK_MENU(ImageMenu), ISep3);
	gtk_menu_append(GTK_MENU(ImageMenu), IFlip1);
	gtk_menu_append(GTK_MENU(ImageMenu), IFlip2);
	gtk_menu_append(GTK_MENU(ImageMenu), IFlip3);
	gtk_menu_append(GTK_MENU(ImageMenu), ISep4);
	gtk_menu_append(GTK_MENU(ImageMenu), IScale);
	
	/* connect to signals */
	gtk_signal_connect(GTK_OBJECT(MainWindow), "delete_event",
							 GTK_SIGNAL_FUNC(CloseWindow), NULL);
	gtk_signal_connect(GTK_OBJECT(area), "configure_event",
							 GTK_SIGNAL_FUNC(a_config), NULL);
	gtk_signal_connect(GTK_OBJECT(area), "expose_event",
							 GTK_SIGNAL_FUNC(a_expose), NULL);

	gtk_signal_connect(GTK_OBJECT(FOpen), "activate",
							 GTK_SIGNAL_FUNC(OpenImageFromMenu), NULL);
	gtk_signal_connect(GTK_OBJECT(FSave), "activate",
							 GTK_SIGNAL_FUNC(SaveImage), NULL);
	gtk_signal_connect(GTK_OBJECT(FSaveAs), "activate",
							 GTK_SIGNAL_FUNC(ShowSaveSel), NULL);

	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(FileSel)->cancel_button),
							 "clicked", (GtkSignalFunc) CloseFileSel, FileSel);
	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(FileSel)->ok_button),
							 "clicked", (GtkSignalFunc) FileOpen, FileSel);

	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(SaveSel)->cancel_button),
							 "clicked", (GtkSignalFunc) CloseSaveSel, SaveSel);
	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(SaveSel)->ok_button),
							 "clicked", (GtkSignalFunc) SaveImageAs, SaveSel);
	
	gtk_signal_connect(GTK_OBJECT(FExit), "activate",
							 GTK_SIGNAL_FUNC(CloseWindow), NULL);
	
	gtk_signal_connect(GTK_OBJECT(IRefresh), "activate",
							 GTK_SIGNAL_FUNC(RefreshImage), NULL);
	gtk_signal_connect(GTK_OBJECT(IBlur), "activate",
							 GTK_SIGNAL_FUNC(BlurImage), NULL);
	gtk_signal_connect(GTK_OBJECT(IBlurMore), "activate",
							 GTK_SIGNAL_FUNC(BlurImageMore), NULL);
	gtk_signal_connect(GTK_OBJECT(ISharpen), "activate",
							 GTK_SIGNAL_FUNC(SharpenImage), NULL);
	gtk_signal_connect(GTK_OBJECT(ISharpenMore), "activate",
							 GTK_SIGNAL_FUNC(SharpenImageMore), NULL);
	gtk_signal_connect(GTK_OBJECT(IFlip1), "activate",
							 GTK_SIGNAL_FUNC(Flip1), NULL);
	gtk_signal_connect(GTK_OBJECT(IFlip2), "activate",
							 GTK_SIGNAL_FUNC(Flip2), NULL);
	gtk_signal_connect(GTK_OBJECT(IFlip3), "activate",
							 GTK_SIGNAL_FUNC(Flip3), NULL);
	
	/* button press events */
	gtk_signal_connect_object(GTK_OBJECT(EventBox), "button_press_event",
							 GTK_SIGNAL_FUNC(ButtonPressed), GTK_OBJECT(RootMenu));

	/* pack into box */
	gtk_box_pack_start(GTK_BOX(vbox1), EventBox, TRUE, TRUE, 0);
	
	/* pack the drawing area into the event box.  this
	 * will allow us to bind X events to the drawing 
	 * area... for menus and such...
	 */
	gtk_container_add(GTK_CONTAINER(EventBox), area);
	
	gtk_widget_set_events(EventBox, GDK_BUTTON_PRESS_MASK);
	
	/* main stuff */
	if(argc == 2){
		sprintf(currentimage, "%s", argv[1]);
		LoadImage(argv[1]);
	} else {
		sprintf(currentimage, "./ee2.png");
		LoadImage("./ee2.png");
	}
	
	/* show the widgets */
	gtk_widget_show(RootMenu);
	gtk_widget_show(EventBox);
	gtk_widget_show(FileMenu);
	gtk_widget_show(FileItem);
	gtk_widget_show(ImageMenu);
	gtk_widget_show(ImageItem);
	gtk_widget_show(FOpen);
	gtk_widget_show(FSave);
	gtk_widget_show(FSaveAs);
	gtk_widget_show(FSep1);
	gtk_widget_show(FExit);
	gtk_widget_show(IRefresh);
	gtk_widget_show(ISep1);
	gtk_widget_show(IBlur);
	gtk_widget_show(IBlurMore);
	gtk_widget_show(ISep2);
	gtk_widget_show(ISharpen);
	gtk_widget_show(ISharpenMore);
	gtk_widget_show(ISep3);
	gtk_widget_show(IFlip1);
	gtk_widget_show(IFlip2);
	gtk_widget_show(IFlip3);
	gtk_widget_show(ISep4);
	gtk_widget_show(IScale);
	gtk_widget_show(area);
	gtk_widget_show(vbox1);
	gtk_widget_show(MainWindow);

	gtk_main();
	return 0;
}

void LoadImage(char *imagetoload)
{
	printf("load image function\n");
	
	/* free the image if there is one */
	if(im){
		imlib_free_image();
		imgw = 0; imgh = 0;
	}
	
	im = imlib_load_image(imagetoload);
	imlib_context_set_image(im);
}


void DrawChecks(char image_w, char image_h)
{
	/* renders the checkerboard scratch pad a la gimp */
	bg = imlib_create_image(image_w, image_h);
	imlib_context_set_image(bg);
	
	for(yy = 0; yy < image_w; yy += 8){
		onoff = (yy / 8) & 0x1;
		for(xx = 0; xx < image_h; xx += 8){
			if(onoff)
			  imlib_context_set_color(144, 144, 144, 255);
			else
			  imlib_context_set_color(100, 100, 100, 255);
			imlib_image_fill_rectangle(xx, yy, 8, 8);
			onoff++;
			if(onoff == 2)
			  onoff = 0;
		}
	}
}

void CloseWindow(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}

void CloseFileSel(GtkWidget *widget, gpointer data)
{
	gtk_widget_hide(FileSel);
}

void FileOpen(GtkWidget *widget, GtkFileSelection *fs)
{
	printf("file open function\n");
	imagefile = gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs));
	sprintf(currentimage, "%s", imagefile);
	gtk_widget_hide(FileSel);
	gtk_widget_hide(area);
	LoadImage(imagefile);
	imlib_context_set_image(im);
	DrawChecks(imgw, imgh);
	imlib_context_set_image(im);
	imgw = imlib_image_get_width();
	imgh = imlib_image_get_height();
	printf("%d - %d\n", imgw, imgh);
	imlib_render_image_on_drawable_at_size(0, 0, imgw, imgh);
	printf("%d - %d\n", imgw, imgh);
	gtk_widget_set_usize(MainWindow, (gint)imgw, (gint)imgh);
	gtk_widget_set_usize(EventBox, (gint)imgw, (gint)imgh);
	gtk_widget_set_usize(area, (gint)imgw, (gint)imgh);
	gtk_widget_show(area);
}

void OpenImageFromMenu(GtkWidget *widget, gpointer data)
{
	gtk_widget_show(FileSel);
}

void SaveImage(GtkWidget *widget, gpointer data)
{
	printf("Saving %s\n...", currentimage);
	imlib_save_image(currentimage);
}

void ShowSaveSel(GtkWidget *widget, gpointer data)
{
	gtk_widget_show(SaveSel);
}

void CloseSaveSel(GtkWidget *widget, gpointer data)
{
	gtk_widget_hide(SaveSel);
}

void SaveImageAs(GtkWidget *widget, GtkFileSelection *fs)
{
	imagefile = gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs));
	gtk_widget_hide(SaveSel);
	imlib_save_image(imagefile);
}

void RefreshImage(GtkWidget *widget, gpointer data)
{
	gtk_widget_hide(area);
	imlib_render_image_on_drawable(0, 0);
	gtk_widget_show(area);
}

void BlurImage(GtkWidget *widget, gpointer data)
{
	imlib_image_blur(1);
	imlib_render_image_on_drawable(0, 0);
}

void BlurImageMore(GtkWidget *widget, gpointer data)
{
	imlib_image_blur(2);
	imlib_render_image_on_drawable(0, 0);
}

void SharpenImage(GtkWidget *widget, gpointer data)
{
	imlib_image_sharpen(1);
	imlib_render_image_on_drawable(0, 0);
}

void SharpenImageMore(GtkWidget *widget, gpointer data)
{
	imlib_image_sharpen(2);
	imlib_render_image_on_drawable(0, 0);
}

void Flip1(GtkWidget *widget, gpointer data)
{
	imlib_image_flip_horizontal();
	imlib_render_image_on_drawable(0, 0);
}

void Flip2(GtkWidget *widget, gpointer data)
{
	imlib_image_flip_vertical();
	imlib_render_image_on_drawable(0, 0);
}

void Flip3(GtkWidget *widget, gpointer data)
{
	gtk_widget_hide(area);
	imlib_image_flip_diagonal();
	imgw = imlib_image_get_width();
	imgh = imlib_image_get_height();
	gtk_widget_set_usize(MainWindow, (gint)imgw, (gint)imgh);
	gtk_widget_set_usize(EventBox, (gint)imgw, (gint)imgh);
	gtk_widget_set_usize(area, (gint)imgw, (gint)imgh);
	imlib_render_image_on_drawable(0, 0);
	gtk_widget_show(area);
}

gint ButtonPressed(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	GtkMenu *menu;
	GdkEventButton *event_button;
	
	menu = GTK_MENU(widget);
	if(event->type == GDK_BUTTON_PRESS){
		event_button = (GdkEventButton *)event;
		if(event_button->button == 3){
			gtk_menu_popup(menu, NULL, NULL, NULL, NULL,
								event_button->button, event_button->time);
			return TRUE;
		}
	}
	return FALSE;
}

gboolean a_config(GtkWidget *widget,
						GdkEventConfigure *event,
						gpointer user_data)
{
	if(!disp){
		disp = GDK_WINDOW_XDISPLAY(widget->window);
      win = GDK_WINDOW_XWINDOW(widget->window);
      vis = GDK_VISUAL_XVISUAL(gtk_widget_get_visual(widget));
      cm = GDK_COLORMAP_XCOLORMAP(gtk_widget_get_colormap(widget));
      root = GDK_WINDOW_XWINDOW(widget->window);
      imlib_get_visual_depth(disp, vis);
	}
	
	imlib_context_set_display(disp);
   imlib_context_set_drawable(win);
   imlib_context_set_visual(vis);
   imlib_context_set_colormap(cm);
	
	if(im){
		imlib_context_set_image(im);
		DrawChecks(imgw, imgh);
		imlib_context_set_image(im);
	}
	
	imgw = imlib_image_get_width();
	imgh = imlib_image_get_height();
	
	/* only happens once! */
	if(im && i == 0){
		gtk_widget_set_usize(MainWindow, (gint)imgw, (gint)imgh);
		gtk_widget_set_usize(EventBox, (gint)imgw, (gint)imgh);
		gtk_widget_set_usize(area, (gint)imgw, (gint)imgh);
	}
	
	printf("configure event\n");
	i++;
	return TRUE;
}

gboolean a_expose(GtkWidget *widget,
						GdkEventConfigure *event,
						gpointer user_data)
{
	printf("exposure event\n");
	if(im){
		imlib_render_image_on_drawable_at_size(0, 0, imgw, imgh);
	}
	return FALSE;
}
