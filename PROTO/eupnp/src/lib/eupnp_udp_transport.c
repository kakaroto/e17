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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

#include "Eupnp.h"
#include "eupnp_log.h"
#include "eupnp_udp_transport.h"


/**
 * @addtogroup Eupnp_UDP_Transport UDP Transport
 *
 */

/**
 * Private API
 */

/**
 * Constructor for the Eupnp_UDP_Datagram class.
 *
 * @param host Host from the datagram came from
 * @param port Port from the datagram came from
 * @param data Datagram data
 * @param data_len Length of datagram data
 */
static Eupnp_UDP_Datagram *
eupnp_udp_datagram_new(const char *host, int port, char *data, size_t data_len)
{
   Eupnp_UDP_Datagram *datagram;

   datagram = malloc(sizeof(Eupnp_UDP_Datagram));

   if (!datagram)
     {
	eina_error_set(EINA_ERROR_OUT_OF_MEMORY);
	ERROR("datagram alloc failed.");
	return NULL;
     }

   if (data)
      datagram->data = data;
   else
     {
	datagram->data = calloc(1, sizeof(char) * (data_len + 1));
	if (!datagram->data)
	  {
	     eina_error_set(EINA_ERROR_OUT_OF_MEMORY);
	     ERROR("could not allocate buffer for new datagram.");
	     return NULL;
	  }
     }
   if (host)
      datagram->host = host;
   if (port)
      datagram->port = port;
   return datagram;
}

/**
 * Destructor for the Eupnp_UDP_Datagram class.
 *
 * @param datagram Eupnp_UDP_Datagram instance to be destroyed
 */
static void
eupnp_udp_datagram_free(Eupnp_UDP_Datagram *datagram)
{
   if (!datagram) return;
   if (datagram->data) free(datagram->data);
   free(datagram);
}

/**
 * Prepares a Eupnp_UDP_Transport instance for work
 *
 * Sets the socket non-blocking, binds it and sets multicast options.
 *
 * @return EINA_TRUE on success, EINA_FALSE on failure.
 */
static Eina_Bool
eupnp_udp_transport_prepare(Eupnp_UDP_Transport *s)
{
   int reuse_addr = 1; // yes

   if (fcntl(s->socket, F_SETFL, O_NONBLOCK) < 0)
     {
	ERROR(" Error setting nonblocking option on socket. %s",
	      strerror(errno));
	return EINA_FALSE;
     }

   if (bind(s->socket, (struct sockaddr *) &s->in_addr, s->in_addr_len) < 0)
     {
	ERROR("Error binding. %s", strerror(errno));
	return EINA_FALSE;
     }

   if (setsockopt(s->socket, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(int)) < 0)
     {
	ERROR("setsockopt SO_REUSE_ADDR failed. %s", strerror(errno));
	return EINA_FALSE;
     }

   if (setsockopt(s->socket, IPPROTO_IP, IP_ADD_MEMBERSHIP,&s->mreq,
		   sizeof(struct ip_mreq)) < 0)
     {
	ERROR("setsockopt IP_ADD_MEMBERSHIP failed. %s", strerror(errno));
	return EINA_FALSE;
     }

   return EINA_TRUE;
}


/*
 * Public API
 */

/**
 * Constructor for the Eupnp_UDP_Transport class.
 *
 * @param addr Address to bind on
 * @param port Port to bind on
 * @param iface_addr Interface address to bind on
 *
 * @return On success, a new Eupnp_UDP_Transport instance. Otherwise NULL.
 */
EAPI Eupnp_UDP_Transport *
eupnp_udp_transport_new(const char *addr, int port, const char *iface_addr)
{
   Eupnp_UDP_Transport *s;

   s = malloc(sizeof(Eupnp_UDP_Transport));
   s->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

   if (s->socket < 0)
     {
	ERROR("Error creating socket. %s", strerror(errno));
	free(s);
	return NULL;
     }

   memset(&s->in_addr, 0, sizeof(s->in_addr));
   s->in_addr.sin_family = AF_INET;
   s->in_addr.sin_port = htons(port);
   s->in_addr.sin_addr.s_addr = inet_addr(iface_addr);
   s->in_addr_len = sizeof(struct sockaddr_in);
   s->mreq.imr_multiaddr.s_addr = inet_addr(addr);
   s->mreq.imr_interface.s_addr = htonl(INADDR_ANY);

   if (!eupnp_udp_transport_prepare(s))
     {
	ERROR("Could not prepare socket.");
	free(s);
	return NULL;
     }

   return s;
}

/**
 * Closes the Eupnp_UDP_Transport socket.
 */
EAPI int
eupnp_udp_transport_close(Eupnp_UDP_Transport *s)
{
   if (!s) return -1;
   return close(s->socket);
}

/**
 * Destructor for the Eupnp_UDP_Transport class.
 *
 * @param s Eupnp_UDP_Transport instance to be destroyed.
 */
EAPI void
eupnp_udp_transport_free(Eupnp_UDP_Transport *s)
{
   free(s);
}

/**
 * Receives a datagram from the socket.
 *
 * @param s Eupnp_UDP_Transport instance
 *
 * @return On success, a Eupnp_UDP_Datagram instance representing the datagram
 *         received. Otherwise, returns NULL.
 */
EAPI Eupnp_UDP_Datagram *
eupnp_udp_transport_recv(Eupnp_UDP_Transport *s)
{
   Eupnp_UDP_Datagram *d;
   int data_len;

   if (!ioctl(s->socket, FIONREAD, &data_len))
      DEBUG("Bytes available for reading: %d", data_len);
   else
     {
	// Don't know how many bytes are available, expect EUPNP_UDP_PACKET_LEN.
	DEBUG("could not determine how many bytes available.");
	data_len = EUPNP_UDP_PACKET_LEN;
     }

   d = eupnp_udp_datagram_new(NULL, 0, NULL, data_len);

   if (!d) return NULL;

   recv(s->socket, d->data, data_len, 0);

   return d;
}

/**
 * Receives a datagram from the socket and identifies the sender
 *
 * @param s Eupnp_UDP_Transport instance
 *
 * @return On success, a Eupnp_UDP_Datagram instance representing the datagram
 *         received. Otherwise, returns NULL.
 */
EAPI Eupnp_UDP_Datagram *
eupnp_udp_transport_recvfrom(Eupnp_UDP_Transport *s)
{
   Eupnp_UDP_Datagram *d;
   int data_len;

   if (!ioctl(s->socket, FIONREAD, &data_len))
      DEBUG("ioctl says %d bytes available for reading", data_len);
   else
     {
	// Don't know how many bytes are available, expect EUPNP_UDP_PACKET_LEN.
	DEBUG("could not determine how many bytes available.");
	data_len = EUPNP_UDP_PACKET_LEN;
     }

   d = eupnp_udp_datagram_new(NULL, 0, NULL, data_len);

   if (!d) return NULL;

   recvfrom(s->socket, d->data, data_len, 0,
	    (struct sockaddr *)&(s->in_addr), &(s->in_addr_len));

   d->host = inet_ntoa(s->in_addr.sin_addr);
   d->port = ntohs(s->in_addr.sin_port);

   return d;
}

/**
 * Sends a UDP datagram to the specified destination.
 *
 * @param s Eupnp_UDP_Transport instance
 * @param buffer Datagram message to send
 * @param addr Destination address
 * @param port Destination port
 *
 * @return The length of the message sent. In case of failure, returns -1.
 */
EAPI int
eupnp_udp_transport_sendto(Eupnp_UDP_Transport *s, const void *buffer, const char *addr, int port)
{
   int cnt;

   s->in_addr.sin_family = AF_INET;
   s->in_addr.sin_port = htons(port);

   if (inet_aton(addr, &(s->in_addr.sin_addr)) == 0)
     {
	ERROR("could not convert address. %s", strerror(errno));
	return -1;
     }

   cnt = sendto(s->socket, buffer,
		strlen((char *)buffer)*sizeof(char), 0,
		(struct sockaddr *)&(s->in_addr), s->in_addr_len);

   return cnt;
}

/**
 * Destructor for the Eupnp_UDP_Datagram class.
 *
 * @param datagram Eupnp_UDP_Datagram to be destroyed.
 */
EAPI void
eupnp_udp_transport_datagram_free(Eupnp_UDP_Datagram *datagram)
{
   eupnp_udp_datagram_free(datagram);
}


