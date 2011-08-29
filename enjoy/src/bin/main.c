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
  "%prog [options] [url]",
  PACKAGE_VERSION "Revision:" stringify(VREV),
  "(C) 2010 ProFUSION embedded systems",
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

int
enjoy_event_id_get(Enjoy_Event_ID event_id)
{
   switch (event_id)
     {
       case ENJOY_EVENT_PLAYER_CAPS_CHANGE: return app.event_id.player.caps_change;
       case ENJOY_EVENT_PLAYER_STATUS_CHANGE: return app.event_id.player.status_change;
       case ENJOY_EVENT_PLAYER_TRACK_CHANGE: return app.event_id.player.track_change;
       case ENJOY_EVENT_TRACKLIST_TRACKLIST_CHANGE: return app.event_id.tracklist.tracklist_change;
     }
   return -1;
}

static void
enjoy_event_id_init()
{
   ecore_init();
   app.event_id.player.caps_change = ecore_event_type_new();
   app.event_id.player.status_change = ecore_event_type_new();
   app.event_id.player.track_change = ecore_event_type_new();
   app.event_id.tracklist.tracklist_change = ecore_event_type_new();
}

static void
enjoy_event_id_shutdown()
{
   ecore_shutdown();
}

static Eina_Bool
enjoy_module_load_one(Eina_Module *module, void *data)
{
   const char *filename = eina_module_file_get(module);
   const char *prefix = "libenjoyplugin";

   if (!strstr(filename, prefix)) return EINA_FALSE;
   if (!eina_module_load(module))
     {
        WRN("Couldn't load module %s.", filename);
        return EINA_FALSE;
     }

   return EINA_TRUE;
}

static void
enjoy_module_load()
{
   puts("loading modules from " LIBRARY_DIR);
   app.modules = eina_module_list_get(NULL, LIBRARY_DIR, EINA_FALSE, enjoy_module_load_one, NULL);
}

static void
enjoy_module_unload()
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
   enjoy_event_id_shutdown();
   cover_shutdown();

   return r;
}

#endif
ELM_MAIN()
