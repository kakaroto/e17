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
#include <string.h>
#include <Eina.h>

#include "eupnp_log.h"
#include "eupnp_service_info.h"
#include "eupnp_private.h"

/*
 * Private API
 */

static int _eupnp_service_info_main_count = 0;
static int _log_dom = -1;

/*
 * Public API
 */

/*
 * Initializes the service info module.
 *
 * @return On error, returns 0. Otherwise, returns the number of times it's been
 * called.
 */
int
eupnp_service_info_init(void)
{
   if (_eupnp_service_info_main_count)
      return ++_eupnp_service_info_main_count;

   if (!eupnp_log_init())
     {
	fprintf(stderr, "Could not initialize eupnp error module.\n");
	return 0;
     }

   if ((_log_dom = eina_log_domain_register("Eupnp.ServiceInfo", EINA_COLOR_BLUE)) < 0)
     {
	ERROR("Failed to create logging domain for service info module.\n");
	eupnp_log_shutdown();
	return 0;
     }

   INFO_D(_log_dom, "Initializing service info module.\n");

   return ++_eupnp_service_info_main_count;

   return 0;
}

/*
 * Shuts down the service info module.
 *
 * @return 0 if completely shutted down the module.
 */
int
eupnp_service_info_shutdown(void)
{
   if (_eupnp_service_info_main_count != 1)
      return --_eupnp_service_info_main_count;

   INFO_D(_log_dom, "Shutting down service info module.\n");

   eina_log_domain_unregister(_log_dom);
   eupnp_log_shutdown();

   return --_eupnp_service_info_main_count;
}

/*
 * Constructor for the Eupnp_Service_Info class.
 *
 * @param udn service's udn
 * @param location service's location. For UPnP this is the base URL, for SSDP
 *                 this is the service's location.
 * @param resource Resource from which the other service attributes (e.g. udn,
 *                 location) depend on.
 * @param resource_free Function used to free the resource
 *
 * @return On success, a Eupnp_Service_Info instance. Otherwise, NULL.
 */
Eupnp_Service_Info *
eupnp_service_info_new(const char *udn, const char *location, const char *service_type, void *resource, void (* resource_free)(void *resource))
{
   /* FIXME Location can point to a root device XML (upnp) or just to the
    * service's location (ssdp).
    *
    * In the first case, this service will be published to the user, who might
    * request for a proxy. For retrieving this proxy, we need both base_URL and
    * control_URL - the first we already have, but the latter is only defined
    * inside the root device's XML, so we need to fetch this XML and find this
    * service's control_URL.
    * TODO 1 or 2 below
    *   1. Write a simple and fast parser for retrieving this control_URL
    *   2. Adapt eupnp_device_parser.c to implement feature specified on item 1
    *
    * In the second case (SSDP), there's no control URL and we're fine by just
    * announcing it.
    * TODO Add a global app option for skipping/disabling UPnP stuff and working
    *      only on SSDP mode.
    */

   CHECK_NULL_RET_VAL(udn, NULL);
   Eupnp_Service_Info *d;

   d = malloc(sizeof(Eupnp_Service_Info));

   if (!d)
     {
	ERROR_D(_log_dom, "Failed to allocate memory for service info\n");
	if (resource && resource_free)
	  resource_free(resource);
	return NULL;
     }

   d->udn = udn;
   d->location = location;
   d->service_type = service_type;
   d->id = NULL;
   d->control_URL = NULL;
   d->scpd_URL = NULL;
   d->eventsub_URL = NULL;
   d->refcount = 1;
   d->_resource = resource;
   d->_resource_free = resource_free;

   DEBUG_D(_log_dom, "Created new service %p\n", d);

   return d;
}

/*
 * Destructor for the Eupnp_Service_Info class.
 *
 * @param d Eupnp_Service_Info instance
 */
void
eupnp_service_info_free(Eupnp_Service_Info *d)
{
   /*
    * UDN, location and service_type are filled by two possible external
    * resources:
    *
    * (1) A device info
    * (2) A HTTP message
    *
    * In case (1), the device info object is the parent and is
    * responsible for freeing the children service. In case (2), the service
    * does not have a parent device object, and since the HTTP message is
    * considered a children of the service, it must be freed by the service.
    *
    * That said, only free service_type and location if the resource or the
    * resource_free function are NULL, which means case (1) where we manually
    * fill these members.
    */
   CHECK_NULL_RET(d);
   DEBUG_D(_log_dom, "Freeing service info %p\n", d);

   free((char *)d->id);
   free((char *)d->control_URL);
   free((char *)d->scpd_URL);
   free((char *)d->eventsub_URL);

   /* Free the shared resource */
   if ((d->_resource) && (d->_resource_free))
      d->_resource_free(d->_resource);
   else
     {
	free((char *)d->service_type);
     }

   free(d);
}

/*
 * Makes a reference to a Eupnp_Service_Info instance.
 *
 * @param service_info Instance to get the reference from
 *
 * @return The reference to the object
 */
Eupnp_Service_Info *
eupnp_service_info_ref(Eupnp_Service_Info *service_info)
{
   CHECK_NULL_RET_VAL(service_info, NULL);

   DEBUG_D(_log_dom, "Service %p refcount %d -> %d\n", service_info, service_info->refcount, service_info->refcount + 1);

   service_info->refcount++;

   return service_info;
}

/*
 * Removes a reference to a Eupnp_Service_Info instance.
 *
 * @param service_info Instance to dereference
 */
void
eupnp_service_info_unref(Eupnp_Service_Info *service_info)
{
   CHECK_NULL_RET(service_info);

   if (service_info->refcount < 1)
     {
	// Someone messed up refcounting
	return;
     }

   DEBUG_D(_log_dom, "Service %p refcount %d -> %d\n", service_info, service_info->refcount, service_info->refcount - 1);

   service_info->refcount--;

   if (!service_info->refcount)
      eupnp_service_info_free(service_info);
}

void
eupnp_service_info_dump(Eupnp_Service_Info *service_info)
{
   CHECK_NULL_RET(service_info);

   INFO_D(_log_dom, "\tService dump\n");
   INFO_D(_log_dom, "\t\tUDN: %s\n", service_info->udn);
   INFO_D(_log_dom, "\t\tlocation: %s\n", service_info->location);
   INFO_D(_log_dom, "\t\ttype: %s\n", service_info->service_type);
   INFO_D(_log_dom, "\t\tid: %s\n", service_info->id);
   INFO_D(_log_dom, "\t\tcontrol URL: %s\n", service_info->control_URL);
   INFO_D(_log_dom, "\t\tscpd URL: %s\n", service_info->scpd_URL);
   INFO_D(_log_dom, "\t\teventsub URL: %s\n", service_info->eventsub_URL);
}
