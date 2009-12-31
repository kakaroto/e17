/* Eupnp - UPnP library
 *
 * Copyright (C) 2009 Andre Dieb Martins <andre.dieb@gmail.com>
 *
 * This file is part of Eupnp.
 *
 * Eupnp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Eupnp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Eupnp.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>

#include <Eina.h>

#include "eupnp_log.h"


int EUPNP_LOGGING_DOM_GLOBAL = -1;


/*
 * Initializes error module.
 *
 * This function initializes error module.
 *
 * @return On error, returns 0. Otherwise, returns the number of times it's been
 * called.
 */
Eina_Bool
eupnp_log_init(void)
{
   EUPNP_LOGGING_DOM_GLOBAL = eina_log_domain_register("Eupnp", EINA_COLOR_RESET);

   if (EUPNP_LOGGING_DOM_GLOBAL < 0)
     {
	fprintf(stderr, "Failed to register global error domain.\n");
	return EINA_FALSE;
     }

   return EINA_TRUE;
}

/*
 * Shuts down the error module.
 *
 * @return 0 if completely shutted down.
 */
Eina_Bool
eupnp_log_shutdown(void)
{
   eina_log_domain_unregister(EUPNP_LOGGING_DOM_GLOBAL);
   return EINA_TRUE;
}
