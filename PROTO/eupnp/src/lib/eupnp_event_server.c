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

#include <string.h>
#include <Eina.h>

#include "eupnp_event_server.h"
#include "eupnp_log.h"
#include "eupnp_core.h"
#include "eupnp_private.h"
#include "eupnp_event_bus.h"
#include "eupnp_http_message.h"


/*
 * Private API
 */

static Eupnp_Server _event_server = NULL;
static const char *_host = NULL;
static int _port = 0;
static const char *_url = NULL;
static int _event_server_init_count = 0;
static int _log_dom = -1;
static int _backends = 0;

static void
_client_data_ready(void *buffer, int size, void *data)
{
   DEBUG_D(_log_dom, "Event received, size %d!\n", size);

   char *bcopy = NULL;

   bcopy = malloc(sizeof(char)*(size + 1));

   if (!bcopy)
     {
	DEBUG_D(_log_dom, "Failed to copy event notification data to buffer.\n");
	return;
     }

   memcpy(bcopy, buffer, size);
   bcopy[size] = '\0';

   DEBUG_D(_log_dom, "event is %s\n", bcopy);

   Eupnp_HTTP_Request *req = eupnp_http_request_parse(bcopy, bcopy, free);

   if (!req)
     {
	memcpy(bcopy, buffer, size);
	bcopy[size] = '\0';
	DEBUG_D(_log_dom, "Special case: %s\n", bcopy);
	ERROR_D(_log_dom, "Could not parse event notification.\n");
	return;
     }

   int t = strtol((char *)(req->uri + 1), NULL, 10);
   eupnp_event_bus_publish(t, (void *)req->payload);

   host_header_err:
	eupnp_http_request_free(req);
}

/*
 * Public API
 */

EAPI int
eupnp_event_server_init(void)
{
   if (_event_server_init_count) return ++_event_server_init_count;

   if (!eupnp_log_init())
     {
	fprintf(stderr, "Failed to initialize eupnp log module.\n");
	return _event_server_init_count;
     }

   if ((_log_dom = eina_log_domain_register("Eupnp.EventServer", EINA_COLOR_BLUE)) < 0)
     {
	fprintf(stderr, "Failed to create a new logging domain.\n");
	goto log_domain_fail;
     }

   if (!eupnp_event_bus_init())
     {
	ERROR("Failed to initialize eupnp event bus library.\n");
	goto event_bus_fail;
     }

   _host = (char *)default_host_ip_get();
   _port = random_port_get();

   if (!_host)
     {
	ERROR("Could not find default host ip.\n");
	goto host_ip_err;
     }

   if (asprintf(&_url, "http://%s:%d", _host, _port) < 0)
     {
	ERROR("Could not mount listening url.\n");
	goto listen_url_err;
     }

   _event_server = eupnp_core_server_add(_host, _port,
					 EUPNP_CLIENT_DATA_CB(_client_data_ready),
					 NULL);

   if (!_event_server)
     {
	// TODO try again with another port, change core to know if it was only
	// a binding error.
	ERROR("Could not create a new server\n");
	goto server_creation_err;
     }

   INFO_D(_log_dom, "Initializing event server module.\n");

   return ++_event_server_init_count;

   server_creation_err:
	free((char *)_url);
   listen_url_err:
	free((char *)_host);
   host_ip_err:
	eupnp_event_bus_shutdown();
   event_bus_fail:
	eina_log_domain_unregister(_log_dom);
	_log_dom = -1;
   log_domain_fail:
	eupnp_log_shutdown();

   return 0;
}

EAPI int
eupnp_event_server_shutdown(void)
{
   if (_event_server_init_count != 1) return --_event_server_init_count;

   INFO_D(_log_dom, "Shutting down event server module.\n");
   free((char *)_host);
   free((char *)_url);
   _port = -1;
   eupnp_core_server_free(_event_server);
   eupnp_event_bus_shutdown();
   eina_log_domain_unregister(_log_dom);
   _log_dom = -1;
   eupnp_log_shutdown();

   return --_event_server_init_count;
}


/*
 * Calls cb(void *buffer, int size, void *data) when a client from "from" sends
 * a request.
 *
 * Associate the pair (cb, from_addr) to an event type that will be emitted by
 * the server whenever events occur.
 */
EAPI int
eupnp_event_server_request_subscribe(Eupnp_Callback cb, void *data)
{
   CHECK_NULL_RET_VAL(cb, EINA_FALSE);

   Eupnp_Event_Type event = eupnp_event_bus_event_type_new();

   if (event < 0) return -1;

   Eupnp_Subscriber *sub;
   sub = eupnp_event_bus_subscribe(event, cb, data);

   if (!sub)
     {
	ERROR_D(_log_dom, "Failed to subscribe on event bus.\n");
	return -1;
     }

   return event;
}

/*
 * Returns the URL at which the server is currently listening on.
 *
 * @return Listening url
 */
EAPI const char *
eupnp_event_server_url_get()
{
   return _url;
}
