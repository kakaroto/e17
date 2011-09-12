/*
 * Copyright 2010, 2011 Mike Blumenkrantz <mike@zentific.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <regex.h>

#include "Azy.h"
#include "azy_private.h"

static void      _azy_client_handler_call_free(Azy_Client  *client,
                                               Azy_Content *content);
static Eina_Bool _azy_client_handler_get(Azy_Client_Handler_Data *hd);
static Eina_Bool _azy_client_handler_call(Azy_Client_Handler_Data *hd);
static void      _azy_client_handler_data_free(Azy_Client_Handler_Data *data);
static Eina_Bool _azy_client_recv_timer(Azy_Client_Handler_Data *hd);
static void      _azy_client_handler_redirect(Azy_Client_Handler_Data *hd);

static void
_azy_client_handler_call_free(Azy_Client  *client,
                              Azy_Content *content)
{
   Ecore_Cb callback;

   DBG("(client=%p, content=%p)", client, content);

   if (client)
     {
        callback = eina_hash_find(client->free_callbacks, &content->id);
        if (callback)
          {
             callback(content->ret);
             eina_hash_del_by_key(client->free_callbacks, &content->id);
          }
     }
   /* http 1.0 requires that we disconnect after every response */
   if ((!content->recv_net->http.version) || (client && client->net && (!client->net->http.version)))
     ecore_con_server_del(content->recv_net->conn);
   azy_content_free(content);
}

static void
_azy_client_handler_data_free(Azy_Client_Handler_Data *hd)
{
   Eina_List *f;
   DBG("(hd=%p, client=%p, net=%p)", hd, hd->client, hd->client->net);
   if (!AZY_MAGIC_CHECK(hd, AZY_MAGIC_CLIENT_DATA_HANDLER))
     return;
   AZY_MAGIC_SET(hd, AZY_MAGIC_NONE);

   f = eina_list_data_find_list(hd->client->conns, hd);
   if (f)
     {
        hd->client->conns = eina_list_remove_list(hd->client->conns, f);

        if (hd->client->conns)
          {
             if (hd->client->recv)
               ecore_event_handler_data_set(hd->client->recv, hd->client->conns->data);
             if (hd->client->net)
               ecore_con_server_data_set(hd->client->net->conn, hd->client);
          }
        else /* if (client->net && client->recv) */
          {
             if (hd->client->recv)
               ecore_event_handler_data_set(hd->client->recv, NULL);
             if (hd->client->net)
               ecore_con_server_data_set(hd->client->net->conn, NULL);
          }
     }

   if (hd->recv)
     azy_net_free(hd->recv);
   if (hd->send) eina_strbuf_free(hd->send);
   free(hd);
}

Eina_Bool
_azy_client_handler_upgrade(Azy_Client_Handler_Data     *hd,
                            int                          type __UNUSED__,
                            Ecore_Con_Event_Server_Upgrade *ev)
{
   if (!AZY_MAGIC_CHECK(hd, AZY_MAGIC_CLIENT_DATA_HANDLER))
     return ECORE_CALLBACK_RENEW;

   EINA_SAFETY_ON_NULL_RETURN_VAL(hd->client, ECORE_CALLBACK_RENEW);

   if (!hd->client->net)
     {
        INFO("Removing probably dead client %p", hd->client);
        _azy_client_handler_data_free(hd);
        return ECORE_CALLBACK_RENEW;
     }
   if (hd->client != (Azy_Client *)ecore_con_server_data_get(ev->server))
     {
        DBG("Ignoring callback due to pointer mismatch");
        return ECORE_CALLBACK_PASS_ON;
     }

   ecore_event_add(AZY_CLIENT_UPGRADE, hd->client, _azy_event_handler_fake_free, NULL);
   return ECORE_CALLBACK_RENEW;
}

/* FIXME: code dupication */
static Eina_Bool
_azy_client_handler_get(Azy_Client_Handler_Data *hd)
{
   void *ret = NULL;
   Azy_Content *content;
   Azy_Client_Return_Cb cb;
   Azy_Client *client;

   DBG("(hd=%p, client=%p, net=%p)", hd, hd->client, hd->recv);

   client = hd->client;
   hd->recv->transport = azy_events_net_transport_get(azy_net_header_get(hd->recv, "content-type"));
   content = azy_content_new(NULL);

   EINA_SAFETY_ON_NULL_RETURN_VAL(content, ECORE_CALLBACK_RENEW);

   content->data = hd->content_data;

   if (hd->recv->transport == AZY_NET_TRANSPORT_JSON) /* assume block of json */
     {
        if (!azy_content_deserialize(content, hd->recv))
          azy_content_error_faultmsg_set(content, AZY_CLIENT_ERROR_MARSHALIZER, "Call return parsing failed.");
        else if (hd->callback && content->retval && (!hd->callback(content->retval, &ret)))
          azy_content_error_faultmsg_set(content, AZY_CLIENT_ERROR_MARSHALIZER, "Call return value demarshalization failed.");
        if (azy_content_error_is_set(content))
          {
             char buf[64];
             snprintf(buf, sizeof(buf), "%lli bytes:\n<<<<<<<<<<<<<\n%%.%llis\n<<<<<<<<<<<<<", hd->recv->size, hd->recv->size);
             ERR(buf, hd->recv->buffer);
          }
        content->ret = ret;
     }
   else if ((hd->recv->transport == AZY_NET_TRANSPORT_XML) && (!hd->callback)) /* assume rss */
     {
        if (!azy_content_deserialize_rss_xml(content, (const char *)hd->recv->buffer, hd->recv->size))
          {
             if (!azy_content_error_is_set(content))
               azy_content_error_faultmsg_set(content, AZY_CLIENT_ERROR_MARSHALIZER, "Call return parsing failed.");
          }
        if (azy_content_error_is_set(content))
          {
             char buf[64];
             snprintf(buf, sizeof(buf), "%lli bytes:\n<<<<<<<<<<<<<\n%%.%llis\n<<<<<<<<<<<<<", hd->recv->size, hd->recv->size);
             ERR(buf, hd->recv->buffer);
          }
     }
   else
     {
        content->ret = hd->recv->buffer;
        content->retsize = hd->recv->size;
     }
   content->id = hd->id;
   content->recv_net = hd->recv;
   hd->recv = NULL;

   _azy_client_handler_data_free(hd);

   cb = eina_hash_find(client->callbacks, &content->id);
   if (cb)
     {
        Eina_Error r;

        r = cb(client, content, content->ret);

        ecore_event_add(AZY_CLIENT_RESULT, &r, (Ecore_End_Cb)_azy_event_handler_fake_free, NULL);
        eina_hash_del_by_key(client->callbacks, &content->id);
        _azy_client_handler_call_free(client, content);
     }
   else
     {
        if (!azy_content_error_is_set(content))
          ecore_event_add(AZY_CLIENT_RETURN, content, (Ecore_End_Cb)_azy_client_handler_call_free, client);
        else
          ecore_event_add(AZY_CLIENT_ERROR, content, (Ecore_End_Cb)_azy_client_handler_call_free, client);
     }

   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_azy_client_handler_call(Azy_Client_Handler_Data *hd)
{
   void *ret = NULL;
   Azy_Content *content;
   Azy_Client_Return_Cb cb;
   Azy_Client *client;

   DBG("(hd=%p, client=%p, net=%p)", hd, hd->client, hd->recv);

   client = hd->client;
   if (azy_rpc_log_dom >= 0)
     {
        char buf[64];
        snprintf(buf, sizeof(buf), "RECEIVED:\n<<<<<<<<<<<<<\n%%.%llis\n<<<<<<<<<<<<<", hd->recv->size);
        RPC_INFO(buf, hd->recv->buffer);
     }
   /* handle HTTP GET request */
   if (!hd->method) return _azy_client_handler_get(hd);
   INFO("Running RPC for %s", hd->method);
   hd->recv->transport = azy_events_net_transport_get(azy_net_header_get(hd->recv, "content-type"));
   content = azy_content_new(hd->method);

   EINA_SAFETY_ON_NULL_RETURN_VAL(content, ECORE_CALLBACK_RENEW);

   content->data = hd->content_data;

   if (!azy_content_deserialize_response(content, hd->recv->transport, (const char *)hd->recv->buffer, hd->recv->size))
     azy_content_error_faultmsg_set(content, AZY_CLIENT_ERROR_MARSHALIZER, "Call return parsing failed.");
   else if ((hd->recv->transport == AZY_NET_TRANSPORT_JSON) && (content->id != hd->id))
     {
        ERR("Content id: %u  |  Call id: %u", content->id, hd->id);
        azy_content_error_faultmsg_set(content, AZY_CLIENT_ERROR_MARSHALIZER, "Call return id does not match.");
     }
   else if (hd->callback && content->retval && (!hd->callback(content->retval, &ret)))
     azy_content_error_faultmsg_set(content, AZY_CLIENT_ERROR_MARSHALIZER, "Call return value demarshalization failed.");

   if (azy_content_error_is_set(content))
     {
        char buf[64];
        snprintf(buf, sizeof(buf), "%s:\n<<<<<<<<<<<<<\n%%.%llis\n<<<<<<<<<<<<<", hd->method, hd->recv->size);
        ERR(buf, hd->recv->buffer);
     }

   content->id = hd->id;
   content->ret = ret;
   content->recv_net = hd->recv;
   hd->recv = NULL;

   _azy_client_handler_data_free(hd);

   cb = eina_hash_find(client->callbacks, &content->id);
   if (cb)
     {
        Eina_Error ret;
        ret = cb(client, content, content->ret);

        ecore_event_add(AZY_CLIENT_RESULT, &ret, (Ecore_End_Cb)_azy_event_handler_fake_free, NULL);
        eina_hash_del_by_key(client->callbacks, &content->id);
        _azy_client_handler_call_free(client, content);
     }
   else
     {
        if (!azy_content_error_is_set(content))
          ecore_event_add(AZY_CLIENT_RETURN, content, (Ecore_End_Cb)_azy_client_handler_call_free, client);
        else
          ecore_event_add(AZY_CLIENT_ERROR, content, (Ecore_End_Cb)_azy_client_handler_call_free, client);
     }

   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_azy_client_recv_timer(Azy_Client_Handler_Data *hd)
{
   if (!AZY_MAGIC_CHECK(hd, AZY_MAGIC_CLIENT_DATA_HANDLER))
     return ECORE_CALLBACK_CANCEL;
   if (!AZY_MAGIC_CHECK(hd->recv, AZY_MAGIC_NET))
     return ECORE_CALLBACK_CANCEL;

   DBG("(hd=%p, client=%p, net=%p)", hd, hd->client, hd->client->net);

   ecore_con_server_flush(hd->client->net->conn);
   if (hd->recv->size < hd->recv->http.content_length)
     {
        if (!hd->recv->nodata)
          {
             hd->recv->nodata = EINA_TRUE;
             return ECORE_CALLBACK_RENEW;
          }
        INFO("Server at '%s' timed out!", azy_net_ip_get(hd->recv));
        ecore_con_server_del(hd->recv->conn);
        _azy_client_handler_data_free(hd);
        return ECORE_CALLBACK_CANCEL;
     }

   return ECORE_CALLBACK_CANCEL;
}

static void
_azy_client_handler_redirect(Azy_Client_Handler_Data *hd)
{
   const char *location, *next;
   Eina_Strbuf *msg;

   hd->recv->size = 0;
   location = azy_net_header_get(hd->recv, "location");
   if (!location)
     {
        /* FIXME */
         azy_events_connection_kill(hd->client->net->conn, EINA_FALSE, NULL);
         return;
     }
   INFO("Handling HTTP %i: redirect to %s", hd->recv->http.res.http_code, location);
   next = strchr(location, '/');
   if (next && (next - location < 8))
     {
        const char *p;
        p = next;
        p += 1;
        if (p && *p && (*p == '/'))
          p += 1;
        next = strchr(p, '/'); /* try normal uri */
        if (!next)
          next = strchr(p, '?');  /* try php uri */
     }
   if (!next)
     next = "/";  /* flail around wildly hoping someone notices */

   azy_net_uri_set(hd->client->net, next);
   azy_net_type_set(hd->client->net, hd->type);
   msg = azy_net_header_create(hd->client->net);
   EINA_SAFETY_ON_NULL_RETURN(msg);
   if (hd->send)
     {
        azy_net_message_length_set(hd->client->net, eina_strbuf_length_get(hd->send));
        eina_strbuf_prepend_length(hd->send, eina_strbuf_string_get(msg), eina_strbuf_length_get(msg));
        eina_strbuf_free(msg);
     }
   else
     hd->send = msg;

   hd->nodelete = EINA_TRUE;

   if ((!hd->client->net->http.version) || (!hd->recv->http.version)) /* handle http 1.0 */
     {
        azy_events_connection_kill(hd->client->net->conn, EINA_FALSE, NULL);
        azy_client_connect(hd->client, hd->client->secure);
        return;
     }

   EINA_SAFETY_ON_NULL_RETURN(msg);
   /* need to handle redirects, so we get a bit crazy here by sending data without helpers */
   if (hd->method)
     {
        if (!ecore_con_server_send(hd->client->net->conn, eina_strbuf_string_get(hd->send), eina_strbuf_length_get(hd->send))) /* FIXME: wtf do we do here? header sent, no body... */
          {
             ERR("Could not queue data for sending to redirect URI!");
             goto error;
          }
     }
   INFO("Send [1/1] complete! %zu bytes queued for sending.", eina_strbuf_length_get(hd->send));

   return;
error:
   eina_strbuf_free(msg);
}

Eina_Bool
_azy_client_handler_data(Azy_Client_Handler_Data     *hd,
                         int                          type,
                         Ecore_Con_Event_Server_Data *ev)
{
   int offset = 0;
   void *data = (ev) ? ev->data : NULL;
   int len = (ev) ? ev->size : 0;
   static unsigned int recursive;
   static unsigned char *overflow;
   static int64_t overflow_length;
   static Azy_Client *client;

   if (!AZY_MAGIC_CHECK(hd, AZY_MAGIC_CLIENT_DATA_HANDLER))
     return ECORE_CALLBACK_RENEW;

   EINA_SAFETY_ON_NULL_RETURN_VAL(hd->client, ECORE_CALLBACK_RENEW);

   if (!hd->client->net)
     {
        INFO("Removing probably dead client %p", hd->client);
        _azy_client_handler_data_free(hd);
        return ECORE_CALLBACK_RENEW;
     }
   if (hd->client != (Azy_Client *)((ev) ? ecore_con_server_data_get(ev->server) : client))
     {
        DBG("Ignoring callback due to pointer mismatch");
        return ECORE_CALLBACK_PASS_ON;
     }
   DBG("(hd=%p, method='%s', ev=%p, data=%p)", hd, (hd) ? hd->method : NULL, ev, (ev) ? ev->data : NULL);
   DBG("(client=%p, server->client=%p)", hd->client, (ev) ? ecore_con_server_data_get(ev->server) : NULL);

   client = hd->client;
   if (hd->type == AZY_NET_TYPE_GET)
     {
        Azy_Event_Download_Status *dse;

        dse = malloc(sizeof(Azy_Event_Download_Status));
        if (dse)
          {
             dse->id = hd->id;
             dse->size = ev->size;
             dse->client = client;
             dse->net = hd->recv;
             ecore_event_add(AZY_EVENT_DOWNLOAD_STATUS, dse, NULL, NULL);
          }
     }

   if (!hd->recv)
     hd->recv = azy_net_new(hd->client->net->conn);

   EINA_SAFETY_ON_NULL_RETURN_VAL(hd->recv, ECORE_CALLBACK_RENEW);
   hd->recv->nodata = EINA_FALSE;

   if (!hd->recv->size)
     {
        hd->recv->buffer = overflow;
        hd->recv->size = overflow_length;
        INFO("%s: Set recv size to %lli from overflow", hd->method, hd->recv->size);

        /* returns offset where http header line ends */
        if (!(offset = azy_events_type_parse(hd->recv, type, data, len)) && ev && (!hd->recv->http.res.http_msg))
          return azy_events_connection_kill(hd->client->net->conn, EINA_FALSE, NULL);
        else if (!offset && overflow)
          {
             hd->recv->buffer = NULL;
             hd->recv->size = 0;
             INFO("%s: Overflow could not be parsed, set recv size to %lli, storing overflow of %lli", hd->method, hd->recv->size, overflow_length);
             return ECORE_CALLBACK_RENEW;
          }
        else
          {
             overflow = NULL;
             overflow_length = 0;
             INFO("%s: Overflow was parsed! Removing...", hd->method);
          }
     }
   if (!hd->recv->headers_read) /* if headers aren't done being read, keep reading them */
     {
        if (!azy_events_header_parse(hd->recv, data, len, offset) && ev)
          return azy_events_connection_kill(hd->client->net->conn, EINA_FALSE, NULL);
     }
   else if (data) /* otherwise keep appending to buffer */
     {
        unsigned char *tmp;

        if (hd->recv->size + len > hd->recv->http.content_length && (hd->recv->http.content_length > 0))
          tmp = realloc(hd->recv->buffer,
                        hd->recv->http.content_length > 0 ?
                        hd->recv->http.content_length :
                        ev->size - offset);
        else
          tmp = realloc(hd->recv->buffer, hd->recv->size + len);

        EINA_SAFETY_ON_NULL_RETURN_VAL(tmp, ECORE_CALLBACK_RENEW);

        hd->recv->buffer = tmp;

        if ((hd->recv->size + len > hd->recv->http.content_length) &&
            (hd->recv->http.content_length > 0))
          {
             overflow_length = (hd->recv->size + len) - hd->recv->http.content_length;
             memcpy(hd->recv->buffer + hd->recv->size, data, len - overflow_length);
             overflow = malloc(overflow_length);
             if (!overflow)
               {
                  ERR("alloc failure, losing %lli bytes", overflow_length);
                  _azy_client_handler_call(hd);
                  return ECORE_CALLBACK_RENEW;
               }
             memcpy(overflow, data + (len - overflow_length), overflow_length);
             WARN("%s: Extra content length of %lli! Set recv size to %lli (previous %lli)",
                  hd->method, overflow_length, hd->recv->size + len - overflow_length, hd->recv->size);
             hd->recv->size += len - overflow_length;
          }
        else
          {
             memcpy(hd->recv->buffer + hd->recv->size, data, len);
             hd->recv->size += len;

             INFO("%s: Incremented recv size to %lli (+%i)", hd->method, hd->recv->size, len);
          }
     }
   else if (hd->recv->size > hd->recv->http.content_length)
     {
        overflow_length = hd->recv->size - hd->recv->http.content_length;
        overflow = malloc(overflow_length);
        if (!overflow)
          {
             ERR("alloc failure, losing %lli bytes", overflow_length);
             _azy_client_handler_call(hd);
             return ECORE_CALLBACK_RENEW;
          }
        memcpy(overflow, hd->recv->buffer, overflow_length);
        WARN("%s: Extra content length of %lli! Set recv size to %lli (previous %lli)",
             hd->method, overflow_length, hd->recv->http.content_length, hd->recv->size);
        hd->recv->size = hd->recv->http.content_length;
     }

   if ((hd->recv->http.res.http_code >= 301) && (hd->recv->http.res.http_code <= 303)) /* ughhhh redirect */
     {
        _azy_client_handler_redirect(hd);
        return ECORE_CALLBACK_RENEW;
     }

   if (hd->recv->overflow)
     {
        overflow = hd->recv->overflow;
        overflow_length = hd->recv->overflow_length;
        hd->recv->overflow = NULL;
        hd->recv->overflow_length = 0;
     }

   if (!hd->recv->headers_read)
     return ECORE_CALLBACK_RENEW;

   if (hd->recv->size < hd->recv->http.content_length)
     {
        if (!hd->recv->timer)
          /* no timer and full content length not received, start timer */
          hd->recv->timer = ecore_timer_add(30, (Ecore_Task_Cb)_azy_client_recv_timer, hd->recv);
        else
          /* timer and full content length not received, reset timer */
          ecore_timer_interval_set(hd->recv->timer, 30);
     }
   else if (hd->recv->size && (hd->recv->http.content_length > 0))
     {
        /* else create a "done" event */
         if (hd->recv->timer)
           {
              ecore_timer_del(hd->recv->timer);
              hd->recv->timer = NULL;
           }
         _azy_client_handler_call(hd);
     }

   if (overflow && client && client->conns && (hd != client->conns->data))
     {
        Azy_Client_Handler_Data *dh;
        const char *method;
        int64_t prev_len;
        unsigned int id;

        dh = client->conns->data;
        if (dh->recv)
          return ECORE_CALLBACK_RENEW;

        id = dh->id;
        /* ref here in case recursive calls free dh to avoid segv */
        method = eina_stringshare_ref(dh->method);
        WARN("%s:%u (%u): Calling %s recursively to try using %lli bytes of overflow data...", method, id, recursive, __PRETTY_FUNCTION__, overflow_length);
        recursive++;
        prev_len = overflow_length;
        _azy_client_handler_data(dh, type, NULL);
        recursive--;
        if (!overflow)
          WARN("%s:%u (%u): Overflow has been successfully used (%lli bytes)!", method, id, recursive, prev_len - overflow_length);
        else
          WARN("%s:%u (%u): Overflow could not be entirely used (%lli bytes gone), storing %lli bytes for next event.", method, id, recursive, prev_len - overflow_length, overflow_length);
        eina_stringshare_del(method);
     }

   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
_azy_client_handler_del(Azy_Client                 *client,
                        int type                    __UNUSED__,
                        Ecore_Con_Event_Server_Del *ev)
{
   Azy_Client_Handler_Data *hd;

   if (ev && (client != ecore_con_server_data_get(ev->server)) && (ecore_con_server_data_get(ev->server)))
     return ECORE_CALLBACK_PASS_ON;

   DBG("(client=%p, net=%p)", client, client->net);
   client->connected = EINA_FALSE;

   if (client->conns)
     {
        hd = client->conns->data;

        if (hd->recv && hd->recv->buffer && hd->recv->size)
          _azy_client_handler_call(client->conns->data);
        else
          {
             Eina_List *l, *l2;

             EINA_LIST_FOREACH_SAFE(client->conns, l, l2, hd)
               {
                  if (!hd->nodelete)
                    _azy_client_handler_data_free(hd);
               }
          }
     }
   if (client->net && (!client->conns))
     {
        azy_net_free(client->net);
        client->net = NULL;
     }
   else if (client->conns)
     {
        hd = client->conns->data;

        if (hd->recv)
          {
             azy_net_code_set(client->net, hd->recv->http.res.http_code);
             azy_net_free(hd->recv);
          }
        hd->recv = NULL;
     }

   ecore_event_add(AZY_CLIENT_DISCONNECTED, client, (Ecore_End_Cb)_azy_event_handler_fake_free, NULL);
   return ECORE_CALLBACK_CANCEL;
}

Eina_Bool
_azy_client_handler_add(Azy_Client                    *client,
                        int type                       __UNUSED__,
                        Ecore_Con_Event_Server_Add *ev __UNUSED__)
{
   Eina_List *l;
   Azy_Client_Handler_Data *hd;

   if (client != ecore_con_server_data_get(ev->server))
     return ECORE_CALLBACK_PASS_ON;
   DBG("(client=%p, net=%p)", client, client->net);

   client->connected = EINA_TRUE;

   ecore_event_add(AZY_CLIENT_CONNECTED, client, _azy_event_handler_fake_free, NULL);
   EINA_LIST_FOREACH(client->conns, l, hd)
     {
        if (hd->nodelete) /* saved call from redirect, send again */
          {
             EINA_SAFETY_ON_TRUE_RETURN_VAL(
               !ecore_con_server_send(client->net->conn, eina_strbuf_string_get(hd->send), eina_strbuf_length_get(hd->send)),
               ECORE_CALLBACK_CANCEL);
             //CRI("Sent>>>>>>>>>\n%*s\n>>>>>>>>", eina_strbuf_length_get(hd->send), eina_strbuf_string_get(hd->send));
             eina_strbuf_free(hd->send);
             hd->send = NULL;
          }
     }
   return ECORE_CALLBACK_CANCEL;
}

