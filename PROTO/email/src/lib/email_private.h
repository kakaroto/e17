#ifndef EMAIL_PRIVATE_H
#define EMAIL_PRIVATE_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

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

#include <Ecore.h>
#include <Ecore_Con.h>
#include "Email.h"
#include <inttypes.h>

#define DBG(...)            EINA_LOG_DOM_DBG(email_log_dom, __VA_ARGS__)
#define INF(...)            EINA_LOG_DOM_INFO(email_log_dom, __VA_ARGS__)
#define WRN(...)            EINA_LOG_DOM_WARN(email_log_dom, __VA_ARGS__)
#define ERR(...)            EINA_LOG_DOM_ERR(email_log_dom, __VA_ARGS__)
#define CRI(...)            EINA_LOG_DOM_CRIT(email_log_dom, __VA_ARGS__)

extern int email_log_dom;
extern Eina_Hash *_email_contacts_hash;

#define EMAIL_POP3_PORT 110
#define EMAIL_POP3S_PORT 995

#define EMAIL_SMTP_PORT 25
#define EMAIL_ESMTP_PORT 27

#define EMAIL_POP3_LIST "LIST\r\n"
#define EMAIL_POP3_STAT "STAT\r\n"
#define EMAIL_POP3_RSET "RSET\r\n"
#define EMAIL_POP3_DELE "DELE %"PRIu32"\r\n"
#define EMAIL_POP3_RETR "RETR %"PRIu32"\r\n"

#define EMAIL_QUIT "QUIT\r\n"

#define EMAIL_SMTP_FROM "MAIL FROM: <%s>\r\n"
#define EMAIL_SMTP_TO "RCPT TO: <%s>\r\n"
#define EMAIL_SMTP_DATA "DATA\r\n"

typedef enum
{
   EMAIL_STATE_INIT,
   EMAIL_STATE_SSL,
   EMAIL_STATE_USER,
   EMAIL_STATE_PASS,
   EMAIL_STATE_CONNECTED
} Email_State;

typedef enum
{
   EMAIL_SMTP_STATE_FROM = 1,
   EMAIL_SMTP_STATE_TO,
   EMAIL_SMTP_STATE_DATA,
   EMAIL_SMTP_STATE_BODY,
} Email_Smtp_State;

struct Email_Message
{
   Email *owner;
   void *data;

   Eina_List *recipients; /* anyone who will receive the message */
   Email_Contact *sender; /* Sender: */
   Eina_List *from; /* Email_Contact * */ /* From: X,Y,Z */
   char *subject;

   char *content;
   size_t csize;
   const char *charset;
   double mimeversion;

   Eina_Hash *headers;
   Eina_List *attachments;
};

struct Email_Attachment
{
   const char *name;
   const char *content_type;

   void *content;
   size_t csize;
};

struct Email_Contact
{
   unsigned int refcount;
   const char *address;
   const char *name;
   Email_Message_Contact_Type type;
};

struct Email
{
   Ecore_Con_Server *svr;
   const char *addr;
   int flags;

   Email_State state;
   Eina_Binbuf *buf;

   const char *username;
   char *password;
   void *data;
   Eina_List *certs;

   Email_Op current;
   Eina_List *ops;
   Eina_List *op_ids;
   Eina_List *cbs;
   void *ev;

   Ecore_Event_Handler *h_data, *h_del, *h_error, *h_upgrade;

   Email_Smtp_State smtp_state;
   unsigned int internal_state;

   union
   {
      struct
      {
         Eina_Binbuf *apop_str;
         Eina_Bool apop : 1;
      } pop_features;
      struct
      {
         const char *domain;
         Eina_Bool ssl : 1;
         Eina_Bool pipelining : 1;
         Eina_Bool eightbit : 1;
         size_t size;

         Eina_Bool cram : 1;
         Eina_Bool login : 1;
         Eina_Bool plain : 1;
      } smtp_features;
   } features;
   Eina_Bool pop3 : 1;
   Eina_Bool imap : 1;
   Eina_Bool smtp : 1;
   Eina_Bool secure : 1;
   Eina_Bool deleted : 1;
};

static inline Eina_Bool
email_op_ok(const unsigned char *data, int size)
{
   return !((size < 3) || (data[0] != '+') || strncasecmp((char*)data + 1, "OK", 2));
}

static inline void
email_write(Email *e, const void *data, size_t size)
{
   DBG("Sending:\n%s", (char*)data);
   ecore_con_server_send(e->svr, data, size);
}

Eina_Hash *_email_contacts_hash;

Eina_Bool upgrade_pop(Email *e, int type, Ecore_Con_Event_Server_Upgrade *ev);
Eina_Bool data_pop(Email *e, int type, Ecore_Con_Event_Server_Data *ev);
Eina_Bool error_pop(Email *e, int type, Ecore_Con_Event_Server_Error *ev );

Eina_Bool upgrade_smtp(Email *e, int type, Ecore_Con_Event_Server_Upgrade *ev);
Eina_Bool data_smtp(Email *e, int type, Ecore_Con_Event_Server_Data *ev);
Eina_Bool error_smtp(Email *e, int type, Ecore_Con_Event_Server_Error *ev );

/* return EINA_TRUE if e->ops should be popped */
Eina_Bool email_pop3_stat_read(Email *e, const unsigned char *recv, size_t size);
Eina_Bool email_pop3_list_read(Email *e, Ecore_Con_Event_Server_Data *ev);
Eina_Bool email_pop3_retr_read(Email *e, Ecore_Con_Event_Server_Data *ev);

Eina_Binbuf *email_message_serialize(Email_Message *msg);
Eina_Bool send_smtp(Email *e);

void email_login_pop(Email *e, Ecore_Con_Event_Server_Data *ev);
void email_login_smtp(Email *e, Ecore_Con_Event_Server_Data *ev);

void email_fake_free(void *d, void *e);
char *email_base64_encode(const char *string, double len, int *);
char *email_base64_decode(const char *string, int len, int *);
void email_md5_digest_to_str(unsigned char *digest, char *ret);
void email_md5_hmac_encode(unsigned char *digest, const char *string, size_t size, const char *key, size_t ksize);
#endif
