#include "email_private.h"
#include "md5.h"

void
email_login_pop(Email *e, Ecore_Con_Event_Server_Data *ev)
{
   char *buf;
   size_t size;

   switch (e->state)
     {
      case EMAIL_STATE_SSL:
        if (!email_op_ok(ev->data, ev->size))
          {
             ERR("Could not create secure connection!");
             ecore_con_server_del(ev->server);
             return;
          }
        ecore_con_ssl_server_upgrade(e->svr, ECORE_CON_USE_MIXED);
        ecore_con_ssl_server_verify(e->svr);
        e->flags = ECORE_CON_USE_MIXED;
        return;
      case EMAIL_STATE_INIT:
        if (!email_op_ok(ev->data, ev->size))
          {
             ERR("Not a POP3 server!");
             ecore_con_server_del(ev->server);
             return;
          }
        if (ev->size > 20)
          {
             const char *end;

             end = (char*)memrchr(ev->data + 3, '>', ev->size - 3);
             if (end)
               {
                  const char *start;

                  start = (char*)memrchr(ev->data + 3, '<', end - (char*)ev->data);
                  if (start)
                    {
                       e->pop_features.apop = EINA_TRUE;
                       e->pop_features.apop_str = eina_binbuf_new();
                       eina_binbuf_append_length(e->pop_features.apop_str, start, end - start + 1);
                    }
               }
          }
        if (e->secure && (!e->flags))
          {
             email_write(e->svr, "STLS\r\n", sizeof("STLS\r\n") - 1);
             e->state++;
             return;
          }
        e->state = EMAIL_STATE_USER;
        ev = NULL;
      case EMAIL_STATE_USER:
        if (!ev)
          {
             unsigned char digest[16], md5buf[33];
             char hexchars[17] = "0123456789abcdef";
             unsigned int x, y;

             if (!e->pop_features.apop)
               {
                  size = sizeof(char) * (sizeof("USER ") - 1 + sizeof("\r\n") - 1 + strlen(e->username)) + 1;
                  buf = alloca(size);
                  snprintf(buf, size, "USER %s\r\n", e->username);
                  email_write(e->svr, buf, size - 1);
                  return;
               }
             e->state++;
             eina_binbuf_append_length(e->pop_features.apop_str, e->password, strlen(e->password));

             md5_buffer(eina_binbuf_string_get(e->pop_features.apop_str), eina_binbuf_length_get(e->pop_features.apop_str), digest);
             for (x = y = 0; x < sizeof(md5buf); x++, y++)
               {
                  md5buf[x++] = hexchars[y >> 4];
                  md5buf[x] = hexchars[y & 15];
               }
             md5buf[32] = 0;
             size = sizeof(char) * (sizeof("APOP ") - 1 + sizeof("\r\n") - 1 + strlen(e->username)) + sizeof(md5buf);
             buf = alloca(size);
             snprintf(buf, size, "APOP %s %s\r\n", e->username, md5buf);
             email_write(e->svr, buf, size - 1);
             return;
          }
        if (!email_op_ok(ev->data, ev->size))
          {
             ERR("Username invalid!");
             ecore_con_server_del(e->svr);
             return;
          }
        size = sizeof(char) * (sizeof("PASS ") - 1 + sizeof("\r\n") - 1 + strlen(e->password)) + 1;
        buf = alloca(size);
        snprintf(buf, size, "PASS %s\r\n", e->password);
        ecore_con_server_send(e->svr, buf, size - 1);
        e->state++;
        return;
      case EMAIL_STATE_PASS:
        if (!email_op_ok(ev->data, ev->size))
          {
             ERR("Credentials invalid!");
             ecore_con_server_del(e->svr);
             return;
          }
        INF("Logged in successfully!");
        e->state++;
        ecore_event_add(EMAIL_EVENT_CONNECTED, e, (Ecore_End_Cb)email_fake_free, NULL);
      default:
        break;
     }
}

Eina_Bool
email_quit_pop(Email *e, Ecore_Cb cb)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(e->state != EMAIL_STATE_CONNECTED, EINA_FALSE);

   e->ops = eina_list_append(e->ops, (uintptr_t*)EMAIL_OP_QUIT);
   e->cbs = eina_list_append(e->cbs, cb);
   email_write(e->svr, "QUIT\r\n", 6);
   return EINA_TRUE;
}
