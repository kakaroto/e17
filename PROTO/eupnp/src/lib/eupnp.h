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

#ifndef _EUPNP_H
#define _EUPNP_H

#include "eupnp_core.h"
#include "eupnp_udp_transport.h"
#include "eupnp_http_message.h"
#include "eupnp_utils.h"
#include "eupnp_log.h"
#include "eupnp_event_bus.h"
#include "eupnp_ssdp.h"
#include "eupnp_control_point.h"
#include "eupnp_service_proxy.h"
#include "eupnp_service_info.h"
#include "eupnp_device_info.h"

int eupnp_init(void);
int eupnp_shutdown(void);

#endif /* _EUPNP_H */
