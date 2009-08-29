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

#ifndef _EUPNP_HTTP_MESSAGE_H
#define _EUPNP_HTTP_MESSAGE_H

#define EUPNP_HTTP_VERSION "HTTP/1.1"
#define EUPNP_HTTP_VERSION_LEN 8

#include "eupnp_udp_transport.h"

struct _Eupnp_HTTP_Header {
   const char *key;
   const char *value;
};

struct _Eupnp_HTTP_Request {
   Eina_Array *headers;
   const char *method;
   const char *uri;
   const char *http_version;
   const char *payload;

   /* Private */
   void *_resource; /* Shared resource */
   void (*_resource_free)(void *resource); /* Resource free function */
};

struct _Eupnp_HTTP_Response {
   Eina_Array *headers;
   const char *http_version;
   const char *reason_phrase;
   int status_code;

   /* Private */
   void *_resource; /* Shared resource */
   void (*_resource_free)(void *resource); /* Resource free function */
};

typedef struct _Eupnp_HTTP_Request Eupnp_HTTP_Request;
typedef struct _Eupnp_HTTP_Response Eupnp_HTTP_Response;
typedef struct _Eupnp_HTTP_Header Eupnp_HTTP_Header;


Eupnp_HTTP_Request  *eupnp_http_request_parse(const char *msg, void *resource, void (*resource_free)(void *resource)) EINA_ARG_NONNULL(1);
Eupnp_HTTP_Response *eupnp_http_response_parse(const char *msg, void *resource, void (*resource_free)(void *resource)) EINA_ARG_NONNULL(1);

Eina_Bool            eupnp_http_message_is_response(const char *msg) EINA_ARG_NONNULL(1);
Eina_Bool            eupnp_http_message_is_request(const char *msg) EINA_ARG_NONNULL(1);

Eupnp_HTTP_Header   *eupnp_http_header_new(const char *key, int key_len, const char *value, int value_len) EINA_ARG_NONNULL(1,2,3);
void                 eupnp_http_header_free(Eupnp_HTTP_Header *h) EINA_ARG_NONNULL(1);
const char          *eupnp_http_header_get(Eina_Array *headers, const char *key) EINA_ARG_NONNULL(1,2);

Eupnp_HTTP_Request  *eupnp_http_request_new(const char *method, int method_len, const char *uri, int uri_len, const char *httpver, int httpver_len, void *resource, void (*resource_free)(void *resource)) EINA_ARG_NONNULL(1,2,3,4,5,6,7,8);
void                 eupnp_http_request_free(Eupnp_HTTP_Request *r) EINA_ARG_NONNULL(1);
void                 eupnp_http_request_dump(const Eupnp_HTTP_Request *r) EINA_ARG_NONNULL(1);
Eina_Bool            eupnp_http_request_header_add(Eupnp_HTTP_Request *r, const char *key, int key_len, const char *value, int value_len) EINA_ARG_NONNULL(1,2,3,4);
const char          *eupnp_http_request_header_get(Eupnp_HTTP_Request *r, const char *key) EINA_ARG_NONNULL(1,2);

Eupnp_HTTP_Response *eupnp_http_response_new(const char *httpver, int httpver_len, const char *status_code, int status_code_len, const char *reason_phrase, int reason_phrase_len, void *resource, void (*resource_free)(void *resource)) EINA_ARG_NONNULL(1,2,3,4,5,6,7,8);
void                 eupnp_http_response_free(Eupnp_HTTP_Response *r) EINA_ARG_NONNULL(1);
void                 eupnp_http_response_dump(Eupnp_HTTP_Response *r) EINA_ARG_NONNULL(1);
Eina_Bool            eupnp_http_response_header_add(Eupnp_HTTP_Response *r, const char *key, int key_len, const char *value, int value_len) EINA_ARG_NONNULL(1,2,3,4);
const char          *eupnp_http_response_header_get(Eupnp_HTTP_Response *r, const char *key) EINA_ARG_NONNULL(1,2);


#endif /* _EUPNP_HTTP_MESSAGE_H */

