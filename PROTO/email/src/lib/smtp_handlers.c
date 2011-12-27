#include "email_private.h"


static void
next_smtp(Email *e)
{
   e->smtp_state = 0;
   e->internal_state = 0;

   DBG("Next queued call");
   e->ops = eina_list_remove_list(e->ops, e->ops);
   if (e->current == EMAIL_OP_SEND)
     e->op_ids = eina_list_remove_list(e->ops, e->op_ids);
   if (!e->ops)
     {
        DBG("No queued calls");
        e->current = 0;
        return;
     }
   e->current = (uintptr_t)eina_list_data_get(e->ops);
   if (e->cbs) e->cbs = eina_list_remove_list(e->cbs, e->cbs);
   switch (e->current)
     {
      case EMAIL_OP_SEND:
        if (!send_smtp(e))
          {
             Email_Send_Cb cb = eina_list_data_get(e->cbs);
             if (cb) cb(e->op_ids->data, EINA_FALSE);
             next_smtp(e);
          }
        break;
      case EMAIL_OP_QUIT:
        email_write(e, EMAIL_QUIT, sizeof(EMAIL_QUIT) - 1);
      default:
        break;
     }
}

Eina_Bool
send_smtp(Email *e)
{
   char *buf;
   size_t size;
   Email_Message *msg;
   Email_Contact *ec;
   Eina_Strbuf *bbuf;

   e->current = EMAIL_OP_SEND;
   msg = e->op_ids->data;
   switch (e->smtp_state)
     {
      case 0:
        e->smtp_state++;
      case EMAIL_SMTP_STATE_FROM:
        if ((!msg->from) && (!msg->sender))
          {
             char buf[1024];
             snprintf(buf, sizeof(buf), "%s@%s", e->username, e->features.smtp_features.domain);
             msg->sender = email_contact_new(buf);
          }
        ec = msg->sender;
        size = sizeof(char) * (sizeof(EMAIL_SMTP_FROM) + strlen(ec->address)) - 2;
        buf = alloca(size);
        snprintf(buf, size, EMAIL_SMTP_FROM, ec->address);
        email_write(e, buf, size - 1);
        e->smtp_state++;
        e->internal_state = 0;
        break;
      case EMAIL_SMTP_STATE_TO:
        ec = eina_list_nth(msg->recipients, e->internal_state++);
        if (!ec)
          {
             e->smtp_state++;
             e->internal_state = 0;
             return send_smtp(e);
          }
        size = sizeof(char) * (sizeof(EMAIL_SMTP_TO) + strlen(ec->address)) - 2;
        buf = alloca(size);
        snprintf(buf, size, EMAIL_SMTP_TO, ec->address);
        email_write(e, buf, size - 1);
        break;
      case EMAIL_SMTP_STATE_DATA:
        email_write(e, EMAIL_SMTP_DATA, sizeof(EMAIL_SMTP_DATA) - 1);
        e->smtp_state++;
        e->internal_state = 0;
        break;
      default:
        bbuf = email_message_serialize(msg);
        e->smtp_state++;
        if (bbuf)
          {
             email_write(e, eina_strbuf_string_get(bbuf), eina_strbuf_length_get(bbuf));
             eina_strbuf_free(bbuf);
          }
        else
          return EINA_FALSE;
     }
   return EINA_TRUE;
}

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
   Email_Send_Cb cb;
   Email_Cb qcb;

   if (e != ecore_con_server_data_get(ev->server))
     {
        DBG("Event mismatch");
        return ECORE_CALLBACK_PASS_ON;
     }

   if (eina_log_domain_level_check(email_log_dom, EINA_LOG_LEVEL_DBG))
     {
        recv = alloca(ev->size + 1);
        memcpy(recv, ev->data, ev->size);
        recv[ev->size] = 0;
        DBG("Receiving %i bytes:\n%s", ev->size, recv);
     }

   if (e->state < EMAIL_STATE_CONNECTED)
     {
        email_login_smtp(e, ev);
        return ECORE_CALLBACK_RENEW;
     }
   if (!e->current) return ECORE_CALLBACK_RENEW;

   cb = eina_list_data_get(e->cbs);
   qcb = eina_list_data_get(e->cbs);
   if (e->current == EMAIL_OP_QUIT)
     {
        if ((ev->size < 3) || (memcmp(ev->data, "221", 3)))
          ERR("Could not QUIT properly!");
        if (qcb) qcb(e);
        ecore_con_server_del(e->svr);
        return ECORE_CALLBACK_RENEW;
     }
   switch (e->smtp_state)
     {
      case EMAIL_SMTP_STATE_BODY:
        if ((ev->size < 3) || (memcmp(ev->data, "354", 3)))
          {
             if (cb) cb(e->op_ids->data, EINA_FALSE);
             next_smtp(e);
             return ECORE_CALLBACK_RENEW;
          }
        if (!send_smtp(e))
          {
             if (cb) cb(e->op_ids->data, EINA_FALSE);
             next_smtp(e);
          }
        break;
      default:
        if ((ev->size < 3) || (memcmp(ev->data, "250", 3)))
          {
             if (cb) cb(e->op_ids->data, EINA_FALSE);
             next_smtp(e);
          }
        else if (e->smtp_state > EMAIL_SMTP_STATE_BODY)
          {
             if (cb) cb(e->op_ids->data, EINA_TRUE);
             next_smtp(e);
          }
        else
          {
             if (!send_smtp(e))
               {
                  if (cb) cb(e->op_ids->data, EINA_FALSE);
                  next_smtp(e);
               }
          }
     }

   return ECORE_CALLBACK_RENEW;
}

Eina_Bool
error_smtp(Email *e __UNUSED__, int type __UNUSED__, Ecore_Con_Event_Server_Error *ev __UNUSED__)
{
   ERR("Error");
   return ECORE_CALLBACK_RENEW;
}
