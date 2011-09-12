/*
 * Copyright 2010, 2011 Mike Blumenkrantz <mike@zentific.com>
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Ecore.h>

#include "identica_Common_Azy.h"

static Eina_Error
ret_(Azy_Client *cli __UNUSED__, int type __UNUSED__, Azy_Content *content)
{
   Eina_List *l, *r;
   identica_Ident *ret;

   if (azy_content_error_is_set(content))
     {
        printf("Error encountered: %s\n", azy_content_error_message_get(content));
        return azy_content_error_code_get(content);
     }

   r = azy_content_return_get(content);
 //  printf("Success? %s!\n", ret ? "YES" : "NO");

   EINA_LIST_FOREACH(r, l, ret)
     identica_Ident_print(NULL, 0, ret);
   return AZY_ERROR_NONE;
}

static void
Array_identica_Ident_free(Eina_List *array)
{
   identica_Ident *id;

   EINA_SAFETY_ON_NULL_RETURN(array);

   EINA_LIST_FREE(array, id)
     identica_Ident_free(id);
}

static Eina_Bool
azy_value_to_Array_identica_Ident(Azy_Value *_array, Eina_List **_narray)
{
  Eina_List *_tmp_narray = NULL, *_item;
  Azy_Value *v;

  EINA_SAFETY_ON_NULL_RETURN_VAL(_narray, EINA_FALSE);

  if ((!_array) || (azy_value_type_get(_array) != AZY_VALUE_ARRAY))
    return EINA_FALSE;

  EINA_LIST_FOREACH(azy_value_children_items_get(_array), _item, v)
  {
    identica_Ident *_item_value = NULL;

    if (!azy_value_to_identica_Ident(v, &_item_value))
    {
      Array_identica_Ident_free(_tmp_narray);
      return EINA_FALSE;
    }

    _tmp_narray = eina_list_append(_tmp_narray, _item_value);
  }

  *_narray = _tmp_narray;
  return EINA_TRUE;
}

static Eina_Bool
download_status(void *data __UNUSED__, int type __UNUSED__, Azy_Event_Download_Status *ev)
{
   int total = -1;

   if (ev->net)
     total = azy_net_message_length_get(ev->net);
   if (total > 0)
     printf("%zu bytes (%i total) transferred for id %u\n", ev->size, total, ev->id);
   else
     printf("%zu bytes transferred for id %u\n", ev->size, ev->id);
   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
disconnected(void *data __UNUSED__, int type __UNUSED__, void *data2 __UNUSED__)
{
   printf("%s:%s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
   ecore_main_loop_quit();
   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
connected(void *data __UNUSED__, int type __UNUSED__, Azy_Client *ev)
{
   Azy_Client_Call_Id id;

   id = azy_client_blank(ev, AZY_NET_TYPE_GET, NULL, (Azy_Content_Cb)azy_value_to_Array_identica_Ident, NULL);
   if (!id) ecore_main_loop_quit();
   azy_client_callback_free_set(ev, id, (Ecore_Cb)Array_identica_Ident_free);

   return ECORE_CALLBACK_RENEW;
}

int
main(void)
{
   Azy_Client *cli;
   eina_init();
   ecore_init();
   azy_init();
   eina_log_domain_level_set("azy", EINA_LOG_LEVEL_DBG);
   eina_log_domain_level_set("ecore_con", EINA_LOG_LEVEL_DBG);

   cli = azy_client_new();

   EINA_SAFETY_ON_NULL_RETURN_VAL(cli, 1);

   EINA_SAFETY_ON_TRUE_RETURN_VAL(!azy_client_host_set(cli, "https://identi.ca", 443), 1);

   EINA_SAFETY_ON_TRUE_RETURN_VAL(!azy_client_connect(cli, EINA_TRUE), 1);

   azy_net_uri_set(azy_client_net_get(cli), "/api/statuses/public_timeline.json");
   azy_net_version_set(azy_client_net_get(cli), 0);

   ecore_event_handler_add(AZY_CLIENT_CONNECTED, (Ecore_Event_Handler_Cb)connected, NULL);
   ecore_event_handler_add(AZY_CLIENT_RETURN, (Ecore_Event_Handler_Cb)ret_, NULL);
   ecore_event_handler_add(AZY_CLIENT_DISCONNECTED, (Ecore_Event_Handler_Cb)disconnected, NULL);
   ecore_event_handler_add(AZY_EVENT_DOWNLOAD_STATUS, (Ecore_Event_Handler_Cb)download_status, NULL);

   ecore_main_loop_begin();

   azy_client_free(cli);

   azy_shutdown();
   ecore_shutdown();
   eina_shutdown();
   return 0;
}

