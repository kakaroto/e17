/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

#include "edje_viewer.h"

static Etk_Widget *_gui_menubar_item_new(Gui *gui, const char *label, 
	Etk_Menu_Shell *menu_shell);
static Etk_Widget *_gui_menu_stock_item_new(Gui *gui, const char *label, 
	Etk_Stock_Id stock_id, Etk_Menu_Shell *menu_shell);
static Etk_Widget *_gui_menu_item_new(Gui *gui, const char *label,
      Etk_Menu_Shell *menu_shell);
static void _gui_menu_item_selected_cb(Etk_Object *obj, void *data);
static void _gui_menu_item_deselected_cb(Etk_Object *obj, void *data);
static void _gui_menu_item_clicked_cb(Etk_Object *obj, void *data);
static void _gui_fm_ok_clicked_cb(Etk_Object *obj, void *data);
static void _gui_fm_cancel_clicked_cb(Etk_Object *obj, void *data);
static Etk_Bool _gui_main_window_deleted_cb(void *data);
static void _gui_open_edje_file_cb(Gui *gui);
static void _gui_tree_checkbox_toggled_cb(Etk_Object *obj, Etk_Tree_Row *row,
      void *data);
static void _gui_send_clicked_cb(Etk_Object *obj, void *data);

void main_window_show(char *file)
{
   Gui *gui;
   Etk_Widget *menubar;
   Etk_Widget *menu;
   Etk_Widget *menuitem;
   Etk_Widget *vbox;
   Etk_Widget *paned;
   Etk_Widget *vpaned;
   Etk_Widget *scrollview;
   Etk_Tree_Col *col, *col2;
   Etk_Widget *hbox;
   Etk_Widget *signal_label, *signal_entry;
   Etk_Widget *source_label, *source_entry;
   Etk_Widget *send_button;
   Etk_Widget *separator;

   int count, i;
   const char *recent;

   gui = calloc(1, sizeof(Gui));
   gui->win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(gui->win), _(WINDOW_TITLE));
   etk_window_wmclass_set(ETK_WINDOW(gui->win), "edje_viewer", "Edje_viewer");
   etk_container_border_width_set(ETK_CONTAINER(gui->win), 0);
   etk_window_resize(ETK_WINDOW(gui->win), WINDOW_WIDTH, WINDOW_HEIGHT);

   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(gui->win), vbox);

   menubar = etk_menu_bar_new();
   etk_box_pack_start(ETK_BOX(vbox), menubar, 
	   ETK_FALSE, ETK_FALSE, 0);

   menuitem = _gui_menubar_item_new(gui, _("File"), ETK_MENU_SHELL(menubar));
   menu = etk_menu_new();
   etk_menu_item_submenu_set(ETK_MENU_ITEM(menuitem), ETK_MENU(menu));
   _gui_menu_stock_item_new(gui, _("Open"), ETK_STOCK_DOCUMENT_OPEN,
	   ETK_MENU_SHELL(menu));
   menuitem = _gui_menu_stock_item_new(gui, _("Open recent"),
	 ETK_STOCK_DOCUMENT_OPEN, ETK_MENU_SHELL(menu));
   separator = etk_menu_item_separator_new();
   etk_menu_shell_append(ETK_MENU_SHELL(menu), ETK_MENU_ITEM(separator));
   _gui_menu_stock_item_new(gui, _("Quit"), ETK_STOCK_DIALOG_CLOSE,
	   ETK_MENU_SHELL(menu));

   /* Recent submenu */
   menu = etk_menu_new();
   etk_menu_item_submenu_set(ETK_MENU_ITEM(menuitem), ETK_MENU(menu));
   count = edje_viewer_config_count_get();
   for (i = 0; i < count; i++) 
     {
	recent = edje_viewer_config_recent_get(i+1);
	_gui_menu_stock_item_new(gui, recent, ETK_STOCK_X_OFFICE_DOCUMENT,
	      ETK_MENU_SHELL(menu));
     }
   if (count == 0)
     _gui_menu_item_new(gui, _("No recent files"), ETK_MENU_SHELL(menu));

   paned = etk_hpaned_new();
   vpaned = etk_vpaned_new();
   etk_box_pack_start(ETK_BOX(vbox), vpaned, ETK_TRUE, ETK_TRUE, 0);
   etk_paned_child1_set(ETK_PANED(vpaned), paned, ETK_TRUE);

   gui->tree = etk_tree_new();
   etk_tree_headers_visible_set(ETK_TREE(gui->tree), ETK_TRUE);
   col = etk_tree_col_new(ETK_TREE(gui->tree), _("Part"),
	 etk_tree_model_text_new(ETK_TREE(gui->tree)), 100);
   etk_tree_col_expand_set(col, ETK_TRUE);
   col2 = etk_tree_col_new(ETK_TREE(gui->tree), _("Visibility"),
	 etk_tree_model_checkbox_new(ETK_TREE(gui->tree)), 30);
   etk_tree_build(ETK_TREE(gui->tree));
   etk_paned_child1_set(ETK_PANED(paned), gui->tree, ETK_TRUE);

   scrollview = etk_scrolled_view_new();
   etk_paned_child2_set(ETK_PANED(paned), scrollview, ETK_TRUE);
   gui->canvas = etk_canvas_new();
   etk_widget_size_request_set(gui->canvas, 500, 500);
   etk_object_notification_callback_add(ETK_OBJECT(gui->canvas), "geometry",
	 canvas_resize_cb, NULL);
   etk_scrolled_view_add_with_viewport(ETK_SCROLLED_VIEW(scrollview),
	 gui->canvas);

   gui->output = etk_tree_new();
   etk_tree_headers_visible_set(ETK_TREE(gui->output), ETK_TRUE);
   col = etk_tree_col_new(ETK_TREE(gui->output), _("Output"),
	 etk_tree_model_text_new(ETK_TREE(gui->output)), 200);
   etk_tree_col_expand_set(col, ETK_TRUE);
   etk_tree_build(ETK_TREE(gui->output));
   etk_paned_child2_set(ETK_PANED(vpaned), gui->output, ETK_FALSE);

   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_pack_start(ETK_BOX(vbox), hbox, ETK_FALSE, ETK_FALSE, 0);

   signal_label = etk_label_new("Signal: ");
   etk_box_pack_start(ETK_BOX(hbox), signal_label, ETK_FALSE, ETK_FALSE, 0);

   signal_entry = etk_entry_new();
   etk_box_pack_start(ETK_BOX(hbox), signal_entry, ETK_FALSE, ETK_FALSE, 0);
   gui->signal_entry = signal_entry;

   source_label = etk_label_new("Source: ");
   etk_box_pack_start(ETK_BOX(hbox), source_label, ETK_FALSE, ETK_FALSE, 0);

   source_entry = etk_entry_new();
   etk_box_pack_start(ETK_BOX(hbox), source_entry, ETK_FALSE, ETK_FALSE, 0);
   gui->source_entry = source_entry;

   send_button = etk_button_new_with_label("Send");
   etk_button_alignment_set(ETK_BUTTON(send_button), 1.0, 0.5);
   etk_box_pack_end(ETK_BOX(hbox), send_button, ETK_FALSE, ETK_FALSE, 0);
   etk_signal_connect("clicked", ETK_OBJECT(send_button), 
	   ETK_CALLBACK(_gui_send_clicked_cb), gui);

   gui->status = etk_statusbar_new();
   etk_box_pack_end(ETK_BOX(vbox), gui->status, ETK_FALSE, ETK_FALSE, 0);

   bg_setup(ETK_CANVAS(gui->canvas));
   etk_widget_show_all(gui->win);

   if (file) list_entries(file, ETK_TREE(gui->tree), ETK_TREE(gui->output),
	 ETK_CANVAS(gui->canvas));
   etk_signal_connect("delete_event", ETK_OBJECT(gui->win), 
	   ETK_CALLBACK(_gui_main_window_deleted_cb), NULL);
   etk_signal_connect("cell_value_changed", ETK_OBJECT(col2),
	 ETK_CALLBACK(_gui_tree_checkbox_toggled_cb), gui);
}

static Etk_Widget *_gui_menubar_item_new (Gui *gui, const char *label, 
	Etk_Menu_Shell *menu_shell)
{
   Etk_Widget *menu_item = NULL;

   if (!menu_shell)
     return NULL;

   menu_item = etk_menu_item_new_with_label(label);
   etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));

   etk_signal_connect("selected", ETK_OBJECT(menu_item),
	   ETK_CALLBACK(_gui_menu_item_selected_cb), gui);
   etk_signal_connect("deselected", ETK_OBJECT(menu_item),
	   ETK_CALLBACK(_gui_menu_item_deselected_cb), gui);

   return menu_item;
}

static Etk_Widget *_gui_menu_stock_item_new(Gui *gui, const char *label, 
	Etk_Stock_Id stock_id, Etk_Menu_Shell *menu_shell)
{
   Etk_Widget *menu_item;
   Etk_Widget *image;

   if (!menu_shell)
     return NULL;

   menu_item = etk_menu_item_image_new_with_label(label);
   image = etk_image_new_from_stock(stock_id, ETK_STOCK_SMALL);
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));

   etk_signal_connect("selected", ETK_OBJECT(menu_item),
	   ETK_CALLBACK(_gui_menu_item_selected_cb), gui);
   etk_signal_connect("deselected", ETK_OBJECT(menu_item),
	   ETK_CALLBACK(_gui_menu_item_deselected_cb), gui);
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
	   ETK_CALLBACK(_gui_menu_item_clicked_cb), gui);

   return menu_item;

}

static Etk_Widget *_gui_menu_item_new(Gui *gui, const char *label,
      Etk_Menu_Shell *menu_shell)
{
   Etk_Widget *menu_item;

   if (!menu_shell)
     return NULL;

   menu_item = etk_menu_item_new_with_label(label);
   etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));

   etk_signal_connect("selected", ETK_OBJECT(menu_item),
	   ETK_CALLBACK(_gui_menu_item_selected_cb), gui);
   etk_signal_connect("deselected", ETK_OBJECT(menu_item),
	   ETK_CALLBACK(_gui_menu_item_deselected_cb), gui);
   etk_signal_connect("activated", ETK_OBJECT(menu_item),
	   ETK_CALLBACK(_gui_menu_item_clicked_cb), gui);

   return menu_item;

}

static void _gui_menu_item_selected_cb(Etk_Object *obj, void *data)
{
   Etk_Menu_Item *item;
   Gui *gui;

   if (!(item = ETK_MENU_ITEM(obj)))
     return;
   if (!(gui = data)) return;

   etk_statusbar_push(ETK_STATUSBAR(gui->status),
	   etk_menu_item_label_get(item), 0);

}

static void _gui_menu_item_deselected_cb(Etk_Object *obj, void *data)
{
   Gui *gui;

   if (!(gui = data)) return;
   etk_statusbar_pop(ETK_STATUSBAR(gui->status), 0);
}

static void _gui_menu_item_clicked_cb(Etk_Object *obj, void *data)
{
   Etk_Menu_Item *item;
   Gui *gui;
   const char *label;

   if (!(item = ETK_MENU_ITEM(obj)))
     return;
   if (!(gui = data)) return;

   label = etk_menu_item_label_get(item);

   if (!strcmp(label, "Quit"))
     _gui_main_window_deleted_cb(NULL);
   else if (!strcmp(label, "Open"))
     _gui_open_edje_file_cb(gui);
   else if (strstr(label, ".edj"))
     list_entries(label, ETK_TREE(gui->tree), ETK_TREE(gui->output),
	   ETK_CANVAS(gui->canvas));
}

static Etk_Bool _gui_main_window_deleted_cb(void *data) 
{
   etk_main_quit();
   return 1;
}

static void _gui_open_edje_file_cb(Gui *gui)
{
   Etk_Widget *button;
   char *cwd;

   if (gui->fm_dialog)
     {
	etk_widget_show_all(ETK_WIDGET(gui->fm_dialog));
	return;
     }

   cwd = malloc(1024);
   getcwd(cwd, 1024);

   gui->fm_dialog = etk_dialog_new();
   etk_window_title_set(ETK_WINDOW(gui->fm_dialog), _("Choose an edje file"));
   etk_signal_connect("delete_event", ETK_OBJECT(gui->fm_dialog),
	   ETK_CALLBACK(etk_window_hide_on_delete), NULL);	

   gui->fm_chooser = etk_filechooser_widget_new();
   if (cwd)
     etk_filechooser_widget_current_folder_set(
	     ETK_FILECHOOSER_WIDGET(gui->fm_chooser), cwd);
   FREE(cwd);
   etk_dialog_pack_in_main_area(ETK_DIALOG(gui->fm_dialog), gui->fm_chooser,
	   ETK_TRUE, ETK_TRUE, 0, ETK_FALSE);

   button = etk_dialog_button_add_from_stock(ETK_DIALOG(gui->fm_dialog), 
	   ETK_STOCK_DIALOG_OK, 1);
   etk_signal_connect("clicked", ETK_OBJECT(button), 
	   ETK_CALLBACK(_gui_fm_ok_clicked_cb), gui);
   button = etk_dialog_button_add_from_stock(ETK_DIALOG(gui->fm_dialog), 
	   ETK_STOCK_DIALOG_CLOSE, 2);
   etk_signal_connect("clicked", ETK_OBJECT(button), 
	   ETK_CALLBACK(_gui_fm_cancel_clicked_cb), gui);

   etk_widget_show_all(gui->fm_dialog);
}

static void _gui_fm_ok_clicked_cb(Etk_Object *obj, void *data)
{
   Gui *gui;
   const char *file;
   const char *dir;

   if (!(gui = data)) return;

   dir = etk_filechooser_widget_current_folder_get(
	   ETK_FILECHOOSER_WIDGET(gui->fm_chooser));
   file = etk_filechooser_widget_selected_file_get(
	   ETK_FILECHOOSER_WIDGET(gui->fm_chooser));
   gui->path = strdup(dir);
   gui->path = strcat(gui->path, "/");
   gui->path = strcat(gui->path, file);
   FREE(dir);
   FREE(file);
   list_entries(gui->path, ETK_TREE(gui->tree), ETK_TREE(gui->output),
	 ETK_CANVAS(gui->canvas));
   etk_window_hide_on_delete(ETK_OBJECT(gui->fm_dialog), NULL);
}

static void _gui_fm_cancel_clicked_cb(Etk_Object *obj, void *data)
{
   Gui *gui;

   if (!(gui = data)) return;

   etk_window_hide_on_delete(ETK_OBJECT(gui->fm_dialog), NULL);
}

static void _gui_tree_checkbox_toggled_cb(Etk_Object *obj, Etk_Tree_Row *row,
      void *data)
{
   Demo_Edje *de;
   Etk_Bool checked;
   Etk_Tree_Col *col;
   Gui *gui;

   if (!(de = etk_tree_row_data_get(row))) return;
   if (!(col = ETK_TREE_COL(obj)) || !row) return;
   if (!(gui = data)) return;

   etk_tree_row_fields_get(row, col, &checked, NULL);
   if (checked)
     edje_part_show(gui->canvas, de);
   else
     edje_part_hide(de);
}

static void _gui_send_clicked_cb(Etk_Object *obj, void *data)
{
  Gui * gui;
  Evas_List *l;
  const char *sig, *src;

  gui = data;
  if (!gui) return;

  sig = etk_entry_text_get(ETK_ENTRY(gui->signal_entry));
  src = etk_entry_text_get(ETK_ENTRY(gui->source_entry));
  if (!sig) sig = "";
  if (!src) src = "";
  for(l = visible_elements_get(); l; l = l->next) {
    Demo_Edje *de;

    de = l->data; 
    if (!de) continue;
    edje_object_signal_emit(de->edje, sig, src);
  }
}
