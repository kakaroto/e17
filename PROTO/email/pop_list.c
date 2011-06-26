#include "email_private.h"

Eina_Bool
email_stat(Email *e, Email_Stat_Cb cb)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(e->state != EMAIL_STATE_CONNECTED, EINA_FALSE);

   e->cbs = eina_list_append(e->cbs, cb);
   if (!e->current)
     {
        e->current = EMAIL_OP_STAT;
        email_write(e, EMAIL_POP3_STAT, sizeof(EMAIL_POP3_STAT) - 1);
     }
   else
     e->ops = eina_list_append(e->ops, (uintptr_t*)EMAIL_OP_STAT);
   return EINA_TRUE;
}

Eina_Bool
email_list(Email *e, Email_List_Cb cb)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(e->state != EMAIL_STATE_CONNECTED, EINA_FALSE);

   e->cbs = eina_list_append(e->cbs, cb);
   if (!e->current)
     {
        e->current = EMAIL_OP_LIST;
        email_write(e, EMAIL_POP3_LIST, sizeof(EMAIL_POP3_LIST) - 1);
     }
   else
     e->ops = eina_list_append(e->ops, (uintptr_t*)EMAIL_OP_LIST);
   return EINA_TRUE;
}

Eina_Bool
email_rset(Email *e, Ecore_Cb cb)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(e->state != EMAIL_STATE_CONNECTED, EINA_FALSE);

   e->cbs = eina_list_append(e->cbs, cb);
   if (!e->current)
     {
        e->current = EMAIL_OP_RSET;
        email_write(e, EMAIL_POP3_RSET, sizeof(EMAIL_POP3_RSET) - 1);
     }
   else
     e->ops = eina_list_append(e->ops, (uintptr_t*)EMAIL_OP_RSET);
   return EINA_TRUE;
}

Eina_Bool
email_delete(Email *e, unsigned int id, Ecore_Cb cb)
{
   char buf[64];
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(e->state != EMAIL_STATE_CONNECTED, EINA_FALSE);

   e->cbs = eina_list_append(e->cbs, cb);
   if (!e->current)
     {
        e->current = EMAIL_OP_DELE;
        snprintf(buf, sizeof(buf), EMAIL_POP3_DELE, id);
        email_write(e, buf, strlen(buf));
     }
   else
     {
        e->op_ids = eina_list_append(e->op_ids, (uintptr_t*)id);
        e->ops = eina_list_append(e->ops, (uintptr_t*)EMAIL_OP_DELE);
     }
   return EINA_TRUE;
}

Eina_Bool
email_retrieve(Email *e, unsigned int id, Email_Retr_Cb cb)
{
   char buf[64];
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(e->state != EMAIL_STATE_CONNECTED, EINA_FALSE);

   e->cbs = eina_list_append(e->cbs, cb);
   if (!e->current)
     {
        e->current = EMAIL_OP_RETR;
        snprintf(buf, sizeof(buf), EMAIL_POP3_RETR, id);
        email_write(e, buf, strlen(buf));
     }
   else
     {
        e->op_ids = eina_list_append(e->op_ids, (uintptr_t*)id);
        e->ops = eina_list_append(e->ops, (uintptr_t*)EMAIL_OP_RETR);
     }
   return EINA_TRUE;
}

Eina_Bool
email_pop3_stat_read(Email *e, const char *recv, size_t size)
{
   Email_Stat_Cb cb;
   int num;
   size_t len;

   cb = e->cbs->data;
   e->cbs = eina_list_remove_list(e->cbs, e->cbs);
   if ((!email_op_ok((unsigned char*)recv, size)) ||
       (sscanf(recv, "+OK %u %zu", &num, &len) != 2))
     {
        ERR("Error with STAT");
        cb(e, 0, 0);
        return EINA_TRUE;
     }
   INF("STAT returned %u messages (%zu octets)", num, len);
   cb(e, num, len);
   return EINA_TRUE;
}

Eina_Bool
email_pop3_list_read(Email *e, Ecore_Con_Event_Server_Data *ev)
{
   Email_List_Cb cb;
   Eina_List *next, *list = NULL;
   Email_List_Item *it;
   const char *n;
   unsigned char *data;
   int len;
   size_t size;

   if ((!e->buf) && (!email_op_ok(ev->data, ev->size)))
     {
        ERR("Error with LIST");
        cb = e->cbs->data;
        e->cbs = eina_list_remove_list(e->cbs, e->cbs);
        cb(e, NULL);
        return EINA_TRUE;
     }
   next = e->ev ? e->ev : list;
   if (e->buf)
     {
        eina_binbuf_append_length(e->buf, ev->data, ev->size);
        data = (unsigned char*)eina_binbuf_string_get(e->buf);
        len = eina_binbuf_length_get(e->buf);
     }
   else
     {
        data = ev->data;
        len = ev->size;
     }
   for (n = (char*)memchr(data + 3, '\n', len - 3), size = len - (n - (char*)data);
        n && (size > 1);
        n = (char*)memchr(n, '\n', size - 1), size = len - (n - (char*)data))
      {
         it = calloc(1, sizeof(Email_List_Item));
         if (sscanf(++n, "%u %zu", &it->id, &it->size) != 2)
           {
              free(it);
              break;
           }
         INF("Message %u: %zu octets", it->id, it->size);
         list = eina_list_append(list, it);
      }
   if (!strncmp(n - 2, "\r\n.\r\n", 5))
     {
        cb = e->cbs->data;
        e->cbs = eina_list_remove_list(e->cbs, e->cbs);
        INF("LIST returned %u messages", eina_list_count(list));
        cb(e, list);
        EINA_LIST_FREE(list, it)
          free(it);
        if (e->buf)
          {
             eina_binbuf_free(e->buf);
             e->buf = NULL;
          }
        return EINA_TRUE;
     }
   else if (!e->buf)
     {
        e->buf = eina_binbuf_new();
        eina_binbuf_append_length(e->buf, (unsigned char*)n, ev->size - (n - (char*)ev->data));
     }
   return EINA_FALSE;
}
