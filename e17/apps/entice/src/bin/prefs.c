#include "prefs.h"
#include "keys.h"
#include <string.h>
#include <Edb.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../config.h"
/* #define PACKAGE_DATA_DIR "/usr/share/entice" */

static Entice_Config *econfig = NULL;


static void entice_config_generate_original_db(char *filename);

/**
 * entice_config_font_cache_get - Get the font cache size in megabytes
 * Returns - value should always be > 0
 */
int
entice_config_font_cache_get(void)
{
   int result = 1;

   if ((econfig) && (econfig->cache.font > 0))
      result = econfig->cache.font;
   return (result);
}

/**
 * entice_config_image_cache_get - Get the font cache size in megabytes
 * Returns - value should always be > 0
 */
int
entice_config_image_cache_get(void)
{
   int result = 4;

   if ((econfig) && (econfig->cache.image > 0))
      result = econfig->cache.image;
   return (result);
}

/**
 * entice_config_theme_get - get the theme name, DO NOT FREE THIS
 * Returns - absolute path to the theme's eet
 */
const char *
entice_config_theme_get(void)
{
   char buf[PATH_MAX];

   if (econfig && econfig->theme)
   {
      struct stat status;

      /* theme doesn't exist by abs/relative path in db */
      if (stat(econfig->theme, &status) != 0)
      {
         snprintf(buf, PATH_MAX, "%s/.entice/%s", getenv("HOME"),
                  econfig->theme);
         if (stat(buf, &status) != 0)
         {
            snprintf(buf, PATH_MAX, "%s/themes/%s", PACKAGE_DATA_DIR,
                     econfig->theme);
            if (stat(buf, &status) != 0)
            {
               snprintf(buf, PATH_MAX, "%s/themes/default.eet",
                        PACKAGE_DATA_DIR);
            }
         }
         if (econfig->theme)
            free(econfig->theme);
         econfig->theme = strdup(buf);
      }
      return (econfig->theme);
   }
   return (NULL);
}

/**
 * entice_config_engine_get - get the ecore_evas engine type
 * Returns - 0 for software, 1 for gl
 */
int
entice_config_engine_get(void)
{
   int result = SOFTWARE_X11;

   if (econfig)
      result = econfig->engine;
   return (result);
}

void
entice_config_geometry_get(int *x, int *y, int *w, int *h)
{
    if(econfig)
    {
	if(x) *x = econfig->x;
	if(y) *y = econfig->y;
	if(w) *w = econfig->w;
	if(h) *h = econfig->h;
    }
}
void
entice_config_geometry_set(int x, int y, int w, int h)
{
    if(econfig)
    {
	char file[PATH_MAX];
	econfig->x = x;
	econfig->y = y;
	econfig->w = w;
	econfig->h = h;
	
	snprintf(file, PATH_MAX, "%s/.entice.db", getenv("HOME"));
	E_DB_INT_SET(file, "/entice/x", x)
	E_DB_INT_SET(file, "/entice/y", y)
	E_DB_INT_SET(file, "/entice/w", w)
	E_DB_INT_SET(file, "/entice/h", h)
	e_db_flush();
    }
}
/**
 * entice_config_new - allocate a new 0'd out Entice_Config
 * Returns a valid 0'd out Entice_Config
 */
Entice_Config *
entice_config_new(void)
{
   Entice_Config *result = NULL;

   if ((result = (Entice_Config *) malloc(sizeof(Entice_Config))))
   {
      char buf[PATH_MAX];

      memset(result, 0, sizeof(Entice_Config));
      snprintf(buf, PATH_MAX, "%s/default.eet", PACKAGE_DATA_DIR);
      result->theme = strdup(buf);
   }
   return (result);
}

/**
 * entice_config_init - parse our config file, setup data structures to
 * be queried later
 */
void
entice_config_init(void)
{
   int i, count;
   char *str = NULL;
   char *symbol = NULL;
   char *signal = NULL;

   char buf[PATH_MAX];
   E_DB_File *db = NULL;
   Entice_Key *key = NULL;

   if (!econfig)
   {
      if ((econfig = entice_config_new()))
      {
         entice_keys_init();
         snprintf(buf, PATH_MAX, "%s/.entice.db", getenv("HOME"));

         /* make sure we have a db, if not generate it */
         if ((db = e_db_open_read(buf)) == NULL)
            entice_config_generate_original_db(buf);
         else
            e_db_close(db);

         /* now actually read the config */
         if ((db = e_db_open_read(buf)))
         {
            if ((str = e_db_str_get(db, "/entice/theme")))
            {
               if (econfig->theme)
                  free(econfig->theme);
               econfig->theme = str;
            }
            if (!e_db_int_get(db, "/entice/engine", &econfig->engine))
               econfig->engine = SOFTWARE_X11;

            if (e_db_int_get(db, "/entice/keys/up/count", &count))
            {
               for (i = 0; i < count; i++)
               {
                  snprintf(buf, PATH_MAX, "/entice/keys/up/%i/symbol", i);
                  if ((symbol = e_db_str_get(db, buf)))
                  {
                     snprintf(buf, PATH_MAX, "/entice/keys/up/%i/signal", i);
                     if ((signal = e_db_str_get(db, buf)))
                     {
                        key = entice_key_new(symbol, signal);
                        entice_keys_up_add(key);
                     }
                  }
               }
            }
            if (e_db_int_get(db, "/entice/keys/down/count", &count))
            {
               for (i = 0; i < count; i++)
               {
                  snprintf(buf, PATH_MAX, "/entice/keys/down/%i/symbol", i);
                  if ((symbol = e_db_str_get(db, buf)))
                  {
                     snprintf(buf, PATH_MAX, "/entice/keys/down/%i/signal",
                              i);
                     if ((signal = e_db_str_get(db, buf)))
                     {
                        key = entice_key_new(symbol, signal);
                        entice_keys_down_add(key);
                     }
                  }
               }
            }
            if (!e_db_int_get(db, "/entice/cache/font", &econfig->cache.font))
               econfig->cache.font = 1;
            if (!e_db_int_get
                (db, "/entice/cache/image", &econfig->cache.image))
               econfig->cache.image = 4;
            if (!e_db_int_get(db, "/entice/x", &econfig->x))
		econfig->x = 0;
            if (!e_db_int_get(db, "/entice/y", &econfig->y))
		econfig->y = 0;
            if (!e_db_int_get(db, "/entice/w", &econfig->w))
		econfig->y = 320;
            if (!e_db_int_get(db, "/entice/h", &econfig->h))
		econfig->y = 120;

            e_db_close(db);
         }
      }
   }
}

static void
entice_config_generate_original_db(char *filename)
{
   int i, count;
   char buf[PATH_MAX];
   E_DB_File *db = NULL;

   char *signals[] = { "EnticeZoomIn", "EnticeZoomOut", "EnticeFullScreen",
      "EnticeImageNext", "EnticeImagePrev", "EnticeZoomDefault",
      "EnticeZoomFit", "EnticeQuit", "EnticeRotateLeft",
      "EnticeRotateRight", "EnticeFlipH", "EnticeFlipV"
   };
   char *keys[] =
      { "equal", "minus", "f", "space", "BackSpace", "n", "w", "q", "Left",
      "Right", "Up", "Down"
   };
   count = sizeof(signals) / sizeof(char *);

   if (filename)
   {
      if ((db = e_db_open(filename)))
      {
         e_db_str_set(db, "/entice/theme", "default.eet");
         e_db_int_set(db, "/entice/engine", 0);

         for (i = 0; i < count; i++)
         {
            snprintf(buf, PATH_MAX, "/entice/keys/up/%i/symbol", i);
            e_db_str_set(db, buf, keys[i]);
            snprintf(buf, PATH_MAX, "/entice/keys/up/%i/signal", i);
            e_db_str_set(db, buf, signals[i]);
         }
         e_db_int_set(db, "/entice/keys/up/count", count);
         e_db_int_set(db, "/entice/cache/font", 1);
         e_db_int_set(db, "/entice/cache/image", 8);

         e_db_close(db);
         e_db_flush();
      }
   }
}
