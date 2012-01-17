#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

#include <E_Ukit.h>
#include "e_ukit_private.h"

void *
unmarshal_property(DBusMessage *msg, DBusError *err)
{
   E_Ukit_Get_Property_Return *ret = NULL;
   DBusMessageIter iter, a_iter;
   int type;
   char *tmp;

   if (!dbus_message_iter_init(msg, &iter)) return NULL; /* no params in message */

   ret = calloc(1, sizeof(E_Ukit_Get_Property_Return));
   if (!ret) 
     {
        dbus_set_error(err, DBUS_ERROR_NO_MEMORY, "");
        return NULL;
     }

   dbus_message_iter_recurse(&iter, &a_iter);
   if (dbus_message_iter_get_arg_type(&a_iter) != DBUS_TYPE_INVALID)
     {
        type = dbus_message_iter_get_arg_type(&a_iter);    

        switch(type)
          {
             case DBUS_TYPE_STRING:
             case DBUS_TYPE_OBJECT_PATH:
               ret->type = E_UKIT_PROPERTY_TYPE_STRING;
               dbus_message_iter_get_basic(&a_iter, &tmp);
               ret->val.s = eina_stringshare_add(tmp);
               break;
             case DBUS_TYPE_INT32:
               ret->type = E_UKIT_PROPERTY_TYPE_INT;
               dbus_message_iter_get_basic(&a_iter, &(ret->val.i));
               break;
             case DBUS_TYPE_UINT32:
               ret->type = E_UKIT_PROPERTY_TYPE_UINT32;
               dbus_message_iter_get_basic(&a_iter, &(ret->val.u));
               break;
             case DBUS_TYPE_UINT64:
               ret->type = E_UKIT_PROPERTY_TYPE_UINT64;
               dbus_message_iter_get_basic(&a_iter, &(ret->val.t));
               break;
             case DBUS_TYPE_INT64:
               ret->type = E_UKIT_PROPERTY_TYPE_INT64;
               dbus_message_iter_get_basic(&a_iter, &(ret->val.x));
               break;
             case DBUS_TYPE_BOOLEAN:
               ret->type = E_UKIT_PROPERTY_TYPE_BOOL;
               dbus_message_iter_get_basic(&a_iter, &(ret->val.b));
               break;
             case DBUS_TYPE_DOUBLE:
               ret->type = E_UKIT_PROPERTY_TYPE_DOUBLE;
               dbus_message_iter_get_basic(&a_iter, &(ret->val.d));
               break;
             case DBUS_TYPE_ARRAY:
               ret->type = E_UKIT_PROPERTY_TYPE_STRLIST;
               {
                  DBusMessageIter list_iter;
                  ret->val.strlist = NULL;
                  dbus_message_iter_recurse(&a_iter, &list_iter);
                  while (dbus_message_iter_get_arg_type(&list_iter) != DBUS_TYPE_INVALID)
                    {
                       char *str;
                       dbus_message_iter_get_basic(&list_iter, &str);
                       tmp = (char*)eina_stringshare_add(str);
                       ret->val.strlist = eina_list_append(ret->val.strlist, tmp);
                       dbus_message_iter_next(&list_iter);
                    }
                }
                break;
             default:
               break;
          }
     }

   return ret;
}

void 
free_property(void *data)
{
   E_Ukit_Get_Property_Return *ret = data;

   if (!ret) return;
   if (ret->type == E_UKIT_PROPERTY_TYPE_STRLIST)
     {
        const char *s;
        EINA_LIST_FREE(ret->val.strlist, s)
          eina_stringshare_del(s);
     }
   else if (ret->type == E_UKIT_PROPERTY_TYPE_STRING)
     eina_stringshare_del(ret->val.s);
   free(ret);
}

void *
unmarshal_device_get_all_properties(DBusMessage *msg, DBusError *err)
{
   E_Ukit_Get_All_Properties_Return *ret = NULL;
   DBusMessageIter iter, a_iter, s_iter, v_iter;
   int type;
   char *tmp;

   /* a{sv} = array of string+variants */
   if (!dbus_message_has_signature(msg, "a{sv}")) 
     {
        dbus_set_error(err, DBUS_ERROR_INVALID_SIGNATURE, "");
        return NULL;
     }

   ret = calloc(1, sizeof(E_Ukit_Get_All_Properties_Return));
   if (!ret) 
     {
         dbus_set_error(err, DBUS_ERROR_NO_MEMORY, "");
         return NULL;
     }

   ret->properties = eina_hash_string_small_new(EINA_FREE_CB(e_ukit_property_free));

   dbus_message_iter_init(msg, &iter);
   dbus_message_iter_recurse(&iter, &a_iter);
   while (dbus_message_iter_get_arg_type(&a_iter) != DBUS_TYPE_INVALID)
   {
      const char *name;
      E_Ukit_Property *prop = calloc(1, sizeof(E_Ukit_Property));
      dbus_message_iter_recurse(&a_iter, &s_iter);
      dbus_message_iter_get_basic(&s_iter, &name);
      dbus_message_iter_next(&s_iter);
      dbus_message_iter_recurse(&s_iter, &v_iter);
      
      type = dbus_message_iter_get_arg_type(&v_iter);
      switch(type)
        {
           case DBUS_TYPE_STRING:
           case DBUS_TYPE_OBJECT_PATH:
             prop->type = E_UKIT_PROPERTY_TYPE_STRING;
             dbus_message_iter_get_basic(&v_iter, &tmp);
             prop->val.s = eina_stringshare_add(tmp);
             break;
           case DBUS_TYPE_INT32:
             prop->type = E_UKIT_PROPERTY_TYPE_INT;
             dbus_message_iter_get_basic(&v_iter, &(prop->val.i));
             break;
           case DBUS_TYPE_UINT32:
             prop->type = E_UKIT_PROPERTY_TYPE_UINT32;
             dbus_message_iter_get_basic(&v_iter, &(prop->val.u));
             break;
           case DBUS_TYPE_UINT64:
             prop->type = E_UKIT_PROPERTY_TYPE_UINT64;
             dbus_message_iter_get_basic(&v_iter, &(prop->val.t));
             break;
           case DBUS_TYPE_INT64:
             prop->type = E_UKIT_PROPERTY_TYPE_INT64;
             dbus_message_iter_get_basic(&v_iter, &(prop->val.x));
             break;
           case DBUS_TYPE_BOOLEAN:
             prop->type = E_UKIT_PROPERTY_TYPE_BOOL;
             dbus_message_iter_get_basic(&v_iter, &(prop->val.b));
             break;
           case DBUS_TYPE_DOUBLE:
             prop->type = E_UKIT_PROPERTY_TYPE_DOUBLE;
             dbus_message_iter_get_basic(&v_iter, &(prop->val.d));
             break;
           case DBUS_TYPE_ARRAY:
             prop->type = E_UKIT_PROPERTY_TYPE_STRLIST;
             {
                DBusMessageIter list_iter;
                prop->val.strlist = NULL;
                dbus_message_iter_recurse(&v_iter, &list_iter);
                while (dbus_message_iter_get_arg_type(&list_iter) != DBUS_TYPE_INVALID)
                {
                   char *str;
                   dbus_message_iter_get_basic(&list_iter, &str);
                   tmp = (char*)eina_stringshare_add(str);
                   prop->val.strlist = eina_list_append(prop->val.strlist, tmp);
                   dbus_message_iter_next(&list_iter);
                }
             }
             break;
           default:
             WARN("EUkit Error: unexpected property type (%s): %c", name, dbus_message_iter_get_arg_type(&v_iter));
             break;
        }
      eina_hash_add(ret->properties, name, prop);

      dbus_message_iter_next(&a_iter);
   }

   return ret;
}

void
free_device_get_all_properties(void *data)
{
   E_Ukit_Get_All_Properties_Return *ret = data;

   if (!ret) return;
   eina_hash_free(ret->properties);
   free(ret);
}

void *
unmarshal_string_list(DBusMessage *msg, DBusError *err)
{
   E_Ukit_String_List_Return *ret = NULL;
   DBusMessageIter iter, sub;
   char *tmp;

   /* ao = array of object strings */
   if (!dbus_message_has_signature(msg, "ao")) 
     {
        dbus_set_error(err, DBUS_ERROR_INVALID_SIGNATURE, "");
        return NULL;
     }

   ret = calloc(1, sizeof(E_Ukit_String_List_Return));
   if (!ret) 
     {
        dbus_set_error(err, DBUS_ERROR_NO_MEMORY, "");
        return NULL;
     }

   ret->strings = NULL;

   dbus_message_iter_init(msg, &iter);
   dbus_message_iter_recurse(&iter, &sub);
   while (dbus_message_iter_get_arg_type(&sub) != DBUS_TYPE_INVALID)
     {
        char *dev = NULL;

        dbus_message_iter_get_basic(&sub, &dev);
        tmp = (char*)eina_stringshare_add(dev);
        if (dev) ret->strings = eina_list_append(ret->strings, tmp);
        dbus_message_iter_next(&sub);
     }

   return ret;
}

void
free_string_list(void *data)
{
   E_Ukit_String_List_Return *ret = data;
   const char *x;

   if (!ret) return;
   EINA_LIST_FREE(ret->strings, x)
     eina_stringshare_del(x);
   free(ret);
}
