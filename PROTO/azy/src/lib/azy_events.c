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

#include <ctype.h>
#include <inttypes.h>
#include <errno.h>
#include "Azy.h"
#include "azy_private.h"

#define AZY_SKIP_BLANK(PTR)                \
  if (PTR && (len > 0) && isspace(*(PTR))) \
    do                                     \
      {                                    \
         (PTR)++;                          \
         len--;                            \
      } while ((PTR) && (len > 0) && isspace(*(PTR)))

#define MAX_HEADER_SIZE 8092

static unsigned int
_azy_events_valid_header_name(const char  *start,
                              unsigned int len)
{
   while ((*start != ':') && (len--))
     {
        if ((!isalnum(*start)) && (*start != '-'))
          return 0;

        start++;
     }

   if (*start == ':') return len;
   return 0;
}

static int
_azy_events_valid_response(Azy_Net *net,
                           const unsigned char *header,
                           int len)
{
   const unsigned char *start;
   unsigned char *p;
   int code;

   if ((len < 16) || strncmp((char*)header, "HTTP/1.", sizeof("HTTP/1.") - 1)) return 0;
   start = header;
   start += 7; len -= 7;

   switch (start[0])
     {
      case '0':
        break;
      case '1':
        net->http.version = 1;
        break;
      default:
        return 0;
     }

   while ((len > 1) && (start[0] == ' '))
     start++; len--;
   if ((len < 7) || (!isdigit(start[0]))) return 0;

   errno = 0;
   code = strtol((char*)start, (char**)&p, 10);
   if (errno || (code < 1) || (code > 999) || (p[0] != ' ')) return 0;
   INFO("HTTP RESPONSE %"PRIi32, code);
   net->http.res.http_code = code;
   len -= (p - start); start += (p - start);

   while ((len > 1) && (start[0] == ' '))
     start++; len--;
   if (len < 3) return 0;
   for (p = (unsigned char*)start; len; len--)
     {
        if ((p[0] == '\r') || (p[0] == '\n'))
          {
             if (p == start) return 0;
             net->http.res.http_msg = eina_stringshare_add_length((char*)start, p - start);
             break;
          }
        if (!isprint(p[0])) return 0;
        if (len > 1) p++;
     }
   if (!net->http.res.http_msg) return 0;
   net->type = AZY_NET_TYPE_RESPONSE;
   return (int)(p - header);
}

static int
_azy_events_valid_request(Azy_Net *net,
                          const unsigned char *header,
                          int len)
{
   char *p = NULL, *uri = NULL;
   const unsigned char *start, *path_start;
   int orig_len;

   DBG("(net=%p, header=%p, len=%i)", net, header, len);
   if (len < 16) return 0;

   start = header;;
   switch (start[0])
     {
      case 'H':
        if (strncmp((char*)start + 1, "EAD", 3)) return 0;
        return EINA_TRUE; /* FIXME: still unsupported */
      case 'G':
        if (strncmp((char*)start + 1, "ET", 2)) return 0;
        net->type = AZY_NET_TYPE_GET;
        start += 3; len -= 3;
        break;
      case 'P':
        if (!strncmp((char*)start + 1, "OST", 3))
          {
             net->type = AZY_NET_TYPE_POST;
             start += 4; len -= 4;
          }
        else if (!strncmp((char*)start + 1, "UT", 2))
          {
             net->type = AZY_NET_TYPE_PUT;
             start += 3; len -= 3;
          }
        else return 0;
        break;
      default:
        return 0;
     }
   if (start[0] != ' ') return 0;
   start++; len--;
   path_start = start;
   orig_len = len;
   for (; len; start++)
     {
        const unsigned char *end;
        for (end = start; len; len--)
          {
             switch (end[0])
               {
                case '\\':
                case ';':
                case '#':
                case '[':
                case ']':
                  /* reserved chars http://tools.ietf.org/html/rfc3986 */
                  return 0;
                case '%':
                  /* must be 3 char escape code + 8 char http version */
                  if (len < 11) return 0;
                  {
                     char code[3];
                     long codechar;

                     errno = 0;
                     code[0] = end[1];
                     code[1] = end[2];
                     code[2] = 0;
                     codechar = strtol(code, NULL, 16);
                     /* invalid escape code */
                     if (errno || (codechar < 32) || (codechar > 126)) return 0;
                     if (!uri)
                       {
                          uri = alloca(orig_len);
                          memcpy(uri, start, end - start);
                          p = uri + (end - start);
                       }
                     else
                       {
                          memcpy(p, start, end - start);
                          p += (end - start);
                       }
                     p[0] = (char)codechar;
                     p++;
                     if (codechar == '\\')
                       {
                          /* escape backslash */
                          p[0] = '\\';
                          p++;
                       }
                     /* new start */
                     len -= 3; start = end;
                     goto out;
                  }
                case '\r':
                case '\n':
                  {
                     unsigned int eo = end - header;
                     if (end - header < 14) return 0;
                     end--;
                     switch (end[0])
                       {
                        case '0':
                          break;
                        case '1':
                          net->http.version = 1;
                          break;
                        default:
                          return 0;
                       }
                     end -= 8;
                     if (strncmp((char*)end, " HTTP/1.", sizeof(" HTTP/1.") - 1)) return 0;
                     for (; (len < orig_len) && (end[0] == ' '); end--, len++);
                     if ((end - path_start < 0) || (end[0] == ' ')) return 0;
                     end++; /* copy up to the space */
                     if (uri)
                       {
                          memcpy(p, start, end - start);
                          net->http.req.http_path = eina_stringshare_add_length(uri, (p - uri) + (end - start));
                       }
                     else
                       net->http.req.http_path = eina_stringshare_add_length((char*)start, end - start);
                     INFO("Requested URI: '%s'", net->http.req.http_path);
                     return (int)eo;
                  }
                default:
                  break;
               }
             if (len > 1) end++;
          }
out:
        continue;
     }
   return 0;
}

static Eina_Bool
_azy_events_valid_header_value(const char  *name,
                               unsigned int len)
{
   while (len--)
     {
        if ((!isprint(*name)) && (!isspace(*name)))
          return EINA_FALSE;

        name++;
     }

   return EINA_TRUE;
}

int
azy_events_type_parse(Azy_Net             *net,
                      int                  type,
                      const unsigned char *header,
                      int                  len)
{
   const unsigned char *start = NULL;
   int size;

   DBG("(net=%p, header=%p, len=%i)", net, header, len);
   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return 0;
     }

   if (net->size && net->buffer)
     {
        unsigned char *buf_start;

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

   if (!start) return 0;

   /* some clients are dumb and send leading cr/nl/etc */
   AZY_SKIP_BLANK(start);

   if (type == ECORE_CON_EVENT_CLIENT_DATA)
     return _azy_events_valid_request(net, start, len);
   return _azy_events_valid_response(net, start, len);
}

Eina_Bool
azy_events_header_parse(Azy_Net       *net,
                        unsigned char *event_data,
                        size_t         event_len,
                        int            offset)
{
   unsigned char *r = NULL, *p = NULL, *start = NULL, *buf_start = NULL;
   unsigned char *data = (event_data) ? event_data + offset : NULL;
   int64_t len = (event_len) ? event_len - offset : 0;
   const char *s = NULL;
   unsigned char slen = 0;
   unsigned char sep[5];
   int line_len = 0;
   int64_t prev_size = 0;

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
        if (event_data && (azy_rpc_log_dom >= 0))
          {
             char buf[64];
             snprintf(buf, sizeof(buf), "STORED:\n<<<<<<<<<<<<<\n%%.%llis\n<<<<<<<<<<<<<", net->size);
             RPC_INFO(buf, net->buffer);
             snprintf(buf, sizeof(buf), "RECEIVED:\n<<<<<<<<<<<<<\n%%.%llis\n<<<<<<<<<<<<<", len - offset);
             RPC_INFO(buf, data);
          }
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

   if ((!len) && (event_len - offset > 0)) /* only blanks were passed, assume http separator */
     {
        net->headers_read = EINA_TRUE;
        return EINA_TRUE;
     }
   /* apparently this can happen? */
   EINA_SAFETY_ON_NULL_RETURN_VAL(start, EINA_FALSE);
   /* find a header or append to buffer */
   if ((!(r = memchr(start, '\r', len)) && !(r = memchr(start, '\n', len)))) /* append to a buffer and use net->overflow */
     {
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
               { /* we currently have \n\r: b64 encoding can leave a trailing \n
                  * so we have to check for an extra \n
                  */
                   if ((x - r < 0) && ((unsigned int)(r + 1 - start) < len) && (r[1] == '\n')) /* \n\r\n */
                     {
                        if (((unsigned int)(r + 2 - start) < len) && (r[2] == '\r')) /* \n\r\n\r */
                          {
                             if (((unsigned int)(r + 3 - start) < len) && (r[3] == '\n'))
                               /* \n\r\n\r\n oh hey I'm gonna stop here before it gets too insane */
                               s = "\r\n";
                             else
                               s = "\n\r";
                          }
                        else
                          s = "\r\n";
                     }
                   else
                     s = "\n\r";
               }
          }
        else
          s = "\r";
     }
   else
     s = "\n";

   slen = strlen(s);
   snprintf((char *)sep, sizeof(sep), "%s%s", s, s);

   p = start;
   line_len = r - p;
   while (len && r)
     {
        unsigned char *ptr, *semi = p;

        if (line_len > MAX_HEADER_SIZE)
          {
             WARN("Ignoring unreasonably large header starting with:\n %.32s\n", p);
             goto skip_header;
          }
        semi += (line_len - _azy_events_valid_header_name((const char *)p, line_len));
        if (semi == p) goto skip_header;

        ptr = semi + 1;
        while ((isspace(*ptr)) && (ptr - p < line_len))
          ptr++;

        if (_azy_events_valid_header_value((const char *)ptr, line_len - (ptr - p)))
          {
             const char *key, *value;

             p[semi - p] = 0;
             ptr[line_len - (ptr - p)] = 0;
             key = (const char *)p;
             value = (const char *)ptr;
             INFO("Found header: key='%s'", key);
             INFO("Found header: value='%s'", value);
             azy_net_header_set(net, key, value);
             if (!strcasecmp(key, "content-length"))
               net->http.content_length = strtol((const char *)value, NULL, 10);
          }

skip_header:
        len -= line_len + slen;
        if (len < slen)
          break;
        p = r + slen;
        /* double separator: STOP */
        if (!strncmp((char*)p, s, slen))
          {
             net->headers_read = EINA_TRUE;
             break;
          }
        r = azy_memstr(p, (const unsigned char *)s, len, slen);
        line_len = r - p;
        /* FIXME: to be fully 1.1 compliant, lines without a colon
         * be filtered and checked to see if is a continuing header
         * from the previous line
         */
     }

   AZY_SKIP_BLANK(p);

   if (!net->headers_read)
     return EINA_TRUE;

   if (!net->http.content_length) net->http.content_length = -1;
   if (len)
     {
        int64_t rlen;
        /* if we get here, we need to append to the buffers */

        if (net->http.content_length > 0)
          {
             if (len > net->http.content_length)
               {
                  rlen = net->http.content_length;
                  net->overflow_length = len - rlen;
                  WARN("Extra content length of %lli!", net->overflow_length);
                  net->overflow = malloc(net->overflow_length);
     /* FIXME: uhhhh fuck? */
                  EINA_SAFETY_ON_NULL_RETURN_VAL(net->overflow, EINA_FALSE);
                  memcpy(net->overflow, p + rlen, net->overflow_length);
#ifdef ISCOMFITOR
                if (azy_rpc_log_dom >= 0)
                  {
                     int64_t x;
                     RPC_INFO("OVERFLOW:\n<<<<<<<<<<<<<");
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

        INFO("Set recv size to %lli (previous %lli)", rlen, prev_size);
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
   const char *c = NULL;;
   DBG("(content_type='%s')", content_type);
   if (!content_type)
     return AZY_NET_TRANSPORT_TEXT;

   if (!strncmp(content_type, "text/", 5))
     c = content_type + 5;
   else if (!strncmp(content_type, "application/", 12))
     c = content_type + 12;

   if (!c) return AZY_NET_TRANSPORT_UNKNOWN;

   if (!strncmp(c, "xml", 3))
     return AZY_NET_TRANSPORT_XML;

   if (!strncmp(c, "json", 4))
     return AZY_NET_TRANSPORT_JSON;

   if (!strncmp(c, "eet", 3))
     return AZY_NET_TRANSPORT_EET;

   if (!strncmp(c, "plain", 5))
     return AZY_NET_TRANSPORT_TEXT;

   if (!strncmp(c, "html", 4))
     return AZY_NET_TRANSPORT_HTML;

   if (!strncmp(c, "atom+xml", 8))
     return AZY_NET_TRANSPORT_ATOM;

   return AZY_NET_TRANSPORT_UNKNOWN;
}

Eina_Bool
azy_events_connection_kill(void       *conn,
                           Eina_Bool   server_client,
                           const char *msg)
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
_azy_event_handler_fake_free(void *data  __UNUSED__,
                             void *data2 __UNUSED__)
{}

void
azy_fake_free(void *data __UNUSED__)
{}

