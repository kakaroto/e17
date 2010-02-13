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

/*
 * Description: listens on the UPnP network and prints devices announcements
 * (arrival or departure).
 */

#include <stdio.h>
#include <string.h>
#include <Eina.h>
#include "Eupnp.h"

static int _log_domain = -1;

#ifdef INF
  #undef INF
#endif
#define INF(...) EINA_LOG_DOM_INFO(_log_domain, __VA_ARGS__)

/*
 * Receives a built device. Note that although this device object is the same
 * as the one received on the on_device_found callback, it contains a lot more
 * information for the user to use, since it's "ready" for use.
 */
static Eina_Bool
on_device_ready(void *user_data, Eupnp_Event_Type event_type, void *event_data)
{
   Eupnp_Device_Info *d = event_data;
   eupnp_device_info_dump(d);
   return EINA_TRUE;
}

/*
 * Called when a device annouces its presence. Receives a device object that
 * contains basic info about the device.
 */
static Eina_Bool
on_device_found(void *user_data, Eupnp_Event_Type event_type, void *event_data)
{
   Eupnp_Device_Info *d = event_data;
   INF("Device found: %s", d->udn);
   return EINA_TRUE;
}

/*
 * Called when a device annouces its departure.
 *
 * Note that this device object received is not the same as the one
 * received on on_device_found and on_device_ready callbacks. This is because
 * Eupnp does not keep track of anything, it leaves to the user to decide what
 * they'll effectivelly use.
 *
 * Users should match these received device info objects to the ones they keep a
 * reference. For example, if a user keeps a list of ready devices (that are
 * passed to on_device_ready), he/she should iterate on that list and match the
 * device udn which announced its departure.
 */
static Eina_Bool
on_device_gone(void *user_data, Eupnp_Event_Type event_type, void *event_data)
{
   Eupnp_Device_Info *d = event_data;

   if (d)
     {
	INF("Device gone");
	INF("\tudn: %s", d->udn);
	INF("\tlocation: %s", d->location);
     }

   return EINA_FALSE;
}

/*
 * Called when a service annouces its presence.
 */
static Eina_Bool
on_service_found(void *user_data, Eupnp_Event_Type event_type, void *event_data)
{
   Eupnp_Service_Info *s = event_data;

   if (s)
     {
	INF("Service found");
	INF("\tudn: %s", s->udn);
	INF("\ttype: %s", s->service_type);
	INF("\tlocation: %s", s->location);
     }

   return EINA_TRUE;
}

/*
 * Called when a service annouces it left.
 */
static Eina_Bool
on_service_gone(void *user_data, Eupnp_Event_Type event_type, void *event_data)
{
   Eupnp_Service_Info *s = event_data;

   if (s)
     {
	INF("Service gone");
	INF("\tudn: %s", s->udn);
	INF("\ttype: %s", s->service_type);
	INF("\tlocation: %s", s->location);
     }

   return EINA_TRUE;
}

/*
 * Run with "EINA_LOG_LEVELS=upnp_sniffer:4,Eupnp.DeviceInfo:4 ./upnp_sniffer"
 * for watching all log messages.
 */
int main(void)
{
   Eupnp_Control_Point *c;
   int ret = -1;

   if (!eupnp_init())
     {
	fprintf(stderr, "Failed to initialize eina log module.\n");
	return ret;
     }

   if ((_log_domain = eina_log_domain_register("upnp_sniffer", EINA_COLOR_BLUE)) < 0)
     {
	fprintf(stderr, "Failed to create a logging domain for the application.\n");
	goto log_domain_reg_error;
     }

   if (!eupnp_ecore_init())
     {
	fprintf(stderr, "Could not initialize eupnp-ecore\n");
	goto eupnp_ecore_init_error;
     }

   c = eupnp_control_point_new();

   if (!c)
     {
	fprintf(stderr, "Could not create control point instance\n");
	goto eupnp_cp_alloc_error;
     }

   /* Subscribe for device events */
   eupnp_event_bus_subscribe(EUPNP_EVENT_DEVICE_FOUND,
			     EUPNP_CALLBACK(on_device_found), NULL);
   eupnp_event_bus_subscribe(EUPNP_EVENT_DEVICE_READY,
			     EUPNP_CALLBACK(on_device_ready), NULL);
   eupnp_event_bus_subscribe(EUPNP_EVENT_DEVICE_GONE,
			     EUPNP_CALLBACK(on_device_gone), NULL);

   /* Subscribe for service events */
   eupnp_event_bus_subscribe(EUPNP_EVENT_SERVICE_FOUND,
			     EUPNP_CALLBACK(on_service_found), NULL);
   eupnp_event_bus_subscribe(EUPNP_EVENT_SERVICE_GONE,
			     EUPNP_CALLBACK(on_service_gone), NULL);

   /* Start control point */
   eupnp_control_point_start(c);

   eupnp_control_point_discovery_request_send(c, 5, "upnp:rootdevice");

   ret = 0;
   INF("Started sniffing on the UPnP network.");
   ecore_main_loop_begin();
   INF("Shutting down application.");

   /* Shutdown procedure */
   eupnp_control_point_stop(c);
   eupnp_control_point_free(c);

   eupnp_cp_alloc_error:
   eupnp_cp_init_error:
      eupnp_ecore_shutdown();
   eupnp_ecore_init_error:
      eina_log_domain_unregister(_log_domain);
   log_domain_reg_error:
      eupnp_shutdown();

   return ret;
}

