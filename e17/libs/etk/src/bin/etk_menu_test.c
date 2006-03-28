#include "etk_test.h"
#include "config.h"

static void _etk_test_menu_window_down_cb(Etk_Object *object, void *event_info, void *data);
static Etk_Widget *_etk_test_menu_item_new(const char *label, Etk_Menu_Shell *menu_shell);
static Etk_Widget *_etk_test_menu_stock_item_new(const char *label, Etk_Stock_Id stock_id, Etk_Menu_Shell *menu_shell);
static Etk_Widget *_etk_test_menu_check_item_new(const char *label, Etk_Menu_Shell *menu_shell);
static Etk_Widget *_etk_test_menu_radio_item_new(const char *label, Etk_Menu_Item_Radio *group_item, Etk_Menu_Shell *menu_shell);
static Etk_Widget *_etk_test_menu_separator_new(Etk_Menu_Shell *menu_shell);
static void _etk_test_menu_item_selected_cb(Etk_Object *object, void *data);
static void _etk_test_menu_item_deselected_cb(Etk_Object *object, void *data);

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

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }
   
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), _("Etk Menu Test"));
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   etk_widget_size_request_set(win, 300, 200);
   
   /****************
    * The window
    ****************/
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(win), vbox);
   
   menu_bar = etk_menu_bar_new();
   etk_box_pack_start(ETK_BOX(vbox), menu_bar, ETK_FALSE, ETK_FALSE, 0);
   
   label = etk_label_new(_("Click me :)"));
   etk_label_alignment_set(ETK_LABEL(label), 0.5, 0.5);
   etk_widget_pass_mouse_events_set(label, ETK_TRUE);
   etk_box_pack_start(ETK_BOX(vbox), label, ETK_TRUE, ETK_TRUE, 0);
   
   _etk_test_menu_statusbar = etk_statusbar_new();
   etk_box_pack_end(ETK_BOX(vbox), _etk_test_menu_statusbar, ETK_FALSE, ETK_FALSE, 0);
   
   /****************
    * Menu Bar
    ****************/
   /* File Menu */
   menu_item = _etk_test_menu_item_new(_("File"), ETK_MENU_SHELL(menu_bar));
   menu = etk_menu_new();
   etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
   _etk_test_menu_stock_item_new(_("Open"), ETK_STOCK_DOCUMENT_OPEN, ETK_MENU_SHELL(menu));
   _etk_test_menu_stock_item_new(_("Save"), ETK_STOCK_DOCUMENT_SAVE, ETK_MENU_SHELL(menu));
   
   /* Edit Menu */
   menu_item = _etk_test_menu_item_new(_("Edit"), ETK_MENU_SHELL(menu_bar));
   menu = etk_menu_new();
   etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
   _etk_test_menu_stock_item_new(_("Cut"), ETK_STOCK_EDIT_CUT, ETK_MENU_SHELL(menu));
   _etk_test_menu_stock_item_new(_("Copy"), ETK_STOCK_EDIT_COPY, ETK_MENU_SHELL(menu));
   _etk_test_menu_stock_item_new(_("Paste"), ETK_STOCK_EDIT_PASTE, ETK_MENU_SHELL(menu));
   
   /* Help Menu */
   menu_item = _etk_test_menu_item_new(_("Help"), ETK_MENU_SHELL(menu_bar));
   menu = etk_menu_new();
   etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
   _etk_test_menu_item_new(_("About"), ETK_MENU_SHELL(menu));

   /****************
    * Popup Menu
    ****************/
   /* Main menu */
   menu = etk_menu_new();
   etk_signal_connect("mouse_down", ETK_OBJECT(win), ETK_CALLBACK(_etk_test_menu_window_down_cb), menu);
   _etk_test_menu_stock_item_new(_("Open"), ETK_STOCK_DOCUMENT_OPEN, ETK_MENU_SHELL(menu));
   _etk_test_menu_stock_item_new(_("Save"), ETK_STOCK_DOCUMENT_SAVE, ETK_MENU_SHELL(menu));
   _etk_test_menu_separator_new(ETK_MENU_SHELL(menu));
   _etk_test_menu_stock_item_new(_("Cut"), ETK_STOCK_EDIT_CUT, ETK_MENU_SHELL(menu));
   _etk_test_menu_stock_item_new(_("Copy"), ETK_STOCK_EDIT_COPY, ETK_MENU_SHELL(menu));
   _etk_test_menu_stock_item_new(_("Paste"), ETK_STOCK_EDIT_PASTE, ETK_MENU_SHELL(menu));
   _etk_test_menu_separator_new(ETK_MENU_SHELL(menu));
   menu_item = _etk_test_menu_item_new(_("Menu Item Test"), ETK_MENU_SHELL(menu));
   
   /* Sub menu 1 */
   menu = etk_menu_new();
   etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
   _etk_test_menu_stock_item_new(_("Item with image"), ETK_STOCK_DOCUMENT_SAVE, ETK_MENU_SHELL(menu));
   menu_item = _etk_test_menu_stock_item_new(_("Item with child"), ETK_STOCK_DOCUMENT_OPEN, ETK_MENU_SHELL(menu));
   _etk_test_menu_separator_new(ETK_MENU_SHELL(menu));
   _etk_test_menu_check_item_new(_("Item with check 1"), ETK_MENU_SHELL(menu));
   _etk_test_menu_check_item_new(_("Item with check 2"), ETK_MENU_SHELL(menu));
   _etk_test_menu_separator_new(ETK_MENU_SHELL(menu));
   radio_item = _etk_test_menu_radio_item_new(_("Radio 1"), NULL, ETK_MENU_SHELL(menu));
   radio_item = _etk_test_menu_radio_item_new(_("Radio 2"), ETK_MENU_ITEM_RADIO(radio_item), ETK_MENU_SHELL(menu));
   _etk_test_menu_radio_item_new(_("Radio 3"), ETK_MENU_ITEM_RADIO(radio_item), ETK_MENU_SHELL(menu));
   
   /* Sub menu 2 */
   menu = etk_menu_new();
   etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
   _etk_test_menu_item_new(_("Child Menu Test"), ETK_MENU_SHELL(menu));

   etk_widget_show_all(win);
}

/* Called when the user clicks on the window */
static void _etk_test_menu_window_down_cb(Etk_Object *object, void *event_info, void *data)
{
   etk_menu_popup(ETK_MENU(data));
}

/* Creates a new "normal" menu item and appends it to the menu shell */
static Etk_Widget *_etk_test_menu_item_new(const char *label, Etk_Menu_Shell *menu_shell)
{
   Etk_Widget *menu_item;
   
   if (!menu_shell)
      return NULL;
   
   menu_item = etk_menu_item_new_with_label(label);
   etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));
   
   etk_signal_connect("selected", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_selected_cb), NULL);
   etk_signal_connect("deselected", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_deselected_cb), NULL);
   
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
   
   etk_signal_connect("selected", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_selected_cb), NULL);
   etk_signal_connect("deselected", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_deselected_cb), NULL);
   
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
   
   etk_signal_connect("selected", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_selected_cb), NULL);
   etk_signal_connect("deselected", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_deselected_cb), NULL);
   
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
   
   etk_signal_connect("selected", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_selected_cb), NULL);
   etk_signal_connect("deselected", ETK_OBJECT(menu_item), ETK_CALLBACK(_etk_test_menu_item_deselected_cb), NULL);
   
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
static void _etk_test_menu_item_selected_cb(Etk_Object *object, void *data)
{
   Etk_Menu_Item *item;
   
   if (!(item = ETK_MENU_ITEM(object)))
      return;
   etk_statusbar_push(ETK_STATUSBAR(_etk_test_menu_statusbar), etk_menu_item_label_get(item), 0);
}

/* Called when a menu item is deselected */
static void _etk_test_menu_item_deselected_cb(Etk_Object *object, void *data)
{
   etk_statusbar_pop(ETK_STATUSBAR(_etk_test_menu_statusbar), 0);
}

