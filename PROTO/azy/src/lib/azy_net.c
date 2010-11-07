/*
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Azy.h"
#include "azy_private.h"

static void
_azy_net_header_hash(__UNUSED__ Eina_Hash *hash,
                      const char           *key,
                      const char           *data,
                      Eina_Strbuf          *header)
{
   eina_strbuf_append_printf(header, "%s: %s\r\n", key, data);
}

Azy_Net *
azy_net_new(void *conn)
{
   if (!conn)
     return NULL;

   Azy_Net *net = calloc(sizeof(Azy_Net), 1);
   net->conn = conn;

   AZY_MAGIC_SET(net, AZY_MAGIC_NET);
   return net;
}

void
azy_net_free(Azy_Net *net)
{
   DBG("(net=%p)", net);

   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return;
     }


   AZY_MAGIC_SET(net, AZY_MAGIC_NONE);
   if (net->http.headers)
     eina_hash_free(net->http.headers);
   if (net->http.req.http_path)
     eina_stringshare_del(net->http.req.http_path);
   if (net->http.res.http_msg)
     eina_stringshare_del(net->http.res.http_msg);
   if (net->buffer && net->size)
     free(net->buffer);
   free(net);
}

const char *
azy_net_header_get(Azy_Net   *net,
                    const char *name)
{
   const char *value;
   char *tmp;

   DBG("(net=%p)", net);

   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return NULL;
     }

   if (!name)
     return NULL;

   tmp = strdupa(name);
   eina_str_tolower(&tmp);
   value = eina_hash_find(net->http.headers, tmp);

   return value;
}

void
azy_net_header_reset(Azy_Net *net)
{
   DBG("(net=%p)", net);
   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return;
     }

   if (!net->http.headers)
     return;

   eina_hash_free_buckets(net->http.headers);
   net->headers_read = EINA_FALSE;
}

Eina_Bool
azy_net_auth_set(Azy_Net   *net,
                  const char *username,
                  const char *password)
{
   DBG("(net=%p)", net);
   char *enc_auth_str;
   Eina_Strbuf *str;

   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return EINA_FALSE;
     }

   if ((!username) || (!password))
     return EINA_FALSE;

   if (!(str = eina_strbuf_new()))
     {
        ERR("Could not allocate memory!");
        return EINA_FALSE;
     }
   eina_strbuf_append_printf(str, "%s:%s", username, password);
   enc_auth_str = azy_base64_encode(eina_strbuf_string_get(str), eina_strbuf_length_get(str));
   eina_strbuf_string_free(str);
   eina_strbuf_append_printf(str, "Basic %s", enc_auth_str);
   azy_net_header_set(net, "Authorization", eina_strbuf_string_get(str));
   free(enc_auth_str);
   eina_strbuf_free(str);
   return EINA_TRUE;
}

Eina_Bool
azy_net_auth_get(Azy_Net    *net,
                  const char **username,
                  const char **password)
{
   DBG("(net=%p)", net);
   size_t auth_str_len, i;
   const char *enc_auth_str, *auth_header;
   char *auth_str;

   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return EINA_FALSE;
     }

   if ((!username) || (!password))
     return EINA_FALSE;

   auth_header = azy_net_header_get(net, "Authorization");

   if ((!auth_header) || (!eina_str_has_prefix(auth_header, "Basic ")))
     return EINA_FALSE;

   enc_auth_str = auth_header + 6;
   auth_str = azy_base64_decode(enc_auth_str, strlen(enc_auth_str));
   auth_str_len = strlen(auth_str);

   if ((!auth_str) || (!auth_str_len))
     return EINA_FALSE;

   for (i = 0; i < auth_str_len; i++)
     if (auth_str[i] == ':')
       break;

   if (i == auth_str_len)
     {
        free(auth_str);
        return EINA_FALSE;
     }

   *username = eina_stringshare_add_length(auth_str, i);
   *password = eina_stringshare_add_length(auth_str + i + 1, auth_str_len - i - 1);

   free(auth_str);
   return EINA_TRUE;
}

const char *
azy_net_uri_get(Azy_Net *net)
{
   DBG("(net=%p)", net);
   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return NULL;
     }

   return net->http.req.http_path;
}

Eina_Bool
azy_net_uri_set(Azy_Net *net, const char *path)
{
   DBG("(net=%p)", net);
   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return EINA_FALSE;
     }
   EINA_SAFETY_ON_NULL_RETURN_VAL(path, EINA_FALSE);

   net->http.req.http_path = eina_stringshare_add(path);
   return EINA_TRUE;
}

int
azy_net_version_get(Azy_Net *net)
{
   DBG("(net=%p)", net);
   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return -1;
     }

   return net->http.version;
}

Eina_Bool
azy_net_version_set(Azy_Net *net, int version)
{
   DBG("(net=%p)", net);
   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return EINA_FALSE;
     }
   EINA_SAFETY_ON_TRUE_RETURN_VAL((version != 0) && (version != 1), EINA_FALSE);

   net->http.version = version;
   return EINA_TRUE;
}

int
azy_net_code_get(Azy_Net *net)
{
   DBG("(net=%p)", net);
   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return -1;
     }

   DBG("(net=%p)", net);

   return net->http.res.http_code;
}

void
azy_net_code_set(Azy_Net *net,
                  int       code)
{
   DBG("(net=%p)", net);
   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return;
     }

   net->http.res.http_code = code;
   net->http.res.http_msg = azy_net_http_msg_get(code);
}

Azy_Net_Type
azy_net_type_get(Azy_Net *net)
{
   DBG("(net=%p)", net);
   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return AZY_NET_TYPE_NONE;
     }

   return net->type;
}

int
azy_net_message_length_get(Azy_Net *net)
{
   DBG("(net=%p)", net);
   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return -1;
     }

   return net->http.content_length;
}

void
azy_net_header_set(Azy_Net   *net,
                    const char *name,
                    const char *value)
{
   DBG("(net=%p)", net);
   const char *old;
   char *tmp;

   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return;
     }

   if ((!name) || (!value))
     return;

   tmp = strdupa(name);
   eina_str_tolower(&tmp);

   if (EINA_UNLIKELY(!net->http.headers))
     net->http.headers = eina_hash_string_small_new((Eina_Free_Cb)eina_stringshare_del);

   if ((old = eina_hash_set(net->http.headers, tmp, eina_stringshare_add(value))))
     eina_stringshare_del(old);
}

const char *
azy_net_ip_get(Azy_Net *net)
{
   DBG("(net=%p)", net);
   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return NULL;
     }

   if (net->server_client)
     return ecore_con_client_ip_get(net->conn);

   if (!ecore_con_server_connected_get(net->conn))
     return NULL;

   return ecore_con_server_ip_get(net->conn);
}

void
azy_net_type_set(Azy_Net     *net,
                  Azy_Net_Type type)
{
   DBG("(net=%p)", net);
   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return;
     }
   if (((type < AZY_NET_TYPE_GET) || (type > AZY_NET_TYPE_RESPONSE_ERROR)))
     return;

   net->type = type;
}

void
azy_net_transport_set(Azy_Net          *net,
                       Azy_Net_Transport transport)
{
   DBG("(net=%p)", net);
   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return;
     }

   net->transport = transport;
   if (transport == AZY_NET_TRANSPORT_XML)
     azy_net_header_set(net, "Content-Type", "text/xml");
   else if (transport == AZY_NET_TRANSPORT_JSON)
     azy_net_header_set(net, "Content-Type", "application/json");
}

Azy_Net_Transport
azy_net_transport_get(Azy_Net *net)
{
   DBG("(net=%p)", net);
   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return AZY_NET_TRANSPORT_UNKNOWN;
     }
   return net->transport;
}

void
azy_net_message_length_set(Azy_Net *net,
                            int       length)
{
   DBG("(net=%p)", net);
   char buf[64];

   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return;
     }
   if (length < 1)
     return;

   net->http.content_length = length;
   snprintf(buf, sizeof(buf), "%i", length);
   azy_net_header_set(net, "Content-Length", buf);
}

Eina_Strbuf *
azy_net_header_create(Azy_Net *net)
{
   DBG("(net=%p)", net);
   Eina_Strbuf *header;

   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return NULL;
     }

   if ((!net->http.headers) || (net->type == AZY_NET_TYPE_NONE))
     return NULL;

   header = eina_strbuf_new();
   switch (net->type)
     {
      case AZY_NET_TYPE_GET:
        eina_strbuf_append_printf(header, "GET %s HTTP/1.%i\r\n",
                                  net->http.req.http_path, net->http.version);

      case AZY_NET_TYPE_POST:
        eina_strbuf_append_printf(header, "POST %s HTTP/1.%i\r\n",
                                  net->http.req.http_path, net->http.version);
        break;

      case AZY_NET_TYPE_PUT:
        /* FIXME: implement this */
        break;

      default:
        eina_strbuf_append_printf(header, "HTTP/1.%i %d %s\r\n",
                                  net->http.version, net->http.res.http_code, net->http.res.http_msg);
     }

   eina_hash_foreach(net->http.headers, (Eina_Hash_Foreach)_azy_net_header_hash, header);

   eina_strbuf_append(header, "\r\n");
   return header;
}

const char *
azy_net_http_msg_get(int code)
{
   switch (code)
     {
      case 100:
        return eina_stringshare_add("Continue");

      case 101:
        return eina_stringshare_add("Switching Protocols");

      case 200:
        return eina_stringshare_add("OK");

      case 201:
        return eina_stringshare_add("Created");

      case 202:
        return eina_stringshare_add("Accepted");

      case 203:
        return eina_stringshare_add("Non-Authoritative Information");

      case 204:
        return eina_stringshare_add("No Content");

      case 205:
        return eina_stringshare_add("Reset Content");

      case 206:
        return eina_stringshare_add("Partial Content");

      case 300:
        return eina_stringshare_add("Multiple Choices");

      case 301:
        return eina_stringshare_add("Moved Permanently");

      case 302:
        return eina_stringshare_add("Found");

      case 303:
        return eina_stringshare_add("See Other");

      case 304:
        return eina_stringshare_add("Not Modified");

      case 305:
        return eina_stringshare_add("Use Proxy");

      case 306:
        return eina_stringshare_add("(Unused)");

      case 307:
        return eina_stringshare_add("Temporary Redirect");

      case 400:
        return eina_stringshare_add("Bad Request");

      case 401:
        return eina_stringshare_add("Unauthorized");

      case 402:
        return eina_stringshare_add("Payment Required");

      case 403:
        return eina_stringshare_add("Forbidden");

      case 404:
        return eina_stringshare_add("Not Found");

      case 405:
        return eina_stringshare_add("Method Not Allowed");

      case 406:
        return eina_stringshare_add("Not Acceptable");

      case 407:
        return eina_stringshare_add("Proxy Authentication Required");

      case 408:
        return eina_stringshare_add("Request Timeout");

      case 409:
        return eina_stringshare_add("Conflict");

      case 410:
        return eina_stringshare_add("Gone");

      case 411:
        return eina_stringshare_add("Length Required");

      case 412:
        return eina_stringshare_add("Precondition Failed");

      case 413:
        return eina_stringshare_add("Request Entity Too Large");

      case 414:
        return eina_stringshare_add("Request-URI Too Long");

      case 415:
        return eina_stringshare_add("Unsupported Media Type");

      case 416:
        return eina_stringshare_add("Requested Range Not Satisfiable");

      case 417:
        return eina_stringshare_add("Expectation Failed");

      case 500:
        return eina_stringshare_add("Internal Server Error");

      case 501:
        return eina_stringshare_add("Not Implemented");

      case 502:
        return eina_stringshare_add("Bad Gateway");

      case 503:
        return eina_stringshare_add("Service Unavailable");

      case 504:
        return eina_stringshare_add("Gateway Timeout");

      case 505:
        return eina_stringshare_add("HTTP Version Not Supported");

      default:
        return eina_stringshare_add("Unknown Status");
     }
}

