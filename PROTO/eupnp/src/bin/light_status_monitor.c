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
 * Description: sends a search for all upnp devices on the network and monitors
 * BinaryLight and DimmableLight devices light status by printing changes on the
 * screen.
 */

#include <stdio.h>
#include <string.h>
#include <Eina.h>
#include "Eupnp.h"

#define SWITCH_POWER_SERVICE_TYPE "urn:schemas-upnp-org:service:SwitchPower:1"

static int _log_domain = -1;

/*
 * Receives a status change event sent from a light device. Copies the event
 * data received and prints it.
 */
static Eina_Bool
on_status_event(Eupnp_State_Variable *var, void *buffer, int size, void *data)
{
   if (size > 0)
     {
	char *tmp;
	tmp = malloc(sizeof(char)*(size + 1));
	memcpy(tmp, buffer, size);
	tmp[size] = '\0';
	INFO_D(_log_domain, "Status changed event data: %s", tmp);
	free(tmp);
     }
   else
	ERROR_D(_log_domain, "Status event incomplete.");

   return EINA_TRUE;
}

/*
 * Receives a ready proxy for a Switch Power service of found light devices.
 * Subscribes the on_status_event callback for changes on the "Status" state
 * variable.
 */
static void
on_switch_power_proxy_ready(void *data, Eupnp_Service_Proxy *proxy)
{
   if (!eupnp_service_proxy_state_variable_events_subscribe(proxy,
							    "Status",   // variable name
							    EUPNP_STATE_VARIABLE_EVENT_CB(on_status_event),
							    EINA_FALSE, // Auto-renew
							    EINA_FALSE, // Infinite subscription
							    100,        // 100 seconds subscription
							    NULL))
     ERROR_D(_log_domain, "Failed to subscribe for events on Status variable.");
}

/*
 * Receives devices ready to be used. Only uses effectivelly BinaryLight and
 * DimmableLight devices.
 */
static Eina_Bool
on_device_ready(void *user_data, Eupnp_Event_Type event_type, void *event_data)
{
   Eupnp_Device_Info *d = event_data;
   Eupnp_Service_Info *service;

   /*
    * Only request proxies for SwitchPower services, which are part of
    * BinaryLight and DimmableLight devices.
    */
   EINA_INLIST_FOREACH(d->services, service)
     if (!strcmp(service->service_type, SWITCH_POWER_SERVICE_TYPE))
	eupnp_service_proxy_new(service, EUPNP_SERVICE_PROXY_READY_CB(on_switch_power_proxy_ready), NULL);

   return EINA_TRUE;
}

/*
 * Sends a search for all upnp devices on the network and monitors BinaryLight
 * and DimmableLight devices light status.
 *
 * Run "EINA_LOG_LEVELS=LightStatusMonitor:5 ./light_status_monitor" for
 * watching all log messages.
 */
int main(void)
{
   Eupnp_Control_Point *c;
   int ret = -1;

   if (!eupnp_init())
     {
	fprintf(stderr, "Failed to initialize eupnp module.\n");
	return ret;
     }

   if (!eupnp_ecore_init())
     {
	fprintf(stderr, "Could not initialize eupnp-ecore\n");
	goto eupnp_ecore_init_error;
     }

   if ((_log_domain = eina_log_domain_register("LightStatusMonitor", EINA_COLOR_BLUE)) < 0)
     {
	fprintf(stderr, "Failed to create a logging domain for the application.\n");
	goto log_domain_reg_error;
     }

   c = eupnp_control_point_new();

   if (!c)
     {
	fprintf(stderr, "Could not create control point instance\n");
	goto eupnp_cp_alloc_error;
     }

   /* Subscribe for device events */
   eupnp_event_bus_subscribe(EUPNP_EVENT_DEVICE_READY,
			     EUPNP_CALLBACK(on_device_ready), NULL);

   /* Start control point */
   eupnp_control_point_start(c);

   /* Send a test search for all devices*/
   if (!eupnp_control_point_discovery_request_send(c, 5, EUPNP_ST_SSDP_ALL))
	ERROR_D(_log_domain, "Failed to perform MSearch.");
   else
	INFO_D(_log_domain, "MSearch sent sucessfully.");

   ret = 0;

   INFO_D(_log_domain, "Finished starting.");
   ecore_main_loop_begin();
   INFO_D(_log_domain, "Closing application.");

   /* Shutdown procedure */
   eupnp_control_point_stop(c);
   eupnp_control_point_free(c);

   eupnp_cp_alloc_error:
      eina_log_domain_unregister(_log_domain);
   log_domain_reg_error:
      eupnp_ecore_shutdown();
   eupnp_ecore_init_error:
      eupnp_shutdown();

   return ret;
}
