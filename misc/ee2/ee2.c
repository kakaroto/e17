/*****************************************
 * Electric Eyes 2                       *
 *****************************************
 * (c) 2000, Joshua Deere                *
 * dphase@locnet.net                     *
 *****************************************/

#include "ee2.h"

GtkWidget *MainWindow, *FileSel, *SaveSel,
			 *MenuBar, *area, *vbox1;

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
int i = 0;
gint simgw = 0, simgh = 0;
char currentimage[255];
char *imagefile = NULL;
char *splashfile = NULL;
Imlib_Image *im = NULL;

int main(int argc, char **argv)
{
	gtk_init (&argc, &argv);
	
	/* tell the widgets what they are */
	MainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	FileSel = gtk_file_selection_new("Open Image...");
	SaveSel = gtk_file_selection_new("Save Image As...");
	MenuBar = gtk_menu_bar_new();
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
	gtk_menu_bar_append(GTK_MENU_BAR(MenuBar), FileItem);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(ImageItem), ImageMenu);
	gtk_menu_bar_append(GTK_MENU_BAR(MenuBar), ImageItem);
	
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

	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(FileSel)->cancel_button),
							 "clicked", (GtkSignalFunc) CloseFileSel, FileSel);
	gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(FileSel)->ok_button),
							 "clicked", (GtkSignalFunc) FileOpen, FileSel);
	
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

	/* pack into box */
	gtk_box_pack_start(GTK_BOX(vbox1), MenuBar, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox1), area, TRUE, TRUE, 0);
	
	/* main stuff */
	if(argc == 2){
		sprintf(currentimage, "%s", argv[1]);
		LoadImage(argv[1]);
	} else {
		sprintf(currentimage, "./ee2.png");
		LoadImage("./ee2.png");
	}
	
	/* show the widgets */
	gtk_widget_show(MenuBar);
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


void CloseWindow(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	gtk_main_quit();
}

void CloseFileSel(GtkWidget *widget, GdkEvent *event, gpointer data)
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
	imgw = imlib_image_get_width();
	imgh = imlib_image_get_height();
	printf("%d - %d\n", imgw, imgh);
	imlib_render_image_on_drawable_at_size(0, 0, imgw, imgh);
	printf("%d - %d\n", imgw, imgh);
	gtk_widget_set_usize(area, imgw, imgh);
	gtk_widget_show(area);
//	gtk_widget_hide(area);	
}

void OpenImageFromMenu(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	gtk_widget_show(FileSel);
}

void SaveImage(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	printf("Saving %s\n...", currentimage);
	imlib_save_image(currentimage);
}

void RefreshImage(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	gtk_widget_hide(area);
	imlib_render_image_on_drawable(0, 0);
	gtk_widget_show(area);
}

void BlurImage(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	imlib_image_blur(1);
	imlib_render_image_on_drawable(0, 0);
}

void BlurImageMore(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	imlib_image_blur(2);
	imlib_render_image_on_drawable(0, 0);
}

void SharpenImage(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	imlib_image_sharpen(1);
	imlib_render_image_on_drawable(0, 0);
}

void SharpenImageMore(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	imlib_image_sharpen(2);
	imlib_render_image_on_drawable(0, 0);
}

void Flip1(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	imlib_image_flip_horizontal();
	imlib_render_image_on_drawable(0, 0);
}

void Flip2(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	imlib_image_flip_vertical();
	imlib_render_image_on_drawable(0, 0);
}

void Flip3(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	gtk_widget_hide(area);
	imlib_image_flip_diagonal();
	imgw = imlib_image_get_width();
	imgh = imlib_image_get_height();
	gtk_widget_set_usize(area, imgw, imgh);
	imlib_render_image_on_drawable(0, 0);
	gtk_widget_show(area);
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
	}
	
	imgw = imlib_image_get_width();
	imgh = imlib_image_get_height();
	
	/* only happens once! */
	if(im && i == 0){
		gtk_widget_set_usize(area, imgw, imgh);
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
