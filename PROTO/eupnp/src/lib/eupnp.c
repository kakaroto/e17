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

/**
 * @page eupnp_app_writing Writing an Application
 *
 * In the following sections we will discuss the basic concepts of Eupnp, how to
 * use it for writing an UPnP application and good practices.
 *
 * @section eupnp_sec_initializing Initializing Eupnp
 *
 * Before using Eupnp libraries and modules, eupnp_init() must be called from
 * the main application. This call will perform the required initialization of
 * the library.
 *
 * Once the library is no longer used, it must be shutted down by calling
 * eupnp_shutdown(). This is usually done at the end of the main application, as
 * illustrated below:
 *
 * @code
 * #include <Eupnp.h>
 *
 * int
 * main(int argc, char *argv[])
 * {
 *     eupnp_init();
 *
 *     // Application
 *
 *     eupnp_shutdown();
 *
 *     return 0;
 * }
 * @endcode
 *
 *
 * eupnp_init() will return 0 whenever it fails to initialize the library
 * (possibly lack of free memory, network down, etc). Otherwise, it'll return
 * the number of times it's been called. The above code checks if the library
 * was successfully initialized:
 *
 * @code
 * #include <Eupnp.h>
 *
 * int
 * main(int argc, char *argv[])
 * {
 *     if (!eupnp_init())
 *       {
 *           // Failed to initialize
 *           return -1;
 *       }
 *
 *     // Application
 *
 *     eupnp_shutdown();
 *
 *     return 0;
 * }
 * @endcode
 *
 */

/**
 * Initializes Eupnp library.
 *
 * This function initializes all Eupnp modules.
 *
 * @return On error, returns 0. Otherwise, returns the number of times it's been
 *         called.
 */
EAPI int
eupnp_init(void)
{
   if (_eupnp_main_count) return ++_eupnp_main_count;

   if (!eina_init())
     {
	fprintf(stderr, "Failed to initialize eina\n");
	return _eupnp_main_count;
     }

   if (!eupnp_log_init())
     {
	fprintf(stderr, "Failed to initialize eupnp error module\n");
	goto log_init_error;
     }

   if (!eupnp_event_bus_init())
     {
	fprintf(stderr, "Failed to initialize eupnp event bus module.\n");
	goto event_bus_init_error;
     }

   if (!eupnp_service_info_init())
     {
	fprintf(stderr, "Failed to initialize eupnp service info module.\n");
	goto service_info_init_error;
     }

   if (!eupnp_device_parser_init())
     {
	fprintf(stderr, "Failed to initialize eupnp device parser module.\n");
	goto device_parser_init_error;
     }

   if (!eupnp_service_parser_init())
     {
	fprintf(stderr, "Failed to initialize eupnp service parser module.\n");
	goto service_parser_init_error;
     }

   if (!eupnp_device_info_init())
     {
	fprintf(stderr, "Failed to initialize eupnp device info module.\n");
	goto device_info_init_error;
     }

   if (!eupnp_ssdp_init())
     {
	fprintf(stderr, "Failed to initialize eupnp ssdp module.\n");
	goto ssdp_init_error;
     }

   if (!eupnp_control_point_init())
     {
	fprintf(stderr, "Failed to initialize eupnp control point module.\n");
	goto control_point_init_error;
     }

   if (!eupnp_service_proxy_init())
     {
	fprintf(stderr, "Failed to initialize eupnp service proxy module.\n");
	goto service_proxy_init_error;
     }

   INFO("Initializing eupnp library");
   return ++_eupnp_main_count;

   service_proxy_init_error:
      eupnp_control_point_shutdown();
   control_point_init_error:
      eupnp_ssdp_shutdown();
   ssdp_init_error:
      eupnp_device_info_shutdown();
   device_info_init_error:
      eupnp_service_parser_shutdown();
   service_parser_init_error:
      eupnp_device_parser_shutdown();
   device_parser_init_error:
      eupnp_service_info_shutdown();
   service_info_init_error:
      eupnp_event_bus_shutdown();
   event_bus_init_error:
      eupnp_log_shutdown();
   log_init_error:
      eina_shutdown();

   return 0;
}

/**
 * Shuts down the Eupnp library.
 *
 * @return 0 if completely shutted down.
 */
EAPI int
eupnp_shutdown(void)
{
   if (_eupnp_main_count != 1) return --_eupnp_main_count;

   INFO("Shutting down eupnp library");

   eupnp_control_point_shutdown();
   eupnp_ssdp_shutdown();
   eupnp_device_info_shutdown();
   eupnp_service_parser_shutdown();
   eupnp_device_parser_shutdown();
   eupnp_service_info_shutdown();
   eupnp_event_bus_shutdown();
   eupnp_log_shutdown();
   eina_shutdown();

   return --_eupnp_main_count;
}

