/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "etk_test.h"
#include "config.h"

static Etk_Bool _etk_test_menu_window_down_cb(Etk_Object *object, void *event_info, void *data);
static Etk_Widget *_etk_test_menu_item_new(const char *label, Etk_Menu_Shell *menu_shell);
static Etk_Widget *_etk_test_menu_stock_item_new(const char *label, Etk_Stock_Id stock_id, Etk_Menu_Shell *menu_shell);
static Etk_Widget *_etk_test_menu_check_item_new(const char *label, Etk_Menu_Shell *menu_shell);
static Etk_Widget *_etk_test_menu_radio_item_new(const char *label, Etk_Menu_Item_Radio *group_item, Etk_Menu_Shell *menu_shell);
static Etk_Widget *_etk_test_menu_separator_new(Etk_Menu_Shell *menu_shell);
static Etk_Bool _etk_test_menu_item_selected_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_test_menu_item_unselected_cb(Etk_Object *object, void *data);

static Etk_Widget *_etk_test_menu_statusbar;

/* Creates the window for the menu test */
void etk_test_menu_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *vbox;
   Etk_Widget *menu_bar;
   Etk_Widget *menu;
   Etk_Widget *menu_item;
   Etk_Widget *radio_item;
   Etk_Widget *label;
   Etk_Widget *toolbar;
   Etk_Widget *button;
   Etk_Widget *sep;

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Menu Test");
   etk_signal_connect_by_code(ETK_WINDOW_DELETE_EVENT_SIGNAL, ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   etk_widget_size_request_set(win, 325, 240);

   /****************
    * The window
    ****************/
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(win), vbox);

   /* Menubar */
   menu_bar = etk_menu_bar_new();
   etk_box_append(ETK_BOX(vbox), menu_bar, ETK_BOX_START, ETK_BOX_NONE, 0);

   /* Toolbar */
   toolbar = etk_toolbar_new();
   etk_box_append(ETK_BOX(vbox), toolbar, ETK_BOX_START, ETK_BOX_NONE, 0);

   button = etk_tool_button_new_from_stock(ETK_STOCK_EDIT_COPY);
   etk_toolbar_append(ETK_TOOLBAR(toolbar), button, ETK_BOX_START);

   button = etk_tool_button_new_from_stock(ETK_STOCK_EDIT_CUT);
   etk_toolbar_append(ETK_TOOLBAR(toolbar), button, ETK_BOX_START);

   button = etk_tool_button_new_from_stock(ETK_STOCK_EDIT_PASTE);
   etk_toolbar_append(ETK_TOOLBAR(toolbar), button, ETK_BOX_START);

   sep = etk_vseparator_new();
   etk_toolbar_append(ETK_TOOLBAR(toolbar), sep, ETK_BOX_START);

   button = etk_tool_button_new_from_stock(ETK_STOCK_EDIT_UNDO);
   etk_toolbar_append(ETK_TOOLBAR(toolbar), button, ETK_BOX_START);

   button = etk_tool_button_new_from_stock(ETK_STOCK_EDIT_REDO);
   etk_toolbar_append(ETK_TOOLBAR(toolbar), button, ETK_BOX_START);

   sep = etk_vseparator_new();
   etk_toolbar_append(ETK_TOOLBAR(toolbar), sep, ETK_BOX_START);

   button = etk_tool_toggle_button_new_from_stock(ETK_STOCK_EDIT_FIND);
   etk_toolbar_append(ETK_TOOLBAR(toolbar), button, ETK_BOX_START);

   /* Window body */
   label = etk_label_new("Click me! :)");
   etk_label_alignment_set(ETK_LABEL(label), 0.5, 0.5);
   etk_widget_pass_mouse_events_set(label, ETK_TRUE);
   etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   /* Statusbar */
   _etk_test_menu_statusbar = etk_statusbar_new();
   etk_box_append(ETK_BOX(vbox), _etk_test_menu_statusbar, ETK_BOX_END, ETK_BOX_NONE, 0);


   /****************
    * Menu Bar
    ****************/
   /* File Menu */
   menu_item = _etk_test_menu_item_new("File", ETK_MENU_SHELL(menu_bar));
   menu = etk_menu_new();
   etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
   _etk_test_menu_stock_item_new("Open", ETK_STOCK_DOCUMENT_OPEN, ETK_MENU_SHELL(menu));
   _etk_test_menu_stock_item_new("Save", ETK_STOCK_DOCUMENT_SAVE, ETK_MENU_SHELL(menu));

   /* Edit Menu */
   menu_item = _etk_test_menu_item_new("Edit", ETK_MENU_SHELL(menu_bar));
   menu = etk_menu_new();
   etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
   _etk_test_menu_stock_item_new("Cut", ETK_STOCK_EDIT_CUT, ETK_MENU_SHELL(menu));
   _etk_test_menu_stock_item_new("Copy", ETK_STOCK_EDIT_COPY, ETK_MENU_SHELL(menu));
   _etk_test_menu_stock_item_new("Paste", ETK_STOCK_EDIT_PASTE, ETK_MENU_SHELL(menu));

   /* Help Menu */
   menu_item = _etk_test_menu_item_new("Help", ETK_MENU_SHELL(menu_bar));
   menu = etk_menu_new();
   etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
   _etk_test_menu_item_new("About", ETK_MENU_SHELL(menu));

   /****************
    * Popup Menu
    ****************/
   /* Main menu */
   menu = etk_menu_new();
   etk_signal_connect_by_code(ETK_WIDGET_MOUSE_DOWN_SIGNAL, ETK_OBJECT(win), ETK_CALLBACK(_etk_test_menu_window_down_cb), menu);
   _etk_test_menu_stock_item_new("Open", ETK_STOCK_DOCUMENT_OPEN, ETK_MENU_SHELL(menu));
   _etk_test_menu_stock_item_new("Save", ETK_STOCK_DOCUMENT_SAVE, ETK_MENU_SHELL(menu));
   _etk_test_menu_separator_new(ETK_MENU_SHELL(menu));
   _etk_test_menu_stock_item_new("Cut", ETK_STOCK_EDIT_CUT, ETK_MENU_SHELL(menu));
   _etk_test_menu_stock_item_new("Copy", ETK_STOCK_EDIT_COPY, ETK_MENU_SHELL(menu));
   _etk_test_menu_stock_item_new("Paste", ETK_STOCK_EDIT_PASTE, ETK_MENU_SHELL(menu));
   _etk_test_menu_separator_new(ETK_MENU_SHELL(menu));
   menu_item = _etk_test_menu_item_new("Menu Item Test", ETK_MENU_SHELL(menu));

   /* Sub menu 1 */
   menu = etk_menu_new();
   etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
   _etk_test_menu_stock_item_new("Item with image", ETK_STOCK_DOCUMENT_SAVE, ETK_MENU_SHELL(menu));
   menu_item = _etk_test_menu_stock_item_new("Item with child", ETK_STOCK_DOCUMENT_OPEN, ETK_MENU_SHELL(menu));
   _etk_test_menu_separator_new(ETK_MENU_SHELL(menu));
   _etk_test_menu_check_item_new("Item with check 1", ETK_MENU_SHELL(menu));
   _etk_test_menu_check_item_new("Item with check 2", ETK_MENU_SHELL(menu));
   _etk_test_menu_separator_new(ETK_MENU_SHELL(menu));
   radio_item = _etk_test_menu_radio_item_new("Radio 1", NULL, ETK_MENU_SHELL(menu));
   radio_item = _etk_test_menu_radio_item_new("Radio 2", ETK_MENU_ITEM_RADIO(radio_item), ETK_MENU_SHELL(menu));
   _etk_test_menu_radio_item_new("Radio 3", ETK_MENU_ITEM_RADIO(radio_item), ETK_MENU_SHELL(menu));

   /* Sub menu 2 */
   menu = etk_menu_new();
   etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
   _etk_test_menu_item_new("Child Menu Test", ETK_MENU_SHELL(menu));

   etk_widget_show_all(win);
}

/* Called when the user clicks on the window */
static Etk_Bool _etk_test_menu_window_down_cb(Etk_Object *object, void *event_info, void *data)
{
   etk_menu_popup(ETK_MENU(data));
   return ETK_TRUE;
}

/* Creates a new "normal" menu item and appends it to the menu shell */
static Etk_Widget *_etk_test_menu_item_new(const char *label, Etk_Menu_Shell *menu_shell)
{
   Etk_Widget *menu_item;

   if (!menu_shell)
      return NULL;

   menu_item = etk_menu_item_new_with_label(label);
   etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));

   etk_signal_connect_by_code(ETK_MENU_ITEM_SELECTED_SIGNAL, ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_selected_cb), NULL);
   etk_signal_connect_by_code(ETK_MENU_ITEM_UNSELECTED_SIGNAL, ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_unselected_cb), NULL);

   return menu_item;
}

/* Creates a new menu item with stock and appends it to the menu shell */
static Etk_Widget *_etk_test_menu_stock_item_new(const char *label, Etk_Stock_Id stock_id, Etk_Menu_Shell *menu_shell)
{
   Etk_Widget *menu_item;
   Etk_Widget *image;

   if (!menu_shell)
      return NULL;

   menu_item = etk_menu_item_image_new_with_label(label);
   image = etk_image_new_from_stock(stock_id, ETK_STOCK_SMALL);
   etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
   etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));

   etk_signal_connect_by_code(ETK_MENU_ITEM_SELECTED_SIGNAL, ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_selected_cb), NULL);
   etk_signal_connect_by_code(ETK_MENU_ITEM_UNSELECTED_SIGNAL, ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_unselected_cb), NULL);

   return menu_item;
}


/* Creates a new menu item with a checkbox and appends it to the menu shell */
static Etk_Widget *_etk_test_menu_check_item_new(const char *label, Etk_Menu_Shell *menu_shell)
{
   Etk_Widget *menu_item;

   if (!menu_shell)
      return NULL;

   menu_item = etk_menu_item_check_new_with_label(label);
   etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));

   etk_signal_connect_by_code(ETK_MENU_ITEM_SELECTED_SIGNAL, ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_selected_cb), NULL);
   etk_signal_connect_by_code(ETK_MENU_ITEM_UNSELECTED_SIGNAL, ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_unselected_cb), NULL);

   return menu_item;
}

/* Creates a new menu item with a radiobox and appends it to the menu shell */
static Etk_Widget *_etk_test_menu_radio_item_new(const char *label, Etk_Menu_Item_Radio *group_item, Etk_Menu_Shell *menu_shell)
{
   Etk_Widget *menu_item;

   if (!menu_shell)
      return NULL;

   menu_item = etk_menu_item_radio_new_with_label_from_widget(label, group_item);
   etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));

   etk_signal_connect_by_code(ETK_MENU_ITEM_SELECTED_SIGNAL, ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_selected_cb), NULL);
   etk_signal_connect_by_code(ETK_MENU_ITEM_UNSELECTED_SIGNAL, ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_unselected_cb), NULL);

   return menu_item;
}

/* Creates a new menu separator and appends it to the menu shell */
static Etk_Widget *_etk_test_menu_separator_new(Etk_Menu_Shell *menu_shell)
{
   Etk_Widget *menu_item;

   menu_item = etk_menu_item_separator_new();
   etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));

   return menu_item;
}

/* Called when a menu item is selected */
static Etk_Bool _etk_test_menu_item_selected_cb(Etk_Object *object, void *data)
{
   Etk_Menu_Item *item;

   if (!(item = ETK_MENU_ITEM(object)))
      return ETK_TRUE;
   etk_statusbar_message_push(ETK_STATUSBAR(_etk_test_menu_statusbar), etk_menu_item_label_get(item), 0);
   return ETK_TRUE;
}

/* Called when a menu item is unselected */
static Etk_Bool _etk_test_menu_item_unselected_cb(Etk_Object *object, void *data)
{
   etk_statusbar_message_pop(ETK_STATUSBAR(_etk_test_menu_statusbar), 0);
   return ETK_TRUE;
}

