/* Evolve
 * Copyright (C) 2007-2008 Hisham Mardam-Bey 
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

#include <string.h>
#include "evolve_private.h"

#define constructor(widget, ...) \
     Etk_Widget *_etk_ ## widget ## _new() \
	 { \
	    return etk_ ## widget ## _new(__VA_ARGS__); \
	 }

#define constructor_evolve(w, ...) \
   Evolve_Widget *evolve_ ## w ## _new() \
       { \
	  Evolve_Widget *widget; \
	  widget = evolve_widget_new(#w); \
	  widget->widget = etk_ ## w ## _new(__VA_ARGS__); \
	  return widget; \
       }

extern Evolve *evolve;
static Etk_Widget *_etk_test_menu_item_new(const char *label, Etk_Menu_Shell *menu_shell);
static Etk_Widget *_etk_test_menu_stock_item_new(const char *label, Etk_Stock_Id stock_id, Etk_Menu_Shell *menu_shell);

constructor(alignment, 0.0, 0.0, 0.0, 0.0);
constructor(hbox, ETK_FALSE, 0);
constructor(vbox, ETK_FALSE, 0);
constructor(button);
constructor(canvas);
constructor(check_button);
constructor(colorpicker);
constructor(combobox);
constructor(dialog);
constructor(entry);
constructor(filechooser_widget);
constructor(frame, "frame");
constructor(iconbox);
constructor(image);
constructor(label, NULL);
constructor(menu);
constructor(menu_bar);
constructor(menu_item);
constructor(menu_item_separator);
constructor(menu_item_image);
constructor(menu_item_check);
constructor(menu_item_radio, NULL);
constructor(notebook);
constructor(hpaned);
constructor(vpaned);
constructor(progress_bar);
constructor(radio_button, NULL);
constructor(hscrollbar, 0.0, 1.0, 0.0, 0.1, 0.2, 0.3);
constructor(vscrollbar, 0.0, 1.0, 0.0, 0.1, 0.2, 0.3);
constructor(hseparator);
constructor(vseparator);
constructor(shadow);
constructor(hslider, 0.0, 1.0, 0.0, 0.1, 0.2);
constructor(vslider, 0.0, 1.0, 0.0, 0.1, 0.2);
constructor(spinner, 0.0, 10.0, 0.0, 0.1, 0.2);
constructor(statusbar);
constructor(table, 1, 1, ETK_FALSE);
constructor(text_view);
constructor(toggle_button);
constructor(tool_button);
constructor(toolbar);
constructor(tree);
constructor(viewport);
constructor(window);

constructor_evolve(alignment, 0.0, 0.0, 0.0, 0.0);
constructor_evolve(hbox, ETK_FALSE, 0);
constructor_evolve(vbox, ETK_FALSE, 0);
constructor_evolve(button);
constructor_evolve(canvas);
constructor_evolve(check_button);
constructor_evolve(colorpicker);
constructor_evolve(combobox);
constructor_evolve(dialog);
constructor_evolve(entry);
constructor_evolve(filechooser_widget);
constructor_evolve(frame, "frame");
constructor_evolve(iconbox);
constructor_evolve(image);
constructor_evolve(label, NULL);
constructor_evolve(menu);
constructor_evolve(menu_bar);
constructor_evolve(menu_item);
constructor_evolve(menu_item_separator);
constructor_evolve(menu_item_image);
constructor_evolve(menu_item_check);
constructor_evolve(menu_item_radio, NULL);
constructor_evolve(notebook);
constructor_evolve(hpaned);
constructor_evolve(vpaned);
constructor_evolve(progress_bar);
constructor_evolve(radio_button, NULL);
constructor_evolve(hscrollbar, 0.0, 1.0, 0.0, 0.1, 0.2, 0.3);
constructor_evolve(vscrollbar, 0.0, 1.0, 0.0, 0.1, 0.2, 0.3);
constructor_evolve(hseparator);
constructor_evolve(vseparator);
constructor_evolve(shadow);
constructor_evolve(hslider, 0.0, 1.0, 0.0, 0.1, 0.2);
constructor_evolve(vslider, 0.0, 1.0, 0.0, 0.1, 0.2);
constructor_evolve(spinner, 0.0, 10.0, 0.0, 0.1, 0.2);
constructor_evolve(statusbar);
constructor_evolve(table, 1, 1, ETK_FALSE);
constructor_evolve(text_view);
constructor_evolve(toggle_button);
constructor_evolve(tool_button);
constructor_evolve(toolbar);
constructor_evolve(tree);
constructor_evolve(viewport);
constructor_evolve(window);

Evolve_Widget *evolve_gui_alignment_new()
{
   Evolve_Widget *widget;
   Etk_Widget *button;
   
   widget = evolve_alignment_new();
   button = etk_button_new();
   etk_widget_size_request_set(button, 64, 64);
   etk_widget_theme_set(button,
			PACKAGE_DATA_DIR"/interfaces/evolve/evolve.eet",
			"checkered_button");
   etk_container_add(ETK_CONTAINER(widget->widget), button);
   etk_signal_connect("mouse-down", ETK_OBJECT(button),
		      ETK_CALLBACK(evolve_gui_mouse_down_new_cb), widget);
   return widget;
}

Evolve_Widget *evolve_gui_hbox_new()
{
   Evolve_Widget *widget;
   Etk_Widget *button;
   int i;
   
   widget = evolve_hbox_new();
   for (i = 0; i <= 2; ++i)
     {
	button = etk_button_new();
	etk_widget_size_request_set(button, 64, 64);
	etk_widget_theme_set(button,
			     PACKAGE_DATA_DIR"/interfaces/evolve/evolve.eet",
			     "checkered_button");
	etk_box_append(ETK_BOX(widget->widget), button, ETK_BOX_START, 
		       ETK_BOX_EXPAND_FILL, 0);
	etk_signal_connect("mouse-down", ETK_OBJECT(button),
			   ETK_CALLBACK(evolve_gui_mouse_down_new_cb), widget);
     }
   return widget;
}

Evolve_Widget *evolve_gui_vbox_new()
{
   Evolve_Widget *widget;
   Etk_Widget *button;
   int i;
   
   widget = evolve_vbox_new();
   for (i = 0; i <= 2; ++i)
     {
	button = etk_button_new();
	etk_widget_size_request_set(button, 64, 64);
	etk_widget_theme_set(button,
			     PACKAGE_DATA_DIR"/interfaces/evolve/evolve.eet",
			     "checkered_button");
	etk_box_append(ETK_BOX(widget->widget), button, ETK_BOX_START,
		       ETK_BOX_EXPAND_FILL, 0);
	etk_signal_connect("mouse-down", ETK_OBJECT(button),
			   ETK_CALLBACK(evolve_gui_mouse_down_new_cb), widget);
     }
   return widget;
}

Evolve_Widget *evolve_gui_button_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_button_new();
   evolve_widget_property_set(widget, "label", "button", EVOLVE_PROPERTY_STRING);
   return widget;
}

/* [TODO] [2] evolve_gui_canvas_new */

Evolve_Widget *evolve_gui_check_button_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_check_button_new();
   evolve_widget_property_set(widget, "label", "check button", EVOLVE_PROPERTY_STRING);
   return widget;
}

Evolve_Widget *evolve_gui_colorpicker_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_colorpicker_new();
   return widget;   
}

/* [TODO] [2] evolve_gui_combobox_new */
/* [TODO] [2] evolve_gui_dialog_new */

Evolve_Widget *evolve_gui_entry_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_entry_new();
   return widget;   
}

/* [TODO] [2] evolve_gui_filechooser_widget_new */

Evolve_Widget *evolve_gui_frame_new()
{
   Evolve_Widget *widget;
   Etk_Widget *button;
   
   widget = evolve_frame_new();
   evolve_widget_property_set(widget, "label", "frame", EVOLVE_PROPERTY_STRING);
   button = etk_button_new();
   etk_widget_size_request_set(button, 64, 64);
   etk_widget_theme_set(button,
			PACKAGE_DATA_DIR"/interfaces/evolve/evolve.eet",
			"checkered_button");   
   etk_signal_connect("mouse-down", ETK_OBJECT(button),
		      ETK_CALLBACK(evolve_gui_mouse_down_new_cb), widget);
   etk_container_add(ETK_CONTAINER(widget->widget), button);
   return widget;
}

Evolve_Widget *evolve_gui_iconbox_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_iconbox_new();
   return widget;   
}

/* [TODO] [2] evolve_gui_image_new: set a default image */
Evolve_Widget *evolve_gui_image_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_image_new();
   return widget;   
}

Evolve_Widget *evolve_gui_label_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_label_new();
   evolve_widget_property_set(widget, "label", "label", EVOLVE_PROPERTY_STRING);
   return widget;   
}

Evolve_Widget *evolve_gui_menu_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_menu_new();
   return widget;
}

Evolve_Widget *evolve_gui_menu_bar_new()
{
   Evolve_Widget *widget;
   Etk_Widget *menu_bar;
   Etk_Widget *menu;
   Etk_Widget *menu_item;
   
   widget = evolve_menu_bar_new();

   menu_bar = widget->widget;
   /* File Menu */
   menu_item = _etk_test_menu_item_new("File", ETK_MENU_SHELL(menu_bar));
   menu = etk_menu_new();
   etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
   _etk_test_menu_stock_item_new("Open", ETK_STOCK_DOCUMENT_OPEN, ETK_MENU_SHELL(menu));
   _etk_test_menu_stock_item_new("Save", ETK_STOCK_DOCUMENT_SAVE, ETK_MENU_SHELL(menu));
   _etk_test_menu_stock_item_new("Quit", ETK_STOCK_DIALOG_CLOSE, ETK_MENU_SHELL(menu));
   
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
   
   return widget;
}

/* [TODO] [2] evolve_gui_menu_new */
/* [TODO] [2] evolve_gui_menu_item_new */
/* [TODO] [2] evolve_gui_menu_item_seperator_new */
/* [TODO] [2] evolve_gui_menu_item_image_new */
/* [TODO] [2] evolve_gui_menu_item_check_new */
/* [TODO] [2] evolve_gui_menu_item_radio_new */

Evolve_Widget *evolve_gui_notebook_new()
{
   Evolve_Widget *widget;
   Etk_Widget *button;
   int i;
   
   widget = evolve_notebook_new();
   for (i = 0; i <=2; ++i)
     {
	char label[6];
	button = etk_button_new();
	etk_widget_size_request_set(button, 64, 64);
	etk_widget_theme_set(button,
			     PACKAGE_DATA_DIR"/interfaces/evolve/evolve.eet",
			     "checkered_button");   
	etk_signal_connect("mouse-down", ETK_OBJECT(button),
			   ETK_CALLBACK(evolve_gui_mouse_down_new_cb), widget);
	snprintf(label, sizeof(label), "tab %d\n", i);
	etk_notebook_page_append(ETK_NOTEBOOK(widget->widget), label, button);
     }   
   return widget;
}

Evolve_Widget *evolve_gui_vpaned_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_vpaned_new();
   return widget;
}

Evolve_Widget *evolve_gui_hpaned_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_hpaned_new();
   return widget;
}

Evolve_Widget *evolve_gui_progress_bar_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_progress_bar_new();
   return widget;
}

Evolve_Widget *evolve_gui_radio_button_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_radio_button_new();
   return widget;
}

Evolve_Widget *evolve_gui_vscrollbar_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_vscrollbar_new();
   return widget;
}

Evolve_Widget *evolve_gui_hscrollbar_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_hscrollbar_new();
   return widget;
}

Evolve_Widget *evolve_gui_vseparator_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_vseparator_new();
   return widget;
}

Evolve_Widget *evolve_gui_hseparator_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_hseparator_new();
   return widget;
}

/* [TODO] [2] evolve_gui_shadow_new */

Evolve_Widget *evolve_gui_vslider_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_vslider_new();
   return widget;
}

Evolve_Widget *evolve_gui_hslider_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_hslider_new();
   return widget;
}

Evolve_Widget *evolve_gui_spinner_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_spinner_new();
   return widget;
}

Evolve_Widget *evolve_gui_statusbar_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_statusbar_new();
   return widget;
}

Evolve_Widget *evolve_gui_table_new()
{
   Evolve_Widget *widget;
   Etk_Widget *button;
   int i;
   int j;
   
   widget = evolve_table_new();
   etk_table_resize(ETK_TABLE(widget->widget), 3, 3);
   for (i = 0; i <=2; ++i)
     {
	for (j = 0; j <=2; ++j)
	  {
	     button = etk_tool_button_new();
	     etk_widget_size_request_set(button, 64, 64);
	     etk_widget_theme_set(button,
				  PACKAGE_DATA_DIR"/interfaces/evolve/evolve.eet",
				  "checkered_button");   
	     etk_signal_connect("mouse-down", ETK_OBJECT(button),
				ETK_CALLBACK(evolve_gui_mouse_down_new_cb), widget);
	     etk_table_attach_default(ETK_TABLE(widget->widget), button,
				      i, i, j, j);
	  }
     }
   return widget;
}

Evolve_Widget *evolve_gui_text_view_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_text_view_new();
   return widget;
}

Evolve_Widget *evolve_gui_toggle_button_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_toggle_button_new();
   return widget;
}

Evolve_Widget *evolve_gui_tool_button_new()
{
   Evolve_Widget *widget;
   
   widget = evolve_tool_button_new();
   return widget;
}

Evolve_Widget *evolve_gui_toolbar_new()
{
   Evolve_Widget *widget;
   Etk_Widget *button;
   int i;
   
   widget = evolve_toolbar_new();
   for (i = 0; i <=4; ++i)
     {
	button = etk_tool_button_new();
	etk_widget_size_request_set(button, 64, 64);
	etk_widget_theme_set(button,
			     PACKAGE_DATA_DIR"/interfaces/evolve/evolve.eet",
			     "checkered_button");   
	etk_signal_connect("mouse-down", ETK_OBJECT(button),
			   ETK_CALLBACK(evolve_gui_mouse_down_new_cb), widget);
	etk_toolbar_append(ETK_TOOLBAR(widget->widget), button, ETK_BOX_START);
	if (i == 2)
	  etk_toolbar_append(ETK_TOOLBAR(widget->widget), etk_vseparator_new(), ETK_BOX_START);
     }
   return widget;
}

/* [TODO] [2] evolve_gui_tree_new */
/* [TODO] [2] evolve_gui_viewport_new */

Evolve_Widget *evolve_gui_window_new()
{
   Eina_List *l;
   Evolve_Widget *widget;
   Etk_Widget *button;
   int num = 0;
   char name[64];
   
   widget = evolve_window_new();
   etk_window_resize(ETK_WINDOW(widget->widget), 320, 240);
   button = etk_button_new();
   etk_widget_size_request_set(button, 64, 64);
   etk_widget_theme_set(button,
			PACKAGE_DATA_DIR"/interfaces/evolve/evolve.eet",
			"checkered_button");   
   etk_signal_connect("mouse-down", ETK_OBJECT(button),
		      ETK_CALLBACK(evolve_gui_mouse_down_new_cb), widget);
   etk_container_add(ETK_CONTAINER(widget->widget), button);
   etk_widget_show_all(widget->widget);
   
   for (l = evolve->widgets; l; l = l->next)
     {
	Evolve_Widget *w = l->data;
	
	if (w && w->type && !strcmp(w->type, widget->type))
	  ++num;
     }
   
   snprintf(name, sizeof(name), "%s%d", widget->type, num);
   widget->name = strdup(name);
   widget->parent = NULL;
   evolve->widgets = eina_list_append(evolve->widgets, widget);
   etk_signal_connect("mouse-down", ETK_OBJECT(widget->widget),
		      ETK_CALLBACK(evolve_gui_mouse_down_select_cb), widget);
   evolve_widget_property_set(widget, "title", widget->name, EVOLVE_PROPERTY_STRING);
   return widget;
}
  
/* Creates a new "normal" menu item and appends it to the menu shell */
static Etk_Widget *_etk_test_menu_item_new(const char *label, Etk_Menu_Shell *menu_shell)
{
   Etk_Widget *menu_item;
   
   if (!menu_shell)
     return NULL;
   
   menu_item = etk_menu_item_new_with_label(label);
   etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));
   
   /*
   etk_signal_connect("selected", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_selected_cb), NULL);
   etk_signal_connect("unselected", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_unselected_cb), NULL);
    */ 
   
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
   
   /*
   etk_signal_connect("selected", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_selected_cb), NULL);
   etk_signal_connect("unselected", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_unselected_cb), NULL);
   */
   
   return menu_item;
}
