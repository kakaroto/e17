/* EXCHANGE - a library to interact with exchange.enlightenment.org
 * Copyright (C) 2008 Massimiliano Calamelli
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library;
 * if not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Ecore.h>

#include "Exchange.h"
#include "exchange_private.h"

static int _exchange_init_count = 0;
static const char *_cache_folder = NULL;

int __exchange_log_domain = -1;

/**
 * @addtogroup Exchange_Init_Group Exchange Init and Shutdown Functions
 *
 * Functions that init and shut down Exchange library.
 *
 * Sample code
 * @code
 * int main(int argc, char **argv)
 * {
 *    exchange_init();
 *
 *    // some code
 *
 *    exchange_shutdown();
 *
 *    return 0;
 * }
 * @endcode
 * @{
 */

static const char *
_exchange_user_homedir_get(void)
{ // TODO This function should be moved into ecore (or I have missed it?)
   char *homedir;
   int len;

   homedir = getenv("HOME");
   if (!homedir) return "/tmp";
   len = strlen(homedir);
   while ((len > 1) && (homedir[len - 1] == '/'))
     {
        homedir[len - 1] = 0;
        len--;
     }
   return homedir;
}

/**
 * @return Returns > 0 if the initialization was successful, 0 otherwise
 * @brief Initializes the Exchange library
 */
EAPI int
exchange_init(void)
{
   if (++_exchange_init_count > 1) return _exchange_init_count;

   if (!eina_init()) return 0;
   __exchange_log_domain = eina_log_domain_register("Exchange", EINA_COLOR_BLUE);
   if (__exchange_log_domain < 0)
   {
      EINA_LOG_ERR("Could not register log domain: Exchange");
      eina_shutdown();
      return 0;
   }

   return _exchange_init_count;
}

/**
 * @return Returns the number of times the init function as been called
 * minus the corresponding shutdown call.
 * @brief Shuts down Exchange if a balanced number of init/shutdown calls have
 * been made
 */
EAPI int
exchange_shutdown(void)
{
   if (--_exchange_init_count > 0) return _exchange_init_count;

   _theme_free_data(); //TODO Whats this ??
   _login_free_data(); //TODO Whats this ??
   _theme_group_free_data(); //TODO Whats this ??

   if (_cache_folder)
   {
      eina_stringshare_del(_cache_folder);
      _cache_folder = NULL;
   }

   xmlCleanupParser();

   eina_log_domain_unregister(__exchange_log_domain);
   __exchange_log_domain = -1;
   eina_shutdown();

   return _exchange_init_count;
}

/**
 * @param object The object to free
 * @return EINA_TRUE on success or EINA_FALSE on errors
 * @brief Free an Exchange_Object structure (can be a theme, a module or an app)
 */
EAPI Eina_Bool
exchange_object_free(Exchange_Object *obj)
{
   if (!obj) return 0;

   if (obj->job_thumbnail) ecore_file_download_abort(obj->job_thumbnail);
   if (obj->job_screenshot) ecore_file_download_abort(obj->job_screenshot);
   if (obj->job_entry) ecore_file_download_abort(obj->job_entry);

   if (obj->name) eina_stringshare_del(obj->name);
   if (obj->author) free(obj->author);
   if (obj->license) free(obj->license);
   if (obj->version) free(obj->version);
   if (obj->url) free(obj->url);
   if (obj->thumbnail) free(obj->thumbnail);
   if (obj->thumbnail_local) eina_stringshare_del(obj->thumbnail_local);
   if (obj->screenshot) free(obj->screenshot);
   if (obj->screenshot_local) eina_stringshare_del(obj->screenshot_local);
   if (obj->created_at) free(obj->created_at);
   if (obj->updated_at) free(obj->updated_at);
   

   free(obj);
   obj = NULL;
   return 1;
}

/**
 * Get the cache directory (usually ~/.cache/exchange.org)
 * @return The cache directory or NULL on errors
 */
EAPI const char *
exchange_cache_dir_get(void)
{
   char buf[4096]; // TODO PATH_MAX

   if (_cache_folder) return _cache_folder;

   snprintf(buf, sizeof(buf), "%s/.cache/exchange.org",
                 _exchange_user_homedir_get());
   if (!ecore_file_exists(buf) && !ecore_file_mkpath(buf))
   {
      ERR("Can't create cache dir '%s'\n", buf);
      return NULL;
   }

   return _cache_folder = eina_stringshare_add(buf);
}

/**
 * @}
 */
