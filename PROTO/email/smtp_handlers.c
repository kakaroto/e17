#include "email_private.h"

#if 0
static void
next_smtp(Email *e)
{
   char buf[64];

   if (e->buf) return;
   if (!e->ops)
     {
        e->current = 0;
        return;
     }
   DBG("Next queued call");
   e->current = (uintptr_t)e->ops->data;
   e->ops = eina_list_remove_list(e->ops, e->ops);
   switch (e->current)
     {
      case EMAIL_OP_STAT:
        email_write(e, EMAIL_POP3_STAT, sizeof(EMAIL_POP3_STAT) - 1);
        break;
      case EMAIL_OP_LIST:
        email_write(e, EMAIL_POP3_LIST, sizeof(EMAIL_POP3_LIST) - 1);
        break;
      case EMAIL_OP_RSET:
        email_write(e, EMAIL_POP3_RSET, sizeof(EMAIL_POP3_RSET) - 1);
        break;
      case EMAIL_OP_DELE:
        snprintf(buf, sizeof(buf), EMAIL_POP3_DELE, (uintptr_t)e->op_ids->data);
        e->op_ids = eina_list_remove_list(e->op_ids, e->op_ids);
        email_write(e, buf, strlen(buf));
        break;
      case EMAIL_OP_RETR:
        snprintf(buf, sizeof(buf), EMAIL_POP3_RETR, (uintptr_t)e->op_ids->data);
        e->op_ids = eina_list_remove_list(e->op_ids, e->op_ids);
        email_write(e, buf, strlen(buf));
        break;
      case EMAIL_OP_QUIT:
        email_write(e, EMAIL_POP3_QUIT, sizeof(EMAIL_POP3_QUIT) - 1);
        break;
      default:
        break;
     }
}
#endif

Eina_Bool
upgrade_smtp(Email *e, int type __UNUSED__, Ecore_Con_Event_Server_Upgrade *ev)
{
   char *buf;
   size_t size;
   
   if (e != ecore_con_server_data_get(ev->server)) return ECORE_CALLBACK_PASS_ON;

   e->state++;
   size = sizeof(char) * (sizeof("EHLO \r\n") + strlen(e->features.smtp_features.domain));

   buf = alloca(size);
   snprintf(buf, size, "EHLO %s\r\n", e->features.smtp_features.domain);
   email_write(e, buf, size - 1);
   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
data_smtp(Email *e, int type __UNUSED__, Ecore_Con_Event_Server_Data *ev)
{
   char *recv;

   if (e != ecore_con_server_data_get(ev->server))
     {
        DBG("Event mismatch");
        return ECORE_CALLBACK_PASS_ON;
     }

   recv = alloca(ev->size + 1);
   memcpy(recv, ev->data, ev->size);
   recv[ev->size] = 0;
   DBG("Receiving %i bytes:\n%s", ev->size, recv);

   if (e->state < EMAIL_STATE_CONNECTED)
     {
        email_login_smtp(e, ev);
        return ECORE_CALLBACK_RENEW;
     }

   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
error_smtp(Email *e __UNUSED__, int type __UNUSED__, Ecore_Con_Event_Server_Error *ev __UNUSED__)
{
   ERR("Error");
   return ECORE_CALLBACK_RENEW;
}
