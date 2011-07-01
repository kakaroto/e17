#include "email_private.h"

// http://www.ietf.org/rfc/rfc4616.txt

static char *
sasl_plain_init(Email *e)
{
   Eina_Binbuf *buf;
   char *ret;
   int size;

   buf = eina_binbuf_new();

   eina_binbuf_append_char(buf, 0);
   eina_binbuf_append_length(buf, (unsigned char*)e->username, eina_stringshare_strlen(e->username));
   eina_binbuf_append_char(buf, 0);
   eina_binbuf_append_length(buf, (unsigned char*)e->password, strlen(e->password));
   ret = email_base64_encode((char*)eina_binbuf_string_get(buf), eina_binbuf_length_get(buf), &size);
   eina_binbuf_free(buf);
   return ret;
}

static void
features_detect_smtp(Email *e, const unsigned char *data, int size)
{
   const unsigned char *p;

   for (p = memchr(data + 4, '\n', size - 4);
        p - data < size - 1;
        p = memchr(p + 1, '\n', size - (p - data)))
     {
        if (p[1] != '2') return;
        p += 5;
        if (!strncasecmp((char*)p, "AUTH", 4))
          {
             const unsigned char *n;
             
             p += 5;
             n = memchr(p, '\n', size - (p - data));
             do
               {
                  if (!strncasecmp((char*)p, "CRAM-MD5", 8))
                    {
                       e->features.smtp_features.cram = EINA_TRUE;
                       INF("Detected CRAM-MD5");
                    }
                  else if (!strncasecmp((char*)p, "DIGEST-MD5", 10))
                    {
                       e->features.smtp_features.digest = EINA_TRUE;
                       INF("Detected DIGEST-MD5");
                    }
                  else if (!strncasecmp((char*)p, "LOGIN", 5))
                    {
                       e->features.smtp_features.login = EINA_TRUE;
                       INF("Detected LOGIN");
                    }
                  else if (!strncasecmp((char*)p, "PLAIN", 5))
                    {
                       e->features.smtp_features.plain = EINA_TRUE;
                       INF("Detected PLAIN");
                    }
                  p = memchr(p + 1, ' ', n - p);
               } while (p++);
             p = n;
          }
        else if (!strncasecmp((char*)p, "PIPELINING", 10))
          e->features.smtp_features.pipelining = EINA_TRUE;
        else if (!strncasecmp((char*)p, "SIZE", 4))
          {
             char *x;
             e->features.smtp_features.size = strtoull((char*)p + 5, &x, 10);
             INF("Detected max size: %zu", e->features.smtp_features.size);
             p = (unsigned char*)x;
          }
     }
}

void
email_login_smtp(Email *e, Ecore_Con_Event_Server_Data *ev)
{
   const unsigned char *p;
   char *buf;
   size_t size;

   switch (e->state)
     {
      case EMAIL_STATE_SSL:
        /* 250 STARTTLS\r\n */
        if ((ev->size < 14) || (((char*)ev->data)[0] != '2'))
          {
             ERR("Could not create secure connection!");
             ecore_con_server_del(ev->server);
             return;
          }
        if (((char*)ev->data)[1] == '5')
          {
             email_write(e, "STARTTLS\r\n", sizeof("STARTTLS\r\n") - 1);
             features_detect_smtp(e, ev->data, ev->size);
          }
        else
          {/* 220 Go ahead\r\n */
             ecore_con_ssl_server_upgrade(e->svr, ECORE_CON_USE_MIXED);
             ecore_con_ssl_server_verify_basic(e->svr);
             e->flags = ECORE_CON_USE_MIXED;
          }
        return;
      case EMAIL_STATE_INIT:
        if (ev->size < 10) goto error;
        if (!memcmp(ev->data, "421", 3))
          {
             INF("Server rejected connection");
             ecore_con_server_del(e->svr);
             return;
          }
        if (memcmp((char*)ev->data, "220", 3)) goto error;
        /* TODO: strncmp memchr(hostname, '.', len)+1 features.domain for MitM attacks? */
        /* 220 heaven.af.mil ESMTP\r\n */
        p = memchr(ev->data + 4, ' ', ev->size - 4);
        if (p && (p - (unsigned char*)ev->data + 7 < ev->size))
          {
             if (!strncasecmp((char*)p + 1, "ESMTP", 5))
               e->features.smtp_features.ssl = EINA_TRUE;
          }

        size = sizeof(char) * (sizeof("EHLO \r\n") + strlen(e->features.smtp_features.domain));

        buf = alloca(size);
        if (e->features.smtp_features.ssl)
          snprintf(buf, size, "EHLO %s\r\n", e->features.smtp_features.domain);
        else
          snprintf(buf, size, "HELO %s\r\n", e->features.smtp_features.domain);
        if (!e->flags) e->state++;
        else e->state = EMAIL_STATE_USER;
        email_write(e, buf, size - 1);
        return;
      case EMAIL_STATE_USER:
        if (ev->size < 3) goto error;
        if (!memcmp(ev->data, "250", 3))
          {
             features_detect_smtp(e, ev->data, ev->size);
             if (e->features.smtp_features.cram)
               {
                  DBG("Beginning AUTH CRAM-MD5");
                  email_write(e, "AUTH CRAM-MD5\r\n", sizeof("AUTH CRAM-MD5\r\n") - 1);
               }
             else if (e->features.smtp_features.plain)
               {
                  char *plain;

                  plain = sasl_plain_init(e);
                  DBG("Beginning AUTH PLAIN");

                  size = sizeof(char) * (sizeof("AUTH PLAIN \r\n") + strlen(plain));
                  buf = alloca(size);
                  snprintf(buf, size, "AUTH PLAIN %s\r\n", plain);
                  free(plain);
                  ecore_con_server_send(e->svr, buf, size - 1);
               }
             else if (e->features.smtp_features.login)
               {
                  INF("Beginning AUTH LOGIN");
                  email_write(e, "AUTH LOGIN\r\n", sizeof("AUTH LOGIN\r\n") - 1);
               }
          }
        else if (!memcmp(ev->data, "235", 3))
          {
             e->state = EMAIL_STATE_CONNECTED;
             INF("SMTP server connected");
             ecore_event_add(EMAIL_EVENT_CONNECTED, e, (Ecore_End_Cb)email_fake_free, NULL);
          }
        else if (!memcmp(ev->data, "334", 3))
          {
             if (e->features.smtp_features.cram)
               {
                  char *b64, md5sum[33];
                  unsigned char digest[16];
                  int bsize;
                  Eina_Strbuf *buf;

                  b64 = email_base64_decode(ev->data + 4, ev->size - 6, &bsize);
                  email_md5_hmac_encode(digest, b64, bsize, e->password, strlen(e->password));
                  free(b64);
                  email_md5_digest_to_str(digest, md5sum);
                  buf = eina_strbuf_new();
                  eina_strbuf_append_printf(buf, "%s %s", e->username, md5sum);
                  b64 = email_base64_encode(eina_strbuf_string_get(buf), eina_strbuf_length_get(buf), &bsize);
                  eina_strbuf_free(buf);
                  ecore_con_server_send(e->svr, b64, bsize);
                  ecore_con_server_send(e->svr, "\r\n", 2);
                  free(b64);
               }
             else if (e->features.smtp_features.login)
               {
                  /* continuation of AUTH LOGIN */
                  char *b64;
                  int bsize;

                  DBG("Continuing with AUTH LOGIN");
                  b64 = email_base64_encode(e->username, strlen(e->username), &bsize);
                  ecore_con_server_send(e->svr, b64, bsize);
                  ecore_con_server_send(e->svr, "\r\n", 2);
                  free(b64);
                  e->state++;
               }
             else goto error;
          }
          
        else goto error;
        break;
      case EMAIL_STATE_PASS:
        if (!memcmp(ev->data, "334", 3))
          {
             /* continuation of AUTH LOGIN */
             char *b64;
             int bsize;

             DBG("Continuing with AUTH LOGIN");
             b64 = email_base64_encode(e->password, strlen(e->password), &bsize);
             ecore_con_server_send(e->svr, b64, bsize);
             ecore_con_server_send(e->svr, "\r\n", 2);
             free(b64);
             e->state++;
          }
        else if (!memcmp(ev->data, "235", 3))
          {
             e->state = EMAIL_STATE_CONNECTED;
             INF("SMTP server connected");
             ecore_event_add(EMAIL_EVENT_CONNECTED, e, (Ecore_End_Cb)email_fake_free, NULL);
          }
      default:
        break;
     }
   return;
error:
   ERR("Not a valid SMTP server");
   ecore_con_server_del(e->svr);
}
