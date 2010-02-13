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

#include "eupnp_private.h"

/**
 * @addtogroup Eupnp_Control_Point_Module Control Point
 *
 */


/**
 * Private API
 */

static int _log_dom = -1; // Logging domain
static Eupnp_Subscriber *_device_found = NULL;

/**
 * Subscribed function for DEVICE_FOUND events.
 *
 * Starts the process of fetching
 */
static Eina_Bool
_on_device_found(void *user_data, Eupnp_Event_Type type, void *event_data)
{
   // TODO add an option for skipping this device mount
   Eupnp_Device_Info *device_info = event_data;
   eupnp_device_info_fetch(eupnp_device_info_ref(device_info));
   eupnp_device_info_unref(device_info);
}

/*
 * Public API
 */

/**
 * Initializes the control point module.
 *
 * @return On error, returns 0. Otherwise, returns the number of times it's been
 * called.
 */
Eina_Bool
eupnp_control_point_init(void)
{
   if ((_log_dom = eina_log_domain_register("Eupnp.ControlPoint", EINA_COLOR_BLUE)) < 0)
     {
	fprintf(stderr, "Failed to create control point logging domain.\n");
	return EINA_FALSE;
     }

   // TODO add a option for skipping this device mount
   if (!(_device_found = eupnp_event_bus_subscribe(EUPNP_EVENT_DEVICE_FOUND,
						   EUPNP_CALLBACK(_on_device_found),
						   NULL)))
     {
	ERROR("Failed to subscribe to device found event.");
	goto subscribe_error;
     }

   INF("Initializing control point module.");
   return EINA_TRUE;

   subscribe_error:
	eina_log_domain_unregister(_log_dom);
	return EINA_FALSE;
}

/**
 * Shuts down the control point
 *
 * @return 0 if completely shutted down the module.
 */
Eina_Bool
eupnp_control_point_shutdown(void)
{
   INF("Shutting down control point module.");
   eupnp_event_bus_unsubscribe(_device_found);
   eina_log_domain_unregister(_log_dom);
   return EINA_FALSE;
}

/**
 * Constructor for the Eupnp_Control_Point class.
 *
 * @return A new Eupnp_Control_Point instance. On error, returns NULL.
 */
EAPI Eupnp_Control_Point *
eupnp_control_point_new(void)
{
   Eupnp_Control_Point *c;

   c = malloc(sizeof(Eupnp_Control_Point));

   if (!c)
     {
	eina_error_set(EINA_ERROR_OUT_OF_MEMORY);
	ERR("Could not allocate a new control point.");
	return NULL;
     }

   c->ssdp_client = eupnp_ssdp_client_new();

   if (!c->ssdp_client)
     {
	ERR("Could not create a ssdp instance for the control point.");
	free(c);
	return NULL;
     }

   return c;
}

/**
 * Destructor for the Eupnp_Control_Point class.
 *
 * @param c Eupnp_Control_Point instance to be destroyed.
 */
EAPI void
eupnp_control_point_free(Eupnp_Control_Point *c)
{
   CHECK_NULL_RET(c);
   DBG("Freeing control point %p", c);
   if (c->ssdp_client) eupnp_ssdp_client_free(c->ssdp_client);
   free(c);
}

/**
 * Starts the control point.
 *
 * @param c Eupnp_Control_Point instance.
 */
EAPI Eina_Bool
eupnp_control_point_start(Eupnp_Control_Point *c)
{
   CHECK_NULL_RET_VAL(c, EINA_FALSE);
   DBG("Starting control point %p", c);
   eupnp_ssdp_client_start(c->ssdp_client);
   return EINA_TRUE;
}

/**
 * Stops the control point.
 *
 * @param c Eupnp_Control_Point instance.
 */
EAPI Eina_Bool
eupnp_control_point_stop(Eupnp_Control_Point *c)
{
   CHECK_NULL_RET_VAL(c, EINA_FALSE);
   DBG("Stopping control point %p", c);
   eupnp_ssdp_client_stop(c->ssdp_client);
   return EINA_TRUE;
}

/**
 * Sends a discovery message to the network. (a.k.a. MSearch)
 *
 * @param c Control point.
 * @param mx maximum wait time in seconds for devices to wait before answering
 *           the search message.
 * @param search_target target for the search. Common values are "ssdp:all",
 *                      "upnp:rootdevice", and so on (refer to the UPnP device
 *                      architecture document for more).
 *
 * @return On success EINA_TRUE, EINA_FALSE on error.
 */
EAPI Eina_Bool
eupnp_control_point_discovery_request_send(Eupnp_Control_Point *c, int mx, const char *search_target)
{
   DBG("Discovery request sent for target %s with mx %d", search_target, mx);
   return eupnp_ssdp_discovery_request_send(c->ssdp_client, mx, search_target);
}
