#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <Elementary.h>
#ifndef ELM_LIB_QUICKLAUNCH

#include "private.h"

#include <Ecore_Getopt.h>
#include <Ecore_File.h>
#include <stdlib.h>
#include <string.h>
#include "gettext.h"

int _log_domain = -1;
static App app;

static const Ecore_Getopt options = {
  PACKAGE_NAME,
  "%prog [options]",
  PACKAGE_VERSION "Revision:" stringify(VREV),
  "(C) 2010-2011 ProFUSION embedded systems",
  "LGPL-3",
  "Music player for mobiles and desktops.",
  EINA_TRUE,
  {
    ECORE_GETOPT_APPEND_METAVAR
    ('a', "add", "Add (recursively) directory to music library.",
     "DIRECTORY", ECORE_GETOPT_TYPE_STR),
    ECORE_GETOPT_APPEND_METAVAR
    ('d', "del", "Delete (recursively) directory from music library.",
     "DIRECTORY", ECORE_GETOPT_TYPE_STR),
    ECORE_GETOPT_VERSION('V', "version"),
    ECORE_GETOPT_COPYRIGHT('C', "copyright"),
    ECORE_GETOPT_LICENSE('L', "license"),
    ECORE_GETOPT_HELP('h', "help"),
    ECORE_GETOPT_SENTINEL
  }
};

char *
enjoy_cache_dir_get(void)
{
   static char *cache = NULL;

   if (!cache)
     {
        cache = getenv("XDG_CACHE_HOME");
        if (!cache || !*cache)
          {
             char *home = getenv("HOME");
             if (!home || !*home)
               {
                  ERR("could not get $HOME");
                  return NULL;
               }
             if (asprintf(&cache, "%s/.cache/%s", home, PACKAGE) < 0)
               {
                  ERR("could not set cache directory");
                  return NULL;
               }
          }
        else
          {
             char *tmpcache;
             if (asprintf(&tmpcache, "%s/%s", cache, PACKAGE) < 0)
               {
                  ERR("could not set cache directory");
                  return NULL;
               }
             cache = tmpcache;
          }
        if (!ecore_file_exists(cache))
          {
             if (!ecore_file_mkpath(cache))
               {
                  ERR("could not create cache dir: %s", cache);
                  return NULL;
               }
          }
     }
   return cache;
}

void
no_free()
{
}

EAPI int ENJOY_EVENT_PLAYER_CAPS_CHANGE = -1;
EAPI int ENJOY_EVENT_PLAYER_STATUS_CHANGE = -1;
EAPI int ENJOY_EVENT_PLAYER_TRACK_CHANGE = -1;
EAPI int ENJOY_EVENT_TRACKLIST_TRACKLIST_CHANGE = -1;

static void
enjoy_event_id_init(void)
{
   ENJOY_EVENT_PLAYER_CAPS_CHANGE = ecore_event_type_new();
   ENJOY_EVENT_PLAYER_STATUS_CHANGE = ecore_event_type_new();
   ENJOY_EVENT_PLAYER_TRACK_CHANGE = ecore_event_type_new();
   ENJOY_EVENT_TRACKLIST_TRACKLIST_CHANGE = ecore_event_type_new();
}

static void
enjoy_module_load(void)
{
   char *path;

   DBG("Loading modules from '%s'", PACKAGE_LIB_DIR "/enjoy/");
   app.modules = eina_module_list_get
     (NULL, PACKAGE_LIB_DIR "/enjoy/", 0, NULL, NULL);

   path = eina_module_environment_path_get("HOME", "/.enjoy/");
   if (path)
     {
        DBG("Loading modules from '%s'", path);
        app.modules = eina_module_list_get(app.modules, path, 0, NULL, NULL);
        free(path);
     }

   path = eina_module_environment_path_get("ENJOY_LIB_DIR", "/enjoy/");
   if (path)
     {
        DBG("Loading modules from '%s'", path);
        app.modules = eina_module_list_get(app.modules, path, 0, NULL, NULL);
        free(path);
     }

   path = eina_module_environment_path_get("ENJOY_MODULES_DIR", NULL);
   if (path)
     {
        DBG("Loading modules from '%s'", path);
        app.modules = eina_module_list_get(app.modules, path, 0, NULL, NULL);
        free(path);
     }

   if (!app.modules)
     {
        INF("No module found!");
        return;
     }

   eina_module_list_load(app.modules);
}

static void
enjoy_module_unload(void)
{
   while (eina_array_count_get(app.modules))
      eina_module_unload(eina_array_pop(app.modules));
   eina_array_free(app.modules);
   app.modules = NULL;
}

EAPI int
elm_main(int argc, char **argv)
{
   int r = 0, args;
   Eina_Bool quit_option = EINA_FALSE;
   const char *home;
   char *s;

   Ecore_Getopt_Value values[] = {
      ECORE_GETOPT_VALUE_LIST(app.add_dirs),
      ECORE_GETOPT_VALUE_LIST(app.del_dirs),
      ECORE_GETOPT_VALUE_BOOL(quit_option),
      ECORE_GETOPT_VALUE_BOOL(quit_option),
      ECORE_GETOPT_VALUE_BOOL(quit_option),
      ECORE_GETOPT_VALUE_BOOL(quit_option),
      ECORE_GETOPT_VALUE_NONE
   };

#if ENABLE_NLS
   setlocale(LC_ALL, "");
   bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
   bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
   textdomain(GETTEXT_PACKAGE);
#endif

   _log_domain = eina_log_domain_register("enjoy", NULL);
   if (_log_domain < 0)
     {
        EINA_LOG_CRIT("could not create log domain 'enjoy'.");
        return -1;
     }

   args = ecore_getopt_parse(&options, values, argc, argv);
   if (args < 0)
     {
        ERR("Could not parse command line options.");
        return -1;
     }

   if (quit_option)
     {
        DBG("Command lines option requires quit.");
        return 0;
     }

   elm_theme_extension_add(NULL, PACKAGE_DATA_DIR "/default.edj");
   elm_theme_overlay_add(NULL, PACKAGE_DATA_DIR "/default.edj");

   home = getenv("HOME");
   if (!home || !home[0])
     {
        CRITICAL("Could not get $HOME");
        r = -1;
        goto end;
     }

   snprintf(app.configdir, sizeof(app.configdir), "%s/.config/enjoy", home);
   if (!ecore_file_mkpath(app.configdir))
     {
        ERR("Could not create %s", app.configdir);
        r = -1;
        goto end;
     }

   app.win = win_new(&app);
   if (!app.win) goto end;

   cover_init();
   enjoy_event_id_init();
   enjoy_module_load();

   elm_run();

 end:
   EINA_LIST_FREE(app.add_dirs, s) free(s);
   EINA_LIST_FREE(app.del_dirs, s) free(s);

   eina_log_domain_unregister(_log_domain);
   _log_domain = -1;
   elm_shutdown();
   enjoy_module_unload();
   cover_shutdown();

   return r;
}

#endif
ELM_MAIN()
