#ifndef SHOTGUN_PRIVATE_H
#define SHOTGUN_PRIVATE_H

#ifndef __UNUSED__
# define __UNUSED__ __attribute__((unused))
#endif

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifndef alloca
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#elif defined __GNUC__
#define alloca __builtin_alloca
#elif defined _AIX
#define alloca __alloca
#else
#include <stddef.h>
void *alloca (size_t);
#endif
#endif

#include <Ecore.h>
#include <Ecore_Con.h>
#include "Shotgun.h"
#include "md5.h"

#ifndef strdupa
# define strdupa(str)       strcpy(alloca(strlen(str) + 1), str)
#endif

#ifndef strndupa
# define strndupa(str, len) strncpy(alloca(len + 1), str, len)
#endif

#define DBG(...)            EINA_LOG_DOM_DBG(shotgun_log_dom, __VA_ARGS__)
#define INF(...)            EINA_LOG_DOM_INFO(shotgun_log_dom, __VA_ARGS__)
#define WRN(...)            EINA_LOG_DOM_WARN(shotgun_log_dom, __VA_ARGS__)
#define ERR(...)            EINA_LOG_DOM_ERR(shotgun_log_dom, __VA_ARGS__)
#define CRI(...)            EINA_LOG_DOM_CRIT(shotgun_log_dom, __VA_ARGS__)

typedef enum
{
   SHOTGUN_DATA_TYPE_UNKNOWN,
   SHOTGUN_DATA_TYPE_MSG,
   SHOTGUN_DATA_TYPE_IQ,
   SHOTGUN_DATA_TYPE_PRES,
   SHOTGUN_DATA_TYPE_FILE
} Shotgun_Data_Type;

typedef enum
{
   SHOTGUN_IQ_TYPE_GET,
   SHOTGUN_IQ_TYPE_SET,
   SHOTGUN_IQ_TYPE_RESULT,
   SHOTGUN_IQ_TYPE_ERROR
} Shotgun_Iq_Type;


/* pre-formatted xml */
typedef enum
{
   SHOTGUN_IQ_PRESET_BIND,
   SHOTGUN_IQ_PRESET_ROSTER,
   SHOTGUN_IQ_PRESET_RESULT,
   SHOTGUN_IQ_PRESET_SESSION,
   SHOTGUN_IQ_PRESET_DISCO_INFO,
   SHOTGUN_IQ_PRESET_GSETTINGS_GET,
   SHOTGUN_IQ_PRESET_GSETTINGS_SET,
   SHOTGUN_IQ_PRESET_MAIL_SEARCH,
   SHOTGUN_IQ_PRESET_OTR_QUERY,
} Shotgun_Iq_Preset;

struct Shotgun_Auth
{
   Eina_Bool changed : 1; /* user or from changed */
   const char *from; /* domain name of account */
   const char *user; /* username */
   const char *resource; /* identifier for "location" of user */
   const char *bind; /* full JID from xmpp:bind */
   const char *base_jid; /* bare JID */
   const char *jid; /* full JID */
   const char *desc; /* current status message */
   Shotgun_User_Status status; /* current status */
   int priority;

   const char *pass; /* stringshared */
   const char *error; /* stringshared */

   Eina_Strbuf *buf;

   Ecore_Con_Server *svr;
   const char *svr_name;
   Ecore_Con_Type ssl_verify;
   Ecore_Timer *keepalive;

   Ecore_Event_Handler *ev_add, *ev_del, *ev_error, *ev_data, *ev_upgrade, *ev_write;

   struct
   {
      Eina_Hash *auth_digestmd5;
      Eina_Bool starttls : 1;
      Eina_Bool sasl_plain : 1;
      Eina_Bool sasl_digestmd5 : 1;
      Eina_Bool sasl_oauth2 : 1;
      Eina_Bool sasl_gtoken : 1;
      Eina_Bool bind : 1;
      Eina_Bool session : 1;
      Eina_Bool archive_management : 1;
      Eina_Bool google_settings : 1;
      Eina_Bool google_nosave : 1;
      struct
      {
         Eina_Bool mail_notifications : 1;
         Eina_Bool archiving : 1;
      } gsettings;
   } features;
   Shotgun_Connection_State state;
   void *data;
   void *settings;
   Shotgun_User_Info *vcard;
   unsigned int pending_ping;
   double ping_delay;
   double ping_timeout;
   unsigned int ping_max_attempts;
   Ecore_Timer *et_ping;
   Ecore_Timer *et_ping_timeout;
};

extern int shotgun_log_dom;

#ifdef __cplusplus
extern "C" {
#endif
static inline void
shotgun_write(Ecore_Con_Server *svr, const void *data, size_t size)
{
   DBG("Sending %zu bytes:\n%s", size, (char*)data);
   ecore_con_server_send(svr, data, size);
}

static inline void
shotgun_fake_free(void *d __UNUSED__, void *d2 __UNUSED__)
{}

void shotgun_message_feed(Shotgun_Auth *auth, char *data, size_t size);
Shotgun_Event_Message *shotgun_message_new(Shotgun_Auth *auth);

void shotgun_iq_feed(Shotgun_Auth *auth, char *data, size_t size);

Shotgun_Event_Presence *shotgun_presence_new(Shotgun_Auth *auth);
void shotgun_presence_feed(Shotgun_Auth *auth, char *data, size_t size);

char *shotgun_base64_encode(const unsigned char *string, double len, size_t *size);
unsigned char *shotgun_base64_decode(const char *string, size_t len, size_t *size);
void shotgun_strtohex(unsigned char *digest, size_t len, char *ret);
void shotgun_md5_hmac_encode(unsigned char *digest, const char *string, size_t size, const void *key, size_t ksize);

Eina_Bool shotgun_login_con(Shotgun_Auth *auth, int type, Ecore_Con_Event_Server_Add *ev);
void shotgun_login(Shotgun_Auth *auth, Ecore_Con_Event_Server_Data *ev);

#ifdef __cplusplus
}
#endif
#endif
