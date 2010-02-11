/** Eupnp - UPnP library
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
#include <string.h>

#include "Eupnp.h"
#include "eupnp_log.h"
#include "eupnp_device_info.h"
#include "eupnp_service_info.h"
#include "eupnp_service_proxy.h"
#include "eupnp_event_bus.h"
#include "eupnp_private.h"

/**
 * Private API
 */

static int _log_dom = -1;
extern int EUPNP_ERROR_DEVICE_PARSER_INSUFFICIENT_FEED;

static void
eupnp_device_info_data_ready(void *buffer, int size, void *data)
{
   DEBUG_D(_log_dom, "Device data ready callback for device %p", data);
   Eupnp_Device_Info *device_info = data;
   eupnp_device_info_ref(device_info);

   if (eupnp_device_parse_xml_buffer(buffer, size, device_info))
     {
	DEBUG_D(_log_dom, "Parsed XML successfully at buffer %p for device info %p", buffer, device_info);

	if (!device_info->xml_parser)
	  {
	     DEBUG_D(_log_dom, "Finished building device %p. Publishing event...", data);
	     eupnp_event_bus_publish(EUPNP_EVENT_DEVICE_READY, device_info);
	  }
     }
   else
     {
	if (eina_error_get() == EUPNP_ERROR_DEVICE_PARSER_INSUFFICIENT_FEED)
	  {
	     // TODO treat size < 4 case
	     WARN_D(_log_dom, "Len < 4 case.");
	  }

	ERROR_D(_log_dom, "Failed to parse XML at buffer %p for device info %p", buffer, device_info);
     }

   eupnp_device_info_unref(device_info);
}

static void
eupnp_device_info_download_completed(Eupnp_Request *request, void *data, const Eupnp_HTTP_Request *req)
{
   DEBUG_D(_log_dom, "Finished building device %p callback called", data);
   eupnp_device_info_unref(data);
   eupnp_core_http_request_free(request);
}

static void
eupnp_device_icon_free(Eupnp_Device_Icon *icon)
{
   CHECK_NULL_RET(icon);

   free((char *)icon->url);
   free((char *)icon->mimetype);
   free(icon);
}

static void
eupnp_device_info_icon_list_clear(Eupnp_Device_Info *d)
{
   Eupnp_Device_Icon *icon;

   while (icon = (void*)d->icons)
     {
	d->icons = eina_inlist_remove(d->icons, d->icons);
	eupnp_device_icon_free(icon);
     }
}

static void
eupnp_device_icon_dump(Eupnp_Device_Icon *icon)
{
   CHECK_NULL_RET(icon);

   INFO_D(_log_dom, "\tIcon dump");
   INFO_D(_log_dom, "\t\tMimetype: %s", icon->mimetype);
   INFO_D(_log_dom, "\t\tURL: %s", icon->url);
   INFO_D(_log_dom, "\t\twidth: %d", icon->width);
   INFO_D(_log_dom, "\t\theight: %d", icon->height);
   INFO_D(_log_dom, "\t\tdepth: %d", icon->depth);
}

static void
eupnp_device_info_service_list_clear(Eupnp_Device_Info *d)
{
   Eupnp_Service_Info *service;

   while (service = (void*)d->services)
     {
	d->services = eina_inlist_remove(d->services, d->services);
	eupnp_service_info_unref(service);
     }
}

static void
eupnp_device_info_embedded_device_list_clear(Eupnp_Device_Info *d)
{
   Eupnp_Device_Info *device;

   while (device = (void *)d->embedded_devices)
     {
	d->embedded_devices = eina_inlist_remove(d->embedded_devices,
						 d->embedded_devices);
	eupnp_device_info_unref(device);
     }
}

/**
 * Initializes the device info module.
 *
 * @return On error, returns 0. Otherwise, returns the number of times the
 * module has been initialized.
 */
Eina_Bool
eupnp_device_info_init(void)
{
   if ((_log_dom = eina_log_domain_register("Eupnp.DeviceInfo", EINA_COLOR_BLUE)) < 0)
     {
	ERROR("Failed to create logging domain for DeviceInfo module.");
	return EINA_FALSE;
     }

   INFO_D(_log_dom, "Initializing device info module.");

   return EINA_TRUE;
}

/**
 * Shuts down the device info module.
 *
 * @return 0 if completely shutted down the module.
 */
Eina_Bool
eupnp_device_info_shutdown(void)
{
   INFO_D(_log_dom, "Shutting down device info module.");
   eina_log_domain_unregister(_log_dom);
   return EINA_TRUE;
}

/**
 * Constructor for the Eupnp_Device_Info class.
 *
 * @param udn Device's udn
 * @param location Device's location
 * @param resource Resource from which the other device attributes (e.g. udn,
 *                 location) depend on.
 * @param resource_free Function used to free the resource
 *
 * @return On success, a Eupnp_Device_Info instance. Otherwise, NULL.
 */
EAPI Eupnp_Device_Info *
eupnp_device_info_new(const char *udn, const char *location, void *resource, void (* resource_free)(void *resource))
{
   Eupnp_Device_Info *d;

   d = calloc(1, sizeof(Eupnp_Device_Info));

   if (!d)
     {
	ERROR_D(_log_dom, "Failed to allocate memory for device info");

	if (resource && resource_free)
	   resource_free(resource);

	return NULL;
     }

   d->udn = udn;
   d->location = location;
   d->refcount = 1;
   d->_resource = resource;
   d->_resource_free = resource_free;

   return d;
}

/**
 * Destructor for the Eupnp_Device_Info class.
 *
 * @param d Eupnp_Device_Info instance
 */
EAPI void
eupnp_device_info_free(Eupnp_Device_Info *d)
{
   CHECK_NULL_RET(d);

   free((char *)d->device_type);
   free((char *)d->friendly_name);
   free((char *)d->base_URL);
   free((char *)d->manufacturer);
   free((char *)d->manufacturer_URL);
   free((char *)d->model_description);
   free((char *)d->model_name);
   free((char *)d->model_number);
   free((char *)d->model_URL);
   free((char *)d->serial_number);
   free((char *)d->upc);
   free((char *)d->presentation_URL);

   if (d->icons) eupnp_device_info_icon_list_clear(d);
   if (d->services) eupnp_device_info_service_list_clear(d);
   if (d->embedded_devices) eupnp_device_info_embedded_device_list_clear(d);

   /** Free the shared resource */
   if ((d->_resource) && (d->_resource_free))
      d->_resource_free(d->_resource);
   else
     {
	// Embedded device, no shared resources.
	free((char *)d->udn);
	free((char *)d->location);
     }

   free(d->xml_parser);
   free(d);
}

/**
 * Makes a reference to a Eupnp_Device_Info instance.
 *
 * @param device_info Instance to get the reference from
 *
 * @return The reference to the object
 */
EAPI Eupnp_Device_Info *
eupnp_device_info_ref(Eupnp_Device_Info *device_info)
{
   CHECK_NULL_RET_VAL(device_info, NULL);

   DEBUG_D(_log_dom, "Device %p refcount %d -> %d", device_info, device_info->refcount, device_info->refcount + 1);

   device_info->refcount++;

   return device_info;
}

/**
 * Removes a reference to a Eupnp_Device_Info instance.
 *
 * @param device_info Instance to dereference
 */
EAPI void
eupnp_device_info_unref(Eupnp_Device_Info *device_info)
{
   CHECK_NULL_RET(device_info);

   if (device_info->refcount < 1)
     {
	// Someone messed up refcounting
	return;
     }

   DEBUG_D(_log_dom, "Device %p refcount %d -> %d", device_info, device_info->refcount, device_info->refcount - 1);

   device_info->refcount--;

   if (!device_info->refcount)
      eupnp_device_info_free(device_info);
}

EAPI void
eupnp_device_info_icon_add(Eupnp_Device_Info *device_info, Eupnp_Device_Icon *icon)
{
   CHECK_NULL_RET(device_info);
   CHECK_NULL_RET(icon);

   device_info->icons = eina_inlist_append(device_info->icons, EINA_INLIST_GET(icon));
}

EAPI void
eupnp_device_info_service_add(Eupnp_Device_Info *device_info, Eupnp_Service_Info *service)
{
   CHECK_NULL_RET(device_info);
   CHECK_NULL_RET(service);

   eupnp_service_info_ref(service);
   device_info->services = eina_inlist_append(device_info->services, EINA_INLIST_GET(service));
}

EAPI const Eupnp_Service_Info *
eupnp_device_info_service_get_by_type(const Eupnp_Device_Info *device_info, const char *service_type)
{
   CHECK_NULL_RET_VAL(device_info, NULL);
   CHECK_NULL_RET_VAL(service_type, NULL);

   Eupnp_Service_Info *s;

   EINA_INLIST_FOREACH(device_info->services, s)
     {
	if (!strcmp(s->service_type, service_type))
	   return (const Eupnp_Service_Info *)s;
     }

   return NULL;
}

EAPI void
eupnp_device_info_device_add(Eupnp_Device_Info *device_info, Eupnp_Device_Info *device)
{
   CHECK_NULL_RET(device_info);
   CHECK_NULL_RET(device);

   eupnp_device_info_ref(device);
   device_info->embedded_devices = eina_inlist_append(device_info->embedded_devices, EINA_INLIST_GET(device));
}

EAPI void
eupnp_device_info_fetch(Eupnp_Device_Info *device_info)
{
   CHECK_NULL_RET(device_info);
   CHECK_NULL_RET(device_info->location);

   DEBUG_D(_log_dom, "Fetching device %p info from %s", device_info, device_info->location);

   eupnp_device_info_ref(device_info);

   if (!eupnp_core_http_request_send(device_info->location, "GET", NULL, NULL, 0,
				     NULL, EUPNP_REQUEST_DATA_CB(eupnp_device_info_data_ready),
				     EUPNP_REQUEST_COMPLETED_CB(eupnp_device_info_download_completed),
				     device_info))
      ERROR_D(_log_dom, "Could not add a new download job for device %p", device_info);
}

EAPI void
eupnp_device_info_dump(const Eupnp_Device_Info *device_info)
{
   CHECK_NULL_RET(device_info);

   Eupnp_Device_Icon *icon;
   Eupnp_Service_Info *service;
   Eupnp_Device_Info *device;

   INFO_D(_log_dom, "Device %p dump:", device_info);
   INFO_D(_log_dom, "\tname: %s", device_info->friendly_name);
   INFO_D(_log_dom, "\tudn: %s", device_info->udn);
   INFO_D(_log_dom, "\ttype: %s", device_info->device_type);
   INFO_D(_log_dom, "\tlocation: %s", device_info->location);
   INFO_D(_log_dom, "\tbase URL: %s", device_info->base_URL);
   INFO_D(_log_dom, "\tspec: %d.%d", device_info->spec_version_major,
	device_info->spec_version_minor);
   INFO_D(_log_dom, "\tupc: %s", device_info->upc);
   INFO_D(_log_dom, "\tpresentation URL: %s", device_info->presentation_URL);
   INFO_D(_log_dom, "\tmanufacturer: %s", device_info->manufacturer);
   INFO_D(_log_dom, "\tmanufacturer URL: %s", device_info->manufacturer_URL);
   INFO_D(_log_dom, "\tmodel name: %s", device_info->model_name);
   INFO_D(_log_dom, "\tmodel description: %s", device_info->model_description);
   INFO_D(_log_dom, "\tmodel URL: %s", device_info->model_URL);
   INFO_D(_log_dom, "\tserial number: %s", device_info->serial_number);

   if (device_info->icons)
     EINA_INLIST_FOREACH(device_info->icons, icon)
        eupnp_device_icon_dump(icon);

   if (device_info->services)
     EINA_INLIST_FOREACH(device_info->services, service)
        eupnp_service_info_dump(service);

   if (device_info->embedded_devices)
     EINA_INLIST_FOREACH(device_info->embedded_devices, device)
        eupnp_device_info_dump(device);
}
