#include "engage.h"
#include "config.h"
#ifdef DMALLOC
#include "dmalloc.h"
#endif

OD_Options      options;

void
od_config_init()
{
  ecore_config_default_int("engage.options.width", 1024);
  ecore_config_default_int("engage.options.height", 100);
  ecore_config_default_string("engage.options.theme", "gentoo");
  ecore_config_default_string("engage.options.engine", "software");
  options.icon_path = PACKAGE_DATA_DIR "/icons/";
  ecore_config_default_int_bound("engage.options.mode", OM_BELOW, 0, 1, 1);
  ecore_config_default_int_bound("engage.options.grab_min_icons", 1, 0, 1, 1);
  ecore_config_default_int_bound("engage.options.grab_app_icons", 1, 0, 1, 1);

  ecore_config_default_int("engage.options.size", 32);
  ecore_config_default_int("engage.options.spacing", 4);
  ecore_config_default_float("engage.options.zoom_factor", 2.0);
  ecore_config_default_int("engage.options.arrow_size", 6);
  ecore_config_default_float("engage.options.zoom_duration", 0.1);

  options.tt_txt_color = 0x00000000;
  options.tt_shd_color = 0x7f000000;
  options.bg_fore = 0x00000000;
  options.bg_back = 0x00000000;
  ecore_config_default_string("engage.options.tt_fa", "Vera");
  ecore_config_default_int("engage.options.tt_fs", 8);
  ecore_config_default_float("engage.options.icon_appear_duration", 0.1);

  ecore_config_load();
  options.width = ecore_config_get_int("engage.options.width");
  options.height = ecore_config_get_int("engage.options.height");
  options.engine = ecore_config_get_string("engage.options.engine");
  options.theme = ecore_config_get_string("engage.options.theme");
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

}
