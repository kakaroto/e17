#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "excessive_private.h"

#ifndef ELM_LIB_QUICKLAUNCH

Eina_Bool crazy_option = EINA_FALSE;
int _log_domain = -1;

static const Ecore_Getopt options = {
  PACKAGE_NAME,
  "%prog [options] [directory]",
  PACKAGE_VERSION "Revision:" stringify(VREV),
  "(C) 2011 cedric.bail@free.fr",
  "GPL-3",
  "Files Browser",
  EINA_TRUE,
  {
    ECORE_GETOPT_STORE_BOOL('c', "crazy", "Burn your CPU and GPU as hell."),
    ECORE_GETOPT_VERSION('V', "version"),
    ECORE_GETOPT_COPYRIGHT('C', "copyright"),
    ECORE_GETOPT_LICENSE('L', "license"),
    ECORE_GETOPT_HELP('h', "help"),
    ECORE_GETOPT_SENTINEL
  }
};

EAPI int
elm_main(int argc, char **argv)
{
   Evas_Object *window;
   Evas_Object *layout;
   Evas_Object *edje;
   Evas_Object *grid;
   Evas_Object *list;
   char path[PATH_MAX];
   Eina_Bool quit_option = EINA_FALSE;
   int args;

   Ecore_Getopt_Value values[] = {
     ECORE_GETOPT_VALUE_BOOL(crazy_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_NONE
   };

   _log_domain = eina_log_domain_register("Excessive", NULL);
   if (_log_domain < 0)
     {
        EINA_LOG_CRIT("could not create log domain 'Excessive'.");
        return -1;
     }

   args = ecore_getopt_parse(&options, values, argc, argv);
   if (args < 0)
     {
        ERR("could not parse command line options.");
        return -1;
     }

   if (quit_option) return 0;

   excessive_browse_init();

   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
   elm_theme_extension_add(NULL, PACKAGE_DATA_DIR "/themes/theme.edj");
   elm_theme_overlay_add(NULL, PACKAGE_DATA_DIR "/themes/theme.edj");
   elm_need_efreet();
   elm_need_ethumb();

   window = elm_win_add(NULL, PACKAGE_NAME, ELM_WIN_BASIC);
   if (!window)
     {
        ERR("could'nt create window.");
        return -1;
     }

   /* FIXME: later remember last size */
   elm_win_alpha_set(window, 1);
   evas_object_resize(window, 800, 600);
   elm_win_title_set(window, PACKAGE_STRING);
   elm_win_autodel_set(window, 1);

   layout = elm_layout_add(window);
   if (!layout)
     {
        ERR("couldn't create layout object.");
        return -1;
     }

   evas_object_size_hint_align_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_weight_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(window, layout);

   if (!elm_layout_theme_set(layout, "layout", "application", "content"))
     {
        ERR("could not load style 'content' from theme");
        return -1;
     }

   evas_object_show(layout);
   evas_object_show(window);

   edje = elm_layout_edje_get(layout);
   grid = edje_object_part_external_object_get(edje, "grid");

   evas_object_data_set(grid, "excessive/layout", layout);
   evas_object_data_set(layout, "excessive/win", window);

   evas_object_size_hint_align_set(grid, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(grid, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_gen_bounce_set(grid, EINA_FALSE, EINA_TRUE);
   elm_gengrid_align_set(grid, 0.5, 0.5);
   elm_gengrid_item_size_set(grid, 128, 128);

   excessive_browse_load(layout);

   list = edje_object_part_external_object_get(edje, "shortcut");

   excessive_shortcut_init(list, grid);

   if (argc < args + 1)
     {
       getcwd(path, PATH_MAX);

       excessive_browse_directory(grid, path);
     }
   else
     {
       excessive_browse_directory(grid, argv[args]);
     }

   elm_run();

   excessive_shortcut_shutdown();
   excessive_browse_shutdown();

   return 0;
}
#endif
ELM_MAIN();

