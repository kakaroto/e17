/*
 * Copyright 2010, 2011 Mike Blumenkrantz <mike@zentific.com>
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <string.h>

#include "T_Test1.azy_client.h"
#include "T_Test2.azy_client.h"

/* this is a demo macro to show use of azy_client_call_checker */
#define CALL_CHECK(X) \
   do \
     { \
        if (!azy_client_call_checker(cli, err, ret, X, __PRETTY_FUNCTION__)) \
          { \
             printf("%s\n", azy_content_error_message_get(err)); \
             exit(1); \
          } \
     } while (0)
     
static Eina_Bool
_disconnected(void *data __UNUSED__, int type __UNUSED__, void *data2 __UNUSED__)
{
   printf("%s:%s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
   ecore_main_loop_quit();
   return ECORE_CALLBACK_RENEW;
}

static Eina_Error
_T_Test1_getBigArray_ret(Azy_Client *client, Azy_Content *content, void *retval)
{
   Eina_List *ret;

   if (azy_content_error_is_set(content))
     {
        printf("Error encountered: %s\n", azy_content_error_message_get(content));
        azy_client_close(client);
        ecore_main_loop_quit();
        return azy_content_error_code_get(content);
     }
   ret = retval;

   if (ret)
     printf("%i list entries\n", eina_list_count(ret));
   printf("%s: Success? %s!\n", __PRETTY_FUNCTION__, ret ? "YES" : "NO");
   return AZY_ERROR_NONE;
}

static Eina_Error
_T_Test1_putBigArray_ret(Azy_Client *client __UNUSED__, Azy_Content *content, void *retval)
{
   Eina_Bool ret;
   const char *data;
 
   if (azy_content_error_is_set(content))
     {
        printf("Error encountered: %s\n", azy_content_error_message_get(content));
        azy_client_close(client);
        ecore_main_loop_quit();
        return azy_content_error_code_get(content);
     }

   ret = (intptr_t)retval;
   data = azy_content_data_get(content);
   printf("data passed to function: %s\n", data);
   eina_stringshare_del(data);
   printf("%s: Success? %s!\n", __PRETTY_FUNCTION__, ret ? "YES" : "NO");
   return AZY_ERROR_NONE;
}

static Eina_Error
_T_Test1_getAll_ret(Azy_Client *client __UNUSED__, Azy_Content *content, void *retval)
{
   T_AllTypes *ret;

   if (azy_content_error_is_set(content))
     {
        printf("Error encountered: %s\n", azy_content_error_message_get(content));
        azy_client_close(client);
        ecore_main_loop_quit();
        return azy_content_error_code_get(content);
     }

   ret = retval;
   printf("%s: Success? %s!\n", __PRETTY_FUNCTION__, ret ? "YES" : "NO");
   return AZY_ERROR_NONE;
}

static Eina_Error
_T_Test1_getAllArrays_ret(Azy_Client *client __UNUSED__, Azy_Content *content, void *retval)
{
   T_AllArrays *ret;

   if (azy_content_error_is_set(content))
     {
        printf("Error encountered: %s\n", azy_content_error_message_get(content));
        azy_client_close(client);
        ecore_main_loop_quit();
        return azy_content_error_code_get(content);
     }

   ret = retval;
   printf("%s: Success? %s!\n", __PRETTY_FUNCTION__, ret ? "YES" : "NO");
   return AZY_ERROR_NONE;
}

static Eina_Error
_T_Test2_auth_ret(Azy_Client *client __UNUSED__, Azy_Content *content, void *retval)
{
   Eina_Bool ret;
 
   if (azy_content_error_is_set(content))
     {
        printf("Error encountered: %s\n", azy_content_error_message_get(content));
        azy_client_close(client);
        ecore_main_loop_quit();
        return azy_content_error_code_get(content);
     }

   ret = (intptr_t)retval;
   printf("%s: Success? %s!\n", __PRETTY_FUNCTION__, ret ? "YES" : "NO");
   return AZY_ERROR_NONE;
}

static Eina_Error
_T_Test1_undefined_ret(Azy_Client *client __UNUSED__, Azy_Content *content, void *retval __UNUSED__)
{
   if (azy_content_error_is_set(content))
     printf("Error encountered: %s\n", azy_content_error_message_get(content));
   azy_client_close(client);
   ecore_main_loop_quit();
   return AZY_ERROR_NONE;
}

static Eina_Bool
connected(void *data __UNUSED__, int type __UNUSED__, Azy_Client *cli)
{
   unsigned int ret;
   const char *s;
   Azy_Content *err;
   Azy_Net *net;

   Eina_List *list = NULL;
   int i;
   
   net = azy_client_net_get(cli);
   err = azy_content_new(NULL);
   
   /* use json transport for big arrays */
   azy_net_transport_set(net, AZY_NET_TRANSPORT_JSON);
   azy_net_uri_set(net, "/RPC2");

   ret = T_Test1_getBigArray(cli, err, NULL);
   CALL_CHECK(_T_Test1_getBigArray_ret);

   for (i = 0; i < 5000; i++)
     list = eina_list_append(list, eina_stringshare_printf("user.bob%d@zonio.net", i));

   ret = T_Test1_putBigArray(cli, list, err, eina_stringshare_add("stored data"));
   CALL_CHECK(_T_Test1_putBigArray_ret);
   
   EINA_LIST_FREE(list, s)
     eina_stringshare_del(s);

   azy_net_transport_set(net, AZY_NET_TRANSPORT_XML);

   ret = T_Test1_getAll(cli, err, NULL);
   CALL_CHECK(_T_Test1_getAll_ret);

   ret = T_Test1_getAllArrays(cli, err, NULL);
   CALL_CHECK(_T_Test1_getAllArrays_ret);

   ret = T_Test2_auth(cli, "name", "pass", err, NULL);
   CALL_CHECK(_T_Test2_auth_ret);

   ret = T_Test1_getAll(cli, err, NULL);
   CALL_CHECK(_T_Test1_getAll_ret);

   /* call undefined servlet methods */
   {
      Azy_Content *content;
      Azy_Value *struc;

      content = azy_content_new("T_Test1.undefined");
      struc = azy_value_struct_new();
      if (!struc)
        {
           azy_client_close(cli);
           return ECORE_CALLBACK_CANCEL;
        }
      azy_value_struct_member_set(struc, "test", azy_value_int_new(100));
      azy_content_param_add(content, struc);
      ret = azy_client_call(cli, content, AZY_NET_TRANSPORT_JSON, (Azy_Content_Cb)azy_value_to_T_Struct);
           if (!azy_client_call_checker(cli, err, ret, _T_Test1_undefined_ret, __PRETTY_FUNCTION__)) 
          { 
             printf("%s\n", azy_content_error_message_get(err)); 
             exit(1); 
          } 
   //   CALL_CHECK(_T_Test1_undefined_ret);
      azy_content_free(content);
   }

   azy_content_free(err);
   return ECORE_CALLBACK_RENEW;
}

int
main(void)
{
   const char *uri;
   Ecore_Event_Handler *handler;
   Azy_Client *cli;

   
   eina_init();
   ecore_init();
   azy_init();
//   eina_log_domain_level_set("ecore_con", EINA_LOG_LEVEL_DBG);
   eina_log_domain_level_set("azy", EINA_LOG_LEVEL_DBG);
   uri = "https://localhost:4444/RPC2";

   /* create object for performing client connections */
   cli = azy_client_new();

   if (!azy_client_host_set(cli, "127.0.0.1", 4444))
     return 1;

   handler = ecore_event_handler_add(AZY_CLIENT_CONNECTED, (Ecore_Event_Handler_Cb)connected, cli);
   handler = ecore_event_handler_add(AZY_CLIENT_DISCONNECTED, (Ecore_Event_Handler_Cb)_disconnected, cli);

   /* connect to the servlet on the server specified by uri */
   if (!azy_client_connect(cli, EINA_FALSE))
     return 1;

   ecore_main_loop_begin();

   azy_client_free(cli);
   azy_shutdown();
   ecore_shutdown();
   eina_shutdown();

   return 0;
}

