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

#include <stdio.h>
#include <string.h>
#include <Ecore.h>
#include <Ecore_Con.h>
#include <Eina.h>

#include "Eupnp_Ecore.h"
#include "eupnp_core.h"
#include "eupnp_http_message.h"


#define DBG(...) EINA_LOG_DOM_DBG(_log_dom, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_log_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_ERR(_log_dom, __VA_ARGS__)

#define STR(x) #x
#define XSTR(x) STR(x)

#define CHECK_NULL_RET(x, ...)						\
  do {									\
        if (x == NULL) {                                                \
	   WRN("%s == NULL!", XSTR(x));					\
	   return __VA_ARGS__;						\
        }                                                               \
    } while (0)

/*
 * Private API
 */

static int _eupnp_ecore_init_count = 0;
static int _log_dom = -1;
static Ecore_Event_Handler *data_handler = NULL;
static Ecore_Event_Handler *completed_handler = NULL;
static Ecore_Event_Handler *cli_data_handler = NULL;
static Ecore_Event_Handler *cli_added_handler = NULL;
static Ecore_Event_Handler *cli_del_handler = NULL;
static Eina_List *srv_clients = NULL;

typedef struct _Eupnp_Ecore_Server Eupnp_Ecore_Server;

struct _Eupnp_Ecore_Server {
   Ecore_Con_Server *server;
   Eupnp_Client_Data_Cb cb;
   void *data;
};

typedef Eina_Bool (*Eupnp_Ecore_Fd_Handler_Cb) (void *data, Ecore_Fd_Handler *handler);
typedef Eina_Bool (*Eupnp_Ecore_Timer_Cb) (void *data);
typedef Eina_Bool (*Eupnp_Ecore_Idler_Cb) (void *data);


static Eupnp_Fd_Handler
_ecore_fd_handler_add(int fd, Eupnp_Fd_Flags flags, Eupnp_Fd_Handler_Cb cb, void *data)
{
   return ecore_main_fd_handler_add(fd, flags, ((Eupnp_Ecore_Fd_Handler_Cb)cb), data, NULL, NULL);
}

static Eina_Bool
_ecore_fd_handler_del(Eupnp_Fd_Handler handler)
{
   ecore_main_fd_handler_del(handler);
   return EINA_TRUE;
}

static Eupnp_Timer
_ecore_timer_add(double interval, Eupnp_Timer_Cb timer, void *data)
{
   return ecore_timer_add(interval, ((Eupnp_Ecore_Timer_Cb)timer), data);
}

static Eina_Bool
_ecore_timer_del(Eupnp_Timer timer)
{
   ecore_timer_del(timer);
   return EINA_TRUE;
}

static Eupnp_Idler
_ecore_idler_add(Eupnp_Idler_Cb idler_func, void *data)
{
   return ecore_idler_add(((Eupnp_Ecore_Idler_Cb)idler_func), data);
}

static Eina_Bool
_ecore_idler_del(Eupnp_Idler idler)
{
   ecore_idler_del(idler);
   return EINA_TRUE;
}

typedef struct _Eupnp_Ecore_Request Eupnp_Ecore_Request;

struct _Eupnp_Ecore_Request {
   Eupnp_Request_Completed_Cb completed_cb;
   Eupnp_Request_Data_Cb data_cb;
   Ecore_Con_Url *con;
   void *data;
   const char *body;
};

static Eina_Bool
_ecore_request_data_cb(void *data, int type, void *event)
{
   Ecore_Con_Event_Url_Data *e = event;
   Ecore_Con_Url *url_con = e->url_con;
   Eupnp_Ecore_Request *request = ecore_con_url_data_get(url_con);

   DBG("Data available event for object %p", request);

   request->data_cb(e->data, e->size, request->data);

   return EINA_TRUE;
}

static Eina_Bool
_ecore_request_completed_cb(void *data, int type, void *event)
{
   Eupnp_Request_Completed_Cb completed_cb = data;
   Ecore_Con_Event_Url_Complete *e = event;
   Ecore_Con_Url *url_con = e->url_con;
   Eupnp_Ecore_Request *request = ecore_con_url_data_get(url_con);

   DBG("Request completed for object %p, status %d", request->data, e->status);

   /* Copy headers */
   char *packet = NULL;
   char *s = NULL;
   Eina_List *l;
   Eina_List *headers = (Eina_List *)ecore_con_url_response_headers_get(url_con);

   EINA_LIST_FOREACH(headers, l, s)
     {
	if (packet)
	  {
	   if ((asprintf(&packet, "%s%s", packet, s)) < 0)
	     {
		ERR("Failed to remount http headers.");
		goto headers_mount_error;
	     }
	  }
	else
	   if ((asprintf(&packet, "%s", s)) < 0)
	     {
		ERR("Failed to remount http headers.");
		goto headers_mount_error;
	     }
     }

   Eupnp_HTTP_Request *req = eupnp_http_request_parse(packet, packet, free);

   if (!req)
     {
	ERR("Failed to parse http headers for request.");
	goto headers_mount_error;
     }

   request->completed_cb(((Eupnp_Request)request), request->data, req);

   eupnp_http_request_free(req);

   return EINA_FALSE;

   headers_mount_error:
     request->completed_cb(((Eupnp_Request)request), request->data, NULL);
     return EINA_FALSE;
}

static Eupnp_Request
_ecore_request(const char *url, const char *req, Eina_Array *additional_headers, const char *content_type, size_t body_length, const char *body, Eupnp_Request_Data_Cb data_cb, Eupnp_Request_Completed_Cb completed_cb, void *data)
{
   CHECK_NULL_RET(url, NULL);
   CHECK_NULL_RET(data_cb, NULL);
   CHECK_NULL_RET(completed_cb, NULL);

   DBG("Creating new %s request for url %s, data %p, headers %p, content-type %s, size %d", req, url, data, additional_headers, content_type, body_length);

   Ecore_Con_Url *con;
   Eupnp_Ecore_Request *request;

   request = malloc(sizeof(Eupnp_Ecore_Request));

   if (!request)
     {
	ERR("Failed to alloc for a new ecore con request object.");
	return NULL;
     }

   // NULL req means POST
   if (req)
      con = ecore_con_url_custom_new(url, req);
   else
      con = ecore_con_url_new(url);

   if (!con)
     {
	ERR("Failed to add an ecore con url job");
	goto con_error;
     }

   request->data = data;
   request->data_cb = data_cb;
   request->completed_cb = completed_cb;
   request->con = con;
   request->body = NULL;

   // Use a copy of body and free it under request completion. We can't assume
   // the request framework copies it.
   if (body)
     {
	request->body = strdup(body);

	if (!request->body)
	  {
	     ERR("Failed to copy request body.");
	     goto body_error;
	  }
     }

   ecore_con_url_data_set(con, request);

   DBG("Adding additional headers %p.", additional_headers);

   /* Insert additional headers */

   if (additional_headers)
     {
	Eina_Array_Iterator it;
	 int i;
	Eupnp_HTTP_Header *h;
	EINA_ARRAY_ITER_NEXT(additional_headers, i, h, it)
	  {
	     DBG("Appending header %s: %s to request request", h->key, h->value);
	     ecore_con_url_additional_header_add(con, h->key, h->value);
	  }
     }

   DBG("Sending request %p", request);

   if (!ecore_con_url_post(con,
                           request->body,
                           body_length,
                           content_type))
     {
	ERR("Failed to send request request");
	goto send_error;
     }

   DBG("Finished sending request.");

   return request;

   send_error:
	free((char *)request->body);
   body_error:
	ecore_con_url_free(con);
   con_error:
	free(request);
	return NULL;
}

static void
_ecore_request_free(Eupnp_Request request)
{
   CHECK_NULL_RET(request);
   Eupnp_Ecore_Request *dl = request;
   free((char *)dl->body);
   ecore_con_url_free(dl->con);
   free(dl);
}


static Eupnp_Server
_ecore_server_add(const char *name, int port, Eupnp_Client_Data_Cb cb, void *data)
{
   DBG("Creating server for %s, port %d, cb %p, data %p", name, port, cb, data);

   Eupnp_Ecore_Server *server;

   server = malloc(sizeof(Eupnp_Ecore_Server));

   if (!server)
     {
	DBG("Failed to alloc for a new server");
	return NULL;
     }

   server->data = data;
   server->cb = cb;

   server->server = ecore_con_server_add(ECORE_CON_REMOTE_TCP, name, port, server);

   if (!server->server)
     {
	DBG("Failed to create ecore con server for %s:%d", name, port);
	free(server);
	return NULL;
     }

   return server;
}

static const char *
_ecore_server_url_get(Eupnp_Server server)
{
   Eupnp_Ecore_Server *srv = server;
   DBG("Server ip: %s", ecore_con_server_ip_get(srv->server));
   return (const char *)ecore_con_server_ip_get(srv->server);
}

static void
_ecore_server_free(Eupnp_Server server)
{
   Eupnp_Ecore_Server *srv = server;
   ecore_con_server_del(srv->server);
   free(srv);
}

typedef struct _Client_Data Client_Data;
struct _Client_Data {
   Ecore_Con_Client *client;
   char *buf;
   int buf_len;
   int total_len;
};

static Client_Data *
_ecore_srv_client_get(Ecore_Con_Client *client)
{
   Eina_List *l;
   Client_Data *d;

   EINA_LIST_FOREACH(srv_clients, l, d)
     if (d->client == client)
	return d;

   return NULL;
}

static int
_ecore_srv_headers_len(const char *msg)
{
   return strstr(msg, "\r\n\r\n") - msg - 4; /* Subtract 4 chars (\r\n\r\n) */
}

static const char *
_ecore_srv_current_date_http_string_get(void)
{
   time_t t = time(NULL);
   struct tm *tm = gmtime(&t);
   static char date[255];

   if (!strftime(date, 255, "%a, %d %b %Y %H:%M:%S %Z", tm))
     {
	fprintf(stderr, "Not enough memory to create new date string.\n");
	return NULL;
     }

   return date;
}


static void
_ecore_srv_respond_ok(Ecore_Con_Client *client)
{
   char *response = NULL;
   int len;
   const char *date = _ecore_srv_current_date_http_string_get();

   if ((len = asprintf(&response, "HTTP/1.1 200 OK\r\n"
		       "Date: %s\r\n"
		       "Content-Length: 0\r\n"
		       "Connection: close\r\n\r\n", date)) < 0)
     {
	ERR("Failed to send ok message.");
	return;
     }

   DBG("Responding to client %p %d chars", client, len);

   if (ecore_con_client_send(client, response, len) < 0)
     {
	ERR("Failed to send ok message stage 2.");
	return;
     }
}

static void
_ecore_srv_find_total_len(Client_Data *d)
{
   char *tmp = malloc(sizeof(char)*(d->buf_len + 1));
   if (!tmp) return;
   memcpy(tmp, d->buf, d->buf_len);
   tmp[d->buf_len] = '\0';

   Eupnp_HTTP_Request *req = eupnp_http_request_parse(tmp, tmp, free);
   if (!req) return;

   const char *len = eupnp_http_header_get(req->headers, "content-length");
   if (!len) return;

   d->total_len = strtol(len, NULL, 10);
   eupnp_http_request_free(req);
   DBG("Found content length header: %d", d->total_len);
}

static Eina_Bool
_ecore_srv_client_data_cb(void *data, int type, void *event)
{
   Ecore_Con_Event_Client_Data *e = event;
   Client_Data *d = _ecore_srv_client_get(e->client);
   DBG("Data for client %p (%p)", e->client, d);

   if (!d)
     {
	ERR("Received data event for untracked client %p", e->client);
	return EINA_TRUE;
     }

   int old_size = d->buf_len;
   char *tmp = realloc(d->buf, sizeof(char)*(old_size + e->size));

   if (!tmp)
     {
	ERR("Failed to alloc more for client data %p", e->client);
	return EINA_TRUE;
     }

   memcpy(tmp + old_size, e->data, e->size);
   d->buf = tmp;
   d->buf_len += e->size;

   if (!d->total_len)
      _ecore_srv_find_total_len(d);

   if ((d->buf_len - _ecore_srv_headers_len(d->buf)) >= d->total_len)
      _ecore_srv_respond_ok(d->client);

   DBG("Total len is %d, buf len is %d", d->total_len, d->buf_len - _ecore_srv_headers_len(d->buf));

   return EINA_TRUE;
}

static Eina_Bool
_ecore_srv_client_add_cb(void *data, int type, void *event)
{
   Ecore_Con_Event_Client_Add *e = event;
   Client_Data *d = NULL;
   DBG("Client add event %p at ip %s", e->client, ecore_con_client_ip_get(e->client));

   d = malloc(sizeof(Client_Data));

   if (!d)
     {
	ERR("Could not alloc for a client %p", e->client);
	return EINA_TRUE;
     }

   d->buf = NULL;
   d->buf_len = 0;
   d->total_len = 0;
   d->client = e->client;

   srv_clients = eina_list_append(srv_clients, d);

   return EINA_TRUE;
}

static Eina_Bool
_ecore_srv_client_del_cb(void *data, int type, void *event)
{
   Ecore_Con_Event_Client_Del *e = event;
   DBG("Client del event %p at ip %s", e->client, ecore_con_client_ip_get(e->client));

   Eina_List *l;
   Client_Data *d;

   EINA_LIST_FOREACH(srv_clients, l, d)
	if (d->client == e->client)
	  {
	     DBG("Found client for del event: %p", e->client);
	     srv_clients = eina_list_remove(srv_clients, d);
	     break;
	  }

   if (!d)
     {
	ERR("Del event on untracked client %p", e->client);
	return EINA_TRUE;
     }

   // Forward event to server
   DBG("Forwarding completed data event to callback.");

   Ecore_Con_Server *server = ecore_con_client_server_get(e->client);
   Eupnp_Ecore_Server *srv = ecore_con_server_data_get(server);
   srv->cb(d->buf, d->buf_len, srv->data);

   free(d->buf);
   free(d);

   return EINA_TRUE;
}

/*
 * Public API
 */

/*
 * Initializes the Eupnp-Ecore library
 *
 * @return On error, returns 0. Otherwise, returns the number of times it's been
 * called.
 */
int
eupnp_ecore_init(void)
{
   if (_eupnp_ecore_init_count) return ++_eupnp_ecore_init_count;

   if (!eina_init())
     {
	fprintf(stderr, "Failed to initialize eina.\n");
	return 0;
     }

   if (!ecore_init())
     {
	fprintf(stderr, "Failed to initialize ecore.\n");
	goto ecore_init_fail;
     }

   if (!ecore_con_init())
     {
	fprintf(stderr, "Failed to initialize ecore con module.\n");
	goto con_fail;
     }

   if (!ecore_con_url_init())
     {
	fprintf(stderr, "Failed to initialize ecore con url module.\n");
	goto con_url_fail;
     }

   if ((_log_dom = eina_log_domain_register("Eupnp.Ecore", EINA_COLOR_BLUE)) < 0)
     {
	fprintf(stderr,
		"Failed to create error domain for eupnp ecore library.\n");
	goto log_dom_fail;
     }

   eupnp_core_fd_handler_add_func_set(_ecore_fd_handler_add);
   eupnp_core_fd_handler_del_func_set(_ecore_fd_handler_del);
   eupnp_core_timer_add_func_set(_ecore_timer_add);
   eupnp_core_timer_del_func_set(_ecore_timer_del);
   eupnp_core_idler_add_func_set(_ecore_idler_add);
   eupnp_core_idler_del_func_set(_ecore_idler_del);
   eupnp_core_request_func_set(_ecore_request);
   eupnp_core_request_free_func_set(_ecore_request_free);
   eupnp_core_server_add_func_set(_ecore_server_add);
   eupnp_core_server_free_func_set(_ecore_server_free);
   eupnp_core_server_listen_url_get_func_set(_ecore_server_url_get);

   data_handler = ecore_event_handler_add(ECORE_CON_EVENT_URL_DATA, _ecore_request_data_cb, NULL);
   if (!data_handler) goto data_handler_err;

   completed_handler = ecore_event_handler_add(ECORE_CON_EVENT_URL_COMPLETE, _ecore_request_completed_cb, NULL);
   if (!completed_handler) goto completed_handler_fail;

   cli_data_handler = ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DATA, _ecore_srv_client_data_cb, NULL);
   if (!cli_data_handler) goto cli_handler_err;

   cli_added_handler = ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_ADD, _ecore_srv_client_add_cb, NULL);
   if (!cli_added_handler) goto cli_added_handler_err;

   cli_del_handler = ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DEL, _ecore_srv_client_del_cb, NULL);
   if (!cli_del_handler) goto cli_del_handler_err;

   INF("Initializing eupnp-ecore library.");

   return ++_eupnp_ecore_init_count;

   cli_del_handler_err:
	ecore_event_handler_del(cli_added_handler);
   cli_added_handler_err:
	ecore_event_handler_del(cli_data_handler);
   cli_handler_err:
	ecore_event_handler_del(completed_handler);
   completed_handler_fail:
	ecore_event_handler_del(data_handler);
   data_handler_err:
	eina_log_domain_unregister(_log_dom);
   log_dom_fail:
	ecore_con_url_shutdown();
   con_url_fail:
        ecore_con_shutdown();
   con_fail:
	ecore_shutdown();
   ecore_init_fail:
	eina_shutdown();

   return 0;
}

/*
 * Shuts down the Eupnp-Ecore library
 *
 * @return 0 if completely shutted down the module.
 */
int
eupnp_ecore_shutdown(void)
{
   if (_eupnp_ecore_init_count != 1) return --_eupnp_ecore_init_count;

   INF("Shutting down eupnp-ecore library.");

   ecore_event_handler_del(cli_del_handler);
   ecore_event_handler_del(cli_added_handler);
   ecore_event_handler_del(cli_data_handler);
   ecore_event_handler_del(completed_handler);
   ecore_event_handler_del(data_handler);
   eina_log_domain_unregister(_log_dom);
   ecore_con_url_shutdown();
   ecore_con_shutdown();
   ecore_shutdown();

   if (srv_clients)
     {
	Client_Data *d;

	EINA_LIST_FREE(srv_clients, d)
	  {
	     free(d->buf);
	     free(d);
	  }
     }

   eina_shutdown();

   return --_eupnp_ecore_init_count;
}
