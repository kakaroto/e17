/*
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Ecore.h>

#include "TTest1.azy_client.h"

#define NUM_CLIENTS 5

static Eina_Bool
_check_err(Azy_Content *err)
{
   if (!err)
     return EINA_TRUE;

   if (!azy_content_error_is_set(err))
     return EINA_FALSE;

   printf("** ERROR **: %s\n", azy_content_error_message_get(err));
   azy_content_error_reset(err);
   return EINA_TRUE;
}

static void
_TTest1_getAll_ret(Azy_Client *client, int type __UNUSED__, Azy_Content *content)
{
   static int x;
   TAllTypes *ret;

   ++x;

   if (azy_content_error_is_set(content))
     {
        printf("Error encountered: %s\n", azy_content_error_message_get(content));
        ecore_main_loop_quit();
        return;
     }

   ret = azy_content_return_get(content);
   printf("#%i: Success? %s!\n", ++x, ret ? "YES" : "NO");
}

static Eina_Bool
_connected(Azy_Client *cli __UNUSED__, int type __UNUSED__, Azy_Client *ev)
{
   Azy_Content *err;

printf("_connected\n");
   err = azy_content_new(NULL);

     {
        Azy_Client_Call_Id ret;
        if (!azy_client_connected_get(ev))
          goto error;
        ret = TTest1_getAll(ev, err);
        if (_check_err(err) || (!ret))
          goto error;
     }
   azy_content_free(err);
   return ECORE_CALLBACK_CANCEL;

error:
   azy_client_free(ev);
   azy_content_free(err);
   return ECORE_CALLBACK_CANCEL;
}

static void
_spawn(void *data __UNUSED__)
{
   int i;

   for (i = 0; i < NUM_CLIENTS; i++)
     {
        Azy_Client *cli;

        cli = azy_client_new();

        if (!cli)
          return;

        if (!azy_client_host_set(cli, "localhost", 4444))
          return;

        if (!azy_client_connect(cli, EINA_TRUE))
          return;

        azy_net_uri_set(azy_client_net_get(cli), "/");
     }
}

int
main(void)
{
   eina_init();
   ecore_init();
   azy_init();
   eina_log_domain_level_set("azy", EINA_LOG_LEVEL_DBG);


   ecore_job_add(_spawn, NULL);
   ecore_event_handler_add(AZY_CLIENT_CONNECTED, (Ecore_Event_Handler_Cb)_connected, NULL);
   ecore_event_handler_add(AZY_CLIENT_RETURN, (Ecore_Event_Handler_Cb)_TTest1_getAll_ret, NULL);
   ecore_main_loop_begin();

   azy_shutdown();
   ecore_shutdown();
   eina_shutdown();
   return 0;
}

