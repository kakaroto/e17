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

#ifndef _EUPNP_SSDP_H
#define _EUPNP_SSDP_H

#include <Eina.h>

#include "Eupnp.h"
#include "eupnp_udp_transport.h"
#include "eupnp_core.h"

#define EUPNP_SSDP_ADDR "239.255.255.250"
#define EUPNP_SSDP_PORT 1900
#define EUPNP_SSDP_LOCAL_IFACE "0.0.0.0"

#define EUPNP_SSDP_MSEARCH_TEMPLATE "M-SEARCH * HTTP/1.1\r\n"     \
                                    "HOST: %s:%d\r\n"             \
                                    "MAN: \"ssdp:discover\"\r\n"  \
                                    "MX: %d\r\n"                  \
                                    "ST: %s\r\n\r\n"              \

#define EUPNP_SSDP_NOTIFY_ALIVE "ssdp:alive"
#define EUPNP_SSDP_NOTIFY_BYEBYE "ssdp:byebye"


/**
 * Shared strings, retrieve them with stringshare{ref|add}
 */
EAPI char *_eupnp_ssdp_notify;
EAPI char *_eupnp_ssdp_msearch;
EAPI char *_eupnp_ssdp_http_version;


/**
 * @typedef Eupnp_SSDP_Client
 *
 * SSDP client class.
 */
typedef struct _Eupnp_SSDP_Client Eupnp_SSDP_Client;

struct _Eupnp_SSDP_Client {
   Eupnp_UDP_Transport *udp_transport;

   /* Private */
   Eupnp_Fd_Handler socket_handler;
};


EAPI int                 eupnp_ssdp_init(void);
EAPI int                 eupnp_ssdp_shutdown(void);

EAPI Eupnp_SSDP_Client  *eupnp_ssdp_client_new(void);
EAPI void                eupnp_ssdp_client_free(Eupnp_SSDP_Client *c) EINA_ARG_NONNULL(1);
EAPI Eina_Bool           eupnp_ssdp_client_start(Eupnp_SSDP_Client *c) EINA_ARG_NONNULL(1);
EAPI Eina_Bool           eupnp_ssdp_client_stop(Eupnp_SSDP_Client *c) EINA_ARG_NONNULL(1);
EAPI Eina_Bool           eupnp_ssdp_discovery_request_send(Eupnp_SSDP_Client *c, int mx, const char *search_target) EINA_ARG_NONNULL(1,2,3);


#endif /* _EUPNP_SSDP_H */
