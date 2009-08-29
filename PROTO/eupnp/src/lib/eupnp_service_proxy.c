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
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "eupnp_log.h"
#include "eupnp_core.h"
#include "eupnp_event_server.h"
#include "eupnp_service_parser.h"
#include "eupnp_service_proxy.h"
#include "eupnp_service_info.h"
#include "eupnp_http_message.h"
#include "eupnp_private.h"


/*
 * Private API
 */
typedef struct _Eupnp_Action_Request Eupnp_Action_Request;

struct _Eupnp_Action_Request {
   Eupnp_Action_Response_Cb response_cb;
   void *data;
   Eupnp_Service_Proxy *proxy;
};

struct _Eupnp_Event_Subscriber {
   void *data;
   Eupnp_State_Variable_Event_Cb cb;
   Eupnp_State_Variable *state_var;
   Eupnp_Service_Proxy *proxy;
   char *eventing_url;
   char *callback;
   char *sid;
   int sid_len;
};

static void
eupnp_event_subscriber_free(Eupnp_Event_Subscriber *subscriber)
{
   CHECK_NULL_RET(subscriber);
   eupnp_service_proxy_unref(subscriber->proxy);
   free(subscriber->sid);
   free(subscriber->eventing_url);
   free(subscriber->callback);
   free(subscriber);
}

#define SOAP_ENVELOPE_BEGIN "<?xml version=\"1.0\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
#define SOAP_BODY_BEGIN     "<s:Body>"
#define SOAP_BODY_END       "</s:Body>"
#define SOAP_ENVELOPE_END   "</s:Envelope>"

#define SOAP_ARGUMENT_APPEND(message, arg_name, arg_fmt, arg_type, arg_list) \
	       ((asprintf(&message,                                          \
		 "%s<%s>" arg_fmt "</%s>",                                   \
		 message,                                                    \
		 arg_name,                                                   \
		 va_arg(arg_list, arg_type),                                 \
		 arg_name) < 0) ? EINA_FALSE : EINA_TRUE)
// FIXME The error above introduces a leak for pointer message.

static int _eupnp_service_proxy_init_count = 0;
static int _log_dom = -1;

/*
 * Callbacks for SCPD xml download
 */
static void
_data_ready(void *buffer, int size, void *data)
{
   Eupnp_Service_Proxy *proxy = data;
   eupnp_service_proxy_ref(proxy);
   DEBUG_D(_log_dom, "Data ready for proxy %p, with xml parser at %p\n", proxy, proxy->xml_parser);

   if (eupnp_service_parse_buffer(buffer, size, proxy))
     DEBUG_D(_log_dom, "Parsed SCPD XML successfully\n");
   else
     ERROR_D(_log_dom, "Failed to parse SCPD XML\n");

   if (!proxy->xml_parser)
     {
	DEBUG_D(_log_dom, "Finished building service proxy %p, forwarding to ready callback.\n", proxy);
	proxy->ready_cb(proxy, proxy->ready_cb_data);
     }
   eupnp_service_proxy_unref(proxy);
}

static void
_download_completed(Eupnp_Request request, void *data, const Eupnp_HTTP_Request *req)
{
   Eupnp_Service_Proxy *proxy = data;
   DEBUG_D(_log_dom, "Proxy download completed %p\n", proxy);
   eupnp_core_http_request_free(request);

   // Unreference ref made at service_proxy_fetch()
   eupnp_service_proxy_unref(proxy);
}

/*
 * Callbacks for actions request responses
 */

static void
_request_data_ready(void *buffer, int size, void *data)
{
   Eupnp_Action_Request *req = data;
   Eupnp_Service_Proxy *proxy = req->proxy;

   DEBUG_D(_log_dom, "Request data ready for proxy %p\n", proxy);

   req->response_cb(proxy, req->data, buffer, size);
}

static void
eupnp_action_request_free(Eupnp_Action_Request *req)
{
   CHECK_NULL_RET(req);
   eupnp_service_proxy_unref(req->proxy);
   free(req);
}

static void
_request_completed(Eupnp_Request request, void *data, const Eupnp_HTTP_Request *req)
{
   DEBUG_D(_log_dom, "Proxy action request completed %p\n", data);
   Eupnp_Action_Request *action_req = data;
   eupnp_core_http_request_free(request);
   eupnp_action_request_free(action_req);
}

/*
 * Subscription
 */

static void
_subscribe_data_ready(void *buffer, int size, void *data)
{
   Eupnp_Event_Subscriber *sub = data;
   DEBUG_D(_log_dom, "Subscription answer for req %p\n", sub);
   // TODO check for errors
}

static void
_subscribe_completed(Eupnp_Request request, void *data, const Eupnp_HTTP_Request *req)
{
   Eupnp_Event_Subscriber *sub = data;
   DEBUG_D(_log_dom, "Finished subscription %p\n", sub);
   DEBUG_D(_log_dom, "Request headers!!! %p\n", req);

   eupnp_http_request_dump(req);

   const char *sid = eupnp_http_header_get(req->headers, "sid");

   if (!sid)
	ERROR_D(_log_dom, "Could not find SID header for request.\n");
   else
     {
	DEBUG_D(_log_dom, "SID: %s\n", sid);
	sub->sid = strdup(sid);
	sub->sid_len = strlen(sub->sid);
     }

   eupnp_core_http_request_free(request);
}

/*
 * Unsubscription
 */

_unsubscribe_data_ready(void *buffer, int size, void *data)
{
   DEBUG_D(_log_dom, "Unsubscription data %s\n", data);
}

_unsubscribe_completed(Eupnp_Request request, void *data, const Eupnp_HTTP_Request *req)
{
   DEBUG_D(_log_dom, "Finished unsubscription\n");
   Eupnp_Event_Subscriber *subscriber = data;
   eupnp_http_request_dump(req);
   // TODO check for errors
   eupnp_core_http_request_free(request);

   DEBUG_D(_log_dom, "Freeing subscriber object after unsubscription.\n");
   eupnp_event_subscriber_free(subscriber);
}


/*
 * Event notification
 */

static Eina_Bool
_event_notification(void *data, Eupnp_Event_Type event_type, void *event_data)
{
   DEBUG_D(_log_dom, "Received event notification, subscriber %p, event %d\n", data, event_type);
   Eupnp_Event_Subscriber *subscriber = data;
   if (!subscriber->cb(subscriber->state_var, event_data, strlen((char *)event_data), subscriber->data))
     {
	eupnp_service_proxy_state_variable_events_unsubscribe(subscriber);
	return EINA_FALSE;
     }
   return EINA_TRUE;
}

/*
 * Public API
 */

int
eupnp_service_proxy_init(void)
{
   if (_eupnp_service_proxy_init_count)
      return ++_eupnp_service_proxy_init_count;

   if (!eina_array_init())
     {
	fprintf(stderr, "Failed to initialize eina array moduel.\n");
	return 0;
     }

   if (!eupnp_log_init())
     {
	fprintf(stderr, "Could not initialize eupnp error module.\n");
	goto error_init_error;
     }

   if ((_log_dom = eina_log_domain_register("Eupnp.ServiceProxy", EINA_COLOR_BLUE)) < 0)
     {
	ERROR("Failed to create logging domain for service proxy module.\n");
	goto log_dom_error;
     }

   if (!eupnp_service_parser_init())
     {
	ERROR("Could not initialize eupnp service parser module.\n");
	goto service_parser_init_error;
     }

   if (!eupnp_event_server_init())
     {
	ERROR("Failed to initialize eupnp event server module.\n");
	goto evt_server_error;
     }

   INFO_D(_log_dom, "Initializing service proxy module.\n");

   return ++_eupnp_service_proxy_init_count;

   evt_server_error:
      eupnp_service_parser_shutdown();
   service_parser_init_error:
      eina_log_domain_unregister(_log_dom);
   log_dom_error:
      eupnp_log_shutdown();
   error_init_error:
      eina_array_shutdown();

   return 0;
}

int
eupnp_service_proxy_shutdown(void)
{
   if (_eupnp_service_proxy_init_count != 1)
      return --_eupnp_service_proxy_init_count;

   INFO_D(_log_dom, "Shutting down service proxy module.\n");

   eupnp_event_server_shutdown();
   eupnp_service_parser_shutdown();
   eina_log_domain_unregister(_log_dom);
   eupnp_log_shutdown();
   eina_array_shutdown();

   return --_eupnp_service_proxy_init_count;
}

Eupnp_Service_Action *
eupnp_service_action_new(void)
{
   Eupnp_Service_Action *a;

   a = malloc(sizeof(Eupnp_Service_Action));

   if (!a)
     {
	ERROR_D(_log_dom, "Could not alloc for service action\n");
	return NULL;
     }

   a->name = NULL;
   a->arguments = NULL;

   return a;
}

void
eupnp_service_action_free(Eupnp_Service_Action *a)
{
   if (!a) return;

   free((char *)a->name);

   if (a->arguments)
     {
	Eupnp_Service_Action_Argument *item;
	
	while (item = (void *)a->arguments)
	  {
	     a->arguments = eina_inlist_remove(a->arguments, a->arguments);
	     eupnp_service_action_argument_free(item);
	  }
     }

   free(a);
}

void
eupnp_service_action_dump(Eupnp_Service_Action *a)
{
   if (!a) return;

   INFO_D(_log_dom, "\tAction dump\n");
   INFO_D(_log_dom, "\t\tname: %s\n", a->name);

   if (a->arguments)
     {
	Eupnp_Service_Action_Argument *arg;

	EINA_INLIST_FOREACH(a->arguments, arg)
	   eupnp_service_action_argument_dump(arg);
     }
}

Eupnp_State_Variable *
eupnp_service_state_variable_new(const char *name, int name_len)
{
   Eupnp_State_Variable *st;

   st = eupnp_service_state_variable_new1();

   if (!st)
     {
	ERROR_D(_log_dom, "Could not alloc for new state variable\n");
	return NULL;
     }

   st->name = malloc(sizeof(char)*(name_len+1));

   if (!st->name)
     {
	ERROR_D(_log_dom, "Could not alloc for state var name\n");
	free(st);
	return NULL;
     }

   memcpy((char *)st->name, name, name_len);
   ((char *)st->name)[name_len] = '\0';

   return st;
}

Eupnp_State_Variable *
eupnp_service_state_variable_new1()
{
   Eupnp_State_Variable *st;

   st = malloc(sizeof(Eupnp_State_Variable));

   if (!st)
     {
	ERROR_D(_log_dom, "Could not alloc for new state variable\n");
	return NULL;
     }

   st->name = NULL;
   st->send_events = EINA_TRUE;
   st->data_type = EUPNP_DATA_TYPE_STRING;
   st->default_value = NULL;
   st->allowed_value_list = NULL;
   st->range_min = NULL;
   st->range_max = NULL;
   st->range_step = NULL;

   return st;
}

void
eupnp_service_state_variable_free(Eupnp_State_Variable *st)
{
   CHECK_NULL_RET(st);

   free((char *)st->name);
   free((char *)st->default_value);
   free((char *)st->range_min);
   free((char *)st->range_max);
   free((char *)st->range_step);

   if (st->allowed_value_list)
     {
	Eupnp_State_Variable_Allowed_Value *item;

	while (item = (void *)st->allowed_value_list)
	  {
	     st->allowed_value_list = eina_inlist_remove(st->allowed_value_list,
						         st->allowed_value_list);
	     eupnp_service_state_variable_allowed_value_free(item);
	  }
     }

   free(st);
}

void
eupnp_service_state_variable_dump(Eupnp_State_Variable *st)
{
   CHECK_NULL_RET(st);

   INFO_D(_log_dom, "\t\tState variable dump\n");
   INFO_D(_log_dom, "\t\t\tname: %s\n", st->name);
   INFO_D(_log_dom, "\t\t\tsendEvents: %d\n", st->send_events);
   INFO_D(_log_dom, "\t\t\tdefault value: %s\n", (char *)st->default_value);
   INFO_D(_log_dom, "\t\t\trange min: %s\n", (char *)st->range_min);
   INFO_D(_log_dom, "\t\t\trange max: %s\n", (char *)st->range_max);
   INFO_D(_log_dom, "\t\t\trange step: %s\n", (char *)st->range_step);
   INFO_D(_log_dom, "\t\t\tdata type: %d\n", st->data_type);

   if (st->allowed_value_list)
   {
      Eupnp_State_Variable_Allowed_Value *item;
      INFO_D(_log_dom, "\t\t\tAllowed value list:\n");

      EINA_INLIST_FOREACH(st->allowed_value_list, item)
	INFO_D(_log_dom, "\t\t\t\t%s\n", item->value);
   }
}

Eina_Bool
eupnp_service_state_variable_allowed_value_add(Eupnp_State_Variable *st, Eupnp_State_Variable_Allowed_Value *v)
{
   CHECK_NULL_RET_VAL(st, EINA_FALSE);
   CHECK_NULL_RET_VAL(v, EINA_FALSE);

   st->allowed_value_list = eina_inlist_append(st->allowed_value_list, EINA_INLIST_GET(v));

   return EINA_TRUE;
}

Eina_Bool
eupnp_service_action_argument_add(Eupnp_Service_Action *action, Eupnp_Service_Action_Argument *arg)
{
   CHECK_NULL_RET_VAL(action, EINA_FALSE);
   CHECK_NULL_RET_VAL(arg, EINA_FALSE);

   action->arguments = eina_inlist_append(action->arguments, EINA_INLIST_GET(arg));

   return EINA_TRUE;
}

Eupnp_Service_Action_Argument *
eupnp_service_action_argument_new(void)
{
   Eupnp_Service_Action_Argument *arg;
   arg = malloc(sizeof(Eupnp_Service_Action_Argument));

   CHECK_NULL_RET_VAL(arg, NULL);

   arg->name = NULL;
   arg->direction = EUPNP_ARGUMENT_DIRECTION_IN;
   arg->related_state_variable = NULL;
   arg->retval = NULL;

   return arg;
}

void
eupnp_service_action_argument_free(Eupnp_Service_Action_Argument *arg)
{
   CHECK_NULL_RET(arg);
   free((char *)arg->name);
   free(arg->retval);
   free(arg);
}

void
eupnp_service_action_argument_dump(Eupnp_Service_Action_Argument *arg)
{
   CHECK_NULL_RET(arg);

   INFO_D(_log_dom, "\t\tArgument dump\n");
   INFO_D(_log_dom, "\t\t\tname: %s\n", arg->name);
   INFO_D(_log_dom, "\t\t\tdirection: %d\n", arg->direction);
   if (arg->related_state_variable)
     INFO_D(_log_dom, "\t\t\trelated state var: %s\n", arg->related_state_variable->name);
   INFO_D(_log_dom, "\t\t\tretval: %s\n", (char *)arg->retval);
}

Eupnp_State_Variable_Allowed_Value *
eupnp_service_state_variable_allowed_value_new(void)
{
   Eupnp_State_Variable_Allowed_Value *v;

   v = malloc(sizeof(Eupnp_State_Variable_Allowed_Value));

   if (!v)
     {
	ERROR_D(_log_dom, "Could not alloc for allowed value\n");
	return NULL;
     }

   v->value = NULL;

   return v;
}

void
eupnp_service_state_variable_allowed_value_free(Eupnp_State_Variable_Allowed_Value *value)
{
   CHECK_NULL_RET(value);
   free((char *)value->value);
   free(value);
}

/*
 * Creates a service proxy.
 *
 * Creates a service proxy capable of controlling the service.
 */
void
eupnp_service_proxy_new(Eupnp_Service_Info *service, Eupnp_Service_Proxy_Ready_Cb ready_cb, void *data)
{
   CHECK_NULL_RET(service);
   CHECK_NULL_RET(service->location);
   CHECK_NULL_RET(service->control_URL);
   CHECK_NULL_RET(ready_cb);
   CHECK_NULL_RET(service->scpd_URL);
   CHECK_NULL_RET(service->service_type);

   Eupnp_Service_Proxy *proxy;
   proxy = malloc(sizeof(Eupnp_Service_Proxy));

   CHECK_NULL_RET(proxy);

   proxy->control_URL = strdup(service->control_URL);
   proxy->base_URL = strdup(service->location);
   proxy->eventsub_URL = strdup(service->eventsub_URL);
   proxy->service_type = strdup(service->service_type);
   proxy->xml_parser = NULL;
   proxy->actions = NULL;
   proxy->state_table = NULL;
   proxy->spec_version_minor = 0;
   proxy->spec_version_major = 0;
   proxy->ready_cb = ready_cb;
   proxy->ready_cb_data = data;
   proxy->refcount = 1;

   eupnp_service_proxy_fetch(proxy, service->location, service->scpd_URL);
}

Eupnp_Service_Proxy *
eupnp_service_proxy_ref(Eupnp_Service_Proxy *proxy)
{
   CHECK_NULL_RET_VAL(proxy, NULL);

   proxy->refcount++;

   DEBUG_D(_log_dom, "Service proxy %p refcount %d -> %d\n", proxy,
	   proxy->refcount - 1, proxy->refcount);

   return proxy;
}

void
eupnp_service_proxy_unref(Eupnp_Service_Proxy *proxy)
{
   CHECK_NULL_RET(proxy);

   if (proxy->refcount < 1)
     {
	return;
     }

   proxy->refcount--;

   DEBUG_D(_log_dom, "Service proxy %p refcount %d -> %d\n", proxy,
	   proxy->refcount + 1, proxy->refcount);

   if (!proxy->refcount)
      eupnp_service_proxy_free(proxy);
}

void
eupnp_service_proxy_free(Eupnp_Service_Proxy *proxy)
{
   CHECK_NULL_RET(proxy);

   free(proxy->xml_parser);

   if (proxy->actions)
     {
	Eupnp_Service_Action *item;

	while (item = (void *)proxy->actions)
	  {
	     proxy->actions = eina_inlist_remove(proxy->actions, proxy->actions);
	     eupnp_service_action_free(item);
	  }
     }

   if (proxy->state_table)
     {
	Eupnp_State_Variable *item;

	while (item = (void *)proxy->state_table)
	  {
	     proxy->state_table = eina_inlist_remove(proxy->state_table,
						   proxy->state_table);
	     eupnp_service_state_variable_free(item);
	  }
     }

   free((char *)proxy->control_URL);
   free((char *)proxy->eventsub_URL);
   free((char *)proxy->base_URL);
   free((char *)proxy->service_type);

   free(proxy);
}

void
eupnp_service_proxy_fetch(Eupnp_Service_Proxy *proxy, const char *base_url, const char *scpd_url)
{
   CHECK_NULL_RET(proxy);
   CHECK_NULL_RET(scpd_url);
   CHECK_NULL_RET(base_url);

   // Reference for downloading, unref if download fails or if download
   // completes.

   if (!eupnp_utils_url_is_relative(scpd_url))
     {
	if (!eupnp_core_http_request_send(scpd_url, "GET", NULL, NULL, 0, NULL,
				     EUPNP_REQUEST_DATA_CB(_data_ready),
				     EUPNP_REQUEST_COMPLETED_CB(_download_completed), proxy))
	  {
	     ERROR_D(_log_dom, "Could not add a new download job for device %p\n", proxy);
	     eupnp_service_proxy_unref(proxy);
	  }
     }
   else
     {
	char *complete_url = NULL;

	if (asprintf(&complete_url, "%s%s", base_url, scpd_url) < 0)
	  {
	     ERROR_D(_log_dom, "Could not form complete url for service proxy.\n");
	     return;
	  }

	if (!eupnp_core_http_request_send(complete_url, "GET", NULL, NULL, 0, NULL,
				     EUPNP_REQUEST_DATA_CB(_data_ready),
				     EUPNP_REQUEST_COMPLETED_CB(_download_completed), proxy))
	  {
	     ERROR_D(_log_dom, "Could not add a new download job for device %p\n", proxy);
	     eupnp_service_proxy_unref(proxy);
	  }

	free(complete_url);
     }
}

void
eupnp_service_proxy_dump(Eupnp_Service_Proxy *proxy)
{
   CHECK_NULL_RET(proxy);

   INFO_D(_log_dom, "\tService Proxy dump\n");
   INFO_D(_log_dom, "\t\tversion: %d.%d\n", proxy->spec_version_major, proxy->spec_version_minor);
   INFO_D(_log_dom, "\t\tcontrol URL: %s\n", proxy->control_URL);
   INFO_D(_log_dom, "\t\tbase URL: %s\n", proxy->base_URL);
   eupnp_service_proxy_actions_dump(proxy);
   eupnp_service_proxy_state_table_dump(proxy);
}

void
eupnp_service_proxy_actions_dump(Eupnp_Service_Proxy *proxy)
{
   CHECK_NULL_RET(proxy);

   Eupnp_Service_Action *action;

   EINA_INLIST_FOREACH(proxy->actions, action)
	eupnp_service_action_dump(action);
}

void
eupnp_service_proxy_state_table_dump(Eupnp_Service_Proxy *proxy)
{
   CHECK_NULL_RET(proxy);

   Eupnp_State_Variable *st;

   EINA_INLIST_FOREACH(proxy->state_table, st)
	eupnp_service_state_variable_dump(st);
}

Eina_Bool
eupnp_service_proxy_action_add(Eupnp_Service_Proxy *proxy, Eupnp_Service_Action *action)
{
   CHECK_NULL_RET_VAL(proxy, EINA_FALSE);
   CHECK_NULL_RET_VAL(action, EINA_FALSE);

   proxy->actions = eina_inlist_append(proxy->actions, EINA_INLIST_GET(action));

   return EINA_TRUE;
}

Eina_Bool
eupnp_service_proxy_state_variable_add(Eupnp_Service_Proxy *proxy, Eupnp_State_Variable *st)
{
   CHECK_NULL_RET_VAL(proxy, EINA_FALSE);
   CHECK_NULL_RET_VAL(st, EINA_FALSE);

   proxy->state_table = eina_inlist_append(proxy->state_table, EINA_INLIST_GET(st));

   return EINA_TRUE;
}

Eupnp_State_Variable *
eupnp_service_proxy_state_variable_get(Eupnp_Service_Proxy *proxy, const char *name, int name_len)
{
   CHECK_NULL_RET_VAL(proxy, NULL);
   CHECK_NULL_RET_VAL(name, NULL);
   CHECK_NULL_RET_VAL(proxy->state_table, NULL);

   if (!name_len) name_len = strlen(name);

   Eupnp_State_Variable *st;

   EINA_INLIST_FOREACH(proxy->state_table, st)
     {
	if (!strncmp(st->name, name, name_len))
	   return st;
     }

   return NULL;
}

Eina_Bool
eupnp_service_proxy_has_action(Eupnp_Service_Proxy *proxy, const char *action)
{
   CHECK_NULL_RET_VAL(action, EINA_FALSE);
   CHECK_NULL_RET_VAL(proxy, EINA_FALSE);
   CHECK_NULL_RET_VAL(proxy->actions, EINA_FALSE);

   Eupnp_Service_Action *a;

   EINA_INLIST_FOREACH(proxy->actions, a)
     if (!strcmp(action, a->name)) return EINA_TRUE;

   return EINA_FALSE;
}


Eina_Bool
eupnp_service_proxy_action_send(Eupnp_Service_Proxy *proxy, const char *action, Eupnp_Action_Response_Cb response_cb, void *data, ...)
{
   CHECK_NULL_RET_VAL(proxy, EINA_FALSE);
   CHECK_NULL_RET_VAL(proxy->control_URL, EINA_FALSE);
   CHECK_NULL_RET_VAL(proxy->base_URL, EINA_FALSE);
   CHECK_NULL_RET_VAL(action, EINA_FALSE);

   if (!eupnp_service_proxy_has_action(proxy, action))
      return EINA_FALSE;

   char *message = NULL;
   char *url = NULL;
   char *full_action_uri = NULL;

   if (asprintf(&url, "%s%s", proxy->base_URL, proxy->control_URL) < 0)
     {
	ERROR_D(_log_dom, "Could not mount url for sending action %s\n", action);
	return EINA_FALSE;
     }

   if (asprintf(&full_action_uri, "\"%s#%s\"", proxy->service_type, action) < 0)
     {
	ERROR_D(_log_dom, "Could not mount full action URI.\n");
	goto action_mount_fail;
     }

   Eupnp_Action_Request *req;

   req = malloc(sizeof(Eupnp_Action_Request));

   if (!req)
     {
	ERROR_D(_log_dom, "Failed to create a new action request.\n");
	goto req_alloc_fail;
     }

   DEBUG_D(_log_dom, "Created action request %p\n", req);

   Eina_Array *add_headers = eina_array_new(1);

   if (!add_headers)
     {
	ERROR_D(_log_dom, "Failed to create additional headers array\n");
	goto headers_fail;
     }

   Eupnp_HTTP_Header *header = eupnp_http_header_new("SOAPAction",
						     strlen("SOAPAction"),
						     full_action_uri,
						     strlen(full_action_uri));

   if (!header)
     {
	ERROR_D(_log_dom, "Failed to create soap header\n");
	goto header_fail;
     }

   if (!eina_array_push(add_headers, header))
     {
	ERROR_D(_log_dom, "Failed to push header into headers\n");
	goto array_push_fail;
     }

   req->data = data;
   req->proxy = eupnp_service_proxy_ref(proxy);
   req->response_cb = response_cb;

   /* Build message body */

   if (asprintf(&message,
		"%s%s<u:%s xmlns:u=\"%s\">",
		SOAP_ENVELOPE_BEGIN, SOAP_BODY_BEGIN,
		action, proxy->service_type) < 0)
     {
	ERROR_D(_log_dom, "Failed to create body message for action %s (stage 1)\n", action);
	goto message_fail;
     }

   DEBUG_D(_log_dom, "Message stage 1: %s\n", message);

   va_list va_args;
   const char *arg_name;
   unsigned int arg_type;

   va_start(va_args, data);
   arg_name = va_arg(va_args, const char *);

   // {arg_name, arg_type, arg_value}, ..., NULL!
   while (arg_name)
     {
	// Collect arg_type
	arg_type = va_arg(va_args, unsigned int);

	switch (arg_type)
	  {
	     case EUPNP_TYPE_INT:
		if (!SOAP_ARGUMENT_APPEND(message, arg_name, "%d", int, va_args))
		   ERROR_D(_log_dom, "Failed to append in parameter %s\n", arg_name);
		break;
	     case EUPNP_TYPE_DOUBLE:
		if (!SOAP_ARGUMENT_APPEND(message, arg_name, "%f", double, va_args))
		   ERROR_D(_log_dom, "Failed to append in parameter %s\n", arg_name);
		break;
	     case EUPNP_TYPE_STRING:
		if (!SOAP_ARGUMENT_APPEND(message, arg_name, "%s", const char *, va_args))
		   ERROR_D(_log_dom, "Failed to append in parameter %s\n", arg_name);
		break;
	     default:
		ERROR_D(_log_dom, "Failed to parse argument type.\n");
		goto arg_parse_fail;
	  }

	arg_name = va_arg(va_args, const char *);
     }

   /* End message body */
   if (asprintf(&message,
		"%s</u:%s>%s%s",
		message, action, SOAP_BODY_END, SOAP_ENVELOPE_END) < 0)
     {
	ERROR_D(_log_dom, "Failed to create body message for action %s (stage 3)\n", action);
	goto message_fail;
     }

   DEBUG_D(_log_dom, "Message stage 3: %s\n", message);

   Eupnp_Request request;

   request = eupnp_core_http_request_send(url,
					  NULL,
					  add_headers,
					  "text/xml; charset=utf-8",
					  strlen(message),
					  strdup(message),
					  EUPNP_REQUEST_DATA_CB(_request_data_ready),
					  EUPNP_REQUEST_COMPLETED_CB(_request_completed),
					  req);

   if (!request)
     {
	ERROR_D(_log_dom, "Failed to perform request for action %s\n", action);
	goto request_fail;
     }

   DEBUG_D(_log_dom, "Finished sending request.\n");

   eina_array_free(add_headers);
   eupnp_http_header_free(header);
   free(url);
   free(message);
   free(full_action_uri);

   return EINA_TRUE;

   /* Failure handling */
   request_fail:
   arg_parse_fail:
     va_end(va_args);
     free(message);
   message_fail:
   array_push_fail:
     eupnp_http_header_free(header);
   header_fail:
     eina_array_free(add_headers);
   headers_fail:
     eupnp_action_request_free(req);
   req_alloc_fail:
     free(full_action_uri);
   action_mount_fail:
     free(url);

   return EINA_FALSE;
}

Eina_Bool
eupnp_service_proxy_has_variable(Eupnp_Service_Proxy *proxy, const char *variable_name)
{
   CHECK_NULL_RET_VAL(variable_name, EINA_FALSE);
   CHECK_NULL_RET_VAL(proxy, EINA_FALSE);
   CHECK_NULL_RET_VAL(proxy->state_table, EINA_FALSE);

   Eupnp_State_Variable *st;

   EINA_INLIST_FOREACH(proxy->state_table, st)
     if (!strcmp(variable_name, st->name)) return EINA_TRUE;

   return EINA_FALSE;
}

Eupnp_Event_Subscriber *
eupnp_service_proxy_state_variable_events_subscribe(Eupnp_Service_Proxy *proxy, const char *var_name, Eupnp_State_Variable_Event_Cb cb, Eina_Bool auto_renew, Eina_Bool infinite_subscription, int timeout, void *data)
{
   // TODO use auto-renew
   CHECK_NULL_RET_VAL(proxy, EINA_FALSE);
   CHECK_NULL_RET_VAL(var_name, EINA_FALSE);
   CHECK_NULL_RET_VAL(cb, EINA_FALSE);
   CHECK_NULL_RET_VAL(proxy->base_URL, EINA_FALSE);
   CHECK_NULL_RET_VAL(proxy->eventsub_URL, EINA_FALSE);

   if (!eupnp_service_proxy_has_variable(proxy, var_name)) return NULL;

   DEBUG_D(_log_dom, "Subscribe stage 1\n");

   const char *listen_addr = eupnp_event_server_url_get();
   int id, callback_len, eventing_url_len;
   Eupnp_Event_Subscriber *ret = NULL;

   if (!listen_addr)
     {
	ERROR_D(_log_dom, "Failed to retrieve event server URL.\n");
	return NULL;
     }

   Eupnp_Event_Subscriber *subscriber;
   subscriber = malloc(sizeof(Eupnp_Event_Subscriber));

   if (!subscriber)
     {
	ERROR_D(_log_dom, "Could not alloc a new event subscriber.\n");
	goto subscriber_alloc_fail;
     }

   subscriber->data = data;
   subscriber->cb = cb;
   subscriber->state_var = eupnp_service_proxy_state_variable_get(proxy, var_name, 0);
   subscriber->proxy = eupnp_service_proxy_ref(proxy);
   subscriber->eventing_url = NULL;
   subscriber->callback = NULL;
   subscriber->sid = NULL;
   subscriber->sid_len = 0;

   /* Sent subscription, subscribe for answers */
   if ((id = eupnp_event_server_request_subscribe(EUPNP_CALLBACK(_event_notification), subscriber)) < 0)
     {
	ERROR_D(_log_dom, "Failed to subscribe for data events on event server.\n");
	goto subscribe_err;
     }

   if ((callback_len = asprintf(&subscriber->callback, "<%s/%d>", listen_addr, id)) < 0)
     {
	ERROR_D(_log_dom, "Failed to mount callback URL.\n");
	subscriber->callback = NULL;
	goto listen_url_err;
     }

   if ((eventing_url_len = asprintf(&subscriber->eventing_url, "%s%s", proxy->base_URL, proxy->eventsub_URL)) < 0)
     {
	ERROR_D(_log_dom, "Failed to compose complete event sub URL.\n");
	subscriber->eventing_url = NULL;
	goto eventing_url_fail;
     }

   Eina_Array *add_headers = eina_array_new(5);

   if (!add_headers)
     {
	ERROR_D(_log_dom, "Could not create headers for subscription.\n");
	goto header_array_fail;
     }

   // HOST: domain name or IP address and optional port components of eventing
   // URL, i.e. host:port

   Eupnp_HTTP_Header *host = eupnp_http_header_new("HOST",
						   strlen("HOST"),
						   proxy->base_URL + 7,
						   strlen(proxy->base_URL) - 7);
   if (!host) goto host_header_err;

   Eupnp_HTTP_Header *callback = eupnp_http_header_new("CALLBACK",
						       strlen("CALLBACK"),
						       subscriber->callback,
						       callback_len);
   if (!callback) goto callback_header_err;

   Eupnp_HTTP_Header *nt = eupnp_http_header_new("NT", strlen("NT"),
						 "upnp:event",
						 strlen("upnp:event"));
   if (!nt) goto nt_header_err;

   Eupnp_HTTP_Header *tout;

   DEBUG_D(_log_dom, "Subscribe stage 3\n");

   if (infinite_subscription)
     tout = eupnp_http_header_new("TIMEOUT",
				  strlen("TIMEOUT"),
				  "infinite",
				  strlen("infinite"));
   else
     {
	char *tmp = NULL;

	if (asprintf(&tmp, "Second-%d", timeout) < 0)
	   goto timeout_print_err;

	tout = eupnp_http_header_new("TIMEOUT",
				     strlen("TIMEOUT"),
				     tmp,
				     strlen(tmp));

	free(tmp);

	if (!tout) goto timeout_header_err;
     }

   if (!eina_array_push(add_headers, host))
	goto header_array_push_error;

   if (!eina_array_push(add_headers, callback))
	goto header_array_push_error;

   if (!eina_array_push(add_headers, nt))
	goto header_array_push_error;

   if (!eina_array_push(add_headers, tout))
	goto header_array_push_error;

   Eupnp_Request req;

   req = eupnp_core_http_request_send(subscriber->eventing_url,
				      "SUBSCRIBE",
				      add_headers,
				      NULL,
				      0,
				      NULL,
				      EUPNP_REQUEST_DATA_CB(_subscribe_data_ready),
				      EUPNP_REQUEST_COMPLETED_CB(_subscribe_completed),
				      subscriber);

   if (!req)
     {
	ERROR_D(_log_dom, "Failed to send subscription\n");
	goto req_fail;
     }

   ret = subscriber;

   /* Cleanup temporary variables */
   req_fail:
   header_array_push_error:
     eupnp_http_header_free(tout);
   timeout_header_err:
   timeout_print_err:
     eupnp_http_header_free(nt);
   nt_header_err:
     eupnp_http_header_free(callback);
   callback_header_err:
     eupnp_http_header_free(host);
   host_header_err:
     eina_array_free(add_headers);
   header_array_fail:

   // Passes if error occured, complete cleanup
   if (ret) return ret;

   listen_url_err:
   eventing_url_fail:
   subscribe_err:
     eupnp_event_subscriber_free(subscriber);
   subscriber_alloc_fail:

   return NULL;
}


Eina_Bool
eupnp_service_proxy_state_variable_events_unsubscribe(Eupnp_Event_Subscriber *subscriber)
{
   CHECK_NULL_RET_VAL(subscriber, EINA_FALSE);
   CHECK_NULL_RET_VAL(subscriber->sid, EINA_FALSE);

   Eina_Bool ret = EINA_FALSE;

   DEBUG_D(_log_dom, "Unsubscribing subscriber %p (%s)\n", subscriber, subscriber->sid);

   Eina_Array *add_headers = eina_array_new(2);

   Eupnp_HTTP_Header *host = eupnp_http_header_new("HOST", strlen("HOST"),
						   subscriber->proxy->base_URL + 7,
						   strlen(subscriber->proxy->base_URL) - 7);
   if (!host) goto host_header_err;

   Eupnp_HTTP_Header *sid = eupnp_http_header_new("SID", strlen("SID"),
						  subscriber->sid,
						  subscriber->sid_len);
   if (!sid) goto sid_header_err;

   if (!eina_array_push(add_headers, host))
      goto unsub_header_push_err;
   if (!eina_array_push(add_headers, sid))
      goto unsub_header_push_err;

   Eupnp_Request req;

   req = eupnp_core_http_request_send(subscriber->eventing_url,
				      "UNSUBSCRIBE",
				      add_headers,
				      NULL,
				      0,
				      NULL,
				      EUPNP_REQUEST_DATA_CB(_unsubscribe_data_ready),
				      EUPNP_REQUEST_COMPLETED_CB(_unsubscribe_completed),
				      subscriber);

   if (!req)
     {
	ERROR_D(_log_dom, "Failed to send unsubscribe message for subscriber %p\n", subscriber);
	goto unsub_req_err;
     }

   ret = EINA_TRUE;
   DEBUG_D(_log_dom, "Finished sending unsubscribe message %p (%s)\n", subscriber, subscriber->sid);

   unsub_req_err:
   unsub_header_push_err:
     eupnp_http_header_free(sid);
   sid_header_err:
     eupnp_http_header_free(host);
   host_header_err:
     eina_array_free(add_headers);

   return ret;
}
