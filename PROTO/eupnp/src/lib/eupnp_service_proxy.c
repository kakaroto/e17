/** Eupnp - UPnP library
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
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <libxml/SAX2.h>

#include "Eupnp.h"
#include "eupnp_service_proxy.h"
#include "eupnp_log.h"
#include "eupnp_core.h"
#include "eupnp_event_server.h"
#include "eupnp_service_info.h"
#include "eupnp_http_message.h"
#include "eupnp_soap.h"
#include "eupnp_private.h"

/**
 * Private API
 */

/**
 * Appends an argument to a soap call message.
 *
 * FIXME The error above introduces a leak for pointer message.
 */
#define SOAP_ARGUMENT_APPEND(message, arg_name, arg_fmt, arg_type, arg_list) \
	       ((asprintf(&message,                                          \
		 "%s<%s>" arg_fmt "</%s>",                                   \
		 message,                                                    \
		 arg_name,                                                   \
		 va_arg(arg_list, arg_type),                                 \
		 arg_name) < 0) ? EINA_FALSE : EINA_TRUE)


/**
 * Allocates and copies 'len' characters from 'from' to 'to'.
 *
 * Used internally for action response XML parsing.
 */
#define COPY_CHARACTERS(to, from, len)                          \
   if (!to)                                                     \
     {                                                          \
        to = malloc(sizeof(char)*(len+1));                      \
	if (!to)                                                \
	  {                                                     \
	     ERROR_D(_action_log_dom,                           \
	             "Could not alloc for action information"); \
	     return;                                            \
	  }                                                     \
	memcpy((char *)to, from, len);                          \
	((char *)to)[len] = '\0';                               \
     }

/**
 * Action response XML parsing and action request structures
 */

typedef struct _Eupnp_Action_Resp_Parser_State Eupnp_Action_Resp_Parser_State;
typedef struct _Eupnp_Action_Resp_Parser Eupnp_Action_Resp_Parser;
typedef struct _Eupnp_Action_Request Eupnp_Action_Request;

typedef enum {
   START_,
   INSIDE_ENVELOPE,
   INSIDE_BODY,
   INSIDE_ACTION_RESPONSE,
   INSIDE_ACTION_ARG,
   FINISH_,
   ERROR_
} Eupnp_Action_Resp_Parser_State_Enum;

struct _Eupnp_Action_Resp_Parser_State {
   Eupnp_Action_Resp_Parser_State_Enum state;
   int state_skip; /* Used for skipping unknown tags */
   Eupnp_Service_Action_Argument *arg;
   Eina_Inlist *evented_vars;
   xmlParserCtxtPtr ctx;
};

struct _Eupnp_Action_Resp_Parser {
   Eupnp_Action_Resp_Parser_State state;
   xmlSAXHandler handler;
   xmlParserCtxtPtr ctx;
};

struct _Eupnp_Action_Request {
   Eupnp_Action_Response_Cb response_cb;
   void *data;
   Eupnp_Service_Proxy *proxy;
   Eupnp_Action_Resp_Parser *xml_parser;
   Eina_Inlist *evented_vars;
};

static int _action_log_dom = -1;

/**
 * Creates a new action parser state.
 */
static void
eupnp_action_resp_parser_state_new(Eupnp_Action_Resp_Parser_State *s)
{
   s->state = START_;
   s->state_skip = 0;
   s->arg = NULL;
   s->ctx = NULL;
   s->evented_vars = NULL;
}

/**
 * SAX callback for writing OUT arguments received on action response.
 */
static void
_characters(void *state, const xmlChar *ch, int len)
{
   Eupnp_Action_Resp_Parser_State *s = state;

   switch(s->state)
     {
	case INSIDE_ACTION_ARG:
	  if (s->arg->value)
	    {
	       // SAX feeds us action OUT arg data progressively, concatenate.
	       char *tmp;
	       char *copy = NULL;

	       COPY_CHARACTERS(copy, ch, len);

	       if (asprintf(&tmp, "%s%s", s->arg->value, copy) < 0)
	         {
		    ERROR_D(_action_log_dom, "Failed to concatenate result.");
		    free(copy);
		    break;
		 }

	       free((char *)s->arg->value);
	       free(copy);
	       s->arg->value = tmp;
	    }
	  else
	    // First data chunk, possibly the only one
	    COPY_CHARACTERS(s->arg->value, ch, len);

	  DEBUG_D(_action_log_dom, "Writing variable %s value: %s", s->arg->name, s->arg->value);
	  break;
     }
}

/**
 * Prints action response XML parsing error to stdout.
 *
 * FIXME Make it print using the parser assigned logger.
 */
static void
error(void *state, const char *msg, ...)
{
   va_list args;
   va_start(args, msg);
   vfprintf(stdout, msg, args);
   va_end(args);
}

/**
 * SAX callback for the start element event.
 */
static void
start_element_ns(void *state, const xmlChar *name, const xmlChar *prefix, const xmlChar *URI, int nb_namespaces, const xmlChar **namespaces, int nb_attributes, int nb_defaulted, const xmlChar **attributes)
{
   DEBUG_D(_action_log_dom, "Start NS at %s, prefix=%s, uri=%s, ndefs=%d, nattrs=%d", name, prefix, URI, nb_namespaces, nb_attributes);

   Eupnp_Action_Resp_Parser_State *s = state;

   switch (s->state)
     {
	case START_:
	  if (!strcmp(name, "Envelope"))
	     s->state = INSIDE_ENVELOPE;
	  else
	     s->state = ERROR_;
	  break;

	case INSIDE_ENVELOPE:
	  if (!strcmp(name, "Body"))
	     s->state = INSIDE_BODY;
	  else
	     s->state_skip++;
	  break;

	case INSIDE_BODY:
	  // Found the first child of body. This tag name should be action +
	  // "Response".
	  // TODO Check if name == action + "Response", here we're just skipping
	  s->state = INSIDE_ACTION_RESPONSE;
	  break;

	case INSIDE_ACTION_RESPONSE:
	  s->arg = eupnp_service_action_argument_new();
	  s->arg->name = strdup(name);
	  DEBUG_D(_action_log_dom, "Writing variable %s", s->arg->name);
	  s->state = INSIDE_ACTION_ARG;
	  break;

	case INSIDE_ACTION_ARG:
	  break;

	default:
	   s->state_skip++;
     }
}

/**
 * SAX callback for the end element event.
 */
static void
end_element_ns(void *state, const xmlChar *name, const xmlChar *prefix, const xmlChar *URI)
{
   Eupnp_Action_Resp_Parser_State *s = state;

   DEBUG_D(_action_log_dom, "End element at %s, prefix=%s, URI=%s", name, prefix, URI);

   if (s->state_skip)
     {
	s->state_skip--;
	return;
     }

   switch(s->state)
     {
	case INSIDE_ENVELOPE:
	  s->state = FINISH_;
	  break;

	case INSIDE_BODY:
	  s->state = INSIDE_ENVELOPE;
	  break;

	case INSIDE_ACTION_RESPONSE:
	  s->state = INSIDE_BODY;
	  break;

	case INSIDE_ACTION_ARG:
	  // Finished parsing an OUT arg, append it to the list and move
	  // back to find more args.
	  DEBUG_D(_action_log_dom, "Added variable %p:%s to evented vars list", s->arg, s->arg->name);
	  s->evented_vars = eina_inlist_append(s->evented_vars, EINA_INLIST_GET(s->arg));
	  s->arg = NULL;
	  s->state = INSIDE_ACTION_RESPONSE;
	  break;
     }
}

/**
 * Creates a new action response XML parser.
 */
static Eupnp_Action_Resp_Parser *
eupnp_action_resp_parser_new(const char *first_chunk, int first_chunk_len, Eupnp_Action_Request *req)
{
   if (first_chunk_len < 4)
     {
	WARN_D(_action_log_dom, "First chunk length less than 4 chars, user must provide more than 4.");
	return NULL;
     }

   Eupnp_Action_Resp_Parser *p;

   p = malloc(sizeof(Eupnp_Action_Resp_Parser));

   if (!p)
     {
	ERROR_D(_action_log_dom, "Failed to alloc for action resp parser");
	return NULL;
     }

   memset(&p->handler, 0, sizeof(xmlSAXHandler));

   p->handler.initialized = XML_SAX2_MAGIC;
   p->handler.characters = &_characters;
   p->handler.error = &error;
   p->handler.startElementNs = &start_element_ns;
   p->handler.endElementNs = &end_element_ns;

   eupnp_action_resp_parser_state_new(&p->state);

   p->ctx = xmlCreatePushParserCtxt(&p->handler, &p->state, first_chunk,
				    first_chunk_len, NULL);

   p->state.ctx = p->ctx;

   /*
    * Force first chunk parse. When not forced, the parser gets lazy on the
    * first time and doesn't parse one-big-chunk feeds.
    */
   xmlParseChunk(p->ctx, NULL, 0, 0);

   if (!p->ctx)
     {
	free(p);
	return NULL;
     }

   return p;
}

/**
 * Frees an action response XML parser.
 */
static void
eupnp_action_resp_parser_free(Eupnp_Action_Resp_Parser *p)
{
   if (!p) return;
   if (p->ctx) xmlFreeParserCtxt(p->ctx);
   free(p);
}

/**
 * Finishes parsing an action response.
 */
static Eina_Bool
eupnp_action_resp_parse_finish(Eupnp_Action_Request *req)
{
   DEBUG_D(_action_log_dom, "Action resp parse finish");
   Eupnp_Action_Resp_Parser *parser = req->xml_parser;

   Eina_Bool ret;
   ret = xmlParseChunk(parser->ctx, NULL, 0, 1);
   req->evented_vars = parser->state.evented_vars;
   req->xml_parser = NULL;
   eupnp_action_resp_parser_free(parser);
   return !ret;
}

static void
eupnp_action_resp_parse_check_finished(Eupnp_Action_Request *req)
{
   Eupnp_Action_Resp_Parser *p = req->xml_parser;
   if (p->state.state == FINISH_)
      eupnp_action_resp_parse_finish(req);
}

/**
 * High level action response parsing function. This function is supposed to be
 * called progressively if you don't have all data ready at once. You can also
 * send all data in just one call.
 *
 * @note Always send initially buffer_len > 4. If you do not have the required 4
 * characters, accumulate in a buffer and send later (libxml-2 constraint).
 */
static Eina_Bool
eupnp_action_resp_parse_xml_buffer(const char *buffer, int buffer_len, Eupnp_Action_Request *req)
{
   Eina_Bool ret = EINA_FALSE;
   CHECK_NULL_RET_VAL(buffer, ret);
   CHECK_NULL_RET_VAL(req, ret);
   if (!buffer_len) return ret;

   Eupnp_Action_Resp_Parser *parser = NULL;

   if (!req->xml_parser)
     {
	req->xml_parser = eupnp_action_resp_parser_new(buffer, buffer_len, req);

	if (!req->xml_parser)
	  {
	     ERROR_D(_action_log_dom, "Failed to parse first chunk.");
	     goto parse_ret;
	  }

	ret = EINA_TRUE;
	goto parse_ret;
     }

   parser = req->xml_parser;

   if (!parser->ctx)
     {
	ERROR_D(_action_log_dom, "chunk_len < 4 case.");
	return EINA_FALSE;
     }

   // Progressive feeds
   if (parser->state.state == FINISH_)
     {
	WARN_D(_action_log_dom, "Already finished parsing");
	ret = EINA_TRUE;
	goto parse_ret;
     }

   DEBUG_D(_action_log_dom, "Parsing XML (%d) at %p", buffer_len, buffer);

   if (!xmlParseChunk(parser->ctx, buffer, buffer_len, 1))
     {
	ret = EINA_TRUE;
	goto parse_ret;
     }

   parse_ret:
     eupnp_action_resp_parse_check_finished(req);
     return ret;
}

/**
 * Event notification subscription structure
 */
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

/**
 * Frees an event subscription structure when the subscription process is over.
 */
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

static int _eupnp_service_proxy_init_count = 0;
static int _log_dom = -1;
extern int EUPNP_ERROR_SERVICE_PARSER_INSUFFICIENT_FEED;

/**
 * Callbacks for SCPD XML download
 */

static void
_data_ready(void *buffer, int size, void *data)
{
   Eupnp_Service_Proxy *proxy = data;
   eupnp_service_proxy_ref(proxy);
   DEBUG_D(_log_dom, "Data ready for proxy %p, with xml parser at %p", proxy, proxy->xml_parser);

   if (eupnp_service_parse_buffer(buffer, size, proxy))
     {
	DEBUG_D(_log_dom, "Parsed XML successfully at buffer %p for proxy %p", buffer, proxy);

	if (!proxy->xml_parser)
	  {
	     DEBUG_D(_log_dom, "Finished building service proxy %p, forwarding to ready callback.", proxy);
	     proxy->ready_cb(proxy->ready_cb_data, proxy);
	  }
     }
   else
     {
	if (eina_error_get() == EUPNP_ERROR_SERVICE_PARSER_INSUFFICIENT_FEED)
	  {
	     // TODO treat size < 4 case
	     WARN_D(_log_dom, "Len < 4 case.");
	  }

	ERROR_D(_log_dom, "Failed to parse XML at buffer %p for proxy %p", buffer, proxy);
     }

   eupnp_service_proxy_unref(proxy);
}

static void
_download_completed(Eupnp_Request request, void *data, const Eupnp_HTTP_Request *req)
{
   Eupnp_Service_Proxy *proxy = data;
   DEBUG_D(_log_dom, "Proxy download completed %p", proxy);
   eupnp_core_http_request_free(request);

   // Unreference ref made at service_proxy_fetch()
   eupnp_service_proxy_unref(proxy);
}

/**
 * Callbacks for actions request responses
 */

static void
_request_data_ready(void *buffer, int size, void *data)
{
   Eupnp_Action_Request *req = data;
   Eupnp_Service_Proxy *proxy = req->proxy;

   DEBUG_D(_log_dom, "Request data ready for proxy %p", proxy);

   if (eupnp_action_resp_parse_xml_buffer(buffer, size, req))
     {
	DEBUG_D(_log_dom, "Parsed action response XML buffer successfully %p, req %p", buffer, req);

	if (!req->xml_parser)
	  {
	     // parser == NULL means the action response XML parsing is
	     // complete, so forward it to the user.
	     DEBUG_D(_log_dom, "Finished parsing action response. %p", req);
	     req->response_cb(req->data, req->evented_vars);
	  }
     }
   else
     ERROR_D(_log_dom, "Failed to parse action XML at buffer %p for proxy %p", buffer, proxy);
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
   DEBUG_D(_log_dom, "Proxy action request completed %p", data);
   Eupnp_Action_Request *action_req = data;
   eupnp_core_http_request_free(request);
   eupnp_action_request_free(action_req);
}

/**
 * Subscription
 */

static void
_subscribe_data_ready(void *buffer, int size, void *data)
{
   Eupnp_Event_Subscriber *sub = data;
   DEBUG_D(_log_dom, "Subscription answer for req %p", sub);
   // TODO check for errors
}

static void
_subscribe_completed(Eupnp_Request request, void *data, const Eupnp_HTTP_Request *req)
{
   Eupnp_Event_Subscriber *sub = data;
   DEBUG_D(_log_dom, "Finished subscription %p", sub);
   DEBUG_D(_log_dom, "Request headers!!! %p", req);

   eupnp_http_request_dump(req);

   const char *sid = eupnp_http_header_get(req->headers, "sid");

   if (!sid)
	ERROR_D(_log_dom, "Could not find SID header for request.");
   else
     {
	DEBUG_D(_log_dom, "SID: %s", sid);
	sub->sid = strdup(sid);
	sub->sid_len = strlen(sub->sid);
     }

   eupnp_core_http_request_free(request);
}

/**
 * Unsubscription
 */

_unsubscribe_data_ready(void *buffer, int size, void *data)
{
   DEBUG_D(_log_dom, "Unsubscription data %s", (char *)data);
}

_unsubscribe_completed(Eupnp_Request request, void *data, const Eupnp_HTTP_Request *req)
{
   DEBUG_D(_log_dom, "Finished unsubscription");
   Eupnp_Event_Subscriber *subscriber = data;
   eupnp_http_request_dump(req);
   // TODO check for errors
   eupnp_core_http_request_free(request);

   DEBUG_D(_log_dom, "Freeing subscriber object after unsubscription.");
   eupnp_event_subscriber_free(subscriber);
}


/**
 * Event notification
 */

static Eina_Bool
_event_notification(void *data, Eupnp_Event_Type event_type, void *event_data)
{
   DEBUG_D(_log_dom, "Received event notification, subscriber %p, event %d", data, event_type);
   Eupnp_Event_Subscriber *subscriber = data;
   if (!subscriber->cb(subscriber->state_var, event_data, strlen((char *)event_data), subscriber->data))
     {
	eupnp_service_proxy_state_variable_events_unsubscribe(subscriber);
	return EINA_FALSE;
     }
   return EINA_TRUE;
}

/**
 * Frees a proxy.
 *
 * This function is hidden for avoiding major problems. One should use only
 * eupnp_service_proxy_ref() and eupnp_service_proxy_unref().
 */
static void
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

/**
 * Requests the service's SCPD XML for mounting the proxy.
 */
static void
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
	     ERROR_D(_log_dom, "Could not add a new download job for device %p", proxy);
	     eupnp_service_proxy_unref(proxy);
	  }
     }
   else
     {
	char *complete_url = NULL;

	if (asprintf(&complete_url, "%s%s", base_url, scpd_url) < 0)
	  {
	     ERROR_D(_log_dom, "Could not form complete url for service proxy.");
	     return;
	  }

	if (!eupnp_core_http_request_send(complete_url, "GET", NULL, NULL, 0, NULL,
				     EUPNP_REQUEST_DATA_CB(_data_ready),
				     EUPNP_REQUEST_COMPLETED_CB(_download_completed), proxy))
	  {
	     ERROR_D(_log_dom, "Could not add a new download job for device %p", proxy);
	     eupnp_service_proxy_unref(proxy);
	  }

	free(complete_url);
     }
}

static void
eupnp_service_proxy_actions_dump(Eupnp_Service_Proxy *proxy)
{
   CHECK_NULL_RET(proxy);

   Eupnp_Service_Action *action;

   EINA_INLIST_FOREACH(proxy->actions, action)
	eupnp_service_action_dump(action);
}

static void
eupnp_service_proxy_state_table_dump(Eupnp_Service_Proxy *proxy)
{
   CHECK_NULL_RET(proxy);

   Eupnp_State_Variable *st;

   EINA_INLIST_FOREACH(proxy->state_table, st)
	eupnp_service_state_variable_dump(st);
}


static void
eupnp_service_proxy_dump(Eupnp_Service_Proxy *proxy)
{
   CHECK_NULL_RET(proxy);

   INFO_D(_log_dom, "\tService Proxy dump");
   INFO_D(_log_dom, "\t\tversion: %d.%d", proxy->spec_version_major, proxy->spec_version_minor);
   INFO_D(_log_dom, "\t\tcontrol URL: %s", proxy->control_URL);
   INFO_D(_log_dom, "\t\tbase URL: %s", proxy->base_URL);
   eupnp_service_proxy_actions_dump(proxy);
   eupnp_service_proxy_state_table_dump(proxy);
}

/*
 * Public API
 */

/**
* Initializes the service proxy module.
*
* @return On error, returns 0. Otherwise, returns the number of times it's been
* called.
*/
EAPI int
eupnp_service_proxy_init(void)
{
   if (_eupnp_service_proxy_init_count)
      return ++_eupnp_service_proxy_init_count;

   if (!eina_init())
     {
	fprintf(stderr, "Failed to initialize eina module.\n");
	return 0;
     }

   if (!eupnp_log_init())
     {
	fprintf(stderr, "Could not initialize eupnp error module.\n");
	goto error_init_error;
     }

   if ((_log_dom = eina_log_domain_register("Eupnp.ServiceProxy", EINA_COLOR_BLUE)) < 0)
     {
	ERROR("Failed to create logging domain for service proxy module.");
	goto log_dom_error;
     }

   if ((_action_log_dom = eina_log_domain_register("Eupnp.ActionParser", EINA_COLOR_BLUE)) < 0)
     {
	ERROR("Failed to create logging domain for action parser module.");
	goto log_dom_error;
     }

   if (!eupnp_service_parser_init())
     {
	ERROR("Could not initialize eupnp service parser module.");
	goto service_parser_init_error;
     }

   if (!eupnp_event_server_init())
     {
	ERROR("Failed to initialize eupnp event server module.");
	goto evt_server_error;
     }

   INFO_D(_log_dom, "Initializing service proxy module.");

   return ++_eupnp_service_proxy_init_count;

   action_resp_error:
      eupnp_event_server_shutdown();
   evt_server_error:
      eupnp_service_parser_shutdown();
   service_parser_init_error:
      eina_log_domain_unregister(_log_dom);
   log_dom_error:
      eupnp_log_shutdown();
   error_init_error:
      eina_shutdown();

   return 0;
}

/**
 * Shuts down the service proxy module.
 *
 * @return 0 if completely shutted down the module.
 */
EAPI int
eupnp_service_proxy_shutdown(void)
{
   if (_eupnp_service_proxy_init_count != 1)
      return --_eupnp_service_proxy_init_count;

   INFO_D(_log_dom, "Shutting down service proxy module.");

   eupnp_event_server_shutdown();
   eupnp_service_parser_shutdown();
   eina_log_domain_unregister(_log_dom);
   eupnp_log_shutdown();
   eina_shutdown();

   return --_eupnp_service_proxy_init_count;
}

/**
 * @brief Creates a service proxy.
 *
 * Creates a service proxy capable of controlling the service. When ready, the
 * proxy is forwarded to 
 *
 * @see eupnp_service_proxy_ref(), eupnp_service_proxy_unref()
 */
EAPI void
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

/**
 * @brief Retrieves a new reference of a service proxy.
 *
 * @return A new reference to @p proxy
 */
EAPI Eupnp_Service_Proxy *
eupnp_service_proxy_ref(Eupnp_Service_Proxy *proxy)
{
   CHECK_NULL_RET_VAL(proxy, NULL);

   proxy->refcount++;

   DEBUG_D(_log_dom, "Service proxy %p refcount %d -> %d", proxy,
	   proxy->refcount - 1, proxy->refcount);

   return proxy;
}

/**
 * @brief Unreference a proxy.
 *
 * Unreferences a proxy. In case no more references are present, the proxy is
 * collected and therefore unusable.
 */
EAPI void
eupnp_service_proxy_unref(Eupnp_Service_Proxy *proxy)
{
   CHECK_NULL_RET(proxy);

   if (proxy->refcount < 1)
     {
	return;
     }

   proxy->refcount--;

   DEBUG_D(_log_dom, "Service proxy %p refcount %d -> %d", proxy,
	   proxy->refcount + 1, proxy->refcount);

   if (!proxy->refcount)
      eupnp_service_proxy_free(proxy);
}

EAPI Eupnp_State_Variable *
eupnp_service_proxy_state_variable_get(const Eupnp_Service_Proxy *proxy, const char *name, int name_len)
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

EAPI Eina_Bool
eupnp_service_proxy_has_action(const Eupnp_Service_Proxy *proxy, const char *action)
{
   CHECK_NULL_RET_VAL(action, EINA_FALSE);
   CHECK_NULL_RET_VAL(proxy, EINA_FALSE);
   CHECK_NULL_RET_VAL(proxy->actions, EINA_FALSE);

   Eupnp_Service_Action *a;

   EINA_INLIST_FOREACH(proxy->actions, a)
     if (!strcmp(action, a->name)) return EINA_TRUE;

   return EINA_FALSE;
}


EAPI Eina_Bool
eupnp_service_proxy_action_send(Eupnp_Service_Proxy *proxy, const char *action, Eupnp_Action_Response_Cb response_cb, void *data, ...)
{
   CHECK_NULL_RET_VAL(proxy, EINA_FALSE);
   CHECK_NULL_RET_VAL(proxy->control_URL, EINA_FALSE);
   CHECK_NULL_RET_VAL(proxy->base_URL, EINA_FALSE);
   CHECK_NULL_RET_VAL(action, EINA_FALSE);

   if (!eupnp_service_proxy_has_action(proxy, action))
     {
	ERROR_D(_log_dom, "Proxy does not contain the requested action.");
	return EINA_FALSE;
     }

   char *message = NULL;
   char *url = NULL;
   char *full_action_uri = NULL;

   if (asprintf(&url, "%s%s", proxy->base_URL, proxy->control_URL) < 0)
     {
	ERROR_D(_log_dom, "Could not mount url for sending action %s", action);
	return EINA_FALSE;
     }

   if (asprintf(&full_action_uri, "\"%s#%s\"", proxy->service_type, action) < 0)
     {
	ERROR_D(_log_dom, "Could not mount full action URI.");
	goto action_mount_fail;
     }

   Eupnp_Action_Request *req;

   req = malloc(sizeof(Eupnp_Action_Request));

   if (!req)
     {
	ERROR_D(_log_dom, "Failed to create a new action request.");
	goto req_alloc_fail;
     }

   DEBUG_D(_log_dom, "Created action request %p", req);

   Eina_Array *add_headers = eina_array_new(1);

   if (!add_headers)
     {
	ERROR_D(_log_dom, "Failed to create additional headers array");
	goto headers_fail;
     }

   Eupnp_HTTP_Header *header = eupnp_http_header_new("SOAPAction",
						     strlen("SOAPAction"),
						     full_action_uri,
						     strlen(full_action_uri));

   if (!header)
     {
	ERROR_D(_log_dom, "Failed to create soap header");
	goto header_fail;
     }

   if (!eina_array_push(add_headers, header))
     {
	ERROR_D(_log_dom, "Failed to push header into headers");
	goto array_push_fail;
     }

   req->data = data;
   req->proxy = eupnp_service_proxy_ref(proxy);
   req->response_cb = response_cb;
   req->xml_parser = NULL;
   req->evented_vars = NULL;

   /* Build message body */

   if (asprintf(&message,
		"%s%s<u:%s xmlns:u=\"%s\">",
		SOAP_ENVELOPE_BEGIN, SOAP_BODY_BEGIN,
		action, proxy->service_type) < 0)
     {
	ERROR_D(_log_dom, "Failed to create body message for action %s (stage 1)", action);
	goto message_fail;
     }

   DEBUG_D(_log_dom, "Message stage 1: %s", message);

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
		   ERROR_D(_log_dom, "Failed to append in parameter %s", arg_name);
		break;
	     case EUPNP_TYPE_DOUBLE:
		if (!SOAP_ARGUMENT_APPEND(message, arg_name, "%f", double, va_args))
		   ERROR_D(_log_dom, "Failed to append in parameter %s", arg_name);
		break;
	     case EUPNP_TYPE_STRING:
		if (!SOAP_ARGUMENT_APPEND(message, arg_name, "%s", const char *, va_args))
		   ERROR_D(_log_dom, "Failed to append in parameter %s", arg_name);
		break;
	     default:
		ERROR_D(_log_dom, "Failed to parse argument type.");
		goto arg_parse_fail;
	  }

	arg_name = va_arg(va_args, const char *);
     }

   /* End message body */
   if (asprintf(&message,
		"%s</u:%s>%s%s",
		message, action, SOAP_BODY_END, SOAP_ENVELOPE_END) < 0)
     {
	ERROR_D(_log_dom, "Failed to create body message for action %s (stage 3)", action);
	goto message_fail;
     }

   DEBUG_D(_log_dom, "Message stage 3: %s", message);

   Eupnp_Request request;

   // FIXME This strdup below introduces a leak
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
	ERROR_D(_log_dom, "Failed to perform request for action %s", action);
	goto request_fail;
     }

   DEBUG_D(_log_dom, "Finished sending request.");

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

EAPI Eina_Bool
eupnp_service_proxy_has_variable(const Eupnp_Service_Proxy *proxy, const char *variable_name)
{
   CHECK_NULL_RET_VAL(variable_name, EINA_FALSE);
   CHECK_NULL_RET_VAL(proxy, EINA_FALSE);
   CHECK_NULL_RET_VAL(proxy->state_table, EINA_FALSE);

   Eupnp_State_Variable *st;

   EINA_INLIST_FOREACH(proxy->state_table, st)
     if (!strcmp(variable_name, st->name)) return EINA_TRUE;

   return EINA_FALSE;
}

EAPI Eupnp_Event_Subscriber *
eupnp_service_proxy_state_variable_events_subscribe(Eupnp_Service_Proxy *proxy, const char *var_name, Eupnp_State_Variable_Event_Cb cb, Eina_Bool auto_renew, Eina_Bool infinite_subscription, int timeout, void *data)
{
   // TODO use auto-renew
   CHECK_NULL_RET_VAL(proxy, EINA_FALSE);
   CHECK_NULL_RET_VAL(var_name, EINA_FALSE);
   CHECK_NULL_RET_VAL(cb, EINA_FALSE);
   CHECK_NULL_RET_VAL(proxy->base_URL, EINA_FALSE);
   CHECK_NULL_RET_VAL(proxy->eventsub_URL, EINA_FALSE);

   if (!eupnp_service_proxy_has_variable(proxy, var_name)) return NULL;

   DEBUG_D(_log_dom, "Subscribe stage 1");

   const char *listen_addr = eupnp_event_server_url_get();
   int id, callback_len, eventing_url_len;
   Eupnp_Event_Subscriber *ret = NULL;

   if (!listen_addr)
     {
	ERROR_D(_log_dom, "Failed to retrieve event server URL.");
	return NULL;
     }

   Eupnp_Event_Subscriber *subscriber;
   subscriber = malloc(sizeof(Eupnp_Event_Subscriber));

   if (!subscriber)
     {
	ERROR_D(_log_dom, "Could not alloc a new event subscriber.");
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
	ERROR_D(_log_dom, "Failed to subscribe for data events on event server.");
	goto subscribe_err;
     }

   if ((callback_len = asprintf(&subscriber->callback, "<%s/%d>", listen_addr, id)) < 0)
     {
	ERROR_D(_log_dom, "Failed to mount callback URL.");
	subscriber->callback = NULL;
	goto listen_url_err;
     }

   if ((eventing_url_len = asprintf(&subscriber->eventing_url, "%s%s", proxy->base_URL, proxy->eventsub_URL)) < 0)
     {
	ERROR_D(_log_dom, "Failed to compose complete event sub URL.");
	subscriber->eventing_url = NULL;
	goto eventing_url_fail;
     }

   Eina_Array *add_headers = eina_array_new(5);

   if (!add_headers)
     {
	ERROR_D(_log_dom, "Could not create headers for subscription.");
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

   DEBUG_D(_log_dom, "Subscribe stage 3");

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
	ERROR_D(_log_dom, "Failed to send subscription");
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


EAPI Eina_Bool
eupnp_service_proxy_state_variable_events_unsubscribe(Eupnp_Event_Subscriber *subscriber)
{
   CHECK_NULL_RET_VAL(subscriber, EINA_FALSE);
   CHECK_NULL_RET_VAL(subscriber->sid, EINA_FALSE);

   Eina_Bool ret = EINA_FALSE;

   DEBUG_D(_log_dom, "Unsubscribing subscriber %p (%s)", subscriber, subscriber->sid);

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
	ERROR_D(_log_dom, "Failed to send unsubscribe message for subscriber %p", subscriber);
	goto unsub_req_err;
     }

   ret = EINA_TRUE;
   DEBUG_D(_log_dom, "Finished sending unsubscribe message %p (%s)", subscriber, subscriber->sid);

   unsub_req_err:
   unsub_header_push_err:
     eupnp_http_header_free(sid);
   sid_header_err:
     eupnp_http_header_free(host);
   host_header_err:
     eina_array_free(add_headers);

   return ret;
}
