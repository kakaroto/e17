/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

#include <edje_viewer_main.h>

static Etk_Widget *_gui_menubar_item_new(Gui *gui, const char *label,
	Etk_Menu_Shell *menu_shell);
static Etk_Widget *_gui_menu_item_new(Gui *gui, const char *label,
        Etk_Menu_Shell *menu_shell);
static Etk_Widget *_gui_menu_stock_item_new(Gui *gui, const char *label,
	Etk_Stock_Id stock_id, Etk_Menu_Shell *menu_shell,
        Etk_Callback callback);

static Etk_Bool _gui_tree_search(Gui *gui, Tree_Search direction);
static void _open_edje_file(Gui *gui);
static void _list_entries(Gui *gui);
static int _gui_part_col_sort_cb(Etk_Tree_Col *col, Etk_Tree_Row *row1,
      Etk_Tree_Row *row2, void *data);

static Etk_Bool _gui_open_last_clicked_cb(Etk_Object *obj, void *data);
static Etk_Bool _gui_sort_parts_clicked_cb(Etk_Object *obj, void *data);
static Etk_Bool _gui_tree_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);
static Etk_Bool _gui_search_entry_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);
static Etk_Bool _gui_search_entry_text_changed_cb(Etk_Entry *entry, void *data);
static Etk_Bool _gui_menu_item_reload_cb(Etk_Menu_Item *item, Gui *gui);
static Etk_Bool _gui_menu_item_quit_cb(Etk_Menu_Item *item, Gui *gui);
static Etk_Bool _gui_menu_item_open_recent_cb(Etk_Menu_Item *item, Gui *gui);
static Etk_Bool _gui_menu_item_open_edje_file_cb(Etk_Menu_Item *item, Gui *gui);
static Etk_Bool _gui_fm_ok_clicked_cb(Etk_Button *btn, Gui *gui);
static Etk_Bool _gui_fm_cancel_clicked_cb(Etk_Button *btn, Gui *gui);
static Etk_Bool _gui_mdi_window_delete_event_cb(Etk_Mdi_Window *mdi, Demo_Edje *de);
static Etk_Bool _gui_mdi_window_moved_event_cb(Etk_Mdi_Window *mdi __UNUSED__, int x, int y, Demo_Edje *de);
static Etk_Bool _gui_mdi_window_size_requested_cb(Etk_Object *mdi __UNUSED__, Etk_Size *size __UNUSED__, Demo_Edje *de);
static Etk_Bool _gui_main_window_deleted_cb(Etk_Window *obj, Gui *gui);
static Etk_Bool _gui_tree_checkbox_toggled_cb(Etk_Object *obj, Etk_Tree_Row *row,
      void *data);
static Etk_Bool _gui_emit_signal_cb(Etk_Object *obj, void *data);
static int _gui_visibility_col_sort_cb(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data);
static void _gui_row_data_string_free(void *data);
static void _gui_row_data_collection_free(void *data);

/* Global variables */
static Evas_Object *Highlighter;
static Eina_List *visible_elements = NULL;

Gui *main_window_show(const char *file)
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
   Evas *evas;
   Eina_List *l;

   int i, ret;
   const char *recent;

   gui = calloc(1, sizeof(Gui));
   gui->win = etk_window_new();
   gui->popup = ETK_POPUP_WINDOW(etk_widget_new(ETK_POPUP_WINDOW_TYPE, "theme-group", "window", NULL));
   etk_window_title_set(ETK_WINDOW(gui->win), _(WINDOW_TITLE));
   etk_window_wmclass_set(ETK_WINDOW(gui->win), "edje_viewer", "Edje_viewer");
   etk_container_border_width_set(ETK_CONTAINER(gui->win), 0);
   etk_window_resize(ETK_WINDOW(gui->win), WINDOW_WIDTH, WINDOW_HEIGHT);

   gui->search_entry = etk_entry_new();

   /* Load the configuration */
   gui->config = calloc(1, sizeof(Edje_Viewer_Config));
   ret = edje_viewer_config_load(gui);
   if (-1 == ret)
     return NULL;

   etk_container_add(ETK_CONTAINER(gui->popup), gui->search_entry);
   etk_widget_show(gui->search_entry);

   Etk_Signal_Connect_Desc search_entru_conn_desc[] = {
     ETK_SC_DESC(ETK_WIDGET_KEY_DOWN_SIGNAL,
		 _gui_search_entry_key_down_cb),
     ETK_SC_DESC(ETK_ENTRY_TEXT_CHANGED_SIGNAL,
		 _gui_search_entry_text_changed_cb),
     ETK_SC_DESC_SENTINEL
   };
   etk_signal_connect_multiple(search_entru_conn_desc,
			       ETK_OBJECT(gui->search_entry), gui);

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
	 ETK_MENU_SHELL(menu), _gui_menu_item_open_edje_file_cb);
   menuitem = _gui_menu_stock_item_new(gui, _("Open recent"),
	 ETK_STOCK_DOCUMENT_OPEN, ETK_MENU_SHELL(menu), NULL);
   _gui_menu_stock_item_new(gui, _("Reload"), ETK_STOCK_VIEW_REFRESH,
	 ETK_MENU_SHELL(menu), ETK_CALLBACK(_gui_menu_item_reload_cb));
   separator = etk_menu_item_separator_new();
   etk_menu_shell_append(ETK_MENU_SHELL(menu), ETK_MENU_ITEM(separator));
   _gui_menu_stock_item_new(gui, _("Quit"), ETK_STOCK_DIALOG_CLOSE,
	   ETK_MENU_SHELL(menu), _gui_menu_item_quit_cb);

   /* Recent submenu */
   menu = etk_menu_new();
   etk_menu_item_submenu_set(ETK_MENU_ITEM(menuitem), ETK_MENU(menu));
   if (!eina_list_count(gui->config->recent))
     _gui_menu_item_new(gui, _("No recent files"), ETK_MENU_SHELL(menu));
   else
     {
	for (l = gui->config->recent; l; l = l->next)
	  {
	     recent = l->data;
	     _gui_menu_stock_item_new(gui, recent, ETK_STOCK_X_OFFICE_DOCUMENT,
		   ETK_MENU_SHELL(menu), _gui_menu_item_open_recent_cb);
	  }
     }

   /* Settings menu */
   menuitem = _gui_menubar_item_new(gui, _("Settings"),
	 ETK_MENU_SHELL(menubar));
   menu = etk_menu_new();
   etk_menu_item_submenu_set(ETK_MENU_ITEM(menuitem), ETK_MENU(menu));
   menuitem = etk_menu_item_check_new_with_label(_("Open Last"));
   etk_menu_shell_append(ETK_MENU_SHELL(menu), ETK_MENU_ITEM(menuitem));
   etk_signal_connect_by_code(ETK_MENU_ITEM_CHECK_TOGGLED_SIGNAL,
			      ETK_OBJECT(menuitem),
			      ETK_CALLBACK(_gui_open_last_clicked_cb), gui);
   etk_menu_item_check_active_set(ETK_MENU_ITEM_CHECK(menuitem), gui->config->open_last);

   menuitem = etk_menu_item_check_new_with_label(_("Sort parts"));
   etk_menu_shell_append(ETK_MENU_SHELL(menu), ETK_MENU_ITEM(menuitem));
   etk_signal_connect_by_code(ETK_MENU_ITEM_CHECK_TOGGLED_SIGNAL,
			      ETK_OBJECT(menuitem),
			      ETK_CALLBACK(_gui_sort_parts_clicked_cb), gui);
   etk_menu_item_check_active_set(ETK_MENU_ITEM_CHECK(menuitem), gui->config->sort_parts);

   /* Main content */
   paned = etk_hpaned_new();
   vpaned = etk_vpaned_new();
   etk_box_append(ETK_BOX(vbox), vpaned, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   etk_widget_padding_set(vpaned, 5, 5, 5, 0);
   etk_paned_child1_set(ETK_PANED(vpaned), paned, ETK_TRUE);

   gui->tree = etk_tree_new();
   etk_tree_mode_set(ETK_TREE(gui->tree), ETK_TREE_MODE_TREE);
   etk_tree_expanders_visible_set(ETK_TREE(gui->tree), ETK_FALSE);
   etk_tree_depth_indent_set(ETK_TREE(gui->tree), 24);
   etk_tree_headers_visible_set(ETK_TREE(gui->tree), ETK_TRUE);
   col = etk_tree_col_new(ETK_TREE(gui->tree), _("Part"), 80, 0.0);
   etk_tree_col_model_add(col, etk_tree_model_text_new());
   etk_tree_col_expand_set(col, ETK_TRUE);
   etk_tree_col_sort_set(col, _gui_part_col_sort_cb, NULL);
   col2 = etk_tree_col_new(ETK_TREE(gui->tree), _("Visibility"), 60, 0.0);
   etk_tree_col_model_add(col2, etk_tree_model_checkbox_new());
   etk_tree_build(ETK_TREE(gui->tree));
   etk_tree_col_sort_set(col2, _gui_visibility_col_sort_cb, NULL);
   etk_widget_size_request_set(gui->tree, 300, 0);
   etk_signal_connect_by_code(ETK_WIDGET_KEY_DOWN_SIGNAL,
			      ETK_OBJECT(gui->tree),
			      ETK_CALLBACK(_gui_tree_key_down_cb), gui);
   etk_paned_child1_set(ETK_PANED(paned), gui->tree, ETK_FALSE);

   gui->mdi_area = etk_mdi_area_new();
   etk_paned_child2_set(ETK_PANED(paned), gui->mdi_area, ETK_TRUE);

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
   etk_signal_connect_by_code(ETK_ENTRY_TEXT_ACTIVATED_SIGNAL,
			      ETK_OBJECT(signal_entry),
			      ETK_CALLBACK(_gui_emit_signal_cb), gui);

   source_label = etk_label_new("Source: ");
   etk_box_append(ETK_BOX(hbox), source_label, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_widget_padding_set(source_label, 5, 0, 0, 0);

   source_entry = etk_entry_new();
   etk_box_append(ETK_BOX(hbox), source_entry, ETK_BOX_START, ETK_BOX_NONE, 0);
   gui->source_entry = source_entry;
   etk_signal_connect_by_code(ETK_ENTRY_TEXT_ACTIVATED_SIGNAL,
			      ETK_OBJECT(source_entry),
			      ETK_CALLBACK(_gui_emit_signal_cb), gui);

   send_button = etk_button_new_with_label("Send");
   etk_button_alignment_set(ETK_BUTTON(send_button), 1.0, 0.5);
   etk_box_append(ETK_BOX(hbox), send_button, ETK_BOX_END, ETK_BOX_NONE, 0);
   etk_signal_connect_by_code(ETK_BUTTON_CLICKED_SIGNAL,
			      ETK_OBJECT(send_button),
			      ETK_CALLBACK(_gui_emit_signal_cb), gui);

   etk_widget_show_all(gui->win);

   ThemeFile = PACKAGE_DATA_DIR"/edje_viewer.edj";
   evas = etk_widget_toplevel_evas_get(gui->win);
   Highlighter = edje_object_add(evas);
   edje_object_file_set(Highlighter, ThemeFile, "highlighter");

   if (file) 
     {
	gui->path = strdup(file);
	_open_edje_file(gui);
     }
   else if (gui->config->open_last)
     {
	char *last_file;

	last_file = eina_list_nth(gui->config->recent, 0);
	if (last_file)
	  {
	     gui->path = last_file;
	     _open_edje_file(gui);
	  }
     }
   etk_signal_connect_by_code(ETK_WINDOW_DELETE_EVENT_SIGNAL,
			      ETK_OBJECT(gui->win),
			      ETK_CALLBACK(_gui_main_window_deleted_cb), NULL);
   etk_signal_connect_by_code(ETK_TREE_COL_CELL_VALUE_CHANGED_SIGNAL,
			      ETK_OBJECT(col2),
			      ETK_CALLBACK(_gui_tree_checkbox_toggled_cb),
			      gui);
   return gui;
}

static Etk_Widget *_gui_menubar_item_new (Gui *gui __UNUSED__, const char *label,
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
	Etk_Stock_Id stock_id, Etk_Menu_Shell *menu_shell,
        Etk_Callback callback)
{
   Etk_Widget *menu_item;
   Etk_Widget *image;

   menu_item = etk_menu_item_image_new_with_label(label);
   image = etk_image_new_from_stock(stock_id, ETK_STOCK_SMALL);
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));

   if (callback)
      etk_signal_connect_by_code(ETK_MENU_ITEM_ACTIVATED_SIGNAL,
				 ETK_OBJECT(menu_item), callback, gui);

   return menu_item;

}

static Etk_Widget *_gui_menu_item_new(Gui *gui __UNUSED__, const char *label,
      Etk_Menu_Shell *menu_shell)
{
   Etk_Widget *menu_item;

   menu_item = etk_menu_item_new_with_label(label);
   etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));

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
   Eina_List *l;

   visible_elements = eina_list_free(visible_elements);

   _list_entries(gui);
   etk_window_title_set(ETK_WINDOW(gui->win), gui->path);
}

static void _list_entries(Gui *gui)
{
   Eina_List *entries = NULL, *collections = NULL;
   Etk_Tree_Col *col1;
   Etk_Tree_Row *row;
   Etk_Tree *tree = ETK_TREE(gui->tree);
   const char *file = gui->path;

   entries = edje_file_collection_list(file);
   col1 = etk_tree_nth_col_get(tree, 0);

   if (entries)
     {
	Eina_List *l;

	etk_tree_clear(tree);

	for (l = entries; l; l = l->next)
	  {
	     char *name;
	     Collection *co;

	     name = l->data;
	     co = calloc(1, sizeof(Collection));
	     collections = eina_list_append(collections, co);
	     co->file = strdup(file);
	     co->part = strdup(name);
	     co->de = NULL;

	     row = etk_tree_row_append(tree, NULL, col1, name, NULL);
	     etk_tree_row_data_set_full(row, co, _gui_row_data_collection_free);
	  }
	edje_file_collection_list_free(entries);
	edje_viewer_config_recent_set(gui, file);
	if (gui->config->sort_parts)
	  etk_tree_col_sort_set(col1, _gui_part_col_sort_cb, NULL);
     }
}

static Etk_Bool _gui_open_last_clicked_cb(Etk_Object *obj, void *data)
{
   Etk_Menu_Item_Check *item;
   Gui *gui;

   if (!(item = ETK_MENU_ITEM_CHECK(obj)))
     return ETK_TRUE;
   if (!(gui = data)) return ETK_TRUE;
   gui->config->open_last = etk_menu_item_check_active_get(item);

   return ETK_TRUE;
}

static Etk_Bool _gui_sort_parts_clicked_cb(Etk_Object *obj, void *data)
{
   Etk_Menu_Item_Check *item;
   Gui *gui;

   if (!(item = ETK_MENU_ITEM_CHECK(obj)))
     return ETK_TRUE;
   if (!(gui = data)) return ETK_TRUE;
   gui->config->sort_parts = etk_menu_item_check_active_get(item);

   return ETK_TRUE;
}

static Etk_Bool _gui_tree_key_down_cb(Etk_Object *object __UNUSED__, Etk_Event_Key_Down *event, void *data)
{
   Gui *gui;
   int wx, wy, x, y, w, h;

   if (!(gui = data))
     return ETK_TRUE;

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

	return ETK_TRUE;
     }

   if (!strlen(event->string) || !strcmp(event->key, "Tab") || !strcmp(event->key, "Escape") 
	 || !(event->modifiers == ETK_MODIFIER_NONE || event->modifiers == ETK_MODIFIER_SHIFT))
     return ETK_TRUE;

   etk_widget_geometry_get(gui->tree, &x, &y, &w, &h);
   etk_window_geometry_get(ETK_WINDOW(gui->win), &wx, &wy, NULL, NULL);
   etk_popup_window_popup_at_xy(gui->popup, wx + x + w/2, wy + y + h);
   etk_widget_focus(gui->search_entry);
   etk_entry_text_set(ETK_ENTRY(gui->search_entry), event->string);

   return ETK_TRUE;
}

static Etk_Bool _gui_search_entry_key_down_cb(Etk_Object *object __UNUSED__, Etk_Event_Key_Down *event, void *data)
{
   Gui *gui;

   if (!(gui = data))
     return ETK_TRUE;

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

   return ETK_TRUE;
}

static Etk_Bool _gui_search_entry_text_changed_cb(Etk_Entry *entry __UNUSED__, void *data)
{
   Gui *gui;
   Evas_Object *editable;
   int cursor_pos;

   if (!(gui = data))
     return ETK_TRUE;

   if (!_gui_tree_search(gui, TREE_SEARCH_START))
     {
	editable = ETK_ENTRY(gui->search_entry)->editable_object;
	cursor_pos = etk_editable_cursor_pos_get(editable);
	etk_editable_delete(editable, cursor_pos - 1, cursor_pos);
     }

   return ETK_TRUE;
}

static Etk_Bool _gui_main_window_deleted_cb(Etk_Window *obj __UNUSED__, Gui *gui __UNUSED__)
{
   etk_main_quit();
   return ETK_TRUE;
}

static Etk_Bool _gui_menu_item_reload_cb(Etk_Menu_Item *item __UNUSED__, Gui *gui)
{
   eet_clearcache();
   edje_file_cache_flush();
   _open_edje_file(gui);
   return ETK_TRUE;
}

static Etk_Bool _gui_menu_item_quit_cb(Etk_Menu_Item *item __UNUSED__, Gui *gui __UNUSED__)
{
   etk_main_quit();
   return ETK_TRUE;
}

static Etk_Bool _gui_menu_item_open_recent_cb(Etk_Menu_Item *item, Gui *gui)
{
   if (gui->path)
      free(gui->path);
   gui->path = strdup(etk_menu_item_label_get(item));
   _open_edje_file(gui);
   return ETK_TRUE;
}

static Etk_Bool _gui_menu_item_open_edje_file_cb(Etk_Menu_Item *item __UNUSED__, Gui *gui)
{
   Etk_Widget *button;
   char *cwd;

   if (gui->fm_dialog)
     {
	etk_widget_show_all(ETK_WIDGET(gui->fm_dialog));
	return ETK_TRUE;
     }

   cwd = malloc(1024);
   getcwd(cwd, 1024);

   gui->fm_dialog = etk_dialog_new();
   etk_window_title_set(ETK_WINDOW(gui->fm_dialog), _("Choose an edje file"));
   etk_signal_connect_by_code(ETK_WINDOW_DELETE_EVENT_SIGNAL,
			      ETK_OBJECT(gui->fm_dialog),
			      ETK_CALLBACK(etk_window_hide_on_delete), NULL);

   gui->fm_chooser = etk_filechooser_widget_new();
   if (cwd)
     etk_filechooser_widget_current_folder_set(
	     ETK_FILECHOOSER_WIDGET(gui->fm_chooser), cwd);
   FREE(cwd);
   etk_dialog_pack_in_main_area(ETK_DIALOG(gui->fm_dialog), gui->fm_chooser,
	   ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   button = etk_dialog_button_add_from_stock(ETK_DIALOG(gui->fm_dialog),
	   ETK_STOCK_DIALOG_OK, 1);
   etk_signal_connect_by_code(ETK_BUTTON_CLICKED_SIGNAL, ETK_OBJECT(button),
			      ETK_CALLBACK(_gui_fm_ok_clicked_cb), gui);
   button = etk_dialog_button_add_from_stock(ETK_DIALOG(gui->fm_dialog),
	   ETK_STOCK_DIALOG_CLOSE, 2);
   etk_signal_connect_by_code(ETK_BUTTON_CLICKED_SIGNAL, ETK_OBJECT(button),
			      ETK_CALLBACK(_gui_fm_cancel_clicked_cb), gui);

   etk_widget_show_all(gui->fm_dialog);

   return ETK_TRUE;
}

static Etk_Bool _gui_fm_ok_clicked_cb(Etk_Button *btn __UNUSED__, Gui *gui)
{
   Etk_Filechooser_Widget *fm_chooser;
   const char *file;
   const char *dir;
   int dir_size;

   fm_chooser = (Etk_Filechooser_Widget*)gui->fm_chooser;

   dir = etk_filechooser_widget_current_folder_get(fm_chooser);
   file = etk_filechooser_widget_selected_file_get(fm_chooser);

   if (!dir || !file)
      return ETK_TRUE;

   if (gui->path)
      free(gui->path);

   dir_size = strlen(dir);
   gui->path = malloc(dir_size + strlen(file) + 2);
   strcpy(gui->path, dir);
   gui->path[dir_size] = '/';
   strcpy(gui->path + dir_size + 1, file);

   _open_edje_file(gui);

   etk_widget_hide(gui->fm_dialog);
   return ETK_TRUE;
}

static Etk_Bool _gui_fm_cancel_clicked_cb(Etk_Button *btn __UNUSED__, Gui *gui)
{
   etk_widget_hide(gui->fm_dialog);
   return ETK_TRUE;
}

static Etk_Bool _gui_mdi_window_delete_event_cb(Etk_Mdi_Window *mdi __UNUSED__, Demo_Edje *de)
{
   if (!de) return ETK_FALSE;
   etk_tree_row_fields_set(de->tree_row, ETK_TRUE, de->tree_col, ETK_FALSE, NULL);
   de->prev.x = de->prev.y = de->prev.w = de->prev.h = 0;
   return ETK_FALSE;
}

static Etk_Bool _gui_mdi_window_moved_event_cb
(Etk_Mdi_Window *mdi __UNUSED__, int x __UNUSED__, int y __UNUSED__, Demo_Edje *de)
{
   Evas_Coord px, py, pw, ph, ox, oy;
   const char *name;

   if (!de->part_row) return ETK_TRUE;
   if (!(name = etk_tree_row_data_get(de->part_row))) return ETK_TRUE;

   evas_object_geometry_get(de->edje_object, &ox, &oy, NULL, NULL);
   edje_object_part_geometry_get(de->edje_object, name, &px, &py, &pw, &ph);

   evas_object_move(Highlighter, px + ox, py + oy);
   return ETK_TRUE;
}

static Etk_Bool _gui_mdi_window_size_requested_cb
(Etk_Object *mdi __UNUSED__, Etk_Size *size __UNUSED__, Demo_Edje *de)
{
   Evas_Coord px, py, pw, ph, x, y, w, h, dx, dy, dw, dh;
   const char *name;

   if (!de->part_row) return ETK_TRUE;
   if (!(name = etk_tree_row_data_get(de->part_row))) return ETK_TRUE;

   edje_object_part_geometry_get(de->edje_object, name, &px, &py, &pw, &ph);
   evas_object_geometry_get(Highlighter, &x, &y, &w, &h);
   if (!de->prev.x && !de->prev.y && !de->prev.w && !de->prev.h)
     {
	de->prev.x = px;
	de->prev.y = py;
	de->prev.w = pw;
	de->prev.h = ph;

	return ETK_TRUE;
     }

   dx = de->prev.x - px;
   dy = de->prev.y - py;
   dw = de->prev.w - pw;
   dh = de->prev.h - ph;

   evas_object_move(Highlighter, x - dx, y - dy);
   evas_object_resize(Highlighter, w - dw, h - dh);

   de->prev.x = px;
   de->prev.y = py;
   de->prev.w = pw;
   de->prev.h = ph;

   return ETK_TRUE;
}

static Etk_Bool _gui_tree_checkbox_toggled_cb(Etk_Object *obj,
					      Etk_Tree_Row *row,
					      void *data)
{
   Etk_Tree_Col *col, *col1;
   Gui *gui;
   Collection *co;
   Etk_Bool checked;

   if (!(col = ETK_TREE_COL(obj)) || !row) return ETK_TRUE;
   if (!(gui = data)) return ETK_TRUE;

   etk_tree_row_fields_get(row, col, &checked, NULL);

   /* FIXME: perhaps an etk function to return the depth of a given row? */
   /* The group row */
   if (row->parent == &row->tree->root)
     {
	if (!(co = etk_tree_row_data_get(row))) return ETK_TRUE;

	if (!co->de)
	  {
	     Eina_List *l = NULL, *parts = NULL;

	     col1 = etk_tree_nth_col_get(row->tree, 0);
	     co->de = edje_part_create(ETK_TREE(gui->output), 
		   ETK_MDI_AREA(gui->mdi_area), co->file, co->part);
	     co->de->tree_col = col;
	     co->de->tree_row = row;
	     co->de->data     = data;

	     etk_signal_connect_by_code(ETK_MDI_WINDOW_DELETE_EVENT_SIGNAL, ETK_OBJECT(co->de->mdi_window),
		   ETK_CALLBACK(_gui_mdi_window_delete_event_cb), co->de);
	     etk_signal_connect_by_code(ETK_MDI_WINDOW_MOVED_SIGNAL, ETK_OBJECT(co->de->mdi_window),
		   ETK_CALLBACK(_gui_mdi_window_moved_event_cb), co->de);
	     etk_signal_connect_by_code(ETK_WIDGET_SIZE_REQUESTED_SIGNAL, ETK_OBJECT(co->de->mdi_window),
		   ETK_CALLBACK(_gui_mdi_window_size_requested_cb), co->de);

	     parts = (Eina_List *) edje_edit_parts_list_get(co->de->edje_object);
	     for (l = parts; l; l = l->next)
	       {
		  Etk_Tree_Row *child;

		  char part_name[256];
		  snprintf(part_name, sizeof(part_name), "<i>%s</i>", (char *) l->data);
		  child = etk_tree_row_append(row->tree, row, col1, part_name, NULL);
		  etk_tree_row_data_set_full(child, strdup((char *) l->data), _gui_row_data_string_free);
	       }
	     edje_edit_string_list_free(parts);
	  }

	if (checked)
	  {
	     visible_elements = eina_list_append(visible_elements, co);
	     etk_widget_show_all(co->de->mdi_window);
	     etk_tree_row_unfold(row);
	  }
	else
	  {
	     visible_elements = eina_list_remove(visible_elements, co);
	     if (co->de->part_row)
	       etk_tree_row_fields_set(co->de->part_row, ETK_TRUE, col, ETK_FALSE, NULL);
	     etk_widget_hide(co->de->mdi_window);
	     etk_tree_row_fold(row);
	  }
     }
   else
     {
	Evas_Object *o;
	Evas_Coord px, py, pw, ph, ox, oy;
	char *name;

	if (!(co = etk_tree_row_data_get(row->parent))) return ETK_TRUE;
	if (!(name = etk_tree_row_data_get(row))) return ETK_TRUE;

	col1 = etk_tree_nth_col_get(row->tree, 0);
	o = co->de->edje_object;
	if (checked)
	  {
	     if (gui->part_row)
	       etk_tree_row_fields_set(gui->part_row, ETK_TRUE, col, ETK_FALSE, NULL);
	     evas_object_geometry_get(o, &ox, &oy, NULL, NULL);
	     edje_object_part_geometry_get(o, name, &px, &py, &pw, &ph);

	     evas_object_move(Highlighter, px + ox, py + oy);

	     evas_object_resize(Highlighter, pw, ph);

	     evas_object_raise(Highlighter);
	     evas_object_show(Highlighter);
	     edje_object_signal_emit(Highlighter, "edje_viewer,state,visible", "edje_viewer");
	     gui->part_row = co->de->part_row = row;
	  }
	else
	  {
	     edje_object_signal_emit(Highlighter, "edje_viewer,state,hidden", "edje_viewer");
	     evas_object_hide(Highlighter);
	     gui->part_row = co->de->part_row = NULL;
	  }
     }

   return ETK_TRUE;
}

static Etk_Bool _gui_emit_signal_cb(Etk_Object *obj __UNUSED__, void *data)
{
  Gui * gui;
  Eina_List *l;
  const char *sig, *src;

  gui = data;
  if (!gui) return ETK_TRUE;

  sig = etk_entry_text_get(ETK_ENTRY(gui->signal_entry));
  src = etk_entry_text_get(ETK_ENTRY(gui->source_entry));
  if (!sig) sig = "";
  if (!src) src = "";
  for (l = visible_elements; l; l = l->next)
    {
       Collection *co;

       co = l->data;
       if (!co) continue;
       edje_object_signal_emit(co->de->edje_object, sig, src);
    }

   return ETK_TRUE;
}

static int _gui_part_col_sort_cb
(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data __UNUSED__)
{
   char *row1_value, *row2_value;

   if (!row1 || !row2 || !col)
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

static int _gui_visibility_col_sort_cb
(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data __UNUSED__)
{
   Etk_Bool row1_value, row2_value;

   if (!row1 || !row2 || !col)
      return 0;

   etk_tree_row_fields_get(row1, col, &row1_value, NULL);
   etk_tree_row_fields_get(row2, col, &row2_value, NULL);

   if (row1_value == row2_value)
      return 0;
   else if (row1_value)
      return -1;
   else
      return 1;
}

static void _gui_row_data_string_free(void *data)
{
   free(data);
}

static void _gui_row_data_collection_free(void *data)
{
   Collection *co;

   co = data;
   if (co->de)
     etk_mdi_window_delete_request(ETK_MDI_WINDOW(co->de->mdi_window));
   FREE(co->part);
   FREE(co->file);
   FREE(co->de);
   FREE(co);
}
