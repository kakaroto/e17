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

#ifndef _EUPNP_EVENT_SERVER_H
#define _EUPNP_EVENT_SERVER_H

#include <Eina.h>

#include "Eupnp.h"
#include "eupnp_event_bus.h"


EAPI int   eupnp_event_server_init(void);
EAPI int   eupnp_event_server_shutdown(void);

EAPI const char *eupnp_event_server_url_get();
EAPI int         eupnp_event_server_request_subscribe(Eupnp_Callback cb, void *data);

#endif /* _EUPNP_EVENT_SERVER_H */
