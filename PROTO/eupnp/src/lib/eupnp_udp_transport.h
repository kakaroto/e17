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

#include <Eina.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef _EUPNP_UDP_TRANSPORT_H
#define _EUPNP_UDP_TRANSPORT_H


#define EUPNP_UDP_PACKET_LEN 5000

typedef struct _Eupnp_UDP_Transport Eupnp_UDP_Transport;
typedef struct _Eupnp_UDP_Datagram Eupnp_UDP_Datagram;


struct _Eupnp_UDP_Transport {
   int socket;
   struct sockaddr_in in_addr;
   struct ip_mreq mreq;
   socklen_t in_addr_len;
   socklen_t mreq_len;
   socklen_t host_len;
};


struct _Eupnp_UDP_Datagram {
   char *data;
   const char *host;
   int port;
};


Eupnp_UDP_Transport   *eupnp_udp_transport_new(const char *addr, int port, const char *iface_addr) EINA_ARG_NONNULL(1,2,3);
int                    eupnp_udp_transport_close(Eupnp_UDP_Transport *s) EINA_ARG_NONNULL(1);
void                   eupnp_udp_transport_free(Eupnp_UDP_Transport *s) EINA_ARG_NONNULL(1);
Eupnp_UDP_Datagram    *eupnp_udp_transport_recv(Eupnp_UDP_Transport *s) EINA_ARG_NONNULL(1);
Eupnp_UDP_Datagram    *eupnp_udp_transport_recvfrom(Eupnp_UDP_Transport *s) EINA_ARG_NONNULL(1);
int                    eupnp_udp_transport_sendto(Eupnp_UDP_Transport *s, const void *buffer, const char *addr, int port) EINA_ARG_NONNULL(1,2,3,4);
void                   eupnp_udp_transport_datagram_free(Eupnp_UDP_Datagram *datagram) EINA_ARG_NONNULL(1);

#endif /* _Eupnp_UDP_Transport_H */
