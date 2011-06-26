#ifndef EMAIL_PRIVATE_H
#define EMAIL_PRIVATE_H

#ifndef __UNUSED__
# define __UNUSED__ __attribute__((unused))
#endif

#include <Ecore.h>
#include <Ecore_Con.h>
#include "Email.h"

#define DBG(...)            EINA_LOG_DOM_DBG(email_log_dom, __VA_ARGS__)
#define INF(...)            EINA_LOG_DOM_INFO(email_log_dom, __VA_ARGS__)
#define WRN(...)            EINA_LOG_DOM_WARN(email_log_dom, __VA_ARGS__)
#define ERR(...)            EINA_LOG_DOM_ERR(email_log_dom, __VA_ARGS__)
#define CRI(...)            EINA_LOG_DOM_CRIT(email_log_dom, __VA_ARGS__)

extern int email_log_dom;

#define EMAIL_POP3_PORT 110
#define EMAIL_POP3S_PORT 995

#define EMAIL_POP3_LIST "LIST\r\n"
#define EMAIL_POP3_STAT "STAT\r\n"
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

   Eina_List *ops;
   Eina_List *cbs;
   void *ev;

   struct
   {
      Eina_Binbuf *apop_str;
      Eina_Bool apop : 1;
   } pop_features;
   Eina_Bool pop3 : 1;
   Eina_Bool imap : 1;
   Eina_Bool secure : 1;
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

void email_pop3_stat_read(Email *e, const char *recv, size_t size);
void email_pop3_list_read(Email *e, Ecore_Con_Event_Server_Data *ev);
void email_login_pop(Email *e, Ecore_Con_Event_Server_Data *ev);
Eina_Bool email_quit_pop(Email *e, Ecore_Cb cb);
void email_fake_free(void *d, void *e);

#endif
