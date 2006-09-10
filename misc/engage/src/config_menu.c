#include "engage.h"
#include "config.h"

#ifdef HAVE_ETK
#include <Etk.h>

Etk_Widget *_od_config_menu = NULL;
Etk_Widget *_od_config_menu_app;
Etk_Widget *_od_config_menu_app_keep, *_od_config_menu_app_remove;

void _od_config_menu_zoom_none_cb(Etk_Object *object, void *data) {
  ecore_config_int_set("engage.options.zoom", 0);
  ecore_config_int_set("engage.options.stretch", 0);
}

void _od_config_menu_zoom_zoom_cb(Etk_Object *object, void *data) {
  ecore_config_int_set("engage.options.zoom", 1);
  ecore_config_int_set("engage.options.stretch", 0);
}

void _od_config_menu_zoom_both_cb(Etk_Object *object, void *data) {
  ecore_config_int_set("engage.options.zoom", 1);
  ecore_config_int_set("engage.options.stretch", 1);
}

void _od_config_menu_config_cb(Etk_Object *object, void *data) {
  if (!ecore_exe_run("examine engage", NULL))
    fprintf(stderr, "'examine' could not be launched - is it in your path?\n"); 
}

void _od_config_menu_quit_cb(Etk_Object *object, void *data) {
  ecore_main_loop_quit();
}

void od_config_menu_init(void) {
  Etk_Widget *menu, *sub_menu, *menu_item;

  menu = etk_menu_new();
  menu_item = etk_menu_item_image_new_with_label("<App name here>");
  _od_config_menu_app = menu_item;
  sub_menu = etk_menu_new();
  etk_menu_shell_append(ETK_MENU_SHELL(menu), ETK_MENU_ITEM(menu_item));
  etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(sub_menu));
  etk_widget_show(sub_menu);

  menu_item = etk_menu_item_new_with_label("Edit Icon");
  etk_menu_shell_append(ETK_MENU_SHELL(sub_menu), ETK_MENU_ITEM(menu_item));
//  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(_od_config_menu_app_edit_cb), NULL);
  menu_item = etk_menu_item_new_with_label("Keep Icon");
  etk_menu_shell_append(ETK_MENU_SHELL(sub_menu), ETK_MENU_ITEM(menu_item));
//  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(_od_config_menu_app_keep_cb), NULL);
  _od_config_menu_app_keep = menu_item;

  menu_item = etk_menu_item_new_with_label("Remove Icon");
  etk_menu_shell_append(ETK_MENU_SHELL(sub_menu), ETK_MENU_ITEM(menu_item));
//  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(_od_config_menu_app_remove_cb), NULL);
  _od_config_menu_app_remove = menu_item;

  menu_item = etk_menu_item_image_new_with_label("Zoom Mode");
  sub_menu = etk_menu_new();
  etk_menu_shell_append(ETK_MENU_SHELL(menu), ETK_MENU_ITEM(menu_item));
  etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(sub_menu));
  etk_widget_show(sub_menu);

  menu_item = etk_menu_item_new_with_label("None");
  etk_menu_shell_append(ETK_MENU_SHELL(sub_menu), ETK_MENU_ITEM(menu_item));
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(_od_config_menu_zoom_none_cb), NULL);
  menu_item = etk_menu_item_new_with_label("Zoom Only");
  etk_menu_shell_append(ETK_MENU_SHELL(sub_menu), ETK_MENU_ITEM(menu_item));
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(_od_config_menu_zoom_zoom_cb), NULL);
  menu_item = etk_menu_item_new_with_label("Zoom and Stretch");
  etk_menu_shell_append(ETK_MENU_SHELL(sub_menu), ETK_MENU_ITEM(menu_item));
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(_od_config_menu_zoom_both_cb), NULL);

  menu_item = etk_menu_item_image_new_from_stock(ETK_STOCK_DOCUMENT_PROPERTIES);
  etk_menu_item_label_set(ETK_MENU_ITEM(menu_item), "Configuration");
  etk_menu_shell_append(ETK_MENU_SHELL(menu), ETK_MENU_ITEM(menu_item));
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(_od_config_menu_config_cb), NULL);
  menu_item = etk_menu_item_separator_new();
  etk_menu_shell_append(ETK_MENU_SHELL(menu), ETK_MENU_ITEM(menu_item));

  menu_item = etk_menu_item_image_new_from_stock(ETK_STOCK_SYSTEM_LOG_OUT);
  etk_menu_item_label_set(ETK_MENU_ITEM(menu_item), "Quit");
  etk_menu_shell_append(ETK_MENU_SHELL(menu), ETK_MENU_ITEM(menu_item));
  etk_signal_connect("activated", ETK_OBJECT(menu_item), ETK_CALLBACK(_od_config_menu_quit_cb), NULL);
  etk_widget_show(menu);

  _od_config_menu  = menu;
}

void od_config_menu_draw(Evas_Coord x, Evas_Coord y) {
  Evas_List      *l;
  Evas_Coord      minx, maxx;
  char           *label;

  if (_od_config_menu == NULL)
    od_config_menu_init();

  minx = x - options.size / 2;
  maxx = x + options.size / 2;
  label = "Not over icon";
  etk_widget_hide(_od_config_menu_app);

  l = dock.icons;
  while (l) {
    OD_Icon *icon;
    icon = l->data;
    if (icon->x >= minx && icon->x <= maxx) {
      int len;
      char *full;
      Etk_Image *image;

      label = icon->a->name;
      etk_widget_show(_od_config_menu_app);
      image = NULL;
      if (icon->a->path) {
        image = etk_image_new_from_edje(icon->a->path, "icon");
      }
      etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(_od_config_menu_app), image);
      if (icon->launcher) {
        etk_widget_hide(_od_config_menu_app_keep);
        etk_widget_show(_od_config_menu_app_remove);
      } else {
        etk_widget_show(_od_config_menu_app_keep);
        etk_widget_hide(_od_config_menu_app_remove);
      }
      break;
    }
    l = l->next;
  }

  etk_menu_item_label_set(ETK_MENU_ITEM(_od_config_menu_app), label);
  etk_menu_popup_in_direction(ETK_MENU(_od_config_menu), ETK_POPUP_ABOVE_RIGHT);
}

#endif

