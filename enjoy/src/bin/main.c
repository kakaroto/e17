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

struct _Enjoy_Plugin {
   EINA_INLIST;
   const char *name;
   const Enjoy_Plugin_Api *api;
   int priority;
   Eina_Bool deleted:1;
   Eina_Bool enabled:1;
};
static int plugins_walking = 0;
static int plugins_deleted = 0;
static Eina_Inlist *plugins_registry = NULL;

static int
_plugin_priority_cmp(const void *pa, const void *pb)
{
   const Enjoy_Plugin *a = pa;
   const Enjoy_Plugin *b = pb;
   int r = a->priority - b->priority;
   if (r) return r;
   return strcmp(a->name, b->name);
}

EAPI Enjoy_Plugin *
enjoy_plugin_register(const char *name, const Enjoy_Plugin_Api *api, int priority)
{
   Enjoy_Plugin *p;
   if (!name)
     {
        ERR("Missing plugin name");
        return NULL;
     }
   if (!api)
     {
        ERR("Missing plugin api");
        return NULL;
     }
   if (api->version != ENJOY_PLUGIN_API_VERSION)
     {
        ERR("Invalid Enjoy_Plugin_Api version: plugin=%u, enjoy=%u",
            api->version, ENJOY_PLUGIN_API_VERSION);
        return NULL;
     }
   if (!api->enable)
     {
        ERR("%s: api->enable == NULL", name);
        return NULL;
     }
   if (!api->disable)
     {
        ERR("%s: api->disable == NULL", name);
        return NULL;
     }

   p = calloc(1, sizeof(Enjoy_Plugin));
   if (!p)
     {
        ERR("Could not allocate plugin structure");
        return NULL;
     }

   p->name = eina_stringshare_add(name);
   p->api = api;
   p->priority = priority;

   plugins_registry = eina_inlist_sorted_insert
     (plugins_registry, EINA_INLIST_GET(p), _plugin_priority_cmp);

   DBG("plugin %s registered %p", name, p);
   return p;
}

void
enjoy_plugins_walk(void)
{
   plugins_walking++;
}

void
enjoy_plugins_unwalk(void)
{
   Eina_Inlist *l;

   plugins_walking--;
   if (plugins_walking > 0) return;
   plugins_walking = 0;

   DBG("delete pending %d plugins", plugins_deleted);
   for (l = plugins_registry; l != NULL && plugins_deleted > 0;)
     {
        Enjoy_Plugin *p = EINA_INLIST_CONTAINER_GET(l, Enjoy_Plugin);

        l = l->next;
        if (!p->deleted) continue;

        DBG("deleted pending %s", p->name);
        plugins_registry = eina_inlist_remove
          (plugins_registry, EINA_INLIST_GET(p));
        eina_stringshare_del(p->name);
        free(p);
        plugins_deleted--;
     }
}

EAPI void
enjoy_plugin_unregister(Enjoy_Plugin *p)
{
   if (!p)
     {
        ERR("No plugin given");
        return;
     }
   if (p->deleted) return;
   p->deleted = EINA_TRUE;

   if (p->enabled) enjoy_plugin_disable(p);

   DBG("plugin %s unregistered %p", p->name, p);
   if (plugins_walking > 0)
     {
        plugins_deleted++;
        return;
     }

   plugins_registry = eina_inlist_remove(plugins_registry, EINA_INLIST_GET(p));
   eina_stringshare_del(p->name);
   free(p);
}

Eina_Bool
enjoy_plugin_enable(Enjoy_Plugin *p)
{
   Eina_Bool r;
   if (!p)
     {
        ERR("No plugin given");
        return EINA_FALSE;
     }
   if (p->enabled) return EINA_TRUE;
   DBG("Enable plugin '%s'", p->name);
   r = p->api->enable(p);
   if (!r) ERR("Failed to enable plugin '%s'", p->name);
   else p->enabled = EINA_TRUE;
   return r;
}

Eina_Bool
enjoy_plugin_disable(Enjoy_Plugin *p)
{
   Eina_Bool r;
   if (!p)
     {
        ERR("No plugin given");
        return EINA_FALSE;
     }
   if (!p->enabled) return EINA_TRUE;
   DBG("Disable plugin '%s'", p->name);
   r = p->api->disable(p);
   if (!r) ERR("Failed to disable plugin '%s'", p->name);
   p->enabled = EINA_FALSE;
   return r;
}

EAPI uint32_t
enjoy_abi_version(void)
{
   return ENJOY_ABI_VERSION;
}

EAPI char *
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

EAPI int ENJOY_EVENT_STARTED = -1;
EAPI int ENJOY_EVENT_QUIT = -1;
EAPI int ENJOY_EVENT_PLAYER_CAPS_CHANGE = -1;
EAPI int ENJOY_EVENT_PLAYER_STATUS_CHANGE = -1;
EAPI int ENJOY_EVENT_PLAYER_TRACK_CHANGE = -1;
EAPI int ENJOY_EVENT_TRACKLIST_TRACKLIST_CHANGE = -1;
EAPI int ENJOY_EVENT_RATING_CHANGE = -1;
EAPI int ENJOY_EVENT_VOLUME_CHANGE = -1;
EAPI int ENJOY_EVENT_POSITION_CHANGE = -1;

static void
enjoy_event_id_init(void)
{
   ENJOY_EVENT_STARTED = ecore_event_type_new();
   ENJOY_EVENT_QUIT = ecore_event_type_new();
   ENJOY_EVENT_PLAYER_CAPS_CHANGE = ecore_event_type_new();
   ENJOY_EVENT_PLAYER_STATUS_CHANGE = ecore_event_type_new();
   ENJOY_EVENT_PLAYER_TRACK_CHANGE = ecore_event_type_new();
   ENJOY_EVENT_TRACKLIST_TRACKLIST_CHANGE = ecore_event_type_new();
   ENJOY_EVENT_RATING_CHANGE = ecore_event_type_new();
   ENJOY_EVENT_VOLUME_CHANGE = ecore_event_type_new();
   ENJOY_EVENT_POSITION_CHANGE = ecore_event_type_new();
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

static int _quit_count = 0;

static void
_enjoy_event_quit_done(void *a __UNUSED__, void *b __UNUSED__)
{
   if (_quit_count > 0) return;
   ecore_main_loop_quit();
}

EAPI void
enjoy_quit(void)
{
   static Eina_Bool _called = EINA_FALSE;
   Enjoy_Plugin *p;

   if (_called) return;
   _called = EINA_TRUE;

   enjoy_plugins_walk();
   EINA_INLIST_FOREACH(plugins_registry, p)
     enjoy_plugin_disable(p);
   enjoy_plugins_unwalk();

   ecore_event_add(ENJOY_EVENT_QUIT, NULL, _enjoy_event_quit_done, NULL);
}

EAPI void
enjoy_quit_freeze(void)
{
   _quit_count++;
}

EAPI void
enjoy_quit_thaw(void)
{
   _quit_count--;
   if (_quit_count > 0) return;
   ecore_main_loop_quit();
}

static Eina_Bool
_cb_started(void *data __UNUSED__, int type __UNUSED__, void *event __UNUSED__)
{
   Enjoy_Plugin *p;

   enjoy_plugins_walk();
   EINA_INLIST_FOREACH(plugins_registry, p)
     enjoy_plugin_enable(p);
   enjoy_plugins_unwalk();

   return ECORE_CALLBACK_PASS_ON;
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
   ecore_event_handler_add(ENJOY_EVENT_STARTED, _cb_started, NULL);

   enjoy_module_load();

   /* will run after other events run, in the main loop */
   ecore_event_add(ENJOY_EVENT_STARTED, NULL, NULL, NULL);
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
