#include "engage.h"
#include "config.h"
#ifdef DMALLOC
#include "dmalloc.h"
#endif

#ifdef HAVE_EWL
#include <Ewl.h>
Ewl_Widget     *menu, *menu_win;
Evas_Object    *embed;
int             init;

void            od_config_menu_hide(void);
#endif

OD_Options      options;

/* listeners */

int
zoom_listener(const char *key, const Ecore_Config_Type type, const int tag,
              void *data)
{
  options.zoom = ecore_config_int_get(key);
}

theme_listener(const char *key, const Ecore_Config_Type type, const int tag,
                  void *data)    
{ 
  char           *path;
  Evas_List      *icons;
  options.theme = ecore_config_theme_get(key);

  path = ecore_config_theme_with_path_get(key);
  
  icons = dock.icons;
  while (icons) {
    od_icon_reload((OD_Icon *)icons->data);
    icons = evas_list_next(icons);
  }
} 

int
od_config_init(void)
{
  int             ret;

  ecore_config_int_create("engage.options.width", 1024, 'W', "width",
                          "The overall width of the application area");
  ecore_config_int_create("engage.options.height", 100, 'H', "height",
                          "The overall height of the application area");
  ecore_config_theme_create("engage.options.theme", "gentoo", 't', "theme",
                            "The theme name to use (minus path and extension)");
  /* not technically correct - iconsets should do this, but it looks better for
   * everything bar 'gentoo' - and we all have the others installed ;) */
  ecore_config_theme_preview_group_set("engage.options.theme", "Terminal");
  ecore_config_string_create("engage.options.engine", "software", 'e', "engine",
                             "The X11 engine to use - either software or gl");
  options.icon_path = PACKAGE_DATA_DIR "/icons/";
  ecore_config_int_create_bound("engage.options.mode", OM_BELOW, 0, 1, 1, 'm',
                                "mode",
                                "The display mode, 0 = ontop + shaped, 1 = below + transp");
  ecore_config_int_create_bound("engage.options.grab_min_icons", 1, 0, 1, 1,
                                'g', "grab-min",
                                "Capture the icons of minimised applications");
  ecore_config_int_create_bound("engage.options.grab_app_icons", 1, 0, 1, 1,
                                'G', "grab-app",
                                "Capture the icons of all running applications");

  ecore_config_int_create("engage.options.size", 32, 's', "size",
                          "Size of icons in default state");
  ecore_config_int_create("engage.options.spacing", 4, 'S', "spacing",
                          "Space in pixels between each icon");
  ecore_config_int_create_bound("engage.options.zoom", 1, 0, 1, 1, 'z', "zoom",
                          "Should we zoom icons?");
  ecore_config_float_create("engage.options.zoom_factor", 2.0, 'Z',
                            "zoom-factor",
                            "Zoom factor of the icons - 1.0 == 100% == nozoom");
  ecore_config_int_create("engage.options.arrow_size", 6, 'a', "arrow-size",
                          "Size (in pixels) of the status arrows");
  ecore_config_float_create("engage.options.zoom_duration", 0.1, 'd',
                            "zoom-time",
                            "Time taken (in seconds) for icons to zoom");

  options.tt_txt_color = 0x00000000;
  options.tt_shd_color = 0x7f000000;
  options.bg_fore = 0x7f000000;
  options.bg_back = 0x7fffffff;
  ecore_config_string_create("engage.options.tt_fa", "Vera", 'f', "font",
                             "The font to use for application titles etc.");
  ecore_config_int_create("engage.options.tt_fs", 8, 'F', "font-size",
                          "The font size (in points)");
  ecore_config_float_create("engage.options.icon_appear_duration", 0.1, 'D',
                            "appear-time",
                            "Time taken (in seconds) for new icons to appear");

  ecore_config_load();
  ret = ecore_config_args_parse();

  options.width = ecore_config_int_get("engage.options.width");
  options.height = ecore_config_int_get("engage.options.height");
  options.engine = ecore_config_string_get("engage.options.engine");
  options.theme = ecore_config_theme_get("engage.options.theme");
  ecore_config_listen("theme", "engage.options.theme", theme_listener, 0, NULL);
  options.mode = ecore_config_int_get("engage.options.mode");
  options.grab_min_icons =
    ecore_config_int_get("engage.options.grab_min_icons");
  options.grab_app_icons =
    ecore_config_int_get("engage.options.grab_app_icons");

  options.size = ecore_config_int_get("engage.options.size");
  options.spacing = ecore_config_int_get("engage.options.spacing");
  options.zoom = ecore_config_int_get("engage.options.zoom");
  ecore_config_listen("zoom", "engage.options.zoom", zoom_listener, 0, NULL);
  options.zoomfactor = ecore_config_float_get("engage.options.zoom_factor");
  options.arrow_size = ecore_config_int_get("engage.options.arrow_size");
  options.dock_zoom_duration =
    ecore_config_float_get("engage.options.zoom_duration");

  options.tt_fa = ecore_config_string_get("engage.options.tt_fa");
  options.tt_fs = ecore_config_int_get("engage.options.tt_fs");
  options.icon_appear_duration =
    ecore_config_float_get("engage.options.icon_appear_duration");

  return ret;
}

#ifdef HAVE_EWL
int             menu_height, menu_width;
Evas_Coord      menu_x, menu_y;

void
od_config_menu_move_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
  ewl_object_request_position(EWL_OBJECT(user_data), CURRENT_X(menu_win),
                              CURRENT_Y(menu_win));
}

void
od_config_menu_out_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
  int x,y;
  evas_pointer_output_xy_get(evas, &x, &y);
  if (x < menu_x || x > (menu_x + menu_width) ||
      y < menu_y || y > (menu_y + menu_height))
    od_config_menu_hide();
}

void
od_config_menu_zoom_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
  ecore_config_int_set("engage.options.zoom", options.zoom ? 0 : 1);
  od_config_menu_hide();
}

void
od_config_menu_quit_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
  od_config_menu_hide();
  ecore_main_loop_quit();
}

void
od_config_menu_init(void)
{
  Ewl_Widget     *menu_box, *item;

  if (init)
    return;
  init = 1;
  /* FIXME: nasty nasty, we need to request the size, not code it in */
  menu_height = 60;
  menu_width = 100;

  menu_win = ewl_embed_new();
  ewl_object_set_fill_policy((Ewl_Object *) menu_win, EWL_FLAG_FILL_FILL);

  embed =
    ewl_embed_set_evas(EWL_EMBED(menu_win), evas,
                       ecore_evas_software_x11_window_get(ee));
  evas_object_layer_set(embed, 999);

  /* FIXME: this should not be needed */
  evas_object_resize(embed, menu_width, menu_height);
  evas_object_show(embed);
  ewl_widget_show(menu_win);

  menu_box = ewl_vbox_new();
  ewl_object_set_fill_policy(EWL_OBJECT(menu_box), EWL_FLAG_FILL_FILL);
  ewl_container_append_child(EWL_CONTAINER(menu_win), menu_box);
  ewl_widget_show(menu_box);

  menu = ewl_imenu_new(NULL, "config menu");
  ewl_callback_append(EWL_WIDGET(menu_win), EWL_CALLBACK_CONFIGURE,
                      od_config_menu_move_cb, EWL_MENU_BASE(menu)->popup);
  ewl_callback_append(EWL_MENU_BASE(menu)->popup, EWL_CALLBACK_FOCUS_OUT,
                      od_config_menu_out_cb, NULL);
  ewl_container_append_child(EWL_CONTAINER(menu_box), menu);
  /* *don't* show the menu, we only want the popup */

  item = ewl_menu_item_new(NULL, "Icon Zooming");
  ewl_container_append_child(EWL_CONTAINER(menu), item);
  ewl_callback_append(item, EWL_CALLBACK_SELECT, od_config_menu_zoom_cb, NULL);
  ewl_widget_show(item);

  item = ewl_menu_separator_new();
  ewl_container_append_child(EWL_CONTAINER(menu), item);
  ewl_widget_show(item);

  item = ewl_menu_item_new(NULL, "Quit");
  ewl_container_append_child(EWL_CONTAINER(menu), item);
  ewl_callback_append(item, EWL_CALLBACK_SELECT, od_config_menu_quit_cb, NULL);
  ewl_widget_show(item);
}

void
od_config_menu_hide(void)
{
  evas_object_move(embed, -1 * CURRENT_W(menu_win), 0);
  ewl_widget_hide(menu);

}

void
od_config_menu_draw(Evas_Coord x, Evas_Coord y)
{
  int             menu_off_x, menu_off_y;

  menu_off_x = 5;
  menu_off_y = 5;

  od_config_menu_init();
  if (x - menu_off_x + menu_width > options.width)
    menu_x = options.width - menu_width;
  else if (x < menu_off_x)
    menu_x = 0;
  else
    menu_x = x - menu_off_x;
  if (y - menu_off_y + menu_height > options.height)
    menu_y = options.height - menu_height;
  else if (y < menu_off_y)
    menu_y = 0;
  else
    menu_y = y - menu_off_y;
  evas_object_move(embed, menu_x, menu_y);
  ewl_callback_call(menu, EWL_CALLBACK_SELECT);
}

#endif
