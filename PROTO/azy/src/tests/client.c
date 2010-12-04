/*
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <string.h>

#include "T_Test1.azy_client.h"
#include "T_Test2.azy_client.h"

/* this function prints client error if any and resets error so that futher calls to client funcs work */
static Eina_Bool
check_err(Azy_Content *err)
{
   if (!err)
     return EINA_TRUE;

   if (!azy_content_error_is_set(err))
     return EINA_FALSE;

   printf("** ERROR **: %s\n", azy_content_error_message_get(err));
   azy_content_error_reset(err);
   return EINA_TRUE;
}

static Eina_Bool
_disconnected(void *data __UNUSED__, int type __UNUSED__, void *data2 __UNUSED__)
{
   printf("%s:%s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
   ecore_main_loop_quit();
   return ECORE_CALLBACK_RENEW;
}

static Eina_Error
_T_Test1_getBigArray_ret(Azy_Client *client, Azy_Content *content)
{
   Eina_List *ret;

   if (azy_content_error_is_set(content))
     {
        printf("Error encountered: %s\n", azy_content_error_message_get(content));
        azy_client_close(client);
        ecore_main_loop_quit();
        return azy_content_error_code_get(content);
     }
   ret = azy_content_return_get(content);

   if (ret)
     printf("%i list entries\n", eina_list_count(ret));
   printf("%s: Success? %s!\n", __PRETTY_FUNCTION__, ret ? "YES" : "NO");
   return AZY_ERROR_NONE;
}

static Eina_Error
_T_Test1_putBigArray_ret(Azy_Client *client __UNUSED__, Azy_Content *content)
{
   Eina_Bool ret;
 
   if (azy_content_error_is_set(content))
     {
        printf("Error encountered: %s\n", azy_content_error_message_get(content));
        azy_client_close(client);
        ecore_main_loop_quit();
        return azy_content_error_code_get(content);
     }

   ret = (intptr_t)azy_content_return_get(content);
   printf("%s: Success? %s!\n", __PRETTY_FUNCTION__, ret ? "YES" : "NO");
   return AZY_ERROR_NONE;
}

static Eina_Error
_T_Test1_getAll_ret(Azy_Client *client __UNUSED__, Azy_Content *content)
{
   T_AllTypes *ret;

   if (azy_content_error_is_set(content))
     {
        printf("Error encountered: %s\n", azy_content_error_message_get(content));
        azy_client_close(client);
        ecore_main_loop_quit();
        return azy_content_error_code_get(content);
     }

   ret = azy_content_return_get(content);
   printf("%s: Success? %s!\n", __PRETTY_FUNCTION__, ret ? "YES" : "NO");
   return AZY_ERROR_NONE;
}

static Eina_Error
_T_Test1_getAllArrays_ret(Azy_Client *client __UNUSED__, Azy_Content *content)
{
   T_AllArrays *ret;

   if (azy_content_error_is_set(content))
     {
        printf("Error encountered: %s\n", azy_content_error_message_get(content));
        azy_client_close(client);
        ecore_main_loop_quit();
        return azy_content_error_code_get(content);
     }

   ret = azy_content_return_get(content);
   printf("%s: Success? %s!\n", __PRETTY_FUNCTION__, ret ? "YES" : "NO");
   return AZY_ERROR_NONE;
}

static Eina_Error
_T_Test2_auth_ret(Azy_Client *client __UNUSED__, Azy_Content *content)
{
   Eina_Bool ret;
 
   if (azy_content_error_is_set(content))
     {
        printf("Error encountered: %s\n", azy_content_error_message_get(content));
        azy_client_close(client);
        ecore_main_loop_quit();
        return azy_content_error_code_get(content);
     }

   ret = (intptr_t)azy_content_return_get(content);
   printf("%s: Success? %s!\n", __PRETTY_FUNCTION__, ret ? "YES" : "NO");
   return AZY_ERROR_NONE;
}

static Eina_Error
_T_Test1_undefined_ret(Azy_Client *client __UNUSED__, Azy_Content *content)
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
   if (check_err(err) || (!ret))
     exit(1);

   if (!azy_client_callback_set(cli, ret, _T_Test1_getBigArray_ret))
     {
        azy_client_close(cli);
        return ECORE_CALLBACK_CANCEL;
     }

   for (i = 0; i < 5000; i++)
     list = eina_list_append(list, eina_stringshare_printf("user.bob%d@zonio.net", i));

   ret = T_Test1_putBigArray(cli, list, err, NULL);
   if (check_err(err) || (!ret))
     exit(1);
   if (!azy_client_callback_set(cli, ret, _T_Test1_putBigArray_ret))
     {
        azy_client_close(cli);
        return ECORE_CALLBACK_CANCEL;
     }
   EINA_LIST_FREE(list, s)
     eina_stringshare_del(s);

   azy_net_transport_set(net, AZY_NET_TRANSPORT_XML);

   ret = T_Test1_getAll(cli, err, NULL);
   if (check_err(err) || (!ret))
     exit(1);
   if (!azy_client_callback_set(cli, ret, _T_Test1_getAll_ret))
     {
        azy_client_close(cli);
        return ECORE_CALLBACK_CANCEL;
     }

   ret = T_Test1_getAllArrays(cli, err, NULL);
   if (check_err(err) || (!ret))
     exit(1);
   if (!azy_client_callback_set(cli, ret, _T_Test1_getAllArrays_ret))
     {
        azy_client_close(cli);
        return ECORE_CALLBACK_CANCEL;
     }

   ret = T_Test2_auth(cli, "name", "pass", err, NULL);
   if (check_err(err) || (!ret))
     exit(1);
   if (!azy_client_callback_set(cli, ret, _T_Test2_auth_ret))
     {
        azy_client_close(cli);
        return ECORE_CALLBACK_CANCEL;
     }

   ret = T_Test1_getAll(cli, err, NULL);
   if (check_err(err) || (!ret))
     exit(1);
   if (!azy_client_callback_set(cli, ret, _T_Test1_getAll_ret))
     {
        azy_client_close(cli);
        return ECORE_CALLBACK_CANCEL;
     }

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
      ret = azy_client_call(cli, content, AZY_NET_TRANSPORT_JSON, NULL);
      if (check_err(err) || (!ret))
        exit(1);
      if (!azy_client_callback_set(cli, ret, _T_Test1_undefined_ret))
        {
           azy_client_close(cli);
           return ECORE_CALLBACK_CANCEL;
        }
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
   eina_log_domain_level_set("azy", EINA_LOG_LEVEL_DBG);
   uri = "https://localhost:4444/RPC2";

   /* create object for performing client connections */
   cli = azy_client_new();

   if (!azy_client_host_set(cli, "localhost", 4444))
     return 1;

   handler = ecore_event_handler_add(AZY_CLIENT_CONNECTED, (Ecore_Event_Handler_Cb)connected, cli);
   handler = ecore_event_handler_add(AZY_CLIENT_DISCONNECTED, (Ecore_Event_Handler_Cb)_disconnected, cli);

   /* connect to the servlet on the server specified by uri */
   if (!azy_client_connect(cli, EINA_TRUE))
     return 1;

   ecore_main_loop_begin();

   azy_client_free(cli);
   azy_shutdown();
   ecore_shutdown();
   eina_shutdown();

   return 0;
}

