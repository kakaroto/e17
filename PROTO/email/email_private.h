#ifndef EMAIL_PRIVATE_H
#define EMAIL_PRIVATE_H

#ifndef __UNUSED__
# define __UNUSED__ __attribute__((unused))
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

#define EMAIL_POP3_PORT 110
#define EMAIL_POP3S_PORT 995

#define EMAIL_SMTP_PORT 25
#define EMAIL_ESMTP_PORT 587

#define EMAIL_POP3_LIST "LIST\r\n"
#define EMAIL_POP3_STAT "STAT\r\n"
#define EMAIL_POP3_RSET "RSET\r\n"
#define EMAIL_POP3_DELE "DELE %"PRIu32"\r\n"
#define EMAIL_POP3_RETR "RETR %"PRIu32"\r\n"
#define EMAIL_POP3_QUIT "QUIT\r\n"

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
   EMAIL_OP_STAT = 1,
   EMAIL_OP_LIST,
   EMAIL_OP_RSET,
   EMAIL_OP_DELE,
   EMAIL_OP_RETR,
   EMAIL_OP_QUIT,
} Email_Op;

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
         size_t size;

         Eina_Bool cram : 1;
         Eina_Bool digest : 1;
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
email_op_ok(unsigned char *data, int size)
{
   return !((size < 3) || (data[0] != '+') || strncasecmp((char*)data + 1, "OK", 2));
}

static inline void
email_write(Email *e, const void *data, size_t size)
{
   DBG("Sending:\n%s", (char*)data);
   ecore_con_server_send(e->svr, data, size);
}

Eina_Bool upgrade_pop(Email *e, int type, Ecore_Con_Event_Server_Upgrade *ev);
Eina_Bool data_pop(Email *e, int type, Ecore_Con_Event_Server_Data *ev);
Eina_Bool error_pop(Email *e, int type, Ecore_Con_Event_Server_Error *ev );

Eina_Bool upgrade_smtp(Email *e, int type, Ecore_Con_Event_Server_Upgrade *ev);
Eina_Bool data_smtp(Email *e, int type, Ecore_Con_Event_Server_Data *ev);
Eina_Bool error_smtp(Email *e, int type, Ecore_Con_Event_Server_Error *ev );

/* return EINA_TRUE if e->ops should be popped */
Eina_Bool email_pop3_stat_read(Email *e, const char *recv, size_t size);
Eina_Bool email_pop3_list_read(Email *e, Ecore_Con_Event_Server_Data *ev);
Eina_Bool email_pop3_retr_read(Email *e, Ecore_Con_Event_Server_Data *ev);


void email_login_pop(Email *e, Ecore_Con_Event_Server_Data *ev);
void email_login_smtp(Email *e, Ecore_Con_Event_Server_Data *ev);
Eina_Bool email_quit_pop(Email *e, Ecore_Cb cb);

void email_fake_free(void *d, void *e);
char *email_base64_encode(const char *string, double len);
#endif
