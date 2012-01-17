#include <Eina.h>
#include <Ecore.h>

#include "plugin.h"
#include "song.h"

static int _albumart_lastfm_log_domain = -1;

#ifdef CRITICAL
#undef CRITICAL
#endif
#ifdef ERR
#undef ERR
#endif
#ifdef WRN
#undef WRN
#endif
#ifdef INF
#undef INF
#endif
#ifdef DBG
#undef DBG
#endif

#define CRITICAL(...) EINA_LOG_DOM_CRIT(_albumart_lastfm_log_domain, __VA_ARGS__)
#define ERR(...)      EINA_LOG_DOM_ERR(_albumart_lastfm_log_domain, __VA_ARGS__)
#define WRN(...)      EINA_LOG_DOM_WARN(_albumart_lastfm_log_domain, __VA_ARGS__)
#define INF(...)      EINA_LOG_DOM_INFO(_albumart_lastfm_log_domain, __VA_ARGS__)
#define DBG(...)      EINA_LOG_DOM_DBG(_albumart_lastfm_log_domain, __VA_ARGS__)

static const char *
albumart_lastfm_prefs_category_get(Enjoy_Preferences_Plugin *p __UNUSED__)
{
   return "Album Arts";
}

static const char *
albumart_lastfm_prefs_label_get(Enjoy_Preferences_Plugin *p __UNUSED__)
{
   return "Search last.fm";
}

static Eina_Bool
albumart_lastfm_prefs_activated(Enjoy_Preferences_Plugin *p __UNUSED__, Evas_Object *naviframe __UNUSED__, Evas_Object **prev_btn __UNUSED__, Evas_Object **next_btn __UNUSED__, Evas_Object **content __UNUSED__, Eina_Bool *auto_prev_btn __UNUSED__)
{
   // TODO
   ERR("TODO!!!");
   return EINA_FALSE;
}

static const Enjoy_Preferences_Plugin_Api prefs_api = {
  ENJOY_PREFERENCES_PLUGIN_API_VERSION,
  albumart_lastfm_prefs_category_get,
  albumart_lastfm_prefs_label_get,
  albumart_lastfm_prefs_activated
};

static Enjoy_Preferences_Plugin *_albumart_lastfm_prefs_plugin = NULL;

static Eina_Bool
albumart_lastfm_enable(Enjoy_Plugin *p __UNUSED__)
{
   if (_albumart_lastfm_prefs_plugin)
     return EINA_TRUE;

   _albumart_lastfm_prefs_plugin = enjoy_preferences_plugin_register
     (&prefs_api, ENJOY_PLUGIN_PRIORITY_NORMAL);
   if (!_albumart_lastfm_prefs_plugin)
     {
        ERR("Could not register preferences plugin!");
        return EINA_FALSE;
     }

   return EINA_TRUE;
}

static Eina_Bool
albumart_lastfm_disable(Enjoy_Plugin *p __UNUSED__)
{
   if (!_albumart_lastfm_prefs_plugin) return EINA_TRUE;
   enjoy_preferences_plugin_unregister(_albumart_lastfm_prefs_plugin);
   _albumart_lastfm_prefs_plugin = NULL;

   return EINA_TRUE;
}

static const Enjoy_Plugin_Api api = {
  ENJOY_PLUGIN_API_VERSION,
  albumart_lastfm_enable,
  albumart_lastfm_disable
};

static Eina_Bool
albumart_lastfm_init(void)
{
   if (_albumart_lastfm_log_domain < 0)
     {
        _albumart_lastfm_log_domain = eina_log_domain_register
          ("enjoy-albumart_lastfm", EINA_COLOR_LIGHTCYAN);
        if (_albumart_lastfm_log_domain < 0)
          {
             EINA_LOG_CRIT("Could not register log domain 'enjoy-albumart_lastfm'");
             return EINA_FALSE;
          }
     }

   if (!ENJOY_ABI_CHECK())
     {
        ERR("ABI versions differ: enjoy=%u, albumart_lastfm=%u",
            enjoy_abi_version(), ENJOY_ABI_VERSION);
        goto error;
     }

   enjoy_plugin_register("preferences/albumart_lastfm",
                         &api, ENJOY_PLUGIN_PRIORITY_NORMAL);

   return EINA_TRUE;

 error:
   eina_log_domain_unregister(_albumart_lastfm_log_domain);
   _albumart_lastfm_log_domain = -1;
   return EINA_FALSE;
}

void
albumart_lastfm_shutdown(void)
{
   if (_albumart_lastfm_log_domain >= 0)
     {
        eina_log_domain_unregister(_albumart_lastfm_log_domain);
        _albumart_lastfm_log_domain = -1;
     }
}

EINA_MODULE_INIT(albumart_lastfm_init);
EINA_MODULE_SHUTDOWN(albumart_lastfm_shutdown);
