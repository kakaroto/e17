/*
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Ecore.h>
#include <Ecore_Con.h>

#include <regex.h>
#include "Azy_Server.h"
#include "Azy_Content.h"
#include "Azy_Net.h"
#include "azy_utils.h"
/*
   static regex_t response = NULL;
        regcomp(&response, "^HTTP/(1\\.[0-1]) ([0-9]{3}) (.+)$", REG_EXTENDED);
 */
static regex_t request = NULL;
static regex_t header_r = NULL;

static int AZY_EVENT_CLIENT;

#define AZY_SKIP_BLANK(PTR) \
  if (isspace(*PTR) && len)  \
    do                       \
      {                      \
         X++;                \
         len--;              \
      } while (isspace(*PTR) && (len > 0))

static int __azy_header_type_parse(Azy_Net   *net,
                                    const char *header,
                                    int         len);
static Eina_Bool __azy_header_line_parse(Azy_Net      *net,
                                          unsigned char *data,
                                          int            len);

static void                _azy_server_client_new(Azy_Server      *server,
                                                   Ecore_Con_Server *conn);
static void                _azy_server_module_free(Azy_Server_Module *module,
                                                    Eina_Bool           shutdown);
static Azy_Server_Module *_azy_server_module_new(azy_module_def *def,
                                                   Azy_Client     *client);
static azy_module_def    *_azy_server_module_def_find(Azy_Server *server,
                                                        const char  *name);
static Azy_Server_Module *_azy_server_client_find_module(Azy_Server_Client *client,
                                                           const char         *name)
static Azy_Server_Module_Method * _azy_server_module_method_def_find(Azy_Server_Module * module, const char *name);
static Eina_Bool _azy_server_module_method_run(Azy_Server_Module *module,
                                                Azy_Content       *call);
static Eina_Bool _azy_server_module_get(Ecore_Thread       *thread,
                                         Azy_Server_Client *client,
                                         Azy_Content       *content);

static Eina_Bool _azy_server_client_kill(Ecore_Con_Server   *svr,
                                          Azy_Server_Client *client,
                                          const char         *msg);
static Eina_Bool _azy_server_client_req_complete(Azy_Server_Client *client);

static void      _azy_server_client_thread_notify(Ecore_Thread       *thread,
                                                   Azy_Content       *content,
                                                   Azy_Server_Client *client);
static void _azy_server_client_download_thread(Ecore_Thread       *thread,
                                                Azy_Server_Client *client);
static void _azy_server_client_upload_thread(Ecore_Thread       *thread,
                                              Azy_Server_Client *client);
static void _azy_server_client_json_thread(Ecore_Thread *thread,
                                            void         *data);
static void _azy_server_client_xml_thread(Ecore_Thread *thread,
                                           void         *data);

static Eina_Bool _azy_server_client_request_handler(Azy_Server_Client *client,
                                                     int                 type,
                                                     Azy_Server_Client *data);
static Eina_Bool _azy_server_client_del_handler(Ecore_Con_Event_Client_Del *ev,
                                                 int                         type,
                                                 Azy_Server_Client         *client);
static Eina_Bool _azy_server_client_data_handler(Ecore_Con_Event_Client_Data *ev,
                                                  int                          type,
                                                  Azy_Server_Client          *client);

static void _azy_server_client_free(Azy_Server_Client *client);

static Eina_Bool
_azy_events_init(void)
{
   regcomp(&request, "^(GET|HEAD|POST|PUT) ([^ @\\]+) HTTP/(1\\.[0-1])$", REG_EXTENDED);
   regcomp(&header_r, "^([a-zA-Z-]+): ([[:alnum:][:punct:] ]+)", REG_EXTENDED);

   AZY_EVENT_CLIENT = ecore_event_type_new();

   return EINA_TRUE;
}

static int
__azy_header_type_parse(Azy_Net            *net,
                         const unsigned char *header,
                         int                  len)
{
   regmatch_t match[4];
   unsigned char first[4096];
   unsigned char *endline, *start;

   EINA_SAFETY_ON_NULL_RETURN_VAL(net, 0);
   EINA_SAFETY_ON_NULL_RETURN_VAL(header, 0);

   start = header;
   /* some clients are dumb and send leading cr/nl/etc */
   AZY_SKIP_SPACE(start);

   if (!(endline = memchr(start, '\r', len)) && !(endline = memchr(start, '\n', len)))
     /*no newline/cr, so invalid start*/
     return 0;

   if ((endline - start) > (sizeof(first) - 1))
     /*start is waaaaaaaaay too long*/
     return 0;

   /*null terminate*/
   snprintf(first, endline - start, "%s", start);
   if (!regexec(&request, first, 4, match, 0))
     {
        int version = 1.1;
        sscanf(start + match[3].rm_so, match[3].rm_eo - match[3].rm_so, "%i", &version);
        net->http.version = version;
        net.http.req.http_path = strndup(start + match[2].rm_so, match[2].rm_eo - match[2].rm_so);
        net->type = AZY_NET_TYPE_RESPONSE;
        return match[3].rm_eo - header;
     }
/*
        else if (!regexec(&response, first, 4, match, 0))
        {
                int code = -1;
                if (sscanf(first + match[2].rm_so, match[2].rm_eo - match[2].rm_so, "%i", &code) != 1)
                        return 0;
                net.http.res.http_code = code;
                eina_stringshare_replace_length(&net.http.res.http_msg, header + match[3].rm_so, match[3].rm_eo - match[3].rm_so);

                if (!strncmp(start + match[1].rm_so, "GET", match[1].rm_eo - match[1].rm_so))
                        net->type = AZY_NET_TYPE_GET;
                else if (!strncmp(start + match[1].rm_so, "POST", match[1].rm_eo - match[1].rm_so))
                        net->type = AZY_NET_TYPE_POST;
                else if (!strncmp(start + match[1].rm_so, "PUT", match[1].rm_eo - match[1].rm_so))
                        net->type = AZY_NET_TYPE_PUT;

                return match[3].rm_eo - header;
        }
 */
   return 0;
}

static Eina_Bool
__azy_header_line_parse(Azy_Net      *net,
                         unsigned char *data,
                         int            len)
{
   regmatch_t match[3];
   unsigned char *c, *r, *p, *start, *buf_start;
   const char *s;
   size_t slen;
   char sep[5];
   unsigned char *buf;
   int line_len;

   if (net->headers_read)
     return EINA_TRUE;
   EINA_SAFETY_ON_NULL_RETURN_VAL(net, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(data, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(len < 1, EINA_FALSE);

   if (net->size)
     {
        /* previous buffer */
         /* alloca should be safe here because ecore_con reads at most 64k
          * and even if no headers were found previously, the entire
          * buffer would not be copied
          */
           buf_start = alloca(sizeof(char) * (len + net->size));
           /* grab and combine buffers */
           memcpy(buf_start, net->buffer, net->size);
           memcpy(buf_start + net->size, data, len);
           free(net->buffer);
           net->buffer = NULL;
           len += net->size;
           net->size = 0;
           start = buf_start;
           AZY_SKIP_SPACE(start);
     }
   else
   /* only current buffer */
     {
        /* copy pointer */
         start = data;
         /* skip all spaces/newlines/etc and decrement len */
         AZY_SKIP_SPACE(start);
     }
   /* find a header or append to buffer */
   if ((!(r = memchr(start, '\r', len)) && !(r = memchr(start, '\n', len))) || !(c = memchr(start, ':', len)))
     {
        unsigned char *tmp;

        if (net->size)
          {
             if (!(tmp = realloc(net->buffer, net->size + len)))
               return EINA_FALSE;
             net->buffer = tmp;
             memcpy(net->buffer + net->size, start, len);
             net->size += len;
          }
        else
          {
             if (!(net->buffer = malloc(sizeof(char) * len)))
               return EINA_FALSE;
             memcpy(net->buffer, start, len);
             net->size = len;
          }
        return EINA_TRUE;
     }

   if (*r == '\r')
     {
        unsigned char *x;
        if ((x = memchr(start, '\n', len)))
          {
             if ((x - r) > 0)
               s = "\r\n";
             else
               s = "\n\r";
          }
        else
          s = "\r";
     }
   else
     s = "\n";

   slen = strlen(s);
   snprintf(sep, sizeof(sep), "%s%s", s, s);
   /* by spec, this is only found between header and content */
   if (azy_memstr(start, sep, len, strlen(sep)))
     net->headers_read = EINA_TRUE;

   p = start;
   line_len = r - p;
   while (len && c && r)
     {
        if (!regexec(&header_r, p, 3, match, 0))
          {
             char *key, *value;

             if ((match[1].rm_eo - match[1].rm_so > 4096) || (match[2].rm_eo - match[2].rm_so > 4096))
               {
                  DBG("Ignoring unreasonably large header starting at:\n %.32s\n", match[1].rm_so);
                  goto out;
               }
             key = strndupa(start + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
             value = strndupa(start + match[2].rm_so, match[2].rm_eo - match[2].rm_so);
             azy_net_header_set(net, key, value);
          }
out:
        len -= line_len + slen;
        if (len < slen)
          break;
        p = r + slen;
        if (net->headers_read)
          /* double separator: STOP */
          if (azy_memstr(p, s, slen, slen))
            break;
        r = azy_memstr(p, s, len, slen);
        line_len = r - p;
        /* FIXME: to be fully 1.1 compliant, lines like this should
         * be filtered and checked to see if is a continuing header
         * from the previous line
         */
        if (!(c = memchr(p, ':', line_len)))
          goto out;
     }

   AZY_SKIP_SPACE(p);

   if (len)
     {
        /* if we get here, we need to append to the buffer */
         net->size = len;
         net->buffer = malloc(sizeof(char) * len);
         memcpy(net->buffer, p, len);
     }

   if (!(c = eina_hash_find(net->http.headers, "content-length")))
     net->http.content_length = -1;
   else
     net->http.content_length = strtol(c, NULL, 10);
   return EINA_TRUE;
}

static void
_azy_server_module_free(Azy_Server_Module *module,
                         Eina_Bool           shutdown)
{
   if (!module)
     return;

   if (shutdown && module->def && module->def->shutdown)
     module->def->shutdown(module);

   free(module->data);
   free(module);
}

static Azy_Server_Module *
_azy_server_module_new(azy_module_def *def,
                        Azy_Client     *client)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(def, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(client, NULL);

   Azy_Server_Module *s = calloc(1, sizeof(Azy_Server_Module));

   if (def->size > 0)
     s->data = calloc(1, def->size);

   s->def = def;
   s->client = client;

   if (s->def->init && !s->def->init(s))
     {
        azy_server_module_free(s, EINA_FALSE);
        return NULL;
     }

   return s;
}

static azy_module_def *
_azy_server_module_def_find(Azy_Server *server,
                             const char  *name)
{
   Eina_List *l;
   azy_module_def *def;

   EINA_SAFETY_ON_NULL_RETURN_VAL(server, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, NULL);

   EINA_LIST_FOREACH(server->modules, l, def)
     {
        if (!strcmp(def->name, name))
          return def;
     }

   return NULL;
}

static Azy_Server_Module_Method *
_azy_server_module_method_def_find(Azy_Server_Module *module,
                                    const char         *name)
{
   int i;

   EINA_SAFETY_ON_NULL_RETURN_VAL(module, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(module->def, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, NULL);

   for (i = 0; i < module->def->methods_count; i++)
     if (!strcmp(module->def->methods[i].name, name))
       return module->def->methods + i;

   return NULL;
}

static Eina_Bool
_azy_server_module_method_run(Azy_Server_Module *module,
                               Azy_Content       *content)
{
   azy_server_module_method_def *method;
   Eina_Bool retval = EINA_FALSE;

   EINA_SAFETY_ON_NULL_RETURN_VAL(module, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(module->def, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(content, EINA_FALSE);

   module->content = content;

   method = _azy_server_module_method_def_find(module, azy_content_method_get(content));

   if (method)
     {
        if (module->def->pre)
          if (!module->def->pre(module, content))
            {
               if (!azy_content_retval_get(content) && !azy_content_error_is_set(content))
                 azy_content_error_faultmsg_set(content, -1, "Pre-call did not return value or set error.");

               goto out;
            }

        retval = method->method(module, content);
     }
   else if (module->def->fallback)
     {
        if (module->def->fallback(module, content))
          {
             // content should be handled
               if (azy_content_retval_get(content) == NULL && !azy_content_error_is_set(content))
                 azy_content_error_faultmsg_set(content, -1, "Fallback did not return value or set error.");
          }
        else
          azy_content_error_faultmsg_set(content, -1, "Method %s not found in %s module.", azy_content_method_get(content), module->def->name);
     }
   else
     azy_content_error_faultmsg_set(content, -1, "Method %s not found in %s module.", azy_content_method_get(content), module->def->name);

out:

   if (module->def->post)
     module->def->post(module, content);

   module->content = NULL;
   return retval;
}

static Eina_Bool
_azy_server_module_get(Ecore_Thread       *thread,
                        Azy_Server_Client *client,
                        Azy_Content       *content)
{
   Azy_Server_Module *module = NULL;
   Azy_Server_Module *cur_module;
   const char *module_name;

   EINA_SAFETY_ON_NULL_RETURN_VAL(thread, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(client, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(content, EINA_FALSE);

   /* get Azy_Server_Module object for current connection and given module name */
   if (!(module_name = azy_content_module_name_get(content, client->net.http.req.http_path + 1)))
     {
        azy_content_error_faultmsg_set(content, -1, "Undefined module name.");
        return EINA_FALSE;
     }

   if (!(module = azy_client_find_module(client, module_name)))
     {
        azy_module_def *def;

        def = _azy_server_module_def_find(client->server, module_name);

        eina_stringshare_del(module_name);
        if (!def)
          {
             azy_content_error_faultmsg_set(content, -1, "Unknown module %s.", module_name);
             return EINA_FALSE;
          }

        if (!(module = _azy_server_module_new(def, client)))
          {
             azy_content_error_faultmsg_set(content, -1, "Servlet initialization failed.");
             return EINA_FALSE;
          }

        eina_array_push(client->modules, module);
     }
   else
     eina_stringshare_del(module_name);

   return _azy_server_module_method_run(module, content);
}

static void
_azy_server_client_new(Azy_Server      *server,
                        Ecore_Con_Server *conn)
{
   if (!server) return;
   Azy_Server_Client *c = malloc(sizeof(Azy_Server_Client));

   if (!c) return;
   c->modules = eina_array_new(3);
   c->net = azy_net_new(conn);
   c->server = server;
   c->last_used = ecore_time_get();

   c->del = ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DEL, (Ecore_Event_Handler_Cb)_azy_server_client_del_handler, c);
   c->data = ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DATA, (Ecore_Event_Handler_Cb)_azy_server_client_data_handler, c);
   /* FIXME: is there other data I want to shove into these handlers? */
   c->request = ecore_event_handler_add(AZY_EVENT_CLIENT, (Ecore_Event_Handler_Cb)_azy_server_client_request_handler, c);
}

static void
_azy_server_client_free(Azy_Server_Client *client)
{
   Eina_Iterator *it;
   Azy_Server_Module *s;
   if (!client)
     return;

   azy_net_free(client->net);
   it = eina_array_iterator_new(client->modules);
   EINA_ITERATOR_FOREACH(it, s)
   azy_server_module_free(s, EINA_TRUE);
   eina_iterator_free(it);
   eina_array_free(client->modules);
   eina_stringshare_del(client->session_id);

   ecore_event_handler_del(c->del);
   ecore_event_handler_del(c->data);
   ecore_event_handler_del(c->request);
   ecore_event_handler_del(c->response);
   ecore_event_handler_del(c->method);

   free(client);
}

static Azy_Server_Module *
_azy_server_client_find_module(Azy_Server_Client *client,
                                const char         *name)
{
   Eina_Iterator *it;
   Azy_Server_Module *module;

   EINA_SAFETY_ON_NULL_RETURN_VAL(client, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, NULL);

   it = eina_array_iterator_new(client->modules);
   EINA_ITERATOR_FOREACH(it, module)
   if (!strcmp(module->def->name, name))
     {
        eina_iterator_free(it);
        return module;
     }
   eina_iterator_free(it);
   return NULL;
}

static void
_azy_server_client_download_thread(Ecore_Thread       *thread,
                                    Azy_Server_Client *client)
{
   Eina_List *l;
   azy_module_def *def;
   Azy_Content *content;

   /* for each available module type, check if it has download hook */
   EINA_LIST_FOREACH(client->server->modules, l, def)
     {
        if (def->download)
          {
             Azy_Server_Module *module = NULL, *existing_module;
             Eina_Iterator *it;

             it = eina_array_iterator_new(client->modules);
             /* check if module is instantiated */
             EINA_ITERATOR_FOREACH(it, existing_module)
             if (existing_module->def == def)
               module = existing_module;

             eina_iterator_free(it);
             /* it's not, instantiate it now */
             if (!module)
               {
                  module = azy_server_module_new(def, client);
                  eina_array_push(client->modules, module);
               }

             if (def->download(module))
               {
                  ecore_thread_notify(thread, module->content);
                  return;
               }
          }
     }

   client->net.http.res.http_code = 501;
   client->net.http.res.http_msg = azy_net_http_msg_get(501);
   azy_net_header_set(client->net, "Content-Type", "text/plain");
   content = azy_content_new(NULL);
   content->buffer = strdup("Download hook is not implemented.");
   content->length = 33;

   ecore_thread_notify(thread, content);
}

static void
_azy_server_client_upload_thread(Ecore_Thread       *thread,
                                  Azy_Server_Client *client)
{
   Azy_Content *content;
   Eina_List *l;
   azy_module_def *def;

   EINA_LIST_FOREACH(client->server->module_defs, l, def)
     {
        if (def->upload)
          {
             Azy_Server_Module *module, *existing_module;
             Eina_Iterator *it;

             it = eina_array_iterator_new(client->modules);
             EINA_ITERATOR_FOREACH(it, existing_module)
             if (existing_module->def == def)
               module = existing_module;

             eina_iterator_free(it);
             if (!module)
               {
                  module = azy_server_module_new(def, client);
                  eina_array_push(client->modules, module);
               }

             if (def->upload(module))
               {
                  ecore_thread_notify(thread, module->content);
                  return;
               }
          }
     }

   client->net.http.res.http_code = 501;
   client->net.http.res.http_msg = azy_net_http_msg_get(501);
   azy_net_header_set(client->net, "Content-Type", "text/plain");
   content = azy_content_new(NULL);
   content->buffer = strdup("Upload hook is not implemented.");
   content->length = 31;

   ecore_thread_notify(thread, content);
}

static void
_azy_server_client_thread_notify(Ecore_Thread       *thread,
                                  Azy_Content       *content,
                                  Azy_Server_Client *client)
{
   int sent, total;
   Eina_Strbuf *msg;

   msg = eina_strbuf_new();

   if (azy_debug_enabled & AZY_DEBUG_CALL)
     azy_content_dump(content, 0);

   if (!client->net.http.res.http_code)
     {
        client->net.http.res.http_code = 200;
        client->net.http.res.http_msg = azy_net_http_msg_get(200);
     }
   if ((client->session_id) && (strlen(client->session_id) > 0))
     {
        char idstr[48];
        snprintf(idstr, sizeof(idstr), "sessid=%s;", client->session_id);
        azy_net_header_set(client->net, "Set-Cookie", idstr);
     }

   msg = azy_net_header_create(net);
   eina_strbuf_append(msg, eina_strbuf_string_get(net->header));
   eina_strbuf_append(msg, content->buffer);
   total = eina_strbuf_length_get(msg);

   if ((sent = ecore_con_server_send(client->net->conn, eina_strbuf_string_get(msg), total)) != total)
     ERR("message sent was %d bytes, but message length is %d bytes!", sent, total);

   azy_content_free(content);
   eina_strbuf_free(msg);
}

static void
_azy_server_client_json_thread(Ecore_Thread       *thread,
                                Azy_Server_Client *client)
{
   Azy_Content *content;

   content = azy_content_new(NULL);

   rs = azy_content_unserialize_request_json(content, client->net->buffer, client->net->size);

   if (!rs)
     azy_content_error_faultmsg_set(content, -1, "Unserialize request failure.");
   else
     _azy_server_module_get(thread, client, content);

   azy_content_serialize_response_json(content);
   azy_net_header_set(client->net, "content-type", "application/json");
   ecore_thread_notify(thread, content);
}

static void
_azy_server_client_xml_thread(Ecore_Thread       *thread,
                               Azy_Server_Client *client)
{
   Azy_Content *content;

   content = azy_content_new(NULL);

   rs = azy_content_unserialize_request_xml(content, client->net->buffer, client->net->size);

   if (!rs)
     azy_content_error_faultmsg_set(content, -1, "Unserialize request failure.");
   else
     _azy_server_module_get(thread, client, content);

   azy_content_serialize_response_xml(content);
   azy_net_header_set(client->net, "content-type", "text/xml");
   ecore_thread_notify(thread, content);
}

static Eina_Bool
_azy_server_client_request_handler(Azy_Server_Client *client,
                                    int                 type,
                                    Azy_Server_Client *data)
{
   Azy_Net_Transport net_transport;

   if ((client != data) || (type != AZY_EVENT_CLIENT))
     return ECORE_CALLBACK_RENEW;

   if ((client->net->type != AZY_NET_TYPE_GET) || (client->net->type != AZY_NET_TYPE_POST) ||
       (client->net->type != AZY_NET_TYPE_PUT))
     return EINA_FALSE;

   if (client->net->type == AZY_NET_TYPE_GET)
     {
        ecore_long_run(_azy_server_client_download_thread, _azy_server_client_thread_notify, NULL, NULL, data, EINA_FALSE);
        return ECORE_CALLBACK_RENEW;
     }
   if (client->net->type == AZY_NET_TYPE_PUT)
     {
        ecore_long_run(_azy_server_client_download_thread, _azy_server_client_thread_notify, NULL, NULL, data, EINA_FALSE);
        return ECORE_CALLBACK_RENEW;
     }
   else if (!strcmp(method, "POST"))
     {
        client->net->transport = azy_net_transport_get(azy_net_header_get(client->net, "content-type"));
        switch (client->net->transport)
          {
           case AZY_NET_TEXT:
             /* FIXME: this isn't supported yet but probably should be somehow? */
             _azy_server_client_kill(client->server, client, error501);
             break;

           case AZY_NET_JSON:
             ecore_long_run(_azy_server_client_json_thread, _azy_server_client_thread_notify, NULL, NULL, data, EINA_FALSE);
             break;

           default:
             ecore_long_run(_azy_server_client_xml_thread, _azy_server_client_thread_notify, NULL, NULL, data, EINA_FALSE);
             break;
          }
     }

   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_azy_server_client_kill(Ecore_Con_Server   *svr,
                         Azy_Server_Client *client,
                         const char         *msg)
{
   ecore_con_server_send(svr, msg, strlen(msg));
   _azy_server_client_free(client);
   ecore_con_server_del(svr);
   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_azy_server_client_req_complete(Azy_Server_Client *client)
{
   if ((client->net->size < client->net->http.content_length) && (!client->net->nodata))
     {
        client->net->nodata = EINA_TRUE;
        return ECORE_CALLBACK_RENEW;
     }
   if (client->net->size > client->net->http.content_length)
     {
        ecore_event_add(AZY_EVENT_CLIENT, NULL, client);
        ecore_timer_del(client->net->timer);
        client->net->timer = NULL;
        return ECORE_CALLBACK_RENEW;
     }
   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_azy_server_client_data_handler(Ecore_Con_Event_Client_Data *ev,
                                 int                          type,
                                 Azy_Server_Client          *client)
{
   char *tmp;
   int offset = 0;

   if (client->server != ecore_con_server_data_get(ecore_con_client_server_get(ev->client)) ||
       (type != ECORE_CON_EVENT_CLIENT_DEL))
     return ECORE_CALLBACK_RENEW;

   client->net->nodata = EINA_FALSE;

   if (EINA_UNLIKELY(!client->net->size))
     {
        /* returns offset where http header line ends */
         if (!(offset = __azy_header_type_parse(client->net, ev->data, ev->size)))
           {
              _azy_server_client_kill(ecore_con_client_server_get(ev->client), client, error400);
              ecore_con_client_del(ev->client);
              return ECORE_CALLBACK_RENEW;
           }
     }
   if (!client->net->header_read) /* if headers aren't done being read, keep reading them */
     {
        if (!azy_net_parse_headers(client->net, ev->data + offset, ev->size - offset))
          {
             _azy_server_client_kill(ecore_con_client_server_get(ev->client), client, error500);
             ecore_con_client_del(ev->client);
             return ECORE_CALLBACK_RENEW;
          }
     }
   else
     {   /* otherwise keep appending to buffer */
        tmp = realloc(client->net->buffer, client->net->size + ev->size - offset);

        if (EINA_UNLIKELY(!tmp))
          {
             ERR("mem allocation failed during receive: %d bytes lost!", ev->size - offset);
             return ECORE_CALLBACK_RENEW;
          }
        client->net->buffer = tmp;
        memcpy(client->net->buffer + client->net->size, ev->data + offset, ev->size - offset);

        client->net->size += ev->size - offset;
     }

   if ((client->net->size < client->net->http.content_length) && (!client->net->timer))
     /* no timer and full content length not received, start timer */
     client->net->timer = ecore_timer_add(1, _azy_server_client_req_complete, client);
   else if (!client->net->timer)
     /* else create a "done" event */
     ecore_event_add(AZY_EVENT_CLIENT, NULL, client);

   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_azy_server_client_del_handler(Ecore_Con_Event_Client_Del *ev,
                                int                         type,
                                Azy_Server_Client         *client)
{
   if (client->server != ecore_con_server_data_get(ecore_con_client_server_get(ev->client)) ||
       (type != ECORE_CON_EVENT_CLIENT_DEL))
     return ECORE_CALLBACK_RENEW;

   ecore_con_client_del(ev->client);
   ecore_event_handler_del(client->del);

   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
azy_server_client_add_handler(Ecore_Con_Event_Client_Add *ev,
                               int                         type,
                               Azy_Server                *server)
{
   static Eina_Bool init;

   if (EINA_UNLIKELY(!init))
     init = _azy_events_init();

   /* avoid collision from multiple servers */
   if (server != ecore_con_server_data_get(ecore_con_client_server_get(ev->client)) ||
       (type != ECORE_CON_EVENT_CLIENT_ADD))
     return ECORE_CALLBACK_RENEW;
   _azy_server_client_new(server, ecore_con_client_server_get(ev->client));

   return ECORE_CALLBACK_RENEW;
}

