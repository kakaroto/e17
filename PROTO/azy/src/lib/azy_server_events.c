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
#include <ctype.h>
#include "Azy.h"
#include "azy_private.h"

EAPI int AZY_SERVER_CLIENT_ADD;
EAPI int AZY_SERVER_CLIENT_UPGRADE;
EAPI int AZY_SERVER_CLIENT_DEL;

#if 0
static const char *error400 = "HTTP/1.1 400 Bad Request\r\n"
                              "Connection: close\r\n"
                              "Content-Type: text/html\r\n\r\n"
                              "<html>"
                              "<head>"
                              "<title>400 Bad Request</title>"
                              "</head>"
                              "<body>"
                              "<h1>Bad Request</h1>"
                              "<p>The server received a request it could not understand.</p>"
                              "</body>"
                              "</html>";
#endif
static const char *error500 = "HTTP/1.1 500 Internal Server Error\r\n"
                              "Connection: close\r\n\r\n"
                              "Content-Type: text/html\r\n\r\n"
                              "<html>"
                              "<head>"
                              "<title>500 Internal Server Error</title>"
                              "</head>"
                              "<body>"
                              "<h1>Internal Server Error</h1>"
                              "<p>The server encountered an internal error or misconfigurationand was unable to complete your request.</p>"
                              "</body>"
                              "</html>";
static const char *error501 = "HTTP/1.1 501 Method Not Implemented\r\n"
                              "Allow: TRACE\r\n"
                              "Connection: close\r\n\r\n"
                              "Content-Type: text/html\r\n\r\n"
                              "<html>"
                              "<head>"
                              "<title>501 Method Not Implemented</title>"
                              "</head>"
                              "<body>"
                              "<h1>Method Not Implemented</h1>"
                              "</body>"
                              "</html>";

static void                      _azy_server_client_new(Azy_Server       *server,
                                                        Ecore_Con_Client *conn);
static Eina_Bool                 _azy_server_module_free(Azy_Server_Module *module,
                                                         Eina_Bool          shutdown);
static Azy_Server_Module        *_azy_server_module_new(Azy_Server_Module_Def *def,
                                                        Azy_Server_Client     *client);
static Azy_Server_Module_Def    *_azy_server_module_def_find(Azy_Server *server,
                                                             const char *name);
static Azy_Server_Module        *_azy_server_client_module_find(Azy_Server_Client *client,
                                                                const char        *name);
static Azy_Server_Module_Method *_azy_server_module_method_find(Azy_Server_Module *module,
                                                                const char        *name);
static Eina_Bool                 _azy_server_client_method_run(Azy_Server_Client *client,
                                                               Azy_Content       *content);

static void _azy_server_client_send(Azy_Server_Client *client,
                                    Azy_Net_Data      *data,
                                    Azy_Content       *content);
static Eina_Bool _azy_server_client_get_put(Azy_Server_Client *client);
static Eina_Bool _azy_server_client_rpc(Azy_Server_Client *client,
                                        Azy_Net_Transport  t);

static Eina_Bool _azy_server_client_handler_request(Azy_Server_Client *client);
static Eina_Bool _azy_server_client_handler_del(Azy_Server_Client          *client,
                                                int                         type,
                                                Ecore_Con_Event_Client_Del *ev);
static Eina_Bool _azy_server_client_handler_data(Azy_Server_Client           *client,
                                                 int                          type,
                                                 Ecore_Con_Event_Client_Data *ev);
static Eina_Bool _azy_server_client_handler_upgrade(Azy_Server_Client           *cl,
                                                    int type                    __UNUSED__,
                                                    Ecore_Con_Event_Client_Upgrade *ev);
static void _azy_server_client_free(Azy_Server_Client *client);

static Eina_Bool
_azy_server_module_free(Azy_Server_Module *module,
                        Eina_Bool          shutdown)
{
   if (!AZY_MAGIC_CHECK(module, AZY_MAGIC_SERVER_MODULE))
     {
        AZY_MAGIC_FAIL(module, AZY_MAGIC_SERVER_MODULE);
        return EINA_TRUE;
     }

   if (shutdown)
     {
        if ((!module->post) && (module->state < AZY_SERVER_MODULE_STATE_POST) && module->def && module->def->post)
          {
             module->executing = EINA_TRUE;
             module->rewind_now = EINA_FALSE;
top:
             module->def->post(module, NULL);
             if (module->rewind_now) goto top;
             else module->state = AZY_SERVER_MODULE_STATE_POST;
             if (module->suspend)
               {
                  module->executing = EINA_FALSE;
                  return EINA_FALSE;
               }
          }
        if (module->def && module->def->shutdown)
          module->def->shutdown(module);
     }

   if (module->def && module->def->shutdown)
     module->def->shutdown(module);

   if (module->new_net && (module->client->current != module->new_net))
     azy_net_free(module->new_net);
   if (module->params) eina_hash_free(module->params);
   free(module->data);
   free(module);
   return EINA_TRUE;
}

static Azy_Server_Module *
_azy_server_module_new(Azy_Server_Module_Def *def,
                       Azy_Server_Client     *client)
{
   Azy_Server_Module *s;
   DBG("(client=%p)", client);
   if (!AZY_MAGIC_CHECK(def, AZY_MAGIC_SERVER_MODULE_DEF))
     {
        AZY_MAGIC_FAIL(def, AZY_MAGIC_SERVER_MODULE_DEF);
        return NULL;
     }
   if (!AZY_MAGIC_CHECK(client, AZY_MAGIC_SERVER_CLIENT))
     {
        AZY_MAGIC_FAIL(client, AZY_MAGIC_SERVER_CLIENT);
        return NULL;
     }

   s = calloc(1, sizeof(Azy_Server_Module));

   EINA_SAFETY_ON_NULL_RETURN_VAL(s, NULL);
   if (def->data_size > 0)
     {
        s->data = calloc(1, def->data_size);
        if (!s->data)
          {
             ERR("alloc!");
             free(s);
             return NULL;
          }
     }

   s->def = def;
   s->client = client;
   s->run_method = EINA_TRUE;

   AZY_MAGIC_SET(s, AZY_MAGIC_SERVER_MODULE);
   if (s->def->init && !s->def->init(s))
     {
        _azy_server_module_free(s, EINA_FALSE);
        return NULL;
     }

   return s;
}

static Azy_Server_Module_Def *
_azy_server_module_def_find(Azy_Server *server,
                            const char *name)
{
   Eina_List *l;
   Azy_Server_Module_Def *def;

   DBG("(server=%p)", server);
   if (!AZY_MAGIC_CHECK(server, AZY_MAGIC_SERVER))
     {
        AZY_MAGIC_FAIL(server, AZY_MAGIC_SERVER);
        return NULL;
     }
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, NULL);

   EINA_LIST_FOREACH(server->module_defs, l, def)
     {
        if (!strcmp(def->name, name))
          return def;
     }

   return NULL;
}

static Azy_Server_Module_Method *
_azy_server_module_method_find(Azy_Server_Module *module,
                               const char        *name)
{
   Eina_List *l;
   Azy_Server_Module_Method *method;

   DBG("(module=%p, name='%s')", module, name);
   if (!AZY_MAGIC_CHECK(module, AZY_MAGIC_SERVER_MODULE))
     {
        AZY_MAGIC_FAIL(module, AZY_MAGIC_SERVER_MODULE);
        return NULL;
     }
   if (!AZY_MAGIC_CHECK(module->def, AZY_MAGIC_SERVER_MODULE_DEF))
     {
        AZY_MAGIC_FAIL(module->def, AZY_MAGIC_SERVER_MODULE_DEF);
        return NULL;
     }
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, NULL);

   EINA_LIST_FOREACH(module->def->methods, l, method)
     if (!strcmp(method->name, name))
       {
          INFO("Found method with name: '%s'", name);
          return method;
       }

   INFO("Could not find method with name: '%s'", name);
   return NULL;
}

static Eina_Bool
_azy_server_client_method_run(Azy_Server_Client *client,
                              Azy_Content       *content)
{
   Azy_Server_Module *module = NULL;
   Azy_Server_Module_Method *method;
   Azy_Net *net = NULL;
   const char *module_name;

   DBG("(client=%p, content=%p)", client, content);
   if (!AZY_MAGIC_CHECK(client, AZY_MAGIC_SERVER_CLIENT))
     {
        AZY_MAGIC_FAIL(client, AZY_MAGIC_SERVER_CLIENT);
        return EINA_FALSE;
     }
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return EINA_FALSE;
     }

   INFO("Running RPC for %s", content->method);
   /* get Azy_Server_Module object for current connection and given module name */
   if (!(module_name = azy_content_module_name_get(content, client->current->http.req.http_path)))
     {
        azy_content_error_faultmsg_set(content, -1, "Undefined module name.");
        return EINA_FALSE;
     }

   if (!(module = _azy_server_client_module_find(client, module_name)))
     {
        Azy_Server_Module_Def *def;

        def = _azy_server_module_def_find(client->server, module_name);

        eina_stringshare_del(module_name);
        if (!def)
          {
             net = azy_net_new(client->current->conn);
             net->server_client = EINA_TRUE;
             net->http.req.http_path = client->current->http.req.http_path;
             net->type = client->current->type;
             net->transport = client->current->transport;
             client->current->http.req.http_path = NULL;
             net->type = client->current->type;
             net->transport = client->current->transport;
             net->http.version = client->current->http.version;
             azy_net_free(client->current);
             client->current = net;
             azy_content_error_faultmsg_set(content, -1, "Unknown module %s.", module_name);
             return EINA_FALSE;
          }

        if (!(module = _azy_server_module_new(def, client)))
          {
             net = azy_net_new(client->current->conn);
             net->server_client = EINA_TRUE;
             net->http.req.http_path = client->current->http.req.http_path;
             net->type = client->current->type;
             net->transport = client->current->transport;
             client->current->http.req.http_path = NULL;
             net->type = client->current->type;
             net->transport = client->current->transport;
             net->http.version = client->current->http.version;
             azy_net_free(client->current);
             client->current = net;
             azy_content_error_faultmsg_set(content, -1, "Module initialization failed.");
             return EINA_FALSE;
          }

        client->modules = eina_list_append(client->modules, module);
     }
   else
     eina_stringshare_del(module_name);

   module->executing = EINA_TRUE;
   module->post = EINA_FALSE;
   module->content = content;
   method = _azy_server_module_method_find(module, azy_content_method_get(content));

top:
   module->rewind_now = EINA_FALSE;
   switch (module->state)
     {
      case AZY_SERVER_MODULE_STATE_INIT:
        if (!module->new_net)
          {
             net = azy_net_new(module->client->current->conn);
             net->server_client = EINA_TRUE;
          }
        if (module->def->pre)
          module->run_method = module->def->pre(module, net ? net : module->new_net);

        if (net)
          {
             /* grab the req path before it gets freed */
             net->http.req.http_path = module->client->current->http.req.http_path;
             net->type = module->client->current->type;
             net->transport = module->client->current->transport;
             module->new_net = net;
          }

        if (module->rewind_now) goto top;
        else if (module->run_method) module->state = AZY_SERVER_MODULE_STATE_PRE;
        else
          {
             module->state = AZY_SERVER_MODULE_STATE_ERR;
             module->client->current->http.req.http_path = NULL;
             module->new_net->type = client->current->type;
             module->new_net->transport = client->current->transport;
             module->new_net->http.version = client->current->http.version;
             azy_net_free(module->client->current);
             module->client->current = module->new_net;
             goto post;
          }
        if (module->suspend)
          {
             module->executing = EINA_FALSE;
             return EINA_TRUE;
          }

      case AZY_SERVER_MODULE_STATE_PRE:
        module->client->current->http.req.http_path = NULL;
        if (module->new_net && (module->client->current != module->new_net))
          {
             module->new_net->type = client->current->type;
             module->new_net->transport = client->current->transport;
             module->new_net->http.version = client->current->http.version;
             azy_net_free(module->client->current);
             module->client->current = module->new_net;
             module->new_net = NULL;
          }
        if (!module->run_method)
          {
             module->state = AZY_SERVER_MODULE_STATE_ERR;
             goto post;
          }

        if (method)
          client->resume_ret = method->method(module, content);
        else if (module->def->fallback)
          {
             if (module->def->fallback(module, content))
               {
                  if ((!azy_content_retval_get(content)) && (!azy_content_error_is_set(content)))
                    azy_content_error_faultmsg_set(content, -1, "Fallback did not return value or set error.");
               }
             else if (!azy_content_error_is_set(content))
               azy_content_error_faultmsg_set(content, -1, "Method %s not found in %s module.", azy_content_method_get(content), module->def->name);
             client->resume_ret = EINA_FALSE;
          }
        else
          {
             azy_content_error_faultmsg_set(content, -1, "Method %s not found in %s module.", azy_content_method_get(content), module->def->name);
             client->resume_ret = EINA_FALSE;
          }

        if (module->rewind_now) goto top;
        else module->state = AZY_SERVER_MODULE_STATE_METHOD;
        if (module->suspend)
          {
             module->executing = EINA_FALSE;
             return EINA_TRUE;
          }

      case AZY_SERVER_MODULE_STATE_METHOD:
      case AZY_SERVER_MODULE_STATE_ERR:
post:
        module->new_net = NULL;
        if (module->def->post)
          client->resume_ret = module->def->post(module, content);

        if (module->rewind_now) goto top;
        else module->state = AZY_SERVER_MODULE_STATE_POST;
        if (module->suspend)
          {
             module->executing = EINA_FALSE;
             return EINA_TRUE;
          }

      case AZY_SERVER_MODULE_STATE_POST:
      default:
        break;
     }
   module->state = AZY_SERVER_MODULE_STATE_INIT;
   if (module->params) eina_hash_free(module->params);
   module->params = NULL;
   module->content = NULL;
   module->run_method = EINA_TRUE;
   module->post = EINA_TRUE;
   client->resume_rpc = NULL;
   module->executing = EINA_FALSE;
   return client->resume_ret;
}

static void
_azy_server_client_new(Azy_Server       *server,
                       Ecore_Con_Client *conn)
{
   Azy_Server_Client *client;

   if (!AZY_MAGIC_CHECK(server, AZY_MAGIC_SERVER))
     {
        AZY_MAGIC_FAIL(server, AZY_MAGIC_SERVER);
        return;
     }
   client = calloc(1, sizeof(Azy_Server_Client));

   EINA_SAFETY_ON_NULL_RETURN(client);
   client->net = azy_net_new(conn);
   client->net->server_client = EINA_TRUE;
   client->ip = ecore_con_client_ip_get(conn);
   client->server = server;

   ecore_con_client_data_set(conn, client);

   client->del = ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DEL, (Ecore_Event_Handler_Cb)_azy_server_client_handler_del, client);
   client->data = ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DATA, (Ecore_Event_Handler_Cb)_azy_server_client_handler_data, client);
   client->upgrade = ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_UPGRADE, (Ecore_Event_Handler_Cb)_azy_server_client_handler_upgrade, client);

   server->clients++;
   ecore_event_add(AZY_SERVER_CLIENT_ADD, server, _azy_event_handler_fake_free, NULL);

   /* FIXME: is there other data I want to shove into these handlers? */
   AZY_MAGIC_SET(client, AZY_MAGIC_SERVER_CLIENT);
}

static void
_azy_server_client_free(Azy_Server_Client *client)
{
   Azy_Server_Module *s;
   Eina_List *l;
   DBG("(client=%p)", client);

   if (!AZY_MAGIC_CHECK(client, AZY_MAGIC_SERVER_CLIENT))
     {
        AZY_MAGIC_FAIL(client, AZY_MAGIC_SERVER_CLIENT);
        return;
     }
   client->dead = EINA_TRUE;
   if (client->suspend || client->resume || (client->executing && client->resuming)) return;
   EINA_LIST_FOREACH(client->modules, l, s)
     if (s->executing) return;

   EINA_LIST_FREE(client->modules, s)
     if (!_azy_server_module_free(s, EINA_TRUE)) return;

   AZY_MAGIC_SET(client, AZY_MAGIC_NONE);
   ecore_con_client_data_set(client->net->conn, NULL);
   azy_net_free(client->net);
   client->net = NULL;
   if (client->current)
     azy_net_free(client->current);
   client->current = NULL;
   if (client->session_id)
     eina_stringshare_del(client->session_id);

   if (client->data)
     ecore_event_handler_del(client->data);
   if (client->del)
     {
        ecore_event_handler_data_set(client->del, NULL);
        ecore_event_handler_del(client->del);
     }
   if (client->upgrade)
     ecore_event_handler_del(client->upgrade);

   client->server->clients--;
   ecore_event_add(AZY_SERVER_CLIENT_DEL, client->server, _azy_event_handler_fake_free, NULL);
   free(client);
}

static Azy_Server_Module *
_azy_server_client_module_find(Azy_Server_Client *client,
                               const char        *name)
{
   Eina_List *l;
   Azy_Server_Module *module;

   DBG("(client=%p)", client);

   EINA_LIST_FOREACH(client->modules, l, module)
     if (!strcmp(module->def->name, name))
       {
          INFO("Found module with name: '%s'", name);
          return module;
       }

   INFO("Could not find module with name: '%s'", name);
   return NULL;
}

static Eina_Bool
_azy_server_client_get_put(Azy_Server_Client *client)
{
   Eina_List *l;
   Azy_Server_Module *module = NULL;
   Azy_Server_Module_Def *def;
   Azy_Net *net = NULL;
   Azy_Server_Module_Cb cb = NULL;
   Azy_Server_Module_Content_Cb fallback = NULL;

   DBG("(client=%p)", client);
   /* for each available module type, check if it has download hook */
   EINA_LIST_FOREACH(client->server->module_defs, l, def)
     {
        if ((client->current->type == AZY_NET_TYPE_GET) && def->download)
          {
             cb = def->download;
             break;
          }
        if ((client->current->type == AZY_NET_TYPE_PUT) && def->upload)
          {
             cb = def->upload;
             break;
          }
        if (def->fallback) fallback = def->fallback;
     }

   if (cb)
     {
        Azy_Server_Module *existing_module;

        /* check if module exists already */
        EINA_LIST_FOREACH(client->modules, l, existing_module)
          if (existing_module->def == def)
            {
               module = existing_module;
               break;
            }

        if (!module)
          {
             module = _azy_server_module_new(def, client);
             client->modules = eina_list_append(client->modules, module);
             if (module->suspend) return EINA_TRUE;
          }
     }

   if ((!module) || (module->state == AZY_SERVER_MODULE_STATE_ERR))
     {
        Azy_Net_Data data;
not_impl:
        net = azy_net_new(client->current->conn);
        net->server_client = EINA_TRUE;

        /* grab the req path before it gets freed */
        net->http.req.http_path = client->current->http.req.http_path;
        client->current->http.req.http_path = NULL;
        net->type = client->current->type;
        net->transport = client->current->transport;

        azy_net_free(client->current);
        client->current = net;
        net->http.res.http_code = 501;
        net->http.res.http_msg = azy_net_http_msg_get(501);
        azy_net_header_set(net, "Content-Type", NULL);
        azy_net_header_set(net, "Content-Type", "text/plain");
        if (client->current->type == AZY_NET_TYPE_GET)
          {
             data.data = (unsigned char *)"Download hook is not implemented.";
             data.size = sizeof("Download hook is not implemented.") - 1;
          }
        else
          {
             data.data = (unsigned char *)"Upload hook is not implemented.";
             data.size = sizeof("Upload hook is not implemented.") - 1;
          }
        _azy_server_client_send(client, &data, NULL);
        if (module && module->recv.data) free(module->recv.data);
        if (module)
          module->state = AZY_SERVER_MODULE_STATE_INIT;
        return EINA_TRUE;
     }
   module->executing = EINA_TRUE;
   module->post = EINA_FALSE;
top:
   module->rewind_now = EINA_FALSE;
   switch (module->state)
     {
      case AZY_SERVER_MODULE_STATE_INIT:
        if (!module->new_net)
          {
             net = azy_net_new(client->current->conn);
             net->server_client = EINA_TRUE;
          }
        if (def->pre)
          module->run_method = def->pre(module, net ? net : module->new_net);

        module->recv.data = client->current->buffer;
        module->recv.size = client->current->size;
        if (net)
          {
             /* grab the req path before it gets freed */
             net->http.req.http_path = client->current->http.req.http_path;
             net->type = module->client->current->type;
             net->transport = module->client->current->transport;
             module->new_net = net;
          }

        if (module->rewind_now) goto top;
        else if (module->run_method) module->state = AZY_SERVER_MODULE_STATE_PRE;
        else
          {
             module->state = AZY_SERVER_MODULE_STATE_ERR;
             client->current->buffer = NULL; /* prevent buffer from being freed */
             client->current->http.req.http_path = NULL;
             module->new_net->type = client->current->type;
             module->new_net->transport = client->current->transport;
             module->new_net->http.version = client->current->http.version;
             azy_net_free(client->current);
             client->current = module->new_net;
             goto post;
          }
        if (module->suspend)
          {
             module->executing = EINA_FALSE;
             return EINA_TRUE;
          }

      case AZY_SERVER_MODULE_STATE_PRE:
        client->current->buffer = NULL; /* prevent buffer from being freed */
        client->current->http.req.http_path = NULL;
        if (module->new_net && (module->client->current != module->new_net))
          {
             module->new_net->type = client->current->type;
             module->new_net->transport = client->current->transport;
             module->new_net->http.version = client->current->http.version;
             azy_net_free(module->client->current);
             module->client->current = module->new_net;
             module->new_net = NULL;
          }
        if (!module->run_method)
          {
             module->state = AZY_SERVER_MODULE_STATE_ERR;
             goto post;
          }
        if (cb)
          client->resume_ret = cb(module);
        else
          client->resume_ret = fallback(module, NULL);

        if (module->rewind_now) goto top;
        else module->state = client->resume_ret ? AZY_SERVER_MODULE_STATE_METHOD : AZY_SERVER_MODULE_STATE_ERR;
        if (module->suspend)
          {
             module->executing = EINA_FALSE;
             return EINA_TRUE;
          }

      case AZY_SERVER_MODULE_STATE_METHOD:
      case AZY_SERVER_MODULE_STATE_ERR:
post:
        module->new_net = NULL;
        if (!client->resume_ret) goto not_impl;  /* line 581ish (above) */
        if (module->def->post)
          client->resume_ret = module->def->post(module, NULL);

        if (module->rewind_now) goto top;
        else module->state = AZY_SERVER_MODULE_STATE_POST;
        if (module->suspend)
          {
             module->executing = EINA_FALSE;
             return EINA_TRUE;
          }

      default:
        free(module->recv.data);
        module->recv.data = NULL;
     }
   module->run_method = EINA_TRUE;
   module->post = EINA_TRUE;
   module->state = AZY_SERVER_MODULE_STATE_INIT;
   if (module->params) eina_hash_free(module->params);
   module->params = NULL;
   module->executing = EINA_FALSE;
   return client->resume_ret;
}

static void
_azy_server_client_send(Azy_Server_Client *client,
                        Azy_Net_Data      *data,
                        Azy_Content       *content)
{
   Eina_Strbuf *header;
   Azy_Net *net;

   net = client->current;

   DBG("(client=%p, data=%p, content=%p)", client, data, content);
   if (!ecore_con_client_connected_get(net->conn))
     {
        WARN("Filtering reply for already disconnected client %s", ecore_con_client_ip_get(net->conn));
        return;
     }

   if (!net->http.res.http_code)
     {
        net->http.res.http_code = 200;
        net->http.res.http_msg = azy_net_http_msg_get(200);
     }
   if (client->session_id)
     {
        char idstr[48];
        snprintf(idstr, sizeof(idstr), "sessid=%s;", client->session_id);
        azy_net_header_set(net, "Set-Cookie", NULL);
        azy_net_header_set(net, "Set-Cookie", idstr);
     }

   net->type = AZY_NET_TYPE_RESPONSE;
   if (!net->http.content_length)
     {
        if (content && content->length)
          azy_net_message_length_set(net, content->length);
        else if (data && data->size)
          azy_net_message_length_set(net, data->size);
     }

   header = azy_net_header_create(net);
   EINA_SAFETY_ON_NULL_GOTO(header, error);

   if (content)
     INFO("Sending response for method: '%s'", content->method);
   else
     INFO("Sending HTTP: %i", client->net->http.res.http_code);

   if (azy_rpc_log_dom >= 0)
     {
        if (content)
          {
             char buf[64];
             snprintf(buf, sizeof(buf), "SENDING:\n<<<<<<<<<<<<<\n%%.%is%%.%llis\n<<<<<<<<<<<<<", (int)eina_strbuf_length_get(header), content->length);
             RPC_INFO(buf, eina_strbuf_string_get(header), content->buffer);
          }
        else
          RPC_INFO("SENDING:\n<<<<<<<<<<<<<\n%s\n<<<<<<<<<<<<<", eina_strbuf_string_get(header));
     }

   EINA_SAFETY_ON_TRUE_GOTO(!ecore_con_client_send(net->conn, eina_strbuf_string_get(header), eina_strbuf_length_get(header)), error);
   INFO("Send [1/2] complete! %zu bytes queued for sending.", eina_strbuf_length_get(header));
   if (content && content->buffer && content->length)
     {
        EINA_SAFETY_ON_TRUE_GOTO(!ecore_con_client_send(net->conn, content->buffer, content->length), error);
        INFO("Send [2/2] complete! %lli bytes queued for sending.", content->length);
     }
   ecore_con_client_flush(net->conn);
   /* http 1.0 requires that we disconnect after every request has been served */
   if (net->http.version == 0)
     {
        INFO("Disconnecting for HTTP/1.0 compliance");
        client->dead = EINA_TRUE;
        ecore_timer_add(0.00001, (Ecore_Task_Cb)ecore_con_client_del, net->conn);
        net->conn = NULL;
     }

error:
   eina_strbuf_free(header);
}

static Eina_Bool
_azy_server_client_rpc(Azy_Server_Client *client,
                       Azy_Net_Transport  t)
{
   Azy_Content *content;

   DBG("(client=%p)", client);
   if (client->resume_rpc)
     content = client->resume_rpc;
   else
     content = azy_content_new(NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(content, EINA_FALSE);

   if (!client->resume_rpc)
     if ((!azy_content_deserialize_request(content, t, (char *)client->current->buffer, client->current->size)) &&
         (!content->faultcode))
       azy_content_error_faultmsg_set(content, -1, "Unserialize request failure.");

   client->resume_rpc = content;
   if (!content->error_set)
     _azy_server_client_method_run(client, content);

   if (client->suspend) return EINA_TRUE;
   azy_content_serialize_response(content, t);
   azy_net_transport_set(client->current, t);
   _azy_server_client_send(client, NULL, content);

   azy_content_free(content);
   client->resume_rpc = NULL;

   return EINA_TRUE;
}

static Eina_Bool
_azy_server_client_handler_request(Azy_Server_Client *client)
{
   Azy_Net *net;

   DBG("(client=%p)", client);
   client->handled = EINA_TRUE;
   EINA_SAFETY_ON_TRUE_RETURN_VAL(client->dead, EINA_TRUE);
   if (client->suspend)
     {
        INFO("Storing new call for suspended client");
        client->suspended_nets = eina_list_append(client->suspended_nets, client->net);
        client->net = azy_net_new(client->net->conn);
        client->net->server_client = EINA_TRUE;
        return EINA_TRUE;
     }
   if (client->resume)
     client->current = client->resume;
   else
     {
        net = azy_net_new(client->net->conn);
        net->server_client = EINA_TRUE;
        net->transport = client->net->transport;
        net->http.version = client->net->http.version;
        net->type = client->net->type;
        client->current = client->net;
        client->net = net;
     }
   client->executing = EINA_TRUE;
   switch (client->current->type)
     {
      case AZY_NET_TYPE_GET:
      case AZY_NET_TYPE_PUT:
        _azy_server_client_get_put(client);
        client->executing = EINA_FALSE;
        if (!client->suspend)
          {
             azy_net_free(client->current);
             client->current = NULL;
             client->resume_ret = EINA_FALSE;
          }
        return ECORE_CALLBACK_RENEW;

      case AZY_NET_TYPE_POST:
        if (!client->current->transport)
          client->current->transport = azy_events_net_transport_get(azy_net_header_get(client->current, "content-type"));
        switch (client->current->transport)
          {
           case AZY_NET_TRANSPORT_JSON:
           case AZY_NET_TRANSPORT_XML:
             /*case AZY_NET_TRANSPORT_EET:*/
             _azy_server_client_rpc(client, client->current->transport);
             client->executing = EINA_FALSE;
             if (!client->suspend)
               {
                  azy_net_free(client->current);
                  client->current = NULL;
                  client->resume_ret = EINA_FALSE;
               }
             return ECORE_CALLBACK_RENEW;

           case AZY_NET_TRANSPORT_TEXT:
           case AZY_NET_TRANSPORT_HTML:
           default:
             /* FIXME: this isn't supported yet but probably should be somehow? */
             break;
          }

      default:
        break;
     }

   azy_events_connection_kill(client->net->conn, EINA_TRUE, error501);
   _azy_server_client_free(client);
   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_azy_server_client_handler_data(Azy_Server_Client           *client,
                                int                          type,
                                Ecore_Con_Event_Client_Data *ev)
{
   int offset = 0;
   void *data = (ev) ? ev->data : NULL;
   int len = (ev) ? ev->size : 0;
   static unsigned char *overflow;
   static int64_t overflow_length;
   static Azy_Server_Client *cli;

   if (type == -500)
     {
        free(overflow);

        overflow = NULL;
        overflow_length = 0;
        if (client) client->data = NULL;
        return ECORE_CALLBACK_CANCEL;
     }
   if (!client->net)
     {
        /* dead client */
         INFO("Removing probably dead client %p", client);
         client->data = NULL;
         return ECORE_CALLBACK_CANCEL;
     }

   if (client != (Azy_Server_Client *)((ev) ? ecore_con_client_data_get(ev->client) : cli))
     {
        DBG("Ignoring callback due to pointer mismatch");
        return ECORE_CALLBACK_PASS_ON;
     }

   DBG("(client=%p, ev=%p, data=%p)", client, ev, (ev) ? ev->data : NULL);

   cli = client;

   if (azy_rpc_log_dom >= 0)
     {
        char buf[64];
        snprintf(buf, sizeof(buf), "RECEIVED:\n<<<<<<<<<<<<<\n%%.%is\n<<<<<<<<<<<<<", len);
        RPC_INFO(buf, data);
     }

   if (!client->net->size)
     {
        client->net->buffer = overflow;
        client->net->size = overflow_length;
        INFO("%s: Set recv size to %lli from overflow", client->ip, client->net->size);

        /* returns offset where http header line ends */
        if (!(offset = azy_events_type_parse(client->net, type, data, len)) && ev && (!client->net->http.req.http_path))
          {
             overflow = NULL;
             return azy_events_connection_kill(client->net->conn, EINA_TRUE, NULL);
          }
        else if (!offset && overflow)
          {
             client->net->buffer = NULL;
             client->net->size = 0;
             INFO("%s: Overflow could not be parsed, set recv size to %lli, storing overflow of %lli", client->ip, client->net->size, overflow_length);
             return ECORE_CALLBACK_RENEW;
          }
        else
          {
             overflow = NULL;
             overflow_length = 0;
             INFO("%s: Overflow was parsed! Removing...", client->ip);
          }
     }

   if (!client->net->headers_read) /* if headers aren't done being read, keep reading them */
     {
        if (!azy_events_header_parse(client->net, data, len, offset) && ev)
          return azy_events_connection_kill(client->net->conn, EINA_TRUE, error500);
     }
   else if (data) /* otherwise keep appending to buffer */
     {
        unsigned char *tmp;

        if (client->net->size + len > client->net->http.content_length && (client->net->http.content_length > 0))
          tmp = realloc(client->net->buffer,
                        client->net->http.content_length > 0 ?
                        client->net->http.content_length :
                        ev->size - offset);
        else
          tmp = realloc(client->net->buffer, client->net->size + len);

        EINA_SAFETY_ON_NULL_RETURN_VAL(tmp, ECORE_CALLBACK_RENEW);

        client->net->buffer = tmp;

        if ((client->net->size + len > client->net->http.content_length) &&
            (client->net->http.content_length > 0))
          {
             overflow_length = (client->net->size + len) - client->net->http.content_length;
             memcpy(client->net->buffer + client->net->size, data, len - overflow_length);
             overflow = malloc(overflow_length);
             if (!overflow)
               {
                  ERR("alloc failure, losing %lli bytes", overflow_length);
                  _azy_server_client_handler_request(client);
                  return ECORE_CALLBACK_RENEW;
               }
             memcpy(overflow, data + (len - overflow_length), overflow_length);
             WARN("%s: Extra content length of %lli! Set recv size to %lli (previous %lli)",
                  client->ip, overflow_length, client->net->size + len - overflow_length, client->net->size);
             client->net->size += len - overflow_length;
          }
        else
          {
             memcpy(client->net->buffer + client->net->size, data, len);
             client->net->size += len;

             INFO("%s: Incremented recv size to %lli (+%i)", client->ip, client->net->size, len);
          }
     }
   else if (client->net->size > client->net->http.content_length)
     {
        overflow_length = client->net->size - client->net->http.content_length;
        overflow = malloc(overflow_length);
        if (!overflow)
          {
             ERR("alloc failure, losing %lli bytes", overflow_length);
             _azy_server_client_handler_request(client);
             return ECORE_CALLBACK_RENEW;
          }
        memcpy(overflow, client->net->buffer, overflow_length);
        WARN("%s: Extra content length of %lli! Set recv size to %lli (previous %lli)",
             client->ip, overflow_length, client->net->http.content_length, client->net->size);
        client->net->size = client->net->http.content_length;
     }

   if (client->net->overflow)
     {
        overflow = client->net->overflow;
        overflow_length = client->net->overflow_length;
        client->net->overflow = NULL;
        client->net->overflow_length = 0;
     }

   if (!client->net->headers_read)
     return ECORE_CALLBACK_RENEW;

   if ((client->net->size && (client->net->size >= client->net->http.content_length) && (client->net->http.content_length > 0)) ||
       ((!client->net->size) && (client->net->http.content_length < 1) && client->net->headers_read))
     _azy_server_client_handler_request(client);

   if (overflow && (!client->net->buffer))
     {
        WARN("%s: Calling %s again to try using %lli bytes of overflow data...", client->ip, __PRETTY_FUNCTION__, overflow_length);
        _azy_server_client_handler_data(client, type, NULL);
        if (!overflow)
          WARN("%s: Overflow has been successfully used!", client->ip);
        else
          WARN("%s: Overflow could not be used, storing %lli bytes for next event.", client->ip, overflow_length);
     }

   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_azy_server_client_handler_del(Azy_Server_Client          *client,
                               int type                    __UNUSED__,
                               Ecore_Con_Event_Client_Del *ev)
{
   DBG("(client=%p, server->client=%p)", client, (ev) ? ecore_con_client_data_get(ev->client) : NULL);
   if (ev && (client != ecore_con_client_data_get(ev->client)))
     {
        DBG("Ignoring callback due to pointer mismatch");
        return ECORE_CALLBACK_PASS_ON;
     }

   if (ev)
     INFO("Client %s has disconnected!", ecore_con_client_ip_get(ev->client));
   else
     INFO("Client has disconnected!");

   if (!ecore_con_server_clients_get(client->server->server))
     _azy_server_client_handler_data(NULL, -500, NULL);

   client->del = NULL;
   if ((!client->net->http.version) && (!client->handled) && client->net->buffer)
     _azy_server_client_handler_request(client);

   _azy_server_client_free(client);

   return ECORE_CALLBACK_CANCEL;
}

static Eina_Bool
_azy_server_client_handler_upgrade(Azy_Server_Client           *cl,
                                   int type                    __UNUSED__,
                                   Ecore_Con_Event_Client_Upgrade *ev)
{
   DBG("(cl=%p, cl->client=%p)", cl, ecore_con_server_data_get(ecore_con_client_server_get(ev->client)));
   if (cl != ecore_con_server_data_get(ecore_con_client_server_get(ev->client)))
     {
        DBG("Ignoring callback due to pointer mismatch");
        return ECORE_CALLBACK_PASS_ON;
     }

   ecore_event_add(AZY_SERVER_CLIENT_UPGRADE, cl, _azy_event_handler_fake_free, NULL);
   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
azy_server_client_handler_add(Azy_Server                 *server,
                              int type                    __UNUSED__,
                              Ecore_Con_Event_Client_Add *ev)
{
   DBG("(server=%p, server->client=%p)", server, ecore_con_server_data_get(ecore_con_client_server_get(ev->client)));
   if (server != ecore_con_server_data_get(ecore_con_client_server_get(ev->client)))
     {
        DBG("Ignoring callback due to pointer mismatch");
        return ECORE_CALLBACK_PASS_ON;
     }

   INFO("Client %s has connected!", ecore_con_client_ip_get(ev->client));
   //ecore_con_client_timeout_set(ev->client, 3);
   _azy_server_client_new(server, ev->client);

   return ECORE_CALLBACK_RENEW;
}

/**
 * @defgroup Azy_Server_Module_Advanced Advanced Server Module Functions
 * @brief Advanced functions which affect #Azy_Server_Module objects
 * @{
 */
/**
 * @brief Resume a suspended client's activities
 * This function is used on a suspended client to resume it,
 * allowing the current and subsequent transmissions to begin processing again.
 * The success value of the current directive is determined by @p ret.
 * @param module The suspended module (NOT #NULL)
 * @param ret The return value to set for the current directive
 */
void
azy_server_module_events_resume(Azy_Server_Module *module, Eina_Bool ret)
{
   Azy_Server_Client *client;
   Azy_Net *net;

   DBG("(module=%p)", module);

   if (!AZY_MAGIC_CHECK(module, AZY_MAGIC_SERVER_MODULE))
     {
        AZY_MAGIC_FAIL(module, AZY_MAGIC_SERVER_MODULE);
        return;
     }
   client = module->client;
   client->resuming = EINA_TRUE;
   if (client->dead && (!client->executing))
     {
        if (client->resume) azy_net_free(client->resume);
        client->resume = NULL;
        if (client->resume_rpc) azy_content_free(client->resume_rpc);
        client->suspend = EINA_FALSE;
        EINA_LIST_FREE(client->suspended_nets, net)
          azy_net_free(net);
        _azy_server_client_free(module->client);
        return;
     }
   if (!module->suspend)
     {
        WARN("Module is currently active, this function has no effect!");
        client->resuming = EINA_FALSE;
        return;
     }

   module->suspend = EINA_FALSE;
   client->suspend = EINA_FALSE;
   if (module->executing)
     {
        if (module->rewind) module->rewind_now = EINA_TRUE;
        module->rewind = EINA_FALSE;
        module->client->resume = NULL;
        client->resuming = EINA_FALSE;
        return;
     }
   if (module->rewind) module->state--;
   switch (module->state)
     {
      case AZY_SERVER_MODULE_STATE_PRE:
        module->run_method = ret;
        break;
      case AZY_SERVER_MODULE_STATE_ERR:
        if (ret) module->state = AZY_SERVER_MODULE_STATE_METHOD;
        client->resume_ret = ret;
        break;
      case AZY_SERVER_MODULE_STATE_METHOD:
        if (!ret) module->state = AZY_SERVER_MODULE_STATE_ERR;
      case AZY_SERVER_MODULE_STATE_POST:
        client->resume_ret = ret;
      default:
        break;
     }
   module->rewind = EINA_FALSE;
   if (module->new_net && (client->current != module->new_net) && (client->current == client->resume) &&
       client->resume_rpc && client->resume_rpc->error_set)
     {
        client->current->http.req.http_path = NULL;
        module->new_net->type = client->current->type;
        module->new_net->transport = client->current->transport;
        module->new_net->http.version = client->current->http.version;
        azy_net_free(client->current);
        client->resume = client->current = module->new_net;
        module->new_net = NULL;
     }
   _azy_server_client_handler_request(client);
   if (client->suspend)
     {
        client->resuming = EINA_FALSE;
        return;
     }
   EINA_LIST_FREE(client->suspended_nets, net)
     {
        client->resume = net;
        _azy_server_client_handler_request(client);
        if (client->suspend)
          {
             client->resuming = EINA_FALSE;
             return;
          }
        client->resume_ret = EINA_FALSE;
     }

   client->resume = NULL;
   client->resuming = EINA_FALSE;
   if (client->dead)
     _azy_server_client_free(client);
}

/**
 * @brief Suspend an active client's activity
 * This function allows for a module (and therefore its client)
 * to be suspended, preventing any further action from occurring until
 * azy_server_module_events_resume is called on it. Since RPC is synchronous
 * by nature, all subsequent methods sent by the client will be stored until
 * the client is resumed.
 * @param module The module
 */
void
azy_server_module_events_suspend(Azy_Server_Module *module)
{
   DBG("(module=%p)", module);

   if (!AZY_MAGIC_CHECK(module, AZY_MAGIC_SERVER_MODULE))
     {
        AZY_MAGIC_FAIL(module, AZY_MAGIC_SERVER_MODULE);
        return;
     }
   EINA_SAFETY_ON_TRUE_RETURN(module->client->dead);
   if (module->suspend)
     {
        WARN("Module is currently suspended, this function has no effect!");
        return;
     }
   EINA_SAFETY_ON_NULL_RETURN(module->client->current);

   module->suspend = EINA_TRUE;
   module->client->suspend = EINA_TRUE;
   module->client->resume = module->client->current;
}

/**
 * @brief Return the suspend state of a module
 * Use this function to determine whether a module is currently suspended
 * @param module The module (NOT #NULL)
 * @return EINA_TRUE if the module is suspended, else EINA_FALSE
 */
Eina_Bool
azy_server_module_events_suspended_get(Azy_Server_Module *module)
{
   DBG("(module=%p)", module);

   if (!AZY_MAGIC_CHECK(module, AZY_MAGIC_SERVER_MODULE))
     {
        AZY_MAGIC_FAIL(module, AZY_MAGIC_SERVER_MODULE);
        return EINA_FALSE;
     }
   return module->suspend;
}

/**
 * @brief Rewind the current module's state
 * Use this function to rewind a suspended client to the most recently called server directive.
 * A client can only rewind by one state. Ensure that you have completed all tasks necessary
 * prior to progressing to a state from which it will be impossible to return.
 * @param module The module (NOT #NULL)
 * @return EINA_TRUE if the module's state was successfully rewound, else EINA_FALSE
 * @note This function can only be called from __pre__, __post__, __fallback__, or a method.
 */
Eina_Bool
azy_server_module_events_rewind(Azy_Server_Module *module)
{
   DBG("(module=%p)", module);

   if (!AZY_MAGIC_CHECK(module, AZY_MAGIC_SERVER_MODULE))
     {
        AZY_MAGIC_FAIL(module, AZY_MAGIC_SERVER_MODULE);
        return EINA_FALSE;
     }
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!module->suspend, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(module->rewind, EINA_FALSE);
   module->rewind = EINA_TRUE;

   return EINA_TRUE;
}
/** @} */
