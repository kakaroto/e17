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

static int _exchange_init_count = 0;

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

/**
 * @return Returns > 0 if the initialization was successful, 0 otherwise
 * @brief Initializes the Exchange library
 */
EAPI int
exchange_init(void)
{
   if (++_exchange_init_count != 1) return _exchange_init_count;

   edje_init();

   return _exchange_init_count;
}

/**
 * @return Returns the number of times the init function as been called
 * minus the corresponding init call.
 * @brief Shuts down Exchange if a balanced number of init/shutdown calls have
 * been made
 */
EAPI int
exchange_shutdown(void)
{
   if (--_exchange_init_count != 0) return _exchange_init_count;

   _theme_free_data();
   _login_free_data();
   _theme_group_free_data();
   _application_list_free_data();
   _module_list_free_data();
   exchange_smart_shutdown();

   edje_shutdown();
   
   return _exchange_init_count;
}

/**
 * @param theme The Theme to free
 * @return 1 on success or 0 on errors
 * @brief Free an Exchange_Theme structure
 */
EAPI unsigned char
exchange_theme_free(Exchange_Theme *theme)
{
   if (!theme) return 0;

   if (theme->author) free(theme->author);
   if (theme->license) free(theme->license);
   if (theme->version) free(theme->version);
   if (theme->url) free(theme->url);
   if (theme->thumbnail) free(theme->thumbnail);
   if (theme->screenshot) free(theme->screenshot);
   if (theme->created_at) free(theme->created_at);
   if (theme->updated_at) free(theme->updated_at);

   free(theme);
   theme = NULL;
   return 1;
}

/**
 * @}
 */
