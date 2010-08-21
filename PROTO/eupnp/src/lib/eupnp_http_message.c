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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include <Eina.h>

#include "Eupnp.h"
#include "eupnp_http_message.h"
#include "eupnp_udp_transport.h"
#include "eupnp_private.h"

/*
 * Private API
 */

/*
 * Parses the first line of a HTTP message
 *
 * Parses first line of the form "a<SP>b<SP>c<SP>" and stores the points on the
 * pointers @p a, @p b and @p c given. Also marks @p headers_start on the
 * beginning of the headers 
 */
static Eina_Bool
eupnp_http_datagram_line_parse(const char *msg, const char **headers_start, const char **a, int *a_len, const char **b, int *b_len, const char **c, int *c_len)
{
   /*
    * Parse first line of the form "a SP b SP c\r\n"
    */
   const char *begin, *end;
   const char *vbegin, *vend;

   *a = msg;
   end = strchr(*a, ' ');

   if (!end)
     {
	ERR("Could not parse DATAGRAM.");
	return EINA_FALSE;
     }

   *a_len = end - *a;

   /* Move our starting point to b */
   *b = end + 1;
   end = strchr(*b, ' ');

   if (!end)
     {
	ERR("Could not parse HTTP.");
	return EINA_FALSE;
     }

   *b_len = end - *b;
   *c = end + 1;
   end = strstr(*c, "\r\n");

   if (!end)
     {
	ERR("Could not parse HTTP request.");
	return EINA_FALSE;
     }

   *c_len = end - *c;
   *headers_start = end + 2;

   return EINA_TRUE;
}

/*
 * Parses HTTP headers
 *
 * Given the starting point, parses the next header and sets the starting point
 * to the next header, if present. Sets the given pointers to the parsed key
 * and value.
 */
static Eina_Bool
eupnp_http_datagram_header_next_parse(const char **line_start, const char **hkey, int *hkey_len, const char **hvalue, int *hvalue_len, int *payload)
{
   const char *end;

   if (!line_start)
      return EINA_FALSE;

   *hkey = *line_start;

   // Find first ':'. Do not trim spaces between the key and ':' - not on
   // RFC2616.
   end = strchr(*hkey, ':');

   if (!end)
     {
	*line_start = NULL;
	return EINA_FALSE;
     }

   *hkey_len = end - *hkey;

   // Move to the first char after ':' and check if the header value is empty.
   *hvalue = end + 1;

   if (**hvalue == '\r' && *(*hvalue+1) == '\n')
     {
	DBG("Empty header value!");
	*line_start = *hvalue + 2;
	*hvalue_len = 0;
	return EINA_TRUE;
     }

   // Header value not empty, skip whitespaces before the actual value.
   while (**hvalue == ' ') (*hvalue)++;

   if (**hvalue == '\r' && *(*hvalue+1) == '\n')
     {
	DBG("Empty header value!");
	*line_start = *hvalue + 2;
	*hvalue_len = 0;
	return EINA_TRUE;
     }

   // Mark value and skip possible whitespaces between value and \r
   end = *hvalue;

   while (*(end+1) != '\r') end++;

   if (*(end+2) != '\n')
     {
	DBG("Header parsing error: character after carrier is not \\n Possibly headers end");
	return EINA_FALSE;
     }

   *hvalue_len = end - *hvalue + 1;

   if (*(end + 2) == '\n' && *(end + 3) == '\r' && *(end + 4) == '\n')
     {
	*line_start = end + 5;
	*payload = 1;
	return EINA_TRUE;
     }

   /* Set line_start for next header */
   *line_start = end + 3;

   return EINA_TRUE;
}

/*
 * Public API
 */

/*
 * Constructor for the Eupnp_HTTP_Header structure
 *
 * Receives pointers to key and value starting points and the length that will
 * be copied from that point on (length). The key will be automatically
 * converted to lowercase.
 *
 * @param key starting point of the key
 * @param key_len key length
 * @param value starting point of the value
 * @param value_len value length
 *
 * @return Eupnp_HTTP_Header instance
 */
Eupnp_HTTP_Header *
eupnp_http_header_new(const char *key, int key_len, const char *value, int value_len)
{
   Eupnp_HTTP_Header *h;

   /* Alloc blob */
   h = malloc(sizeof(Eupnp_HTTP_Header) + key_len + 1 + value_len + 1);

   if (!h)
     {
	eina_error_set(EINA_ERROR_OUT_OF_MEMORY);
	ERR("header alloc error.");
	return NULL;
     }

   h->key = (char *)h + sizeof(Eupnp_HTTP_Header);
   h->value = (char *)h + sizeof(Eupnp_HTTP_Header) + sizeof(char)*(key_len + 1);
   memcpy((void *)h->key, key, key_len);
   memcpy((void *)h->value, value, value_len);
   ((char *) h->key)[key_len] = '\0';
   ((char *) h->value)[value_len] = '\0';

   return h;
}

/*
 * Destructor for the Eupnp_HTTP_Header structure
 *
 * Frees the object and its attributes.
 *
 * @param h previously created header
 */
void
eupnp_http_header_free(Eupnp_HTTP_Header *h)
{
   free(h);
}

static void
eupnp_http_header_key_lower(Eupnp_HTTP_Header *h)
{
   CHECK_NULL_RET(h);

   char *p = (char *) h->key;

   while (*p != '\0')
     {
	*p = tolower(*p);
	(p)++;
     }
}

/*
 * Retrieves the header value associated with the key
 *
 * Returns the header value for the given key, if the header is present on the
 * array passed. Key given must be lowercase.
 *
 * @param headers headers array
 * @param key key to search for
 *
 * @return value of the header associated with the key or NULL if not found.
 */
const char *
eupnp_http_header_get(const Eina_Array *headers, const char *key)
{
   CHECK_NULL_RET(headers, NULL);
   CHECK_NULL_RET(key, NULL);

   Eina_Array_Iterator it;
   int i;
   Eupnp_HTTP_Header *h;

   EINA_ARRAY_ITER_NEXT(headers, i, h, it)
      if (!strcmp(h->key, key))
	return h->value;

   return NULL;
}

/*
 * Constructor for the Eupnp_HTTP_Request structure
 *
 * Receives pointers to starting points and lengths of the method, uri
 * and http version attributes and constructs the object. Also initializes the
 * headers array. For dealing with headers, refer to the
 * eupnp_http_request_header_* functions.
 *
 * @param method starting point of the method
 * @param method_len method length
 * @param uri starting point of the uri
 * @param uri_len uri length
 * @param httpver starting point of the http version
 * @param httpver_len http version length
 *
 * @return Eupnp_HTTP_Request instance
 */
Eupnp_HTTP_Request *
eupnp_http_request_new(const char *method, int method_len, const char *uri, int uri_len, const char *httpver, int httpver_len, void *resource, void (*resource_free)(void *resource))
{
   Eupnp_HTTP_Request *h;
   h = malloc(sizeof(Eupnp_HTTP_Request));

   if (!h)
     {
	eina_error_set(EINA_ERROR_OUT_OF_MEMORY);
	ERR("Could not create HTTP message.");
	return NULL;
     }

   h->headers = eina_array_new(10);

   if (!h->headers)
     {
	ERR("Could not allocate memory for HTTP headers table.");
	goto req_headers_error;
     }

   h->method = eina_stringshare_add_length(method, method_len);

   if (!h->method)
     {
	ERR("Could not stringshare HTTP request method.");
	goto req_method_error;
     }

   h->http_version = eina_stringshare_add_length(httpver, httpver_len);

   if (!h->http_version)
     {
	ERR("Could not stringshare HTTP request version.");
	goto req_httpver_error;
     }

   h->uri = eina_stringshare_add_length(uri, uri_len);

   if (!h->uri)
     {
	ERR("Could not stringshare HTTP request URI.");
	goto req_uri_error;
     }

   h->_resource = resource;
   h->_resource_free = resource_free;
   h->payload = NULL;

   return h;

   req_uri_error:
      eina_stringshare_del(h->http_version);
   req_httpver_error:
      eina_stringshare_del(h->method);
   req_method_error:
      eina_array_free(h->headers);
   req_headers_error:
      free(h);

   // Free the resource
   if (resource && resource_free)
      resource_free(resource);

   return NULL;
}

/*
 * Destructor for the Eupnp_HTTP_Request structure
 *
 * Frees the object and its attributes, including headers added.
 *
 * @param r previously created request
 */
void
eupnp_http_request_free(Eupnp_HTTP_Request *r)
{
   CHECK_NULL_RET(r);

   eina_stringshare_del(r->method);
   eina_stringshare_del(r->http_version);
   eina_stringshare_del(r->uri);
   free((char *)r->payload);

   if (r->headers)
     {
	Eina_Array_Iterator it;
	Eupnp_HTTP_Header *h;
	int i;

	EINA_ARRAY_ITER_NEXT(r->headers, i, h, it)
	   eupnp_http_header_free(h);

	eina_array_free(r->headers);
     }

   /* Free the shared resource */
   if (r->_resource && r->_resource_free)
      r->_resource_free(r->_resource);

   free(r);
}


/*
 * Prints out info about the Eupnp_HTTP_Request object
 *
 * Use EINA_ERROR_LEVEL=3 for seeing the printed messages.
 *
 * @param r: previously created request
 */
void
eupnp_http_request_dump(const Eupnp_HTTP_Request *r)
{
   CHECK_NULL_RET(r);

   DBG("Dumping HTTP request");
   if (r->method)
      DBG("* Method: %s", r->method);
   if (r->uri)
      DBG("* URI: %s", r->uri);
   if (r->http_version)
     DBG("* HTTP Version: %s", r->http_version);

   if (r->headers)
     {
	Eina_Array_Iterator it;
	Eupnp_HTTP_Header *h;
	int i;

	EINA_ARRAY_ITER_NEXT(r->headers, i, h, it)
	   DBG("** %s:\t%s", h->key, h->value);
     }

   if (r->payload)
      DBG("* Payload: %s", r->payload);

}

/*
 * Adds a header to a request
 *
 * Adds a header to the request object given pointers to the key
 * and value starting points and respective string lengths.
 *
 * @param m request to add the header into
 * @param key starting point for the key
 * @param key_len key length
 * @param value starting point for the value
 * @param value_len value length
 *
 * @note This function does not check duplicate headers. If added twice,
 *       header will be duplicated.
 *
 * @return On success returns EINA_TRUE, otherwise EINA_FALSE.
 */
Eina_Bool
eupnp_http_request_header_add(Eupnp_HTTP_Request *r, const char *key, int key_len, const char *value, int value_len)
{
   Eupnp_HTTP_Header *h;

   h = eupnp_http_header_new(key, key_len, value, value_len);
   eupnp_http_header_key_lower(h);

   if (!h)
     {
	eina_error_set(EINA_ERROR_OUT_OF_MEMORY);
	ERR("header alloc error.");
	return EINA_FALSE;
     }

   if (!eina_array_push(r->headers, h))
     {
	WRN("incomplete headers");
	eupnp_http_header_free(h);
	return EINA_FALSE;
     }

   return EINA_TRUE;
}

/*
 * Retrieves the header value associated with the key of a HTTP request
 *
 * Returns the header value for the given key, if the header is present on the
 * given HTTP request. Key given must be lowercase.
 *
 * @param m HTTP request
 * @param key key to search for
 *
 * @return value of the header associated with the key or NULL if not found.
 */
const char *
eupnp_http_request_header_get(Eupnp_HTTP_Request *r, const char *key)
{
   CHECK_NULL_RET(r, NULL);
   CHECK_NULL_RET(r->headers, NULL);
   CHECK_NULL_RET(key, NULL);

   return eupnp_http_header_get(r->headers, key);
}

/*
 * Constructor for the Eupnp_HTTP_Response structure
 *
 * Receives pointers to starting points and lengths of the http version, status
 * code and http version attributes and constructs the object. Also initializes
 * the headers array. For dealing with headers, refer to the
 * eupnp_http_response_header_* functions.
 *
 * @param httpver starting point of the http version
 * @param httpver_len http version length
 * @param status_code starting point of the status code
 * @param status_len status length
 * @param reason_phrase starting point of the reason phrase
 * @param reason_phrase_len reason phrase length
 *
 * @return Eupnp_HTTP_Response instance
 */
Eupnp_HTTP_Response *
eupnp_http_response_new(const char *httpver, int httpver_len, const char *status_code, int status_code_len, const char *reason_phrase, int reason_phrase_len, void *resource, void (*resource_free)(void *resource))
{
   Eupnp_HTTP_Response *r;
   r = malloc(sizeof(Eupnp_HTTP_Response));

   if (!r)
     {
	eina_error_set(EINA_ERROR_OUT_OF_MEMORY);
	ERR("Could not create HTTP response.");
	return NULL;
     }

   r->headers = eina_array_new(10);

   if (!r->headers)
     {
	ERR("Could not allocate memory for HTTP headers.");
	goto resp_headers_error;
     }

   r->http_version = eina_stringshare_add_length(httpver, httpver_len);

   if (!r->http_version)
     {
	ERR("Could not stringshare http response version.");
	goto resp_httpver_error;
     }

   r->status_code = strtol(status_code, NULL, 10);
   r->reason_phrase = eina_stringshare_add_length(reason_phrase,
						    reason_phrase_len);

   if (!r->reason_phrase)
     {
	ERR("Could not stringshare http response phrase.");
	goto resp_reason_error;
     }

   r->_resource = resource;
   r->_resource_free = resource_free;

   return r;

   resp_reason_error:
      eina_stringshare_del(r->http_version);
   resp_httpver_error:
      eina_array_free(r->headers);
   resp_headers_error:
      free(r);

   // Free the resource
   if (resource && resource_free)
      resource_free(resource);

   return NULL;
}

/*
 * Destructor for the Eupnp_HTTP_Response structure
 *
 * Frees the object and its attributes, including headers added.
 *
 * @param r previously created response
 */
void
eupnp_http_response_free(Eupnp_HTTP_Response *r)
{
   CHECK_NULL_RET(r);

   eina_stringshare_del(r->http_version);
   eina_stringshare_del(r->reason_phrase);

   if (r->headers)
     {
	Eina_Array_Iterator it;
	Eupnp_HTTP_Header *h;
	int i;

	EINA_ARRAY_ITER_NEXT(r->headers, i, h, it)
	   eupnp_http_header_free(h);

	eina_array_free(r->headers);
     }

   /* Free the shared resource */
   if (r->_resource)
      r->_resource_free(r->_resource);

   free(r);
}

/*
 * Prints out info about the Eupnp_HTTP_Response object
 *
 * Use EINA_ERROR_LEVEL=3 for seeing the printed messages.
 *
 * @param r: previously created response
 */
void
eupnp_http_response_dump(Eupnp_HTTP_Response *r)
{
   CHECK_NULL_RET(r);

   DBG("Dumping HTTP response");
   if (r->http_version)
      DBG("* HTTP Version: %s", r->http_version);
   if (r->status_code)
      DBG("* Status Code: %d", r->status_code);
   if (r->reason_phrase)
      DBG("* Reason Phrase: %s", r->reason_phrase);

   if (r->headers)
     {
	Eina_Array_Iterator it;
	Eupnp_HTTP_Header *h;
	int i;

	EINA_ARRAY_ITER_NEXT(r->headers, i, h, it)
	   DBG("** %s: %s", h->key, h->value);
     }
}

/*
 * Adds a header to a response
 *
 * Adds a header to the response object given pointers to the key
 * and value starting points and respective string lengths.
 *
 * @param m request to add the header into
 * @param key starting point for the key
 * @param key_len key length
 * @param value starting point for the value
 * @param value_len value length
 *
 * @note This function does not check duplicate headers. If added twice,
 *       header will be duplicated.
 *
 * @return On success returns EINA_TRUE, otherwise EINA_FALSE.
 */
Eina_Bool
eupnp_http_response_header_add(Eupnp_HTTP_Response *r, const char *key, int key_len, const char *value, int value_len)
{
   Eupnp_HTTP_Header *h;

   h = eupnp_http_header_new(key, key_len, value, value_len);
   eupnp_http_header_key_lower(h);

   if (!h)
     {
	eina_error_set(EINA_ERROR_OUT_OF_MEMORY);
	ERR("header alloc error.");
	return EINA_FALSE;
     }

   if (!eina_array_push(r->headers, h))
     {
	WRN("incomplete headers");
	eupnp_http_header_free(h);
	return EINA_FALSE;
     }

   return EINA_TRUE;
}

/*
 * Retrieves the header value associated with the key of a HTTP response
 *
 * Returns the header value for the given key, if the header is present on the
 * given HTTP response. Key given must be lowercase.
 *
 * @param m HTTP response
 * @param key key to search for
 *
 * @return value of the header associated with the key or NULL if not found.
 */
const char *
eupnp_http_response_header_get(Eupnp_HTTP_Response *r, const char *key)
{
   CHECK_NULL_RET(r, NULL);
   CHECK_NULL_RET(r->headers, NULL);
   CHECK_NULL_RET(key, NULL);

   return eupnp_http_header_get(r->headers, key);
}

/*
 * Checks if a message type is response
 *
 * @p msg HTTP message
 *
 * @return EINA_TRUE if the message type is response, EINA_FALSE otherwise.
 */
Eina_Bool
eupnp_http_message_is_response(const char *msg)
{
   if (!strncmp(msg, EUPNP_HTTP_VERSION, EUPNP_HTTP_VERSION_LEN))
      return EINA_TRUE;
   return EINA_FALSE;
}

/*
 * Checks if a message type is request
 *
 * @p msg HTTP message
 *
 * @return EINA_TRUE if the message type is request, EINA_FALSE otherwise.
 */
Eina_Bool
eupnp_http_message_is_request(const char *msg)
{
   return (!eupnp_http_message_is_response(msg));
}

/*
 * Parses a request message and mounts the request object
 *
 * Parses a HTTP request (previously known to be a request, see
 * eupnp_http_message_is_request()) and returns the
 * request object with attributes already set.
 *
 * @param msg HTTP message.
 * @param resource Resource that contains the message
 * @param resource_free Function that correctly frees the resource
 *
 * @note The resouce parameter expected is a information holder structure, e.g.
 *       Eupnp_UDP_Datagram, Eupnp_TCP_Message and so on.
 *
 * @return Eupnp_HTTP_Request instance if parsed successfully, NULL otherwise.
 */
Eupnp_HTTP_Request *
eupnp_http_request_parse(const char *msg, void *resource, void (*resource_free)(void *resource))
{
   CHECK_NULL_RET(msg, NULL);

   Eupnp_HTTP_Request *r;
   const char *method;
   const char *uri;
   const char *http_version;
   const char *headers_start, *next_header;
   const char *hkey_begin, *hv_begin;
   int method_len, uri_len, httpver_len;
   int hk_len, hv_len;
   int payload = 0;

   if (!eupnp_http_datagram_line_parse(msg, &headers_start, &method, &method_len, &uri, &uri_len, &http_version, &httpver_len))
     {
	ERR("Could not parse request line.");
	return NULL;
     }

   r = eupnp_http_request_new(method, method_len, uri, uri_len,
				   http_version, httpver_len,
				   resource, resource_free);

   if (!r)
     {
	ERR("Could not create new HTTP request.");
	return NULL;
     }

   next_header = headers_start;

   while (next_header)
     {
	if (eupnp_http_datagram_header_next_parse(&next_header, &hkey_begin, &hk_len, &hv_begin, &hv_len, &payload))
	  {
	     if (!eupnp_http_request_header_add(r, hkey_begin, hk_len, hv_begin, hv_len))
	       {
		  ERR("Could not add header to the request.");
		  break;
	       }

	     if (payload)
	       {
		 if (next_header)
		   {
		      // We always receive null terminated messages
		      r->payload = strdup(next_header);
		   }
		 break;
	       }
	  }
	else
	  {
	     // Finished parsing headers
	     break;
	  }
     }

   return r;
}

/*
 * Parses a response message and mounts the response object
 *
 * Parses a HTTP response (previously known to be a response, see
 * eupnp_http_message_is_response()) and returns the
 * response object with attributes already set.
 *
 * @param msg HTTP message
 * @param resource Resource that contains the message
 * @param resource_free Function that correctly frees the resource
 *
 * @note The resouce parameter expected is a information holder structure, e.g.
 *       Eupnp_UDP_Datagram, Eupnp_TCP_Message and so on.
 *
 * @return Eupnp_HTTP_Response instance if parsed successfully, NULL otherwise.
 */
Eupnp_HTTP_Response *
eupnp_http_response_parse(const char *msg, void *resource, void (*resource_free)(void *resource))
{
   CHECK_NULL_RET(msg, NULL);
   CHECK_NULL_RET(resource, NULL);
   CHECK_NULL_RET(resource_free, NULL);

   Eupnp_HTTP_Response *r;
   const char *reason_phrase;
   const char *status_code;
   const char *http_version;
   const char *headers_start, *next_header;
   const char *hkey_begin, *hv_begin;
   int sc_len, rp_len, httpver_len;
   int hk_len, hv_len;
   int payload = 0;

   if (!eupnp_http_datagram_line_parse
		(msg, &headers_start, &http_version, &httpver_len, &status_code,
		 &sc_len, &reason_phrase, &rp_len))
     {
	ERR("Could not parse response line.");
	return NULL;
     }

   r = eupnp_http_response_new(http_version, httpver_len, status_code,
				    sc_len, reason_phrase, rp_len,
				    resource, resource_free);

   if (!r)
     {
	ERR("Could not create new HTTP response.");
	return NULL;
     }

   next_header = headers_start;

   while (next_header)
     {
	if (eupnp_http_datagram_header_next_parse(&next_header, &hkey_begin, &hk_len, &hv_begin, &hv_len, &payload))
	  {
	     if (!eupnp_http_response_header_add(r, hkey_begin, hk_len, hv_begin, hv_len))
	       {
		  ERR("Could not add header to the response.");
		  break;
	       }
	  }
	else
	   break;
     }

   return r;
}
