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
#include "eupnp_core.h"
#include "eupnp_http_message.h"
#include "eupnp_private.h"

#define SOAP_ENVELOPE_BEGIN "<?xml version=\"1.0\"?>"\
                            "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"" \
                            " s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
#define SOAP_ENVELOPE_END   "</s:Envelope>"
#define SOAP_BODY_BEGIN     "<s:Body>"
#define SOAP_BODY_END       "</s:Body>"

#define SOAP_ARGUMENT_APPEND(message, arg_name, arg_fmt, arg_type, arg_list) \
	       ((asprintf(&message,                                          \
		 "%s<%s>" arg_fmt "</%s>",                                   \
		 message,                                                    \
		 arg_name,                                                   \
		 va_arg(arg_list, arg_type),                                 \
		 arg_name) < 0) ? EINA_FALSE : EINA_TRUE)

#define COPY_CHARACTERS(to, from, len)                          \
   if (!to)                                                     \
     {                                                          \
        to = malloc(sizeof(char)*(len+1));                      \
	if (!to)                                                \
	  {                                                     \
	     ERR_(                          \
	             "Could not alloc for action information"); \
	     return;                                            \
	  }                                                     \
	memcpy((char *)to, from, len);                          \
	((char *)to)[len] = '\0';                               \
     }


typedef struct _Eupnp_Action_Parser Eupnp_Action_Parser;
typedef struct _Eupnp_Action_Parser_Context Eupnp_Action_Parser_Context;
typedef struct _Eupnp_Action_Request Eupnp_Action_Request;

typedef enum {
   ACTION_START,
   ACTION_INSIDE_ENVELOPE,
   ACTION_INSIDE_BODY,
   ACTION_INSIDE_RESPONSE,
   ACTION_INSIDE_ARG,
   ACTION_FINISH,
   ACTION_ERROR
} Eupnp_Action_Parser_State;

struct _Eupnp_Action_Parser_Context {
   Eupnp_Action_Parser_State state;
   int state_skip; /* Used for skipping unknown tags */
   Eupnp_Service_Action_Argument *arg;
   Eina_Inlist *evented_vars;
   xmlParserCtxtPtr ctx;
};

struct _Eupnp_Action_Parser {
   Eupnp_Action_Parser_Context state;
   xmlSAXHandler handler;
   xmlParserCtxtPtr ctx;
};

struct _Eupnp_Action_Request {
   Eupnp_Action_Response_Cb response_cb;
   void *data;
   Eupnp_Service_Proxy *proxy;
   Eupnp_Action_Parser *xml_parser;
   Eina_Inlist *evented_vars;
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

static int _action_log_dom = -1;
static int _log_dom = -1;
extern int EUPNP_ERROR_SERVICE_PARSER_INSUFFICIENT_FEED;
static Eina_Bool _event_server_inited = EINA_FALSE;

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

#define WRN_(...) EINA_LOG_DOM_WARN(_action_log_dom, __VA_ARGS__)
#define ERR_(...) EINA_LOG_DOM_ERR(_action_log_dom, __VA_ARGS__)
#define DBG_(...) EINA_LOG_DOM_DBG(_action_log_dom, __VA_ARGS__)

static void
eupnp_action_parser_chars_cb(void *state, const xmlChar *ch, int len)
{
   Eupnp_Action_Parser_Context *s = state;

   switch(s->state)
     {
	case ACTION_INSIDE_ARG:
	  if (s->arg->value)
	    {
	       // SAX feeds us action OUT arg data progressively, concatenate.
	       char *tmp;
	       char *copy = NULL;

	       COPY_CHARACTERS(copy, ch, len);

	       if (asprintf(&tmp, "%s%s", s->arg->value, copy) < 0)
	         {
		    ERR_("Failed to concatenate result.");
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

	  DBG_("Writing variable %s value: %s", s->arg->name, s->arg->value);
	  break;
     }
}

static void
eupnp_action_parser_error_cb(void *state, const char *msg, ...)
{
   va_list args;
   va_start(args, msg);
   vfprintf(stdout, msg, args);
   va_end(args);
}

static void
eupnp_action_parser_element_start_cb(void *state, const xmlChar *name,
				     const xmlChar *prefix, const xmlChar *URI,
				     int nb_namespaces, const xmlChar **namespaces,
				     int nb_attributes, int nb_defaulted,
				     const xmlChar **attributes)
{
   DBG_("Start NS at %s, prefix=%s, uri=%s, ndefs=%d, nattrs=%d", name, prefix, URI, nb_namespaces, nb_attributes);

   Eupnp_Action_Parser_Context *s = state;

   switch (s->state)
     {
	case ACTION_START:
	  if (!strcmp(name, "Envelope"))
	     s->state = ACTION_INSIDE_ENVELOPE;
	  else
	     s->state = ACTION_ERROR;
	  break;

	case ACTION_INSIDE_ENVELOPE:
	  if (!strcmp(name, "Body"))
	     s->state = ACTION_INSIDE_BODY;
	  else
	     s->state_skip++;
	  break;

	case ACTION_INSIDE_BODY:
	  // Found the first child of body. This tag name should be action +
	  // "Response".
	  // TODO Check if name == action + "Response", here we're just skipping
	  s->state = ACTION_INSIDE_RESPONSE;
	  break;

	case ACTION_INSIDE_RESPONSE:
	  s->arg = (Eupnp_Service_Action_Argument *) eupnp_service_action_argument_new();
	  s->arg->name = strdup(name);
	  DBG_("Writing variable %s", s->arg->name);
	  s->state = ACTION_INSIDE_ARG;
	  break;

	case ACTION_INSIDE_ARG:
	  break;

	default:
	   s->state_skip++;
     }
}

static void
eupnp_action_parser_element_end_cb(void *state, const xmlChar *name,
				   const xmlChar *prefix, const xmlChar *URI)
{
   Eupnp_Action_Parser_Context *s = state;

   DBG_("End element at %s, prefix=%s, URI=%s", name, prefix, URI);

   if (s->state_skip)
     {
	s->state_skip--;
	return;
     }

   switch(s->state)
     {
	case ACTION_INSIDE_ENVELOPE:
	  s->state = ACTION_FINISH;
	  break;

	case ACTION_INSIDE_BODY:
	  s->state = ACTION_INSIDE_ENVELOPE;
	  break;

	case ACTION_INSIDE_RESPONSE:
	  s->state = ACTION_INSIDE_BODY;
	  break;

	case ACTION_INSIDE_ARG:
	  // Finished parsing an OUT arg, append it to the list and move
	  // back to find more args.
	  DBG_("Added variable %p:%s to evented vars list", s->arg, s->arg->name);
	  s->evented_vars = eina_inlist_append(s->evented_vars, EINA_INLIST_GET(s->arg));
	  s->arg = NULL;
	  s->state = ACTION_INSIDE_RESPONSE;
	  break;
     }
}

static Eupnp_Action_Parser *
eupnp_action_parser_new(const char *first_chunk, int first_chunk_len, Eupnp_Action_Request *req)
{
   Eupnp_Action_Parser *p;

   if (first_chunk_len < 4)
     {
	WRN_("First chunk length less than 4 chars, user must provide more than 4.");
	return NULL;
     }

   p = calloc(1, sizeof(Eupnp_Action_Parser));
   if (!p)
     {
	ERR_("Failed to alloc for action parser");
	return NULL;
     }

   p->state.state = ACTION_START;
   p->handler.initialized = XML_SAX2_MAGIC;
   p->handler.characters = &eupnp_action_parser_chars_cb;
   p->handler.error = &eupnp_action_parser_error_cb;
   p->handler.startElementNs = &eupnp_action_parser_element_start_cb;
   p->handler.endElementNs = &eupnp_action_parser_element_end_cb;

   p->ctx = xmlCreatePushParserCtxt(&p->handler, &p->state, first_chunk,
				    first_chunk_len, NULL);
   p->state.ctx = p->ctx;

   /*
    * Force first chunk parse. When not forced, parser gets lazy and
    * does not parse one-big-chunk feeds.
    */
   xmlParseChunk(p->ctx, NULL, 0, 0);

   if (!p->ctx)
     {
	free(p);
	return NULL;
     }

   return p;
}

static void
eupnp_action_parser_free(Eupnp_Action_Parser *p)
{
   if (!p) return;
   if (p->ctx) xmlFreeParserCtxt(p->ctx);
   free(p);
}

static void
eupnp_action_parse_post_parse(Eupnp_Action_Request *req)
{
   Eupnp_Action_Parser *p = req->xml_parser;

   if (p->state.state == ACTION_FINISH) {
      DBG_("Action parsed successfully.");

      xmlParseChunk(p->ctx, NULL, 0, 1);
      req->evented_vars = p->state.evented_vars;
      req->xml_parser = NULL;
      eupnp_action_parser_free(p);
   }
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
eupnp_action_parse_xml_buffer(const char *buffer, int buffer_len, Eupnp_Action_Request *req)
{
   CHECK_NULL_RET(buffer, EINA_FALSE);
   CHECK_NULL_RET(req, EINA_FALSE);
   if (!buffer_len) return EINA_FALSE;

   Eina_Bool ret = EINA_FALSE;
   Eupnp_Action_Parser *parser = NULL;

   if (!req->xml_parser)
     {
	req->xml_parser = eupnp_action_parser_new(buffer, buffer_len, req);

	if (!req->xml_parser)
	  {
	     ERR_("Failed to parse first chunk.");
	     goto parse_ret;
	  }

	ret = EINA_TRUE;
	goto parse_ret;
     }

   parser = req->xml_parser;

   if (!parser->ctx)
     {
	ERR_("chunk_len < 4 case.");
	return EINA_FALSE;
     }

   // Progressive feeds
   if (parser->state.state == ACTION_FINISH)
     {
	WRN_("Already finished parsing");
	ret = EINA_TRUE;
	goto parse_ret;
     }

   DBG_("Parsing XML (%d) at %p", buffer_len, buffer);

   if (!xmlParseChunk(parser->ctx, buffer, buffer_len, 1))
     {
	ret = EINA_TRUE;
	goto parse_ret;
     }

   parse_ret:
     eupnp_action_parse_post_parse(req);
     return ret;
}

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

/**
 * Callbacks for SCPD XML download
 */

static void
_data_ready(void *buffer, int size, void *data)
{
   Eupnp_Service_Proxy *proxy = data;
   eupnp_service_proxy_ref(proxy);

   DBG("Data ready for proxy %p, with xml parser at %p", proxy, proxy->xml_parser);

   if (eupnp_service_parse_buffer(buffer, size, proxy))
     {
	DBG("Parsed XML successfully at buffer %p for proxy %p", buffer, proxy);

	if (!proxy->xml_parser)
	  {
	     DBG("Finished building service proxy %p, forwarding to ready callback.", proxy);
	     proxy->ready_cb(proxy->ready_cb_data, proxy);
	  }
     }
   else
     {
	if (eina_error_get() == EUPNP_ERROR_SERVICE_PARSER_INSUFFICIENT_FEED)
	  {
	     // TODO treat size < 4 case
	     WRN("Len < 4 case.");
	  }

	ERR("Failed to parse XML at buffer %p for proxy %p", buffer, proxy);
     }

   eupnp_service_proxy_unref(proxy);
}

static void
_download_completed(Eupnp_Request request, void *data, const Eupnp_HTTP_Request *req)
{
   Eupnp_Service_Proxy *proxy = data;

   DBG("Proxy download completed %p", proxy);
   eupnp_core_http_request_free(request);

   // Unreference ref made at service_proxy_fetch()
   eupnp_service_proxy_unref(proxy);
}

/**
 * Callbacks for actions request responses
 */

static void
eupnp_request_data_ready(void *buffer, int size, void *data)
{
   Eupnp_Action_Request *req = data;
   Eupnp_Service_Proxy *proxy = req->proxy;

   DBG("Request data ready for proxy %p", proxy);

   if (eupnp_action_parse_xml_buffer(buffer, size, req))
     {
	DBG("Parsed action response XML buffer successfully %p, req %p", buffer, req);

	if (!req->xml_parser)
	  {
	     // parser == NULL means the action response XML parsing is
	     // complete, so forward it to the user.
	     DBG("Finished parsing action response. %p", req);
	     req->response_cb(req->data, req->evented_vars);
	  }
     }
   else
     ERR("Failed to parse action XML at buffer %p for proxy %p", buffer, proxy);
}

static void
eupnp_action_request_free(Eupnp_Action_Request *req)
{
   CHECK_NULL_RET(req);
   eupnp_service_proxy_unref(req->proxy);
   free(req);
}

static void
eupnp_request_completed(Eupnp_Request request, void *data, const Eupnp_HTTP_Request *req)
{
   DBG("Proxy action request completed %p", data);
   Eupnp_Action_Request *action_req = data;
   eupnp_core_http_request_free(request);
   eupnp_action_request_free(action_req);
}

/**
 * Subscription
 */

static void
eupnp_subscription_data_ready(void *buffer, int size, void *data)
{
   Eupnp_Event_Subscriber *sub = data;
   DBG("Subscription answer for req %p", sub);
   // TODO check for errors
}

static void
eupnp_subscription_completed(Eupnp_Request request, void *data, const Eupnp_HTTP_Request *req)
{
   Eupnp_Event_Subscriber *sub = data;
   DBG("Finished subscription %p", sub);
   DBG("Request headers!!! %p", req);

   eupnp_http_request_dump(req);

   const char *sid = eupnp_http_header_get(req->headers, "sid");

   if (!sid)
	ERR("Could not find SID header for request.");
   else
     {
	DBG("SID: %s", sid);
	sub->sid = strdup(sid);
	sub->sid_len = strlen(sub->sid);
     }

   eupnp_core_http_request_free(request);
}

/**
 * Unsubscription
 */

void
eupnp_unsubscribe_data_ready(void *buffer, int size, void *data)
{
   DBG("Unsubscription data %s", (char *)data);
}

void
eupnp_unsubscribe_completed(Eupnp_Request request, void *data, const Eupnp_HTTP_Request *req)
{
   DBG("Finished unsubscription");
   Eupnp_Event_Subscriber *subscriber = data;
   eupnp_http_request_dump(req);
   // TODO check for errors
   eupnp_core_http_request_free(request);

   DBG("Freeing subscriber object after unsubscription.");
   eupnp_event_subscriber_free(subscriber);
}


/**
 * Event notification
 */

static Eina_Bool
eupnp_event_notification_cb(void *data, Eupnp_Event_Type event_type, void *event_data)
{
   DBG("Received event notification, subscriber %p, event %d", data, event_type);
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

   free((char *)proxy->control_url);
   free((char *)proxy->eventsub_url);
   free((char *)proxy->base_url);
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
	     ERR("Could not add a new download job for device %p", proxy);
	     eupnp_service_proxy_unref(proxy);
	  }
     }
   else
     {
	char *complete_url = NULL;

	if ((base_url[strlen(base_url)] == '/' && scpd_url[0] != '/') ||
	    (base_url[strlen(base_url)] != '/' && scpd_url[0] == '/'))
	  {
	     if (asprintf(&complete_url, "%s%s", base_url, scpd_url) < 0)
	       {
		  ERR("Could not form complete url for service proxy.");
		  return;
	       }
	  }
	else
	     if (asprintf(&complete_url, "%s/%s", base_url, scpd_url) < 0)
	       {
		  ERR("Could not form complete url for service proxy.");
		  return;
	       }

	DBG("Complete url is %s", complete_url);

	if (!eupnp_core_http_request_send(complete_url, "GET", NULL, NULL, 0, NULL,
				     EUPNP_REQUEST_DATA_CB(_data_ready),
				     EUPNP_REQUEST_COMPLETED_CB(_download_completed), proxy))
	  {
	     ERR("Could not add a new download job for device %p", proxy);
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

   INF("\tService Proxy dump");
   INF("\t\tversion: %d.%d", proxy->spec_version_major, proxy->spec_version_minor);
   INF("\t\tcontrol URL: %s", proxy->control_url);
   INF("\t\tbase URL: %s", proxy->base_url);
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
Eina_Bool
eupnp_service_proxy_init(void)
{
   if ((_log_dom = eina_log_domain_register("Eupnp.ServiceProxy", EINA_COLOR_BLUE)) < 0)
     {
	EINA_LOG_DOM_ERR
	  (EUPNP_LOGGING_DOM_GLOBAL,
	   "Failed to create logging domain for service proxy module.");
	return EINA_FALSE;
     }

   if ((_action_log_dom = eina_log_domain_register("Eupnp.ActionParser", EINA_COLOR_BLUE)) < 0)
     {
	ERR("Failed to create logging domain for action parser module.");
	return EINA_FALSE;
     }

   INF("Initializing service proxy module.");
   return EINA_TRUE;
}

/**
 * Shuts down the service proxy module.
 *
 * @return 0 if completely shutted down the module.
 */
Eina_Bool
eupnp_service_proxy_shutdown(void)
{
   INF("Shutting down service proxy module.");

   if (_event_server_inited) eupnp_event_server_shutdown();
   eina_log_domain_unregister(_log_dom);
   return EINA_TRUE;
}

/**
 * @brief Creates a service proxy.
 *
 * Creates a service proxy capable of controlling the service. When ready, the
 * proxy is forwarded to 
 *
 * @see eupnp_service_proxy_ref(), eupnp_service_proxy_unref()
 */
void
eupnp_service_proxy_new(const Eupnp_Service_Info *service, Eupnp_Service_Proxy_Ready_Cb ready_cb, void *data)
{
   CHECK_NULL_RET(service);
   CHECK_NULL_RET(service->location);
   CHECK_NULL_RET(service->control_url);
   CHECK_NULL_RET(ready_cb);
   CHECK_NULL_RET(service->scpd_url);
   CHECK_NULL_RET(service->service_type);

   Eupnp_Service_Proxy *proxy;
   proxy = calloc(1, sizeof(Eupnp_Service_Proxy));

   CHECK_NULL_RET(proxy);

   proxy->control_url = strdup(service->control_url);
   proxy->base_url = strdup(service->location);
   proxy->eventsub_url = strdup(service->eventsub_url);
   proxy->service_type = strdup(service->service_type);
   proxy->ready_cb = ready_cb;
   proxy->ready_cb_data = data;
   proxy->refcount = 1;

   eupnp_service_proxy_fetch(proxy, service->location, service->scpd_url);
}

/**
 * @brief Retrieves a new reference of a service proxy.
 *
 * @return A new reference to @p proxy
 */
EAPI Eupnp_Service_Proxy *
eupnp_service_proxy_ref(Eupnp_Service_Proxy *proxy)
{
   CHECK_NULL_RET(proxy, NULL);

   proxy->refcount++;

   DBG("Service proxy %p refcount %d -> %d", proxy,
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

   DBG("Service proxy %p refcount %d -> %d", proxy,
	   proxy->refcount + 1, proxy->refcount);

   if (!proxy->refcount)
      eupnp_service_proxy_free(proxy);
}

EAPI const Eupnp_State_Variable *
eupnp_service_proxy_state_variable_get(const Eupnp_Service_Proxy *proxy, const char *name, int name_len)
{
   CHECK_NULL_RET(proxy, NULL);
   CHECK_NULL_RET(name, NULL);
   CHECK_NULL_RET(proxy->state_table, NULL);

   if (!name_len) name_len = strlen(name);

   const Eupnp_State_Variable *st;

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
   CHECK_NULL_RET(action, EINA_FALSE);
   CHECK_NULL_RET(proxy, EINA_FALSE);
   CHECK_NULL_RET(proxy->actions, EINA_FALSE);

   Eupnp_Service_Action *a;

   EINA_INLIST_FOREACH(proxy->actions, a)
     if (!strcmp(action, a->name)) return EINA_TRUE;

   return EINA_FALSE;
}


EAPI Eina_Bool
eupnp_service_proxy_action_send(Eupnp_Service_Proxy *proxy, const char *action, Eupnp_Action_Response_Cb response_cb, void *data, ...)
{
   CHECK_NULL_RET(proxy, EINA_FALSE);
   CHECK_NULL_RET(proxy->control_url, EINA_FALSE);
   CHECK_NULL_RET(proxy->base_url, EINA_FALSE);
   CHECK_NULL_RET(action, EINA_FALSE);

   Eupnp_Action_Request *req;
   Eupnp_HTTP_Header *header;
   Eina_Array *add_headers;
   Eupnp_Request request;
   char *full_action_uri = NULL;
   char *message = NULL;
   char *url = NULL;
   unsigned int arg_type;
   const char *arg_name;
   va_list va_args;

   if (!eupnp_service_proxy_has_action(proxy, action))
     {
	ERR("Proxy does not contain the requested action.");
	return EINA_FALSE;
     }

   if (asprintf(&url, "%s%s", proxy->base_url, proxy->control_url) < 0)
     {
	ERR("Could not mount url for sending action %s", action);
	return EINA_FALSE;
     }

   if (asprintf(&full_action_uri, "\"%s#%s\"", proxy->service_type, action) < 0)
     {
	ERR("Could not mount full action URI.");
	goto action_mount_fail;
     }

   req = malloc(sizeof(Eupnp_Action_Request));
   if (!req)
     {
	ERR("Failed to create a new action request.");
	goto req_alloc_fail;
     }

   DBG("Created action request %p", req);

   add_headers = eina_array_new(1);
   if (!add_headers)
     {
	ERR("Failed to create additional headers array");
	goto headers_fail;
     }

   header = eupnp_http_header_new("SOAPAction",
				  strlen("SOAPAction"),
				  full_action_uri,
				  strlen(full_action_uri));
   if (!header)
     {
	ERR("Failed to create soap header");
	goto header_fail;
     }

   if (!eina_array_push(add_headers, header))
     {
	ERR("Failed to push header into headers");
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
	ERR("Failed to create body message for action %s (stage 1)", action);
	goto message_fail;
     }

   DBG("Message stage 1: %s", message);

   // {arg_name, arg_type, arg_value}, ..., NULL!
   va_start(va_args, data);
   arg_name = va_arg(va_args, const char *);

   while (arg_name)
     {
	// Collect arg_type
	arg_type = va_arg(va_args, unsigned int);

	switch (arg_type)
	  {
	     case EUPNP_TYPE_INT:
		if (!SOAP_ARGUMENT_APPEND(message, arg_name, "%d", int, va_args))
		   ERR("Failed to append in parameter %s", arg_name);
		break;
	     case EUPNP_TYPE_DOUBLE:
		if (!SOAP_ARGUMENT_APPEND(message, arg_name, "%f", double, va_args))
		   ERR("Failed to append in parameter %s", arg_name);
		break;
	     case EUPNP_TYPE_STRING:
		if (!SOAP_ARGUMENT_APPEND(message, arg_name, "%s", const char *, va_args))
		   ERR("Failed to append in parameter %s", arg_name);
		break;
	     default:
		ERR("Failed to parse argument type.");
		goto arg_parse_fail;
	  }

	arg_name = va_arg(va_args, const char *);
     }

   /* End message body */
   if (asprintf(&message,
		"%s</u:%s>%s%s",
		message, action, SOAP_BODY_END, SOAP_ENVELOPE_END) < 0)
     {
	ERR("Failed to create body message for action %s (stage 3)", action);
	goto message_fail;
     }

   DBG("Message stage 3: %s", message);

   request = eupnp_core_http_request_send(url,
					  NULL,
					  add_headers,
					  "text/xml; charset=utf-8",
					  strlen(message),
					  message,
					  EUPNP_REQUEST_DATA_CB(eupnp_request_data_ready),
					  EUPNP_REQUEST_COMPLETED_CB(eupnp_request_completed),
					  req);

   if (!request)
     {
	ERR("Failed to perform request for action %s", action);
	goto request_fail;
     }

   DBG("Finished sending request.");

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
   CHECK_NULL_RET(variable_name, EINA_FALSE);
   CHECK_NULL_RET(proxy, EINA_FALSE);
   CHECK_NULL_RET(proxy->state_table, EINA_FALSE);

   Eupnp_State_Variable *st;

   EINA_INLIST_FOREACH(proxy->state_table, st)
     if (!strcmp(variable_name, st->name)) return EINA_TRUE;

   return EINA_FALSE;
}

EAPI Eupnp_Event_Subscriber *
eupnp_service_proxy_state_variable_events_subscribe(Eupnp_Service_Proxy *proxy, const char *var_name, Eupnp_State_Variable_Event_Cb cb, Eina_Bool auto_renew, Eina_Bool infinite_subscription, int timeout, void *data)
{
   // TODO use auto-renew
   CHECK_NULL_RET(proxy, EINA_FALSE);
   CHECK_NULL_RET(var_name, EINA_FALSE);
   CHECK_NULL_RET(cb, EINA_FALSE);
   CHECK_NULL_RET(proxy->base_url, EINA_FALSE);
   CHECK_NULL_RET(proxy->eventsub_url, EINA_FALSE);

   Eupnp_Event_Subscriber *subscriber;
   Eupnp_Event_Subscriber *ret = NULL;
   Eupnp_HTTP_Header *host, *callback, *nt, *tout;
   Eina_Array *add_headers;
   Eupnp_Request req;
   const char *listen_addr;
   int id, callback_len, eventing_url_len;

   if (!eupnp_service_proxy_has_variable(proxy, var_name)) return NULL;

   DBG("Subscribe stage 1");

   if (!_event_server_inited)
     {
        if (!eupnp_event_server_init())
	  {
	     ERR("Failed to initialize event server");
	     return NULL;
	  }
	else
	   _event_server_inited = EINA_TRUE;
     }

   listen_addr = (const char *) eupnp_event_server_url_get();
   if (!listen_addr)
     {
	ERR("Failed to retrieve event server URL.");
	return NULL;
     }

   subscriber = calloc(1, sizeof(Eupnp_Event_Subscriber));
   if (!subscriber)
     {
	ERR("Could not alloc a new event subscriber.");
	goto subscriber_alloc_fail;
     }

   subscriber->data = data;
   subscriber->cb = cb;
   subscriber->state_var = (Eupnp_State_Variable *)eupnp_service_proxy_state_variable_get(proxy, var_name, 0);
   subscriber->proxy = eupnp_service_proxy_ref(proxy);

   /* Sent subscription, subscribe for answers */
   if ((id = eupnp_event_server_request_subscribe(EUPNP_CALLBACK(eupnp_event_notification_cb), subscriber)) < 0)
     {
	ERR("Failed to subscribe for data events on event server.");
	goto subscribe_err;
     }

   if ((callback_len = asprintf(&subscriber->callback, "<%s/%d>", listen_addr, id)) < 0)
     {
	ERR("Failed to mount callback URL.");
	subscriber->callback = NULL;
	goto listen_url_err;
     }

   if ((eventing_url_len = asprintf(&subscriber->eventing_url, "%s%s", proxy->base_url, proxy->eventsub_url)) < 0)
     {
	ERR("Failed to compose complete event sub URL.");
	subscriber->eventing_url = NULL;
	goto eventing_url_fail;
     }

   add_headers = eina_array_new(5);
   if (!add_headers)
     {
	ERR("Could not create headers for subscription.");
	goto header_array_fail;
     }

   // HOST: domain name or IP address and optional port components of eventing
   // URL, i.e. host:port

   host = eupnp_http_header_new("HOST", strlen("HOST"), proxy->base_url + 7,
				strlen(proxy->base_url) - 7);
   if (!host) goto host_header_err;

   callback = eupnp_http_header_new("CALLBACK", strlen("CALLBACK"),
				    subscriber->callback, callback_len);
   if (!callback) goto callback_header_err;

   nt = eupnp_http_header_new("NT", strlen("NT"), "upnp:event",
			      strlen("upnp:event"));
   if (!nt) goto nt_header_err;

   DBG("Subscribe stage 3");

   if (infinite_subscription)
     tout = eupnp_http_header_new("TIMEOUT", strlen("TIMEOUT"),
				     "infinite", strlen("infinite"));
   else
     {
	char *tmp = NULL;

	if (asprintf(&tmp, "Second-%d", timeout) < 0)
	   goto timeout_print_err;

	tout = eupnp_http_header_new("TIMEOUT", strlen("TIMEOUT"),
				        tmp,  strlen(tmp));
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

   req = eupnp_core_http_request_send(subscriber->eventing_url,
				      "SUBSCRIBE",
				      add_headers,
				      NULL,
				      0,
				      NULL,
				      EUPNP_REQUEST_DATA_CB(eupnp_subscription_data_ready),
				      EUPNP_REQUEST_COMPLETED_CB(eupnp_subscription_completed),
				      subscriber);
   if (!req)
     {
	ERR("Failed to send subscription");
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
   CHECK_NULL_RET(subscriber, EINA_FALSE);
   CHECK_NULL_RET(subscriber->sid, EINA_FALSE);

   Eina_Bool ret = EINA_FALSE;
   Eina_Array *add_headers;
   Eupnp_HTTP_Header *host, *sid;
   Eupnp_Request req;

   DBG("Unsubscribing subscriber %p (%s)", subscriber, subscriber->sid);

   add_headers = eina_array_new(2);
   host = eupnp_http_header_new("HOST", strlen("HOST"),
				subscriber->proxy->base_url + 7,
				strlen(subscriber->proxy->base_url) - 7);
   if (!host) goto host_header_err;

   sid = eupnp_http_header_new("SID", strlen("SID"), subscriber->sid,
			       subscriber->sid_len);
   if (!sid) goto sid_header_err;

   if (!eina_array_push(add_headers, host))
      goto unsub_header_push_err;
   if (!eina_array_push(add_headers, sid))
      goto unsub_header_push_err;

   req = eupnp_core_http_request_send(subscriber->eventing_url,
				      "UNSUBSCRIBE",
				      add_headers,
				      NULL,
				      0,
				      NULL,
				      EUPNP_REQUEST_DATA_CB(eupnp_unsubscribe_data_ready),
				      EUPNP_REQUEST_COMPLETED_CB(eupnp_unsubscribe_completed),
				      subscriber);
   if (!req)
     {
	ERR("Failed to send unsubscribe message for subscriber %p", subscriber);
	goto unsub_req_err;
     }

   ret = EINA_TRUE;
   DBG("Finished sending unsubscribe message %p (%s)", subscriber, subscriber->sid);

   unsub_req_err:
   unsub_header_push_err:
     eupnp_http_header_free(sid);
   sid_header_err:
     eupnp_http_header_free(host);
   host_header_err:
     eina_array_free(add_headers);

   return ret;
}
