#include "email_private.h"

static void
next_pop(Email *e)
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

Eina_Bool
upgrade_pop(Email *e, int type __UNUSED__, Ecore_Con_Event_Server_Upgrade *ev)
{
   if (e != ecore_con_server_data_get(ev->server)) return ECORE_CALLBACK_PASS_ON;

   e->state++;
   email_login_pop(e, NULL);
   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
data_pop(Email *e, int type __UNUSED__, Ecore_Con_Event_Server_Data *ev)
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
        email_login_pop(e, ev);
        return ECORE_CALLBACK_RENEW;
     }

   if (!e->current) return ECORE_CALLBACK_RENEW;

   switch (e->current)
     {
      case EMAIL_OP_STAT:
        if (!email_pop3_stat_read(e, recv, ev->size)) return ECORE_CALLBACK_RENEW;
        break;
      case EMAIL_OP_LIST:
        if (!email_pop3_list_read(e, ev)) return ECORE_CALLBACK_RENEW;
        break;
      case EMAIL_OP_RETR:
        if (!email_pop3_retr_read(e, ev)) return ECORE_CALLBACK_RENEW;
        break;
      case EMAIL_OP_DELE:
      case EMAIL_OP_QUIT:
      {
         Ecore_Cb cb;

         cb = e->cbs->data;
         e->cbs = eina_list_remove_list(e->cbs, e->cbs);
         if (!email_op_ok(ev->data, ev->size))
           {
              if (e->current == EMAIL_OP_DELE) ERR("Error with DELE");
              else ERR("Error with QUIT");
           }
         else
           {
              if (e->current == EMAIL_OP_DELE) INF("DELE successful");
              else INF("QUIT");
           }
         if (cb) cb(e);
         if (e->current == EMAIL_OP_QUIT) ecore_con_server_del(e->svr);
         break;
      }
      default:
        break;
     }
   next_pop(e);
   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
error_pop(Email *e __UNUSED__, int type __UNUSED__, Ecore_Con_Event_Server_Error *ev __UNUSED__)
{
   ERR("Error");
   return ECORE_CALLBACK_RENEW;
}
