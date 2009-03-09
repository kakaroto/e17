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
   exchange_smart_shutdown();

   edje_shutdown();
   
   return _exchange_init_count;
}

/**
 * @param object The object to free
 * @return 1 on success or 0 on errors
 * @brief Free an Exchange_Object structure (can be a theme, a module or an app)
 */
EAPI unsigned char
exchange_object_free(Exchange_Object *obj)
{
   if (!obj) return 0;

   if (obj->name) eina_stringshare_del(obj->name);
   if (obj->author) free(obj->author);
   if (obj->license) free(obj->license);
   if (obj->version) free(obj->version);
   if (obj->url) free(obj->url);
   if (obj->thumbnail) free(obj->thumbnail);
   if (obj->screenshot) free(obj->screenshot);
   if (obj->created_at) free(obj->created_at);
   if (obj->updated_at) free(obj->updated_at);

   free(obj);
   obj = NULL;
   return 1;
}

/**
 * @}
 */
