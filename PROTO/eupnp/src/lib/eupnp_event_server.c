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
#include <time.h>

#include <Eina.h>

#include "Eupnp.h"
#include "eupnp_core.h"
#include "eupnp_private.h"
#include "eupnp_http_message.h"


/*
 * Private API
 */

static Eupnp_Server _event_server = NULL;
static char *_host = NULL;
static int _port = 0;
static char *_url = NULL;
static int _log_dom = -1;

#undef DBG
#undef INF
#undef WRN
#undef ERR
#undef CRIT
#define DBG(...) EINA_LOG_DOM_DBG(_log_dom, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_log_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_ERR(_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_log_dom, __VA_ARGS__)
#define CRIT(...) EINA_LOG_DOM_CRIT(_log_dom, __VA_ARGS__)


static void
_client_data_ready(void *buffer, int size, void *data __UNUSED__)
{
   DBG("Event received, size %d!", size);

   char *bcopy = NULL;

   bcopy = malloc(sizeof(char)*(size + 1));

   if (!bcopy)
     {
	DBG("Failed to copy event notification data to buffer.");
	return;
     }

   memcpy(bcopy, buffer, size);
   bcopy[size] = '\0';

   DBG("Event is %s", bcopy);

   Eupnp_HTTP_Request *req = eupnp_http_request_parse(bcopy, NULL, NULL);

   if (!req)
     {
	DBG("Special case: %s", bcopy);
	ERR("Could not parse event notification.");
	free(bcopy);
	return;
     }

   int t = strtol((char *)(req->uri + 1), NULL, 10);
   eupnp_event_bus_publish(t, (void *)req->payload);
   eupnp_http_request_free(req);
   free(bcopy);
}

/*
 * Public API
 */

Eina_Bool
eupnp_event_server_init(void)
{
   int retries = 5;

   if ((_log_dom = eina_log_domain_register("Eupnp.EventServer", EINA_COLOR_BLUE)) < 0)
     {
	EINA_LOG_DOM_ERR
	  (EUPNP_LOGGING_DOM_GLOBAL, "Failed to create a new logging domain.");
	goto log_domain_fail;
     }

   srand(time(NULL));
   _host = (char *)eupnp_utils_default_host_ip_get();
 retry_port:
   _port = (49152 + rand() / (RAND_MAX / (65535 - 49152 + 1) + 1));

   if (!_host)
     {
	ERR("Could not find default host ip.");
	goto host_ip_err;
     }

   if (asprintf(&_url, "http://%s:%d", _host, _port) < 0)
     {
	ERR("Could not mount listening url.");
	goto listen_url_err;
     }

   _event_server = eupnp_core_server_add(_host, _port,
					 EUPNP_CLIENT_DATA_CB(_client_data_ready),
					 NULL);

   if (!_event_server)
     {
	// TODO try again with another port, change core to know if it was only
	// a binding error.
	ERR("Could not create a new server");
	retries--;
	if (retries > 0)
	  goto retry_port;
	else
	  goto server_creation_err;
     }

   INF("Initializing event server module.");

   return EINA_TRUE;

   server_creation_err:
	free((char *)_url);
   listen_url_err:
	free((char *)_host);
   host_ip_err:
	eina_log_domain_unregister(_log_dom);
   log_domain_fail:
	_log_dom = -1;

   return EINA_FALSE;
}

Eina_Bool
eupnp_event_server_shutdown(void)
{
   INF("Shutting down event server module.");
   free((char *)_host);
   free((char *)_url);
   _port = -1;
   eupnp_core_server_free(_event_server);
   eina_log_domain_unregister(_log_dom);
   _log_dom = -1;
   return EINA_TRUE;
}


/*
 * Calls cb(void *buffer, int size, void *data) when a client from "from" sends
 * a request.
 *
 * Associate the pair (cb, from_addr) to an event type that will be emitted by
 * the server whenever events occur.
 */
int
eupnp_event_server_request_subscribe(Eupnp_Callback cb, void *data)
{
   CHECK_NULL_RET(cb, EINA_FALSE);

   Eupnp_Event_Type event = eupnp_event_bus_event_type_new();

   Eupnp_Subscriber *sub;
   sub = eupnp_event_bus_subscribe(event, cb, data);

   if (!sub)
     {
	ERR("Failed to subscribe on event bus.");
	return -1;
     }

   return event;
}

/*
 * Returns the URL at which the server is currently listening on.
 *
 * @return Listening url
 */
const char *
eupnp_event_server_url_get()
{
   return (const char *) _url;
}
