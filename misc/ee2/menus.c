/*****************************************
 * Electric Eyes 2                       *
 *****************************************
 * (c) 2000, Joshua Deere                *
 * dphase@locnet.net                     *
 *****************************************/

#include "ee2.h"

GtkWidget *RootMenu;

static void filemenu_cb(gpointer);
static void imagemenu_cb(gpointer);

void
menus_init(void)
{
  GtkWidget *menu, *item;

  RootMenu = gtk_menu_new();
  item = gtk_tearoff_menu_item_new();
  gtk_menu_append(GTK_MENU(RootMenu), item);
  gtk_widget_show(item);

  /* File Menu */
  menu = gtk_menu_new();
  item = gtk_tearoff_menu_item_new();
  gtk_menu_append(GTK_MENU(menu), item);
  gtk_widget_show(item);
  item = gtk_menu_item_new_with_label("Open...");
  gtk_menu_append(GTK_MENU(menu), item);
  gtk_signal_connect_object(GTK_OBJECT(item), "activate", GTK_SIGNAL_FUNC(filemenu_cb), (gpointer) 1);
  gtk_widget_show(item);
  item = gtk_menu_item_new_with_label("Save");
  gtk_menu_append(GTK_MENU(menu), item);
  gtk_signal_connect_object(GTK_OBJECT(item), "activate", GTK_SIGNAL_FUNC(filemenu_cb), (gpointer) 2);
  gtk_widget_show(item);
  item = gtk_menu_item_new_with_label("Save As...");
  gtk_menu_append(GTK_MENU(menu), item);
  gtk_signal_connect_object(GTK_OBJECT(item), "activate", GTK_SIGNAL_FUNC(filemenu_cb), (gpointer) 3);
  gtk_widget_show(item);
  item = gtk_menu_item_new();  /* Separator */
  gtk_widget_set_sensitive(item, FALSE);
  gtk_menu_append(GTK_MENU(menu), item);
  gtk_widget_show(item);
  item = gtk_menu_item_new_with_label("Exit");
  gtk_menu_append(GTK_MENU(menu), item);
  gtk_signal_connect_object(GTK_OBJECT(item), "activate", GTK_SIGNAL_FUNC(CloseWindow), (gpointer) NULL);
  gtk_widget_show(item);
  item = gtk_menu_item_new_with_label("File");
  gtk_widget_show(item);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), menu);
  gtk_menu_append(GTK_MENU(RootMenu), item);

  /* Image Menu */
  menu = gtk_menu_new();
  item = gtk_tearoff_menu_item_new();
  gtk_menu_append(GTK_MENU(menu), item);
  gtk_widget_show(item);
  item = gtk_menu_item_new_with_label("Refresh");
  gtk_menu_append(GTK_MENU(menu), item);
  gtk_signal_connect_object(GTK_OBJECT(item), "activate", GTK_SIGNAL_FUNC(imagemenu_cb), (gpointer) 1);
  gtk_widget_show(item);
  item = gtk_menu_item_new();  /* Separator */
  gtk_widget_set_sensitive(item, FALSE);
  gtk_menu_append(GTK_MENU(menu), item);
  gtk_widget_show(item);
  item = gtk_menu_item_new_with_label("Blur");
  gtk_menu_append(GTK_MENU(menu), item);
  gtk_signal_connect_object(GTK_OBJECT(item), "activate", GTK_SIGNAL_FUNC(imagemenu_cb), (gpointer) 2);
  gtk_widget_show(item);
  item = gtk_menu_item_new_with_label("Sharpen");
  gtk_menu_append(GTK_MENU(menu), item);
  gtk_signal_connect_object(GTK_OBJECT(item), "activate", GTK_SIGNAL_FUNC(imagemenu_cb), (gpointer) 3);
  gtk_widget_show(item);
  item = gtk_menu_item_new_with_label("Blur More");
  gtk_menu_append(GTK_MENU(menu), item);
  gtk_signal_connect_object(GTK_OBJECT(item), "activate", GTK_SIGNAL_FUNC(imagemenu_cb), (gpointer) 4);
  gtk_widget_show(item);
  item = gtk_menu_item_new_with_label("Sharpen More");
  gtk_menu_append(GTK_MENU(menu), item);
  gtk_signal_connect_object(GTK_OBJECT(item), "activate", GTK_SIGNAL_FUNC(imagemenu_cb), (gpointer) 5);
  gtk_widget_show(item);
  item = gtk_menu_item_new();  /* Separator */
  gtk_widget_set_sensitive(item, FALSE);
  gtk_menu_append(GTK_MENU(menu), item);
  gtk_widget_show(item);
  item = gtk_menu_item_new_with_label("Flip Horizontally");
  gtk_menu_append(GTK_MENU(menu), item);
  gtk_signal_connect_object(GTK_OBJECT(item), "activate", GTK_SIGNAL_FUNC(imagemenu_cb), (gpointer) 6);
  gtk_widget_show(item);
  item = gtk_menu_item_new_with_label("Flip Vertically");
  gtk_menu_append(GTK_MENU(menu), item);
  gtk_signal_connect_object(GTK_OBJECT(item), "activate", GTK_SIGNAL_FUNC(imagemenu_cb), (gpointer) 7);
  gtk_widget_show(item);
  item = gtk_menu_item_new_with_label("Flip Diagonally");
  gtk_menu_append(GTK_MENU(menu), item);
  gtk_signal_connect_object(GTK_OBJECT(item), "activate", GTK_SIGNAL_FUNC(imagemenu_cb), (gpointer) 8);
  gtk_widget_show(item);
  item = gtk_menu_item_new();  /* Separator */
  gtk_widget_set_sensitive(item, FALSE);
  gtk_menu_append(GTK_MENU(menu), item);
  gtk_widget_show(item);
  item = gtk_menu_item_new_with_label("Scale Image");
  gtk_menu_append(GTK_MENU(menu), item);
  gtk_signal_connect_object(GTK_OBJECT(item), "activate", GTK_SIGNAL_FUNC(imagemenu_cb), (gpointer) 9);
  gtk_widget_show(item);
  item = gtk_menu_item_new_with_label("Image");
  gtk_widget_show(item);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), menu);
  gtk_menu_append(GTK_MENU(RootMenu), item);
	
  /* view menu */
  menu = gtk_menu_new();
  item = gtk_tearoff_menu_item_new();
  gtk_menu_append(GTK_MENU(menu), item);
	gtk_widget_show(item);
	item = gtk_menu_item_new_with_label("Image Browser");
	gtk_menu_append(GTK_MENU(menu), item);
	gtk_signal_connect_object(GTK_OBJECT(item), "activate",
									  GTK_SIGNAL_FUNC(browser_show), (gpointer) NULL);
	gtk_widget_show(item);
	item = gtk_menu_item_new_with_label("View");
	gtk_widget_show(item);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), menu);
	gtk_menu_append(GTK_MENU(RootMenu), item);

  /* The rest of the root menu */
  item = gtk_menu_item_new(); /* seperator */
  gtk_menu_append(GTK_MENU(RootMenu), item);
  gtk_widget_set_sensitive(item, FALSE);
  gtk_widget_show(item);
  item = gtk_menu_item_new_with_label("Exit");
  gtk_menu_append(GTK_MENU(RootMenu), item);
  gtk_signal_connect_object(GTK_OBJECT(item), "activate", GTK_SIGNAL_FUNC(CloseWindow), (gpointer) NULL);
  gtk_widget_show(item);

  gtk_widget_show(RootMenu);
}

static void
filemenu_cb(gpointer item)
{
  int i = (int) item;

  switch (i) {
    case 1: gtk_widget_show(FileSel); break;
    case 2: SaveImage(NULL, NULL); break;
    case 3: gtk_widget_show(SaveSel); break;
    default: break;
  }
}

static void
imagemenu_cb(gpointer item)
{
  int i = (int) item;

  imlib_context_set_image(im);
  switch (i) {
    case 1: LoadImage(currentimage); break;
    case 2: imlib_image_blur(1); break;
    case 3: imlib_image_sharpen(1); break;
    case 4: imlib_image_blur(2); break;
    case 5: imlib_image_sharpen(2); break;
    case 6: imlib_image_flip_horizontal(); break;
    case 7: imlib_image_flip_vertical(); break;
    case 8: imlib_image_flip_diagonal(); break;
    case 9: break;
    default: break;
  }
  DrawImage(im, 0, 0);
}
