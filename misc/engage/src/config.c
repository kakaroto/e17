#include "engage.h"
#include "config.h"
#ifdef DMALLOC
#include "dmalloc.h"
#endif

#ifdef HAVE_EWL
#include <Ewl.h>
Ewl_Widget     *menu, *menu_win, *menu_app_name;
Evas_Object    *embed;
int             init;

void            od_config_menu_hide(void);
#endif

typedef enum od_config_colors {
  TT_TXT,
  TT_SHD,
  BG_FORE,
  BG_BACK
} od_config_colors;

OD_Options      options;

/* listeners */

int
zoom_listener(const char *key, const Ecore_Config_Type type, const int tag,
              void *data)
{
  options.zoom = ecore_config_boolean_get(key);
  return 1;
}

int
theme_listener(const char *key, const Ecore_Config_Type type, const int tag,
               void *data)
{
  char           *path;
  Evas_List      *icons;

  if (options.theme)
    free(options.theme);
  options.theme = ecore_config_theme_get(key);

  path = ecore_config_theme_with_path_get(key);

  icons = dock.icons;
  while (icons) {
    od_icon_reload((OD_Icon *) icons->data);
    icons = evas_list_next(icons);
  }
  return 1;
}

int
colour_listener(const char *key, const Ecore_Config_Type type, const int tag, 
                void *data)
{
  long colour;

  colour = ecore_config_argbint_get(key);

  switch (tag) {
    case BG_FORE:
      options.bg_fore = colour;
      break;
    case BG_BACK:
      options.bg_back = colour;
      break;
  }
  return 1;
}

int
od_config_init(void)
{
  int             ret;

  ecore_config_int_create("engage.options.width", 1024, 'W', "width",
                          "The overall width of the application area");
  ecore_config_int_create("engage.options.height", 130, 'H', "height",
                          "The overall height of the application area");
  ecore_config_theme_create("engage.options.theme", "none", 't', "theme",
                            "The theme name to use (minus path and extension)");
  /* not technically correct - iconsets should do this, but it looks better for
   * everything bar 'gentoo' - and we all have the others installed ;) */
  ecore_config_theme_preview_group_set("engage.options.theme", "Terminal");
  ecore_config_theme_search_path_append(PACKAGE_DATA_DIR "/themes/");
  ecore_config_string_create("engage.options.engine", "software", 'e', "engine",
                             "The X11 engine to use - either software or gl");
  options.icon_path = PACKAGE_DATA_DIR "/icons/";
  ecore_config_int_create_bound("engage.options.mode", OM_BELOW, 0, 1, 1, 'm',
                                "mode",
                                "The display mode, 0 = ontop + shaped, 1 = below + transp");

  ecore_config_int_create("engage.options.reserve", 52, 'R', "reserve",
                          "The amount of space reserved at the bottom of the screen");
#ifdef XINERAMA
  ecore_config_int_create("engage.options.head", 0, 'X', "head",
                          "Which Xinerama head to display the docker on");
#endif
  
  ecore_config_boolean_create("engage.options.grab_min_icons", 0, 'g',
                              "grab-min",
                              "Capture the icons of minimised applications");
  ecore_config_boolean_create("engage.options.grab_app_icons", 0, 'G',
                               "grab-app",
                               "Capture the icons of all running applications");
  ecore_config_boolean_create("engage.options.auto_hide", 0, 'A', "auto-hide",
                              "Auto hide the engage bar");

  ecore_config_int_create("engage.options.size", 37, 's', "size",
                          "Size of icons in default state");
  ecore_config_int_create("engage.options.spacing", 4, 'S', "spacing",
                          "Space in pixels between each icon");
  ecore_config_boolean_create("engage.options.zoom", 1, 'z', "zoom",
                                "Should we zoom icons?");
  ecore_config_float_create("engage.options.zoom_factor", 2.0, 'Z',
                            "zoom-factor",
                            "Zoom factor of the icons - 1.0 == 100% == nozoom");
  ecore_config_float_create("engage.options.zoom_duration", 0.2, 'd',
                            "zoom-time",
                            "Time taken (in seconds) for icons to zoom");

  ecore_config_argb_create("engage.options.bg_fore", "#7f000000", 'B', "bg-outline-color", "Background outline color");
  ecore_config_argb_create("engage.options.bg_back", "#7fffffff", 'b', "bg-main-color", "Background main color");

  ecore_config_float_create("engage.options.icon_appear_duration", 0.1, 'D',
                            "appear-time",
                            "Time taken (in seconds) for new icons to appear");

  ecore_config_boolean_create("engage.options.tray", 1, 'T', "tray",
                              "Enable system tray");
  ecore_config_boolean_create("engage.options.ignore_running", 0, 'i',
                              "ignore-running", "Ignore running apps");
  ecore_config_boolean_create("engage.options.ignore_iconified", 0, 'I',
                              "ignore-iconified", "Ignore iconified windows");

  ecore_config_load();
  ret = ecore_config_args_parse();

  options.width = ecore_config_int_get("engage.options.width");
  options.height = ecore_config_int_get("engage.options.height");
  options.engine = ecore_config_string_get("engage.options.engine");
  options.theme = ecore_config_theme_get("engage.options.theme");
  ecore_config_listen("theme", "engage.options.theme", theme_listener, 0, NULL);
  options.mode = ecore_config_int_get("engage.options.mode");

  options.reserve = ecore_config_int_get("engage.options.reserve");
#ifdef XINERAMA
  options.head = ecore_config_int_get("engage.options.head");
#endif
  
  options.grab_min_icons =
    ecore_config_boolean_get("engage.options.grab_min_icons");
  options.grab_app_icons =
    ecore_config_boolean_get("engage.options.grab_app_icons");
  options.auto_hide = ecore_config_boolean_get("engage.options.auto_hide");
  
  options.size = ecore_config_int_get("engage.options.size");
  options.spacing = ecore_config_int_get("engage.options.spacing");
  options.zoom = ecore_config_boolean_get("engage.options.zoom");
  ecore_config_listen("zoom", "engage.options.zoom", zoom_listener, 0, NULL);
  options.zoomfactor = ecore_config_float_get("engage.options.zoom_factor");
  options.dock_zoom_duration =
    ecore_config_float_get("engage.options.zoom_duration");

  options.bg_fore = ecore_config_argbint_get("engage.options.bg_fore");
  ecore_config_listen("colour", "engage.options.bg_fore", 
                      colour_listener, BG_FORE, NULL);
  options.bg_back = ecore_config_argbint_get("engage.options.bg_back");
  ecore_config_listen("colour", "engage.options.bg_back", 
                      colour_listener, BG_BACK, NULL);

  options.icon_appear_duration =
    ecore_config_float_get("engage.options.icon_appear_duration");

  options.tray = ecore_config_boolean_get("engage.options.tray");
  options.ignore_run = ecore_config_boolean_get("engage.options.ignore_running");
  options.ignore_min = ecore_config_boolean_get("engage.options.ignore_iconified");

  return ret;
}

#ifdef HAVE_EWL
int             menu_height, menu_width;
Evas_Coord      menu_x, menu_y;

void
od_config_menu_move_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
  ewl_object_position_request(EWL_OBJECT(user_data), CURRENT_X(menu_win),
                              CURRENT_Y(menu_win));
}

void
od_config_menu_out_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
  int             x, y;
  evas_pointer_output_xy_get(evas, &x, &y);
  if (x <= menu_x + 1 || x >= (menu_x + menu_width) ||
      y <= menu_y + 1 || y >= (menu_y + menu_height))
    od_config_menu_hide();
}

void
od_config_menu_zoom_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
  ecore_config_int_set("engage.options.zoom", options.zoom ? 0 : 1);
  od_config_menu_hide();
}

void
od_config_menu_config_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
  if (!ecore_exe_run("examine engage", NULL))
    fprintf(stderr, "'examine' could not be launched - is it in your path?\n");
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
  menu_height = 87; //121;
  menu_width = 100;

  menu_win = ewl_embed_new();
  ewl_object_fill_policy_set((Ewl_Object *) menu_win, EWL_FLAG_FILL_FILL);

  embed =
    ewl_embed_evas_set(EWL_EMBED(menu_win), evas,
                       EWL_EMBED_EVAS_WINDOW(ecore_evas_software_x11_window_get(ee)));
  evas_object_layer_set(embed, 999);

  /* FIXME: this should not be needed */
  evas_object_resize(embed, menu_width, menu_height);
  evas_object_show(embed);
  ewl_widget_show(menu_win);

  menu_box = ewl_vbox_new();
  ewl_object_fill_policy_set(EWL_OBJECT(menu_box), EWL_FLAG_FILL_FILL);
  ewl_container_child_append(EWL_CONTAINER(menu_win), menu_box);
  ewl_widget_show(menu_box);

  menu = ewl_imenu_new();
  ewl_button_label_set(EWL_BUTTON(menu), "config menu");
  ewl_callback_append(EWL_WIDGET(menu_win), EWL_CALLBACK_CONFIGURE,
                      od_config_menu_move_cb, EWL_MENU_BASE(menu)->popup);
  ewl_callback_append(EWL_MENU_BASE(menu)->popup, EWL_CALLBACK_FOCUS_OUT,
                      od_config_menu_out_cb, NULL);
  ewl_container_child_append(EWL_CONTAINER(menu_box), menu);
  /* *don't* show the menu, we only want the popup */
/*
  item = ewl_menu_item_new(NULL, "<name goes here>");
  ewl_container_child_append(EWL_CONTAINER(menu), item);
  ewl_callback_append(item, EWL_CALLBACK_FOCUS_IN, od_config_menu_config_cb, NULL);
  ewl_widget_show(item);
  menu_app_name = item;

  item = ewl_menu_separator_new();
  ewl_container_child_append(EWL_CONTAINER(menu), item);
  ewl_widget_show(item);
*/


  item = ewl_menu_item_new();
  ewl_button_label_set(EWL_BUTTON(item), "Icon Zooming");
  ewl_container_child_append(EWL_CONTAINER(menu), item);
  ewl_callback_append(item, EWL_CALLBACK_FOCUS_IN, od_config_menu_zoom_cb, NULL);
  ewl_widget_show(item);

  item = ewl_menu_item_new();
  ewl_button_label_set(EWL_BUTTON(item), "Configuration");
  ewl_container_child_append(EWL_CONTAINER(menu), item);
  ewl_callback_append(item, EWL_CALLBACK_FOCUS_IN, od_config_menu_config_cb, NULL);
  ewl_widget_show(item);

  item = ewl_separator_new();
  ewl_container_child_append(EWL_CONTAINER(menu), item);
  ewl_widget_show(item);

  item = ewl_menu_item_new();
  ewl_button_label_set(EWL_BUTTON(item), "Quit");
  ewl_container_child_append(EWL_CONTAINER(menu), item);
  ewl_callback_append(item, EWL_CALLBACK_FOCUS_IN, od_config_menu_quit_cb, NULL);
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
  Evas_Coord      menu_off_x, menu_off_y;
  Evas_List      *l;
  Evas_Coord      minx, maxx;
  char           *menu_text_launcher, *menu_text_not_launcher;

  menu_off_x = 5;
  menu_off_y = 5;

  od_config_menu_init();
  
  minx = x - options.size / 2 ;
  maxx = x + options.size / 2;

  menu_text_launcher = "don't keep \"%s\" in docker";
  menu_text_not_launcher = "keep \"%s\" in docker";
  l = dock.icons;
  while (l) {
    OD_Icon *icon;
    icon = l->data;
    if (icon->x >= minx && icon->x <= maxx) {
      int len;
      char *full;
      if (icon->launcher) {
        len = strlen(menu_text_launcher) + strlen(icon->name) - 1;
	full = malloc(len);
        snprintf(full, len, menu_text_launcher, icon->name);
      } else {
        len = strlen(menu_text_not_launcher) + strlen(icon->name) - 1;
	full = malloc(len);
        snprintf(full, len, menu_text_not_launcher, icon->name);
      }
      printf("context menu on %s\n", icon->name);
//      ewl_button_label_set(EWL_BUTTON(menu_app_name), full);
      free(full);
      break;
    }
    l = l->next;
  }

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
  ewl_callback_call(menu, EWL_CALLBACK_FOCUS_IN);
}

#endif
