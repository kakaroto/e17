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

#include "Azy.h"
#include "azy_private.h"
/**
 * @defgroup Azy_Net Network Functions
 * @brief Functions which interact with the network transport
 * @{
 */
/* append a header hash to a strbuf */
static void
azy_net_header_hash_(Eina_Hash *hash __UNUSED__,
                     const char     *key,
                     const char     *data,
                     Eina_Strbuf    *header)
{
   eina_strbuf_append_printf(header, "%s: %s\r\n", key, data);
}

/**
 * @brief Create a new #Azy_Net object
 *
 * This function is used to create an object which will store/manipulate
 * all network-related information.  HTTP version defaults to 1.1.
 * @param conn Either an #Ecore_Con_Client or an #Ecore_Con_Server (NOT #NULL)
 * @return A new #Azy_Net object, or #NULL on failure/error
 */
Azy_Net *
azy_net_new(void *conn)
{
   Azy_Net *net;

   if (!conn) return NULL;

   net = calloc(1, sizeof(Azy_Net));
   EINA_SAFETY_ON_NULL_RETURN_VAL(net, NULL);
   net->conn = conn;
   net->http.version = 1;

   AZY_MAGIC_SET(net, AZY_MAGIC_NET);
   return net;
}

/**
 * @brief Free an #Azy_Net object
 *
 * This function frees an #Azy_Net object, including all data associated with it.
 * It does NOT free the Ecore_Con client/server object.
 * @param net The object to free (NOT #NULL)
 */
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
   free(net->buffer);
   memset(net, 0, sizeof(Azy_Net)); /* zero out data for security */
   free(net);
}

/**
 * @brief Find a value for a http header
 *
 * This function returns the http header value for the header name
 * specified.  For example, if name is "Connection", the return
 * might be "close"
 * @param net The #Azy_Net object to get the header from (NOT #NULL)
 * @param name The name of the header (NOT #NULL)
 * @return The value of the header, or #NULL if header is not present
 */
const char *
azy_net_header_get(Azy_Net    *net,
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

/**
 * @brief Free all headers associated with @p net
 *
 * This function resets the http headers for @p net, freeing all
 * memory associated with them.
 * @param net The #Azy_Net object containing the headers to free (NOT #NULL)
 */
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

/**
 * @brief Set up http basic auth headers using a name and password
 *
 * This function is used to set up http basic authentication using
 * base64 encoded copies of @p username and @p password strings.  If you don't know
 * what this is, see http://en.wikipedia.org/wiki/Basic_access_authentication
 * @param net The net object (NOT #NULL)
 * @param username The username to use (NOT #NULL)
 * @param password The password to use (NOT #NULL)
 * @return #EINA_TRUE on success, else #EINA_FALSE
 */
Eina_Bool
azy_net_auth_set(Azy_Net    *net,
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
   azy_net_header_set(net, "Authorization", NULL);
   azy_net_header_set(net, "Authorization", eina_strbuf_string_get(str));
   free(enc_auth_str);
   eina_strbuf_free(str);
   return EINA_TRUE;
}

/**
 * @brief Get username and password from http basic auth headers
 *
 * This function is used to get a http basic authentication username/password pair
 * using base64 encodes of @p username and @p password.  If you don't know
 * what this is, see http://en.wikipedia.org/wiki/Basic_access_authentication
 * @param net The net object (NOT #NULL)
 * @param username A pointer to store the stringshared username in (NOT #NULL)
 * @param password A pointer to store the stringshared password in (NOT #NULL)
 * @return #EINA_TRUE on success, else #EINA_FALSE
 */
Eina_Bool
azy_net_auth_get(Azy_Net     *net,
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

/**
 * @brief Returns the http uri (path) that requests go to/came from
 *
 * This function returns the uri specified in the HTTP header.
 * Example:
 *  GET {/images/logo.png}<==URI HTTP/1.1
 * @param net The network object (NOT #NULL)
 * @return The uri string, or #NULL on error
 */
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

/**
 * @brief Sets the http uri (path) that requests go to/came from
 *
 * This function sets the uri specified in the HTTP header.
 * Example:
 *  GET {/images/logo.png}<==URI HTTP/1.1
 * @param net The network object (NOT #NULL)
 * @param path The uri string (NOT NULL)
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
azy_net_uri_set(Azy_Net    *net,
                const char *path)
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

/**
 * @brief Returns the http protocol version used
 *
 * This function returns 0 for http v1.0 or 1 for http v1.1.
 * @param net The network object (NOT #NULL)
 * @return 0 or 1 for version, else -1 on error
 */
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

/**
 * @brief Set the http protocol version used
 *
 * This function sets the protocol version to be used in the @p net object.
 * Use 0 for http 1.0 or 1 for http 1.1.  Other values will return EINA_FALSE
 * @param net The network object (NOT #NULL)
 * @param version 0 or 1 for relevant version
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
azy_net_version_set(Azy_Net *net,
                    int      version)
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

/**
 * @brief Return the status code of an http response
 *
 * This function returns the http status code from a response.
 * Some commonly used status codes are 404 (resource not found) and
 * 200 (OK).
 * @param net The network object (NOT #NULL)
 * @return The status code or -1 on error
 */
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

/**
 * @brief Set the status code of an http response
 *
 * This function Sets the http status code for a response as well
 * as its corresponding http message (200 OK).
 * @param net The network object (NOT #NULL)
 * @param code The status code
 * @return The status code or -1 on error
 */
void
azy_net_code_set(Azy_Net *net,
                 int      code)
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

/**
 * @brief Returns the http method used in the network object
 *
 * This function returns the #Azy_Net_Type of the http method used in @p net
 * which correspond to a matching method string (eg. AZY_NET_TYPE_GET == GET)
 * @param net The network object (NOT #NULL)
 * @return The #Azy_Net_Type used, or #AZY_NET_TYPE_NONE on error
 */
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

/**
 * @brief Sets the http method used in the network object
 *
 * This function sets the #Azy_Net_Type of the http method used in @p net
 * which correspond to a matching method string (eg. AZY_NET_TYPE_GET == GET)
 * @param net The network object (NOT #NULL)
 * @param type The #Azy_Net_Type to be used
 */
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
   if (((!type) || (type > AZY_NET_TYPE_RESPONSE_ERROR)))
     return;

   net->type = type;
}

/**
 * @brief Return the message length of a transmission
 *
 * This function is used to return the content-length set in the header
 * of @p net, and is equivalent to calling azy_net_header_get(net, "content-length").
 * @param net The network object (NOT #NULL)
 * @return The content length, or -1 on error
 */
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

/**
 * @brief Set the message length of a transmission
 *
 * This function is used to set the content-length in the header
 * of @p net, but is NOT equivalent to calling
 * azy_net_header_set(net, "content-length", value).
 * @param net The network object (NOT #NULL)
 * @param length The content length (length > 1)
 */
void
azy_net_message_length_set(Azy_Net *net,
                           int      length)
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
   azy_net_header_set(net, "content-length", NULL);
   azy_net_header_set(net, "content-length", buf);
}

/**
 * @brief Set a http header for use in a transmission
 *
 * This function is used to set an http header in @p net object. If
 * @p value is NULL, the header with @p name will be deleted.
 * @param net The network object (NOT #NULL)
 * @param name The header's name (NOT #NULL)
 * @param value The header's value
 */
void
azy_net_header_set(Azy_Net    *net,
                   const char *name,
                   const char *value)
{
   DBG("(net=%p)", net);
   const char *old, *n;
   char *tmp;

   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return;
     }

   EINA_SAFETY_ON_NULL_RETURN(name);
   EINA_SAFETY_ON_TRUE_RETURN(!name[0]);
   EINA_SAFETY_ON_TRUE_RETURN(value && (!value[0]));

   tmp = strdupa(name);
   eina_str_tolower(&tmp);

   if (EINA_UNLIKELY(!net->http.headers))
     {
        if (!value) return;
        net->http.headers = eina_hash_string_small_new((Eina_Free_Cb)eina_stringshare_del);
     }
   else if (!value)
     {
        eina_hash_del_by_key(net->http.headers, name);
        return;
     }

   n = eina_stringshare_add(value);
   if ((old = eina_hash_set(net->http.headers, tmp, n)))
     {
        eina_hash_set(net->http.headers, tmp, eina_stringshare_printf("%s;%s", old, value));
        eina_stringshare_del(old);
        eina_stringshare_del(n);
     }
}

/**
 * @brief Return the ip address associated with a network object
 *
 * This function returns the ip address of the client/server to which it represents.
 * Note that while the returned string is stringshared, it must not be freed since
 * it still belongs to @p net.
 * @param net The network object (NOT #NULL)
 * @return The ip address, or #NULL on failure
 */
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

/**
 * @brief Set the http transport to be used in a network object
 *
 * This function is similar, but NOT the same as setting the "content-type"
 * header, as it sets internal variables to make type retrieval faster.
 * @param net The network object (NOT #NULL)
 * @param transport The #Azy_Net_Transport to use
 */
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
   /* reset content-type header */
   azy_net_header_set(net, "content-type", NULL);
   if (transport == AZY_NET_TRANSPORT_XML)
     azy_net_header_set(net, "content-type", "text/xml");
   else if (transport == AZY_NET_TRANSPORT_JSON)
     azy_net_header_set(net, "content-type", "application/json");
}

/**
 * @brief Get the http transport used in a network object
 *
 * This function retrieves the #Azy_Net_Transport used in @p net,
 * representing the content-type.
 * @param net The network object (NOT #NULL)
 * @return The #Azy_Net_Transport used, or #AZY_NET_TRANSPORT_UNKNOWN on failure
 */
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

/**
 * @brief Create an http header string from data in a network object
 *
 * This function creates a full http header from data previously set in
 * @p net, including the http method line, content-type/length, and any others
 * which were set.
 * @param net The network object (NOT #NULL)
 * @return A new #Eina_Strbuf containing the header string, or #NULL on failure
 */
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

   EINA_SAFETY_ON_TRUE_RETURN_VAL((!net->http.headers) &&
                                  (net->type == AZY_NET_TYPE_NONE) &&
                                  (net->transport == AZY_NET_TRANSPORT_UNKNOWN), NULL);

   header = eina_strbuf_new();
   switch (net->type)
     {
      case AZY_NET_TYPE_GET:
        eina_strbuf_append_printf(header, "GET %s HTTP/1.%i\r\n",
                                  net->http.req.http_path, net->http.version);
        break;

      case AZY_NET_TYPE_POST:
        eina_strbuf_append_printf(header, "POST %s HTTP/1.%i\r\n",
                                  net->http.req.http_path, net->http.version);
        break;

      case AZY_NET_TYPE_PUT:
        eina_strbuf_append_printf(header, "PUT %s HTTP/1.%i\r\n",
                                  net->http.req.http_path, net->http.version);
        break;

      default:
        eina_strbuf_append_printf(header, "HTTP/1.%i %d %s\r\n",
                                  net->http.version, net->http.res.http_code, net->http.res.http_msg);
        if ((net->http.res.http_code == 426) || (net->http.res.http_code == 101))
          {
             azy_net_header_set(net, "upgrade", "TLS/1.0, HTTP/1.1");
             azy_net_header_set(net, "connection", "Upgrade");
          }
     }


   if (net->http.headers)
     eina_hash_foreach(net->http.headers, (Eina_Hash_Foreach)azy_net_header_hash_, header);

   eina_strbuf_append(header, "\r\n");
   return header;
}

/**
 * @brief Return a stringshared http status string
 *
 * This function takes an http status code and returns a stringshared string
 * which corresponds to that code.
 * @param code The status code (NOT #NULL)
 * @return The http status message for @p code
 */
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

      case 426:
        return eina_stringshare_add("Upgrade Required");

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

/** @} */
