#include "engage.h"
#include "config.h"
#ifdef DMALLOC
#include "dmalloc.h"
#endif

OD_Options      options;

int
od_config_init(int argc, char **argv)
{
  int             ret;

  ecore_config_create_int("engage.options.width", 1024, 'W', "width",
                          "The overall width of the application area");
  ecore_config_create_int("engage.options.height", 100, 'H', "height",
                          "The overall height of the application area");
  ecore_config_create_theme("engage.options.theme", "gentoo", 't', "theme",
                            "The theme name to use (minus path and extension)");
  /* not technically correct - iconsets should do this, but it looks better for
   * everything bar 'gentoo' - and we all have the others installed ;) */
  ecore_config_set_theme_preview_group("engage.options.theme", "Terminal");
  ecore_config_create_string("engage.options.engine", "software", 'e', "engine",
                             "The X11 engine to use - either software or gl");
  options.icon_path = PACKAGE_DATA_DIR "/icons/";
  ecore_config_create_int_bound("engage.options.mode", OM_BELOW, 0, 1, 1, 'm',
                                "mode",
                                "The display mode, 0 = ontop + shaped, 1 = below + transp");
  ecore_config_create_int_bound("engage.options.grab_min_icons", 1, 0, 1, 1,
                                'g', "grab-min",
                                "Capture the icons of minimised applications");
  ecore_config_create_int_bound("engage.options.grab_app_icons", 1, 0, 1, 1,
                                'G', "grab-app",
                                "Capture the icons of all running applications");

  ecore_config_create_int("engage.options.size", 32, 's', "size",
                          "Size of icons in default state");
  ecore_config_create_int("engage.options.spacing", 4, 'S', "spacing",
                          "Space in pixels between each icon");
  ecore_config_create_float("engage.options.zoom_factor", 2.0, 'z', "zoom",
                            "Zoom factor of the icons - 1.0 == 100% == nozoom");
  ecore_config_create_int("engage.options.arrow_size", 6, 'a', "arrow-size",
                          "Size (in pixels) of the status arrows");
  ecore_config_create_float("engage.options.zoom_duration", 0.1, 'd',
                            "zoom-time",
                            "Time taken (in seconds) for icons to zoom");

  options.tt_txt_color = 0x00000000;
  options.tt_shd_color = 0x7f000000;
  options.bg_fore = 0x7f000000;
  options.bg_back = 0x7fffffff;
  ecore_config_create_string("engage.options.tt_fa", "Vera", 'f', "font",
                             "The font to use for application titles etc.");
  ecore_config_create_int("engage.options.tt_fs", 8, 'F', "font-size",
                          "The font size (in points)");
  ecore_config_create_float("engage.options.icon_appear_duration", 0.1, 'D',
                            "appear-time",
                            "Time taken (in seconds) for new icons to appear");

  ecore_config_load();
  ret = ecore_config_args_parse(argc, argv);

  options.width = ecore_config_get_int("engage.options.width");
  options.height = ecore_config_get_int("engage.options.height");
  options.engine = ecore_config_get_string("engage.options.engine");
  options.theme = ecore_config_get_theme("engage.options.theme");
  options.mode = ecore_config_get_int("engage.options.mode");
  options.grab_min_icons =
    ecore_config_get_int("engage.options.grab_min_icons");
  options.grab_app_icons =
    ecore_config_get_int("engage.options.grab_app_icons");

  options.size = ecore_config_get_int("engage.options.size");
  options.spacing = ecore_config_get_int("engage.options.spacing");
  options.zoomfactor = ecore_config_get_float("engage.options.zoom_factor");
  options.arrow_size = ecore_config_get_int("engage.options.arrow_size");
  options.dock_zoom_duration =
    ecore_config_get_float("engage.options.zoom_duration");

  options.tt_fa = ecore_config_get_string("engage.options.tt_fa");
  options.tt_fs = ecore_config_get_int("engage.options.tt_fs");
  options.icon_appear_duration =
    ecore_config_get_float("engage.options.icon_appear_duration");

  return ret;
}
