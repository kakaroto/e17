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

#include <Eina.h>

#include "Eupnp.h"
#include "eupnp_private.h"

/**
 * Private API
 */

static int _log_dom = -1;
#undef DBG
#undef INF
#undef WRN
#undef ERR
#undef CRIT
#define DBG(...) EINA_LOG_DOM_DBG(_log_dom, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_log_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_ERR(_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_log_dom, __VA_ARGS__)
#define CRIT(...) EINA_LOG_DOM_CRIT(_log_dom, __VA_ARGS__)

/*
 * Public API
 */

/**
 * Initializes the service info module.
 *
 * @return On error, returns 0. Otherwise, returns the number of times it's been
 * called.
 */
Eina_Bool
eupnp_service_info_init(void)
{
   if ((_log_dom =
	eina_log_domain_register("Eupnp.ServiceInfo", EINA_COLOR_BLUE)) < 0)
     {
	EINA_LOG_DOM_ERR
	  (EUPNP_LOGGING_DOM_GLOBAL,
	   "Failed to create logging domain for service info module.");
	return EINA_FALSE;
     }

   INF("Initializing service info module.");
   return EINA_TRUE;
}

/**
 * Shuts down the service info module.
 *
 * @return 0 if completely shutted down the module.
 */
Eina_Bool
eupnp_service_info_shutdown(void)
{
   INF("Shutting down service info module.");
   eina_log_domain_unregister(_log_dom);
   return EINA_TRUE;
}

/**
 * Constructor for the Eupnp_Service_Info class.
 *
 * @param udn service's udn
 * @param location service's location. For UPnP this is the base URL, for SSDP
 *                 this is the service's location.
 * @param service_type service's type
 * @param resource Resource from which the other service attributes (e.g. udn,
 *                 location) depend on.
 * @param resource_free Function used to free the resource
 *
 * @return On success, a Eupnp_Service_Info instance. Otherwise, NULL.
 */
EAPI Eupnp_Service_Info *
eupnp_service_info_new(const char *udn, const char *location, const char *service_type, void *resource, void (* resource_free)(void *resource))
{
   /* FIXME Location can point to a root device XML (upnp) or just to the
    * service's location (ssdp).
    *
    * In the first case, this service will be published to the user, who might
    * request for a proxy. For retrieving this proxy, we need both base_URL and
    * control_url - the first we already have, but the latter is only defined
    * inside the root device's XML, so we need to fetch this XML and find this
    * service's control_url.
    * TODO 1 or 2 below
    *   1. Write a simple and fast parser for retrieving this control_url
    *   2. Adapt eupnp_device_parser.c to implement feature specified on item 1
    *
    * In the second case (SSDP), there's no control URL and we're fine by just
    * announcing it.
    * TODO Add a global app option for skipping/disabling UPnP stuff and working
    *      only on SSDP mode.
    */

   CHECK_NULL_RET(udn, NULL);
   Eupnp_Service_Info *d;

   d = calloc(1, sizeof(Eupnp_Service_Info));

   if (!d)
     {
	ERR("Failed to allocate memory for service info");
	if (resource && resource_free)
	  resource_free(resource);
	return NULL;
     }

   d->udn = udn;
   d->location = location;
   d->service_type = service_type;
   d->refcount = 1;
   d->_resource = resource;
   d->_resource_free = resource_free;

   DBG("Created new service %p", d);

   return d;
}

/**
 * Destructor for the Eupnp_Service_Info class.
 *
 * @param d Eupnp_Service_Info instance
 */
EAPI void
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
   DBG("Freeing service info %p", d);

   free((char *)d->id);
   free((char *)d->control_url);
   free((char *)d->scpd_url);
   free((char *)d->eventsub_url);

   /* Free the shared resource */
   if ((d->_resource) && (d->_resource_free))
      d->_resource_free(d->_resource);
   else
     {
	free((char *)d->service_type);
     }

   free(d);
}

/**
 * Makes a reference to a Eupnp_Service_Info instance.
 *
 * @param service_info Instance to get the reference from
 *
 * @return The reference to the object
 */
EAPI Eupnp_Service_Info *
eupnp_service_info_ref(Eupnp_Service_Info *service_info)
{
   CHECK_NULL_RET(service_info, NULL);

   DBG("Service %p refcount %d -> %d", service_info, service_info->refcount, service_info->refcount + 1);

   service_info->refcount++;

   return service_info;
}

/**
 * Removes a reference to a Eupnp_Service_Info instance.
 *
 * @param service_info Instance to dereference
 */
EAPI void
eupnp_service_info_unref(Eupnp_Service_Info *service_info)
{
   CHECK_NULL_RET(service_info);

   if (service_info->refcount < 1)
     {
	// Someone messed up refcounting
	return;
     }

   DBG("Service %p refcount %d -> %d", service_info, service_info->refcount, service_info->refcount - 1);

   service_info->refcount--;

   if (!service_info->refcount)
      eupnp_service_info_free(service_info);
}

EAPI void
eupnp_service_info_dump(const Eupnp_Service_Info *service_info)
{
   CHECK_NULL_RET(service_info);

   INF("\tService dump");
   INF("\t\tUDN: %s", service_info->udn);
   INF("\t\tlocation: %s", service_info->location);
   INF("\t\ttype: %s", service_info->service_type);
   INF("\t\tid: %s", service_info->id);
   INF("\t\tcontrol URL: %s", service_info->control_url);
   INF("\t\tscpd URL: %s", service_info->scpd_url);
   INF("\t\teventsub URL: %s", service_info->eventsub_url);
}

