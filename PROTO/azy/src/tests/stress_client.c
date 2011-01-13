/*
 * Copyright 2010, 2011 Mike Blumenkrantz <mike@zentific.com>
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Ecore.h>

#include "T_Test1.azy_client.h"

#ifdef HAVE_MYSQL
# include "T_SQL.azy_client.h"
#endif

#define NUM_CLIENTS 100

static Eina_List *clients;

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


#ifdef HAVE_MYSQL
static Eina_Error
ret_(Azy_Client *client __UNUSED__, int type __UNUSED__, Azy_Content *content)
{

   static int x;

   x++;
   if (azy_content_error_is_set(content))
     {
        printf("%u: Error encountered: %s\n", azy_content_id_get(content), azy_content_error_message_get(content));
        return azy_content_error_code_get(content);
     }
   //printf("#%i: Success? %s!\n", x, azy_content_return_get(content) ? "YES" : "NO");

   if (x == (NUM_CLIENTS * NUM_CLIENTS))
     ecore_main_loop_quit();
   return AZY_ERROR_NONE;
}
#else

static Eina_Error
ret_(Azy_Client *client __UNUSED__, int type __UNUSED__, Azy_Content *content)
{
   static int x;
   T_AllTypes *ret;

   x++;
   if (azy_content_error_is_set(content))
     {
        printf("%i: Error encountered: %s\n", x, azy_content_error_message_get(content));
        return azy_content_error_code_get(content);
     }

   ret = azy_content_return_get(content);
 //  printf("#%i: Success? %s!\n", x, ret ? "YES" : "NO");

   if (x == (NUM_CLIENTS * NUM_CLIENTS))
     ecore_main_loop_quit();
   return AZY_ERROR_NONE;
}
#endif

static Eina_Bool
disconnected(void *data __UNUSED__, int type __UNUSED__, void *data2 __UNUSED__)
{
   printf("%s:%s:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
   ecore_main_loop_quit();
   exit(0);
   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
connected(Azy_Client *cli __UNUSED__, int type __UNUSED__, Azy_Client *ev)
{
   Azy_Content *err;
   int i;

   err = azy_content_new(NULL);

   for (i = 0; i < NUM_CLIENTS; i++)
     {
        Azy_Client_Call_Id ret;
        if (!azy_client_connected_get(ev))
          goto error;
#ifdef HAVE_MYSQL
        ret = T_SQL_test(ev, err, NULL);
        if (check_err(err) || (!ret))
          goto error;
#else
        ret = T_Test1_getAll(ev, err, NULL);
        if (check_err(err) || (!ret))
          goto error;
#endif
     }
   azy_content_free(err);
   return ECORE_CALLBACK_CANCEL;

error:
   azy_client_free(ev);
   azy_content_free(err);
   return ECORE_CALLBACK_CANCEL;
}

static void
spawn(void *data __UNUSED__)
{
   int i;

   for (i = 0; i < NUM_CLIENTS; i++)
     {
        Azy_Client *cli;

        cli = azy_client_new();

        if (!cli)
          return;

        if (!azy_client_host_set(cli, "127.0.0.1", 4444))
          return;

        if (!azy_client_connect(cli, EINA_TRUE))
          return;

        azy_net_uri_set(azy_client_net_get(cli), "/");
        azy_net_transport_set(azy_client_net_get(cli), AZY_NET_TRANSPORT_XML);
        clients = eina_list_append(clients, cli);
     }
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


   ecore_job_add(spawn, NULL);
   ecore_event_handler_add(AZY_CLIENT_CONNECTED, (Ecore_Event_Handler_Cb)connected, NULL);
   ecore_event_handler_add(AZY_CLIENT_RETURN, (Ecore_Event_Handler_Cb)ret_, NULL);
   ecore_event_handler_add(AZY_CLIENT_DISCONNECTED, (Ecore_Event_Handler_Cb)disconnected, NULL);
   ecore_main_loop_begin();

   EINA_LIST_FREE(clients, cli)
     azy_client_free(cli);

   azy_shutdown();
   ecore_shutdown();
   eina_shutdown();
   return 0;
}

