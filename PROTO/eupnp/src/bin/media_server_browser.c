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
 * Description: application that searchs for media server devices. When a media
 * server device is located, the application then performs an action called
 * Browse on the server Content Directory service, which is virtually the same
 * as browsing a folder on any file manager system.
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <Eina.h>
#include "Eupnp.h"

#define MEDIA_SERVER_DEVICE_TYPE "urn:schemas-upnp-org:device:MediaServer:1"
#define CDS_SERVICE_TYPE "urn:schemas-upnp-org:service:ContentDirectory:1"

static _log_domain = -1;

/*
 * Receives the Browse action (see below) response. For the
 * ContentDirectory:Browse() action, we expect a DIDL formatted XML containing
 * the response.
 *
 * @note To see this XML response, turn on debug as taught below before main().
 */
static void
action_response(void *data, Eina_Inlist *evented_vars)
{
   INFO_D(_log_domain, "Browse response. %p", evented_vars);

   Eupnp_Service_Action_Argument *arg;

   EINA_INLIST_FOREACH(evented_vars, arg)
     INFO_D(_log_domain, "%s: %s", arg->name, arg->value);
}


/*
 * Receives a service proxy of a ContentDirectory service (available on
 * Media Server devices). In this example we do the first Browse call, which
 * browses the media server root directory. The action is sent asynchronously
 * and the response is forwarded to the assigned action_response() callback.
 */
static void
on_proxy_ready(void *data, Eupnp_Service_Proxy *proxy)
{
   if (!eupnp_service_proxy_action_send(proxy, "Browse", EUPNP_ACTION_RESPONSE_CB(action_response),
					NULL, // data
					"ObjectID", EUPNP_TYPE_STRING, "0",
					"BrowseFlag", EUPNP_TYPE_STRING, "BrowseDirectChildren",
					"Filter", EUPNP_TYPE_STRING, "",
					"StartingIndex", EUPNP_TYPE_INT, 0,
					"RequestedCount", EUPNP_TYPE_INT, 25,
					"SortCriteria", EUPNP_TYPE_STRING, "dc:title",
					NULL))
      ERROR_D(_log_domain, "Failed to send proxy action.");
}

/*
 * Called when a device is ready to be used. If the device contains a
 * ContentDirectory service, a proxy is requested and this proxy is forwarded to
 * the on_proxy_ready() callback.
 */
static Eina_Bool
on_device_ready(void *user_data, Eupnp_Event_Type event_type, void *event_data)
{
   Eupnp_Device_Info *d = event_data;
   Eupnp_Service_Info *service;

   // Request proxies for Content Directory services, if present
   EINA_INLIST_FOREACH(d->services, service)
	if (!strcmp(service->service_type, CDS_SERVICE_TYPE))
	   eupnp_service_proxy_new(service, EUPNP_SERVICE_PROXY_READY_CB(on_proxy_ready), NULL);

   return EINA_TRUE;
}

/*
 * Run "EINA_LOG_LEVEL=0 EINA_LOG_LEVELS=MediaServerBrowser:5 ./media_server_browser"
 * for watching only application log messages.
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

   if ((_log_domain = eina_log_domain_register("MediaServerBrowser", EINA_COLOR_BLUE)) < 0)
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
   eupnp_event_bus_subscribe(EUPNP_EVENT_DEVICE_READY,
			     EUPNP_CALLBACK(on_device_ready), NULL);

   /* Start control point */
   eupnp_control_point_start(c);

   /* Send a test search for media server devices */
   if (!eupnp_control_point_discovery_request_send(c, 5, MEDIA_SERVER_DEVICE_TYPE))
     {
	ERROR_D(_log_domain, "Failed to perform MSearch.");
     }
   else
	INFO_D(_log_domain, "MSearch sent sucessfully.");

   ret = 0;

   INFO_D(_log_domain, "Finished starting media server browser.");
   ecore_main_loop_begin();
   INFO_D(_log_domain, "Closing application.");

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

