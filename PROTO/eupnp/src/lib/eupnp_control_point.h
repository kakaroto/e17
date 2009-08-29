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

#ifndef _EUPNP_CONTROL_POINT_H
#define _EUPNP_CONTROL_POINT_H

#include <Eina.h>
#include <eupnp_ssdp.h>

/*
 * Generic Search Targets (ST's)
 */
#define EUPNP_ST_SSDP_ALL "ssdp:all"
#define EUPNP_ST_UPNP_ROOTDEVICE "upnp:rootdevice"


typedef struct _Eupnp_Control_Point Eupnp_Control_Point;


struct _Eupnp_Control_Point {
   Eupnp_SSDP_Server *ssdp_server;
};

int                  eupnp_control_point_init(void);
int                  eupnp_control_point_shutdown(void);

Eupnp_Control_Point *eupnp_control_point_new(void);
void                 eupnp_control_point_free(Eupnp_Control_Point *c) EINA_ARG_NONNULL(1);
Eina_Bool            eupnp_control_point_start(Eupnp_Control_Point *c) EINA_ARG_NONNULL(1);
Eina_Bool            eupnp_control_point_stop(Eupnp_Control_Point *c) EINA_ARG_NONNULL(1);
Eina_Bool            eupnp_control_point_discovery_request_send(Eupnp_Control_Point *c, int mx, const char *search_target) EINA_ARG_NONNULL(1,2,3);
int                  eupnp_control_point_ssdp_socket_get(Eupnp_Control_Point *c);


#endif /* _EUPNP_CONTROL_POINT_H */
