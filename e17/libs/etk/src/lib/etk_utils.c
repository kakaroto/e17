/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
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
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

/** @file etk_utils.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_utils.h"

#include <sys/time.h>

#include "etk_types.h"
#include "etk_signal.h"

/**
 * @addtogroup Etk_Utils
 * @{
 */

/**
 * @brief A utility function to use as a swapped callback.
 * It sets to NULL the pointer located at the adress stored in @a data. @n
 * For example, if you want to set "pointer" to NULL when "button" is clicked, you can do: @n
 * etk_signal_connect_swapped(ETK_OBJECT(button), "clicked", ETK_CALLBACK(etk_callback_set_null), &pointer);
 * @param data @a data stores the location of the pointer to set to NULL
 */
Etk_Bool etk_callback_set_null(void *data)
{
   if (!data)
      return ETK_TRUE;
   *((void **)data) = NULL;

   return ETK_TRUE;
}

/**
 * @brief Gets the current time, in milliseconds
 * @return Returns the current time, in milliseconds
 */
unsigned int etk_current_time_get()
{
   struct timeval timev;

   gettimeofday(&timev, NULL);
   return ((timev.tv_sec * 1000) + ((timev.tv_usec) / 1000));
}

/** @} */
