/*
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <regex.h>
#include <ctype.h>
#include "Azy.h"
#include "azy_private.h"

#define AZY_SKIP_BLANK(PTR) \
  if (PTR && (len > 0) && isspace(*(PTR)))  \
    do                       \
      {                      \
         (PTR)++;              \
         len--;              \
      } while ((PTR) && isspace(*(PTR)) && (len > 0))

#define MAX_HEADER_SIZE 4096

static char _init = 0;
static regex_t __response;
static regex_t request;

static void
_azy_events_init(void)
{
   regcomp(&request, "^(GET|HEAD|POST|PUT) ([^ @\\]+) HTTP/1\\.([0-1])$", REG_EXTENDED);
   regcomp(&__response, "^HTTP/1\\.([0-1]) ([0-9]{3}) (.+)$", REG_EXTENDED);
   _init = 1;
}

static Eina_Bool
_azy_events_valid_header_name(const char *name, unsigned int len)
{
   while (len--)
     {
        if ((!isalnum(*name)) && (*name != '-'))
          return EINA_FALSE;

        name++;
     }

   return EINA_TRUE;
}

static Eina_Bool
_azy_events_valid_header_value(const char *name, unsigned int len)
{
   while (len--)
     {
        if (!isprint(*name))
          return EINA_FALSE;

        name++;
     }

   return EINA_TRUE;
}

int
azy_events_type_parse(Azy_Net            *net,
                       int                  type,
                       const unsigned char *header,
                       int                  len)
{
   regmatch_t match[4];
   char *first = NULL;
   const unsigned char *endline = NULL, *start = NULL;

   DBG("(net=%p, header=%p, len=%i)", net, header, len);
   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return 0;
     }

   if (net->size && net->buffer)
     {
        unsigned char *buf_start;
        int size;

        /* previous buffer */
        size = (net->size + len > MAX_HEADER_SIZE) ? MAX_HEADER_SIZE : net->size + len;
        buf_start = alloca(size);
        /* grab and combine buffers */
        if (header)
          {
             memcpy(buf_start, net->buffer, size);
             if (net->size < size)
               memcpy(buf_start + net->size, header, size - net->size);
             len = size;
          }
        else
          {
             memcpy(buf_start, net->buffer, size);
             len = size;
          }

        start = buf_start;
        AZY_SKIP_BLANK(start);
     }
   else
     {
        /* copy pointer */
         start = header;
         /* skip all spaces/newlines/etc and decrement len */
         AZY_SKIP_BLANK(start);
     }

   if (!start)
     return 0;

   /* some clients are dumb and send leading cr/nl/etc */
   AZY_SKIP_BLANK(start);

   if (!(endline = memchr(start, '\r', MAX_HEADER_SIZE)) && !(endline = memchr(start, '\n', MAX_HEADER_SIZE)))
     /*no newline/cr, so invalid start*/
     return 0;

   if ((endline - start) > MAX_HEADER_SIZE)
     /* FIXME: 4kb of headers is waaaaaaaaay too long for right now but I suppose it's possible? */
     return 0;

   /*null terminate*/
   first = alloca((endline - start) + 1);
   memcpy(first, start, endline - start);
   first[endline - start] = '\0';
   if (EINA_UNLIKELY(!_init))
     _azy_events_init();
   if (type == ECORE_CON_EVENT_CLIENT_DATA)
     {
        if (!regexec(&request, first, 4, match, 0))
          {
             int version = 1;
             char buf[8];

             memcpy(buf, start + match[3].rm_so, sizeof(buf));
             sscanf(buf, "%i", &version);
             net->http.version = version;
             net->http.req.http_path = eina_stringshare_add_length((const char*)start + match[2].rm_so, match[2].rm_eo - match[2].rm_so);

             if (!strncmp((const char*)start + match[1].rm_so, "GET", match[1].rm_eo - match[1].rm_so))
               net->type = AZY_NET_TYPE_GET;
             else if (!strncmp((const char*)start + match[1].rm_so, "POST", match[1].rm_eo - match[1].rm_so))
               net->type = AZY_NET_TYPE_POST;
             else if (!strncmp((const char*)start + match[1].rm_so, "PUT", match[1].rm_eo - match[1].rm_so))
               net->type = AZY_NET_TYPE_PUT;

             return match[3].rm_eo;
          }
     }
   else if (!regexec(&__response, first, 4, match, 0))
     {
        int code = -1;
        char buf[8];

        memcpy(buf, start + match[2].rm_so, sizeof(buf));
        if (sscanf(buf, "%3i", &code) == 1)
          net->http.res.http_code = code;
        eina_stringshare_replace_length(&net->http.res.http_msg, (const char *)start + match[3].rm_so, match[3].rm_eo - match[3].rm_so);

        net->type = AZY_NET_TYPE_RESPONSE;

        return match[3].rm_eo;
     }

   return 0;
}

Eina_Bool
azy_events_header_parse(Azy_Net      *net,
                         unsigned char *event_data,
                         size_t         event_len,
                         int            offset)
{
   unsigned char *c = NULL, *r = NULL, *p = NULL, *start = NULL, *buf_start = NULL;
   unsigned char *data = (event_data) ? event_data + offset : NULL;
   size_t len = (event_len) ? event_len - offset : 0;
   const char *s = NULL;
   unsigned char slen = 0;
   unsigned char sep[5];
   int line_len = 0;
   long long int prev_size = 0;

   DBG("(net=%p, event_data=%p, len=%zu, offset=%i)", net, event_data, event_len, offset);
   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return EINA_FALSE;
     }
   if (net->headers_read)
     return EINA_TRUE;
   EINA_SAFETY_ON_TRUE_RETURN_VAL((!net->buffer) && (!data), EINA_FALSE);

   if (net->size && net->buffer)
     {
#if 0
        if (event_data)
          {
             char buf[64];
             snprintf(buf, sizeof(buf), "STORED:\n<<<<<<<<<<<<<\n%%.%llis\n<<<<<<<<<<<<<", net->size);
             INFO(buf, net->buffer);
             snprintf(buf, sizeof(buf), "RECEIVED:\n<<<<<<<<<<<<<\n%%.%zus\n<<<<<<<<<<<<<", len - offset);
             INFO(buf, data);

          }
#endif
        /* previous buffer */
         /* alloca should be safe here because ecore_con reads at most 64k
          * and even if no headers were found previously, the entire
          * buffer would not be copied
          */
        buf_start = alloca(len + net->size - offset);
        /* grab and combine buffers */
        if (event_data)
          {
             memcpy(buf_start, net->buffer + offset, net->size - offset);
             memcpy(buf_start + net->size, event_data, len);
          }
        else
          memcpy(buf_start, net->buffer + offset, net->size - offset);
          
        free(net->buffer);
        net->buffer = NULL;
        len += net->size - offset;

        prev_size = net->size;
        net->size = 0;
        start = buf_start;
        AZY_SKIP_BLANK(start);
     }
   else
   /* only current buffer plus possible net->overflow */
     {
        /* copy pointer */
         start = data;
         /* skip all spaces/newlines/etc and decrement len */
         AZY_SKIP_BLANK(start);
     }

   /* apparently this can happen? */
   EINA_SAFETY_ON_NULL_RETURN_VAL(start, EINA_FALSE);
   /* find a header or append to buffer */
   if ((!(r = memchr(start, '\r', len)) && !(r = memchr(start, '\n', len))) || !(c = memchr(start, ':', len)))
     {  /* append to a buffer and use net->overflow */
        unsigned char *tmp;

        if (net->size)
          {
             tmp = realloc(net->buffer, net->size + len);
             EINA_SAFETY_ON_NULL_RETURN_VAL(tmp, EINA_FALSE);

             net->buffer = tmp;
             memcpy(net->buffer + net->size, start, len);
             net->size += len;
          }
        else
          {
             tmp = realloc(net->buffer, len);
             EINA_SAFETY_ON_NULL_RETURN_VAL(tmp, EINA_FALSE);

             net->buffer = tmp;
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
   snprintf((char *)sep, sizeof(sep), "%s%s", s, s);
   /* by spec, this is only found between header and content */
   if (azy_memstr(start, sep, len, 2 * slen))
     net->headers_read = EINA_TRUE;

   p = start;
   line_len = r - p;
   while (len && c && r)
     {
        const unsigned char *ptr, *semi;

        if (line_len > 4096)
          {
             WARN("Ignoring unreasonably large header starting with:\n %.32s\n", p);
             goto skip_header;
          }
        semi = memchr(p, ':', line_len);
        if ((!semi) || (semi - p + 1 >= line_len))
          goto skip_header;
        if (!_azy_events_valid_header_name((const char*)p, semi - p))
          goto skip_header;

        ptr = semi + 1;
        while ((isspace(*ptr)) && (ptr - p < line_len))
          ptr++;

        if (!_azy_events_valid_header_value((const char*)ptr, line_len - (ptr - p)))
          goto skip_header;
        {
           char *key, *value;

           key = strndupa((const char*)p, semi - p);
           value = strndupa((const char*)ptr, line_len - (ptr - p));
           INFO("Found header: key='%s'", key);
           INFO("Found header: value='%s'", value);
           azy_net_header_set(net, key, value);
        }


skip_header:
        len -= line_len + slen;
        if (len < slen)
          break;
        p = r + slen;
        if (net->headers_read)
          /* double separator: STOP */
          if (azy_memstr(p, (const unsigned char *)s, slen, slen))
            break;
        r = azy_memstr(p, (const unsigned char *)s, len, slen);
        line_len = r - p;
        /* FIXME: to be fully 1.1 compliant, lines like this should
         * be filtered and checked to see if is a continuing header
         * from the previous line
         */
        if (!(c = memchr(p, ':', line_len)))
          goto out;
     }

   AZY_SKIP_BLANK(p);

out:
   if (!net->headers_read)
     return EINA_TRUE;

   if (!(c = eina_hash_find(net->http.headers, "content-length")))
     net->http.content_length = -1;
   else
     net->http.content_length = strtol((const char *)c, NULL, 10);
   if (len)
     {
        size_t rlen;
        /* if we get here, we need to append to the buffers */

        if (net->http.content_length > 0)
          {
             if ((long long int)len > net->http.content_length)
               {
                  rlen = net->http.content_length;
                  net->overflow_length = (long long int)(len - rlen);
                  WARN("Extra content length of %lli!", net->overflow_length);
                  net->overflow = malloc(net->overflow_length);
                  /* FIXME: uhhhh fuck? */
                  EINA_SAFETY_ON_NULL_RETURN_VAL(net->overflow, EINA_FALSE);
                  memcpy(net->overflow, p + rlen, net->overflow_length);
#ifdef ISCOMFITOR
     {
        long long int x;
        INFO("OVERFLOW:\n<<<<<<<<<<<<<");
        for (x = 0; x < net->overflow_length; x++)
          putc(net->overflow[x], stdout);
        fflush(stdout);
     }
#endif
               }
             else
               rlen = len;
          }
        else
          /* this shouldn't be possible unless someone is violating spec */
          rlen = len;
        
        INFO("Set recv size to %zu (previous %lli)", rlen, prev_size);
        net->size = rlen;
        net->buffer = malloc(rlen);
        /* FIXME: cleanup */
        EINA_SAFETY_ON_NULL_RETURN_VAL(net->buffer, EINA_FALSE);

        memcpy(net->buffer, p, rlen);
     }

   return EINA_TRUE;
}

Azy_Net_Transport
azy_events_net_transport_get(const char *content_type)
{
   if (!content_type)
     return AZY_NET_TRANSPORT_TEXT;

   if (!strncmp(content_type, "text/xml", 8))
     return AZY_NET_TRANSPORT_XML;

   if (!strncmp(content_type, "application/xml", 15))
     return AZY_NET_TRANSPORT_XML;

   if (!strncmp(content_type, "application/json", 16))
     return AZY_NET_TRANSPORT_JSON;

   if (!strncmp(content_type, "text/plain", 10))
     return AZY_NET_TRANSPORT_TEXT;

   if (!strncmp(content_type, "text/html", 9))
     return AZY_NET_TRANSPORT_HTML;

   return -1;
}

Eina_Bool
azy_events_connection_kill(void             *conn,
                            Eina_Bool         server_client,
                            const char       *msg)
{
   DBG("(conn=%p, server_client=%i, msg='%s')", conn, server_client, msg);
   if (msg)
     {
        if (server_client)
          ecore_con_client_send(conn, msg, strlen(msg));
        else
          ecore_con_server_send(conn, msg, strlen(msg));
     }

   if (server_client)
     ecore_con_client_del(conn);
   else
     ecore_con_server_del(conn);
   return ECORE_CALLBACK_RENEW;
}

void
_azy_event_handler_fake_free(void *data __UNUSED__, void *data2 __UNUSED__)
{}
