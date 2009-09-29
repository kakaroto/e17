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

#ifndef _EUPNP_SOAP_H
#define _EUPNP_SOAP_H

#include <stdio.h>

#define SOAP_ENVELOPE_BEGIN "<?xml version=\"1.0\"?>"\
                            "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"" \
                            " s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
#define SOAP_ENVELOPE_END   "</s:Envelope>"
#define SOAP_BODY_BEGIN     "<s:Body>"
#define SOAP_BODY_END       "</s:Body>"


#endif /* _EUPNP_SOAP_H */
