/*
 * Copyright 2010, 2011 Mike Blumenkrantz <mike@zentific.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "azy_private.h"
#include "cJSON.h"
#include <math.h>

cJSON *
azy_value_serialize_json(Azy_Value *val)
{
   Eina_List *l;
   Azy_Value *v;
   cJSON *object;

   EINA_SAFETY_ON_NULL_RETURN_VAL(val, NULL);

   switch (azy_value_type_get(val))
     {
      case AZY_VALUE_ARRAY:
      {
         object = cJSON_CreateArray();
         EINA_LIST_FOREACH(azy_value_children_items_get(val), l, v)
           cJSON_AddItemToArray(object, azy_value_serialize_json(v));
         return object;
      }

      case AZY_VALUE_STRUCT:
      {
         object = cJSON_CreateObject();
         EINA_LIST_FOREACH(azy_value_children_items_get(val), l, v)
           cJSON_AddItemToObject(object, azy_value_struct_member_name_get(v),
                                 azy_value_serialize_json(azy_value_struct_member_value_get(v)));
         return object;
      }

      case AZY_VALUE_MEMBER:
        /* FIXME: I think this is right? */
        return azy_value_serialize_json(azy_value_struct_member_value_get(val));

      case AZY_VALUE_INT:
      {
         int int_val = -1;
         azy_value_int_get(val, &int_val);
         return cJSON_CreateNumber(int_val);
      }

      case AZY_VALUE_STRING:
      case AZY_VALUE_TIME:
      {
         const char *str_val;
         azy_value_string_get(val, &str_val);
         object = cJSON_CreateString(str_val);
         eina_stringshare_del(str_val);
         return object;
      }

      case AZY_VALUE_BOOL:
      {
         Eina_Bool bool_val = -1;
         azy_value_bool_get(val, &bool_val);

         if (bool_val)
           return cJSON_CreateTrue();

         return cJSON_CreateFalse();
      }

      case AZY_VALUE_DOUBLE:
      {
         double dbl_val = -1;
         azy_value_double_get(val, &dbl_val);
         return cJSON_CreateNumber(dbl_val);
      }

      case AZY_VALUE_BASE64:
      {
         object = cJSON_CreateString(val->str_val);
         return object;
      }
     }

   object = cJSON_CreateNull();
   return object;
}

Azy_Value *
azy_value_deserialize_json(cJSON *object)
{
   if (!object)
     return NULL;

   switch (object->type)
     {
      case cJSON_Object:
      {
         Azy_Value *str;
         const char *name;

         str = azy_value_struct_new();

         EINA_SAFETY_ON_NULL_RETURN_VAL(str, NULL);
         cJSON *child = object->child;

         while (child)
           {
              name = child->string;
              azy_value_struct_member_set(str, name, azy_value_deserialize_json(child));
              child = child->next;
           }
         return str;
      }

      case cJSON_Array:
      {
         uint32_t i;
         Azy_Value *arr = azy_value_array_new();
         uint32_t arr_len = cJSON_GetArraySize(object);

         for (i = 0; i < arr_len; i++)
           azy_value_array_push(arr, azy_value_deserialize_json(cJSON_GetArrayItem(object, i)));

         return arr;
      }

      case cJSON_False:
      case cJSON_True:
        return azy_value_bool_new(object->valueint);

      case cJSON_Number:
      {
         double d = object->valuedouble;

         //this is some crazy from cJSON.c for int detection
         if ((fabs(((double)object->valueint) - d) <= __DBL_EPSILON__) && (d <= __INT_MAX__) && (d >= -__INT_MAX__ - 1))
           return azy_value_int_new(object->valueint);

         return azy_value_double_new(object->valuedouble);
      }

      case cJSON_String:
        return azy_value_string_new(object->valuestring);
     }

   return NULL;
}

Eina_Bool
azy_content_serialize_request_json(Azy_Content *content)
{
   Eina_List *l;
   Azy_Value *v;
   cJSON *object, *params;
   char *msg;

   if ((!content) || (content->buffer))
     return EINA_FALSE;

   object = cJSON_CreateObject();
   cJSON_AddStringToObject(object, "method", content->method);

   if (content->params)
     {
        params = cJSON_CreateArray();
        EINA_LIST_FOREACH(content->params, l, v)
          cJSON_AddItemToArray(params, azy_value_serialize_json(v));

        cJSON_AddItemToObject(object, "params", params);
     }
   cJSON_AddNumberToObject(object, "id", content->id);

   if (eina_log_domain_level_check(azy_log_dom, EINA_LOG_LEVEL_DBG))
     msg = cJSON_Print(object);
   else
     msg = cJSON_PrintUnformatted(object);
   azy_content_buffer_set_(content, (unsigned char *)msg, strlen(msg));

   cJSON_Delete(object);
   return EINA_TRUE;
}

Eina_Bool
azy_content_serialize_response_json(Azy_Content *content)
{
   cJSON *object, *error;
   char *msg;

   if ((!content) || (content->buffer))
     return EINA_FALSE;

   if (content->error_set)
     {
        object = cJSON_CreateObject();
        cJSON_AddNullToObject(object, "result");

        error = cJSON_CreateObject();
        cJSON_AddNumberToObject(error, "code", content->faultcode);
        cJSON_AddStringToObject(error, "message", azy_content_error_message_get(content));

        cJSON_AddItemToObject(object, "error", error);
        cJSON_AddNumberToObject(object, "id", content->id);
     }
   else if (content->retval)
     {
        object = cJSON_CreateObject();
        cJSON_AddItemToObject(object, "result", azy_value_serialize_json(content->retval));
        cJSON_AddNullToObject(object, "error");
        cJSON_AddNumberToObject(object, "id", content->id);
     }
   else
     {
        DBG("No return found in %s!", __PRETTY_FUNCTION__);
        return EINA_FALSE;
     }

   if (eina_log_domain_level_check(azy_log_dom, EINA_LOG_LEVEL_DBG))
     msg = cJSON_Print(object);
   else
     msg = cJSON_PrintUnformatted(object);
   azy_content_buffer_set_(content, (unsigned char *)msg, strlen(msg));

   cJSON_Delete(object);
   return EINA_TRUE;
}

Eina_Bool
azy_content_deserialize_json(Azy_Content *content,
                             const char  *buf,
                             ssize_t len  __UNUSED__)
{
   cJSON *object;

   EINA_SAFETY_ON_NULL_RETURN_VAL(buf, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(content, EINA_FALSE);

   if (!(object = cJSON_Parse(buf)))
     {
        ERR("%s", eina_error_msg_get(AZY_ERROR_REQUEST_JSON_OBJECT));
        return EINA_FALSE;
     }

   content->retval = azy_value_deserialize_json(object);
   cJSON_Delete(object);
   return EINA_TRUE;
}

Eina_Bool
azy_content_deserialize_request_json(Azy_Content *content,
                                     const char  *buf,
                                     ssize_t len  __UNUSED__)
{
   cJSON *object, *grab;
   int i;

   if ((!content) || (!buf))
     return EINA_FALSE;

   if (!(object = cJSON_Parse(buf)))
     {
        azy_content_error_code_set(content, AZY_ERROR_REQUEST_JSON_OBJECT);
        return EINA_FALSE;
     }

   if ((grab = cJSON_GetObjectItem(object, "id")))
     content->id = grab->valueint;

   if (!(grab = cJSON_GetObjectItem(object, "method")))
     {
        azy_content_error_code_set(content, AZY_ERROR_REQUEST_JSON_METHOD);
        cJSON_Delete(object);
        return EINA_FALSE;
     }

   content->method = eina_stringshare_add(grab->valuestring);

   grab = cJSON_GetObjectItem(object, "params");

   for (i = 0; grab && (i < cJSON_GetArraySize(grab)); i++)
     {
        Azy_Value *v;

        if (!(v = azy_value_deserialize_json(cJSON_GetArrayItem(grab, i))))
          {
             azy_content_error_faultmsg_set(content, -1, "Can't parse JSON-RPC request. Failed to deserialize parameter %d.", i);
             cJSON_Delete(object);
             return EINA_FALSE;
          }

        content->params = eina_list_append(content->params, v);
     }

   cJSON_Delete(object);
   return EINA_TRUE;
}

Eina_Bool
azy_content_deserialize_response_json(Azy_Content *content,
                                      const char  *buf,
                                      ssize_t len  __UNUSED__)
{
   cJSON *object, *grab, *error;
   Azy_Value *ret;

   if ((!content) || (!buf))
     return EINA_FALSE;

   if (!(object = cJSON_Parse(buf)))
     {
        azy_content_error_code_set(content, AZY_ERROR_RESPONSE_JSON_OBJECT);
        return EINA_FALSE;
     }

   if ((grab = cJSON_GetObjectItem(object, "id")))
     content->id = grab->valueint;

   if ((grab = cJSON_GetObjectItem(object, "error")) && (error = cJSON_GetObjectItem(grab, "code")))
     {
        int code;
        const char *msg = NULL;
        cJSON *obj;

        code = error->valueint;
        obj = cJSON_GetObjectItem(grab, "message");
        if (obj)
          msg = obj->valuestring;

        if (msg)
          {
#if defined(HAVE_EINA_ERROR_FIND)
             Eina_Error e;

             e = eina_error_find(msg);
             if (e)
               azy_content_error_faultcode_set(content, e, code);
             else
#endif
               azy_content_error_faultmsg_set(content, code, "%s", msg);
          }
        else
          azy_content_error_code_set(content, AZY_ERROR_RESPONSE_JSON_ERROR);

        cJSON_Delete(object);
        return EINA_TRUE;
     }

   grab = cJSON_GetObjectItem(object, "result");

   if ((!grab) || (grab->type == cJSON_NULL))
     {
        azy_content_error_code_set(content, AZY_ERROR_RESPONSE_JSON_NULL);
        cJSON_Delete(object);
        return EINA_FALSE;
     }

   if (!(ret = azy_value_deserialize_json(grab)))
     {
        azy_content_error_code_set(content, AZY_ERROR_RESPONSE_JSON_INVALID);
        cJSON_Delete(object);
        return EINA_FALSE;
     }

   azy_content_retval_set(content, ret);
   cJSON_Delete(object);
   return EINA_TRUE;
}

