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

static Etk_Bool _gui_tree_search(Gui *gui, Tree_Search direction);
static void _open_edje_file(Gui *gui);

static void _gui_menu_item_clicked_cb(Etk_Object *obj, void *data);
static void _gui_open_last_clicked_cb(Etk_Object *obj, void *data);
static void _gui_sort_parts_clicked_cb(Etk_Object *obj, void *data);
static void _gui_tree_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);
static void _gui_search_entry_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);
static void _gui_search_entry_text_changed_cb(Etk_Entry *entry, void *data);
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
   Etk_Tree_Col *col, *col2;
   Etk_Widget *hbox;
   Etk_Widget *signal_label, *signal_entry;
   Etk_Widget *source_label, *source_entry;
   Etk_Widget *send_button;
   Etk_Widget *separator;
   Etk_Bool check;

   int count, i;
   const char *recent;

   gui = calloc(1, sizeof(Gui));
   gui->win = etk_window_new();
   gui->popup = ETK_POPUP_WINDOW(etk_widget_new(ETK_POPUP_WINDOW_TYPE, "theme_group", "window", NULL));
   etk_window_title_set(ETK_WINDOW(gui->win), _(WINDOW_TITLE));
   etk_window_wmclass_set(ETK_WINDOW(gui->win), "edje_viewer", "Edje_viewer");
   etk_container_border_width_set(ETK_CONTAINER(gui->win), 0);
   etk_window_resize(ETK_WINDOW(gui->win), WINDOW_WIDTH, WINDOW_HEIGHT);

   gui->search_entry = etk_entry_new();
   etk_container_add(ETK_CONTAINER(gui->popup), gui->search_entry);
   etk_widget_show(gui->search_entry);
   etk_signal_connect("key_down", ETK_OBJECT(gui->search_entry), 
	 ETK_CALLBACK(_gui_search_entry_key_down_cb), gui);
   etk_signal_connect("text_changed", ETK_OBJECT(gui->search_entry), 
	 ETK_CALLBACK(_gui_search_entry_text_changed_cb), gui);

   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(gui->win), vbox);

   menubar = etk_menu_bar_new();
   etk_box_append(ETK_BOX(vbox), menubar,
	   ETK_BOX_START, ETK_BOX_NONE, 0);

   /* Main menu */
   menuitem = _gui_menubar_item_new(gui, _("File"), ETK_MENU_SHELL(menubar));
   menu = etk_menu_new();
   etk_menu_item_submenu_set(ETK_MENU_ITEM(menuitem), ETK_MENU(menu));
   _gui_menu_stock_item_new(gui, _("Open"), ETK_STOCK_DOCUMENT_OPEN,
	 ETK_MENU_SHELL(menu));
   menuitem = _gui_menu_stock_item_new(gui, _("Open recent"),
	 ETK_STOCK_DOCUMENT_OPEN, ETK_MENU_SHELL(menu));
   _gui_menu_stock_item_new(gui, _("Reload"), ETK_STOCK_VIEW_REFRESH,
	 ETK_MENU_SHELL(menu));
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

   /* Settings menu */
   menuitem = _gui_menubar_item_new(gui, _("Settings"),
	 ETK_MENU_SHELL(menubar));
   menu = etk_menu_new();
   etk_menu_item_submenu_set(ETK_MENU_ITEM(menuitem), ETK_MENU(menu));
   menuitem = etk_menu_item_check_new_with_label(_("Open Last"));
   etk_menu_shell_append(ETK_MENU_SHELL(menu), ETK_MENU_ITEM(menuitem));
   etk_signal_connect("activated", ETK_OBJECT(menuitem),
	   ETK_CALLBACK(_gui_open_last_clicked_cb), gui);
   check = edje_viewer_config_open_last_get();
   etk_menu_item_check_active_set(ETK_MENU_ITEM_CHECK(menuitem), check);

   menuitem = etk_menu_item_check_new_with_label(_("Sort parts"));
   etk_menu_shell_append(ETK_MENU_SHELL(menu), ETK_MENU_ITEM(menuitem));
   etk_signal_connect("activated", ETK_OBJECT(menuitem),
	   ETK_CALLBACK(_gui_sort_parts_clicked_cb), gui);
   check = edje_viewer_config_sort_parts_get();
   etk_menu_item_check_active_set(ETK_MENU_ITEM_CHECK(menuitem), check);

   /* Main content */
   paned = etk_hpaned_new();
   vpaned = etk_vpaned_new();
   etk_box_append(ETK_BOX(vbox), vpaned, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   etk_widget_padding_set(vpaned, 5, 5, 5, 0);
   etk_paned_child1_set(ETK_PANED(vpaned), paned, ETK_TRUE);

   gui->tree = etk_tree_new();
   etk_tree_headers_visible_set(ETK_TREE(gui->tree), ETK_TRUE);
   col = etk_tree_col_new(ETK_TREE(gui->tree), _("Part"), 80, 0.0);
   etk_tree_col_model_add(col, etk_tree_model_text_new());
   etk_tree_col_expand_set(col, ETK_TRUE);
   etk_tree_col_sort_set(col, gui_part_col_sort_cb, NULL);
   col2 = etk_tree_col_new(ETK_TREE(gui->tree), _("Visibility"), 60, 0.0);
   etk_tree_col_model_add(col2, etk_tree_model_checkbox_new());
   etk_tree_build(ETK_TREE(gui->tree));
   etk_widget_size_request_set(gui->tree, 300, 0);
   etk_signal_connect("key_down", ETK_OBJECT(gui->tree), ETK_CALLBACK(_gui_tree_key_down_cb), gui);
   etk_paned_child1_set(ETK_PANED(paned), gui->tree, ETK_FALSE);

   gui->canvas = etk_canvas_new();
   etk_widget_size_request_set(gui->canvas, 500, 500);
   etk_object_notification_callback_add(ETK_OBJECT(gui->canvas), "geometry",
	 canvas_resize_cb, NULL);
   etk_paned_child2_set(ETK_PANED(paned), gui->canvas, ETK_TRUE);

   gui->output = etk_tree_new();
   etk_tree_headers_visible_set(ETK_TREE(gui->output), ETK_TRUE);
   col = etk_tree_col_new(ETK_TREE(gui->output), _("Output"), 200, 0.0);
   etk_tree_col_model_add(col, etk_tree_model_text_new());
   etk_tree_col_expand_set(col, ETK_TRUE);
   etk_tree_build(ETK_TREE(gui->output));
   etk_widget_size_request_set(gui->output, 0, 100);
   etk_paned_child2_set(ETK_PANED(vpaned), gui->output, ETK_FALSE);

   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_widget_padding_set(hbox, 5, 5, 0, 5);

   signal_label = etk_label_new("Signal: ");
   etk_box_append(ETK_BOX(hbox), signal_label, ETK_BOX_START, ETK_BOX_NONE, 0);

   signal_entry = etk_entry_new();
   etk_box_append(ETK_BOX(hbox), signal_entry, ETK_BOX_START, ETK_BOX_NONE, 0);
   gui->signal_entry = signal_entry;

   source_label = etk_label_new("Source: ");
   etk_box_append(ETK_BOX(hbox), source_label, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_widget_padding_set(source_label, 5, 0, 0, 0);

   source_entry = etk_entry_new();
   etk_box_append(ETK_BOX(hbox), source_entry, ETK_BOX_START, ETK_BOX_NONE, 0);
   gui->source_entry = source_entry;

   send_button = etk_button_new_with_label("Send");
   etk_button_alignment_set(ETK_BUTTON(send_button), 1.0, 0.5);
   etk_box_append(ETK_BOX(hbox), send_button, ETK_BOX_END, ETK_BOX_NONE, 0);
   etk_signal_connect("clicked", ETK_OBJECT(send_button),
	   ETK_CALLBACK(_gui_send_clicked_cb), gui);

   bg_setup(ETK_CANVAS(gui->canvas));
   etk_widget_show_all(gui->win);

   check = edje_viewer_config_open_last_get();
   if (file) 
     {
	gui->path = file;
	_open_edje_file(gui);
     }
   else if (check)
     {
	file = edje_viewer_config_last_get();
	if (file)
	  {
	     gui->path = file;
	     _open_edje_file(gui);
	  }
     }
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

   etk_signal_connect("activated", ETK_OBJECT(menu_item),
	   ETK_CALLBACK(_gui_menu_item_clicked_cb), gui);

   return menu_item;

}

static Etk_Bool _gui_tree_search(Gui *gui, Tree_Search direction)
{
   Etk_Tree_Row *row, *current;
   const char *search_text, *col_text;

   search_text = etk_entry_text_get(ETK_ENTRY(gui->search_entry));
   current = etk_tree_selected_row_get(ETK_TREE(gui->tree));

   if (direction == TREE_SEARCH_START)
     for (row = etk_tree_first_row_get(ETK_TREE(gui->tree)); row; 
	   row = etk_tree_row_next_get(row))
       {
	  etk_tree_row_fields_get(row, 
		etk_tree_nth_col_get(ETK_TREE(gui->tree), 0), &col_text, NULL);
	  if (strstr (col_text, search_text) != NULL)
	    {
	       etk_tree_row_scroll_to(row, ETK_TRUE);
	       etk_tree_row_select(row);
	       return ETK_TRUE;
	    }
       }
   else if (direction == TREE_SEARCH_NEXT)
     for (row = etk_tree_row_next_get(current);
	   row; row = etk_tree_row_next_get(row))
       {
	  etk_tree_row_fields_get(row, 
		etk_tree_nth_col_get(ETK_TREE(gui->tree), 0), &col_text, NULL);
	  if (strstr (col_text, search_text) != NULL)
	    {
	       etk_tree_row_scroll_to(row, ETK_TRUE);
	       etk_tree_row_select(row);
	       return ETK_TRUE;
	    }
       }
   else if (direction == TREE_SEARCH_PREV)
     for (row = etk_tree_row_prev_get(current);
	   row; row = etk_tree_row_prev_get(row))
       {
	  etk_tree_row_fields_get(row, 
		etk_tree_nth_col_get(ETK_TREE(gui->tree), 0), &col_text, NULL);
	  if (strstr (col_text, search_text) != NULL)
	    {
	       etk_tree_row_scroll_to(row, ETK_TRUE);
	       etk_tree_row_select(row);
	       return ETK_TRUE;
	    }
       }
   return ETK_FALSE;	 
}

static void _open_edje_file(Gui *gui)
{
   list_entries(gui->path, ETK_TREE(gui->tree), ETK_CANVAS(gui->canvas));
   etk_window_title_set(ETK_WINDOW(gui->win), gui->path);
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
   else if (!strcmp(label, "Reload"))
     _open_edje_file(gui);
   else if (strstr(label, ".edj"))
     {
	gui->path = strdup(label);
	_open_edje_file(gui);
     }
}

static void _gui_open_last_clicked_cb(Etk_Object *obj, void *data)
{
   Etk_Menu_Item_Check *item;
   Gui *gui;
   Etk_Bool check;

   if (!(item = ETK_MENU_ITEM_CHECK(obj)))
     return;
   if (!(gui = data)) return;
   check = etk_menu_item_check_active_get(item);
   edje_viewer_config_open_last_set(check);
}

static void _gui_sort_parts_clicked_cb(Etk_Object *obj, void *data)
{
   Etk_Menu_Item_Check *item;
   Gui *gui;
   Etk_Bool check;

   if (!(item = ETK_MENU_ITEM_CHECK(obj)))
     return;
   if (!(gui = data)) return;
   check = etk_menu_item_check_active_get(item);
   edje_viewer_config_sort_parts_set(check);
}

static void _gui_tree_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   Gui *gui;
   int wx, wy, x, y, w, h;

   if (!(gui = data))
     return;

   if (!strcmp(event->key, "Return") || !strcmp(event->key, "KP_Enter") || !strcmp(event->keyname, "space"))
     {
	Etk_Tree_Row *row;
	Etk_Tree_Col *col;
	Etk_Bool checked;

	row = etk_tree_selected_row_get(ETK_TREE(gui->tree));
	col = etk_tree_nth_col_get(ETK_TREE(gui->tree), 1);
	etk_tree_row_fields_get(row, col, &checked, NULL);
	checked = checked ? ETK_FALSE : ETK_TRUE;
	etk_tree_row_fields_set(row, ETK_TRUE, col, checked, NULL);

	return;
     }

   if (!strlen(event->string) || !strcmp(event->key, "Tab") || !strcmp(event->key, "Escape") 
	 || !(event->modifiers == ETK_MODIFIER_NONE || event->modifiers == ETK_MODIFIER_SHIFT))
     return;

   etk_widget_geometry_get(gui->tree, &x, &y, &w, &h);
   etk_window_geometry_get(ETK_WINDOW(gui->win), &wx, &wy, NULL, NULL);
   etk_popup_window_popup_at_xy(gui->popup, wx + x + w/2, wy + y + h);
   etk_widget_focus(gui->search_entry);
   etk_entry_text_set(ETK_ENTRY(gui->search_entry), event->string);
}

static void _gui_search_entry_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   Gui *gui;

   if (!(gui = data))
     return;

   if (event->modifiers & ETK_MODIFIER_CTRL)
     {
	if (!strcmp(event->keyname, "g") || !strcmp(event->keyname, "s"))
	  _gui_tree_search(gui, TREE_SEARCH_NEXT);
     }
   else if (!strcmp(event->key, "Return") || !strcmp(event->key, "KP_Enter"))
     {
	Etk_Tree_Row *row;
	Etk_Tree_Col *col;
	Etk_Bool checked;

	row = etk_tree_selected_row_get(ETK_TREE(gui->tree));
	col = etk_tree_nth_col_get(ETK_TREE(gui->tree), 1);
	etk_tree_row_fields_get(row, col, &checked, NULL);
	checked = checked ? ETK_FALSE : ETK_TRUE;
	etk_tree_row_fields_set(row, ETK_TRUE, col, checked, NULL);

	etk_widget_focus(gui->tree);
	etk_popup_window_popdown(gui->popup);
     }
   else if (!strcmp(event->key, "Escape"))
     {
	etk_widget_focus(gui->tree);
	etk_popup_window_popdown(gui->popup);
     }
   else if (!strcmp(event->keyname, "Up"))
     _gui_tree_search(gui, TREE_SEARCH_PREV);
   else if (!strcmp(event->keyname, "Down"))
     _gui_tree_search(gui, TREE_SEARCH_NEXT);
}

static void _gui_search_entry_text_changed_cb(Etk_Entry *entry, void *data)
{
   Gui *gui;
   Evas_Object *editable;
   int cursor_pos;

   if (!(gui = data))
     return;

   if (!_gui_tree_search(gui, TREE_SEARCH_START))
     {
	editable = ETK_ENTRY(gui->search_entry)->editable_object;
	cursor_pos = etk_editable_cursor_pos_get(editable);
	etk_editable_delete(editable, cursor_pos - 1, cursor_pos);
     }
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
   etk_window_hide_on_delete(ETK_OBJECT(gui->fm_dialog), NULL);
   _open_edje_file(gui);
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
   Collection *co;
   Etk_Bool checked;
   Etk_Tree_Col *col;
   Gui *gui;

   if (!(co = etk_tree_row_data_get(row))) return;
   if (!(col = ETK_TREE_COL(obj)) || !row) return;
   if (!(gui = data)) return;

   if (!co->de)
     co->de = edje_part_create(ETK_TREE(gui->output), 
	   ETK_CANVAS(gui->canvas), co->file, co->part);

   etk_tree_row_fields_get(row, col, &checked, NULL);
   if (checked)
     edje_part_show(ETK_CANVAS(gui->canvas), co->de);
   else
     edje_part_hide(co->de);
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

int gui_part_col_sort_cb
(Etk_Tree *tree, Etk_Tree_Row *row1, Etk_Tree_Row *row2, Etk_Tree_Col *col,
 void *data)
{
   char *row1_value, *row2_value;

   if (!tree || !row1 || !row2 || !col)
      return 0;

   etk_tree_row_fields_get(row1, col, &row1_value, NULL);
   etk_tree_row_fields_get(row2, col, &row2_value, NULL);

   if (strcmp(row1_value, row2_value) < 0)
      return -1;
   else if (strcmp(row1_value, row2_value) > 0)
      return 1;
   else
      return 0;
}
