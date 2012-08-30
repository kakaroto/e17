#include "shotgun_private.h"
#include <ctype.h>
#include "xml.h"

int shotgun_log_dom = -1;

int SHOTGUN_EVENT_CONNECT = 0;
int SHOTGUN_EVENT_CONNECTION_STATE = 0;
int SHOTGUN_EVENT_DISCONNECT = 0;
int SHOTGUN_EVENT_MESSAGE = 0;
int SHOTGUN_EVENT_PRESENCE = 0;
int SHOTGUN_EVENT_IQ = 0;

static Eina_Bool
ev_write(Shotgun_Auth *auth, int type __UNUSED__, Ecore_Con_Event_Server_Write *ev)
{
   if ((auth != ecore_con_server_data_get(ev->server)) || (!auth))
     return ECORE_CALLBACK_PASS_ON;
   ecore_timer_reset(auth->keepalive);
   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
keepalive(Shotgun_Auth *auth)
{
   ecore_con_server_send(auth->svr, " ", 1);
   return EINA_TRUE;
}

static Eina_Bool
ping_timeout(Shotgun_Auth *auth)
{
   /**
    * We haven't received ping response during the auth->ping_timeout delay
    * Current connection have to die.
    */
   ERR("Ping timeout, count : %d", auth->pending_ping);
   ecore_timer_freeze(auth->et_ping_timeout);
   if (auth->pending_ping > auth->ping_max_attempts)
      shotgun_disconnect(auth);

   return EINA_TRUE;
}

static Eina_Bool
ping(Shotgun_Auth *auth)
{
   /**
    * Sends ping request to the server (iq tag)
    * (Re)Init the timeout timer, increase the pending pings counter
    */
   xml_iq_ping_write(auth);
   auth->pending_ping++;
   ecore_timer_reset(auth->et_ping_timeout);
   ecore_timer_thaw(auth->et_ping_timeout);

   return EINA_TRUE;
}

Eina_Bool 
shotgun_ping_received(Shotgun_Auth *auth)
{
   auth->pending_ping = 0;
   ecore_timer_freeze(auth->et_ping_timeout);

   return EINA_TRUE;
}

static Eina_Bool
disc(Shotgun_Auth *auth, int type __UNUSED__, Ecore_Con_Event_Server_Del *ev)
{
   if ((auth != ecore_con_server_data_get(ev->server)) || (!auth))
     return ECORE_CALLBACK_PASS_ON;

   INF("Disconnected");
   ecore_con_server_del(auth->svr);
   auth->svr = NULL;
   shotgun_disconnect(auth);
   return EINA_FALSE;
}

static Shotgun_Data_Type
shotgun_data_tokenize(Shotgun_Auth *auth, Ecore_Con_Event_Server_Data *ev)
{
   const char *data;
   data = auth->buf ? eina_strbuf_string_get(auth->buf) : ev->data;
   if (data[0] != '<') return SHOTGUN_DATA_TYPE_UNKNOWN;

   switch (data[1])
     {
      case 'm':
        return SHOTGUN_DATA_TYPE_MSG;
      case 'i':
        return SHOTGUN_DATA_TYPE_IQ;
      case 'p':
        return SHOTGUN_DATA_TYPE_PRES;
      default:
        break;
     }
   return SHOTGUN_DATA_TYPE_UNKNOWN;
}

static Eina_Bool
shotgun_data_detect(Shotgun_Auth *auth, Ecore_Con_Event_Server_Data *ev)
{
   size_t len = ev->size;
   const char *tag, *data = (char*)ev->data;
   char buf[24] = {0};

   if (((char*)ev->data)[ev->size - 1] != '>')
     {
        if (!auth->buf)
          {
             DBG("Creating event buffer");
             auth->buf = eina_strbuf_new();
          }
        if ((len >= 21) &&
                 (!memcmp(data, "<?xml version=", sizeof("<?xml version=") - 1)))
          {
             DBG("Received xml version tag");
             data += 21, len -= 21;
          }
        DBG("Appending %zu to buffer", len);
        //fprintf(stdout, "%*s\n", len, (char*)data);
        eina_strbuf_append_length(auth->buf, data, len);
        return EINA_FALSE;
     }

   if (auth->buf)
     {
        DBG("Appending %i to buffer", ev->size);
        //fprintf(stdout, "%*s\n", ev->size, (char*)ev->data);
        eina_strbuf_append_length(auth->buf, ev->data, ev->size);
     }


   data = auth->buf ? (char*)eina_strbuf_string_get(auth->buf) : (char*)ev->data;
   len = auth->buf ? eina_strbuf_length_get(auth->buf) : (size_t)ev->size;

   if ((len >= 21) &&
            (!memcmp(data, "<?xml version=", sizeof("<?xml version=") - 1)))
     {
        DBG("Received xml version tag");
        data += 21, len -= 21;
     }

   tag = data + 1, len--;;
   while ((tag[0] != '>') && (tag[0] != ' '))
     {
        //DBG("\ndata: '%s'\ntag: '%s'", data, tag);
        tag++, len--;
     }

   if (!memcmp(data, "<stream:stream", sizeof("<stream:stream") - 1)) return EINA_TRUE;
   if ((tag[len - 2] == '/') && (len >= 7) && memcmp(data, "<stream", 7)) return EINA_TRUE;
   //fprintf(stderr, "tag: %*s || end: %*s\n", tag - data - 1, data + 1, tag - data - 1, tag + len - (tag - data));
   if ((data != tag + len - (tag - data) - 1) && (!memcmp(data + 1, tag + len - (tag - data), tag - data - 1)))
     {
        if (eina_log_domain_level_check(shotgun_log_dom, EINA_LOG_LEVEL_DBG))
          {
             memcpy(buf, data + 1, sizeof(buf) - 1);
             DBG("'%s' and '%s' match!", buf, tag + len - (tag - data));
             DBG("Releasing buffered event!");
          }
        return EINA_TRUE;
     }

   memcpy(buf, data + 1, sizeof(buf) - 1);
   DBG("'%s' and '%s' do not match!", buf, tag + len - (tag - data));
   if (!auth->buf)
     {
        DBG("Creating event buffer");
        auth->buf = eina_strbuf_new();
        DBG("Appending %i to buffer", ev->size);
        eina_strbuf_append_length(auth->buf, ev->data, ev->size);
     }
   return EINA_FALSE;
}

static Eina_Bool
data(Shotgun_Auth *auth, int type __UNUSED__, Ecore_Con_Event_Server_Data *ev)
{
   char *recv, *data, *p;
   size_t size;

   if ((auth != ecore_con_server_data_get(ev->server)) || (!auth))
     return ECORE_CALLBACK_PASS_ON;

   ecore_timer_reset(auth->keepalive);
   if (ev->size == 1)
     {
        DBG("Received carriage return");
        return ECORE_CALLBACK_RENEW;
     }
   else if ((ev->size == 38) &&
            (!memcmp(ev->data, "<?xml version=\"1.0", sizeof("<?xml version=\"1.0") - 1)) &&
            (((unsigned char*)ev->data)[ev->size - 1] == '>'))
     {
        DBG("Received xml version tag");
        return ECORE_CALLBACK_RENEW;
     }
   if (eina_log_domain_level_check(shotgun_log_dom, EINA_LOG_LEVEL_DBG))
     {
        recv = alloca(ev->size + 1);
        memcpy(recv, ev->data, ev->size);
        for (p = recv + ev->size - 1; isspace(*p); p--)
          *p = 0;
        recv[ev->size] = 0;
        DBG("Receiving %i bytes:\n%s", ev->size, recv);
     }

   if (!shotgun_data_detect(auth, ev))
     return ECORE_CALLBACK_RENEW;

   if (auth->state < SHOTGUN_CONNECTION_STATE_CONNECTED)
     {
        shotgun_login(auth, ev);
        return ECORE_CALLBACK_RENEW;
     }

   data = auth->buf ? (char*)eina_strbuf_string_get(auth->buf) : (char*)ev->data;
   size = auth->buf ? eina_strbuf_length_get(auth->buf) : (size_t)ev->size;

   switch (shotgun_data_tokenize(auth, ev))
     {
      case SHOTGUN_DATA_TYPE_MSG:
        shotgun_message_feed(auth, data, size);
        break;
      case SHOTGUN_DATA_TYPE_IQ:
        shotgun_iq_feed(auth, data, size);
        break;
      case SHOTGUN_DATA_TYPE_PRES:
        shotgun_presence_feed(auth, data, size);
        break;
      default:
        ERR("UNPARSABLE TAG %d", shotgun_data_tokenize(auth, ev));
        break;
     }
   if (auth->buf) eina_strbuf_free(auth->buf);
   auth->buf = NULL;

   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
error(Shotgun_Auth *auth, int type __UNUSED__, Ecore_Con_Event_Server_Error *ev)
{
   if ((auth != ecore_con_server_data_get(ev->server)) || (!auth))
     return ECORE_CALLBACK_PASS_ON;
   ERR("%s", ev->error);
   shotgun_disconnect(auth);
   return ECORE_CALLBACK_RENEW;
}

int
shotgun_init(void)
{
   eina_init();
   ecore_init();
   ecore_con_init();

   /* real men don't accept failure as a possibility */
   shotgun_log_dom = eina_log_domain_register("shotgun", EINA_COLOR_RED);

   SHOTGUN_EVENT_CONNECT = ecore_event_type_new();
   SHOTGUN_EVENT_CONNECTION_STATE = ecore_event_type_new();
   SHOTGUN_EVENT_DISCONNECT = ecore_event_type_new();
   SHOTGUN_EVENT_MESSAGE = ecore_event_type_new();
   SHOTGUN_EVENT_PRESENCE = ecore_event_type_new();
   SHOTGUN_EVENT_IQ = ecore_event_type_new();

   return 1;
}

Eina_Bool
shotgun_connect(Shotgun_Auth *auth)
{
   if ((!auth->user) || (!auth->from) || (!auth->svr_name)) return EINA_FALSE;
   if (auth->changed)
     {
        auth->jid = eina_stringshare_printf("%s@%s/%s", auth->user, auth->from, auth->resource);
        auth->base_jid = eina_stringshare_printf("%s@%s", auth->user, auth->from);
        auth->changed = EINA_FALSE;
     }
   else if (!auth->jid) auth->jid = eina_stringshare_printf("%s@%s/%s", auth->user, auth->from, auth->resource);
   else if (!auth->base_jid) auth->base_jid = eina_stringshare_printf("%s@%s", auth->user, auth->from);
   auth->ev_add = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD, (Ecore_Event_Handler_Cb)shotgun_login_con, auth);
   auth->ev_del = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL, (Ecore_Event_Handler_Cb)disc, auth);
   auth->ev_data = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA, (Ecore_Event_Handler_Cb)data, auth);
   auth->ev_error = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ERROR, (Ecore_Event_Handler_Cb)error, auth);
   auth->ev_upgrade = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_UPGRADE, (Ecore_Event_Handler_Cb)shotgun_login_con, auth);
   auth->ev_write = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_WRITE, (Ecore_Event_Handler_Cb)ev_write, auth);
   auth->svr = ecore_con_server_connect(ECORE_CON_REMOTE_NODELAY, auth->svr_name, 5222, auth);
   if (auth->svr) auth->keepalive = ecore_timer_add(300, (Ecore_Task_Cb)keepalive, auth);

   return !!auth->svr;
}

void
shotgun_disconnect(Shotgun_Auth *auth)
{
   if (!auth) return;
   if (auth->svr_name)
     ecore_event_add(SHOTGUN_EVENT_DISCONNECT, auth, shotgun_fake_free, NULL);
   if (auth->ev_add) ecore_event_handler_del(auth->ev_add);
   if (auth->ev_del) ecore_event_handler_del(auth->ev_del);
   if (auth->ev_data) ecore_event_handler_del(auth->ev_data);
   if (auth->ev_error) ecore_event_handler_del(auth->ev_error);
   if (auth->ev_upgrade) ecore_event_handler_del(auth->ev_upgrade);
   if (auth->ev_write) ecore_event_handler_del(auth->ev_write);
   if (auth->svr) ecore_con_server_del(auth->svr);
   if (auth->keepalive) ecore_timer_del(auth->keepalive);
   if (auth->et_ping) ecore_timer_del(auth->et_ping);
   if (auth->et_ping_timeout) ecore_timer_del(auth->et_ping_timeout);
   auth->keepalive = NULL;
   auth->ev_add = NULL;
   auth->ev_del = NULL;
   auth->ev_data = NULL;
   auth->ev_error = NULL;
   auth->ev_upgrade = NULL;
   auth->ev_write = NULL;
   auth->svr = NULL;
   auth->state = 0;
   memset(&auth->features, 0, sizeof(auth->features));
   auth->pending_ping = 0;
}

void
shotgun_free(Shotgun_Auth *auth)
{
   if (!auth) return;
   eina_stringshare_del(auth->user);
   eina_stringshare_del(auth->from);
   eina_stringshare_del(auth->resource);
   eina_stringshare_del(auth->jid);
   eina_stringshare_del(auth->svr_name);
   eina_stringshare_del(auth->bind);
   eina_stringshare_del(auth->desc);
   eina_stringshare_del(auth->error);
   eina_stringshare_del(auth->pass);
   auth->svr_name = NULL;
   shotgun_disconnect(auth);
   free(auth->settings);
   if (auth->buf) eina_strbuf_free(auth->buf);
   if (auth->vcard) shotgun_user_info_free(auth->vcard);
   free(auth);
}

Shotgun_Auth *
shotgun_new(const char *svr_name, const char *username, const char *domain)
{
   Shotgun_Auth *auth;

   auth = calloc(1, sizeof(Shotgun_Auth));
   if (username) auth->user = eina_stringshare_add(username);
   if (domain) auth->from = eina_stringshare_add(domain);
   auth->resource = eina_stringshare_add("SHOTGUN!");
   if (username && domain)
     { 
        auth->jid = eina_stringshare_printf("%s@%s/%s", auth->user, auth->from, auth->resource);
        auth->base_jid = eina_stringshare_printf("%s@%s", auth->user, auth->from);
     }
   if (svr_name) auth->svr_name = eina_stringshare_add(svr_name);
   auth->pending_ping = 0;
   auth->ping_max_attempts = 5;
   shotgun_ping_delay_set(auth, 60);
   shotgun_ping_timeout_set(auth, 30);

   return auth;
}

void
shotgun_ssl_verify_set(Shotgun_Auth *auth, Eina_Bool verify)
{
   EINA_SAFETY_ON_NULL_RETURN(auth);
   auth->ssl_verify = ECORE_CON_LOAD_CERT * (!!verify);
}

Eina_Bool
shotgun_ssl_verify_get(Shotgun_Auth *auth)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, EINA_FALSE);
   return auth->ssl_verify;
}

Shotgun_Connection_State
shotgun_connection_state_get(Shotgun_Auth *auth)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, 0);
   return auth->state;
}

void
shotgun_username_set(Shotgun_Auth *auth, const char *username)
{
   eina_stringshare_replace(&auth->user, username);
   auth->changed = EINA_TRUE;
}

const char *
shotgun_username_get(Shotgun_Auth *auth)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, NULL);

   return auth->user;
}

const char *
shotgun_resource_get(Shotgun_Auth *auth)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, NULL);

   return auth->resource;
}

void
shotgun_resource_set(Shotgun_Auth *auth, const char *resource)
{
   eina_stringshare_replace(&auth->resource, resource);
   auth->changed = EINA_TRUE;
}

const char *
shotgun_password_get(Shotgun_Auth *auth)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, NULL);

   return auth->pass;
}

void
shotgun_domain_set(Shotgun_Auth *auth, const char *domain)
{
   eina_stringshare_replace(&auth->from, domain);
   auth->changed = EINA_TRUE;
}

const char *
shotgun_domain_get(Shotgun_Auth *auth)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, NULL);

   return auth->from;
}

void
shotgun_servername_set(Shotgun_Auth *auth, const char *svr_name)
{
   eina_stringshare_replace(&auth->svr_name, svr_name);
}

const char *
shotgun_servername_get(Shotgun_Auth *auth)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, NULL);

   return auth->svr_name;
}

const char *
shotgun_jid_get(Shotgun_Auth *auth)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, NULL);
   return auth->base_jid;
}

const char *
shotgun_jid_full_get(Shotgun_Auth *auth)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, NULL);
   return auth->jid;
}

void
shotgun_data_set(Shotgun_Auth *auth, void *data)
{
   EINA_SAFETY_ON_NULL_RETURN(auth);
   auth->data = data;
}

void *
shotgun_data_get(Shotgun_Auth *auth)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, NULL);
   return auth->data;
}

void
shotgun_settings_set(Shotgun_Auth *auth, void *settings)
{
   EINA_SAFETY_ON_NULL_RETURN(auth);
   auth->settings = settings;
}

void *
shotgun_settings_get(Shotgun_Auth *auth)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, NULL);
   return auth->settings;
}

void
shotgun_password_set(Shotgun_Auth *auth, const char *password)
{
   EINA_SAFETY_ON_NULL_RETURN(auth);
   eina_stringshare_replace(&auth->pass, password);
}

void *
shotgun_vcard_get(Shotgun_Auth *auth)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(auth, NULL);
   return auth->vcard;
}

void
shotgun_vcard_set(Shotgun_Auth *auth, void *data)
{
   EINA_SAFETY_ON_NULL_RETURN(auth);
   auth->vcard = data;
}

void
shotgun_ping_start(Shotgun_Auth *auth)
{
   auth->et_ping = ecore_timer_add(auth->ping_delay, (Ecore_Task_Cb)ping, auth);
   auth->et_ping_timeout = ecore_timer_add(auth->ping_timeout, (Ecore_Task_Cb)ping_timeout, auth);
   ecore_timer_freeze(auth->et_ping_timeout);
}

void
shotgun_ping_delay_set(Shotgun_Auth *auth, double delay)
{
   EINA_SAFETY_ON_NULL_RETURN(auth);
   auth->ping_delay = delay;
   if (auth->et_ping) 
      ecore_timer_interval_set(auth->et_ping, auth->ping_delay);
}

void
shotgun_ping_timeout_set(Shotgun_Auth *auth, double timeout)
{
   EINA_SAFETY_ON_NULL_RETURN(auth);
   auth->ping_timeout = timeout;
   if (auth->et_ping_timeout) 
      ecore_timer_interval_set(auth->et_ping_timeout, auth->ping_timeout);
}

void
shotgun_ping_max_attempts_set(Shotgun_Auth *auth, unsigned int max)
{
   EINA_SAFETY_ON_NULL_RETURN(auth);
   auth->ping_max_attempts = max;
}
