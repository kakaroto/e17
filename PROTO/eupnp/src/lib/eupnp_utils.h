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

#ifndef _EUPNP_UTILS_H
#define _EUPNP_UTILS_H

#include <Eina.h>

EAPI char       *current_date_http_string_get();
EAPI int         random_port_get();
EAPI const char *default_host_ip_get();

EAPI int         eupnp_utils_url_base_get(const char *url);
EAPI Eina_Bool   eupnp_utils_url_is_relative(const char *url);

EAPI double      eupnp_time_get(void);

#endif /* _EUPNP_UTILS_H */
