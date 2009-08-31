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

#include "Eupnp.h"


static int _eupnp_main_count = 0;


/*
 * Initializes Eupnp library.
 *
 * This function initializes all Eupnp modules.
 *
 * @return On error, returns 0. Otherwise, returns the number of times it's been
 * called.
 */
EAPI int
eupnp_init(void)
{
   if (_eupnp_main_count) return ++_eupnp_main_count;

   if (!eupnp_log_init())
     {
	fprintf(stderr, "Failed to initialize eupnp error module\n");
	return _eupnp_main_count;
     }

   if (!eupnp_event_bus_init())
     {
	fprintf(stderr, "Failed to initialize eupnp event bus module.\n");
	goto event_bus_init_error;
     }

   if (!eupnp_device_info_init())
     {
	fprintf(stderr, "Failed to initialize eupnp device info module.\n");
	goto device_info_init_error;
     }

   if (!eupnp_service_info_init())
     {
	fprintf(stderr, "Failed to initialize eupnp service info module.\n");
	goto service_info_init_error;
     }

   if (!eupnp_ssdp_init())
     {
	fprintf(stderr, "Failed to initialize eina array module\n");
	goto ssdp_init_error;
     }

   if (!eupnp_control_point_init())
     {
	fprintf(stderr, "Failed to initialize eupnp control point module\n");
	goto control_point_init_error;
     }

   INFO("Initializing eupnp library\n");

   return ++_eupnp_main_count;

   control_point_init_error:
      eupnp_ssdp_shutdown();
   ssdp_init_error:
      eupnp_service_info_shutdown();
   service_info_init_error:
      eupnp_device_info_shutdown();
   device_info_init_error:
      eupnp_event_bus_shutdown();
   event_bus_init_error:
      eupnp_log_shutdown();

   return 0;
}

/*
 * Shuts down the Eupnp library.
 *
 * @return 0 if completely shutted down.
 */
EAPI int
eupnp_shutdown(void)
{
   if (_eupnp_main_count != 1) return --_eupnp_main_count;

   INFO("Shutting down eupnp library\n");

   eupnp_control_point_shutdown();
   eupnp_ssdp_shutdown();
   eupnp_service_info_shutdown();
   eupnp_device_info_shutdown();
   eupnp_event_bus_shutdown();
   eupnp_log_shutdown();

   return --_eupnp_main_count;
}

