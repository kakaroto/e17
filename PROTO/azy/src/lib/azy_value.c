/*
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>

#include "Azy.h"
#include "azy_private.h"

static Azy_Value *
_azy_value_new()
{
   Azy_Value *v = calloc(sizeof(Azy_Value), 1);
   return azy_value_ref(v);
}

Azy_Blob *
azy_blob_new(const char *buf,
              int         len)
{
   if (!buf)
     return NULL;

   Azy_Blob *b = calloc(sizeof(Azy_Blob), 1);
   b->buf = eina_stringshare_add(buf);
   b->len = len < 0 ? eina_stringshare_strlen(buf) : len;
   b->refs = 1;
   return b;
}

Azy_Blob *
azy_blob_ref(Azy_Blob *b)
{
   if (!b)
     return NULL;

   b->refs++;
   return b;
}

void
azy_blob_unref(Azy_Blob *b)
{
   if (!b)
     return;

   if (--b->refs == 0)
     {
        eina_stringshare_del(b->buf);
        free(b);
     }
}

Azy_Value *
azy_value_ref(Azy_Value *val)
{
   if (!val)
     return NULL;

   val->ref++;

   return val;
}

void
azy_value_unref(Azy_Value *val)
{
   Azy_Value *v;
   if (!val)
     return;

   if (--val->ref != 0)
     return;

   if (val->type == AZY_VALUE_BLOB)
     azy_blob_unref(val->blob_val);

   eina_stringshare_del(val->str_val);
   eina_stringshare_del(val->member_name);
   azy_value_unref(val->member_value);
   EINA_LIST_FREE(val->children, v)
     azy_value_unref(v);
   free(val);
}

/* base types */
Azy_Value *
azy_value_string_new(const char *val)
{
   Azy_Value *v = _azy_value_new();
   v->type = AZY_VALUE_STRING;
   v->str_val = eina_stringshare_add(val ? val : "");
   return v;
}

Azy_Value *
azy_value_int_new(int val)
{
   Azy_Value *v = _azy_value_new();
   v->type = AZY_VALUE_INT;
   v->int_val = val;
   return v;
}

Azy_Value *
azy_value_bool_new(Eina_Bool val)
{
   Azy_Value *v = _azy_value_new();
   v->type = AZY_VALUE_BOOLEAN;
   v->int_val = val;
   return v;
}

Azy_Value *
azy_value_double_new(double val)
{
   Azy_Value *v = _azy_value_new();
   v->type = AZY_VALUE_DOUBLE;
   v->dbl_val = val;
   return v;
}

Azy_Value *
azy_value_time_new(const char *val)
{
   Azy_Value *v = _azy_value_new();
   v->type = AZY_VALUE_TIME;
   v->str_val = eina_stringshare_add(val ? val : "");
   return v;
}

Azy_Value *
azy_value_blob_new(Azy_Blob *val)
{
   if (!val)
     return NULL;

   Azy_Value *v = _azy_value_new();
   v->type = AZY_VALUE_BLOB;
   v->blob_val = azy_blob_ref(val);
   return v;
}

Eina_Bool
azy_value_to_int(Azy_Value *val,
                  int        *nval)
{
   if ((!nval) || (!val) || (val->type != AZY_VALUE_INT))
     return EINA_FALSE;

   *nval = val->int_val;
   return EINA_TRUE;
}

Eina_Bool
azy_value_to_string(Azy_Value  *val,
                     const char **nval)
{
   if ((!nval) || (!val) || (val->type != AZY_VALUE_STRING))
     return EINA_FALSE;

   *nval = eina_stringshare_add(val->str_val);
   return EINA_TRUE;
}

Eina_Bool
azy_value_to_bool(Azy_Value *val,
                   int        *nval)
{
   if ((!nval) || (!val) || (val->type != AZY_VALUE_BOOLEAN))
     return EINA_FALSE;

   *nval = val->int_val;
   return EINA_TRUE;
}

Eina_Bool
azy_value_to_double(Azy_Value *val,
                     double     *nval)
{
   if ((!nval) || (!val) || (val->type != AZY_VALUE_DOUBLE))
     return EINA_FALSE;

   *nval = val->dbl_val;
   return EINA_TRUE;
}

Eina_Bool
azy_value_to_time(Azy_Value  *val,
                   const char **nval)
{
   if ((!nval) || (!val) || ((val->type != AZY_VALUE_TIME) && (val->type != AZY_VALUE_STRING)))
     return EINA_FALSE;

   *nval = eina_stringshare_add(val->str_val);
   return EINA_TRUE;
}

Eina_Bool
azy_value_to_blob(Azy_Value *val,
                   Azy_Blob **nval)
{
   if ((!nval) || (!val) || ((val->type != AZY_VALUE_BLOB) && (val->type != AZY_VALUE_STRING)))
     return EINA_FALSE;

   if (val->blob_val)
     *nval = azy_blob_ref(val->blob_val);
   else
     *nval = azy_blob_new(val->str_val, strlen(val->str_val));
   return EINA_TRUE;
}

Eina_Bool
azy_value_to_value(Azy_Value  *val,
                    Azy_Value **nval)
{
   if ((!nval) || (!val))
     return EINA_FALSE;

   *nval = azy_value_ref(val);
   return EINA_TRUE;
}

Azy_Value_Type
azy_value_type_get(Azy_Value *val)
{
   if (!val)
     return -1;

   return val->type;
}

Eina_List *
azy_value_struct_members_get(Azy_Value *val)
{
   if ((!val) || (val->type != AZY_VALUE_STRUCT))
     return NULL;

   return val->children;
}

const char *
azy_value_struct_member_name_get(Azy_Value *val)
{
   if ((!val) || (val->type != AZY_VALUE_MEMBER))
     return NULL;

   return val->member_name;
}

Azy_Value *
azy_value_struct_member_value_get(Azy_Value *val)
{
   if ((!val) || (val->type != AZY_VALUE_MEMBER))
     return NULL;

   return val->member_value;
}

Azy_Value *
azy_value_struct_member_get(Azy_Value *val,
                             const char *name)
{
   Eina_List *l;
   Azy_Value *m;

   if ((!val) || (val->type != AZY_VALUE_STRUCT))
     return NULL;

   EINA_LIST_FOREACH(val->children, l, m)

     if (!strcmp(azy_value_struct_member_name_get(m), name))
       return azy_value_struct_member_value_get(m);

   return NULL;
}

Eina_List *
azy_value_items_get(Azy_Value *val)
{
   if ((!val) || (val->type != AZY_VALUE_ARRAY))
     return NULL;

   return val->children;
}

/* composite types */

Azy_Value *
azy_value_struct_new(void)
{
   Azy_Value *v = _azy_value_new();
   v->type = AZY_VALUE_STRUCT;
   return v;
}

Azy_Value *
azy_value_struct_new_from_string(const char *name, const char *value)
{
   Azy_Value *v, *str;

   v = _azy_value_new();
   v->type = AZY_VALUE_STRUCT;
   str = azy_value_string_new(value);
   azy_value_struct_member_set(v, name, str);
   return v;
}

Azy_Value *
azy_value_struct_new_from_int(const char *name, int value)
{
   Azy_Value *v, *i;

   v = _azy_value_new();
   v->type = AZY_VALUE_STRUCT;
   i = azy_value_int_new(value);
   azy_value_struct_member_set(v, name, i);
   return v;
}

Azy_Value *
azy_value_struct_new_from_double(const char *name, double value)
{
   Azy_Value *v, *d;

   v = _azy_value_new();
   v->type = AZY_VALUE_STRUCT;
   d = azy_value_double_new(value);
   azy_value_struct_member_set(v, name, d);
   return v;
}

Azy_Value *
azy_value_array_new(void)
{
   Azy_Value *v = _azy_value_new();
   v->type = AZY_VALUE_ARRAY;
   return v;
}

void
azy_value_struct_member_set(Azy_Value *str,
                             const char *name,
                             Azy_Value *val)
{
   Eina_List *l;
   Azy_Value *m;

   if ((!str) || (str->type != AZY_VALUE_STRUCT) || (!val))
     return;

   EINA_LIST_FOREACH(str->children, l, m)
     if (!strcmp(m->member_name, name))
       {
          azy_value_unref(m->member_value);
          m->member_value = val;
          return;
       }

   Azy_Value *v = _azy_value_new();
   v->type = AZY_VALUE_MEMBER;
   v->member_name = eina_stringshare_add(name);
   v->member_value = val;
   str->children = eina_list_append(str->children, v);
}

void
azy_value_array_append(Azy_Value *arr,
                        Azy_Value *val)
{
   if ((!val) || (!arr))
     return;

   arr->children = eina_list_append(arr->children, val);
}

Eina_Bool
azy_value_retval_is_error(Azy_Value  *v,
                           int         *errcode,
                           const char **errmsg)
{
   if ((!v) || (v->type != AZY_VALUE_STRUCT) || (!errcode) || (!errmsg))
     return EINA_FALSE;

   if (!azy_value_to_int(azy_value_struct_member_get(v, "faultCode"), errcode) ||
       !azy_value_to_string(azy_value_struct_member_get(v, "faultString"), errmsg))
     return EINA_FALSE;

   return EINA_TRUE;
}

Eina_Bool
__azy_value_is_complicated(Azy_Value *v,
                            int         max_strlen)
{
   return (azy_value_type_get(v) == AZY_VALUE_STRUCT && azy_value_struct_members_get(v))
          || (azy_value_type_get(v) == AZY_VALUE_ARRAY && azy_value_items_get(v))
          || (azy_value_type_get(v) == AZY_VALUE_STRING && v->str_val && eina_stringshare_strlen(v->str_val) > max_strlen);
}

static Eina_Bool
__azy_value_list_is_complicated(Azy_Value *v)
{
   Eina_List *l;
   Azy_Value *val;

   if (!v)
     return EINA_FALSE;

   if (v->type == AZY_VALUE_ARRAY)
     {
        if (eina_list_count(azy_value_items_get(v)) > 8)
          return EINA_TRUE;
        else
          EINA_LIST_FOREACH(azy_value_items_get(v), l, val)
            if (__azy_value_is_complicated(val, 35))
              return EINA_TRUE;
     }
   else if (v->type == AZY_VALUE_STRUCT)
     {
        if (eina_list_count(azy_value_struct_members_get(v)) > 5)
          return EINA_TRUE;
        else
          EINA_LIST_FOREACH(azy_value_items_get(v), l, val)
            if (__azy_value_is_complicated(azy_value_struct_member_value_get(val), 25))
              return EINA_TRUE;
     }

   return EINA_FALSE;
}

void
azy_value_dump(Azy_Value  *v,
                Eina_Strbuf *string,
                unsigned int indent)
{
   Eina_List *l;
   Azy_Value *val;
   char buf[256] = {0};

   if (!v)
     return;

   memset(buf, ' ', MIN(indent * 2, sizeof(buf) - 1));

   switch (azy_value_type_get(v))
     {
      case AZY_VALUE_ARRAY:
      {
         if (!azy_value_items_get(v))
           eina_strbuf_append(string, "[]");
         else if (!__azy_value_list_is_complicated(v))
           {
              eina_strbuf_append(string, "[ ");
              EINA_LIST_FOREACH(azy_value_items_get(v), l, val)
                {
                   azy_value_dump(val, string, indent + 1);
                   eina_strbuf_append_printf(string, "%s ", l->next ? "," : "");
                }
              eina_strbuf_append_char(string, ']');
           }
         else
           {
              eina_strbuf_append_char(string, '[');
              EINA_LIST_FOREACH(azy_value_items_get(v), l, val)
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
         if (!azy_value_struct_members_get(v))
           eina_strbuf_append(string, "{}");
         else if (!__azy_value_list_is_complicated(v))
           {
              eina_strbuf_append(string, "{ ");
              EINA_LIST_FOREACH(azy_value_items_get(v), l, val)
                {
                   azy_value_dump(val, string, indent);
                   eina_strbuf_append_printf(string, "%s ", l->next ? "," : "");
                }
              eina_strbuf_append_char(string, '}');
           }
         else
           {
              eina_strbuf_append_char(string, '{');
              EINA_LIST_FOREACH(azy_value_items_get(v), l, val)
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

      case AZY_VALUE_TIME:
      {
         eina_strbuf_append_printf(string, "%s", v->str_val);
         break;
      }

      case AZY_VALUE_BLOB:
      {
         eina_strbuf_append_printf(string, "<BLOB>");
         break;
      }
     }
}

