#include "email_private.h"

Eina_Bool
email_stat(Email *e, Email_Stat_Cb cb)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(e->state != EMAIL_STATE_CONNECTED, EINA_FALSE);

   e->cbs = eina_list_append(e->cbs, cb);
   if (!e->ops) email_write(e, "STAT\r\n", 6);
   e->ops = eina_list_append(e->ops, (uintptr_t*)EMAIL_OP_STAT);
   return EINA_TRUE;
}

Eina_Bool
email_list(Email *e, Email_List_Cb cb)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(e->state != EMAIL_STATE_CONNECTED, EINA_FALSE);

   e->cbs = eina_list_append(e->cbs, cb);
   if (!e->ops) email_write(e, "LIST\r\n", 6);
   e->ops = eina_list_append(e->ops, (uintptr_t*)EMAIL_OP_LIST);
   return EINA_TRUE;
}

void
email_pop3_stat_read(Email *e, const char *recv, size_t size)
{
   Email_Stat_Cb cb;
   int num;
   size_t len;

   cb = e->cbs->data;
   e->cbs = eina_list_remove_list(e->cbs, e->cbs);
   e->ops = eina_list_remove_list(e->ops, e->ops);
   if ((!email_op_ok((unsigned char*)recv, size)) ||
       (sscanf(recv, "+OK %u %zu", &num, &len) != 2))
     {
        ERR("Error with STAT");
        cb(e, 0, 0);
        return;
     }
   INF("STAT returned %u messages (%zu octets)", num, len);
   cb(e, num, len);
}

void
email_pop3_list_read(Email *e, Ecore_Con_Event_Server_Data *ev)
{
   Email_List_Cb cb;
   Eina_List *next, *list = NULL;
   Email_List_Item *it;
   const char *n;
   size_t size;

   if ((!e->buf) && (!email_op_ok(ev->data, ev->size)))
     {
        ERR("Error with LIST");
        cb = e->cbs->data;
        e->cbs = eina_list_remove_list(e->cbs, e->cbs);
        e->ops = eina_list_remove_list(e->ops, e->ops);
        cb(e, NULL);
        return;
     }
   next = e->ev ? e->ev : list;
   if (e->buf)
     {
        eina_binbuf_append_length(e->buf, ev->data, ev->size);
        ev->data = (unsigned char*)eina_binbuf_string_get(e->buf);
        ev->size = eina_binbuf_length_get(e->buf);
     }
   for (n = (char*)memchr(ev->data + 3, '\n', ev->size - 3), size = ev->size - (n - (char*)ev->data);
        n && (size > 1);
        n = (char*)memchr(n, '\n', size - 1), size = ev->size - (n - (char*)ev->data))
      {
         it = calloc(1, sizeof(Email_List_Item));
         if (sscanf(++n, "%lu %zu", &it->id, &it->size) != 2)
           {
              free(it);
              break;
           }
         INF("Message %lu: %zu octets", it->id, it->size);
         list = eina_list_append(list, it);
      }
   if (!strncmp(n - 2, "\r\n.\r\n", 5))
     {
        cb = e->cbs->data;
        e->cbs = eina_list_remove_list(e->cbs, e->cbs);
        e->ops = eina_list_remove_list(e->ops, e->ops);
        INF("LIST returned %u messages", eina_list_count(list));
        cb(e, list);
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
        eina_binbuf_append_length(e->buf, n, ev->size - (n - (char*)ev->data));
     }
}
