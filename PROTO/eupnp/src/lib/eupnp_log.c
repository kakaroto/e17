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


#include <stdio.h>
#include <Eina.h>

#include "eupnp_log.h"


static int _eupnp_log_init_count = 0;
int EUPNP_LOGGING_DOM_GLOBAL = -1;


/*
 * Initializes error module.
 *
 * This function initializes error module.
 *
 * @return On error, returns 0. Otherwise, returns the number of times it's been
 * called.
 */
int
eupnp_log_init(void)
{
   if (_eupnp_log_init_count) return ++_eupnp_log_init_count;

   if (!eina_log_init())
     {
	fprintf(stderr, "Failed to initialize eina error module.\n");
	return 0;
     }

   EUPNP_LOGGING_DOM_GLOBAL = eina_log_domain_register("Eupnp", EINA_COLOR_RESET);

   if (EUPNP_LOGGING_DOM_GLOBAL < 0)
     {
	fprintf(stderr, "Failed to register global error domain.\n");
	eina_log_shutdown();
	return 0;
     }

   return ++_eupnp_log_init_count;
}

/*
 * Shuts down the error module.
 *
 * @return 0 if completely shutted down.
 */
int
eupnp_log_shutdown(void)
{
   if (_eupnp_log_init_count != 1) return --_eupnp_log_init_count;

   eina_log_domain_unregister(EUPNP_LOGGING_DOM_GLOBAL);
   eina_log_shutdown();

   return --_eupnp_log_init_count;
}
