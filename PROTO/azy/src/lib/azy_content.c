/*
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "Azy.h"
#include "azy_private.h"

Azy_Content *
azy_content_new(const char *method)
{
   /* method can safely be null here */
    Azy_Content *c = calloc(1, sizeof(Azy_Content));

    EINA_SAFETY_ON_NULL_RETURN_VAL(c, NULL);
    if (method && method[0])
      c->method = eina_stringshare_add(method);

    DBG("(method=%s) = %p", method, c);
    AZY_MAGIC_SET(c, AZY_MAGIC_CONTENT);
    return c;
}

void
azy_content_free(Azy_Content *content)
{
   Azy_Value *v;

   DBG("(content=%p)", content);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return;
     }

   AZY_MAGIC_SET(content, AZY_MAGIC_NONE);
   if (content->method )
     eina_stringshare_del(content->method);
   EINA_LIST_FREE(content->params, v)
     azy_value_unref(v);
   if (content->retval)
     azy_value_unref(content->retval);
   if (content->length)
     free(content->buffer);
   if (content->faultmsg)
     eina_stringshare_del(content->faultmsg);
   if (content->recv_net)
     azy_net_free(content->recv_net);
   free(content);
}

void
azy_content_data_set(Azy_Content *content,
                     const void  *data)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return;
     }
   content->data = (void*)data;
}

void *
azy_content_data_get(Azy_Content *content)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }
   return content->data;
}

Eina_Bool
azy_content_serialize_request(Azy_Content *content,
                               Azy_Net_Transport type)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return EINA_FALSE;
     }

   if (type == AZY_NET_TRANSPORT_JSON)
     return azy_content_serialize_request_json(content);

   if (type == AZY_NET_TRANSPORT_XML)
     return azy_content_serialize_request_xml(content);

   ERR("ILLEGAL TYPE PASSED! %i", type);
   return EINA_FALSE;
}

Eina_Bool
azy_content_unserialize_request(Azy_Content *content,
                                 Azy_Net_Transport type,
                                 const char   *buf,
                                 ssize_t       len)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return EINA_FALSE;
     }

   if (type == AZY_NET_TRANSPORT_JSON)
     return azy_content_unserialize_request_json(content, buf, len);

   if (type == AZY_NET_TRANSPORT_XML)
     return azy_content_unserialize_request_xml(content, buf, len);

   azy_content_error_faultmsg_set(content, 1, eina_stringshare_add_length(buf, len));
   return EINA_FALSE;
}

Eina_Bool
azy_content_serialize_response(Azy_Content *content,
                                Azy_Net_Transport type)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return EINA_FALSE;
     }

   if (type == AZY_NET_TRANSPORT_JSON)
     return azy_content_serialize_response_json(content);


   if (type == AZY_NET_TRANSPORT_XML)
     return azy_content_serialize_response_xml(content);

   ERR("ILLEGAL TYPE PASSED! %i", type);
   return EINA_FALSE;
}

Eina_Bool
azy_content_unserialize_response(Azy_Content *content,
                                  Azy_Net_Transport type,
                                  const char   *buf,
                                  ssize_t       len)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return EINA_FALSE;
     }

   if (type == AZY_NET_TRANSPORT_JSON)
     return azy_content_unserialize_response_json(content, buf, len);

   if (type == AZY_NET_TRANSPORT_XML)
     return azy_content_unserialize_response_xml(content, buf, len);

   azy_content_error_faultmsg_set(content, 1, eina_stringshare_add_length(buf, len));
   return EINA_FALSE;
}

const char *
azy_content_method_full_get(Azy_Content *content)
{
   DBG("(content=%p)", content);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }

   return content->method;
}

const char *
azy_content_method_get(Azy_Content *content)
{
   const char *ret;
   DBG("(content=%p)", content);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }

   if (!content->method)
     return NULL;

   if ((ret = strchr(content->method, '.')))
     return ret + 1;

   return content->method;
}

const char *
azy_content_module_name_get(Azy_Content *content,
                             const char   *fallback)
{
   const char *ret;
   DBG("(content=%p)", content);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }

   if ((!content->method) || (!(ret = strchr(content->method, '.'))))
     return eina_stringshare_add(fallback);

   return eina_stringshare_add_length(content->method, ret - content->method);
}

Azy_Net *
azy_content_net_get(Azy_Content *content)
{
   DBG("(content=%p)", content);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }
   return content->recv_net;
}

void
azy_content_param_add(Azy_Content *content,
                       Azy_Value   *val)
{
   DBG("(content=%p, val=%p)", content, val);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return;
     }

   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return;
     }

   content->params = eina_list_append(content->params, val);
}

Azy_Value *
azy_content_param_get(Azy_Content *content,
                       unsigned int  pos)
{
   DBG("(content=%p, pos=%u)", content, pos);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }

   return eina_list_nth(content->params, pos);
}

Eina_List *
azy_content_params_get(Azy_Content *content)
{
   DBG("(content=%p)", content);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }

   return content->params;
}

/* retval manipulation */

void
azy_content_retval_set(Azy_Content *content,
                        Azy_Value   *val)
{
   DBG("(content=%p, val=%p)", content, val);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return;
     }

   content->retval = val;
}

Azy_Value *
azy_content_retval_get(Azy_Content *content)
{
   DBG("(content=%p)", content);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }

   return content->retval;
}

/* error manipulation */

void
azy_content_error_code_set(Azy_Content *content,
                            Eina_Error    code)
{
   DBG("(content=%p, code=%d)", content, code);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return;
     }

   content->error_set = EINA_TRUE;
   content->errcode = code;
   content->faultcode = code;
}

void
azy_content_error_faultcode_set(Azy_Content *content,
                                 Eina_Error    code,
                                 int           faultcode)
{
   DBG("(content=%p, code=%d)", content, code);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return;
     }

   content->error_set = EINA_TRUE;
   content->errcode = code;
   content->faultcode = faultcode;
}

void
azy_content_error_faultmsg_set(Azy_Content *content,
                                int           faultcode,
                                const char   *fmt,
                                ...)
{
   va_list args;
   char *msg;

   DBG("(content=%p, faultcode='%d')", content, faultcode);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return;
     }

   if (!fmt)
     return;

   if (content->faultmsg)
     {
        DBG("already set error : %s", content->faultmsg);
        return;
     }

   content->error_set = EINA_TRUE;

   va_start(args, fmt);
   if (vasprintf(&msg, fmt, args) < 0)
     ERR("Memory error in %s!", __PRETTY_FUNCTION__);
   va_end(args);

   content->faultcode = faultcode;
   content->faultmsg = eina_stringshare_add(msg);
   free(msg);
}

Eina_Bool
azy_content_error_is_set(Azy_Content *content)
{
   DBG("(content=%p)", content);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return EINA_FALSE;
     }

   return content->error_set;
}

void
azy_content_error_reset(Azy_Content *content)
{
   DBG("(content=%p)", content);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return;
     }
   if (!content->error_set)
     return;

   if (content->faultmsg)
     {
        eina_stringshare_del(content->faultmsg);
        content->faultmsg = NULL;
     }
   content->errcode = 0;
   content->faultcode = 0;

   content->error_set = EINA_FALSE;
   return;
}

Eina_Error
azy_content_error_code_get(Azy_Content *content)
{
   DBG("(content=%p)", content);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return -1;
     }

   return content->errcode;
}

const char *
azy_content_error_message_get(Azy_Content *content)
{
   DBG("(content=%p)", content);

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }

   if (!content->error_set)
     return NULL;

   if (content->faultmsg)
     return content->faultmsg;

   return eina_error_msg_get(content->errcode);
}

char *
azy_content_dump_string(const Azy_Content *content,
                         unsigned int  indent)
{
   Eina_List *l;
   Eina_Strbuf *string;
   char buf[256] = {0};
   char *ret;
   Eina_Bool single_line = EINA_TRUE;
   Azy_Value *v;

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }

   memset(buf, ' ', MIN(indent * 2, sizeof(buf) - 1));
   string = eina_strbuf_new();

   // split parameters on spearate lines?
   if (eina_list_count(content->params) > 6)
     single_line = EINA_FALSE;
   else
     EINA_LIST_FOREACH(content->params, l, v)
       if (azy_value_multi_line_get_(v, 25))
         single_line = EINA_FALSE;

   eina_strbuf_append_printf(string, "%s%s(", buf, content->method ? content->method : "<anonymous>");

   if (single_line)
     EINA_LIST_FOREACH(content->params, l, v)
       {
          azy_value_dump(v, string, indent);

          if (l->next)
            eina_strbuf_append(string, ", ");
       }
   else
     {
        EINA_LIST_FOREACH(content->params, l, v)
          {
             eina_strbuf_append_printf(string, "\n%s  ", buf);
             azy_value_dump(v, string, indent + 1);

             if (l->next)
               eina_strbuf_append(string, ",");
          }

        eina_strbuf_append_printf(string, "\n%s", buf);
     }

   eina_strbuf_append(string, ")");

   if (content->retval)
     {
        eina_strbuf_append(string, " = ");
        azy_value_dump(content->retval, string, indent);
     }

   if (content->errcode)
     eina_strbuf_append_printf(string, " = { faultCode: %d, faultString: \"%s\" }", content->faultcode, content->faultmsg ? content->faultmsg : "");

   ret = eina_strbuf_string_steal(string);
   eina_strbuf_free(string);

   return ret;
}

void
azy_content_dump(const Azy_Content *content,
                  unsigned int  indent)
{
   char *str;

   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return;
     }

   if (!(str = azy_content_dump_string(content, indent)))
     return;

   DBG("%s\n", str);
   free(str);
}

unsigned char *
azy_content_buffer_get(Azy_Content *content)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }

   return content->buffer;
}

int
azy_content_length_get(Azy_Content *content)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return -1;
     }

   return content->length;
}

Eina_Bool
azy_content_buffer_set(Azy_Content  *content,
                        unsigned char *buffer,
                        int            length)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return EINA_FALSE;
     }
   if ((!buffer) || (length < 1))
     return EINA_FALSE;

   if (content->buffer)
     free(content->buffer);

   content->buffer = buffer;
   content->length = length;
   return EINA_TRUE;
}

void
azy_content_buffer_reset(Azy_Content *content)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return;
     }
   if (!content->buffer) return;

   free(content->buffer);
   content->buffer = NULL;
   content->length = 0;
}

void *
azy_content_return_get(Azy_Content *content)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return NULL;
     }
   return content->ret;
}


Azy_Client_Call_Id
azy_content_id_get(Azy_Content *content)
{
   if (!AZY_MAGIC_CHECK(content, AZY_MAGIC_CONTENT))
     {
        AZY_MAGIC_FAIL(content, AZY_MAGIC_CONTENT);
        return 0;
     }
   return content->id;
}
