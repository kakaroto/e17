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

#ifndef _EUPNP_SERVICE_INFO_H
#define _EUPNP_SERVICE_INFO_H

#include <Eina.h>

typedef struct _Eupnp_Service_Info Eupnp_Service_Info;

struct _Eupnp_Service_Info {
   EINA_INLIST;

   const char *udn;
   const char *location;
   const char *service_type;
   const char *id;
   const char *control_URL;
   const char *scpd_URL;
   const char *eventsub_URL;

   /* Private */
   int refcount;
   void *_resource; /* Shared resource */
   void (*_resource_free)(void *resource); /* Resource free function */
};


EAPI int                   eupnp_service_info_init(void);
EAPI int                   eupnp_service_info_shutdown(void);

EAPI Eupnp_Service_Info   *eupnp_service_info_new(const char *udn, const char *location, const char *service_type, void *resource, void (*resource_free)(void *resource)) EINA_ARG_NONNULL(1,2,3,4);
EAPI void                  eupnp_service_info_free(Eupnp_Service_Info *d) EINA_ARG_NONNULL(1);
EAPI Eupnp_Service_Info   *eupnp_service_info_ref(Eupnp_Service_Info *service_info) EINA_ARG_NONNULL(1);
EAPI void                  eupnp_service_info_unref(Eupnp_Service_Info *service_info) EINA_ARG_NONNULL(1);
EAPI void                  eupnp_service_info_dump(const Eupnp_Service_Info *service_info) EINA_ARG_NONNULL(1);

#endif /* _EUPNP_SERVICE_INFO_H */
