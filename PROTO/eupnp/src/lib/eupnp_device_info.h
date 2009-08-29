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

#ifndef _EUPNP_DEVICE_INFO_H
#define _EUPNP_DEVICE_INFO_H

#include <Eina.h>

#include "eupnp_service_info.h"

typedef struct _Eupnp_Device_Info Eupnp_Device_Info;
typedef struct _Eupnp_Device_Icon Eupnp_Device_Icon;

struct _Eupnp_Device_Info {
   EINA_INLIST;

   const char *udn;
   const char *location;
   const char *base_URL;
   const char *device_type;
   const char *friendly_name;
   const char *manufacturer;
   const char *manufacturer_URL;
   const char *model_description;
   const char *model_name;
   const char *model_number;
   const char *model_URL;
   const char *serial_number;
   const char *upc;
   const char *presentation_URL;
   int spec_version_major;
   int spec_version_minor;

   Eina_Inlist *icons; /* List of Eupnp_Device_Icon */
   Eina_Inlist *services; /* List of Eupnp_Service_Info */
   Eina_Inlist *embedded_devices; /* List of Eupnp_Device_Info */

   /* Private */
   void *xml_parser;
   int refcount;
   void *_resource; /* Shared resource */
   void (*_resource_free)(void *resource); /* Resource free function */
};

struct _Eupnp_Device_Icon {
   EINA_INLIST;

   int width;
   int height;
   int depth;
   const char *url;
   const char *mimetype;
};


int                   eupnp_device_info_init(void);
int                   eupnp_device_info_shutdown(void);

Eupnp_Device_Info    *eupnp_device_info_new(const char *udn, const char *location, void *resource, void (*resource_free)(void *resource)) EINA_ARG_NONNULL(1,2,3,4);
void                  eupnp_device_info_free(Eupnp_Device_Info *d) EINA_ARG_NONNULL(1);
Eupnp_Device_Info    *eupnp_device_info_ref(Eupnp_Device_Info *device_info) EINA_ARG_NONNULL(1);
void                  eupnp_device_info_unref(Eupnp_Device_Info *device_info) EINA_ARG_NONNULL(1);
void                  eupnp_device_info_fetch(Eupnp_Device_Info *device_info) EINA_ARG_NONNULL(1);
void                  eupnp_device_info_icon_add(Eupnp_Device_Info *device_info, Eupnp_Device_Icon *icon) EINA_ARG_NONNULL(1,2);
void                  eupnp_device_info_service_add(Eupnp_Device_Info *device_info, Eupnp_Service_Info *service) EINA_ARG_NONNULL(1,2);
void                  eupnp_device_info_device_add(Eupnp_Device_Info *device, Eupnp_Device_Info *new) EINA_ARG_NONNULL(1,2);
void                  eupnp_device_info_dump(Eupnp_Device_Info *device_info) EINA_ARG_NONNULL(1);

#endif /* _EUPNP_DEVICE_INFO_H */
