/*
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
 */

/*
 * Copyright 2006-2008 Ondrej Jirman <ondrej.jirman@zonio.net>
 *
 * This file is part of libxr.
 *
 * Libxr is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option) any
 * later version.
 *
 * Libxr is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libxr.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <signal.h>
#include <Ecore.h>

#include "TTest1.azy_client.h"

static int
_check_err(azy_content *err)
{
   if (!err)
     return 1;

   if (!azy_content_error_is_set(err))
     return 0;

   printf("** ERROR **: %s\n", azy_content_error_message_get(err));
   azy_content_error_reset(err);
   return 1;
}

static void *
_thread_func(void *data)
{
   azy_content *err = azy_content_new(NULL);
   char *uri = data;
   eina_log_level_set(EINA_LOG_LEVEL_DBG);

   azy_client *client = azy_client_new();

   if (!client)
     return NULL;

   azy_client_uri_set(client, uri);

   if (!azy_client_connect(client, EINA_FALSE))
     {
        azy_client_free(client);
        return NULL;
     }

   int stop = 0;
   int i;

   for (i = 0; i < 100 && !stop; i++)
     {
        /* call some servlet methods */
         TAllTypes *t = TTest1_getAll(client, err);

         if (_check_err(err))
           stop = 1;

         TAllTypes_free(t);
     }

   azy_client_close(client);
   azy_client_free(client);
   return NULL;
}

int
main(int   ac,
     char *av[])
{
   Ecore_Thread *t[1024];
   ecore_init();
   eina_threads_init();
   int count = 100, i;
   char *uri = ac == 2 ? av[1] : "https://localhost:4444/TTest1";

   for (i = 0; i < count; i++)
     t[i] = ecore_thread_run((Ecore_Thread_Blocking_Cb)_thread_func, NULL, NULL, uri);

   return 0;
}

