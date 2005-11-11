#include "etk_test.h"
#include "config.h"

/* Called when the user tries to destroy the window */
static Etk_Bool _etk_test_menu_window_deleted_cb(Etk_Object *object, void *data)
{
   etk_widget_hide(ETK_WIDGET(object));
   return TRUE;
}

/* Called when the user clicks on the window */
static Etk_Bool _etk_test_menu_window_down_cb(Etk_Object *object, void *event_info, void *data)
{
   etk_menu_popup(ETK_MENU(data));
   
   return TRUE;
}

/* Create the window for the menu test */
void etk_test_menu_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *menu, *menu_item;
   Etk_Widget *label;
   Etk_Widget *image;

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }
   
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), _("Etk Menu Test"));
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(_etk_test_menu_window_deleted_cb), NULL);
   etk_widget_size_request_set(win, 300, 200);
   
   label = etk_label_new(_("Click me :)"));
   etk_label_alignment_set(ETK_LABEL(label), 0.5, 0.5);
   etk_container_add(ETK_CONTAINER(win), label);
   
   menu = etk_menu_new();
   menu_item = etk_menu_item_new_with_label(_("Menu Item Test"));
   image = etk_image_new_from_file(PACKAGE_DATA_DIR "/images/open.png");
   etk_menu_item_image_set(ETK_MENU_ITEM(menu_item), ETK_IMAGE(image));
   etk_menu_append(ETK_MENU(menu), ETK_MENU_ITEM(menu_item));
   etk_widget_show(menu_item);
   
   menu_item = etk_menu_item_new_with_label(_("Test 2"));
   etk_menu_append(ETK_MENU(menu), ETK_MENU_ITEM(menu_item));
   etk_widget_show(menu_item);
   
   etk_signal_connect("mouse_down", ETK_OBJECT(win), ETK_CALLBACK(_etk_test_menu_window_down_cb), menu);

   etk_widget_show_all(win);
}
