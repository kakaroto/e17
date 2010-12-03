/*
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>

#include "Azy.h"
#include "azy_private.h"

#ifndef MIN
# define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

static Azy_Value *
_azy_value_new(void)
{
   Azy_Value *v;

   v = calloc(1, sizeof(Azy_Value));

   EINA_SAFETY_ON_NULL_RETURN_VAL(v, NULL);
   AZY_MAGIC_SET(v, AZY_MAGIC_VALUE);
   return azy_value_ref(v);
}

static Eina_Bool
_azy_value_list_multi_line_get(Azy_Value *v)
{
   Eina_List *l;
   Azy_Value *val;

   if (!v)
     return EINA_FALSE;

   if (v->type == AZY_VALUE_ARRAY)
     {
        if (eina_list_count(azy_value_children_items_get(v)) > 8)
          return EINA_TRUE;
        else
          EINA_LIST_FOREACH(azy_value_children_items_get(v), l, val)
            if (_azy_value_multi_line_get(val, 35))
              return EINA_TRUE;
     }
   else if (v->type == AZY_VALUE_STRUCT)
     {
        if (eina_list_count(azy_value_children_items_get(v)) > 5)
          return EINA_TRUE;
        else
          EINA_LIST_FOREACH(azy_value_children_items_get(v), l, val)
            if (_azy_value_multi_line_get(azy_value_struct_member_value_get(val), 25))
              return EINA_TRUE;
     }

   return EINA_FALSE;
}

Eina_Bool
_azy_value_multi_line_get(Azy_Value *val,
                          int        max_strlen)
{
   switch (val->type)
     {
      case AZY_VALUE_STRUCT:
      case AZY_VALUE_ARRAY:
        if (azy_value_children_items_get(val))
          return EINA_TRUE;
        break;
      case AZY_VALUE_STRING:
        if (val->str_val && (eina_stringshare_strlen(val->str_val) > max_strlen))
          return EINA_TRUE;
        break;
      default:
        break;
     }
   return EINA_FALSE;
}

Azy_Value *
azy_value_ref(Azy_Value *val)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return NULL;
     }

   val->ref++;

   return val;
}

void
azy_value_unref(Azy_Value *val)
{
   Azy_Value *v;

   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return;
     }

   --val->ref;
   if (val->ref)
     return;

   AZY_MAGIC_SET(val, AZY_MAGIC_NONE);

   if (val->str_val) eina_stringshare_del(val->str_val);
   if (val->member_name) eina_stringshare_del(val->member_name);
   if (val->member_value)
     azy_value_unref(val->member_value);
   EINA_LIST_FREE(val->children, v)
     azy_value_unref(v);
   free(val);
}

/* base types */
Azy_Value *
azy_value_string_new(const char *val)
{
   Azy_Value *v;

   v = _azy_value_new();
   EINA_SAFETY_ON_NULL_RETURN_VAL(v, NULL);
   v->type = AZY_VALUE_STRING;
   v->str_val = eina_stringshare_add(val ? val : "");
   return v;
}

Azy_Value *
azy_value_int_new(int val)
{
   Azy_Value *v;

   v = _azy_value_new();
   EINA_SAFETY_ON_NULL_RETURN_VAL(v, NULL);
   v->type = AZY_VALUE_INT;
   v->int_val = val;
   return v;
}

Azy_Value *
azy_value_bool_new(Eina_Bool val)
{
   Azy_Value *v;

   v = _azy_value_new();
   EINA_SAFETY_ON_NULL_RETURN_VAL(v, NULL);
   v->type = AZY_VALUE_BOOLEAN;
   v->int_val = val;
   return v;
}

Azy_Value *
azy_value_double_new(double val)
{
   Azy_Value *v;

   v = _azy_value_new();
   EINA_SAFETY_ON_NULL_RETURN_VAL(v, NULL);
   v->type = AZY_VALUE_DOUBLE;
   v->dbl_val = val;
   return v;
}

Azy_Value *
azy_value_time_new(const char *val)
{
   Azy_Value *v;

   v = _azy_value_new();
   EINA_SAFETY_ON_NULL_RETURN_VAL(v, NULL);
   v->type = AZY_VALUE_TIME;
   v->str_val = eina_stringshare_add(val ? val : "");
   return v;
}

Azy_Value *
azy_value_base64_new(const char *base64)
{
   Azy_Value *val;

   if (!base64) return NULL;
   
   val = _azy_value_new();
   EINA_SAFETY_ON_NULL_RETURN_VAL(val, NULL);
   val->type = AZY_VALUE_BASE64;
   val->str_val = eina_stringshare_add(base64);
   return val;
}

Eina_Bool
azy_value_to_int(Azy_Value *val,
                  int        *nval)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return EINA_FALSE;
     }
   if ((!nval) || (val->type != AZY_VALUE_INT))
     return EINA_FALSE;

   *nval = val->int_val;
   return EINA_TRUE;
}

Eina_Bool
azy_value_to_string(Azy_Value  *val,
                     const char **nval)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return EINA_FALSE;
     }
   if (!nval) return EINA_FALSE;

   switch (val->type)
     {
      case AZY_VALUE_STRING:
      case AZY_VALUE_TIME:
        *nval = eina_stringshare_ref(val->str_val);
        break;
      case AZY_VALUE_BASE64:
        {
           char *buf;
           buf = azy_base64_decode(val->str_val, eina_stringshare_strlen(val->str_val));
           EINA_SAFETY_ON_NULL_RETURN_VAL(buf, EINA_FALSE);
           *nval = eina_stringshare_add(buf);
           free(buf);
           break;
        }
      default:
        return EINA_FALSE;
     }
        
   return EINA_TRUE;
}

Eina_Bool
azy_value_to_base64(Azy_Value  *val,
                    const char **nval)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return EINA_FALSE;
     }
   if ((!nval) || (val->type != AZY_VALUE_BASE64)) return EINA_FALSE;

   *nval = eina_stringshare_ref(val->str_val);
   return EINA_TRUE;
}

Eina_Bool
azy_value_to_bool(Azy_Value *val,
                  Eina_Bool *nval)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return EINA_FALSE;
     }
   if ((!nval) || (val->type != AZY_VALUE_BOOLEAN))
     return EINA_FALSE;

   *nval = !!val->int_val;
   return EINA_TRUE;
}

Eina_Bool
azy_value_to_double(Azy_Value *val,
                     double     *nval)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return EINA_FALSE;
     }
   if ((!nval) || ((val->type != AZY_VALUE_DOUBLE) && (val->type != AZY_VALUE_INT)))
     return EINA_FALSE;

   if (val->type == AZY_VALUE_DOUBLE)
     *nval = val->dbl_val;
   else
     *nval = (double)val->int_val;
   return EINA_TRUE;
}

Eina_Bool
azy_value_to_value(Azy_Value  *val,
                    Azy_Value **nval)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return EINA_FALSE;
     }
   
   if (!nval)
     return EINA_FALSE;

   *nval = azy_value_ref(val);
   return EINA_TRUE;
}

Azy_Value_Type
azy_value_type_get(Azy_Value *val)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return -1;
     }

   return val->type;
}

const char *
azy_value_struct_member_name_get(Azy_Value *val)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return NULL;
     }
   if (val->type != AZY_VALUE_MEMBER)
     return NULL;

   return val->member_name;
}

Azy_Value *
azy_value_struct_member_value_get(Azy_Value *val)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return NULL;
     }
   if (val->type != AZY_VALUE_MEMBER)
     return NULL;

   return val->member_value;
}

Azy_Value *
azy_value_struct_member_get(Azy_Value *val,
                             const char *name)
{
   Eina_List *l;
   Azy_Value *m;

   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return NULL;
     }
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, NULL);


   if (val->type != AZY_VALUE_STRUCT)
     return NULL;

   EINA_LIST_FOREACH(val->children, l, m)

     if (!strcmp(azy_value_struct_member_name_get(m), name))
       return azy_value_struct_member_value_get(m);

   return NULL;
}

Eina_List *
azy_value_children_items_get(Azy_Value *val)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return NULL;
     }
   if ((val->type != AZY_VALUE_ARRAY) && (val->type != AZY_VALUE_STRUCT))
     return NULL;

   return val->children;
}

/* composite types */

Azy_Value *
azy_value_struct_new(void)
{
   Azy_Value *v;

   v = _azy_value_new();
   EINA_SAFETY_ON_NULL_RETURN_VAL(v, NULL);
   v->type = AZY_VALUE_STRUCT;
   return v;
}

Azy_Value *
azy_value_struct_new_from_string(const char *name, const char *value)
{
   Azy_Value *v, *str;

   v = _azy_value_new();
   EINA_SAFETY_ON_NULL_RETURN_VAL(v, NULL);
   v->type = AZY_VALUE_STRUCT;
   str = azy_value_string_new(value);
   if (!str)
     {
        azy_value_unref(v);
        return NULL;
     }
   azy_value_struct_member_set(v, name, str);
   return v;
}

Azy_Value *
azy_value_struct_new_from_int(const char *name, int value)
{
   Azy_Value *v, *i;

   v = _azy_value_new();
   EINA_SAFETY_ON_NULL_RETURN_VAL(v, NULL);
   v->type = AZY_VALUE_STRUCT;
   i = azy_value_int_new(value);
   if (!i)
     {
        azy_value_unref(v);
        return NULL;
     }
   azy_value_struct_member_set(v, name, i);
   return v;
}

Azy_Value *
azy_value_struct_new_from_double(const char *name, double value)
{
   Azy_Value *v, *d;

   v = _azy_value_new();
   EINA_SAFETY_ON_NULL_RETURN_VAL(v, NULL);
   v->type = AZY_VALUE_STRUCT;
   d = azy_value_double_new(value);
   if (!d)
     {
        azy_value_unref(v);
        return NULL;
     }
   azy_value_struct_member_set(v, name, d);
   return v;
}

Azy_Value *
azy_value_array_new(void)
{
   Azy_Value *v;

   v = _azy_value_new();
   EINA_SAFETY_ON_NULL_RETURN_VAL(v, NULL);
   v->type = AZY_VALUE_ARRAY;
   return v;
}

void
azy_value_struct_member_set(Azy_Value *str,
                             const char *name,
                             Azy_Value *val)
{
   Eina_List *l;
   Azy_Value *m, *v;

   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return;
     }

   if (!AZY_MAGIC_CHECK(str, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(str, AZY_MAGIC_VALUE);
        return;
     }

   EINA_SAFETY_ON_NULL_RETURN(name);

   if (str->type != AZY_VALUE_STRUCT)
     return;

   EINA_LIST_FOREACH(str->children, l, m)
     if (!strcmp(m->member_name, name))
       {
          azy_value_unref(m->member_value);
          m->member_value = val;
          return;
       }

   v = _azy_value_new();
   EINA_SAFETY_ON_NULL_RETURN(v);
   v->type = AZY_VALUE_MEMBER;
   v->member_name = eina_stringshare_add(name);
   v->member_value = val;
   str->children = eina_list_append(str->children, v);
}

void
azy_value_array_append(Azy_Value *arr,
                        Azy_Value *val)
{
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return;
     }
   if (!AZY_MAGIC_CHECK(arr, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(arr, AZY_MAGIC_VALUE);
        return;
     }

   arr->children = eina_list_append(arr->children, val);
}

Eina_Bool
azy_value_retval_is_error(Azy_Value  *val,
                           int         *errcode,
                           const char **errmsg)
{
   Azy_Value *c, *s;
   
   if (!AZY_MAGIC_CHECK(val, AZY_MAGIC_VALUE))
     {
        AZY_MAGIC_FAIL(val, AZY_MAGIC_VALUE);
        return EINA_FALSE;
     }
   if ((val->type != AZY_VALUE_STRUCT) || (!errcode) || (!errmsg))
     return EINA_FALSE;

   c = azy_value_struct_member_get(val, "faultCode");
   s = azy_value_struct_member_get(val, "faultString");

   if ((!c) && (!s))
     return EINA_FALSE;

   if (s)
     *errmsg = s->str_val;
   if (c)
     *errcode = c->int_val;

   return EINA_TRUE;
}

void
azy_value_dump(Azy_Value  *v,
                Eina_Strbuf *string,
                unsigned int indent)
{
   Eina_List *l;
   Azy_Value *val;
   char buf[256];

   if (!v)
     return;

   memset(buf, ' ', MIN(indent * 2, sizeof(buf) - 1));

   switch (azy_value_type_get(v))
     {
      case AZY_VALUE_ARRAY:
      {
         if (!azy_value_children_items_get(v))
           eina_strbuf_append(string, "[]");
         else if (!_azy_value_list_multi_line_get(v))
           {
              eina_strbuf_append(string, "[ ");
              EINA_LIST_FOREACH(azy_value_children_items_get(v), l, val)
                {
                   azy_value_dump(val, string, indent + 1);
                   eina_strbuf_append_printf(string, "%s ", l->next ? "," : "");
                }
              eina_strbuf_append_char(string, ']');
           }
         else
           {
              eina_strbuf_append_char(string, '[');
              EINA_LIST_FOREACH(azy_value_children_items_get(v), l, val)
                {
                   eina_strbuf_append_printf(string, "\n%s  ", buf);
                   azy_value_dump(val, string, indent + 1);

                   if (l->next)
                     eina_strbuf_append_char(string, ',');
                }
              eina_strbuf_append_printf(string, "\n%s]", buf);
           }

         break;
      }

      case AZY_VALUE_STRUCT:
      {
         if (!azy_value_children_items_get(v))
           eina_strbuf_append(string, "{}");
         else if (!_azy_value_list_multi_line_get(v))
           {
              eina_strbuf_append(string, "{ ");
              EINA_LIST_FOREACH(azy_value_children_items_get(v), l, val)
                {
                   azy_value_dump(val, string, indent);
                   eina_strbuf_append_printf(string, "%s ", l->next ? "," : "");
                }
              eina_strbuf_append_char(string, '}');
           }
         else
           {
              eina_strbuf_append_char(string, '{');
              EINA_LIST_FOREACH(azy_value_children_items_get(v), l, val)
                {
                   eina_strbuf_append_printf(string, "\n%s  ", buf);
                   azy_value_dump(val, string, indent);

                   if (l->next)
                     eina_strbuf_append_char(string, ',');
                }
              eina_strbuf_append_printf(string, "\n%s}", buf);
           }

         break;
      }

      case AZY_VALUE_MEMBER:
      {
         eina_strbuf_append_printf(string, "%s: ", azy_value_struct_member_name_get(v));
         azy_value_dump(azy_value_struct_member_value_get(v), string, indent + 1);
         break;
      }

      case AZY_VALUE_INT:
      {
         eina_strbuf_append_printf(string, "%d", v->int_val);
         break;
      }

      case AZY_VALUE_STRING:
      case AZY_VALUE_TIME:
      case AZY_VALUE_BASE64:
      {
         eina_strbuf_append_printf(string, "\"%s\"", v->str_val);
         break;
      }

      case AZY_VALUE_BOOLEAN:
      {
         eina_strbuf_append_printf(string, "%s", v->int_val ? "true" : "false");
         break;
      }

      case AZY_VALUE_DOUBLE:
      {
         eina_strbuf_append_printf(string, "%g", v->dbl_val);
         break;
      }
     }
}

