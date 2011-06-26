#include "email_private.h"

Eina_Bool
email_pop3_retr_read(Email *e, Ecore_Con_Event_Server_Data *ev)
{
   Email_Retr_Cb cb;
   size_t size;
   unsigned char *data;

   cb = e->cbs->data;
   if ((!e->buf) && (!email_op_ok(ev->data, ev->size)))
     {
        ERR("Error with RETR");
        e->cbs = eina_list_remove_list(e->cbs, e->cbs);
        if (cb) cb(e, NULL);
        return EINA_TRUE;
     }
   if (e->buf)
     {
        eina_binbuf_append_length(e->buf, ev->data, ev->size);
        data = (unsigned char*)eina_binbuf_string_get(e->buf);
        size = eina_binbuf_length_get(e->buf);
     }
   else
     {
        data = memchr(ev->data, '\n', ev->size);
        if (!data) data = ev->data;
        size = ev->size - (data - (unsigned char*)ev->data);
        if (size && (data[0] == '\n')) data++, size--;
     }
   if (ev->size < 12) goto append; /* this is probably an error in the making */
   if (strncmp((char*)data + size - 5, "\r\n.\r\n", 5)) goto append;

   if (!e->buf)
     {
        e->buf = eina_binbuf_new();
        eina_binbuf_append_length(e->buf, data, size);
     }

   INF("Message retrieved: %zu bytes", eina_binbuf_length_get(e->buf));
   e->cbs = eina_list_remove_list(e->cbs, e->cbs);
   if (cb) cb(e, e->buf);
   eina_binbuf_free(e->buf);
   e->buf = NULL;
   return EINA_TRUE;

append:
   if (!e->buf)
     {
        e->buf = eina_binbuf_new();
        eina_binbuf_append_length(e->buf, data, size);
     }
   return EINA_FALSE;
}
