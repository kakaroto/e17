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

#include "Exchange.h"
#include <string.h>

/**
 * @addtogroup Exchange_Local_Theme_Group Exchange Local Theme Functions
 *
 * Functions that handles local theme metadata.
 *
 * @{
 */

/**
 * @param file The path to the theme file
 * @return Returns the name of the local theme, NULL otherwise
 * @brief Get theme name from the local theme file. 
 */
EAPI char *
exchange_local_theme_name_get(const char *file)
{
   return (char *)edje_file_data_get(file, "theme/name");
}

/**
 * @param file The path to the theme file
 * @return Returns the author of the local theme, NULL otherwise
 * @brief Get author from the local theme file. 
 */
EAPI char *
exchange_local_theme_author_get(const char *file)
{
   return (char *)edje_file_data_get(file, "theme/author");
}

/**
 * @param file The path to the theme file
 * @return Returns the license of the local theme, NULL otherwise
 * @brief Get license from the local theme file. 
 */
EAPI char *
exchange_local_theme_license_get(const char *file)
{
   return (char *)edje_file_data_get(file, "theme/license");
}

/**
 * @param file The path to the theme file
 * @return Returns the version of the local theme, NULL otherwise
 * @brief Get version from the local theme file. 
 */
EAPI char *
exchange_local_theme_version_get(const char *file)
{
   return (char *)edje_file_data_get(file, "theme/version");
}

/**
 * @param file The path to the theme file
 * @return Returns 1 if there's a update available, 0 if not, -1 if theme (local file or remote data) doesn't contains version
 * @brief Check if there's a update available for a local theme. 
 */
EAPI int
exchange_local_theme_check_update(const char *file)
{
   char *local_name = NULL;
   char *local_ver = NULL;
   char *remote_ver = NULL;
   int ret;

   local_name = exchange_local_theme_name_get(file);
   local_ver = exchange_local_theme_version_get(file);
   remote_ver = exchange_remote_theme_version_get(local_name);

   if ((local_ver == NULL) || (remote_ver == NULL))
      return -1;

   if (strcmp(remote_ver, local_ver) > 0)
      ret = 1;
   else
      ret = 0;

   free(local_name);
   free(local_ver);

   return ret;
}

/**
 * @}
 */
