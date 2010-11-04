#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <regex.h>

#include "Azy.h"
#include "azy_private.h"


static void _azy_client_handler_call_free(Azy_Client *client, Azy_Content *content);
static Eina_Bool _azy_client_handler_call(Azy_Client_Handler_Data *handler_data);
static void _azy_client_handler_data_free(Azy_Client_Handler_Data *data);
static Eina_Bool _azy_client_recv_timer(Azy_Client_Handler_Data *handler_data);

static void
_azy_client_handler_call_free(Azy_Client *client, Azy_Content *content)
{
   void (*callback)(void *);

   if (client)
     {
        callback = eina_hash_find(client->free_callbacks, &content->id);
        if (callback)
          {
             callback(content->ret);
             eina_hash_del_by_key(client->free_callbacks, &content->id);
          }
     }
   azy_content_free(content);
}

static void
_azy_client_handler_data_free(Azy_Client_Handler_Data *handler_data)
{
   DBG("(handler_data=%p, client=%p, net=%p)", handler_data, handler_data->client, handler_data->client->net);
   if (!handler_data)
     return;
   ecore_event_handler_del(handler_data->data);
   azy_net_free(handler_data->recv);
   free(handler_data);
}

static Eina_Bool
_azy_client_handler_call(Azy_Client_Handler_Data *handler_data)
{
   void *ret = NULL;
   Azy_Content *content;
   Azy_Client_Return_Cb cb;
   Azy_Client *client;

   DBG("(handler_data=%p, client=%p, net=%p)", handler_data, handler_data->client, handler_data->recv);

   client = handler_data->client;
#ifdef ISCOMFITOR
   char buf[64];
   snprintf(buf, sizeof(buf), "RECEIVED:\n<<<<<<<<<<<<<\n%%.%llis\n<<<<<<<<<<<<<", handler_data->recv->size);
   INFO(buf, handler_data->recv->buffer);
#endif

   INFO("Running RPC for %s", handler_data->method);
   handler_data->recv->transport = azy_events_net_transport_get(azy_net_header_get(handler_data->recv, "content-type"));
   content = azy_content_new(handler_data->method);
   EINA_SAFETY_ON_NULL_RETURN_VAL(content, EINA_FALSE);

   if (!azy_content_unserialize_response(content, handler_data->recv->transport, (const char*)handler_data->recv->buffer, handler_data->recv->size))
     azy_content_error_faultmsg_set(content, AZY_CLIENT_ERROR_MARSHALIZER, "Call return parsing failed.");
   else if ((handler_data->recv->transport == AZY_NET_JSON) && (content->id != handler_data->id))
     azy_content_error_faultmsg_set(content, AZY_CLIENT_ERROR_MARSHALIZER, "Call return id does not match.");
   else if (!handler_data->callback(azy_content_retval_get(content), &ret))
     azy_content_error_faultmsg_set(content, AZY_CLIENT_ERROR_MARSHALIZER, "Call return value demarshalization failed.");

#ifdef ISCOMFITOR
   if (azy_content_error_is_set(content))
     {
        char buf[64];
        snprintf(buf, sizeof(buf), "%s:\n<<<<<<<<<<<<<\n%%.%llis\n<<<<<<<<<<<<<", handler_data->method, handler_data->recv->size);
        ERR(buf, handler_data->recv->buffer);
     }
#endif

   content->id = handler_data->id;
   content->ret = ret;

   _azy_client_handler_data_free(handler_data);

   cb = eina_hash_find(client->callbacks, &content->id);
   if (cb)
     {
        cb(client, content);
        eina_hash_del_by_key(client->callbacks, &content->id);
        _azy_client_handler_call_free(client, content);
     }
   else
     {
       if (!azy_content_error_is_set(content))
         ecore_event_add(AZY_CLIENT_RETURN, content, (Ecore_End_Cb)_azy_client_handler_call_free, NULL);
       else
         ecore_event_add(AZY_CLIENT_ERROR, content, (Ecore_End_Cb)_azy_client_handler_call_free, NULL);
     }
   client->conns = eina_list_remove(client->conns, handler_data);

   if (client->conns)
     {
        handler_data = client->conns->data;
        /* set handler for current call */
        handler_data->data = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA, (Ecore_Event_Handler_Cb)_azy_client_handler_data, handler_data);
     }
   return EINA_TRUE;
}

static Eina_Bool
_azy_client_recv_timer(Azy_Client_Handler_Data *handler_data)
{
   if (!handler_data->recv)
     return ECORE_CALLBACK_CANCEL;

   DBG("(handler_data=%p, client=%p, net=%p)", handler_data, handler_data->client, handler_data->client->net);

   ecore_con_server_flush(handler_data->client->net->conn);
   if (handler_data->recv->size < handler_data->recv->http.content_length)
     {
        if (!handler_data->recv->nodata)
          {
             handler_data->recv->nodata = EINA_TRUE;
             return ECORE_CALLBACK_RENEW;
          }
        INFO("Server at '%s' timed out!", azy_net_ip_get(handler_data->recv));
        ecore_con_server_del(handler_data->recv->conn); 
        _azy_client_handler_data_free(handler_data);
        return ECORE_CALLBACK_CANCEL;
     }

   return ECORE_CALLBACK_CANCEL;
}

Eina_Bool
_azy_client_handler_data(Azy_Client_Handler_Data    *handler_data,
                          int                          type,
                          Ecore_Con_Event_Server_Data *ev)
{
   int offset = 0;
   void *data = (ev) ? ev->data : NULL;
   int len = (ev) ? ev->size : 0;
   static unsigned char *overflow;
   static long long int overflow_length;
   static Azy_Client *client;

   EINA_SAFETY_ON_NULL_RETURN_VAL(handler_data, ECORE_CALLBACK_RENEW);
   EINA_SAFETY_ON_NULL_RETURN_VAL(handler_data->client, ECORE_CALLBACK_RENEW);
   EINA_SAFETY_ON_NULL_RETURN_VAL(handler_data->client->net, ECORE_CALLBACK_RENEW);
   DBG("(handler_data=%p, method='%s', ev=%p, data=%p)", handler_data, handler_data->method, ev, (ev) ? ev->data : NULL);

#ifdef ISCOMFITOR
   if (data)
     {
        char buf[64];
        snprintf(buf, sizeof(buf), "RECEIVED:\n<<<<<<<<<<<<<\n%%.%is\n<<<<<<<<<<<<<", len);
        INFO(buf, data);
     }
#endif

   client = handler_data->client;
   
   if (!handler_data->recv)
     handler_data->recv = azy_net_new(handler_data->client->net->conn);

   EINA_SAFETY_ON_NULL_RETURN_VAL(handler_data->recv, ECORE_CALLBACK_RENEW);
   handler_data->recv->nodata = EINA_FALSE;

   if (!handler_data->recv->size)
     {
        handler_data->recv->buffer = overflow;
        handler_data->recv->size = overflow_length;
        INFO("%s: Set recv size to %lli from overflow", handler_data->method, handler_data->recv->size);
        
        /* returns offset where http header line ends */
         if (!(offset = azy_events_type_parse(handler_data->recv, type, data, len)) && ev)
           return azy_events_connection_kill(handler_data->client->net->conn, EINA_FALSE, NULL);
         else if (!offset && overflow)
           {
              handler_data->recv->buffer = NULL;
              handler_data->recv->size = 0;
              INFO("%s: Set recv size to %lli, storing overflow of %lli", handler_data->method, handler_data->recv->size, overflow_length);
              return EINA_TRUE;
           }
         else
           {
              overflow = NULL;
              overflow_length = 0;
           }
           
     }
   if (!handler_data->recv->headers_read) /* if headers aren't done being read, keep reading them */
     {
        if (!azy_events_header_parse(handler_data->recv, data, len, offset) && ev)
          return azy_events_connection_kill(handler_data->client->net->conn, EINA_FALSE, NULL);
     }
   else
     {   /* otherwise keep appending to buffer */
        unsigned char *tmp;
        
        if (handler_data->recv->size + len > handler_data->recv->http.content_length)
          tmp = realloc(handler_data->recv->buffer, handler_data->recv->http.content_length);
        else
          tmp = realloc(handler_data->recv->buffer, handler_data->recv->size + len);

        EINA_SAFETY_ON_NULL_RETURN_VAL(tmp, ECORE_CALLBACK_RENEW);
        
        handler_data->recv->buffer = tmp;

        if (handler_data->recv->size + len > handler_data->recv->http.content_length)
          {
             overflow_length = (handler_data->recv->size + len) - handler_data->recv->http.content_length;
             memcpy(handler_data->recv->buffer + handler_data->recv->size, data, len - overflow_length);
             overflow = malloc(overflow_length);
             if (!overflow)
               {
                  ERR("alloc failure, losing %lli bytes", overflow_length);
                  _azy_client_handler_call(handler_data);
                  return ECORE_CALLBACK_RENEW;
               }
             memcpy(overflow, data + (len - overflow_length), overflow_length);
             WARN("%s: Extra content length of %lli! Set recv size to %lli (previous %lli)",
                  handler_data->method, overflow_length, handler_data->recv->size + len - overflow_length, handler_data->recv->size);
             handler_data->recv->size += len - overflow_length;

          }
        else
          {
             memcpy(handler_data->recv->buffer + handler_data->recv->size, data, len);
             handler_data->recv->size += len;

             INFO("%s: Incremented recv size to %lli (+%i)", handler_data->method, handler_data->recv->size, len);
          }
     }

   if (handler_data->recv->overflow)
     {
        overflow = handler_data->recv->overflow;
        overflow_length = handler_data->recv->overflow_length;
        handler_data->recv->overflow = NULL;
        handler_data->recv->overflow_length = 0;
     }

   if (!handler_data->recv->headers_read)
     return ECORE_CALLBACK_RENEW;

   if (handler_data->recv->size < handler_data->recv->http.content_length)
     {
        if (!handler_data->recv->timer)
          /* no timer and full content length not received, start timer */
          handler_data->recv->timer = ecore_timer_add(5, (Ecore_Task_Cb)_azy_client_recv_timer, handler_data->recv);
        else
          /* timer and full content length not received, reset timer */
          ecore_timer_delay(handler_data->recv->timer, 5);
     }
   else
     {
         /* else create a "done" event */
         if (handler_data->recv->timer)
           {
              ecore_timer_del(handler_data->recv->timer);
              handler_data->recv->timer = NULL;
           }
         _azy_client_handler_call(handler_data);
     }

   if (overflow && (handler_data != client->conns->data))
     {
        Azy_Client_Handler_Data *dh;
        char *method;

        dh = client->conns->data;
        if (dh->recv)
          return ECORE_CALLBACK_RENEW;

        method = calloc(sizeof(char), strlen(dh->method) + 1);
        sprintf(method, "%s", dh->method);
        WARN("%s: Calling %s again to try using %lli bytes of overflow data...", method, __PRETTY_FUNCTION__, overflow_length);
        _azy_client_handler_data(dh, type, NULL);
        if (!overflow)
          WARN("%s: Overflow has been successfully used!", method);
        else
          WARN("%s: Overflow could not be used, storing %lli bytes for next event.", method, overflow_length);
        free(method);
     }

   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
_azy_client_handler_del(Azy_Client                    *client,
                         int type                        __UNUSED__,
                         Ecore_Con_Event_Server_Del *del __UNUSED__)
{
   Azy_Client_Handler_Data *handler_data;

   DBG("(client=%p, net=%p)", client, client->net);
   if (!client->connected)
     return ECORE_CALLBACK_RENEW;
   client->connected = EINA_FALSE;
   azy_net_free(client->net);
   client->net = NULL;
   EINA_LIST_FREE(client->conns, handler_data)
     _azy_client_handler_data_free(handler_data);

   ecore_event_add(AZY_CLIENT_DISCONNECTED, client, _azy_event_handler_fake_free, NULL);
   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
_azy_client_handler_add(Azy_Client                    *client,
                         int type                        __UNUSED__,
                         Ecore_Con_Event_Server_Add *add __UNUSED__)
{
   DBG("(client=%p, net=%p)", client, client->net);

   client->connected = EINA_TRUE;

   ecore_event_add(AZY_CLIENT_CONNECTED, client, _azy_event_handler_fake_free, NULL);
   return ECORE_CALLBACK_RENEW;
}
