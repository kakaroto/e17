#include "email_private.h"

Eina_Bool
email_stat(Email *e, Email_Stat_Cb cb)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(e->state != EMAIL_STATE_CONNECTED, EINA_FALSE);

   e->cbs = eina_list_append(e->cbs, cb);
   if (!e->ops)
     {
        e->current = EMAIL_OP_STAT;
        email_write(e, "STAT\r\n", 6);
     }
   e->ops = eina_list_append(e->ops, (uintptr_t*)EMAIL_OP_STAT);
   return EINA_TRUE;
}

Eina_Bool
email_list(Email *e, Email_List_Cb cb)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(e->state != EMAIL_STATE_CONNECTED, EINA_FALSE);

   e->cbs = eina_list_append(e->cbs, cb);
   if (!e->ops)
     {
        e->current = EMAIL_OP_LIST;
        email_write(e, "LIST\r\n", 6);
     }
   e->ops = eina_list_append(e->ops, (uintptr_t*)EMAIL_OP_LIST);
   return EINA_TRUE;
}

Eina_Bool
email_rset(Email *e, Email_Cb cb)
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
email_delete(Email *e, unsigned int id, Email_Cb cb)
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
email_pop3_stat_read(Email *e, const unsigned char *recv, size_t size)
{
   Email_Stat_Cb cb;
   int num;
   size_t len;

   cb = eina_list_data_get(e->cbs);
   if ((!email_op_ok((const unsigned char *)recv, size)) ||
       (sscanf((char*)recv, "+OK %u %zu", &num, &len) != 2))
     {
        ERR("Error with STAT");
        if (cb) cb(e, 0, 0);
        return EINA_TRUE;
     }
   INF("STAT returned %u messages (%zu octets)", num, len);
   if (cb) cb(e, num, len);
   return EINA_TRUE;
}

Eina_Bool
email_pop3_list_read(Email *e, Ecore_Con_Event_Server_Data *ev)
{
   Email_List_Cb cb;
   Eina_List *list = NULL;
   Email_List_Item *it;
   const char *p, *n;
   const unsigned char *data;
   size_t size;

   if ((!e->buf) && (!email_op_ok(ev->data, ev->size)))
     {
        ERR("Error with LIST");
        cb = eina_list_data_get(e->cbs);
        if (cb) cb(e, NULL);
        return EINA_TRUE;
     }
   if (e->buf)
     {
        eina_binbuf_append_length(e->buf, ev->data, ev->size);
        data = eina_binbuf_string_get(e->buf);
        size = eina_binbuf_length_get(e->buf);
     }
   else
     {
        data = ev->data;
        size = ev->size;
     }
   for (n = (char*)memchr(data + 3, '\n', size - 3), size -= (n - (char*)data);
        n && (size > 1);
        p = n, n = (char*)memchr(n, '\n', size - 1), size -= (n - (char*)data))
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
   if (n[0] == '.')
     {
        cb = eina_list_data_get(e->cbs);
        INF("LIST returned %u messages", eina_list_count(list));
        if (cb) cb(e, list);
        EINA_LIST_FREE(list, it)
          free(it);
        if (e->buf)
          {
             eina_binbuf_free(e->buf);
             e->buf = NULL;
          }
     }
   else if (!e->buf)
     {
        e->buf = eina_binbuf_new();
        eina_binbuf_append_length(e->buf, (const unsigned char*)(n), size - (n - (char*)data));
     }
   return EINA_TRUE;
}
