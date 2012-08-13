#include "shotgun_private.h"
#include "xml.h"

// http://www.ietf.org/rfc/rfc4616.txt

static char *
sasl_init(Shotgun_Auth *auth, size_t *size)
{
   Eina_Binbuf *buf;
   char *ret;

   buf = eina_binbuf_new();

   eina_binbuf_append_char(buf, 0);
   eina_binbuf_append_length(buf, (unsigned char*)auth->user, eina_stringshare_strlen(auth->user));
   eina_binbuf_append_char(buf, 0);
   eina_binbuf_append_length(buf, (unsigned char*)auth->pass, strlen(auth->pass));
   ret = shotgun_base64_encode(eina_binbuf_string_get(buf), eina_binbuf_length_get(buf), size);
   eina_binbuf_free(buf);
   return ret;
}

static void
sasl_digestmd5_init(Shotgun_Auth *auth)
{
   char buf[1024], cnonce[128], *b64, *sasl, *user, *pass;
   const char *realm, *nonce;
   unsigned char digest[16];
   char md5buf[33];
   char ha1[33];
   size_t size;

   realm = eina_hash_find(auth->features.auth_digestmd5, "realm");
   if (!realm) realm = auth->from;
   nonce = eina_hash_find(auth->features.auth_digestmd5, "nonce");
   if (!nonce) return;
   //DBG("DIGEST-MD5: NONCE %s", nonce);
   user = eina_str_convert("iso-8859-1", "utf-8", auth->user);
   pass = eina_str_convert("iso-8859-1", "utf-8", auth->pass);
   /* X */
   snprintf(buf, sizeof(buf), "%s:%s:%s", user ?: auth->user, realm, pass ?: auth->pass);
   //DBG("DIGEST-MD5: X %s", buf);
   md5_buffer(buf, strlen(buf), digest);
   /* Y */
   memcpy(buf, digest, sizeof(digest));
   snprintf(cnonce, sizeof(cnonce), "%.16g", ecore_time_unix_get());
   shotgun_strtohex((void*)cnonce, 32, md5buf);
   memcpy(cnonce, md5buf, sizeof(md5buf));
   cnonce[32] = 0;
   //DBG("DIGEST-MD5: CNONCE %s", cnonce);
   /* A1 */
   snprintf(&buf[16], sizeof(buf) - 16, ":%s:%s", nonce, cnonce);
   //DBG("DIGEST-MD5: A1 %s", buf + 16);
   /* HA1 */
   md5_buffer(buf, strlen(&buf[16]) + 16, digest);
   shotgun_strtohex(digest, 32, md5buf);
   memcpy(ha1, md5buf, sizeof(md5buf));
   ha1[32] = 0;
   //DBG("DIGEST-MD5: HA1 %s", ha1);
   /* A2 */
   snprintf(buf, sizeof(buf), "AUTHENTICATE:xmpp/%s", realm);
   size = strlen(buf);
   //DBG("DIGEST-MD5: A2 %s", buf);
   /* HA2 */
   md5_buffer(buf, size + strlen(&buf[size]), digest);
   shotgun_strtohex(digest, 32, md5buf);
   //DBG("DIGEST-MD5: HA2 %s", md5buf);
   /* KD */
   snprintf(buf, sizeof(buf), "%s:%s:00000001:%s:auth:%s", ha1, nonce, cnonce, md5buf);
   //DBG("DIGEST-MD5: KD %s", buf);
   md5_buffer(buf, strlen(buf), digest);
   shotgun_strtohex(digest, 32, md5buf);
   //DBG("DIGEST-MD5: HKD %s", md5buf);
   /* Z */
   snprintf(buf, sizeof(buf), "username=\"%s\",realm=\"%s\",nonce=\"%s\",cnonce=\"%s\",nc=00000001,qop=auth,digest-uri=\"xmpp/%s\",response=%s,charset=utf-8",
            auth->user, realm, nonce, cnonce, realm, md5buf);
   //DBG("DIGEST-MD5: Z %s", buf);
   b64 = shotgun_base64_encode((void*)buf, strlen(buf), &size);
   sasl = xml_sasl_digestmd5_write(b64, &size);
   shotgun_write(auth->svr, sasl, size);
   free(user);
   free(pass);
   free(b64);
   free(sasl);
}

static void
shotgun_stream_init(Shotgun_Auth *auth)
{
   size_t len;
   char *xml;

   xml = xml_stream_init_create(auth, "en", &len);
   shotgun_write(auth->svr, xml, len);
   free(xml);
}

static Eina_Bool
shotgun_login_bind(Shotgun_Auth *auth, Ecore_Con_Event_Server_Data *ev)
{
   char *out;
   size_t len;

   out = xml_iq_write_preset(auth, SHOTGUN_IQ_PRESET_BIND, &len);
   if (!out) return EINA_FALSE;

   shotgun_write(ev->server, out, len);
   free(out);
   auth->state = SHOTGUN_CONNECTION_STATE_SESSION;
   if (!auth->features.session) auth->state = SHOTGUN_CONNECTION_STATE_FINALIZING;
   ecore_event_add(SHOTGUN_EVENT_CONNECTION_STATE, auth, shotgun_fake_free, NULL);
   return EINA_TRUE;
}

static Eina_Bool
shotgun_login_session(Shotgun_Auth *auth, Ecore_Con_Event_Server_Data *ev)
{
   char *out;
   size_t len;

   out = xml_iq_write_preset(auth, SHOTGUN_IQ_PRESET_SESSION, &len);
   if (!out) return EINA_FALSE;

   shotgun_write(ev->server, out, len);
   free(out);
   auth->state = SHOTGUN_CONNECTION_STATE_FINALIZING;
   ecore_event_add(SHOTGUN_EVENT_CONNECTION_STATE, auth, shotgun_fake_free, NULL);
   return EINA_TRUE;
}

static Eina_Bool
shotgun_login_bind_check(Shotgun_Auth *auth)
{
   if (!auth->features.bind) return EINA_TRUE;

   EINA_SAFETY_ON_NULL_RETURN_VAL(auth->bind, EINA_FALSE);
   INF("Bind: %s", auth->bind);
   return EINA_TRUE;
}

static void
shotgun_login_complete(Shotgun_Auth *auth)
{
    INF("Login complete!");
    auth->state = SHOTGUN_CONNECTION_STATE_CONNECTED;
    ecore_event_add(SHOTGUN_EVENT_CONNECT, auth, shotgun_fake_free, NULL);
}

Eina_Bool
shotgun_login_con(Shotgun_Auth *auth, int type, Ecore_Con_Event_Server_Add *ev)
{
   if ((auth != ecore_con_server_data_get(ev->server)) || (!auth))
     return ECORE_CALLBACK_PASS_ON;

   if (type == ECORE_CON_EVENT_SERVER_ADD)
     INF("Connected!");
   else
     {
        INF("STARTTLS succeeded!");
        auth->state = SHOTGUN_CONNECTION_STATE_FEATURES;
        ecore_event_add(SHOTGUN_EVENT_CONNECTION_STATE, auth, shotgun_fake_free, NULL);
     }
   auth->svr = ev->server;
   shotgun_stream_init(auth);
   return ECORE_CALLBACK_RENEW;
}

void
shotgun_login(Shotgun_Auth *auth, Ecore_Con_Event_Server_Data *ev)
{
   char *out;
   char *data;
   size_t len, size;
   int ret;

   data = auth->buf ? (char*)eina_strbuf_string_get(auth->buf) : (char*)ev->data;
   size = auth->buf ? eina_strbuf_length_get(auth->buf) : (size_t)ev->size;

   if ((size > sizeof("<stream:error")) &&
       (!memcmp(data, "<stream:error", sizeof("<stream:error") - 1)))
     {
        ERR("Error in login!");
        shotgun_disconnect(auth);
        return;
     }

   switch (auth->state)
     {
      case SHOTGUN_CONNECTION_STATE_NONE:
        if (!xml_stream_init_read(auth, data, size)) break;

        if (auth->features.starttls)
          {
             auth->state = SHOTGUN_CONNECTION_STATE_TLS;
             shotgun_write(ev->server, XML_STARTTLS, sizeof(XML_STARTTLS) - 1);
             break;
          }
        /* who cares */
        ERR("STARTTLS support not detected in stream!");
        shotgun_disconnect(auth);
        break;
      case SHOTGUN_CONNECTION_STATE_TLS:
        if (xml_starttls_read(data, size))
          {
             if (auth->ssl_verify)
               {
                       Eina_Log_Level level;
                       level = eina_log_domain_level_get("ecore_con");
#ifndef SUPER_LOUD_CERT_LOADING
                       eina_log_domain_level_set("ecore_con", EINA_LOG_LEVEL_UNKNOWN);
#endif
                       ecore_con_ssl_server_cafile_add(ev->server, "/etc/ssl/certs");
                       eina_log_domain_level_set("ecore_con", level);
                       ecore_con_ssl_server_verify_basic(ev->server);
                       ecore_con_ssl_server_verify_name_set(ev->server, auth->from);
               }
             ecore_con_ssl_server_upgrade(ev->server, ECORE_CON_USE_MIXED | auth->ssl_verify);
          }
        else
          shotgun_disconnect(auth);
        return;

      case SHOTGUN_CONNECTION_STATE_FEATURES:
        if (!xml_stream_init_read(auth, data, size)) break;
        out = sasl_init(auth, &len);
        if (!out) shotgun_disconnect(auth);
        else
          {
             char *send;
             send = xml_sasl_write(auth, out, &len);
#ifdef SHOTGUN_AUTH_VISIBLE
             shotgun_write(ev->server, send, len);
#else
             ecore_con_server_send(ev->server, send, len);
#endif
             free(out);
             free(send);
             auth->state = SHOTGUN_CONNECTION_STATE_SASL;
             ecore_event_add(SHOTGUN_EVENT_CONNECTION_STATE, auth, shotgun_fake_free, NULL);
          }
        break;
      case SHOTGUN_CONNECTION_STATE_SASL:
        ret = xml_sasl_read(auth, data, size);
        if (!ret)
          {
             ERR("Login failed!");
             shotgun_disconnect(auth);
             break;
          }
        if ((ret == 2) && auth->features.sasl_digestmd5)
          {
             if (auth->features.auth_digestmd5)
               sasl_digestmd5_init(auth);
             else
               {
                  out = xml_sasl_digestmd5_write(NULL, &len);
                  EINA_SAFETY_ON_NULL_GOTO(out, error);
                  shotgun_write(ev->server, out, len);
                  free(out);
               }
             break;
          }
        /* yes, another stream. */
        shotgun_stream_init(auth);
        auth->state++;
        ecore_event_add(SHOTGUN_EVENT_CONNECTION_STATE, auth, shotgun_fake_free, NULL);
        break;
      case SHOTGUN_CONNECTION_STATE_BIND:
        if (!xml_stream_init_read(auth, data, size))
          break;
        if (auth->features.bind)
          {
             if (!shotgun_login_bind(auth, ev)) goto error;
             break;
          }
        data = NULL;
        /* intentional lack of break */
      case SHOTGUN_CONNECTION_STATE_SESSION:
        if (data)
          {
             xml_iq_read(auth, data, size);
             if (!shotgun_login_bind_check(auth)) goto error;
          }
        if (auth->features.session)
          {
             if (!shotgun_login_session(auth, ev)) goto error;
             break;
          }
        data = NULL;
      case SHOTGUN_CONNECTION_STATE_FINALIZING:
        if (data)
          {
             xml_iq_read(auth, data, size);
             if (auth->error) goto error;
          }
        shotgun_login_complete(auth);
      default:
        break;
     }
   if (auth->buf) eina_strbuf_free(auth->buf);
   auth->buf = NULL;
   return;
error:
   ERR("wtf");
   shotgun_disconnect(auth);
}
