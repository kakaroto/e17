#include "engage.h"
#include "Ecore_X.h"
#ifdef HAVE_ETK
#include <Etk.h>
#endif
#ifdef DMALLOC
#include "dmalloc.h"
#endif

static int      handle_idle(void *data);
bool            need_redraw = false;

static int
exit_cb(void *data, int type, void *event)
{
  ecore_main_loop_quit();
  return (0);
}

int
main(int argc, char **argv)
{
  if (ecore_init() == 0) {
    exit(0);
  }

  if (ecore_file_init() == 0) {
    ecore_shutdown();
    exit(0);
  }

  if (ecore_desktop_init() == 0) {
    ecore_file_shutdown();
    ecore_shutdown();
    exit(0);
  }
  ecore_desktop_paths_extras_clear();
  ecore_desktop_paths_prepend_user(ECORE_DESKTOP_PATHS_ICONS, "~/.e/e/icons");
  ecore_desktop_paths_regen();
  
  if (ecore_x_init(NULL) == 0) {
    ecore_desktop_shutdown();
    ecore_file_shutdown();
    ecore_shutdown();
    exit(0);
  }

  if((ecore_config_init("engage")) == ECORE_CONFIG_ERR_FAIL) {
     ecore_x_shutdown();
     ecore_desktop_shutdown();
     ecore_file_shutdown();
     ecore_shutdown();
     exit(0);
  }

  ecore_app_args_set(argc, (const char **) argv);
  if (od_config_init() != ECORE_CONFIG_PARSE_CONTINUE) {
    ecore_config_shutdown();
    ecore_x_shutdown();
    ecore_desktop_shutdown();
    ecore_file_shutdown();
    ecore_shutdown();
    exit(0);
  }

  e_intl_init();
  e_app_init();
  e_app_cache_init();

  ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, exit_cb, NULL);
  ecore_evas_init();
#ifdef HAVE_ETK
  etk_init(&argc, &argv);
#endif
  edje_init();
  edje_frametime_set(1.0 / 30.0);

  od_window_init();
  od_dock_init();

  if (options.tray)
    od_tray_init();

  od_icon_add_path(options.icon_path);
  ecore_config_evas_font_path_apply(evas);
  userconfig_load();

  od_icon_add_path(options.icon_path);

  od_dock_icons_update_begin();
  ecore_idle_enterer_add(handle_idle, NULL);
  ecore_main_loop_begin();

  edje_shutdown();
#ifdef HAVE_ETK
  etk_shutdown();
#endif
  ecore_evas_shutdown();
  e_app_cache_shutdown();
  e_app_shutdown();
  e_intl_shutdown();
  ecore_config_save();
  ecore_config_shutdown();
  ecore_x_shutdown();
  ecore_desktop_shutdown();
  ecore_file_shutdown();
  ecore_shutdown();

  return 0;
}

int
handle_idle(void *data)
{
  if (need_redraw) {
    od_dock_redraw(NULL);
    evas_render(evas);
    need_redraw = false;
  }
  return 0;
}
